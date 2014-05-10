#include <windows.h>

#include "SpamMeNot.h"

void WINAPI startMainThread() {
    SpamMeNot spamMeNot;
    spamMeNot.run();
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved) {
    switch ( dwReason ) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startMainThread, NULL, 0, NULL) == NULL) {
            // We did not successfully create a thread.
            return FALSE;
        }
        break;
    case DLL_PROCESS_DETACH:
        // We should really clean up a bit on detach, but whatever
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}