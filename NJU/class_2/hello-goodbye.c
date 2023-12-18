#include <stdio.h>

__attribute__((constructor)) void hello() {
    printf("Hello,World\n");
}

__attribute__((destructor)) void goodbye() {
    printf("Goodbye,Cruel OS World!\n");
}

int main(){
    printf("middle!\n");
}
