#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // copy_to_user
#include <linux/cdev.h>
#include <linux/init.h>

// Registros
// BTN_STATUS

struct cdev chrono_cdev;


ssize_t btn_read(struct file *filp, char *buffer, size_t len, loff_t *offset) {
    int status;
    
    // Busy waiting hasta que se presione la tecla (bit 0 en 1)
    do {
        status = IN(0); // BTN_STATUS en el registro 0
    } while ((status == 0); // Espera activa mientras bit 0 == 0

    // Limpiamos el estado de la tecla (bit 1 en 0)
    OUT(0, status & ~2); // Forzamos bit 1 a 0 (los demás igual)

    int btn_value = 1; // BTN_PRESSED
    copy_to_user(buffer, &btn_value, sizeof(btn_value));
    return sizeof(btn_value);
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = btn_read
};
