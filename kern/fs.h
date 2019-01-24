#pragma once

#include <inc/mfp_io.h>
#include <inc/types.h>
#include <inc/fcb.h>
#include <inc/printf.h>
#include <mips/cpu.h>

#define BLOCK_SIZE 4
#define CAPACITY 64

// 封装二维数组以进行return操作
struct Wrapper
{
	char name_table[CAPACITY][16];					// 文件名列表
};
struct Wrapper w;
struct FileControlBlock fat_table[CAPACITY];		// FCB列表
int fs_size;										// 文件数量

void fs_init();
struct FileControlBlock fs_find(char*);
int fs_add(char*, int);
int fs_delete(char*); 

struct Wrapper fs_find_all_names();
struct FileControlBlock* fs_find_all_fcbs();
int fs_get_size();
struct FileControlBlock fs_read_fcb(int);
void fs_write_fcb(int, struct FileControlBlock);
