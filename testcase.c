#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
int main(void)
{
    printf(1,"%d",numOpenFiles());
    exit(99);
}