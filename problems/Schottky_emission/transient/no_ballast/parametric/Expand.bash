#!/bin/bash

folder="backup"

#for List in "Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_10_um_V_45.6_V" ; do 
for List in "Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_3_um_V_22.4_V" "Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_4_um_V_26.5_V" "Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_5_um_V_29.0_V" "Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_6_um_V_33.3_V" "Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_7_um_V_36_V" "Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_8_um_V_39.8_V" "Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_10_um_V_45.6_V" ; do 
#for inputFile in `ls ${folder}/Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_3_um_V_22.4_V.i` ; do

	inputFile="${folder}/${List}.i"

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
	sed -i "s?-N base?-N ${base}?g" "ReInflate.job" ;
	qsub ReInflate.job
	
	cd ..
		
done
