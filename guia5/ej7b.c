#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // copy_to_user
#include <linux/cdev.h>
#include <linux/init.h>


irqreturn_t btn_irq_controladora(int irq, void *dev_id) {
    if (IN(1) == 1) { // ARM_STATUS
        up(&sem_brazo_listo);
    }
    if (IN(2) == 1) { // DATA_READY
        up(&sem_data_ready);
    }
    return IRQ_HANDLED;
}

irqreturn_t btn_irq_timer(int irq, void *dev_id) {
   
    // Desbloquear el write
    up(&sem2);
    return IRQ_HANDLED;
}

void write(int sector, void *data){
    int sectores_por_pista = cantidad_sectores_por_pista();
    int pista = sector / sectores_por_pista;
    int sector_rel = sector % sectores_por_pista;

 

    // Encender el motor si está apagado
    if (IN(0) == 0) { // DOR_IO
        OUT(0, 1); // DOR_IO = 1 → encender
        down(&sem2); 
    }

    // Si el motor aún no se apagó del todo, esperar
    while (IN(0) != 0) { // DOR_STATUS sigue en 1
    }

    down(&sem2);
    down(&sem2);
    down(&sem2);
    down(&sem2);

    
    // para el brazo
    OUT(1, pista); // Indica al brazo que debe ir a la pista indicada.
    down(&sem_brazo_listo); // Esperar que ARM_STATUS sea 1

    OUT(2, sector_rel); // SEEK_SECTOR
    down(&sem_data_ready); // Esperar que DATA_READY sea 1
    escribir_datos(data);

    OUT(0, 0); // DOR_IO = 0
}


struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = btn_read
};

static int __init init_module(void) {
    
    // Inicializamos semáforo en 0 (bloqueado)
    sema_init(&sem_brazo_listo, 0);
    sema_init(&sem_data_ready, 0);
    sema_init(&sem2, 0);

    // Pedimos la interrupción 6
    request_irq(6, btn_irq_controladora, 0, "btn_irq", NULL);

    // Pedimos la interrupción 7
    request_irq(7, btn_irq_timer, 0, "btn_irq", NULL);

   

    return 0;
}

void __exit cleanup_module(void) {
    unregister_chrdev(major, "btn");
    free_irq(6, NULL);
    free_irq(7, NULL);

    printk(KERN_INFO "btn eliminado\n");
}

MODULE_LICENSE("GPL");