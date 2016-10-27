.thumb

.global ampatch_start

.global amstub_start
.global amstub_size

ampatch_start: @ Overwrites the start of ampxi_cmd46.
ldr r1, =0x00100001
bx r1
.pool

ampatch_end:
.word 0

.align 2

.thumb

amstub_start: @ Written to am-module 0x00100000.
.word 0, 0 @ Overwritten with the original code from the above am function.
b amstub_codestart
.align 2

amstub_retaddr:
.word 0 @ Overwritten with the addr to return to in the ampxi_cmd46 function.

amstub_codestart:
bx pc
nop

@ pxiam_cmd46 params: inr0=handle* inr1=unused inr2/inr3=u64 titleid insp0=buf* insp4=bufsize insp8=u32 writepos insp12=u8 section insp16=u8 operation
.arm
push {r0, r1, r2, r3}
sub sp, sp, #28
@ Input params on stack at this point can be accessed from sp+0x4c.

ldrb r0, [sp, #0x4c+12] @ section_index, only handle the valid ones.
cmp r0, #5
bcc amstub_finish
cmp r0, #8
bcs amstub_finish

ldr r0, [sp, #0x4c] @ bufptr+= 0x20000, so that it uses the custom savedata.
ldr r1, =0x20000
add r0, r0, r1

mov r3, #1 @ if(((*u32_bufptr) & (1<<((section_index-5)*2)))==0)return;
ldrb r1, [sp, #0x4c+12]
mov r2, #3
sub r1, r1, #5
mul r1, r1, r2
lsl r3, r3, r1
ldr r1, [r0]
str r0, [sp, #20]
tst r1, r3
beq amstub_lpstart

lsl r3, r3, #1 @ Clear bit1 in the section-flags so that the same section doesn't get handled twice.
bic r1, r1, r3
str r1, [r0]

lsl r3, r3, #1 @ Set bit2 in the section-flags indicating that this section-importing was hooked normally, and hence the original input data is available at <override data_addr+0x20000>.
orr r1, r1, r3
str r1, [r0]

add r0, r0, #16

ldr r2, [sp, #0x4c+8] @ writepos
add r0, r0, r2

ldrb r1, [sp, #0x4c+12] @ bufptr += <size of each section from table before the current one> * 2
sub r1, r1, #5
mov r2, #0
b amstub_bufptrcalclp_end

amstub_bufptrcalclp_start:
ldr r3, [sp, #20]
add r3, r3, #4
ldr r3, [r3, r2, lsl #2]
lsl r3, r3, #1
add r0, r0, r3

add r2, r2, #1

amstub_bufptrcalclp_end:
cmp r2, r1
bcc amstub_bufptrcalclp_start

ldrb r1, [sp, #0x4c+12]
sub r1, r1, #5
ldr r3, [sp, #20]
add r3, r3, #4
ldr r3, [r3, r1, lsl #2]

ldr r2, [sp, #0x4c+4] @ r2 = size

ldr r1, [sp, #0x4c] @ r1 = original bufptr

str r0, [sp, #0x4c]

add r0, r0, r3 @ bufptr+= <size of current section from table>

amstub_cpylp: @ Copy the original input data to <override data_addr+section_size> in the buffer.
ldr r3, [r1], #4
str r3, [r0], #4
subs r2, r2, #4
bgt amstub_cpylp

amstub_lpstart:
/*mov r1, #0
str r1, [sp, #24]

amstub_lpbegin: @ Loop through the section flags in the buffer, for the section bit with each section.
ldr r0, [sp, #20]
ldr r0, [r0]
mov r3, #3
mul r2, r1, r3
add r2, r2, #1
mov r3, #1
lsl r3, r3, r2
tst r0, r3
beq amstub_lpnext

bic r0, r0, r3 @ Clear the enable bit so that this doesn't get processed again if this hook runs again.
ldr r2, [sp, #20]
str r0, [r2]

@ Call ampxi_cmd46() with about the same params as if it were done with the main hook functionality with section bit0. Size is loaded from: ((u32*)(flags+4))[loopindex]

ldr r0, [sp, #20]
add r0, r0, #0x10
ldr r2, =0x40000
mul r3, r1, r2
add r0, r0, r3
str r0, [sp, #0] @ buf*

ldr r0, [sp, #20]
add r0, r0, #4
ldr r0, [r0, r1, lsl #2]
str r0, [sp, #4] @ size

mov r3, #0 @ writepos
str r3, [sp, #8]

add r1, r1, #5 @ section
str r1, [sp, #12]

ldrb r0, [sp, #0x4c+16] @ operation
str r0, [sp, #16]

add r0, sp, #28
ldm r0, {r0, r1, r2, r3}
adr ip, amstub_retaddr
ldr ip, [ip]
sub ip, ip, #8
blx ip

amstub_lpnext:
ldr r1, [sp, #24]
add r1, r1, #1
str r1, [sp, #24]
cmp r1, #3
bcc amstub_lpbegin*/

amstub_finish:
add sp, sp, #28
pop {r0, r1, r2, r3}

adr ip, amstub_retaddr
ldr ip, [ip]
bx ip
.pool

amstub_end:
.word 0

amstub_size:
.word amstub_end - amstub_start

