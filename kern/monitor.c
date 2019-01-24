#include "monitor.h"

// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/printf.h>
#include <inc/string.h>
#include <inc/mmu.h>
#include <drivers/uart.h>
#include "fs.h"


#include <kern/console.h>
#include <inc/tlbop.h>


#define CMDBUF_SIZE	80	// enough for one VGA text line

struct Command {
    const char *name;
    const char *desc;
    // return -1 to force monitor to exit
    int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
    { "help", "Display this list of commands", mon_help },
    { "kerninfo", "Display information about the kernel", mon_kerninfo },
    { "tlb", "Display 16 tlb table entries", mon_tlb },
    { "sw", "Display the value of switches", mon_sw },
    { "play", "Simulate a simple electric piano", mon_play },
    { "game", "A simple MineSweeper game", mon_game },
    { "ls", "List files and directories", mon_ls },
    { "touch", "Create file", mon_touch },
    { "mkdir", "Create directory", mon_mkdir },
    { "rm", "Delete files or directories", mon_rm },
    { "about", "Display developers' name of this project", mon_about },

};


/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(commands); i++)
        printf("%s - %s\n", commands[i].name, commands[i].desc);
    return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
    extern char _start[], entry[], etext[], edata[], end[];
    extern u_long npage;
    extern Pde *boot_pgdir;
    printf("Special kernel symbols:\n");
    printf("    KERNBASE is %x \n",KERNBASE);
    printf("****Virtual page table**** \n");
    printf("    VPT is %x \n",VPT);
    printf("    KSTACKTOP is %x \n",KSTACKTOP);
    printf("    KSTKSIZE is %d \n",KSTKSIZE);
    printf("    ULIM is %x \n",ULIM); 
    printf("    UVPT is %x\n",UVPT);
    printf("    UPAGES is %x\n",UPAGES);
    printf("    UENVS is %x\n",UENVS);
    printf("    UTOP is %x\n",UTOP);
    printf("    UXSTACKTOP is %x\n",UXSTACKTOP);
    printf("    TIMESTACK is %x\n",TIMESTACK);
    printf("    USTACKTOP is %x\n",USTACKTOP);
    printf("    UTEXT is %x\n",UTEXT);

    printf("  _start     %x\n", _start);
    printf("  entry      %x\n", boot_pgdir);
    printf("  end        %x \n", end);
    printf(" the count of page is %d\n",npage);
    printf(" %d bytes to a page \n",BY2PG);
    printf(" %d bytes mapped by a page directory entry \n",PDMAP);

    return 0;
}

/***** printf tlb info*****/
int
mon_tlb(int argc, char **argv, struct Trapframe *tf)
{
    print_tlb();
    return 0;
}

/***** Display the value of switches *****/
int
mon_sw(int argc, char **argv, struct Trapframe *tf)
{
    int sw;
    sw = get_switches();
    printf("switches' value is 0x%x\n",sw);
    return 0;
}

/***** stimulate a simple electric piano *****/
int 
mon_play(int argc, char **argv, struct Trapframe *tf)
{
    char *buf;
    printf("*****input x to exit*******\n");
    while (1) {
        buf = readline("Play> ");
        if (buf != NULL&& *buf!='x')
        {
            set_seven_seg_value(*buf);
            set_leds(*buf);
            switch(*buf-48)
            {
                case 0:
                delay_zero();
                printf("pause~\n");
                break;
                case 1:
                delay_do();
                printf("do~\n");
                break;
                case 2:
                delay_re();
                printf("re~\n");
                break;
                case 3:
                delay_mi();
                printf("mi~\n");
                break;
                case 4:
                delay_fa();
                printf("fa~\n");
                break;
                case 5:
                delay_so();
                printf("so~\n");
                break;
                case 6:
                delay_la();
                printf("la~\n");
                break;
                case 7:
                delay_xi();
                printf("xi~\n");
                break;                
            }
            printf("buf is %c\n",*buf);
            //break;
        }
        if(*buf=='x')
        {
            printf("*****thanks for playing this naive game(#^.^#)*******\n");
            break;
        }    

    }
    return 0;
}


/***** A simple MineSweeper game ******/
void menu()
{
	printf("******************************\n");
	printf("******1.play     0.exit*******\n");
	printf("******************************\n");
}
#define MAXFORGAME 10

//打印棋盘
void display(char arr[])
{
    int i=0;
   
    for(;i<10;i++){
        printf("%c ",arr[10*i+0]);
        printf("%c ",arr[10*i+1]);
        printf("%c ",arr[10*i+2]);
        printf("%c ",arr[10*i+3]);
        printf("%c ",arr[10*i+4]);
        printf("%c ",arr[10*i+5]);
        printf("%c ",arr[10*i+6]);
        printf("%c ",arr[10*i+7]);
        printf("%c ",arr[10*i+8]);
        printf("%c ",arr[10*i+9]);
        printf("\n");
    }
}

