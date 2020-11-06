#include "gencmd.h"

char* send_command(char *command)
{
    VCHI_INSTANCE_T vchi_instance;
    VCHI_CONNECTION_T *vchi_connection = NULL;

    vcos_init();

    if ( vchi_initialise( &vchi_instance ) != 0)
    {
        fprintf( stderr, "VCHI initialization failed\n" );
        return "Error: VCHI initialization failed";
    }

    //create a vchi connection
    if ( vchi_connect( NULL, 0, vchi_instance ) != 0)
    {
        fprintf( stderr, "VCHI connection failed\n" );
        return "Error: VCHI connection failed";
    }

    vc_vchi_gencmd_init(vchi_instance, &vchi_connection, 1 );


    int i = 1;
    char buffer[ GENCMDSERVICE_MSGFIFO_SIZE ];
    size_t buffer_offset = 0;
    clock_t before=0, after=0;
    double time_diff;
    uint32_t show_time = 0;
    int ret;

    //reset the string
    buffer[0] = '\0';

    buffer_offset = vcos_safe_strcpy( buffer, command, sizeof(buffer), buffer_offset );
    buffer_offset = vcos_safe_strcpy( buffer, " ", sizeof(buffer), buffer_offset );

    //send the gencmd for the argument
    if (( ret = vc_gencmd_send( "%s", buffer )) != 0 )
    {
        printf( "vc_gencmd_send returned %d\n", ret );
    }

    //get + print out the response!
    if (( ret = vc_gencmd_read_response( buffer, sizeof( buffer ) )) != 0 )
    {
        printf( "vc_gencmd_read_response returned %d\n", ret );
    }
    buffer[ sizeof(buffer) - 1 ] = 0;

    vc_gencmd_stop();

    //close the vchi connection
    if ( vchi_disconnect( vchi_instance ) != 0)
    {
        fprintf( stderr, "VCHI disconnect failed\n" );
    }
    char *result = malloc(strlen( buffer ));
    vcos_safe_strcpy( result, buffer, strlen(buffer), 0 );
    return result;
}

