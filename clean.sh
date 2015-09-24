#!/bin/bash
set -e

for directory in */; do
	echo "Clean '$directory'"
	cd $directory
	make clean
	cd ..
done
