#!/bin/bash

oldFolder="base"

gas="Ar"

for VOut in $( seq -1 0.1 3 )
do
	for phi in $( seq 2.5 0.5 2.5 )
	do
		for tOn in $( seq 2 1 2 )
		do
			for tOff in $( seq 100 20 100 )
			do
				for d in $( seq 10 1 10 )
				do
					for VPlasma in $( seq 40 -1 40 )
					do
						folder="${gas}_phi_${phi}_eV_tOn_${tOn}_ns_tOff_${tOff}_ns_d_${d}_um_VPlasma_${VPlasma}_V_VOut_${VOut}_V"
						cp -rT ${oldFolder} ${folder}
						cd ${folder}
							sed -i 's?VOut = 1E-3?VOut = '${VOut}'E-3?g' "Initial.i" ;
							sed -i 's?VOut = 1E-3?VOut = '${VOut}'E-3?g' "SteadyState.i" ;
						
							sed -i 's?vhigh = 200E-3?vhigh = '${VPlasma}'E-3?g' "Initial.i" ;
							sed -i 's?vhigh = 200E-3?vhigh = '${VPlasma}'E-3?g' "SteadyState.i" ;
							
							sed -i 's?gap = 4E-6?gap = '${d}'E-6?g' "Initial.i" ;
							sed -i 's?gap = 4E-6?gap = '${d}'E-6?g' "SteadyState.i" ;
							
							sed -i 's?tOn = 0.5E-9?tOn = '${tOn}'E-9?g' "Initial.i" ;
							sed -i 's?tOn = 0.5E-9?tOn = '${tOn}'E-9?g' "SteadyState.i" ;
							
							sed -i 's?tOff = 21E-9?tOff = '${tOff}'E-9?g' "Initial.i" ;
							sed -i 's?tOff = 21E-9?tOff = '${tOff}'E-9?g' "SteadyState.i" ;
							
							sed -i 's?work_function = 4.00?work_function = '${phi}'?g' "Initial.i" ;
							sed -i 's?work_function = 4.00?work_function = '${phi}'?g' "SteadyState.i" ;
							
							sed -i "s?-N ${oldFolder}?-N ${folder}?g" "Submission.job" ;
							
							qsub Submission.job
						cd ..
					done
				done
			done
		done
	done
done
