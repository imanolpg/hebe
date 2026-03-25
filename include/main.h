#pragma once

constexpr bool isDebug =
#ifdef HEBE_DEBUG
    true;
#else
    false;
#endif

#define EXPORT_IR_CODE false