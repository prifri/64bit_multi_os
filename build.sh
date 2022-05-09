cd ./source/
make

if [ -z $1]
then
	exit 0
fi

cd ..

if [ "$1" == "run" ]
then
	./run_qemu.sh
fi
