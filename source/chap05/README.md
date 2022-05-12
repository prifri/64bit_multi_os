[copy](https://github.com/kkamagui/mint64os-examples/tree/master/source_code/chap05)

# DIsk.img


```
--------------+ 
              |
01.kernel32   |
              |
--------------+ 0x512 => Bootloader에서 disk read로 0x10000에 load된다.
              |
00.Bootloader |
              |
--------------+ 0x00  => bios에 0x7C00 에 1 sector size만큼 load된다.
                         즉 disk의 1번 sector만큼은 biost에서 읽으므로
						 Bootloader에서는 2번부터 읽어야된다.
```

# Test가 정상 동작 안하는문제

숫자 print가 1024개가 되야되는데 안된다. 책을 산 직후(2012년이였나)에는 이게 됬는데
동일한 code에서 안되고 나름 source 재분석과 debug를 했는데 이상할점이 없어 qemu문제라고 파악되서 검색을 해봤다.

## floppy Qemu bug

[link1](https://lists.gnu.org/archive/html/qemu-devel/2018-12/msg00421.html)

현재 최신버전 qemu에서 floppy read가 정상동작 안하고 있다. (broken 된다고한다.)
그래서 test를 하면 print를 정상적으로 출력안한다. 
2018년도 쯤부터 있었던일인데 2022년인 지금까지 수정안된것으로 보아 별 고칠 생각이
없는듯 싶다. 일단 어느정도까진 floppy read가 되니 실습은 적당히 불러와보고 나중에 disk load로 넘어갈때가지 버티는게 좋을듯 하다.
