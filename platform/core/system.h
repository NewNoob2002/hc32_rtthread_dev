#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include <stdio.h>
#include "hc32_ddl.h"
#ifdef __cplusplus
extern "C" {
#endif
const char *en_result_to_string(en_result_t result);
const char* MakeTimeString(uint64_t ms, char* buf, uint16_t len);
#ifdef __cplusplus
}
#endif

#endif