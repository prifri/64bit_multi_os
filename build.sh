if [ -z $1 ]
then
	echo "./build.sh dir_name"
	echo "ex) ./build.sh chap04"
	echo "ex) ./build.sh chap04 run"
	echo "ex) ./build.sh chap04 clean"
	exit 1
fi

CMD=$2

###########################
# find dir
###########################

PREFIX="chap"
DIR=./source/"$1"

if [ ! -d ${DIR} ]
then
	DIR=./source/"${PREFIX}""$1"
fi

if [ ! -d ${DIR} ]
then
	DIR=./source/"${PREFIX}0""$1"
fi

###########################
# build or clean
###########################

TARGET=${DIR}/Disk.img

cd ${DIR}

if [ "${CMD}" == "clean" ]
then
	make clean
	exit 0
fi

make

if [ $? -ne 0 ]
then
	exit 0
fi
###########################
# qemu run
###########################

if [ "${CMD}" == "run" ]
then
	cd ../../
	./run_qemu.sh ${TARGET} 
fi
