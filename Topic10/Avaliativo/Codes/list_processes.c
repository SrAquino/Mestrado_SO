#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SrAquino");
MODULE_DESCRIPTION("Módulo de Kernel que percorre a lista de processos.");
MODULE_VERSION("1.0");

static int __init list_processes_init(void)
{
    struct task_struct *task;

    printk(KERN_INFO "Iniciando o módulo de kernel para listar processos.\n");

    // Itera pela lista de processos
    for_each_process(task)
    {
        printk(KERN_INFO "PID: %d | Nome: %s | Estado: %ld\n",
               task->pid, task->comm, task->state);
    }

    return 0; // Retorna sucesso
}

static void __exit list_processes_exit(void)
{
    printk(KERN_INFO "Removendo o módulo de kernel.\n");
}

module_init(list_processes_init);
module_exit(list_processes_exit);
