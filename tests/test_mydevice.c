#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define DEVICE_PATH "/dev/mydevice"

int main(void)
{
    int fd;
    char write_buffer[] = "Hello from user space!";
    char read_buffer[100] = {0};
    ssize_t bytes_written;
    ssize_t bytes_read;

    fd = open(DEVICE_PATH, O_RDWR);

    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    printf("Device opened successfully.\n");

    bytes_written = write(fd, write_buffer, strlen(write_buffer));

    if (bytes_written < 0) {
        perror("Write failed");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Wrote %zd bytes: %s\n", bytes_written, write_buffer);

    bytes_read = read(fd, read_buffer, sizeof(read_buffer) - 1);

    if (bytes_read < 0) {
        perror("Read failed");
        close(fd);
        return EXIT_FAILURE;
    }

    read_buffer[bytes_read] = '\0';
    printf("Read %zd bytes: %s\n", bytes_read, read_buffer);

    if (strcmp(write_buffer, read_buffer) == 0) {
        printf("TEST PASSED: Written and read data match.\n");
    } else {
        printf("TEST FAILED: Data does not match.\n");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);
    printf("Device closed successfully.\n");

    return EXIT_SUCCESS;
}
