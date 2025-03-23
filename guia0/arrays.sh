#!/bin/bash

mi_array=(1 2 "tres" "cuatro" 5) # creo array con elementos
declare -a array_vacio # creo array vacío
mi_array+=("six") # agrego elemento
mi_array+=(7) # agrego elemento
echo ${mi_array[0]} # imprimo el primer elemento
echo ${mi_array[2]} # imprimo el tercer elemento
echo ${mi_array[@]} # imprimo todos los elementos
