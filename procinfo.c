#include "types.h"
#include "stat.h"
#include "user.h"
int main(int argc, char **argv)
{
  if(argc < 2){
    printf(2, "usage: procinfo pid...\n");
    exit(-1);
  }
  procinfo(atoi(argv[1]));
  exit(0);
}