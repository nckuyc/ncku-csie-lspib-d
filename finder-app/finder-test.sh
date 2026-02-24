#!/bin/sh
set -e
set -u

NUMFILES=10
WRITESTR=AELD_IS_FUN
WRITEDIR=/tmp/aeld-data

username=$(cat /etc/finder-app/conf/username.txt)
assignment=$(cat /etc/finder-app/conf/assignment.txt)

if [ $# -lt 3 ]
then
    if [ $# -ge 1 ]
    then
        NUMFILES=$1
    fi
else
    NUMFILES=$1
    WRITESTR=$2
    WRITEDIR=/tmp/aeld-data/$3
fi

MATCHSTR="The number of files are ${NUMFILES} and the number of matching lines are ${NUMFILES}"

rm -rf "${WRITEDIR}"

if [ "$assignment" != "assignment1" ]
then
    mkdir -p "$WRITEDIR"
fi

for i in $( seq 1 $NUMFILES )
do
    writer "$WRITEDIR/${username}$i.txt" "$WRITESTR"
done

OUTPUTSTRING=$(finder.sh "$WRITEDIR" "$WRITESTR")

echo "${OUTPUTSTRING}" > /tmp/assignment4-result.txt

echo "${OUTPUTSTRING}" | grep "${MATCHSTR}"
if [ $? -eq 0 ]; then
    echo "success"
    exit 0
else
    echo "failed"
    exit 1
fi
