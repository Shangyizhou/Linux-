可通过man 7 signal查看帮助文档获取。也可查看/usr/src/linux-headers-3.16.0-30/arch/s390/include/uapi/asm/signal.h

Signal      Value   Action  Comment

────────────────────────────────────────────

SIGHUP     1    Term  Hangup detected on controlling terminal or death of controlling process

SIGINT     2    Term  Interrupt from keyboard

SIGQUIT    3    Core  Quit from keyboard

SIGILL     4    Core  Illegal Instruction

SIGFPE     8    Core  Floating point exception

SIGKILL    9    Term  Kill signal

SIGSEGV    11   Core  Invalid memory reference

SIGPIPE    13   Term  Broken pipe: write to pipe with no readers

SIGALRM    14   Term  Timer signal from alarm(2)

SIGTERM    15   Term  Termination signal

SIGUSR1 30,10,16  Term   User-defined signal 1

SIGUSR2 31,12,17  Term  User-defined signal 2

SIGCHLD 20,17,18  Ign   Child stopped or terminated

SIGCONT 19,18,25  Cont  Continue if stopped

SIGSTOP 17,19,23  Stop  Stop process

SIGTSTP 18,20,24  Stop  Stop typed at terminal

SIGTTIN 21,21,26  Stop  Terminal input for background process

SIGTTOU 22,22,27  Stop  Terminal output for background process

The signals SIGKILL and SIGSTOP cannot be caught, blocked, or ignored.      

在标准信号中，有一些信号是有三个“Value”，第一个值通常对alpha和sparc架构有效，中间值针对x86、arm和其他架构，最后一个应用于mips架构。一个‘-’表示在对应架构上尚未定义该信号。

不同的操作系统定义了不同的系统信号。因此有些信号出现在Unix系统内，也出现在Linux中，而有的信号出现在FreeBSD或Mac OS中却没有出现在Linux下。这里我们只研究Linux系统中的信号。
