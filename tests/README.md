# Character Device Driver Tests

## Test Program

`test_mydevice.c` checks basic communication between user space
and the Linux character device driver.

## What It Tests

1. Opens `/dev/mydevice` for reading and writing.
2. Writes a test message to the device.
3. Reads the message back.
4. Compares the written and read data.
5. Reports whether the test passed or failed.
6. Closes the device.

## Compile

From the project root:

```bash
gcc -Wall -Wextra -o tests/test_mydevice tests/test_mydevice.c
