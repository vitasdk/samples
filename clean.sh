#!/bin/bash
set -e

runclean () {
	echo "Clean '$1'"
	cd $1
	make clean
	cd ..
}

cleandirs ()
{
	for directory in "$@"; do
		if [ -a $directory/Makefile ]; then
			runclean $directory
		else
			cleandirs $directory/*
		fi
	done
}

cleandirs */
