#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");

dev_t dev;
struct cdev *meu_cdev;
struct list_head data_list;

struct data_node {
    struct list_head list;
    char *data;
};

int meu_open(struct inode *inode, struct file *filp) {
    return 0;
}

ssize_t meu_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
    struct data_node *node;
    ssize_t ret;

    if (list_empty(&data_list)) {
        if (*off == 0) {
            if (copy_to_user(buf, "vazia", 5)) {
                return -EFAULT;
            }
            *off += 5;
            return 5;
        } else {
            return 0;
        }
    }

    node = list_first_entry(&data_list, struct data_node, list);
    list_del(&node->list);

    if (copy_to_user(buf, node->data, strlen(node->data))) {
        ret = -EFAULT;
    } else {
        ret = strlen(node->data);
        *off += ret;
    }

    kfree(node->data);
    kfree(node);

    return ret;
}

ssize_t meu_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
    struct data_node *node;

    node = kmalloc(sizeof(*node), GFP_KERNEL);
    if (!node) {
        return -ENOMEM;
    }

    node->data = kmalloc(len, GFP_KERNEL);
    if (!node->data) {
        kfree(node);
        return -ENOMEM;
    }

    if (copy_from_user(node->data, buf, len)) {
        kfree(node->data);
        kfree(node);
        return -EFAULT;
    }

    list_add_tail(&node->list, &data_list);

    return len;
}

struct file_operations m_fops = {
    .owner = THIS_MODULE,
    .open = meu_open,
    .read = meu_read,
    .write = meu_write,
};

static int __init meu_init(void) {
    int major, minor;
    printk(KERN_ALERT "Iniciando . . .\n");
    alloc_chrdev_region(&dev, 0, 1, "drive_SO");
    major = MAJOR(dev);
    minor = MINOR(dev);
    printk(KERN_ALERT "Major: %d Minor: %d\n", major, minor);

    meu_cdev = cdev_alloc();
    meu_cdev->ops = &m_fops;
    cdev_add(meu_cdev, dev, 1);

    INIT_LIST_HEAD(&data_list);

    return 0;
}

static void __exit meu_exit(void) {
    struct data_node *node, *tmp;

    list_for_each_entry_safe(node, tmp, &data_list, list) {
        list_del(&node->list);
        kfree(node->data);
        kfree(node);
    }

    cdev_del(meu_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_ALERT "Saindo . . .\n");
}

module_init(meu_init);
module_exit(meu_exit);