#include "system.h"

const char *en_result_to_string(en_result_t result)
{
    switch (result)
    {
        case Ok: return "Ok";
        case Error: return "Error";
        case ErrorAddressAlignment: return "ErrorAddressAlignment";
        case ErrorAccessRights: return "ErrorAccessRights";
        case ErrorInvalidParameter: return "ErrorInvalidParameter";
        case ErrorOperationInProgress: return "ErrorOperationInProgress";
        case ErrorInvalidMode: return "ErrorInvalidMode";
        case ErrorUninitialized: return "ErrorUninitialized";
        case ErrorBufferFull: return "ErrorBufferFull";
        case ErrorTimeout: return "ErrorTimeout";
        case ErrorNotReady: return "ErrorNotReady";
        case OperationInProgress: return "OperationInProgress";
    }
    return "Unknown";
}

const char* MakeTimeString(uint64_t ms, char* buf, uint16_t len)
{
    uint64_t ss = ms / 1000;
    uint64_t mm = ss / 60;
    uint32_t hh = (uint32_t)(mm / 60);

    snprintf(buf, len,"%d:%02d:%02d",
				 hh,
        (uint32_t)(mm % 60),
        (uint32_t)(ss % 60));

    return buf;
}