#!/bin/bash

for i in `find ./ -name \*.c |xargs wc -l | sort`
do
    if test -f $i;then
        echo $i >> result.txt
    fi
done

