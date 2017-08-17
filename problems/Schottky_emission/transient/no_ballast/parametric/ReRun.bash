#!/bin/bash

folder="backup"

for inputFile in `ls ${folder}/*.i` ; do
	base=`basename "${inputFile%.*}"`

	#cp base/Submission.job ${base}/Submission.job
	
	cp -rT base ${base}
	
	for g in `ls -d ${folder}/${base}*` ; do 
		cp -rT ${g} ${base}/$(echo `basename "$g"` | sed "s/${base}/SteadyState/g")
	done
	
	cd ${base}
	
	mv SteadyState.csv TotalRelNorm.csv
	
	#sed -i "s?-N base?-N ${base}?g" "${base}/Submission.job" ;
	
	sed -i "s?-N base?-N ${base}?g" "Submission.job" ;
	qsub Submission.job
	
	cd ..
		
done
