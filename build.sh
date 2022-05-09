if [ -z $1 ]
then
	echo "./build.sh dir_name"
	echo "ex) ./build.sh chap04"
	echo "ex) ./build.sh chap04 run"
	exit 1
fi

DIR=./source/"$1"
cd ${DIR}

TARGET=${DIR}/Disk.img
make

if [ $? -ne 0 ];
then
	exit 1
fi

if [ -z $2 ]
then
	exit 0
fi

cd ../../

if [ "$2" == "run" ]
then
	./run_qemu.sh ${TARGET}
fi
