#pragma once
#include <oni/utils/types.h>

uint8_t SelfElevateAndRun(uint8_t* userlandPayload, uint32_t userlandSize, void(*elevatedEntryPoint)(void* arguments));
