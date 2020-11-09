#include "decode_jpeg_mmal.h"


#define _check_mmal(x) \
    do { \
        status = (x); \
        if (status != MMAL_SUCCESS) { \
            fprintf(stderr, "%s:%d: %s: %s (0x%08x)\n", __FILE__, __LINE__, #x, mmal_status_to_string(status), status); \
            goto error; \
        } \
    } while (0)


static void log_format(MMAL_ES_FORMAT_T *format, MMAL_PORT_T *port)
{
   const char *name_type;

   if(port)
      fprintf(stderr, "%s:%s:%i", port->component->name,
               port->type == MMAL_PORT_TYPE_CONTROL ? "ctr" :
                  port->type == MMAL_PORT_TYPE_INPUT ? "in" :
                  port->type == MMAL_PORT_TYPE_OUTPUT ? "out" : "invalid",
               (int)port->index);

   switch(format->type)
   {
   case MMAL_ES_TYPE_AUDIO: name_type = "audio"; break;
   case MMAL_ES_TYPE_VIDEO: name_type = "video"; break;
   case MMAL_ES_TYPE_SUBPICTURE: name_type = "subpicture"; break;
   default: name_type = "unknown"; break;
   }

   fprintf(stderr, "type: %s, fourcc: %4.4s\n", name_type, (char *)&format->encoding);
   fprintf(stderr, " bitrate: %i, framed: %i\n", format->bitrate,
            !!(format->flags & MMAL_ES_FORMAT_FLAG_FRAMED));
   fprintf(stderr, " extra data: %i, %p\n", format->extradata_size, format->extradata);
   switch(format->type)
   {
   case MMAL_ES_TYPE_AUDIO:
      fprintf(stderr, " samplerate: %i, channels: %i, bps: %i, block align: %i\n",
               format->es->audio.sample_rate, format->es->audio.channels,
               format->es->audio.bits_per_sample, format->es->audio.block_align);
      break;

   case MMAL_ES_TYPE_VIDEO:
      fprintf(stderr, " width: %i, height: %i, (%i,%i,%i,%i)\n",
               format->es->video.width, format->es->video.height,
               format->es->video.crop.x, format->es->video.crop.y,
               format->es->video.crop.width, format->es->video.crop.height);
      fprintf(stderr, " pixel aspect ratio: %i/%i, frame rate: %i/%i\n",
               format->es->video.par.num, format->es->video.par.den,
               format->es->video.frame_rate.num, format->es->video.frame_rate.den);
      break;

   case MMAL_ES_TYPE_SUBPICTURE:
      break;

   default: break;
   }

   if(!port)
      return;

   fprintf(stderr, " buffers num: %i(opt %i, min %i), size: %i(opt %i, min: %i), align: %i\n",
            port->buffer_num, port->buffer_num_recommended, port->buffer_num_min,
            port->buffer_size, port->buffer_size_recommended, port->buffer_size_min,
            port->buffer_alignment_min);
}

/** Callback from the control port.
 * Component is sending us an event. */
static void control_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
   printf("OK18\n");
   struct CONTEXT_T *ctx = (struct CONTEXT_T *)port->userdata;

   switch (buffer->cmd)
   {
   case MMAL_EVENT_EOS:
      printf("OK19\n");
      /* Only sink component generate EOS events */
      break;
   case MMAL_EVENT_ERROR:
      /* Something went wrong. Signal this to the application */
      printf("OK20\n");
      ctx->status = *(MMAL_STATUS_T *)buffer->data;
      break;
   default:
      break;
   }

   /* Done with the event, recycle it */
   mmal_buffer_header_release(buffer);

   /* Kick the processing thread */
   vcos_semaphore_post(&ctx->semaphore);
}

/** Callback from the input port.
 * Buffer has been consumed and is available to be used again. */
static void input_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
   struct CONTEXT_T *ctx = (struct CONTEXT_T *)port->userdata;

   /* The decoder is done with the data, just recycle the buffer header into its pool */
   mmal_buffer_header_release(buffer);

   /* Kick the processing thread */
   vcos_semaphore_post(&ctx->semaphore);
}

/** Callback from the output port.
 * Buffer has been produced by the port and is available for processing. */
static void output_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
   struct CONTEXT_T *ctx = (struct CONTEXT_T *)port->userdata;

   /* Queue the decoded video frame */
   mmal_queue_put(ctx->queue, buffer);

   /* Kick the processing thread */
   vcos_semaphore_post(&ctx->semaphore);
}

