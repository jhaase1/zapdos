#!/bin/bash

tmpfile=$(mktemp)
file="CurrentCycle.log"

for f in `ls */${file}` ; do
	dName="$( dirname $f )"
	
	timeInfo=`echo "$( grep -A 1 "time = " ${f} | tail -2 2>/dev/null )" | tr -d ' \t\n\r\f'`
	
	if [[ ${#timeInfo} -eq 0 ]]; then
		timeInfo=",,"
	fi
	
	cycleInfo=`echo "$( cat $dName/TotalRelNorm.csv 2>/dev/null  )" | tr -d ' \t\n\r\f'`

	if [[ ${#cycleInfo} -eq 0 ]]; then
		cycleInfo=",,"
	fi
	
	line=`echo "$dName, ${timeInfo}, ${cycleInfo}"`
	
	echo ${line} >> $tmpfile
done

sed -e "s/dt=/,/g" -i $tmpfile
sed -e "s/time=//g" -i $tmpfile

cut --complement -d "," -f 2 $tmpfile | column -s, -t | sort -Vk1,1

rm $tmpfile