#!/bin/bash

for i in `seq -w 0 99`
do
    wget https://www.pf.is.s.u-tokyo.ac.jp/syspro/static/kadai1/1.pdf.$i
    cat ./1.pdf.$i >> 1.pdf
    rm ./1.pdf.$i
done