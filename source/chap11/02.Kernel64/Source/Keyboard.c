#include "Types.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"

#define KEYBOARD_READ_REG_STATUS	0x64
#define KEYBOARD_WRITE_REG_CTRL	0x64
//******************************************************************
// 키보드 컨트롤러와 제어 함수
//******************************************************************
// output buffer(port 0x60)에 수신된 데이터가 있는지 여부를 반환
bool kIsOutputBufferFull(void)
{
	if (kInPortByte(KEYBOARD_READ_REG_STATUS) & 0x01)
	{
		return true;
	}

	return false;
}

// input buffer(port 0x60)에 프로세서가 쓴 데이터가 남아잇는지 여부 확인
bool kIsInputBufferFull(void)
{
	// status register(port 0x64)에서 읽은 값에 입력 버퍼 상태 비트(b[1])이
	// set되있으면 아직 키보드가 data를 가져가지 않았음.
	if (kInPortByte(KEYBOARD_READ_REG_STATUS) & 0x2)
	{
		return true;
	}

	return false;
}

// keyboard active
bool kActivateKeyboard(void)
{
	int i;
	int j;
}
