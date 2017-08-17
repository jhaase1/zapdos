#!/bin/bash


for f in $( seq 1 "$#" ) ; do
	g=${!f}
	outputFile=`ls $g/$g.o*`
	jobNumber=${outputFile#"$g/$g.o"}
	
	qdel ${jobNumber}
	rm -rf ${g}
	
done
