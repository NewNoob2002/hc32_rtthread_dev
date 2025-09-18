#pragma once
#include <stdio.h>

#ifndef INFO_PRINTF
#define INFO_PRINTF(fmt, ...)   printf("[I] " fmt "\n", ##__VA_ARGS__)
#endif

#ifndef DEBUG_PRINTF
#define DEBUG_PRINTF(fmt, ...)  printf("[D] " fmt "\n", ##__VA_ARGS__)
#endif

#ifndef ERROR_PRINTF
#define ERROR_PRINTF(fmt, ...)  printf("[E] " fmt "\n", ##__VA_ARGS__)
#endif