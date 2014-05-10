#ifndef __MEMREADWRITE_H
#define __MEMREADWRITE_H
#include <vector>

#include <Windows.h>

#define ReadPointer(x, y, ...) _ReadPointer(x, y, __VA_ARGS__, OFFSET_END)
#define ReadAddress(y, ...) _ReadAddress(y, __VA_ARGS__)

extern const int OFFSET_END;

inline BOOL IsPageReadable(PMEMORY_BASIC_INFORMATION mbi);

BOOL _ReadPointer(PULONG_PTR pulValue, LPCVOID lpcvBase, ...);

// convenience function which returns the data instead of TRUE/FALSE
DWORD ReadAddress( LPCVOID address, ... );

// Determines if the given address can be read from
bool IsReadable( LPCVOID address );

bool WriteAddress(__in LPVOID lpcvBase, __in LPCVOID lpcvWriteValue, __in size_t uSize);

#endif //__MEMREADWRITE_H
