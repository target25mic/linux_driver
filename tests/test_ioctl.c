#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#define DEVICE_PATH "/dev/mydevice"
#define MYDEVICE_IOC_MAGIC 'M'
#define MYDEVICE_GET_BUFFER_SIZE \
    _IOR(MYDEVICE_IOC_MAGIC, 1, int)

int main(void)
{
    int fd;
    int buffer_size = 0;

    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    if (ioctl(fd, MYDEVICE_GET_BUFFER_SIZE, &buffer_size) < 0) {
        perror("ioctl failed");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Device buffer capacity: %d bytes\n", buffer_size);

    if (buffer_size == 1024) {
        printf("TEST PASSED: ioctl returned the correct buffer size.\n");
    } else {
        printf("TEST FAILED: Unexpected buffer size.\n");
        close(fd);
        return EXIT_FAILURE;
    }
    if (ioctl(fd, 999, &buffer_size) != -1) {
    printf("TEST FAILED: Unsupported ioctl was accepted.\n");
    close(fd);
    return EXIT_FAILURE;
}

if (errno == EINVAL) {
    printf("TEST PASSED: Unsupported ioctl rejected.\n");
} else {
    perror("Unexpected ioctl error");
    close(fd);
    return EXIT_FAILURE;
}

    close(fd);
    return EXIT_SUCCESS;
}
