#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
int main(void)
{
    int staus;
    int tt = fork();
    int pid = wait(&staus);
    if (tt != 0)
    {
        printf(1, " status of child : %d , PID: %d\n", staus, pid);
    }
    if (tt == 0)
    {

        exit(69);
    }
    exit(99);
}