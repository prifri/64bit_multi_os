[BITS 64]

SECTION .text

global kInPortByte, kOutPortByte

; read port 1byte
; PARAM: port_number
kInPortByte:
  push rdx    ; 함수에서 임시로 사용하는 register를 stack에 저장
            ; 함수의 마지막 부분에서 스택에 삽입된 값을 꺼내 복원
  mov rdx, rdi  ; rdx에 1번 인자(port number 저장
  mov rax, 0    ; rax clear
  in  al, dx    ; al에 저장. al은 함수의 반환값으로 사용.
  
  pop rdx       ; 함수에서 사용이 끝난 레지스터를 복원
  ret           ; 함수를 호출한 다음 코드를 위치로 복귀

; write port 1byte
; PARAM: port_number, value
kOutPortByte:
  push rdx      ; 함수에서 임시로 사용하는 regi를 stack에 저장
  push rax      ; 함수에서 마지막 부분에서 스택에 삽입된 값을 꺼내 복원
  
  mov rdx, rdi  ; rdx에 1번인자 저장(port_number)
  mov rax, rsi  ; rax에 write value 저장
  out dx, al    ; dx에 저장된 포트 어드레스에 al에 저장된 한 바이트를 씀.
  
  pop rax       ; 함수에서 사용이 끝난 regi 복원
  pop rdx
  ret

