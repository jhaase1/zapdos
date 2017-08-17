#!/bin/bash

# Create summary and move summarized data to results folder
for f in `ls V*/SteadyState_out.e` ; do 
	cp base/cycleAnalysis.py $(dirname $f)
	cd $(dirname $f)
	echo "Entering: $(dirname $f)"
	pvpython cycleAnalysis.py
	cp PowerAndEfficiency.csv "../results/$(dirname $f).csv"
	cd ..
done
