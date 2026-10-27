#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("A basic Linux character device driver");
MODULE_VERSION("1.0");

static int __init mydevice_init(void)
{
    pr_info("mydevice: module loaded\n");
    return 0;
}

static void __exit mydevice_exit(void)
{
    pr_info("mydevice: module unloaded\n");
}

module_init(mydevice_init);
module_exit(mydevice_exit);

