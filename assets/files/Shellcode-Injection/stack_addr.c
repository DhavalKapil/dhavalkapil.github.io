/**
 *  Printing the address of stack
 *
 *  https://dhavalkapil.com/blogs/Shellcode-Injection/
 */

#include <stdio.h>

int main()
{
    int a;
    printf("%p\n", &a);
    return 0;
}
