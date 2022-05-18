#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "Types.h"

void kReadCPUID(uint32_t dwEAX, uint32_t *pdwEAX, uint32_t *pdwEBX,
				uint32_t *pdwECX, uint32_t *pdwEDX);
void kSwitchAndExecute64bitKernel(void);
#endif
