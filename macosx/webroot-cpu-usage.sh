#!/bin/bash
#
# Monitor cpu usage of Webroot daemon.
# When too high for too long, kill the daemon.
#
if ! [ $(id -u) = 0 ]; then
    # This script needs to be run as root.
    exec sudo "$0"
    exit 0
fi

count=0
echo "Monitoring Webroot."

while sleep 2
do
    #
    # Get the process ID of the Webroot daemon.
    #
    pid=$(pgrep WSDaemon)
    if [ "$pid" -le 0 ]; then
        echo "No Webroot daemon present."
        exit
    fi
    #echo "Pid: $pid"

    #
    # Get the CPU usage of the Webroot process.
    #
    usage=($(ps -o %cpu -p $pid))
    usage=$(awk "BEGIN{print int(${usage[1]})}")

    if [ "$usage" -gt 0 ]; then
        date=$(date +'%T')
        echo "$date, pid $pid, cpu $usage%"
    fi

    if [ "$usage" -lt 95 ]; then
        # Reset the counter.
        count=0
        continue
    fi

    ((count += 1))
    #echo "Count: $count"

    if [ $count -gt 10 ]; then
        echo "Killing Webroot."
        kill -KILL $pid
        count=0
    fi
done
