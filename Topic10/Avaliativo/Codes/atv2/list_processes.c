#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

dev_t dev;
struct cdev *meu_cdev;

static int __init listar_processos_init(void)
{
    struct task_struct *task;

    printk(KERN_INFO "Iniciando o module para listar processos...\n");

    for_each_process(task) {
        printk(KERN_INFO "PID: %d | Nome: %s | Estado: %ld\n",
               task->pid, task->comm, task->state);
    }

    return 0;
}

int meu_open(struct inode *inode, struct file *filp){
    int minor;
    minor = iminor(inode);
    printk(KERN_ALERT "Minor> %d\n", minor);
    return 0;
}

struct file_operations m_fops = 
{
    .owner = THIS_MODULE,
    .open = meu_open,
};

static int meu_init(void){
    int major, minor;
    printk(KERN_ALERT "Iniciando . . .\n");
    alloc_chrdev_region(&dev,0,1,"drive_SO");
    major = MAJOR(dev);
    minor = MINOR(dev);
    printk(KERN_ALERT "Major: %d Minor: %d\n",major,minor);

    meu_cdev = cdev_alloc();
    meu_cdev->ops = &m_fops;
    cdev_add(meu_cdev,dev,1);
    
    return 0;
}

static void meu_exit(void){

    unregister_chrdev_region(dev,1);
    cdev_del(meu_cdev);
    printk(KERN_ALERT "Finalizando . . .\n");
}

module_init(listar_processos_init);
module_exit(meu_exit);