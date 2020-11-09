#include "read_file_with_mmap.h"

READING_RESULT_T* read_file(char *file_path)
{
    READING_RESULT_T *result = malloc(sizeof(READING_RESULT_T));
    result->length = -1;

    int fd;
    struct stat s;
    const char * mapped;
    int i;

    fd = open(file_path, O_RDONLY);
    if (fd < 0)
    {
        char * error = strerror(errno);
        result->errors = malloc(strlen(error));
        memcpy(result->errors, error, strlen(error));
	close(fd);
        return result;
    }

    if (fstat(fd, &s) < 0)
    {
        char * error = strerror(errno);
        result->errors = malloc(strlen(error));
        memcpy(result->errors, error, strlen(error));
	close(fd);
        return result;
    }

    mapped = mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED)
    {
        char * error = strerror(errno);
        result->errors = malloc(strlen(error));
        memcpy(result->errors, error, strlen(error));
	close(fd);
        return result;
    }

    result->data = malloc(s.st_size);
    memcpy(result->data, mapped, s.st_size);
    result->length = s.st_size;

    if (munmap(mapped , s.st_size) < 0)
    {
        free(result->data);
        char * error = strerror(errno);
        result->errors = malloc(strlen(error));
        memcpy(result->errors, error, strlen(error));
        result->length = -1;
	close(fd);
        return result;
    }
    close(fd);
    return result;
}
