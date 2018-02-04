#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeff Schornick");
MODULE_DESCRIPTION("APES HW2 Problem 5");
MODULE_VERSION("0.01");

static int __init apes_init(void) {
 printk(KERN_INFO "Module init!\n");
 return 0;
}

static void __exit apes_exit(void) {
 printk(KERN_INFO "Module exit!\n");
}

module_init(apes_init);
module_exit(apes_exit);
