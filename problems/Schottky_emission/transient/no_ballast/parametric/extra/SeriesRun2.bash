#!/bin/bash

oldFolder="base"

for d in $( seq 15.0 -1.0 6.0 )
do
	for phi in 4.40 #$( seq 4.40 -0.1 3.80 )
	do
		folder="V_200_V_d_${d}_um_phi_${phi}_eV_tOn_0.5_ns"
		cp -r ${oldFolder} ${folder}
		cd ${folder}
			sed -i "s?user_work_function = 4.00?user_work_function = ${phi}?g" "Transient.i" ;
			sed -i "s?user_work_function = 4.00?user_work_function = ${phi}?g" "SteadyState.i" ;
			
			sed -i 's?dom0Size = ${/ 2E-6 ${position_units}}?dom0Size = ${/ '${d}'E-6 ${position_units}}?g' "Transient.i" ;
			sed -i 's?dom0Size = ${/ 2E-6 ${position_units}}?dom0Size = ${/ '${d}'E-6 ${position_units}}?g' "SteadyState.i" ;
			
			sed -i "s?${oldFolder}?${folder}?g" "Submission.job" ;
			
			#qsub Submission.job
			./Submission.job
		cd ..
	done
done
