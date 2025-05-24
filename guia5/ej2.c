#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // copy_to_user
#include <linux/cdev.h>
#include <linux/init.h>

// Registros
// CHRONO_CURRENT_TIME = 0
// CHRONO_CTRL = 1
// CHRONO_RESET = 1

struct cdev chrono_cdev;


ssize_t chrono_read(struct file *filp, char *buffer, size_t len, loff_t *offset) {
    int tiempo = IN(0); // CHRONO_CURRENT_TIME
    copy_to_user(buffer, &tiempo, sizeof(tiempo));
    return sizeof(tiempo);
}

ssize_t chrono_write(struct file *filp, const char *buffer, size_t len, loff_t *offset) {
    OUT(1, 1); // CHRONO_CTRL = 1, CHRONO_RESET = 1
    return 1;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = chrono_read,
    .write = chrono_write,
};
