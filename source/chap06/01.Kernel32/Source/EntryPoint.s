# file      EntryPoint.s
# date      2008/11/27
# author    kkamagui 
#           Copyright(c)2008 All rights reserved by kkamagui
# brief     보호 모드 커널 엔트리 포인트에 관련된 소스 파일

[ORG 0x00]          ; 코드의 시작 어드레스를 0x00으로 설정
[BITS 16]           ; 이하의 코드는 16비트 코드로 설정

SECTION .text       ; text 섹션(세그먼트)을 정의

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x1000  ; 보호 모드 엔트리 포인트의 시작 어드레스(0x10000)를 
                    ; 세그먼트 레지스터 값으로 변환
    mov ds, ax      ; DS 세그먼트 레지스터에 설정
    mov es, ax      ; ES 세그먼트 레지스터에 설정
    ;***** PRIFRI, 2022.05.12:
    ; swi off후 GDT 설정
    ;*****
    cli             ; 인터럽트가 발생하지 못하도록 설정
    lgdt [ GDTR ]   ; GDTR 자료구조를 프로세서에 설정하여 GDT 테이블을 로드

;***** PRIFRI, 2022.05.12:
; 다끄고 일단 보호모드로 동작 시킨다.
;*****
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 보호모드로 진입
    ; Disable Paging, Disable Cache, Internal FPU, Disable Align Check, 
    ; Enable ProtectedMode
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, 0x4000003B ; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
    mov cr0, eax        ; CR0 컨트롤 레지스터에 위에서 저장한 플래그를 설정하여 
                        ; 보호 모드로 전환

;***** PRIFRI, 2022.05.12:
; 리얼모드에서 CS segment selector는 segment의 start address를 저장하는 용도로
; 사용했는데 보호모드에서는 segment 정보를 desc에 저장하고 segment selector는
; 해당 desc를 가리키는 용도로 사용한다.
;
; 가리키는 방식은 lgdt 명령어로 저장된 GDTR안에서 가리키는 GDT의 주소를 기준으로
; 정해진다. segment 한개당 크기가 8이므로 NULL다음에 CODEDESCRIPTOR을 가리키고
; 싶다면 0x08을 DATADESCRIPTOR을 가리키고 싶다면 0x10 한다.
;
; DATADESCRIPTOR : 0x10
; CODEDESCRIPTOR | 0x08
; GDT, NULL      | 0x00
; 
; jmp주소가 역시 절대주소로 PROTECTEDMODE가 저장되있으므로 0x10000 offset을
; 더해준다.
;*****
    ; 커널 코드 세그먼트를 0x00을 기준으로 하는 것으로 교체하고 EIP의 값을 0x00을 기준으로 재설정
    ; CS 세그먼트 셀렉터 : EIP
    jmp dword 0x08: ( PROTECTEDMODE - $$ + 0x10000 )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; 보호 모드로 진입
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
[BITS 32]               ; 이하의 코드는 32비트 코드로 설정
PROTECTEDMODE:
    mov ax, 0x10        ; 보호 모드 커널용 데이터 세그먼트 디스크립터를 AX 레지스터에 저장
