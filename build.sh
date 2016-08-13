#!/bin/bash
set -e

runmake () {
	echo "Build '$1'"
	cd $1
	make
	cd ..
}

makedirs ()
{
	for directory in "$@"; do
		if [ -a $directory/Makefile ]; then
			runmake $directory
		else
			makedirs $directory/*
		fi
	done
}

makedirs */
