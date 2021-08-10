#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(int status)
{
  myproc()->exitStatus=status;
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  myproc()->hsize+=n;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Print hello world on the console
int 
sys_helloWorld(void)
{
  cprintf("Hello World\n");  
  return 0;
}

// Return number of files open by the current process
int 
sys_numOpenFiles(void)
{
  int count=0;
  for(int i=0;i<16;i++)
  if(myproc()->ofile[i]!=0)
    count++;

  return count;
}

// Return current size of heap memory allocated to the process 
int 
sys_memAlloc(void)
{
  return myproc()->hsize;
}
// Print different stages of process context switches
int 
sys_getprocesstimedetails(void)
{
  struct rtcdate createdDT=myproc()->creationDT;
  struct rtcdate lastContextInDT=myproc()->lastContextInDT;
  struct rtcdate lastContextOutDT=myproc()->lastContextOutDT;
 
  // if we get any negative value
  if(createdDT.second<0||createdDT.minute<0||createdDT.hour<0||createdDT.day<0||createdDT.month<0||createdDT.year<0
    ||lastContextInDT.second<0||lastContextInDT.minute<0||lastContextInDT.hour<0||lastContextInDT.day<0||lastContextInDT.month<0||lastContextInDT.year<0
    ||lastContextOutDT.second<0||lastContextOutDT.minute<0||lastContextOutDT.hour<0||lastContextOutDT.day<0||lastContextOutDT.month<0||lastContextOutDT.year<0)
    return -1;
  
  cprintf("processCreationDateTime: %d:%d:%d %d:%d:%d\n",createdDT.second,createdDT.minute,createdDT.hour,createdDT.day,createdDT.month,createdDT.year);
  cprintf("processLastContextSwitchedOutDateTime: %d:%d:%d %d:%d:%d\n",lastContextOutDT.second,lastContextOutDT.minute,lastContextOutDT.hour,lastContextOutDT.day,lastContextOutDT.month,lastContextOutDT.year);
  cprintf("processLastContextSwitchedInDateTime: %d:%d:%d %d:%d:%d\n",lastContextInDT.second,lastContextInDT.minute,lastContextInDT.hour,lastContextInDT.day,lastContextInDT.month,lastContextInDT.year);
  return 1;
}