#!/bin/bash
# Test code for syspro2018 kadai5
# Written by Shinichi Awamoto and Daichi Morita
# set -o xtrace

# v0.0.2 (Masashi Ikarashi)
#  logging test script output into file 'test5_dump'

dump_file=$(realpath ./test5_dump)
if [ -z $OUTPUT_LOGGED ]; then
    OUTPUT_LOGGED=1 bash $0 | tee $dump_file
    exit
fi

kadai_signature="kadai05"
version=v0.0.2

dir=$(mktemp XXXX -d --tmpdir=/tmp)
trap "rm -rf ${dir}" 0

# 課題Aのテスト
kadai-a() {
local prog=myexec

check_makefile $prog
[ -f $prog ] || return 1


if [ ! -z "`diff -q <(./$prog /bin/ls -l /var) <(ls -l /var)`" ]; then
    show_error "${FUNCNAME[0]}" "diff detected"
fi

local out=__strace.txt
strace -f ./$prog /bin/false 2> $out
if ! file_contains_string $out "^\[pid \+[0-9]\+\]"; then
    show_error "${FUNCNAME[0]}" "fork is not called"
fi

if ! file_contains_string $out "wait"; then
    show_error "${FUNCNAME[0]}" "wait is not called"
fi

rm -f $out
}

# 課題B,C,Dのテスト
kadai-bcde() {
local shell=ish

check_makefile $shell
[ -f $shell ] || return 1

kadai-b
kadai-c
kadai-d
}

# 課題Bのテスト
kadai-b() {
local out=__output.txt

export OSENSHU=__2018__
script-b | ./$shell > $out
local RET=$?
if [ $RET -ne 0 ]; then
    show_error "${FUNCNAME[0]}" "exit with status $RET"
    return 1        
fi

grep -q "ECHOTEST1" $out || show_error "kadai-b" "command failed: /bin/echo ECHOTEST1"
grep -q "ECHOTEST2" $out || show_error "kadai-b" "command failed: /bin/echo ECHOTEST2"
grep -q "ECHOTEST3" $out || show_error "kadai-b" "command failed: /bin/echo ECHOTEST3"
grep -q "__2018__" $out || show_error "kadai-b" "Failed to read environement variables"

unset OSENSHU
rm -f $out
}

script-b() {
cat << END
/bin/echo ECHOTEST1
/bin/echo ECHOTEST2
/bin/echo ECHOTEST3
/usr/bin/printenv OSENSHU
exit
END
}


# 課題Cのテスト
kadai-c() {
check_ish c1 "Failed to redicrect out"
check_ish c2 "Failed to redicrect in"
check_ish c3 "Failed to pipe"
check_ish c4 "Failed to pipe large file" sleep
}

script-c1() {
cat << END
/usr/bin/wc -l /proc/meminfo > $out
/bin/true
/bin/false
/usr/bin/wc -l /proc/meminfo > $out
exit
END
}

script-c2() {
cat << END
/usr/bin/cut -d: -f1 < /proc/meminfo > $out
/bin/true
/bin/false
/usr/bin/cut -d: -f1 < /proc/meminfo > $out
exit
END
}


script-c3() {
cat << END
/usr/bin/cut -d: -f1 /proc/meminfo | /usr/bin/tr -d '\n' > $out
/bin/true
/bin/false
/usr/bin/cut -d: -f1 /proc/meminfo | /usr/bin/tr -d '\n' > $out
exit
END
}

script-c4() {
cat << END
/usr/bin/find /usr | /bin/cat > $out
exit
END
}

# 課題Dのテスト
kadai-d() {
check_ish d1 "Failed to append"
check_ish d2 "Failed to multiple pipes"   
}

script-d1() {
cat << END
/usr/bin/cut -d: -f1 /proc/meminfo | /usr/bin/tr -d '\n' > $out
/usr/bin/cut -d: -f1 /proc/meminfo | /usr/bin/tr -d '\n' >> $out
/usr/bin/cut -d: -f1 /proc/meminfo | /usr/bin/tr -d '\n' >> $out
/usr/bin/cut -d: -f1 /proc/meminfo | /usr/bin/tr -d '\n' >> $out
exit
END
}

script-d2() {
cat << END
/bin/cat < /proc/meminfo | /bin/cat | /bin/cat | /usr/bin/wc -l > $out
exit
END
}

# 第1引数 入力スクリプトのid
# 第2引数 error時の出力メッセージ
check_ish() {
    local shell=ish
    local script=script-$1
    local out=__output.txt
    local ans=__answer.txt

    $script | sh
    mv $out $ans
    if [ ! "$3" = "" ]; then
	$script | ./$shell > /dev/null 2>&1 &
	sleep 1
	disown -a
	pkill -Kill $shell
    else
	$script | ./$shell > /dev/null 2>&1
    fi

    diff -q $out $ans > /dev/null 2>&1 || show_error "${FUNCNAME[1]}" "$2"
    rm -f $out $ans
}

# 第１引数：ファイル名
file_exists() {
    [ -n "`find . -name $1`" ]
    return $?
}

# 第１引数：ファイル名
# 第２引数：文字列
file_contains_string() {
    if file_exists $1 ; then
	[ -n "`grep "$2" $1`" ]
	return $?
    else
	return 1
    fi
}

# 引数：バイナリファイル名
check_makefile() {
    if ! file_exists Makefile; then
	show_error "${FUNCNAME[1]}" "Missing Makefile."
    fi

    if ! file_contains_string Makefile "\-Wall" ; then
	show_error "${FUNCNAME[1]}" "Missing -Wall option."
    fi

    for file in $@; do
	if file_exists $file; then
	    show_error "${FUNCNAME[1]}" "Do not include the binary($file) in your submissions!"
	fi
    done

    if file_exists *.o; then
	show_error "${FUNCNAME[1]}" "Do not include object files(*.o) in your submissions!"
    fi

    make $@ > /dev/null 2>&1

    for file in $@; do
	if ! file_exists $file; then
	    show_error "${FUNCNAME[1]}" "Failed to generate the binary($file) with '$ make $file'"
	fi
    done
}

check_report() {
    if [ -f report.md ]; then
	show_info "$1" "Found 'report.md'."
    elif [ -f report.txt ]; then
	show_info "$1" "Found 'report.txt'."
    else
	show_info "$1" "No report file."
    fi
}

# ディレクトリの存在確認とチェック関数呼び出し
check_kadai() {
    if [ -d $1 ]; then
	cp -r $1 ${dir}
	pushd ${dir}/$1 > /dev/null 2>&1

	check_report $1

	$1

	popd > /dev/null 2>&1
    else
	show_error "$1" "No '$1' directory!"
    fi
}

# check_kadaiとは異なり、エラーメッセージを表示しない
check_optional_kadai() {
    if [ -d $1 ]; then
	cp -r $1 ${dir}
	pushd ${dir}/$1 > /dev/null 2>&1

	check_report $1

	$1

	popd > /dev/null 2>&1
    else
	show_info "$1" "No '$1' directory!"
    fi
}

# 第一引数：課題名
# 第二引数：メッセージ
show_error() {
    echo -e "$1:\e[1;31m error: \e[m$2"
}

# 第一引数：課題名
# 第二引数：メッセージ
show_info() {
    echo -e "$1:\e[1;34m  info: \e[m$2"
}

echo "#############################################"
show_info "${kadai_signature}" "Running tests. (${version})"

check_kadai kadai-a
check_kadai kadai-bcde

show_info "${kadai_signature}" "All tests finished!"
echo "#############################################"

rm -rf ${dir}
