#!/bin/bash
set -e

for directory in */; do
	echo "Build '$directory'"
	cd $directory
	make
	cd ..
done
