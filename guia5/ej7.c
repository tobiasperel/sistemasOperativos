void write(int sector, void *data){
    int sectores_por_pista = cantidad_sectores_por_pista();
    int pista = sector / sectores_por_pista;
    int sector_rel = sector % sectores_por_pista;

 

    // Encender el motor si está apagado
    if (IN(0) == 0) { // DOR_IO
        OUT(0, 1); // DOR_IO = 1 → encender
        sleep(50); // esperar velocidad
    }

    // Si el motor aún no se apagó del todo, esperar
    int t = 0;
    while (IN(0) != 0 && t < 200) { // DOR_STATUS sigue en 1
        sleep(10);
        t += 10;
    }
    
    // para el brazo
    OUT(1, pista); // Indica al brazo que debe ir a la pista indicada.
    while (IN(1) == 0); // Esperar que ARM_STATUS sea 1

    OUT(2, sector_rel); // SEEK_SECTOR
    while (IN(2) == 0); // Esperar que DATA_READY sea 1

    escribir_datos(data);

    OUT(0, 0); // DOR_IO = 0
}