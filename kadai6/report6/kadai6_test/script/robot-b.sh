#!/bin/bash
#set -o errexit
# set -o nounset
# set -o xtrace

. $1

main() {
		_init
		local tty=$(tr -d '\n' < ____TTY.txt)
		
		spawn "/bin/ls"
		sleep 0.01
		[[ ! $(fg_job) =~ ish ]] && _exit 1
		
		ctrl_c
		spawn "/usr/bin/touch ____ALIVE1.txt"
		sleep 0.01
		[[ ! -r ____ALIVE1.txt ]] && _exit 2
		spawn "/bin/sleep 2"
		sleep 0.01
		[[ ! $(fg_job) =~ sleep ]] && _exit 3
		sleep 2
		
		spawn "/bin/sleep 3"
		sleep 1
		ctrl_c
		spawn "/bin/true"
		spawn "/usr/bin/touch ____ALIVE.txt"
		
		[[ $(fg_job) =~ sleep ]] && _exit 4
		[[ ! $(fg_job) =~ ish ]] && _exit 1
		 
		sleep 0.01
		if [[ -r ____ALIVE.txt ]]; then
				_exit 0
		else
		    _exit 5
		fi
}

main "$@"

