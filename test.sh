#!/bin/bash

./my_tr < cities_ES.tsv > cities_ES1.csv
cmp cities_ES1.csv cities_ES1.csv 
if [ $? -eq 0 ]; then
    echo "Substitució: Passat."
else
    echo "Substitució: Falla."
    exit 1
fi

./my_tr -d < cities_ES.csv > cities_ES1.csv
if [ $(grep " " cities_ES1.csv | wc -l) -ne 0 ]; then
    echo "Eliminació: Falla."
    exit 1
else
    echo "Eliminació: Passat."
fi

./my_tr -s < cities_ES.csv > cities_ES1.csv
tr -s "[:print:]" < cities_ES.csv > cities_ES2.csv
cmp cities_ES1.csv cities_ES2.csv
if [ $? -eq 0 ]; then
    echo "Squeezing: Passat."
else
    echo "Squeezing: Falla."
    exit 1
fi  
rm cities_ES1.csv cities_ES2.csv
