#include "syscall.h"

#define _syscall0(NUMBER) ({\
        int retval;         \
        asm volatile(       \
        "int $0x80;"        \
        : "=a"(retval)      \
        : "0"(NUMBER)       \
        );                  \
        retval;             \
    })

#define _syscall1(NUMBER,ARG1) ({       \
        int retval;                     \
        asm volatile(                   \
        "int $0x80;"                    \
        : "=a"(retval)                  \
        : "0"(NUMBER),"b"(ARG1)         \
        );                              \
        retval;                         \
    })

#define _syscall2(NUMBER,ARG1,ARG2) ({       \
        int retval;                     \
        asm volatile(                   \
        "int $0x80;"                    \
        : "=a"(retval)                  \
        : "0"(NUMBER),"b"(ARG1),"c"(ARG2)   \
        );                              \
        retval;                         \
    })

#define _syscall3(NUMBER,ARG1,ARG2,ARG3) ({       \
        int retval;                     \
        asm volatile(                   \
        "int $0x80;"                    \
        : "=a"(retval)                  \
        : "0"(NUMBER),"b"(ARG1),"c"(ARG2),"d"(ARG3) \
        );                              \
        retval;                         \
    })

uint_32 getpid(void) {
    return _syscall0(SYS_GETPID);
}
