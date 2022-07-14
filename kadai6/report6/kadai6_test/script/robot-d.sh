#!/bin/bash
#set -o errexit
#set -o nounset
#set -o xtrace

. $1

main() {
		_init
		local tty=$(tr -d '\n' < ____TTY.txt)

		ctrl_z
		[[ ! $(fg_job) =~ ish ]] && _exit 1

		spawn "/bin/sleep 3"
		sleep 1
		ctrl_z
		sleep 0.5
		[[ ! $(fg_job) =~ ish ]] && _exit 2
		[[ ! $(bg_job) =~ sleep ]] && _exit 3

		spawn "bg"
		[[ ! $(fg_job) =~ ish ]] && _exit 4
		sleep 2
		spawn "/bin/true"
		[[ $(bg_job) =~ sleep ]] && _exit 5
		_exit 0
}

main "$@"

