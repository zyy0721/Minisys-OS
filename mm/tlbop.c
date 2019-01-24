#include <inc/tlbop.h>
#include <inc/m32c0.h>
#include <inc/printf.h>
#include <inc/mmu.h>

void print_tlb() {
    u32 tlb_size = mips_tlb_size();
    printf("TLB size: %d\n", tlb_size);
    unsigned int i;
    tlbhi_t phi; tlblo_t plo0, plo1; unsigned pmsk;
    for (i = 0; i < tlb_size; ++i) {
        mips_tlbri2(&phi, &plo0, &plo1, &pmsk, i);
        printf("%d hi %x lo0 %x lo1 %x pmsk %x\n", i, phi, plo0, plo1, pmsk);
    }
}
extern Pde *boot_pgdir;
extern int mCONTEXT;
void my_tlb_refill() {

    /**
     * 
     * 页目录在 mCONTEXT
     * 要重填的虚拟地址是在 BadVAddr
     * 通过查找页目录来看虚拟地址对应的物理地址，然后填入TLB，用封装tlb库
     */
    u32 badvaddr = mips32_get_c0(C0_BADVADDR);
    u32 pa = (va2pa((Pde *)mCONTEXT, badvaddr) | 0x80000000);

    /* TODO: 判断NOPAGE等情况 */

    printf("In my_tlb_fill badvaddr %x pa %x\n", badvaddr, pa);
    /* !!!NOTICE!!! TLB被设置但是地址转换失败 */
    mips_tlbrwr2(badvaddr, pa | 0b110, 0, 0);

    /* 看一看我们的重填结果 */
    print_tlb();
}