# FreeRTOS ported to plain AI32 (x86)

This is a proof of concept for running FreeRTOS in userspace on x86. This means:
 - Completely deterministic
 - No linux systemcalls (except for ```_exit()```)
 - No dependency on system timers

## How does it work?

First,  FreeRTOS  was  ported  as  normal  by  implementing  ```portmacro.h```,
```port.c```   and   ```port.asm```  files.  The   key   functions   here   are
```vPortYield()``` and ```vPortTickISR()```.

```vPortTickISR()```  would   normally   be  called  periodically  by  a  timer
interrupt, and  normally, on micro controllers, the timer interrupt's frequency
would be derived either directly  from  the  system  clock, or at least be some
multiple of the  system clock. In other words, the number of instruction cycles
that can be executed per tick is a fixed number.

This presents a problem for a linux implementation, because  the  ```clock()```
system calls are far from  deterministic,  and  certainly don't have any direct
connection to how many instructions run per period of time.  This  is made more
uncertain  by  the  linux scheduler swapping in/out other tasks on the  system.
Yes,  your  Teams Meeting will influence your FreeRTOS  simulation  in  a  very
significant way.

The  solution  used  in  this  proof  of  concept  is  to  create  a  so-called
```InstructionCallback()``` function.  Calls to this function are then inserted
into the code under test at periodic intervals. The ```InstructionCallback()```
function can then essentially be seen as the "CPU clock". From this we can then
derive a clock frequency for  our  tick  timer,  and  call ```vPortTickISR()```
after making the appropriate number of divisions.

Because  ```InstructionCallback()```  can  be  inserted  anywhere,  a  wrapper,
```InstructionCallback_Safe()```, needed to be written in assembly which  makes
sure to  push  and  restore  all state, so that we don't clobber any registers.

## How to build

You may need to ```sudo apt install nasm``` first.

```sh
cmake --preset default
cmake --build build-Debug/
./build-Debug/Harness
```

