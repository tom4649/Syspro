#!/bin/bash
# Test code for syspro2018 kadai3
# Written by Shinichi Awamoto and Daichi Morita

kadai_signature="kadai03"
version=v0.0.1

dir=$(mktemp XXXX -d --tmpdir=/tmp)
trap "rm -rf ${dir}" 0

# 隱ｲ鬘窟縺ｮ繝�せ繝�
kadai-a() {
    binary_name=unsafe_btree

    if ! file_exists btree.c; then
	show_error "${FUNCNAME[0]}" "Missing 'btree.c'."
    fi

    if ! file_exists main.c; then
	show_error "${FUNCNAME[0]}" "Missing 'main.c'."
    fi

    check_makefile ${binary_name}

    if [ -f ${binary_name} ] && [ `./${binary_name} | wc -l` -ge 10000 ]; then
	show_error "${FUNCNAME[0]}" "Nodes were not lost."
    fi

    unset binary_name
}

# 隱ｲ鬘沓縺ｮ繝�せ繝�
kadai-b() {
    binary_name=safe_btree

    if ! file_exists btree.c; then
	show_error "${FUNCNAME[0]}" "Missing 'btree.c'."
    fi

    if ! file_exists main.c; then
	show_error "${FUNCNAME[0]}" "Missing 'main.c'."
    fi

    check_makefile ${binary_name}

    if [ -f ${binary_name} ] && [ `./${binary_name} | wc -l` -ne 10000 ]; then
	show_error "${FUNCNAME[0]}" "Nodes were lost."
    fi

    unset binary_name
}

# 隱ｲ鬘靴縺ｮ繝�せ繝�
kadai-c() {
    binary_name=bb

    if ! file_exists bb.c; then
	show_error "${FUNCNAME[1]}" "Missing 'bb.c'."
    fi

    if ! file_exists main.c; then
	show_error "${FUNCNAME[1]}" "Missing 'main.c'."
    fi

    check_makefile ${binary_name}

    if [ -f ${binary_name} ] && [ `./${binary_name} | wc -l` -ne 10000 ]; then
	show_error "${FUNCNAME[0]}" "Nodes were lost."
    fi

    unset binary_name
}

# 隱ｲ鬘轡縺ｮ繝�せ繝�
kadai-d() {
    :
}

# 隨ｬ�大ｼ墓焚�壹ヵ繧｡繧､繝ｫ蜷�
file_exists() {
    [ -n "`find . -name $1`" ]
    return $?
}

# 隨ｬ�大ｼ墓焚�壹ヵ繧｡繧､繝ｫ蜷�
# 隨ｬ�貞ｼ墓焚�壽枚蟄怜�
file_contains_string() {
    if file_exists $1 ; then
	[ -n "`grep $2 $1`" ]
	return $?
    else
	return 1
    fi
}

# 隨ｬ�大ｼ墓焚�壹ヰ繧､繝翫Μ繝輔ぃ繧､繝ｫ蜷�
check_makefile() {
    if ! file_exists Makefile; then
	show_error "${FUNCNAME[1]}" "Missing Makefile."
    fi

    if ! file_contains_string Makefile "\-Wall" ; then
	show_error "${FUNCNAME[1]}" "Missing -Wall option."
    fi

    if file_exists $1; then
	show_error "${FUNCNAME[1]}" "Do not include the binary($1) in your submissions!"
    fi

    if file_exists *.o; then
	show_error "${FUNCNAME[1]}" "Do not include object files(*.o) in your submissions!"
    fi

    make $1 > /dev/null 2>&1

    if ! file_exists $1; then
	show_error "${FUNCNAME[1]}" "Failed to generate the binary($1) with '$ make $1'"
    fi
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

# 繝�ぅ繝ｬ繧ｯ繝医Μ縺ｮ蟄伜惠遒ｺ隱阪→繝√ぉ繝�け髢｢謨ｰ蜻ｼ縺ｳ蜃ｺ縺�
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

# check_kadai縺ｨ縺ｯ逡ｰ縺ｪ繧翫√お繝ｩ繝ｼ繝｡繝�そ繝ｼ繧ｸ繧定｡ｨ遉ｺ縺励↑縺�
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

# 隨ｬ荳蠑墓焚�夊ｪｲ鬘悟錐
# 隨ｬ莠悟ｼ墓焚�壹Γ繝�そ繝ｼ繧ｸ
show_error() {
    echo -e "$1:\e[1;31m error: \e[m$2"
}

# 隨ｬ荳蠑墓焚�夊ｪｲ鬘悟錐
# 隨ｬ莠悟ｼ墓焚�壹Γ繝�そ繝ｼ繧ｸ
show_info() {
    echo -e "$1:\e[1;34m  info: \e[m$2"
}

echo "#############################################"
show_info "${kadai_signature}" "Running tests. (${version})"

check_kadai kadai-a
check_kadai kadai-b
check_kadai kadai-c
check_optional_kadai kadai-d

show_info "${kadai_signature}" "All tests finished!"
echo "#############################################"

rm -rf ${dir}
