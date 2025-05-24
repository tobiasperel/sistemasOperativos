#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // copy_to_user
#include <linux/cdev.h>
#include <linux/init.h>

// Registros
// BTN_STATUS

struct cdev chrono_cdev;

irqreturn_t btn_irq_handler(int irq, void *dev_id) {
    // Limpiamos el estado de la tecla (bit 1 en 0)
    int stat
    OUT(0, status & ~2); // Forzamos bit 1 a 0 (los demás igual)

    // Desbloquear el read
    up(&sem);
    return IRQ_HANDLED;
}


ssize_t btn_read(struct file *filp, char *buffer, size_t len, loff_t *offset) {
    

    int btn_value = 1; // BTN_PRESSED
    copy_to_user(buffer, &btn_value, sizeof(btn_value));
    return sizeof(btn_value);
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = btn_read
};

static int __init init_module(void) {
    
    // Inicializamos semáforo en 0 (bloqueado)
    sema_init(&sem, 0);

    // Pedimos la interrupción 7
    request_irq(7, btn_irq_handler, 0, "btn_irq", NULL);

    // Habilitamos las interrupciones del botón
    OUT(0, 4); // BTN_INT == 4

    return 0;
}

void __exit cleanup_module(void) {
    unregister_chrdev(major, "btn");
    free_irq(7, NULL);
    printk(KERN_INFO "btn eliminado\n");
}

MODULE_LICENSE("GPL");