//扫描雷
int  get_boom(char arr[], int x, int y)//计算周围八个位置雷的个数
{
    int num;
    if(x == 1){
        if(y == 1){
            num = arr[10 * x + (y + 1)] + arr[10 * (x + 1) + y] + arr[10 * (x + 1) + (y + 1)]
                  - 3 * '0';
        }else if(y == 9){
            num = arr[10 * x + (y - 1)] + arr[10 * (x + 1) + (y - 1)] + arr[10 * (x + 1) + y]
                  - 3 * '0';
        }else{
            num = arr[10 * x + (y - 1)] + arr[10 * x + (y + 1)]
                  + arr[10 * (x + 1) + (y - 1)] + arr[10 * (x + 1) + y] + arr[10 * (x + 1) + (y + 1)]
                  - 5 * '0';
        }
    }else if(x == 9){
        if(y == 1){
            num = arr[10 * (x - 1) + y] + arr[10 * (x - 1) + (y + 1)] + arr[10 * x + (y + 1)]
                  - 3 * '0';
        }else if(y == 9){
            num = arr[10 * (x - 1) + (y - 1)] + arr[10 * (x - 1) + y] + arr[10 * x + (y - 1)]
                  - 3 * '0';
        }else{
            num = arr[10 * (x - 1) + (y - 1)] + arr[10 * (x - 1) + y] + arr[10 * (x - 1) + (y + 1)]
                  + arr[10 * x + (y - 1)] + arr[10 * x + (y + 1)]
                  - 5 * '0';
        }
    }else{
        if(y == 1){
            num = arr[10 * (x - 1) + y] + arr[10 * (x - 1) + (y + 1)] + arr[10 * x + (y + 1)]
                  + arr[10 * (x + 1) + y] + arr[10 * (x + 1) + (y + 1)]
                  - 5 * '0';
        }else if(y == 9){
            num = arr[10 * (x - 1) + (y - 1)] + arr[10 * (x - 1) + y] + arr[10 * x + (y - 1)]
                  + arr[10 * (x + 1) + (y - 1)] + arr[10 * (x + 1) + y]
                  - 5 * '0';
        }else{
            num = arr[10 * (x - 1) + (y - 1)] + arr[10 * (x - 1) + y] + arr[10 * (x - 1) + (y + 1)]
                  + arr[10 * x + (y - 1)] + arr[10 * x + (y + 1)]
                  + arr[10 * (x + 1) + (y - 1)] + arr[10 * (x + 1) + y] + arr[10 * (x + 1) + (y + 1)]
                  - 8 * '0';
        }
    }
    return num;
}

//对某一点进行扩展
void fun(char arr[], char show[], int x, int y)
{
    if ((x >= 1) && (x <= 9) && (y >= 1) && (y <= 9))//递归约束条件
    {
        if (get_boom(arr, x, y) == 0)//判断雷的个数
        {
            show[10 * x + y] = ' ';
            if (show[10 * (x - 1) + (y - 1)] == '*')//对周围八个位置分别进行递归
                fun(arr, show, x - 1, y - 1);
            if (show[10 * (x - 1) + y] == '*')
                fun(arr, show, x - 1, y);
            if (show[10 * (x - 1) + (y + 1)] == '*')
                fun(arr, show, x - 1, y + 1);
            if (show[10 * x + (y - 1)] == '*')
                fun(arr, show, x, y - 1);
            if (show[10 * x + (y + 1)] == '*')
                fun(arr, show, x, y + 1);
            if (show[10 * (x + 1) + (y - 1)] == '*')
                fun(arr, show, x + 1, y - 1);
            if (show[10 * (x + 1) + y] == '*')
                fun(arr, show, x + 1, y);
            if (show[10 * (x + 1) + (y + 1)] == '*')
                fun(arr, show, x + 1, y + 1);
        }
        else
            show[10 * x + y] = get_boom(arr, x, y) + '0';//如果周围有雷则显示雷的个数
        
    }
}


void game()
{
    char arr[100] = {' ','1','2','3','4','5','6','7','8','9',
                     '1','0','0','1','0','0','0','0','0','1',
                     '2','0','0','0','0','0','0','0','0','0',
                     '3','0','0','0','0','0','0','0','0','0',
                     '4','0','0','0','0','0','0','0','0','0',
                     '5','0','0','0','0','0','0','0','1','1',
                     '6','0','0','1','0','0','0','0','0','0',
                     '7','0','1','0','0','0','0','0','0','1',
                     '8','0','1','0','0','0','0','0','1','0',
                     '9','0','1','0','0','0','0','0','0','0'};
    char show[100] = {' ','1','2','3','4','5','6','7','8','9',
                      '1','*','*','*','*','*','*','*','*','*',
                      '2','*','*','*','*','*','*','*','*','*',
                      '3','*','*','*','*','*','*','*','*','*',
                      '4','*','*','*','*','*','*','*','*','*',
                      '5','*','*','*','*','*','*','*','*','*',
                      '6','*','*','*','*','*','*','*','*','*',
                      '7','*','*','*','*','*','*','*','*','*',
                      '8','*','*','*','*','*','*','*','*','*',
                      '9','*','*','*','*','*','*','*','*','*'};
    int win=0;
    int x = 0;
    int y = 0;
    char *xchar;
    char *ychar;

    
    display(show);
    while (win != MAXFORGAME)//判断是否排完雷
    {
        printf("请输入坐标：\n");
        xchar = readline("X(row) input> ");
        x = *xchar - '0';
        ychar = readline("Y(col) input> ");
        y = *ychar - '0';
        printf("x is %d y is %d\n",x,y);
        if (((x >= 1) && (x <= 9)) && ((y >= 1) && (y <= 9)))
        {
            if (arr[10 * x + y] == '1')
            {
                printf("踩雷，游戏结束!\n");
                printf("*****thanks for playing this naive game(#^.^#)*******\n");
                break;
            }
            else
            {
                fun(arr, show, x, y);
                win = 0;
                int i=1;
                
                for (; i <= 9; i++)
                {

                        if (show[10 * i + 0] == '*'){
                            win++;
                        }
                        if (show[10 * i + 1] == '*'){
                            win++;
                        }
                        if (show[10 * i + 2] == '*'){
                            win++;
                        }
                        if (show[10 * i + 3] == '*'){
                            win++;
                        }
                        if (show[10 * i + 4] == '*'){
                            win++;
                        }
                        if (show[10 * i + 5] == '*'){
                            win++;
                        }
                        if (show[10 * i + 6] == '*'){
                            win++;
                        }
                        if (show[10 * i + 7] == '*'){
                            win++;
                        }
                        if (show[10 * i + 8] == '*'){
                            win++;
                        }
                        if (show[10 * i + 9] == '*'){
                            win++;
                        }
 
                }
                display(show);
            }
        }
        else
            printf("输入坐标有误\n");
    }
    if (win == MAXFORGAME){
        printf("CONGRATULATIONS!雷阵如下：\n");
        display(arr);
    }
}


int 
mon_game(int argc, char **argv, struct Trapframe *tf)
{
    char *buf;
    menu();
    while (1) {
        buf = readline("Game> ");
        if (buf != NULL&& *buf!='0')
        {
            game();
           
            break;
        }
        if(*buf=='0')
        {
            printf("*****thanks for playing this naive game(#^.^#)*******\n");
            break;
        }    

    }
    return 0;
}

/***** File system operations *****/
int
mon_ls(int argc, char **argv, struct Trapframe *tf)
{
    int i;
    int size = fs_get_size();
    struct Wrapper w = fs_find_all_names();
    struct FileControlBlock* fcbs = fs_find_all_fcbs();

    for (i = 0; i < size; i++) {
        char desc[] = "----";
        desc[0] = fcbs[i].type ? 'd' : '-';
        desc[1] = fcbs[i].acl & 4 ? 'r' : '-';
        desc[2] = fcbs[i].acl & 2 ? 'w' : '-';
        desc[3] = fcbs[i].acl & 1 ? 'x' : '-';
        printf("%-4s %-4d %-12s\n", desc, fcbs[i].size, w.name_table[i]);
    }

    return 0;
}

int
mon_touch(int argc, char **argv, struct Trapframe *tf)
{
    return fs_add(argv[1], 0);
}

int
mon_mkdir(int argc, char **argv, struct Trapframe *tf)
{
    return fs_add(argv[1], 1);
}

int
mon_rm(int argc, char **argv, struct Trapframe *tf)
{
    return fs_delete(argv[1]);
}

/***** Display developers' name of this project *****/
int
mon_about(int argc, char **argv, struct Trapframe *tf)
{
    printf("******************************************\n");
	printf("****** 东南大学计算机科学与工程学院 ******\n");
    printf("******      09015336 张奕裕        *******\n");
    printf("******      09015326 陈衍庆        *******\n");
    printf("******      09015331 莫少煌        *******\n");
    printf("******      09015335 叶橄强        *******\n");
    printf("******      09015303 郭文通        *******\n");
    printf("******      09015306 唐雪婷        *******\n");
    printf("******      09015308 莫景雯        *******\n");
	printf("******************************************\n");
    return 0;
}

/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
    int argc;
    char *argv[MAXARGS];
    int i;

    // Parse the command buffer into whitespace-separated arguments
    argc = 0;
    argv[argc] = 0;
    while (1) {
        // gobble whitespace
        while (*buf && strchr(WHITESPACE, *buf))
            *buf++ = 0;
        if (*buf == 0)
            break;

        // save and scan past next arg
        if (argc == MAXARGS-1) {
            printf("Too many arguments (max %d)\n", MAXARGS);
            return 0;
        }
        argv[argc++] = buf;
        while (*buf && !strchr(WHITESPACE, *buf))
            buf++;
    }
    argv[argc] = 0;

    // Lookup and invoke the command
    if (argc == 0)
        return 0;
    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        if (strcmp(argv[0], commands[i].name) == 0)
            return commands[i].func(argc, argv, tf);
    }
    printf("Unknown command '%s'\n", argv[0]);
    return 0;
}

void
monitor(struct Trapframe *tf)
{
    char *buf;

    printf("******Welcome to the MiniSys_OS kernel monitor!******\n");
    printf("Type 'help' for a list of commands.\n");


    while (1) {
        buf = readline("K> ");
        if (buf != NULL)
            if (runcmd(buf, tf) < 0)
                break;
    }
}
