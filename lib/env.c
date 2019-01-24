#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/env.h>
#include <inc/kerelf.h>
#include <inc/sched.h>
#include <inc/pmap.h>


struct Env *envs = NULL;		// All environments
struct Env *curenv = NULL;	        // the current env

static struct Env_list env_free_list;	// Free list

extern Pde *boot_pgdir;
extern char *KERNEL_SP;


/* Overview:
 *  This function is for making an unique ID for every env.
 *
 * Pre-Condition:
 *  Env e is exist.
 *
 * Post-Condition:
 *  return e's envid on success.
 */

u_int mkenvid(struct Env *e)
{
	static u_long next_env_id = 0;

    /* lower bits of envid hold e's position in the envs array. */
	u_int idx = e - envs;

    /*  high bits of envid hold an increasing number. */
	return (++next_env_id << (1 + LOG2NENV)) | idx;
}

/* Overview:
 *  Converts an envid to an env pointer.
 *  If envid is 0 , set *penv = curenv;otherwise set *penv = envs[ENVX(envid)];
 *
 * Pre-Condition:
 *  Env penv is exist,checkperm is 0 or 1.
 *
 * Post-Condition:
 *  return 0 on success,and sets *penv to the environment.
 *  return -E_BAD_ENV on error,and sets *penv to NULL.
 */
int envid2env(u_int envid, struct Env **penv, int checkperm)
{
	struct Env *e;

    /*  If envid is zero, return the current environment.*/
	if (envid == 0) {
		*penv = curenv;
		return 0;
	}

	e = &envs[ENVX(envid)];

	if (e->env_status == ENV_FREE || e->env_id != envid) {
		*penv = 0;
		return -E_BAD_ENV;
	}

    /*  Check that the calling environment has legitimate permissions
     *  to manipulate the specified environment.
     *  If checkperm is set, the specified environment
     *  must be either the current environment.
     *  or an immediate child of the current environment.ok */

    if (checkperm && e != curenv && e->env_parent_id != curenv->env_id){
        *penv = 0;
        return -E_BAD_ENV;
    }
	*penv = e;
	return 0;
}

/* Overview:
 *  Mark all environments in 'envs' as free and insert them into the env_free_list.
 *  Insert in reverse order,so that the first call to env_alloc() return envs[0].
 *  
 * 
 *  You may use these defines to make it:
 *      LIST_INIT,LIST_INSERT_HEAD
 */
void
env_init(void)
{
	int i;
    /*Step 1: Initial env_free_list. */
    printf("in func env_init 111\n");
	LIST_INIT(&env_free_list);
    printf("in func env_init 222\n");
    /*Step 2: Travel the elements in 'envs', init every element(mainly initial its status, mark it as free)
     * and inserts them into the env_free_list as reverse order. */
	for(i=NENV-1;i>=0;i--){
        //printf("in func env_init for loop 333\n");
		envs[i].env_status = ENV_FREE;
		LIST_INSERT_HEAD(&env_free_list,&envs[i],env_link);
	}

}


/* Overview:
 *  Initialize the kernel virtual memory layout for environment e.
 *  Allocate a page directory, set e->env_pgdir and e->env_cr3 accordingly,
 *  and initialize the kernel portion of the new environment's address space.
 *  Do NOT map anything into the user portion of the environment's virtual address space.
 */
/***Your Question Here***/
static int
env_setup_vm(struct Env *e)
{

	int i, r;
	struct Page *p = NULL;
	Pde *pgdir;

    /*Step 1: Allocate a page for the page directory and add its reference.
     *pgdir is the page directory of Env e. */
	if ((r = page_alloc(&p)) < 0) {
		panic("env_setup_vm - page_alloc error\n");
		return r;
	}
	p->pp_ref++;
	pgdir = (Pde *)page2kva(p);
    
    /*Step 2: Zero pgdir's field before UTOP. */
	for (i = 0; i < PDX(UTOP); i++) {
		pgdir[i] = 0;
	}

    /*Step 3: Copy kernel's boot_pgdir to pgdir. */

    /* 
     *  The VA space of all envs is identical above UTOP
     *  (except at VPT and UVPT, which we've set below).
     *  See ./inc/mmu.h for layout.
     *  
     */
	for (i = PDX(UTOP); i <= PDX(~0); i++) {
		pgdir[i] = boot_pgdir[i];
	}
	e->env_pgdir = pgdir;
	e->env_cr3   = PADDR(pgdir);

    /*Step 4: VPT and UVPT map the env's own page table, with
     *different permissions. */
    e->env_pgdir[PDX(VPT)]   = e->env_cr3;
    e->env_pgdir[PDX(UVPT)]  = e->env_cr3 | PTE_V | PTE_R;
	return 0;
}

