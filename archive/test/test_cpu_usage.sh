#!/bin/bash
PID=$1
if [ -z "$PID" ]; then
    echo Usage: $0 PID
    exit 1
fi

PROCESS_STAT=($(sed -E 's/\([^)]+\)/X/' "/proc/$PID/stat"))
PROCESS_UTIME=${PROCESS_STAT[13]}
PROCESS_STIME=${PROCESS_STAT[14]}
PROCESS_STARTTIME=${PROCESS_STAT[21]}
SYSTEM_UPTIME_SEC=$(tr . ' ' </proc/uptime | awk '{print $1}')

CLK_TCK=$(getconf CLK_TCK)

let PROCESS_UTIME_SEC="$PROCESS_UTIME / $CLK_TCK"
let PROCESS_STIME_SEC="$PROCESS_STIME / $CLK_TCK"
let PROCESS_STARTTIME_SEC="$PROCESS_STARTTIME / $CLK_TCK"

let PROCESS_ELAPSED_SEC="$SYSTEM_UPTIME_SEC - $PROCESS_STARTTIME_SEC"
let PROCESS_USAGE_SEC="$PROCESS_UTIME_SEC + $PROCESS_STIME_SEC"
let PROCESS_USAGE="$PROCESS_USAGE_SEC * 100 / $PROCESS_ELAPSED_SEC"

echo The PID $PID has spent ${PROCESS_UTIME_SEC}s aka ${PROCESS_UTIME}clock ticks in user mode and ${PROCESS_STIME_SEC}s aka ${PROCESS_STIME}clock ticks in kernel mode. 
echo Total CPU usage is ${PROCESS_USAGE_SEC}s. The process has been running for ${PROCESS_ELAPSED_SEC}s. So the process has used ${PROCESS_USAGE}% of CPU.