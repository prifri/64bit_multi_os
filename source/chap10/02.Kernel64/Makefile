# file		Makefile
# date		2008/12/14
# author	kkamagui 
# brief		보호 모드 커널을 빌드하기 위한 make 파일

################################################################################
# 빌드 환경 및 규칙 설정 
################################################################################
# 컴파일러 및 링커 정의
NASM64 = nasm -f elf64
GCC64 = gcc -c -m64 -ffreestanding
LD32 = ld -melf_x86_64 -T ../elf_x86_64.x -nostdlib -e Main -Ttext 0x200000

# 바이너리 이미지 생성을 위한 OBJCOPY 옵션 정의
# -j : section 추철.
# -S : 재배치 정보와 심볼 정보 제거
# -O : 생성할 file format
# 만들어진 elf 파일에서 특정 section 정보를 추출해서 새로 만들겠다는것.
OBJCOPY64 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECTDIRECTORY = Temp
SOURCEDIRECTORY	= Source

################################################################################
# 빌드 항목 및 빌드 방법 설정
################################################################################
all:prepare Kernel64.bin

# 오브젝트 파일이 위치할 디렉터리를 생성
prepare:
	mkdir -p $(OBJECTDIRECTORY)

dep:
	@echo === Make Dependancy File ===
	make -C $(OBJECTDIRECTORY) -f ../Makefile InternalDependency
	@echo === Dependancy Search Complete ===

# 디렉터리를 오브젝트 파일 디렉터리로 이동해서 의존성 파일 및 실행 파일을 생성
ExecuteInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../Makefile Kernel64.elf

# 커널 이미지를 바이너리 파일로 변환
Kernel64.elf.bin: ExecuteInternalBuild
	$(OBJCOPY64) $(OBJECTDIRECTORY)/Kernel64.elf $@

# 소스 파일을 제외한 나머지 파일 정리	
clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*

################################################################################
# Make에 의해 다시 호출되는 부분, Temp 디렉터리를 기준으로 수행됨
################################################################################
# 빌드할 assembly 소스 파일 정의, Temp 디렉터리를 기준으로 설정
ENTRYPOINTSOURCEFILE = ../$(SOURCEDIRECTORY)/EntryPoint.s
ENTRYPOINTOBJECTFILE = EntryPoint.o
#***** PRIFRI, 2022.05.16:
# wildcard : 디렉토리에서 특정 패턴의 파일들을 추출한다.
#*****
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
#***** PRIFRI, 2022.05.16:
# $(patsubst 수정할패턴, 교체할 패턴, 입력문자열)
# CSOURCEFILES의 내용에서 .c 확장자를 .o로 변경하겠다는뜻.
# subst(대상 문자열, 수정할 문자열, 입력문자열)
#
# CSOURCEFILES의 c파일들을 o로 교체하고 거기서 Main.o파일은 공백으로 만들겠다는것.
# Main.o를 무조건 제일 앞에 두고 나머지 뒤로 둘려고 하는 속셈이다.
#*****
COBJECTFILES = $(subst Main.o, , $(notdir $(patsubst %.c,%.o,$(CSOURCEFILES))))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

$(ENTRYPOINTOBJECTFILE) : $(ENTRYPOINTSOURCEFILE)
	$(NASM64) -o $@ $<

# .c 파일을 .o 파일로 바꾸는 규칙 정의
# $< : Dependancy의 첫번째항목. 즉 ../$(SOURCEDIRECTORY)/%.c
# ../$(SOURCEDIRECTORY)/%.c 에 해당하는 file들을 전부 %.o확장자로 생성한다는것.
%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<

# .asm 파일을 .o 파일로 바꾸는 규칙 정의
%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM64) -f elf32 -o $@ $<

# 실제 의존성에 관련된 파일을 생성
#***** PRIFRI, 2022.05.16:
# -MM : compiler header를 제외한 header Dependancy 관계를 찾아서 저장한다.
#*****
InternalDependency:
	$(GCC64) -MM $(CSOURCEFILES) > Dependency.dep

# 실제 커널 이미지를 빌드
#***** PRIFRI, 2022.05.16:
# CENTRYPOINTOBJECTFILE : Main.o 의 이름만 있따.
# COBJECTFILES : Main.o제외한 것들만 존재한다.
# 이렇게 하면 제일 앞에 있는 Main.o가 첫위치로 될것이다.
#*****
Kernel64.elf: $(CENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD32) -o $@ $^

# 현재 디릭토리의 파일 중, dependency 파일이 있으면 make에 포함	
ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif
