#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define DEVICE_PATH "/dev/mydevice"
#define BUFFER_SIZE 1024

int main(void)
{
    int fd;
    char value = 'X';

    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    if (lseek(fd, BUFFER_SIZE, SEEK_SET) < 0) {
        perror("Failed to set offset");
        close(fd);
        return EXIT_FAILURE;
    }

    errno = 0;
    ssize_t result = write(fd, &value, 1);

    if (result == -1 && errno == EINVAL) {
        printf("TEST PASSED: Write at buffer-end offset rejected.\n");
    } else {
        printf("TEST FAILED: Unexpected result: %zd\n", result);
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);
    return EXIT_SUCCESS;
}
