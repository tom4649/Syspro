#!/bin/bash
# Test code for syspro2018 kadai2
# Written by Shinichi Awamoto and Daichi Morita

dir=$(mktemp XXXX -d --tmpdir=/tmp)

echo "#############################################"
echo "kadai2: Running tests. (v0.0.1)"

# 課題Aのテスト
if [ -d kadai-a ]; then
    cp -r kadai-a $dir
    pushd $dir/kadai-a > /dev/null 2>&1

    if [ ! -f Makefile ]; then
    	echo "kadai-a: Missing Makefile."
    fi

    make measure > /dev/null 2>&1

    if [ ! -f measure ]; then
	    echo "kadai-a: Failed to generate the binary(measure) with '$ make measure'"
    fi

    make clean > /dev/null 2>&1

    if [ -f measure ]; then
	    echo "kadai-a: Failed to remove the binary(measure) with '$ make clean'."
    fi

    if [ ! -z "`find . -name \*.o`" ]; then
	    echo "kadai-a: Failed to remove object files(*.o) with '$ make clean'."
    fi

    if [ `grep '\-Wall' Makefile | wc -l` -eq 0 ]; then
	    echo "kadai-a: Missing '-Wall' option."
    fi

    if [ ! -f report-a.txt ]; then
	    echo "kadai-a: Missing report-a.txt"
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-a: No 'kadai-a' directory!"
fi

# 課題Bのテスト
if [ -d kadai-b ]; then
    cp -r kadai-b $dir
    pushd $dir/kadai-b > /dev/null 2>&1

    if [ ! -f Makefile ]; then
	    echo "kadai-b: Missing Makefile."
    fi

    make mycp > /dev/null 2>&1

    if [ ! -f mycp ]; then
	    echo "kadai-b: Failed to generate the binary(mycp) with '$ make mycp'"
    fi

    # mycpのテスト
    man open > tmp.txt
    ./mycp tmp.txt myresult.txt
    cp tmp.txt result.txt

    if [ ! -z "`diff myresult.txt result.txt`" ]; then
        echo "kadai-b: diff detected between the result of cp and mycp"
    fi

    make clean > /dev/null 2>&1

    if [ -f mycp ]; then
	    echo "kadai-b: Failed to remove the binary(mycp) with '$ make clean'."
    fi

    if [ ! -z "`find . -name \*.o`" ]; then
	    echo "kadai-b: Failed to remove object files(*.o) with '$ make clean'."
    fi

    if [ `grep '\-Wall' Makefile | wc -l` -eq 0 ]; then
	    echo "kadai-b: Missing '-Wall' option."
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-b: No 'kadai-b' directory!"
fi

# 課題Cのテスト
if [ -d kadai-c ]; then
    cp -r kadai-c $dir
    pushd $dir/kadai-c > /dev/null 2>&1

    if [ ! -f Makefile ]; then
	    echo "kadai-c: Missing Makefile."
    fi

    make measure1 > /dev/null 2>&1

    if [ ! -f measure1 ]; then
	    echo "kadai-c: Failed to generate the binary(measure1) with '$ make measure1'"
    fi

    make measure2 > /dev/null 2>&1

    if [ ! -f measure2 ]; then
	    echo "kadai-c: Failed to generate the binary(measure2) with '$ make measure2'"
    fi

    make clean > /dev/null 2>&1

    if [ -f measure1 ]; then
	    echo "kadai-c: Failed to remove the binary(measure1) with '$ make clean'."
    fi

    if [ -f measure2 ]; then
	    echo "kadai-c: Failed to remove the binary(measure2) with '$ make clean'."
    fi

    if [ ! -z "`find . -name \*.o`" ]; then
	    echo "kadai-c: Failed to remove object files(*.o) with '$ make clean'."
    fi

    if [ `grep '\-Wall' Makefile | wc -l` -eq 0 ]; then
	    echo "kadai-c: Missing '-Wall' option."
    fi

    if [ ! -f report-c.txt ]; then
	    echo "kadai-c: Missing report-c.txt"
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-c: No 'kadai-c' directory!"
fi

# 課題Dのテスト
if [ -d kadai-d ]; then
    cp -r kadai-d $dir
    pushd $dir/kadai-d > /dev/null 2>&1

    if [ ! -f Makefile ]; then
	    echo "kadai-d: Missing Makefile."
    fi

    make mywc > /dev/null 2>&1

    if [ ! -f mywc ]; then
	    echo "kadai-d: Failed to generate the binary(mywc) with '$ make mywc'"
    fi

    # mywcのテスト
    man open > tmp.txt
    ./mywc tmp.txt > myresult.txt
    wc tmp.txt > result.txt

    sed 's/^[ \t]*//' myresult.txt | sed -e 's/\s\+/ /g' > myresult1.txt
    sed 's/^[ \t]*//' result.txt | sed -e 's/\s\+/ /g' > result1.txt

    if [ ! -z "`diff myresult1.txt result1.txt`" ]; then
        echo "kadai-d: diff detected between the result of wc and mywc"
    fi

    make clean > /dev/null 2>&1

    if [ -f mywc ]; then
	    echo "kadai-d: Failed to remove the binary(mywc) with '$ make clean'."
    fi

    if [ ! -z "`find . -name \*.o`" ]; then
	    echo "kadai-d: Failed to remove object files(*.o) with '$ make clean'."
    fi

    if [ `grep '\-Wall' Makefile | wc -l` -eq 0 ]; then
	    echo "kadai-d: Missing '-Wall' option."
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-d: No 'kadai-d' directory!"
fi

echo "kadai2: All tests have finished!"
echo "#############################################"

rm -rf $dir
