#include <inc/asm/regdef.h>
#include <inc/asm/cp0regdef.h>
#include <inc/asm/asm.h>
#include <inc/stackframe.h>


.macro	__build_clear_sti
	STI
.endm

.macro	__build_clear_cli
	CLI
.endm

.macro	BUILD_HANDLER exception handler clear
	.align	5
	NESTED(handle_\exception, TF_SIZE, sp)
	.set	noat

nop

	SAVE_ALL
	__build_clear_\clear
	.set	at
	move	a0, sp
	jal	\handler
	nop
	j	ret_from_exception
	nop
	END(handle_\exception)
.endm

FEXPORT(ret_from_exception)
	.set noat
	.set noreorder
	RESTORE_SOME
	.set at
	lw	k0,TF_EPC(sp)
	lw	sp,TF_REG29(sp) /* Deallocate stack */
//1:	j	1b
	nop
	eret


///////////////////////// HANDLE INT ////////////////////////////
.set noreorder
.align	5
NESTED(handle_int, TF_SIZE, sp)
.set	noat

//1: j 1b
nop

SAVE_ALL
CLI
.set	at
/* jump to timer_irq after check */
mfc0	t0, CP0_CAUSE
mfc0	t2, CP0_STATUS
and		t0, t2
andi	t1, t0, STATUSF_IP2
bnez	t1, timer_irq
nop
END(handle_int)

////////////////////////////TIMER IRQ/////////////////////////////
	.extern delay
timer_irq:
//1:	j	sched_yield
//	nop
	nop
	jal clear_timer0_int /* clean up timer interrupt */
	nop
	jal	mytest_irq /* jump to mytest_irq to print something */
	nop
	j sched_yield
	nop
	/*li t1, 0xff
	lw    t0, delay
	addu  t0, 1
	sw	t0, delay
	beq	t0,t1,1f	
	nop*/
	j	ret_from_exception
	nop

////////////////////RESERVED EXCEPTION HANDLER/////////////////////
LEAF(do_reserved)
END(do_reserved)

///////////////////TLB REFILL EXCEPTION HANDLER///////////////////
	.extern tlbra
	.set	noreorder
NESTED(do_refill,0 , sp)
/**
 * 暂时留着用的代码，看看他的实现思路
 * 最后跳过这一段，加上jal my_tlb_refill去做我们自己的重填
 */
	nop

	//jal my_tlb_refill
	nop
 	.extern	mCONTEXT
	nop
    lui   k1,%hi(mCONTEXT)          //当前进程的PGD表地址高位

mfc0  k0,CP0_BADVADDR       //取TLB MISS vaddr地址

lw    k1,%lo(mCONTEXT)(k1)  //(页表地址高位|低位) -> k1

srl   k0,k0,22                           //k0 右移22 bit;即pgd = 高10bit

sll   k0,k0,2                             //k0 左移2bit;因为每项占４个字节,所以pgd*4

addu  k1,k1,k0                       //k1=pgd_current+pgd_index*4;

mfc0 k0,CP0_CONTEXT              //context -> k0

lw   k1,0(k1)                            //k1<- pgd_current[pgd_index];即PTE表入口

srl   k0,k0,1         //k0右移1bit ,因为一次映射2页,去掉1bit

andi   k0,k0,0xff8     //k0取9bit ;后面还有3bit,因为每项8BYTE

addu  k1,k1,k0                        //pte_base + pte_index

lw k0,0(k1)                           //k0<- pte_base(pte_index):           entry0

lw k1,4(k1)       // k1<- pte_base(pte_index) +４:    entry1 lw k1,sizeof(pte_t)(k1) 

srl k0,k0,6                            // k0右移6bit; 后6bit是软件位,不填到TLB里

mtc0 k0,CP0_ENTRYLO0

srl k1,k1,6

mtc0 k1,CP0_ENTRYLO1                                 

ehb               //ehb的gcc实现 sll    zero,zero,0x3

tlbwr                                 //写入TLB
nop
eret
	nop
END(do_refill)



BUILD_HANDLER reserved do_reserved cli
BUILD_HANDLER tlb	do_refill	cli
BUILD_HANDLER mod	page_fault_handler cli
