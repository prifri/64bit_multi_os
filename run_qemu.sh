#!/bin/bash

if [ -z $1 ]
then
	echo "./run_qemu.sh file_name"
	echo "ex) ./run_qemu.sh ./source/chap04/Disk.img"
	exit 0
fi

if [ ! -f $1 ]
then
	echo "$1 not exists"
	exit 0
fi

# https://sean.tistory.com/134?category=869595 
qemu-system-x86_64 -m 64 -fda ${1} -rtc base=localtime -M pc
