#!/bin/bash
#set -o errexit
#set -o nounset
#set -o xtrace

. $1

main() {
		_init

		spawn "/bin/sleep 10 &"
		spawn "/bin/sleep 10 &"
		spawn "/bin/sleep 10 &"
		spawn "/usr/bin/touch ____ALIVE.txt &"
		sleep 0.01
		spawn "/bin/true"
		[[ $(fg_job) =~ sleep ]] && _exit 1
		[[ ! $(fg_job) =~ ish ]] && _exit 2
		_exit 0
}

main "$@"

