#include "string.h"
#include "debug.h"

void memset(void* dst_,uint_8 value,uint_32 size)
{
    ASSERT(dst_ != NULL);
    uint_8* p = dst_;
    while (size--){
        *p = value;
        p++;
    }
}

void memcpy(void* dst_,void* src_,uint_32 size)
{
    ASSERT(dst_ != NULL && src_ != NULL);
    uint_8* dst = dst_;
    const uint_8* src = src_;
    while (size--){
        *dst++ = *src++;
    }
}

int memcmp(const void* a_,const void* b_,uint_32 size)
{
    ASSERT(a_ != NULL && b_ != NULL);
    const uint_8* a = a_;
    const uint_8* b = b_;
    while (size--) {
        if (*a != *b){
            return *a > *b ? 1:-1;
        }
        a++;b++;
    }
    return 0;
}

char* strcpy(char* dst_,const char* src_)
{
    ASSERT(dst_ != NULL && src_ != NULL);
    char* ret = dst_;
    while ((*dst_++ = *src_++)) ;   //这代码有点妙
    return ret;
}

uint_32 strlen(const char* str)
{
    ASSERT(str != NULL);
    const char* p = str;
    while (*p) p++;
    return (p-str);
}

int_8 strcmp(const char* a,const char* b)
{
    ASSERT(a != NULL && b != NULL);
    while (*a != 0 && *a == *b){
        a++;
        b++;
    }
    return *a < *b ? -1: *a>*b;
}

char* strchr(const char* str,uint_8 ch)
{
    ASSERT(str != NULL);
    while(*str) {
        if (*str++ == ch)
            return (char*)str;
    }
    return NULL;
}

char* strrchr(const char* str,char ch)
{
    ASSERT(str != NULL);
    char* ret = NULL;
    while(*str) {
        if (*str == ch)
            ret = (char*)str;
        str++;
    }

    return ret;
}

char* strcat(char* dst_,const char* src_)
{
    ASSERT(dst_ != NULL && src_ != NULL);
    char* ret = dst_;
    while(*dst_++) ;
    dst_--;
    while((*dst_++ = *src_++)) ;
    return ret;
}


uint_32 strchrs(const char* str,uint_8 ch)
{
    ASSERT(str != NULL);
    uint_32 cnt = 0;
    while (*str)
    {
        if (*str == ch)
            cnt++;
        str++;
    }
    return cnt;
}
