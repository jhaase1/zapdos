#!/bin/bash

for f in `ls -d Xe*` ; do
	cd $f ;
	
	cp ~/noBallast/base/Submission.job Submission.job
	sed -i "s?-N base?-N ${f}?g" "Submission.job" ;

	if [ ! -f SteadyState.e ]; then
		for g in `ls PreviousCycle* -d | grep -v '\.i'` ; do 
			mv "$g" $(echo "$g" | sed 's/PreviousCycle/SteadyState/g') ;
		done
	fi
	
	qsub Submission.job

	cd .. ;
done
