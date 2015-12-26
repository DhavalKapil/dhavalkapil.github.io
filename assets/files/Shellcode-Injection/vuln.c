/**
 *  Vulnerable program to demonstrate shellcode injection
 *
 *  https://dhavalkapil.com/blogs/Shellcode-Injection/
 */

#include <stdio.h>
#include <string.h>

void func(char *name)
{
    char buf[100];
    strcpy(buf, name);
    printf("Welcome %s\n", buf);
}

int main(int argc, char *argv[])
{
    func(argv[1]);
    return 0;
}
