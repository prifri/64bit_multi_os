[BITS 64]

SECTION .text

; 외부에서 정의된 함수를 쓸수 있도록 선언함
extern Main

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; code section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
; IA-32e 64bit mode kernel용 data segment desc를 각 segment에 설정
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  ; stack을 0x600000 ~ 0x6FFFFF 영역에 생성 (1MB)
  mov rsp, 0x6ffff8
  mov rbp, 0x6ffff8

  call Main

  jmp $
