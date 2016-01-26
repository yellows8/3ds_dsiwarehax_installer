.arm

.global initsrv_allservices

initsrv_allservices: @ Initialize a srv handle which has access to all services.
push {r4, r5, r6, lr}
sub sp, sp, #0x4

mov r0, sp
ldr r1, =0xffff8001
bl svcGetProcessId

cmp r0, #0
bne initsrv_allservices_end

mov r4, #0
ldr r5, [sp, #0]
mov r6, #0

adr r0, kernelmode_searchval_overwrite @ r4=address(0=cur kprocess), r5=searchval, r6=val to write
bl svcBackdoor @ Overwrite kprocess PID with 0.

mov r4, r3
ldr r5, [sp, #0]

bl srvExit
bl srvInit

adr r0, kernelmode_writeval @ r4=addr, r5=u32val
bl svcBackdoor @ Restore the original PID.

mov r0, #0

initsrv_allservices_end:
add sp, sp, #0x4
pop {r4, r5, r6, pc}
.pool

kernelmode_searchval_overwrite: @ r4=kprocess, r5=searchval, r6=val to write. out r3 = overwritten addr.
cpsid i @ disable IRQs
push {r4, r5, r6}

cmp r4, #0
bne kernelmode_searchval_overwrite_lp
ldr r4, =0xffff9004
ldr r4, [r4]

kernelmode_searchval_overwrite_lp:
ldr r0, [r4]
cmp r0, r5
addne r4, r4, #4
bne kernelmode_searchval_overwrite_lp

str r6, [r4]
mov r3, r4
pop {r4, r5, r6}
bx lr
.pool

kernelmode_writeval: @ r4=addr, r5=u32val
cpsid i @ disable IRQs
str r5, [r4]
bx lr

