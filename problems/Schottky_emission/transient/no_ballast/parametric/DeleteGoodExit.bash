#!/bin/bash

file="CurrentCycle.log"

for f in `ls */${file}` ; do
	#editedTime="$(date +%s -r "$PWD/$f/$(ls -1thr "$PWD/$f/" | tail -n 1)")"
	#currentTime="$(date +%s)"
	
	g=`dirname ${f}`
	outputFile=`ls $g/$g.o*`
	jobNumber=${outputFile#"$g/$g.o"}
	jobResult=`qstat -j $jobNumber 2> /dev/null`
	
	#[ $(( ${currentTime} - ${editedTime} )) -gt 3600 ] && 
	
	if [ -f "$PWD/${g}/PowerAndEfficiency.csv" ] && [ ${#jobResult} -lt 50 ] 
	then
		echo "Deleting: ${g}"
		rm -rf "${g}" ;
	fi
done
