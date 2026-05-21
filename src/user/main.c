#include "libuser.h"

void user_main() {
    int answer = 42;
    int negative = -1337;
    char* name = "cumpile";
    
    printf("Hello from user mode! (using printf)\n");
    printf("OS Name: %s\n", name);
    printf("Decimal answer: %d\n", answer);
    printf("Negative answer: %d\n", negative);
    printf("Hexadecimal answer: 0x%x\n", answer);
    printf("Character test: %c%c%c\n", 'O', 'K', '!');
    printf("Unsigned test: %u, Percent: %%\n", 1234567890);

    while (1) {
        asm volatile("pause"); // pause is safe in ring 3
    }
}
