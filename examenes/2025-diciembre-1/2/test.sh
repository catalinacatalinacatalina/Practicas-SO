#!/bin/bash

# Compilar control.c
gcc -o control control.c -lrt

# Compilar truck.c
gcc -o truck truck.c -lrt

# Inicializar el sistema con 2 muelles
./control 2

./truck 101 50 &
./truck 102 100 &
./truck 103 50 &
./truck 104 50 &
./truck 105 50 &
./truck 106 50 &
./truck 107 50 &
./truck 108 50 &
./truck 109 50 &
./truck 110 50 &


# Esperar a que terminen todos
wait