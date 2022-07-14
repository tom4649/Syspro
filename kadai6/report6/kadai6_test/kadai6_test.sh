#!/bin/bash
# Test code for syspro2018 kadai6
# Written by Shinichi Awamoto and Daichi Morita
# set -o xtrace

# v0.0.2 (Masashi Ikarashi)
#  logging test script output into file 'test6_dump'

kadai_signature="kadai06"
version=v0.0.2

dump_file=$(realpath ./test6_dump)
if [ -z $OUTPUT_LOGGED ]; then
    OUTPUT_LOGGED=1 bash $0 | tee $dump_file
    exit
fi

dir=$(mktemp XXXX -d --tmpdir=/tmp)
trap "rm -rf ${dir}" 0

testdir=kadai6_test
termdir=${testdir}/pseudoterm
scriptdir=${testdir}/script

# 課題Aのテスト
kadai-a() {
    local bin=count

    check_makefile $bin
    [ -f $bin ] || return 1

    local errno=0
    check_ish a
    if [[ $errno -eq 10 ]]; then
	show_error "${FUNCNAME[0]}" "count is not exit after 10 ctrl-c"
    elif [[ $errno -ne 0 ]]; then
	show_error "${FUNCNAME[0]}" "count is exit by $errno ctrl-c"
    fi
}

# 課題B,C,Dのテスト
kadai-bcde() {
    local bin=ish

    check_makefile $bin
    [ -f $bin ] || return 1

    kadai-b
    kadai-c
    kadai-d
}

# 課題Bのテスト
kadai-b() {
    local errno=0
    check_ish b
    if [[ $errno -eq 1 ]]; then
	show_error "${FUNCNAME[0]}" "ish is not foreground process after /bin/ls"
    elif [[ $errno -eq 2 ]]; then
	show_error "${FUNCNAME[0]}" "ish is killed by ctrl+c(SIGINT)"
    elif [[ $errno -eq 3 ]]; then
	show_error "${FUNCNAME[0]}" "sleep is not run as a foreground process"
    elif [[ $errno -eq 4 ]]; then
	show_error "${FUNCNAME[0]}" "sleep survived ctrl+c(SIGINT)"
    elif [[ $errno -eq 5 ]]; then
	show_error "${FUNCNAME[0]}" "ish is killed by ctrl+c(SIGINT) while sleep"
    fi
}


# 課題Cのテスト
kadai-c() {
    local errno=0
    check_ish c
    if [[ $errno -eq 1 ]]; then
	show_error "${FUNCNAME[0]}" "/bin/sleep 10 & is not executed as a background process"
    elif [[ $errno -eq 2 ]]; then
	show_error "${FUNCNAME[0]}" "ish is not foreground process after /bin/sleep 10 &"
    fi
}

# 課題Dのテスト
kadai-d() {
    local errno=0
    check_ish d
    if [[ $errno -eq 1 ]]; then
	show_error "${FUNCNAME[0]}" "ish is stopped by ctrl-z (SIGTSTP)"
    elif [[ $errno -eq 2 ]]; then
	show_error "${FUNCNAME[0]}" "ish is not foreground process after suspending /bin/sleep 10"
    elif [[ $errno -eq 3 ]]; then
	show_error "${FUNCNAME[0]}" "sleep is killed by ctrl-z (SIGTSTP)"
    elif [[ $errno -eq 4 ]]; then
	show_error "${FUNCNAME[0]}" "ish is not foreground process after bg"
    elif [[ $errno -eq 5 ]]; then
	show_error "${FUNCNAME[0]}" "sleep is not done after bg"
    fi
}

# 第1引数 入力スクリプトのid
check_ish() {
    local common=${dir}/${scriptdir}/common.sh
    local robot=${dir}/${scriptdir}/robot-$1.sh

    ${dir}/${termdir}/pseudoterm `pwd`/$bin $robot $common
    local RET=$?

    local zombie=0
    [[ -r ____RESULT.sh ]] && . ____RESULT.sh

    if [[ $RET -ne 0 ]]; then
	show_error "${FUNCNAME[1]}" "ish done with exit status=$RET"
    fi

    [[ $zombie -ne 0 ]] && show_error "${FUNCNAME[1]}" "there is a zombie process"

    rm -f ____*
}

build_term() {
    cp -r $(dirname $(realpath $0)) ${dir}
    pushd ${dir}/${termdir}/ > /dev/null 2>&1
    make pseudoterm > /dev/null 2>&1
    popd > /dev/null 2>&1
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

build_term
check_kadai kadai-a
check_kadai kadai-bcde

show_info "${kadai_signature}" "All tests finished!"
echo "#############################################"

rm -rf ${dir}
