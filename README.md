# 64bit_multi_os

# 1. 참고자료

[원작자 github](https://github.com/kkamagui/mint64os-examples/blob/master/README.md) 

[환경](https://sean.tistory.com/7?category=869595)

# 2. 환경설정

## 2.1. 개발환경

ubuntu 20.04 64bit 

## 2.2. 패키지설치

```
sudo apt install binutils bison flex libc6-dev libtool make patchutils libgmp-dev libmpfr-dev libmpc-dev nasm qemu-kvm
```

# 3. build

```
./build.sh {chap dir}
```

# 4. run

## 4.1 build & run

```
./build.sh {chap dir} run
```

## 4.2 only run

```
./run_qemu.sh {image}
```

# 오탈자

[원작자 link](https://kkamagui.tistory.com/781)
