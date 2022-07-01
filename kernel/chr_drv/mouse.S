#include <linux/config.h>

.text
.globl mouse_interrupt



mouse_interrupt:
    pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	push %ds
	push %es
    movl $0x10,%eax
    mov %ax,%ds
    mov %ax,%es
    xor %eax,%eax
    inb $0x60,%al
    pushl %eax
    call readmouse
    addl $4,%esp
    
    movb $0x20,%al   /* 8259A执行操作命令字EOI */
    outb %al,$0xA0   /* 向8259A从片发送EOI */
    outb %al,$0x20   /* 向8259A主片发送EOI */

    pop %es
    pop %ds
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
    iret
