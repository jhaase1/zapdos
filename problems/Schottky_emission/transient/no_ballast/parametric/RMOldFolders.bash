#!/bin/bash

for f in `ls -d V*` ; do
	if [ $(($(date +%s) - $(date +%s -r "$PWD/$f/$(ls -1thr "$PWD/$f/" | tail -n 1)"))) -gt 64800 ]  ; then
		echo "Deleting: $f"
		rm -rf "$f" ;
	fi
done
