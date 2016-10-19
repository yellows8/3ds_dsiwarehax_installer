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
@ Input params on stack at this point can be accessed from sp+0x30.

ldrb r0, [sp, #0x30+12] @ section_index. Only handle "public.*".
cmp r0, #5
bcc amstub_finish
cmp r0, #6
bcs amstub_finish

ldr r0, [sp, #0x30] @ bufptr+= 0x20000, so that it uses the custom savedata.
ldr r2, [sp, #0x30+8] @ writepos
ldr r1, =0x20000
add r0, r0, r1
add r0, r0, r2
str r0, [sp, #0x30]

amstub_finish:
pop {r0, r1, r2, r3}

adr r1, amstub_retaddr
ldr r1, [r1]
bx r1
.pool

amstub_end:
.word 0

amstub_size:
.word amstub_end - amstub_start

