#!/bin/bash

oldFolder="base"

gas="Ar"

for VLow in 0.0 # `echo "$( seq -2.0 0.2 -0.2 )" && echo " $( seq 0.2 0.2 1.0 )"` # $( seq -2.0 0.2 -0.2 ) #
do
	for phi in $( seq 3.5 0.5 3.5 )
	do
		for tOn in $( seq 2 1 2 )
		do
			for tOff in $( seq 100 25 250 )
			do
				for d in $( seq 4 1 4 )
				#for d in $( seq 3 1 3 )
				do
					for VHigh in 25.5 28.4 31.5 35.0 38.9 # 31.5 # 25.5 28.4 35.0 38.9 # $( seq 18.0 0.1 23.0 )
					#for VHigh in 19.2 21.3 23.6 26.3 29.2 # 23.6 # 19.2 21.3 26.3 29.2 # $( seq 18.0 0.1 23.0 )
					do
						folder="${gas}_phi_${phi}_eV_tOn_${tOn}_ns_tOff_${tOff}_ns_d_${d}_um_VHigh_${VHigh}_V_VLow_${VLow}_V"
						
						cp -rT ${oldFolder} ${folder}
						cd ${folder}
						
							sed -i 's?VLow = 1E-3?VLow = '${VLow}'E-3?g' "Initial.i" ;
							sed -i 's?VLow = 1E-3?VLow = '${VLow}'E-3?g' "SteadyState.i" ;
						
							sed -i 's?vhigh = 200E-3?vhigh = '${VHigh}'E-3?g' "Initial.i" ;
							sed -i 's?vhigh = 200E-3?vhigh = '${VHigh}'E-3?g' "SteadyState.i" ;
							
							sed -i 's?gap = 4E-6?gap = '${d}'E-6?g' "Initial.i" ;
							sed -i 's?gap = 4E-6?gap = '${d}'E-6?g' "SteadyState.i" ;
							
							sed -i 's?tOn = 0.5E-9?tOn = '${tOn}'E-9?g' "Initial.i" ;
							sed -i 's?tOn = 0.5E-9?tOn = '${tOn}'E-9?g' "SteadyState.i" ;
							
							sed -i 's?tOff = 21E-9?tOff = '${tOff}'E-9?g' "Initial.i" ;
							sed -i 's?tOff = 21E-9?tOff = '${tOff}'E-9?g' "SteadyState.i" ;
							
							sed -i 's?work_function = 4.00?work_function = '${phi}'?g' "Initial.i" ;
							sed -i 's?work_function = 4.00?work_function = '${phi}'?g' "SteadyState.i" ;
							
							sed -i "s?-N ${oldFolder}?-N ${folder}?g" "Submission.job" ;
							sed -i "s?-N ${oldFolder}?-N ${folder}?g" "DoNotDeflate.job" ;
							
							qsub Submission.job
						cd ..
					done
				done
			done
		done
	done
done