/* Overview:
 *  Allocates and Initializes a new environment.
 *  On success, the new environment is stored in *new.
 *
 * Pre-Condition:
 *  If the new Env doesn't have parent, parent_id should be zero.
 *  env_init has been called before this function.
 *
 * Post-Condition:
 *  return 0 on success, and set appropriate values for Env new.
 *  return -E_NO_FREE_ENV on error, if no free env.
 *
 * 
 *  You may use these functions and defines:
 *      LIST_FIRST,LIST_REMOVE,mkenvid (Not All)
 *  You should set some states of Env:
 *      id , status , the sp register, CPU status , parent_id
 *      (the value of PC should NOT be set in env_alloc)
 */

int
env_alloc(struct Env **new, u_int parent_id)
{
	int r;
	struct Env *e;
    
    /*Step 1: Get a new Env from env_free_list*/
	if((e=LIST_FIRST(&env_free_list))==NULL){
		printf("Sorry,alloc env failed!\n");
		return -E_NO_FREE_ENV;
	}
    
    /*Step 2: Call certain function(has been implemented) to init kernel memory layout for this new Env.
     *The function mainly maps the kernel address to this new Env address. */
	env_setup_vm(e);

    /*Step 3: Initialize every field of new Env with appropriate values*/
	e->env_parent_id = parent_id;
	e->env_status = ENV_RUNNABLE;
	e->env_id = mkenvid(e);
    /*Step 4: focus on initializing env_tf structure, located at this new Env. 
     * especially the sp register,CPU status. */
    e->env_tf.cp0_status = 0x10001004;
	e->env_tf.regs[29] = USTACKTOP;

    /*Step 5: Remove the new Env from Env free list*/
	*new = e;
	LIST_REMOVE(e,env_link);
	return 0;
}

/* Overview:
 *   This is a call back function for kernel's elf loader.
 * Elf loader extracts each segment of the given binary image.
 * Then the loader calls this function to map each segment
 * at correct virtual address.
 *
 *   `bin_size` is the size of `bin`. `sgsize` is the
 * segment size in memory.
 *
 * Pre-Condition:
 *   bin can't be NULL.
 *    va may NOT aligned 4KB.
 *
 * Post-Condition:
 *   return 0 on success, otherwise < 0.
 */
static int load_icode_mapper(u_long va, uint32_t sgsize,
							 u_char *bin, uint32_t bin_size, void *user_data)
{
	struct Env *env = (struct Env *)user_data;
	struct Page *p = NULL;
	u_long i;
	int r;
	u_long offset = va - ROUNDDOWN(va, BY2PG);
	/*Step 1: load all content of bin into memory. */
	for (i = 0; i < bin_size; i += BY2PG) {
		/*  You should alloc a page and increase the reference count of it. */
		if(page_alloc(&p)<0){
			printf("Sorry,alloc page failed!\n");
			return -E_NO_MEM;
		}
		p->pp_ref++;
		if(i==0) {
			printf("load_icode_mapper kva %x offset %x\n", page2kva(p), offset);
			bcopy(bin,(char *)page2kva(p)+offset,((BY2PG-offset)<bin_size-i)?(BY2PG-offset):(bin_size - i));
		}
		else
			bcopy(bin+i-offset,(char *)page2kva(p),(BY2PG<bin_size-i)?BY2PG:(bin_size-i));
		r = page_insert(env->env_pgdir,p,va+i,PTE_V|PTE_R);
		if(r<0){
			printf("Sorry,insert a page is failed!\n");
			return -E_NO_MEM;
		}

	}
	/*Step 2: alloc pages to reach `sgsize` when `bin_size` < `sgsize`.
    * i has the value of `bin_size` now. */
	while (i < sgsize) {
		if(page_alloc(&p)<0){
			printf("Sorry,alloc page failed!\n");
			return -E_NO_MEM;
		}
		p->pp_ref++;
		r = page_insert(env->env_pgdir,p,va+i,PTE_V|PTE_R);
		if(r<0){
			printf("Sorry,alloc page failed!\n");
			return -E_NO_MEM;
		}
		//bzero(page2kva(p)+offset,BY2PG);
		i+=BY2PG;
	}
	return 0;
}
/* Overview:
 *  Sets up the the initial stack and program binary for a user process.
 *  This function loads the complete binary image by using elf loader,
 *  into the environment's user memory. The entry point of the binary image
 *  is given by the elf loader. And this function maps one page for the
 *  program's initial stack at virtual address USTACKTOP - BY2PG.
 *
 * 
 *  All mappings are read/write including those of the text segment.
 *  may use these :
 *      page_alloc, page_insert, page2kva , e->env_pgdir and load_elf.
 */
