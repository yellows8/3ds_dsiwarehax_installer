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

.arm
push {r0, r1, r2, r3}
@bkpt #0
pop {r0, r1, r2, r3}

adr r1, amstub_retaddr
ldr r1, [r1]
bx r1

amstub_end:
.word 0

amstub_size:
.word amstub_end - amstub_start

