#!/bin/bash

fileName="SteadyState_out.e"
newFolder="download"

minimumsize=90000

for pathName in `ls */${fileName}` ; do
	folderName="`echo ${pathName} | cut -d "/" -f1`"
	
	actualsize=$(wc -c <"$pathName")
	
	if [ $actualsize -ge $minimumsize ]; then
		mv "${folderName}/${fileName}" "${newFolder}/${folderName}.e"
		rm -rf "${folderName}"
	else
		echo Did not move $pathName
	fi
done
