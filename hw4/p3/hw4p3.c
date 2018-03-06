#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/sched.h>  // task_struct
#include <linux/slab.h>  // kmalloc

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeff Schornick");
MODULE_DESCRIPTION("APES HW4 Problem 3");
MODULE_VERSION("0.01");

#define STATE_RUN 0
#define STATE_UNRUN 1
#define STATE_STOP 2

const char state_strs[][11] = { "running", "unrunnable", "stopped" };

extern struct task_struct init_task;

static int __init hw4p3_init(void)
{
    struct task_struct *task;
    //char *status_str = kmalloc(1024, GFP_KERNEL);
    //char status_str[1024];
    const char *state_str;
    uint child_count = 0;
    struct list_head *node;

    printk(KERN_INFO "hw4p3: Module init!\n");

    printk(KERN_INFO "hw4p3: Walking process tree\n");

    // print name, PID, status, # of children, and nice value of current process
    // and all anscestor processes
    for ( task = current; task != &init_task; task = task->parent ) {

        if (task->state == 0) {
            state_str = state_strs[STATE_RUN];
        } else  if (task->state == -1) {
            state_str = state_strs[STATE_UNRUN];
        } else {
            state_str = state_strs[STATE_STOP];
        }

        list_for_each( node, &task->children) {
            child_count++;
        }

        printk( KERN_INFO "hw4p3: PID: %d, Name: '%s', State: %ld (%s), Children: %u, Nice: %d", task->pid, task->comm, task->state, state_str, child_count, task_nice(task));
        /* snprintf( status_str, 1024, "PID: %d, Name: '%s', State: %ld (%s), Children: %u", task->pid, task->comm, task->state, state_str, child_count); */
        /* printk(KERN_INFO "%s\n", status_str); */
    }

    //kfree(str);

    return 0;
}

static void __exit hw4p3_exit(void)
{
    printk(KERN_INFO "hw4p3: Module exit!\n");
}

module_init(hw4p3_init);
module_exit(hw4p3_exit);
