#!/bin/bash
# Script pour compiler tous les fichiers .c dans l'environnement WSL
cd /mnt/c/Users/riadb/Downloads/tpreseau
for f in *.c; do
    echo "Compiling $f..."
    gcc "$f" -o "${f%.c}.out"
done
echo "Compilation terminee."
