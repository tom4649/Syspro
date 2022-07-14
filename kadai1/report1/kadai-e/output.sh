#!/bin/bash

((strace //bin/cat /proc/meminfo) 1> cat.txt) 2> /dev/null
((strace //bin/cat /proc/meminfo) 2> strace.txt) 1> /dev/null
(strace //bin/cat /proc/meminfo) 1> all.txt 2>&1