static void
load_icode(struct Env *e, u_char *binary, u_int size)
{
	/*  must figure out which permissions will need
	 *  for the different mappings you create.
	 *  Remember that the binary image is an a.out format image,
	 *  which contains both text and data.
     */
	struct Page *p = NULL;
	u_long entry_point;
	u_long r;
    u_long perm;
    
    /*Step 1: alloc a page. */
	if(page_alloc(&p)<0){
		printf("Sorry,alloc page failed!\n");
		return;
	}
    printf("in func load_icode finish step1\n");
	
    /*Step 2: Use appropriate perm to set initial stack for new Env. */
    /* The user-stack should be writable? */
	perm = PTE_V|PTE_R;
	page_insert(e->env_pgdir,p,USTACKTOP-BY2PG,perm);
    printf("in func load_icode finish step2, va %x\n", USTACKTOP-BY2PG);
    /*Step 3:load the binary by using elf loader. */
    r = load_elf(binary,size,&entry_point,e,load_icode_mapper);
	if(r<0){
		printf("Sorry.load entire image failed!\n");
		return;
	}
	printf("in func load_icode finish step3\n");
    /***Your Question Here***/
    /*Step 4:Set CPU's PC register as appropriate value. */
	e->env_tf.pc = entry_point;
    printf("in func load_icode finish step4, pc %x\n", entry_point);
}

/* Overview:
 *  Allocates a new env with env_alloc, loads te named elf binary into 
 *  it with load_icode. This function is ONLY called during kernel
 *  initialization, before running the first user_mode environment.
 *
 * 
 *  this function wrap the env_alloc and load_icode function.
 */
void
env_create(u_char *binary, int size)
{
	struct Env *e;
    /*Step 1: Use env_alloc to alloc a new env. */
	if(env_alloc(&e,0)<0){
		printf("Sorry,env can't create because alloc env failed!\n");
		return;
	}
    printf("in func env_create finish step1\n");
    /*Step 2: Use load_icode() to load the named elf binary. */
	load_icode(e,binary,size);
    printf("in func env_create finish step2\n");
}

/* Overview:
 *  Frees env e and all memory it uses.
 */
void
env_free(struct Env *e)
{
	Pte *pt;
	u_int pdeno, pteno, pa;

    /* Note the environment's demise.*/
	printf("[%08x] free env %08x\n", curenv ? curenv->env_id : 0, e->env_id);

    /* Flush all mapped pages in the user portion of the address space */
	for (pdeno = 0; pdeno < PDX(UTOP); pdeno++) {
        /* only look at mapped page tables. */
		if (!(e->env_pgdir[pdeno] & PTE_V)) {
			continue;
		}
        /* find the pa and va of the page table. */
		pa = PTE_ADDR(e->env_pgdir[pdeno]);
		pt = (Pte *)KADDR(pa);
        /* Unmap all PTEs in this page table. */
		for (pteno = 0; pteno <= PTX(~0); pteno++)
			if (pt[pteno] & PTE_V) {
				page_remove(e->env_pgdir, (pdeno << PDSHIFT) | (pteno << PGSHIFT));
			}
        /* free the page table itself. */
		e->env_pgdir[pdeno] = 0;
		page_decref(pa2page(pa));
	}
    /* free the page directory. */
	pa = e->env_cr3;
	e->env_pgdir = 0;
	e->env_cr3 = 0;
	page_decref(pa2page(pa));
    /* return the environment to the free list. */
	e->env_status = ENV_FREE;
	LIST_INSERT_HEAD(&env_free_list, e, env_link);
}

/* Overview:
 *  Frees env e, and schedules to run a new env 
 *  if e is the current env.
 */
void
env_destroy(struct Env *e)
{
    /* free e. */
	env_free(e);

    /* schedule to run a new environment. */
	if (curenv == e) {
		curenv = NULL;
        /*Why this? */
		// bcopy((void *)KERNEL_SP - sizeof(struct Trapframe),
		// 	  (void *)TIMESTACK - sizeof(struct Trapframe),
		// 	  sizeof(struct Trapframe));
		printf("i am killed ... \n");
		sched_yield();
	}
}

extern void env_pop_tf(struct Trapframe *tf, int id);
extern void lcontext(u_int contxt);

/* Overview:
 *  Restores the register values in the Trapframe with the
 *  env_pop_tf, and context switch from curenv to env e.
 *
 * Post-Condition:
 *  Set 'e' as the curenv running environment.
 *
 * 
 *  You may use these functions:
 *      env_pop_tf and lcontext.
 */