;***** PRIFRI, 2022.05.12:
; ds : data 영역을 가키키게 해야되므로 data desc offset인 0x10 입력.
; es, fs, gs, ss도 data seg를 가리키게 해놓는다.
;*****
    mov ds, ax          ; DS 세그먼트 셀렉터에 설정
    mov es, ax          ; ES 세그먼트 셀렉터에 설정
    mov fs, ax          ; FS 세그먼트 셀렉터에 설정
    mov gs, ax          ; GS 세그먼트 셀렉터에 설정
    
    ; 스택을 0x00000000~0x0000FFFF 영역에 64KB 크기로 생성
    mov ss, ax          ; SS 세그먼트 셀렉터에 설정
    mov esp, 0xFFFE     ; ESP 레지스터의 어드레스를 0xFFFE로 설정
    mov ebp, 0xFFFE     ; EBP 레지스터의 어드레스를 0xFFFE로 설정
    
    ; 화면에 보호 모드로 전환되었다는 메시지를 찍는다.
    push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000 )    ; 출력할 메시지의 어드레스르 스택에 삽입
    push 2                                          ; 화면 Y 좌표(2)를 스택에 삽입
    push 0                                          ; 화면 X 좌표(0)를 스택에 삽입
    call PRINTMESSAGE                               ; PRINTMESSAGE 함수 호출
    add esp, 12                                     ; 삽입한 파라미터 제거

    jmp $               ; 현재 위치에서 무한 루프 수행


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   함수 코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 메시지를 출력하는 함수
;   스택에 x 좌표, y 좌표, 문자열
PRINTMESSAGE:
    push ebp        ; 베이스 포인터 레지스터(BP)를 스택에 삽입
    mov ebp, esp    ; 베이스 포인터 레지스터(BP)에 스택 포인터 레지스터(SP)의 값을 설정
    push esi        ; 함수에서 임시로 사용하는 레지스터로 함수의 마지막 부분에서
    push edi        ; 스택에 삽입된 값을 꺼내 원래 값으로 복원
    push eax
    push ecx
    push edx
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; X, Y의 좌표로 비디오 메모리의 어드레스를 계산함
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;***** PRIFRI, 2022.05.12:
; 32bit mode이므로 dword 단위로 접근하는게 보인다.
;*****
    ; Y 좌표를 이용해서 먼저 라인 어드레스를 구함
    mov eax, dword [ ebp + 12 ] ; 파라미터 2(화면 좌표 Y)를 EAX 레지스터에 설정
    mov esi, 160                ; 한 라인의 바이트 수(2 * 80 컬럼)를 ESI 레지스터에 설정
    mul esi                     ; EAX 레지스터와 ESI 레지스터를 곱하여 화면 Y 어드레스 계산
    mov edi, eax                ; 계산된 화면 Y 어드레스를 EDI 레지스터에 설정
    
    ; X 좌료를 이용해서 2를 곱한 후 최종 어드레스를 구함
    mov eax, dword [ ebp + 8 ]  ; 파라미터 1(화면 좌표 X)를 EAX 레지스터에 설정
    mov esi, 2                  ; 한 문자를 나타내는 바이트 수(2)를 ESI 레지스터에 설정
    mul esi                     ; EAX 레지스터와 ESI 레지스터를 곱하여 화면 X 어드레스를 계산
    add edi, eax                ; 화면 Y 어드레스와 계산된 X 어드레스를 더해서
                                ; 실제 비디오 메모리 어드레스를 계산
                                
    ; 출력할 문자열의 어드레스      
    mov esi, dword [ ebp + 16 ] ; 파라미터 3(출력할 문자열의 어드레스)

.MESSAGELOOP:               ; 메시지를 출력하는 루프
    mov cl, byte [ esi ]    ; ESI 레지스터가 가리키는 문자열 위치에서 한 문자를 
                            ; CL 레지스터에 복사
                            ; CL 레지스터는 ECX 레지스터의 하위 1바이트를 의미
                            ; 문자열은 1바이트면 충분하므로 ECX 레지스터의 하위 1바이트만 사용

    cmp cl, 0               ; 복사된 문자와 0을 비교
    je .MESSAGEEND          ; 복사한 문자의 값이 0이면 문자열이 종료되었음을
                            ; 의미하므로 .MESSAGEEND로 이동하여 문자 출력 종료
;***** PRIFRI, 2022.05.12:
; 32bit mode이므로 16bit와는 다르게 0xB8000으로 직접 접근하는게 보인다.
;*****
    mov byte [ edi + 0xB8000 ], cl  ; 0이 아니라면 비디오 메모리 어드레스 
                                    ; 0xB8000 + EDI 에 문자를 출력
    
    add esi, 1              ; ESI 레지스터에 1을 더하여 다음 문자열로 이동
    add edi, 2              ; EDI 레지스터에 2를 더하여 비디오 메모리의 다음 문자 위치로 이동
                            ; 비디오 메모리는 (문자, 속성)의 쌍으로 구성되므로 문자만 출력하려면
                            ; 2를 더해야 함

    jmp .MESSAGELOOP        ; 메시지 출력 루프로 이동하여 다음 문자를 출력

