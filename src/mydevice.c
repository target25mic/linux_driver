#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "mydevice"
#define CLASS_NAME  "mydevice_class"
#define BUFFER_SIZE 1024

#define MYDEVICE_IOC_MAGIC 'M'
#define MYDEVICE_GET_BUFFER_SIZE \
    _IOR(MYDEVICE_IOC_MAGIC, 1, int)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Linux character device driver with read and write support");
MODULE_VERSION("1.0");

static dev_t dev_number;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;

static char device_buffer[BUFFER_SIZE];
static size_t data_size;
static DEFINE_MUTEX(buffer_mutex);

static int mydevice_open(struct inode *inode, struct file *file)
{
    pr_info("mydevice: device opened\n");
    return 0;
}

static int mydevice_release(struct inode *inode, struct file *file)
{
    pr_info("mydevice: device closed\n");
    return 0;
}

static ssize_t mydevice_read(struct file *file,
                             char __user *user_buffer,
                             size_t count, loff_t *offset)
{
    size_t bytes_to_copy;
    if (*offset < 0)
        return -EINVAL;

    if (mutex_lock_interruptible(&buffer_mutex))
        return -ERESTARTSYS;

    if (*offset >= data_size) {
        mutex_unlock(&buffer_mutex);
        return 0;
    }

    bytes_to_copy = min(count, data_size - (size_t)*offset);

    if (copy_to_user(user_buffer, device_buffer + *offset,
                     bytes_to_copy)) {
        mutex_unlock(&buffer_mutex);
        return -EFAULT;
    }

    *offset += bytes_to_copy;
    mutex_unlock(&buffer_mutex);

    pr_info("mydevice: read %zu bytes\n", bytes_to_copy);
    return bytes_to_copy;
}

static ssize_t mydevice_write(struct file *file,
                              const char __user *user_buffer,
                              size_t count, loff_t *offset)
{
    size_t bytes_to_copy;

    if (*offset < 0 || *offset >= BUFFER_SIZE)
        return -EINVAL;

    if (mutex_lock_interruptible(&buffer_mutex))
        return -ERESTARTSYS;

    bytes_to_copy = min(count, (size_t)(BUFFER_SIZE - *offset));

    if (copy_from_user(device_buffer + *offset,
                       user_buffer, bytes_to_copy)) {
        mutex_unlock(&buffer_mutex);
        return -EFAULT;
    }

    *offset += bytes_to_copy;

    if (*offset > data_size)
        data_size = *offset;

    mutex_unlock(&buffer_mutex);

    pr_info("mydevice: wrote %zu bytes\n", bytes_to_copy);
    return bytes_to_copy;
    
}
static long mydevice_ioctl(struct file *file,
                           unsigned int cmd,
                           unsigned long arg)
{
    int buffer_size = BUFFER_SIZE;

    switch (cmd) {
    case MYDEVICE_GET_BUFFER_SIZE:
        if (copy_to_user((int __user *)arg,
                         &buffer_size,
                         sizeof(buffer_size))) {
            return -EFAULT;
        }
        return 0;

    default:
        return -EINVAL;
    }
}
static const struct file_operations mydevice_fops = {
    .owner = THIS_MODULE,
    .open = mydevice_open,
    .read = mydevice_read,
    .write = mydevice_write,
    .unlocked_ioctl = mydevice_ioctl,
    .release = mydevice_release,
    .llseek = default_llseek,
};

static int __init mydevice_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("mydevice: failed to allocate device number\n");
        return ret;
    }

    cdev_init(&my_cdev, &mydevice_fops);
    my_cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_cdev, dev_number, 1);
    if (ret < 0)
        goto unregister_region;

    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        ret = PTR_ERR(my_class);
        goto delete_cdev;
    }

    my_device = device_create(my_class, NULL, dev_number,
                              NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        ret = PTR_ERR(my_device);
        goto destroy_class;
    }

    pr_info("mydevice: registered major=%d minor=%d\n",
            MAJOR(dev_number), MINOR(dev_number));
    return 0;

destroy_class:
    class_destroy(my_class);
delete_cdev:
    cdev_del(&my_cdev);
unregister_region:
    unregister_chrdev_region(dev_number, 1);
    return ret;
}

static void __exit mydevice_exit(void)
{
    device_destroy(my_class, dev_number);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);

    pr_info("mydevice: module unloaded\n");
}

module_init(mydevice_init);
module_exit(mydevice_exit);
