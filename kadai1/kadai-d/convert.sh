#!/bin/bash


for i in `find ./ -name \*.cpp`
do
    sed -i 's/\s*$//' $i
    sed -i -e "s/NEET the 3rd/Akiyoshi Tomihari/g" $i
    sed -i -e "s/neet3@example.com/tkaaynakiyoshi@gmail.com/g" $i
    mv $i ${i/.cpp/.cc}
done