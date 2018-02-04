#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeff Schornick");
MODULE_DESCRIPTION("APES HW2 Problem 5");
MODULE_VERSION("0.01");

#define DEFAULT_TIMER_PERIOD_MS 500
#define TIMER_FLAGS 0

static char *name;
module_param(name, charp, S_IRUGO | S_IWUSR);

static uint timer_period_ms = DEFAULT_TIMER_PERIOD_MS;
module_param(timer_period_ms, uint, S_IRUGO | S_IWUSR);

static struct timer_list apes_timer;


void apes_timer_callback( struct timer_list *timer )
{
    static uint32_t count = 0;
    printk( "Timer expired @ %ld (name = %s, count = %u)\n", jiffies, name, ++count );
    if( mod_timer(&apes_timer, jiffies + msecs_to_jiffies(timer_period_ms)) ) {
        printk(KERN_ERR "Could not modify timer!\n");
    }
}

static int __init apes_init(void)
{
    printk(KERN_INFO "Module init!\n");

    timer_setup(&apes_timer, apes_timer_callback, TIMER_FLAGS);

    printk(KERN_INFO "Setting up timer with period of %u ms\n", timer_period_ms);
    if( mod_timer(&apes_timer, jiffies + msecs_to_jiffies(timer_period_ms)) ) {
        printk(KERN_ERR "Could not modify timer!\n");
    }

    return 0;
}

static void __exit apes_exit(void)
{
    if ( del_timer(&apes_timer) ) {
        printk(KERN_INFO "Deleted active timer\n");
    } else {
        printk(KERN_INFO "No active timer to delete\n");
    }
    printk(KERN_INFO "Module exit!\n");
}

module_init(apes_init);
module_exit(apes_exit);
