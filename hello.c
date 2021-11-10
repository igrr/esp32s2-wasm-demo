#include <stdio.h>

extern void delay_ms(int ms);

int main(void)
{
    puts("Hello world\n");
    delay_ms(1000);
    puts("Webassembly done\n");
    return 0;
}