static MMAL_STATUS_T config_port(MMAL_PORT_T *port,
                                 const MMAL_FOURCC_T encoding,
                                 const int width, const int height)
{
   port->format->type = MMAL_ES_TYPE_VIDEO;
   port->format->encoding = encoding;
   port->format->es->video.width  = VCOS_ALIGN_UP(width,  32);
   port->format->es->video.height = VCOS_ALIGN_UP(height, 16);
   port->format->es->video.crop.x = 0;
   port->format->es->video.crop.y = 0;
   port->format->es->video.crop.width  = width;
   port->format->es->video.crop.height = height;

   port->format->es->video.frame_rate.num = 0;
   port->format->es->video.frame_rate.den = 1;
   port->format->es->video.par.num = 1;
   port->format->es->video.par.den = 1;


   return mmal_port_format_commit(port);
}

static void display_port_format_info(MMAL_PORT_T *port)
{
   fprintf(stderr, "%s\n", port->name);
   fprintf(stderr, " type: %i, fourcc: %4.4s\n", port->format->type, (char *)&(port->format)->encoding);
   fprintf(stderr, " bitrate: %i, framed: %i\n", port->format->bitrate,
           !!(port->format->flags & MMAL_ES_FORMAT_FLAG_FRAMED));
   fprintf(stderr, " extra data: %i, %p\n", port->format->extradata_size, port->format->extradata);
   fprintf(stderr, " width: %i, height: %i, (%i,%i,%i,%i)\n",
           port->format->es->video.width, port->format->es->video.height,
           port->format->es->video.crop.x, port->format->es->video.crop.y,
           port->format->es->video.crop.width, port->format->es->video.crop.height);
}


#define ENCODING_DECODER_IN MMAL_ENCODING_JPEG
#define ENCODING_DECODER_OUT MMAL_ENCODING_I422
#define ENCODING_ISP_OUT    MMAL_ENCODING_BGR24
#define WIDTH  1920
#define HEIGHT 1080
#define ZERO_COPY 0

#define MIN(a, b) \
  ({ __typeof__(a) _a = (a); \
     __typeof__(b) _b = (b); \
     _a < _b ? _a: _b; })

/**
 * Input file path -> Output RGB image
 */
