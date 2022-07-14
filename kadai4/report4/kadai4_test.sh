#!/bin/bash
# Test code for syspro2018 kadai4
# Written by Shinichi Awamoto and Daichi Morita
# set -o xtrace

# 修正ログ
# v0.0.2
#  check_report の第二引数にreport_requiredを指定することで
#  レポートがない場合にerrorとするよう変更。
#  それに伴い、check_kadai,check_optional_kadaiも変更し、
#  課題C,Dでレポートを必須とした。 
#  また、README.txtを含めるよう修正した。 
#  kadai-cで15秒以上経過したら、トラップするよう修正した。
#  by 2019年度TA 五反田
# v0.0.3
#  - rename show_warn to show_note
#  - add shoe_warn
#  - 課題Cの実行が長すぎる際のトラップがうまく動作していなかったので削除
#  - clientからのconnectが早すぎて失敗することがあるので、sleepを追加
#  by 2019年度TA 五反田
# v0.0.4
#  - README.txtのチェックを削除
# v0.0.5
#  - fix kadai-a check


kadai_signature="kadai04"
version=v0.0.5

dir=$(mktemp XXXX -d --tmpdir=/tmp)
dump_file=$(realpath ./test_dump)
trap "rm -rf ${dir}" 0

# 課題Aのテスト
kadai-a() {
    local client=udpechoclient
    local server=udpechoserver

    check_makefile $client $server 
    [ -f $client ] && [ -f $server ] || return 1

    local port=$(($RANDOM % 100 + 25555))
    
    man 2 open > __before.txt
    ./${server} $port > /dev/null 2>&1 &
    sleep 0.2
    ./${client} 127.0.0.1 $port < __before.txt > __after.txt &

    sleep 1

    disown -a
    pkill -KIll $client
    pkill -Kill $server

    if [ ! -z "`diff __before.txt __after.txt`" ]; then
        show_error kadai-a "diff detected between clientinput and output"
    fi
}

# 課題Bのテスト
kadai-b() {
    local client=tcpechoclient
    local server1=tcpechoserver1
    local server2=tcpechoserver2

    check_makefile $client $server1 $server2
    [ -f $client ] && [ -f $server1 ] && [ -f $server2 ] || return 1

    if ! check_tcp $client $server1; then
        show_error kadai-b "tcpechoserver1: diff detected between client input and output"        
    fi

    if ! check_tcp $client $server2; then
        show_error kadai-b "tcpechoserver2: diff detected between client input and output"        
    fi
}

# 第1引数 tcpechoclientの実行ファイル名
# 第2引数 tcpechoserverの実行ファイル名
check_tcp() {
		local port=$(($RANDOM % 100 + 25555))
    local before=__before
    local after1=__after1
    local after2=__after2

		yes "some text" | head -n 10000 > $before	
        
    ./$2 $port > /dev/null 2>&1 &
    sleep 0.2
    ./$1 127.0.0.1 $port < $before > $after1 &
    ./$1 127.0.0.1 $port < $before > $after2 &

    sleep 2

    disown -a
    pkill -Kill $1
    pkill -Kill $2

    diff $before $after1 > /dev/null 2>&1 && diff $before $after2 > /dev/null 2>&1
    local RET=$?
    rm -f $before $after1 $after2 
    return $RET
}

# 課題Cのテスト
kadai-c() {
    local client=iperfc
    local server=iperfs

    check_makefile $client $server
    [ -f $client ] && [ -f $server ] || return 1

		local port=$(($RANDOM % 100 + 25555))
    ./$server $port > /dev/null 2>&1 &
    sleep 0.2

    iperfc_result=$(./$client 127.0.0.1 $port)
    if [ $(echo $iperfc_result |wc -l) -ne 1 ]; then
    	show_error "${FUNCNAME[0]}" "the output must be one line and the format is: (data size) (elapsed time) (throughput)"
    else
      show_info kadai-c "iprefc result: $iperfc_result"
    fi

    disown -a
    pkill -Kill $client
    pkill -Kill $server
}

# 課題Dのテスト
kadai-d() {
    :
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
	[ -n "`grep $2 $1`" ]
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
    if [[ ${2:-1} = 'report_required' ]];then
      show_error "$1" "Missing report file or the name of report is wrong."
      show_info "$1" "report should be 'report.txt' or 'report.md'"
    else
      show_info "$1" "report file not found. (but not required)"
    fi
  fi
}

# ディレクトリの存在確認とチェック関数呼び出し
check_kadai() {
    if [ -d $1 ]; then
	cp -r $1 ${dir}
	pushd ${dir}/$1 > /dev/null 2>&1

	check_report $1 ${2:-}
	
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

	check_report $1 ${2:-"report_required"}

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
    echo    "$1: error: $2" >> $dump_file
}

# 第一引数：課題名
# 第二引数：メッセージ
show_info() {
    echo -e "$1:\e[1;34m  info: \e[m$2"
    echo    "$1: info: $2" >> $dump_file
}

show_warn() {
    echo -e "$1:\e[1;33m  info: \e[m$2"
    echo    "$1: warn: $2" >> $dump_file
}

show_note() {
    echo -e "$1:  \e[1;44mnote: $2\e[m"
    echo    "$1: note: $2" >> $dump_file
}

echo "#############################################"
echo "#############################################" > $dump_file
show_info "${kadai_signature}" "Running tests. (${version})"

check_kadai kadai-a
check_kadai kadai-b
check_kadai kadai-c report_required
check_optional_kadai kadai-d

show_info "${kadai_signature}" "All tests finished!"
show_note "${kadai_signature}" "This scripts dump the result into $dump_file"
show_note "${kadai_signature}" "Please don't remove and include this file in submission"
echo "#############################################"
echo "#############################################" >> $dump_file

rm -rf ${dir}
