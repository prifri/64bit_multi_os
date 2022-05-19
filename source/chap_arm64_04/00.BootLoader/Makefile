NAME		= BootLoader.bin
CROSS		= aarch64-linux-gnu-
#CFLAGS		= -ggdb3 -std=gnu99 -Wall
LDFLAGS 	= -Bstatic --gc-sections -nostartfiles -nostdlib

all: ${NAME}
%.o: %.asm
	${CROSS}as -o $@ $<

%.o: %.c
	${CROSS}gcc ${CFLAGS} -c -o $@ $<

$(NAME): main.o BootLoader.o 
	${CROSS}ld $(LDFLAGS) -o $@ -T link.ld $<
	
# 소스 파일을 제외한 나머지 파일 정리	
clean:
	rm -f ${NAME} BootLoader.o main.o
