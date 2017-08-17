#!/bin/bash

var=$(cat BadExit.txt | md5sum)

file="CurrentCycle.log"

for f in `ls */${file}` ; do
	var2=$(tail -n 6 ${f} | md5sum)
	
	g=`dirname ${f}`
	outputFile=`ls $g/$g.o*`
	jobNumber=${outputFile#"$g/$g.o"}
	jobResult=`qstat -j $jobNumber 2> /dev/null`
	
	if [[ $var == $var2 ]] && [[ ${#jobResult} -lt 50 ]] ; then
		echo "deleting ${g}"
		rm -rf ${g};
#	else
#		echo "not deleting $(dirname ${f})";
	fi

done
