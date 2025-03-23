#!/bin/bash

#mostrar pregunta
echo "el año de boca se termino el febrero?"

# leer resultado

read respuesta

#evaluar

if [ "$respuesta" = "si" ]; then
	echo "preterporada y afuera"
elif [ "$respuesta" = "no" ]; then
	echo "repasemos"
else 
	echo "hay que contestar si o no papa"
fi 


