#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeff Schornick");
MODULE_DESCRIPTION("APES HW2 Problem 5");
MODULE_VERSION("0.01");

static struct timer_list apes_timer;

#define APES_TIMER_FLAGS 0
#define TIMER_PERIOD_MS  500

void apes_timer_callback( struct timer_list *timer )
{
    static uint32_t calls = 0;
    printk( "Timer expired @ %ld (count = %u)\n", jiffies, calls );
    mod_timer( &apes_timer, jiffies + msecs_to_jiffies(TIMER_PERIOD_MS) );
}

static int __init apes_init(void)
{
    int retval;

    printk(KERN_INFO "Module init!\n");

    timer_setup(&apes_timer, apes_timer_callback, APES_TIMER_FLAGS);

    retval = mod_timer( &apes_timer, jiffies + msecs_to_jiffies(TIMER_PERIOD_MS) );
    if( retval ) {
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
