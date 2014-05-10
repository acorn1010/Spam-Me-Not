#include "SpamMeNot.h"

#include <string>

#include "memreadwrite.h"

#include "MemScan.h"
#include "SpamFilter.h"

// ASM DEFINITIONS
#define JMP(frm,to) (((int)(to) - (int)(frm))-5)

namespace {
    struct ChatMessage {
        char *message; // Japanese characters start with E3 it seems?  I'm not sure what encoding this is, but we can just treat it like chars...
        unsigned int unknown; // Value of 0x100?
        int length; // Length of message + 1 for NULL
    };

    SpamFilter spamFilter;
    DWORD logTellMessageFunction = 0;
    DWORD logTellMessageReturn = 0;

    bool __stdcall logTellMessage(ChatMessage *message);
    bool __stdcall logTellMessageTry(ChatMessage *message);

    // This seems to also be logging linkshell messages.
    void __declspec(naked) logTellMessageHook() {
        __asm {
            push ebp
            mov ebp, esp
            pushad
          
            mov eax, [ebp+0x10]
            push eax
            call logTellMessageTry
            cmp eax, 1

            je skipTellMessage
            popad

            // Original instructions
            sub esp, 0x58 // sub esp, 88
            jmp dword ptr [logTellMessageReturn]

skipTellMessage:
            popad
            mov esp, ebp
            pop ebp
            ret 0x18
        }
    }

    bool __stdcall logTellMessageTry(ChatMessage *message) {
        bool result = false;

  //      __try {
            result = logTellMessage(message);
/*        } __except(EXCEPTION_EXECUTE_HANDLER) {
        }*/

        return result;
    }

    /**
     * Returns true if the message should be blocked, else false.
     */
    bool __stdcall logTellMessage(ChatMessage *message) {
        return spamFilter.isSpam(std::string(message->message, message->length - 1));
    }
};

void SpamMeNot::run() {
    init();
}

void SpamMeNot::init() {
    // Enable hooks
    //55 8b ec 83 ec ?? a1 ?? ?? ?? ?? 33 c5 89 45 fc 53 8b 5d 10 56 8b 75 0c 57 8b f9
    Scanner scanner;
    logTellMessageFunction = scanner.GetAddressFromAoB(
        "\x55\x8b\xec\x83\xec\x00\xa1\x00\x00\x00\x00\x33\xc5\x89\x45\xfc\x53\x8b\x5d\x10\x56\x8b\x75\x0c\x57\x8b\xf9",
        "xxxxx?x????xxxxxxxxxxxxxxxx", 1, 0x00101000, 0x7FFFFFFF);
    logTellMessageReturn = logTellMessageFunction + 6;
    BYTE logTellMessageArr[] = {0xE9, 0, 0, 0, 0};
    *(DWORD *)(logTellMessageArr+1) = JMP(logTellMessageFunction, logTellMessageHook);
    WriteAddress((LPVOID)logTellMessageFunction, logTellMessageArr, sizeof(logTellMessageArr));
}