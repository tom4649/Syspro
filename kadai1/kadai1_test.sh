#!/bin/bash
# Test code for syspro2022 kadai1
# Written by Shinichi Awamoto

dir=$(mktemp XXXX -d --tmpdir=/tmp)

pushd $dir > /dev/null 2>&1
echo "kadai1: Downloading kadai1.tar.gz..."
wget http://www.pf.is.s.u-tokyo.ac.jp/syspro/static/kadai1.tar.gz
tar zxvf kadai1.tar.gz > /dev/null 2>&1
popd > /dev/null 2>&1

echo "#############################################"
echo "kadai1: Running tests. (v0.0.4)"

if [ -d kadai-a ]; then
    cp -r kadai-a $dir
    pushd $dir/kadai-a > /dev/null 2>&1

    # 課題Aのテスト
    if [ ! -f Makefile ]; then
	      echo "kadai-a: missing Makefile."
    fi

    if [ -f kadai-a ]; then
	      echo "kadai-a: Do not include the binary(kadai-a) in your submissions!"
    fi

    make kadai-a > /dev/null 2>&1

    if [ ! -f kadai-a ]; then
	      echo "kadai-a: Failed to generate the binary(kadai-a) with '$ make kadai-a'"
    fi

    make clean > /dev/null 2>&1

    if [ -f kadai-a ]; then
	      echo "kadai-a: Failed to remove the binary(kadai-a) with '$ make clean'."
    fi

    if [ ! -z "`find . -name \*.o`" ]; then
	      echo "kadai-a: Failed to remove object files(*.o) with '$ make clean'."
    fi

    if [ `grep '\-Wall' Makefile | wc -l` -eq 0 ]; then
	      echo "kadai-a: Missing '-Wall' option."
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-a: No 'kadai-a' directory!"
fi

if [ -d kadai-b ]; then
    cp -r kadai-b $dir
    pushd $dir/kadai-b > /dev/null 2>&1

    # 課題Bのテスト
    export LC_ALL=C
    tar xf ../coreutils-8.9.tar.gz

    if [ ! -f sort.sh ]; then
	      echo "kadai-b: Missing 'sort.sh'."
    fi

    ./sort.sh > /dev/null 2>&1

    if [ ! -f result.txt ]; then
	      echo "kadai-b: Failed to generate 'result.txt' with '$ ./sort.sh'."
    else
	      if [ `cat result.txt | wc -l` -eq 0 ]; then
	          echo "kadai-b: 'result.txt' is empty!"
	      fi
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-b: No 'kadai-b' directory!"
fi

if [ -d kadai-c ]; then
    cp -r kadai-c $dir
    pushd $dir/kadai-c > /dev/null 2>&1

    # 課題Cのテスト

    tmpfile=$(mktemp XXXX)
    wget http://www.pf.is.s.u-tokyo.ac.jp/syspro/static/1.pdf -O $tmpfile > /dev/null 2>&1

    if [ ! -f concatenation.sh ]; then
	      echo "kadai-c: Missing 'concatenation.sh'."
    fi

    ./concatenation.sh > /dev/null 2>&1

    if [ ! -f 1.pdf ]; then
	      echo "kadai-c: Failed to generate '1.pdf' with '$ ./concatenation.sh'."
    else
	      if [ ! -z "`diff $tmpfile 1.pdf`" ]; then
	          echo "kadai-c: Is your 1.pdf valid? (detect diff)"
	      fi
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-c: No 'kadai-c' directory!"
fi

if [ -d kadai-d ]; then
    cp -r kadai-d $dir
    pushd $dir/kadai-d > /dev/null 2>&1

    # 課題Dのテスト
    if [ ! -f convert.sh ]; then
	      echo "kadai-d: Missing 'convert.sh'."
    fi

    ./convert.sh > /dev/null 2>&1

    if [ ! -z "`find . -name \*.cpp`" ]; then
	      echo "kadai-d: Did you convert *.cpp to *.cc?"
    else
	      if [ `grep 'NEET the 3rd' *.cc | wc -l` -ne 0 ]; then
	          echo "kadai-d: Did you replace 'NEET the 3rd' to your name?"
	      fi
	      if [ `grep 'neet3@example.com' *.cc | wc -l` -ne 0 ]; then
	          echo "kadai-d: Did you replace 'neet3@example.com' to your email address?"
	      fi
	      if [ `grep " $" *.cc | wc -l` -ne 0 ]; then
	          echo "kadai-d: Did you remove all trailing whitespaces?"
	      fi
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-d: No 'kadai-d' directory!"
fi

if [ -d kadai-e ]; then
    cp -r kadai-e $dir
    pushd $dir/kadai-e > /dev/null 2>&1

    # 課題Eのテスト
    if [ ! -f output.sh ]; then
	      echo "kadai-e: Missing 'output.sh'."
    fi

    ./output.sh > /dev/null 2>&1

    if [ ! -f cat.txt ]; then
	      echo "kadai-e: Failed to generate 'cat.txt' with '$ ./output.sh'."
    fi

    if [ ! -f strace.txt ]; then
	      echo "kadai-e: Failed to generate 'strace.txt' with '$ ./output.sh'."
    fi

    if [ ! -f all.txt ]; then
	      echo "kadai-e: Failed to generate 'all.txt' with '$ ./output.sh'."
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-e: No 'kadai-e' directory!"
fi

if [ -d kadai-f ]; then
    cp -r kadai-f $dir
    pushd $dir/kadai-f > /dev/null 2>&1

    # 課題Fのテスト
    if [ ! -f report.txt ]; then
	      echo "kadai-f: Missing report.txt."
    fi

    popd > /dev/null 2>&1
else
    echo "kadai-f: No 'kadai-f' directory!"
fi

echo "kadai1: All tests have finished!"
echo "#############################################"

rm -rf $dir
