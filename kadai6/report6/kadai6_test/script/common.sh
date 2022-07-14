#!/bin/bash
gettime='/bin/date +%s%N'

search_zombie() {
ps ax | grep $1 | grep -q '<defunct>'
}

_init() {
spawn "/usr/bin/tty > ____TTY.txt"
sync
sleep 0.01
}

_exit() {
record "errno=$1"
if [[ "$1" -eq 0 ]]; then
check_zombie && record "zombie=1"
fi
spawn "exit"
sleep 2
killall -9 ish > /dev/null 2>&1
exit 0
}

record() {
# echo "$@" | tee -a ____RESULT.sh >&2
echo "$@" >> ____RESULT.sh
}

check_zombie() {
local tty=$(tr -d '\n' < ____TTY.txt)
[[ $(ps -t $tty | grep '<defunct>' | wc -l) -ne 0 ]]
}

_ps() {
local tty=$(tr -d '\n' < ____TTY.txt)
ps -t $tty -o pgid,tpgid,cmd | tr -s ' ' | \
  sed -e '1d' -e "s/^[ ]\+//g" | cut -d' ' -f1-3
}

fg_job() {
_ps | while read pgid tpgid cmd; do
[[ $pgid -eq $tpgid ]] && echo -n $cmd
done
}

fg_job_includes() {
_ps | while read pgid tpgid cmd; do
[[ $pgid -eq $tpgid ]] && echo -n $cmd
done | grep -q "$1"
}

bg_job() {
_ps | while read pgid tpgid cmd; do
		echo -n $cmd
done
}
spawn() {
echo $@
}

ctrl_c() {
echo -e "\x03"
}

ctrl_z() {
echo -e "\x1a"
}

timer_start() {
spawn "$gettime > ____T1.txt"
}

timer_end() {
spawn "$gettime > ____T2.txt"
}

timecheck() {
sync
[[ -r ____T1.txt && -r ____T2.txt ]] && _timecheck ____T1.txt ____T2.txt <(echo $1)
}

_timecheck() {
local s=$(tr -d '\n' < $1)
local e=$(tr -d '\n' < $2)
local ok=$(tr -d '\n' < $3)

[[ -z $s || -z $e || -z $ok ]] && return 1
# milliseconds (input: nanoseconds)
s=$(($s / 1000000))
e=$(($e / 1000000))
# milliseconds (input: seconds)
ok=$(($ok * 1000))
local delta=$((e - s - ok))
local eps=300
[[ ${delta#-} -lt $eps ]]
}

