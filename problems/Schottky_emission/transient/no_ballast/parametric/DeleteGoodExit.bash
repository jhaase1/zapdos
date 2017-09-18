#!/bin/bash

file="CurrentCycle.log"

for f in `ls */${file}` ; do
	#editedTime="$(date +%s -r "$PWD/$f/$(ls -1thr "$PWD/$f/" | tail -n 1)")"
	#currentTime="$(date +%s)"
	
	g=`dirname ${f}`
	outputFile=`ls -tr1 $g/$g.o* | tail -n 1`
	jobNumber=${outputFile#"$g/$g.o"}
	jobResult=`qstat -j $jobNumber 2> /dev/null`
	
	#[ $(( ${currentTime} - ${editedTime} )) -gt 3600 ] && 
	
	if [ -f "$PWD/${g}/PowerAndEfficiency.csv" ] && [ ${#jobResult} -lt 50 ] && [ "$(ls -tr1 ${g} | tail -n 1)"="${outputFile}" ] && [ $(($(date +%s) - $(date +%s -r ${outputFile}))) -gt 3600 ]
	then
		echo "Deleting: ${g}"
		rm -rf "${g}" ;
	fi
done