.MESSAGEEND:
    pop edx     ; 함수에서 사용이 끝난 EDX 레지스터부터 EBP 레지스터까지를 스택에
    pop ecx     ; 삽입된 값을 이용해서 복원
    pop eax     ; 스택은 가장 마지막에 들어간 데이터가 가장 먼저 나오는
    pop edi     ; 자료구조(Last-In, First-Out)이므로 삽입(push)의 역순으로
    pop esi     ; 제거(pop) 해야 함
    pop ebp     ; 베이스 포인터 레지스터(BP) 복원
    ret         ; 함수를 호출한 다음 코드의 위치로 복귀

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   데이터 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 아래의 데이터들을 8byte에 맞춰 정렬하기 위해 추가
align 8, db 0

; GDTR의 끝을 8byte로 정렬하기 위해 추가
dw 0x0000
; GDTR 자료구조 정의
; GDT = NULL desc + Code desc + Data desc
; size[15:0] :  end - start - 1 
; base[31:16] : start - current + 0x10000
; 현재 bootloader에 의해 0x10000으로 로딩되므로 compile당시 주소에 offset값을
; 더해주는 개념.
GDTR:
    dw GDTEND - GDT - 1         ; 아래에 위치하는 GDT 테이블의 전체 크기
    dd ( GDT - $$ + 0x10000 )   ; 아래에 위치하는 GDT 테이블의 시작 어드레스

; GDT(Global Descriptor Table) 테이블 정의
GDT:
;***** PRIFRI, 2022.05.12:
; NULLDescriptor는 제약사항상 반드시 최초에 위치해야된다. all0
;*****
    ; 널(NULL) 디스크립터, 반드시 0으로 초기화해야 함
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00

;***** PRIFRI, 2022.05.12:
; - P(Present)
;   discriptor의 유효여부.
; - DPL(Descriptor Privilege Level)
;   0 ~ 3의 범위를 가지며 사용 권한을 의미. 0이 제일 높다.
; - G(Granularity)
;   Segment에 곱해질 가중치 (x4K)
; - D/B(Default Operation Size)
;   1로 설정하면 32비트용 Segment, 0으로 설정하면 16bit용 Segment
; - L
;   IA-32e 모드에서 사용. 64bit code segment인지 32bit code segment인지 구별.
;   1이면 64bit, 0이면 32bit.
; - segment size(limit)
;   16 + 4의 구조로 되있다.
; - base addr 
;   16 + 8 + 8 구조로 되있다.
; - S
;   Descriptor type. 0이면 system desc, 1이면 segment desc
; ------
;*****
    ; 보호 모드 커널용 코드 세그먼트 디스크립터
    CODEDESCRIPTOR:     
;***** PRIFRI, 2022.05.12:
; segment 크기 . G = 1 이므로 1MB * 4K = 4GB
; base addr : 0
; P = 1 : 유효 desc
; DPL = 0 : 
;*****
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
;***** PRIFRI, 2022.05.12:
; 1001 1010
; ^.   ^- type = exec, read
;  +----- P    = 1 -> desc vaild
;         DPL  = 0b00 -> 최고 권한.
;         S    = 1 -> seg desc
; 
; exec + read이니 code section의 개념이 된다. 하지만 linux의 경우 aaternative나
; static key에 의해 code 자체가변경될수도있으니 exec + read이면 무조건
; code section이 되는 절대적인 개념은 아니다.
;*****
        db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
;***** PRIFRI, 2022.05.12:
; 1101 1111
; ^.    ^--- limit [19:16]. [15:0]과 합치면 0xfffff. 1MB에 G가 set이므로 4GB
;  +-------- G = 1    -> seg limit x4K
;            D/B = 1 -> 32bit seg 설정
;            L   = 0 -> IA-32e mode 32bit
;            AVL = not used(rsv)
;*****
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]  
        
    ; 보호 모드 커널용 데이터 세그먼트 디스크립터
    DATADESCRIPTOR:
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
;***** PRIFRI, 2022.05.12:
; type 2 = read,wrtie
;
; read, write이니 data section이 된다. data인데 실행할 필요가 없기때문이다.
; 실행해야되는 code는 code section으로 load해야될것이다.
;*****
        db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]
GDTEND:

; 보호 모드로 전환되었다는 메시지
SWITCHSUCCESSMESSAGE: db 'Switch To Protected Mode Success~!!', 0

times 512 - ( $ - $$ )  db  0x00    ; 512바이트를 맞추기 위해 남은 부분을 0으로 채움
