[BITS 32] ; 32bit code 라는것을 알림.

; C 언어에서 호출할 수 있또록 이름을 노출함(Export)
global kReadCPUID, kSwitchAndExecure64bitkernel

SECTION .text ;text section 정의

; CPUID를 반환
; param: dword dwEAX, dword* pdwEAX, *pdwEBX, *pdwECX, *pdwEDX
kReadCPUID:
  push ebp  ; bp(EBP)를 스택에 삽입
;***** PRIFRI, 2022.05.18:
; esp에 param 들이 쌓여있는 상태. 함수 수행전에 기존 register값을 backup해놓기
; 위해 stack에 넣어야되는데, 그 전에 stack에 쌓여있는 인자값을 나중에 접근하기
; 위해 ebp에 올려놓고 stack에 regi값들을 backup한다.
;*****
  mov ebp, esp  ; base pointer register(EBP)에 stack pointer regi(ESP)의 값을 set
  push eax      ; 함수에서 임시로 사용하는 레지스터로 함수의 마지막 부분에서
  push ebx      ; 스택에 삽입된 값을 꺼내 원래 값으로 복원
  push ecx
  push edx
  push esi

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; EAX register의 값으로 CPUID 명령어 실행
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;***** PRIFRI, 2022.05.18:
; ebp + 8  : dwEAX
; ebp + 12 : *pdwEAX
; ebp + 16 : *pdwEBX
; ebp + 20 : *pdwECX
; ebp + 24 : *pdwEDX
;*****
mov eax, dword [ ebp + 8 ] ; dwEAX param을 EAX 에 저장
;***** PRIFRI, 2022.05.18:
; cpuid : eax에 저장된 값에 따라 동작 수행.
; 0x0 : 기본 cpuid정보 조회
; 0x80000001 : 확장 기능 cpuid 정보 조회
;*****
cpuid                      ; cpuid실행.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 반환된 값을 파라미터에 저장
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; *pdwEAX
mov esi, dword [ ebp + 12 ] ; param 2(*pdwEAX)를 esi register에 저장
mov dword [esi], eax        ; *pdwEAX가 ptr이므로 ptr이 가리키는 어드레스에
                            ; eax 레지스터값을 저장

; *pdwEBX
mov esi, dword [ ebp + 16 ] ; param 3(*pdwEBX)를 esi register에 저장
mov dword [esi], ebx        ; *pdwEBX가 ptr이므로 ptr이 가리키는 어드레스에
                            ; eax 레지스터값을 저장

; *pdwECX
mov esi, dword [ ebp + 16 ] ; param 4(*pdwECX)를 esi register에 저장
mov dword [esi], ecx        ; *pdwECX가 ptr이므로 ptr이 가리키는 어드레스에
                            ; eax 레지스터값을 저장

; *pdwEDX
mov esi, dword [ ebp + 16 ] ; param 6(*pdwEDX)를 esi register에 저장
mov dword [esi], edx        ; *pdwEDX가 ptr이므로 ptr이 가리키는 어드레스에
                            ; eax 레지스터값을 저장

; 함수에서 사용이 끝난 esi regi부터 ebp regi까지 stack에 삽입된 값을 이용해
; 복원. 스택은 가장 마지막에 들어간 데이터가 가장 먼저 나오는 자료구조이므로
; 삽입의 역순으로 제거해야함.
pop esi
pop edx
pop ecx
pop ebx
pop eax
pop ebp
ret     ; 함수를 호출한 다음 코드의 위치로 복귀

; IA-32e 64bit mode로 전환하고 64bit 커널을 수행
; PARAM: 없음.
kSwitchAndExecute64bitKernel:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; CR4 control register의 PAE bit를 1로 설정.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;***** PRIFRI, 2022.05.18:
; Physical Address Extensions. 36bit 이상의 물리 메모리를 사용할지 여부 결정.
;*****
mov eax, cr4    ; cr4 control register값을 eax에 저장
or eax, 0x20    ; PAE(b[5]) set
mov cr4, eax

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; CR3 control register에 PML4 table의 address와 cache enable
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;***** PRIFRI, 2022.05.18:
; CR3. PDBR(Page-Directory Base Register)라고도 불린다.
; page fault 예외처리시 CR2에 set되는 page fault로 점프한다.
;*****
mov eax, 0x100000 ; eax에 pml4 table address set.
mov cr3, eax      ; cr3 control register에 저장.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; IA32_EFER.LME를 1로 set. IA-32e mode enable
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;***** PRIFRI, 2022.05.18:
; MSR(Model-Specific Register)
; EFER(Extended Feature Enable Register)
; RDMSR(Read From MSR) :
;   ecx : read할 msr register value
;   edx : 상위 32bit result
;   eax : 하위 32bit result
; WRMSR(Write To MSR)
;   ecx : write할 msr
;   edx : write 할 상위 32bit 
;   eax : write 할 하위 32bit 
;
; LME : Long Mode Enable. IA-32e mode 활성화
;*****
mov ecx, 0xC0000080 ; IA32_EFER MSR register의 address를 저장
rdmsr               ; msr read

or eax, 0x100       ; eax에 저장된 IA32_EFER MSR의 하위 32bit에서 LME(bit[8]) set
wrmsr               ; msr write

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; CR0 control register를 NW(b[29]) clear, CD(b[30] clear, PG(b[31]) set.
; 설정하여 캐시 기능과 페이징 기능을 활성화.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;***** PRIFRI, 2022.05.18:
; NW : 0이면 write through, 1이면 write back
; CD : cache disable. 1이면 disable
; PG : Paging. 1이면 사용
;*****
mov eax, cr0    ; eax 에 cr0 저장.
or eax, 0xE000000 ; nw, cd, pg set
xor eax, 0x6000000 ; nw, cd clear
mov cr0, eax ; cr0 write

jmp 0x08:0x200000 ; cs segment selector를 IA_32 mode용 code segment desc로 교체.
                  ; 및 2MB로 이동.

; 여기는 실행되지 않음.
jmp $
