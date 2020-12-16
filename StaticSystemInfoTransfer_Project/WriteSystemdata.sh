#!/bin/sh

SPATH="./SystemData/"

# Get target 5 lines
cat /proc/cpuinfo | grep "cpu \|model*" | head --lines=5 > "$SPATH"cpuinfo.txt
# Only Using memory
cat /proc/meminfo | grep -v " 0" > "$SPATH"meminfo.txt
find /home/baami/OJT/ -type f -mtime -1 -ls > "$SPATH"recentlyfile.txt
df -h > "$SPATH"harddiskinfo.txt
