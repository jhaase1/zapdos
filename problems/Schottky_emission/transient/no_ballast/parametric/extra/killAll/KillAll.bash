#!/bin/bash

oldFolder="base"

for V in $( seq 1 1 30 )
do
	qsub Submission.job
done