void
env_run(struct Env *e)
{
	/*Step 1: save register state of curenv. */
    /* if there is a environment running,you should do
    *  context switch.You can imitate env_destroy() 's behaviors.*/
	struct Trapframe *old = (struct Trapframe *)(TIMESTACK-sizeof(struct Trapframe));
	printf("curenv %x e %x\n", curenv, e);
	if(curenv){
		bcopy(old,&(curenv->env_tf),sizeof(struct Trapframe));
		//curenv->env_tf.pc += 4;//aim to mips 32
		curenv->env_tf.pc = old->cp0_epc;
		printf("cp0_epc:%x\n",curenv->env_tf.pc);
	}
	printf("id:%d\n",e->env_id);
    /*Step 2: Set 'curenv' to the new environment. */
	curenv = e;
    /*Step 3: Use lcontext() to switch to its address space. */
	printf("KADDR %x env_cr3 %x\n", KADDR(e->env_cr3), e->env_cr3);
	lcontext(KADDR(curenv->env_cr3));	
	printf("in env_run finish lcontext\n");
    /*Step 4: Use env_pop_tf() to restore the environment's
     * environment   registers and drop into user mode in the
     * the   environment.
     */
    /*should use GET_ENV_ASID there. */
    printf("in env_run before env_pop_tf\n");
	env_pop_tf(&(curenv->env_tf),GET_ENV_ASID(curenv->env_id));
	printf("in env_run after env_pop_tf\n");
}

void env_check()
{
        struct Env *temp, *pe, *pe0, *pe1, *pe2;
        struct Env_list fl;
        int re = 0;
        // should be able to allocate three envs
        pe0 = 0;
        pe1 = 0;
        pe2 = 0;
        assert(env_alloc(&pe0, 0) == 0);
        assert(env_alloc(&pe1, 0) == 0);
        assert(env_alloc(&pe2, 0) == 0);
		printf("env_alloc for pe0 pe1 pe2 successed!\n");
        assert(pe0);
        assert(pe1 && pe1 != pe0);
        assert(pe2 && pe2 != pe1 && pe2 != pe0);

        // temporarily steal the rest of the free envs
        fl = env_free_list;
        // now this env_free list must be empty!!!!
        LIST_INIT(&env_free_list);

        // should be no free memory
        assert(env_alloc(&pe, 0) == -E_NO_FREE_ENV);

        
    // recover env_free_list
        env_free_list = fl;

        printf("pe0->env_id %d\n",pe0->env_id);
        printf("pe1->env_id %d\n",pe1->env_id);
        printf("pe2->env_id %d\n",pe2->env_id);

        assert(pe0->env_id == 2048);
        assert(pe1->env_id == 4097);
        assert(pe2->env_id == 6146);
        printf("env_init() work well!\n");

        /* check envid2env work well */
        pe2->env_status = ENV_FREE;
        re = envid2env(pe2->env_id, &pe, 0);

        assert(pe == 0 && re == -E_BAD_ENV);

        pe2->env_status = ENV_RUNNABLE;
        re = envid2env(pe2->env_id, &pe, 0);

        assert(pe->env_id == pe2->env_id &&re == 0);

        temp = curenv;
        curenv = pe0;
        re = envid2env(pe2->env_id, &pe, 1);
        
        //printf("pointer:the envid of e is %x\n", pe);

        assert(pe == 0 && re == -E_BAD_ENV);
        curenv = temp;
        printf("envid2env() work well!\n");

        /* check env_setup_vm() work well */
        printf("pe1->env_pgdir %x\n",pe1->env_pgdir);
        printf("pe1->env_cr3 %x\n",pe1->env_cr3);
    
        printf("pointer:pe2->env_pgdir[PDX(UTOP)] %x\n", pe2->env_pgdir[PDX(UTOP)]);
        printf("pointer:boot_pgdir %x\n", boot_pgdir[PDX(UTOP)]);

        assert(pe2->env_pgdir[PDX(UTOP)] == boot_pgdir[PDX(UTOP)]);
        assert(pe2->env_pgdir[PDX(UTOP)-1] == 0);
        printf("env_setup_vm passed!\n");

        printf("the status actually is: 0x%x\n", pe2->env_tf.cp0_status);

        assert(pe2->env_tf.cp0_status == 0x10001004);
        printf("pe2`s sp register %x\n",pe2->env_tf.regs[29]);
        env_free(pe0);
        env_free(pe1);
        env_free(pe2);
        printf("env_check() succeeded!\n");

}
