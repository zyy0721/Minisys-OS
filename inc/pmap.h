#ifndef _PMAP_H_
#define _PMAP_H_

#include <inc/types.h>
#include <inc/queue.h>
#include <inc/mmu.h>
#include <inc/printf.h>


LIST_HEAD(Page_list, Page);
typedef LIST_ENTRY(Page) Page_LIST_entry_t;

// 在 MIPS CPU 中，地址转换以 4KB 大小为单位，称为页. 我们使用 Page 结构体
// 来作为记录一页内存的相关信息的数据结构
struct Page {
    // pp_link 是当前节点指向链表中下一个节点的指针，其类型为 LIST_ENTRY(Page)
    Page_LIST_entry_t pp_link;      /* free list link */

    // Ref is the count of pointers (usually in page table entries)
    // to this page.  This only holds for pages allocated using
    // page_alloc.  Pages allocated at boot time using pmap.c's "alloc"
    // do not have valid reference count fields.

    // pp_ref 用来记录这一物理页面的引用次数
    u_short pp_ref;
};

extern struct Page *pages;

void mips_detect_memory();
void mips_vm_init();
void mips_init();
void page_init(void);
void page_check();
int page_alloc(struct Page **pp);
void page_free(struct Page *pp);
void page_decref(struct Page *pp);
int pgdir_walk(Pde *pgdir, u_long va, int create, Pte **ppte);
int page_insert(Pde *pgdir, struct Page *pp, u_long va, u_int perm);
struct Page *page_lookup(Pde *pgdir, u_long va, Pte **ppte);
void page_remove(Pde *pgdir, u_long va) ;
void tlb_invalidate(Pde *pgdir, u_long va);
void tlb_out(u_int entryhi);
void boot_map_segment(Pde *pgdir, u_long va, u_long size, u_long pa, int perm);

inline u_long page2ppn(struct Page *pp);
inline u_long page2pa(struct Page *pp);
inline u_long page2kva(struct Page *pp);
inline u_long va2pa(Pde *pgdir, u_long va);
struct Page *pa2page(u_long pa);

#endif /* _PMAP_H_ */
