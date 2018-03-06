#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <../kernel/sched/sched.h>

#include <linux/kthread.h>
#include <linux/kfifo.h>
//#include <linux/spinlock.h>

#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeff Schornick");
MODULE_DESCRIPTION("APES HW4 P4");
MODULE_VERSION("0.01");

#define DEFAULT_TIMER_PERIOD_MS 500
#define TIMER_FLAGS 0

#define TASK_CUR 0
#define TASK_PREV 1
#define TASK_NEXT 2

static char *name;
module_param(name, charp, S_IRUGO | S_IWUSR);

static uint timer_period_ms = DEFAULT_TIMER_PERIOD_MS;
module_param(timer_period_ms, uint, S_IRUGO | S_IWUSR);

static struct timer_list apes_timer;

struct task_struct *logger_task;

struct runtime_data {
    pid_t pid[3];
    u64  vruntime[3];
};

struct kfifo fifo;
spinlock_t lock;

void apes_timer_callback( struct timer_list *timer )
{
    //static uint32_t count = 0;

    struct runtime_data rtd;
    struct task_struct *task;

    rtd.vruntime[TASK_CUR] = current->se.vruntime;
    rtd.pid[TASK_CUR] = current->pid;

    task = next_task(current);
    rtd.pid[TASK_NEXT] = task->pid;
    rtd.vruntime[TASK_NEXT] = task->se.vruntime;

    task = list_entry(current->tasks.prev, struct task_struct, tasks);
    rtd.pid[TASK_PREV] = task->pid;
    rtd.vruntime[TASK_PREV] = task->se.vruntime;

    //printk( "Timer expired @ %ld (name = %s, count = %u)\n", jiffies, name, ++count );

    printk( KERN_INFO "hw4p4: Adding new data to FIFO\n");
    kfifo_in( &fifo, &rtd, sizeof(rtd) );
    wake_up_process(logger_task);

    if( mod_timer(&apes_timer, jiffies + msecs_to_jiffies(timer_period_ms)) ) {
        printk(KERN_ERR "hw4p4: Could not modify timer!\n");
    }
}

int logger_func(void *data)
{

    struct runtime_data rtd;
    uint len;

    printk(KERN_INFO "hw4p4: Logger thread, PID: %d\n", current->pid);
    while(!kthread_should_stop()) {
        set_current_state(TASK_INTERRUPTIBLE);
        schedule();
        printk(KERN_INFO "hw4p4: Logger woke up\n");
        if( !kfifo_is_empty(&fifo) ) {
            len = kfifo_out( &fifo, &rtd, sizeof(rtd) );
            printk(KERN_INFO "hw4p4: Logger thread got %u bytes from FIFO\n", len);
            //printk(KERN_INFO "hw4p4: Got %u bytes\n", len);
            printk(KERN_INFO "hw4p4: CURR: pid = %d, vruntime = %llu\n", rtd.pid[TASK_CUR], rtd.vruntime[TASK_CUR]);
            printk(KERN_INFO "hw4p4: PREV: pid = %d, vruntime = %llu\n", rtd.pid[TASK_PREV], rtd.vruntime[TASK_PREV]);
            printk(KERN_INFO "hw4p4: NEXT: pid = %d, vruntime = %llu\n", rtd.pid[TASK_NEXT], rtd.vruntime[TASK_NEXT]);
        }
    }
    printk(KERN_INFO "hw4p4: Logger thread exitinng");
    return 0;
}

static int __init hw4p4_init(void)
{

    printk(KERN_INFO "hw4p4: Module init!\n");

    printk(KERN_INFO "hw4p4: Allocting new kfifo\n");
    if( kfifo_alloc(&fifo, 1024, GFP_KERNEL) ) {
        printk(KERN_ERR "Failed to allocate kfifo");
        return -1;
    };


    timer_setup(&apes_timer, apes_timer_callback, TIMER_FLAGS);

    printk(KERN_INFO "hw4p4: Setting up timer with period of %u ms\n", timer_period_ms);
    if( mod_timer(&apes_timer, jiffies + msecs_to_jiffies(timer_period_ms)) ) {
        printk(KERN_ERR "hw4p4: Could not modify timer!\n");
        return -1;
    }

    logger_task = kthread_run( logger_func, NULL, "hw4p4_logger" );
    if( IS_ERR(logger_task) ) {
        printk(KERN_ERR "hw4p4: Could not start logger thread\n");
        return PTR_ERR(logger_task);
    }


    return 0;
}

static void __exit hw4p4_exit(void)
{

    if ( del_timer(&apes_timer) ) {
        printk(KERN_INFO "hw4p4: Deleted active timer\n");
    } else {
        printk(KERN_INFO "hw4p4: No active timer to delete\n");
    }

    kfifo_free(&fifo);

    printk(KERN_INFO "hw4p4: telling logger task to stop\n");
    kthread_stop(logger_task);
    printk(KERN_INFO "hw4p4: Module exit!\n");
}

module_init(hw4p4_init);
module_exit(hw4p4_exit);
