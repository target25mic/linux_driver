#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_PATH "/dev/mydevice"
#define BUFFER_SIZE 1024

int main(void)
{
    int fd;
    char buffer[BUFFER_SIZE + 100];
    ssize_t bytes_written;

    memset(buffer, 'A', sizeof(buffer));

    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    bytes_written = write(fd, buffer, sizeof(buffer));

    if (bytes_written < 0) {
        perror("Write failed");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Requested write: %zu bytes\n", sizeof(buffer));
    printf("Actual write: %zd bytes\n", bytes_written);

    if (bytes_written == BUFFER_SIZE) {
        printf("TEST PASSED: Buffer limit enforced.\n");
    } else {
        printf("TEST FAILED: Unexpected write size.\n");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);
    return EXIT_SUCCESS;
}
