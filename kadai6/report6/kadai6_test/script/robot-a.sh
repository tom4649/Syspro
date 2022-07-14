#!/bin/bash
#set -o errexit
#set -o nounset
#set -o xtrace


main() {
		for i in `seq 1 9`
		do
				ctrl_c
				[[ ! `ps -a` =~ count ]] && _exit $i
		done
		ctrl_c
		[[ `ps -a` =~ count ]] && _exit 10
		_exit 0
}

ctrl_c() {
echo -e "\x03"
}

_exit() {
	echo "errno=$1" >> ____RESULT.sh	
	killall -9 count > /dev/null 2>&1
	exit 0
}

main "$@"