DECODING_RESULT_T* decode_jpeg_mmal(char *file_path, bool use_mmap, bool debug_info)
{
   MMAL_STATUS_T status = MMAL_EINVAL;
   MMAL_COMPONENT_T *decoder = NULL;
   MMAL_COMPONENT_T *isp = NULL;
   MMAL_CONNECTION_T *conn_decoder_isp = NULL;
   MMAL_POOL_T *pool_in = NULL, *pool_out = NULL;
   MMAL_BOOL_T eos_sent = MMAL_FALSE, eos_received = MMAL_FALSE;
   unsigned int in_count = 0, out_count = 0;
   MMAL_BUFFER_HEADER_T *buffer;
   DECODING_RESULT_T *result = malloc(sizeof(DECODING_RESULT_T));
   result->length = 0;

   bcm_host_init();

   vcos_semaphore_create(&context.semaphore, "example", 1);


   /* Open file */
   int fd;
   struct stat s;
   const char * mapped;
   int not_read_bytes;
   int read_bytes;

   fd = open(file_path, O_RDONLY);
   if (fd < 0)
   {
      goto error;
   }

   if (fstat(fd, &s) < 0)
   {
      goto error;
   }

   mapped = mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
   if (mapped == MAP_FAILED)
   {
      goto error;
   }
   not_read_bytes = s.st_size;

   /* Create the decoder component.
    * This specific component exposes 2 ports (1 input and 1 output). Like most components
    * its expects the format of its input port to be set by the client in order for it to
    * know what kind of data it will be fed. */
   _check_mmal(mmal_component_create("vc.ril.image_decode", &decoder));

   /* Enable control port so we can receive events from the component */
   decoder->control->userdata = (void *)&context;
   _check_mmal(mmal_port_enable(decoder->control, control_callback));

   // Setup decoder component
   _check_mmal(config_port(decoder->input[0],
                           ENCODING_DECODER_IN, 0, 0));
   _check_mmal(config_port(decoder->output[0],
                           ENCODING_DECODER_OUT, WIDTH, HEIGHT));
   //_check_mmal(mmal_port_parameter_set(decoder->output[0],
   //                                    &source_pattern.hdr));
   _check_mmal(mmal_port_parameter_set_boolean(decoder->output[0],
                                                MMAL_PARAMETER_ZERO_COPY,
                                                MMAL_TRUE));
   /* Set the zero-copy parameter on the output port */
   _check_mmal(mmal_port_enable(decoder->input[0], input_callback));

   // Setup the isp component.
   _check_mmal(mmal_component_create("vc.ril.isp", &isp));
   _check_mmal(mmal_port_enable(isp->control, control_callback));
   _check_mmal(config_port(isp->input[0],
                           ENCODING_DECODER_OUT, WIDTH, HEIGHT));
   _check_mmal(config_port(isp->output[0],
                           ENCODING_ISP_OUT, WIDTH, HEIGHT));
   _check_mmal(mmal_port_enable(isp->output[0], output_callback));
   _check_mmal(mmal_port_parameter_set_boolean(isp->input[0],
                                                MMAL_PARAMETER_ZERO_COPY,
                                                MMAL_TRUE));
   _check_mmal(mmal_port_parameter_set_boolean(isp->output[0],
                                                MMAL_PARAMETER_ZERO_COPY,
                                                MMAL_FALSE));
   if (debug_info)
   {
      /* Display the output port format */
      display_port_format_info(decoder->output[0]);
      /* Display the output port format */
      display_port_format_info(isp->output[0]);
   }

   /* Create a queue to store our decoded frame(s). The callback we will get when
    * a frame has been decoded will put the frame into this queue. */
   context.queue = mmal_queue_create();

   /* The format of both ports is now set so we can get their buffer requirements and create
    * our buffer headers. We use the buffer pool API to create these. */
   decoder->input[0]->buffer_num = decoder->input[0]->buffer_num_recommended;
   decoder->input[0]->buffer_size = decoder->input[0]->buffer_size_recommended;
   decoder->output[0]->buffer_num = decoder->output[0]->buffer_num_recommended;
   decoder->output[0]->buffer_size = decoder->output[0]->buffer_size_recommended;
   isp->input[0]->buffer_num = isp->input[0]->buffer_num_recommended;
   isp->input[0]->buffer_size = isp->input[0]->buffer_size_recommended;
   isp->output[0]->buffer_num = isp->output[0]->buffer_num_recommended;
   isp->output[0]->buffer_size = isp->output[0]->buffer_size_recommended;
   pool_in = mmal_port_pool_create(decoder->input[0],
                                   decoder->input[0]->buffer_num,
                                   decoder->input[0]->buffer_size);


   /* Store a reference to our context in each port (will be used during callbacks) */
   decoder->input[0]->userdata = (void *)&context;
   decoder->output[0]->userdata = (void *)&context;
   isp->input[0]->userdata = (void *)&context;
   isp->output[0]->userdata = (void *)&context;

   pool_out = mmal_port_pool_create(isp->output[0],
                              isp->output[0]->buffer_num,
                              isp->output[0]->buffer_size);
   // real work

   while ((buffer = mmal_queue_get(pool_out->queue)) != NULL)
   {
      _check_mmal(mmal_port_send_buffer(isp->output[0], buffer));
   }

   /* Component won't start processing data until it is enabled. */
   _check_mmal(mmal_component_enable(decoder));
   _check_mmal(mmal_component_enable(isp));

   // Connect decoder[0] -- [0]isp
   _check_mmal(mmal_connection_create(&conn_decoder_isp,
                                    decoder->output[0], isp->input[0],
                                    MMAL_CONNECTION_FLAG_TUNNELLING));
   _check_mmal(mmal_connection_enable(conn_decoder_isp));

   /* Start decoding */
   if (debug_info)
   {
      fprintf(stderr, "start decoding\n");
   }

   /* This is the main processing loop */
   while(!eos_received && out_count < 10000)
   {
      VCOS_STATUS_T vcos_status;

      /* Wait for buffer headers to be available on either of the decoder ports */
      vcos_status = vcos_semaphore_wait_timeout(&context.semaphore, 2000);
      if (vcos_status != VCOS_SUCCESS)
         fprintf(stderr, "vcos_semaphore_wait_timeout failed - status %d\n", vcos_status);

      /* Check for errors */
      if (context.status != MMAL_SUCCESS)
         break;

      /* Send data to decode to the input port of the video decoder */
      if (!eos_sent && (buffer = mmal_queue_get(pool_in->queue)) != NULL)
      {
         // buffer->length = fread(buffer->data, 1, buffer->alloc_size - 128, source_file);

         read_bytes = MIN(not_read_bytes, buffer->alloc_size - 128);
         if (read_bytes > 0)
         {
            printf("OK 21 read bytes %d", read_bytes);
            not_read_bytes -= read_bytes;
            memcpy(buffer->data, mapped, read_bytes);
            buffer->length = read_bytes;
         }
         else {
            printf("OK 21 read bytes %d", read_bytes);
            buffer->length = 0;
         }
         buffer->offset = 0;

         if(!buffer->length) {
            eos_sent = MMAL_TRUE;
         }

         buffer->flags = buffer->length ? 0 : MMAL_BUFFER_HEADER_FLAG_EOS;
         buffer->pts = buffer->dts = MMAL_TIME_UNKNOWN;
         _check_mmal(mmal_port_send_buffer(decoder->input[0], buffer));
         in_count++;
         //fprintf(stderr, "Input buffer %p to port %s. in_count %u\n", buffer, decoder->input[0]->name, in_count);
      }

      /* Get our output frames */
      while ((buffer = mmal_queue_get(context.queue)) != NULL)
      {
         /* We have a frame, do something with it (why not display it for instance?).
          * Once we're done with it, we release it. It will automatically go back
          * to its original pool so it can be reused for a new video frame.
          */
         eos_received = buffer->flags & MMAL_BUFFER_HEADER_FLAG_EOS;

         if (buffer->cmd)
         {
            fprintf(stderr, "received event length %d, %4.4s\n", buffer->length, (char *)&buffer->cmd);
            if (buffer->cmd == MMAL_EVENT_FORMAT_CHANGED)
            {
               printf("OK17\n");
               MMAL_EVENT_FORMAT_CHANGED_T *event = mmal_event_format_changed_get(buffer);
               if (event)
               {
                  fprintf(stderr, "----------Port format changed----------\n");
                  log_format(isp->output[0]->format, isp->output[0]);
                  fprintf(stderr, "-----------------to---------------------\n");
                  log_format(event->format, 0);
                  fprintf(stderr, " buffers num (opt %i, min %i), size (opt %i, min: %i)\n",
                           event->buffer_num_recommended, event->buffer_num_min,
                           event->buffer_size_recommended, event->buffer_size_min);
                  fprintf(stderr, "----------------------------------------\n");
               }
               mmal_buffer_header_release(buffer);
               mmal_port_disable(isp->output[0]);

               //Clear out the queue and release the buffers.
               while(mmal_queue_length(pool_out->queue) < pool_out->headers_num)
               {
                  buffer = mmal_queue_wait(context.queue);
                  mmal_buffer_header_release(buffer);
                  fprintf(stderr, "Retrieved buffer %p\n", buffer);
               }

               //Assume we can't reuse the output buffers, so have to disable, destroy
               //pool, create new pool, enable port, feed in buffers.
               mmal_port_pool_destroy(isp->output[0], pool_out);

               status = mmal_format_full_copy(isp->output[0]->format, event->format);
               isp->output[0]->format->encoding = ENCODING_DECODER_OUT;
               isp->output[0]->buffer_num = MAX_BUFFERS;
               isp->output[0]->buffer_size = decoder->output[0]->buffer_size_recommended;

               if (status == MMAL_SUCCESS)
                  status = mmal_port_format_commit(isp->output[0]);
               if (status != MMAL_SUCCESS)
               {
                  fprintf(stderr, "commit failed on output - %d\n", status);
               }

               mmal_port_enable(isp->output[0], output_callback);
               pool_out = mmal_port_pool_create(isp->output[0], isp->output[0]->buffer_num, isp->output[0]->buffer_size);
            }
            else
            {
               mmal_buffer_header_release(buffer);
            }
            continue;
         }
         else
         {
            if (debug_info)
            {
               fprintf(stderr, "decoded frame (flags %x, size %d) count %d\n", buffer->flags, buffer->length, out_count);
            }
            result->data = malloc(buffer->length);
            result->length = buffer->length;
            memcpy(result->data, buffer->data, buffer->length);

            // Do something here with the content of the buffer

            mmal_buffer_header_release(buffer);

            out_count++;
         }
      }

      /* Send empty buffers to the output port of the decoder */
      while ((buffer = mmal_queue_get(pool_out->queue)) != NULL)
      {
         _check_mmal(mmal_port_send_buffer(isp->output[0], buffer));
      }
   }

   /* Stop decoding */
   if (debug_info)
   {
      fprintf(stderr, "stop decoding\n");
   }

   /* Stop everything. Not strictly necessary since mmal_component_destroy()
    * will do that anyway */
   _check_mmal(mmal_connection_disable(conn_decoder_isp));
   _check_mmal(mmal_port_disable(decoder->input[0]));
   _check_mmal(mmal_port_disable(isp->output[0]));
   _check_mmal(mmal_component_disable(decoder));
   _check_mmal(mmal_component_disable(isp));

error:
   /* Cleanup everything */
   if (pool_in)
   {
      mmal_port_pool_destroy(decoder->input[0], pool_in);
   }
   if (pool_out)
   {
      mmal_port_pool_destroy(isp->output[0], pool_out);
   }
   if (decoder)
   {
      mmal_component_destroy(decoder);
   }
   if (decoder)
   {
      mmal_component_destroy(isp);
   }
   if (context.queue) {
      mmal_queue_destroy(context.queue);
   }

   if (mapped) {
      munmap(mapped, s.st_size);
      close(fd);
   }
   vcos_semaphore_delete(&context.semaphore);
   if (status != MMAL_SUCCESS)
   {
      if (result->length > 0)
      {
         free(result->data);
         result->length = 0;
      }
      result->errors = "errors";
   }
   return result;
}
