# 07 -> 08

IA-32e mode kernel memory 초기화

## A20

real mode offset max address
```
0xFFFF:0xFFFF => 0xFFFF * 0x10 + 0xFFFF
               = 0xFFFF0 + 0xFFFF
			   = 0x10FFEF
```

1MB 이상을 넘어가는 영역을 접근하면 하위어드레 0xFFEF만 남았다.

- A20이 disable 인경우
3MB + 64KB = 0x310000 => 0b0011 0001 0000 0000 0000 0000
                      => 0b0010 0001 0000 0000 0000 0000
					  (b[19] bit가 언제나 0로 인식)
					  => 0x210000 => 2MB + 64KB

0x30000으로 접근해도 0x20000으로 접근된다. 즉 1, 3, 5.. MB의 홀수를 접근하면 
짝수 0, 2, 4, .. MB가 접근된다. 만약 0x10000으로 접근해 0 clear를 한다면 0x0번지가
 0 clear가 될것이고, 만약 0x0번지가 code영역이라면 문제가 발생할것이다.

# 수정사항
