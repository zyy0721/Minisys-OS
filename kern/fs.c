#include "fs.h"
#include <inc/string.h>
#include <inc/printf.h>

void fs_init()
{

	int i;
		// 清空name_table
	for (i = 0; i < CAPACITY; i++)
		memset(w.name_table[i], 0, 16);

	fs_size = 2;
	strcpy(w.name_table[0], "test");
	strcpy(w.name_table[1], "app.exe");
	struct FileControlBlock fcb;
	fcb.type = 1;
	fcb.acl = 7;
	fcb.date = 1540000000;
	fcb.size = BLOCK_SIZE;
	fat_table[0] = fcb;
	fcb.type = 0;
	fcb.acl = 5;
	fcb.date = 1540000000;
	fcb.size = BLOCK_SIZE;
	fat_table[1] = fcb;

	// // 初始化name_table,fat_table,fs_size
	// int i;
	// for (i = 0; i < CAPACITY; i++)
	// {
	// 	u32 addr = FAT_NAME_ADDR + i * 16;
	// 	u_ll name = mips_get_dword(addr, NULL);
	// 	if (name != 0)
	// 	{
	// 		name_table[i] = name;
	// 		fat_table[i] = fs_read_fcb(i);
	// 	}
	// 	else
	// 	{
	// 		fs_size = i;
	// 		break;
	// 	}
	// }



}

int fs_add(char* filename, int mode)
{
	struct FileControlBlock fcb;
	fcb.type = mode;
	fcb.acl = 7;
	fcb.date = 1540000000;
	fcb.size = BLOCK_SIZE;

    strcpy(w.name_table[fs_size], filename);
	fat_table[fs_size] = fcb;
    //printf("fs_size is %d\n",fs_size);
	// printf("cur: %s  prev: %s", name_table[fs_size], name_table[fs_size-1]);

	fs_size += 1;

	return 0;
}

int fs_delete(char* filename)
{
	int i, index = -1;
	for (i = 0; i < fs_size; i++)
	{
		if (strcmp(w.name_table[i], filename) == 0)
		{
			index = i;
			break;
		}
	}

	printf("index: %d\n", index);

	// -1 代表找不到文件
	if (index == -1)
		return 1;
	else
	{
		
		for (i = index; i < fs_size; i++)
		{
			strcpy(w.name_table[i], w.name_table[i + 1]);
			fat_table[i] = fat_table[i + 1];
		}
		fs_size--;
	}
}

struct Wrapper fs_find_all_names()
{
	return w;
}

struct FileControlBlock* fs_find_all_fcbs()
{
	return fat_table;
}

int fs_get_size()
{
	return fs_size;
}

struct FileControlBlock fs_find(char* filename)
{
	int index = -1;
	int i;
	for (i = 0; i < fs_size; i++)
	{
		if (strcmp(w.name_table[i], filename) == 0)
			index = i;
	}

	if (index != -1)
		return fat_table[index];
	else
	{
		struct FileControlBlock ret = {-1, -1, -1, -1, -1};
		return ret;
	}
}

struct FileControlBlock fs_read_fcb(int index)
{
	struct FileControlBlock ret;

	u32 base = FAT_FCB_ADDR + index * 16;
	ret.type = mips_get_byte(base, NULL);
	ret.acl = mips_get_byte(base + 1, NULL);
	ret.size = mips_get_byte(base + 2, NULL);
	ret.pointer = mips_get_byte(base + 3, NULL);
	ret.date = mips_get_word(base + 4, NULL);

	return ret;
}

void fs_write_fcb(int index, struct FileControlBlock fcb)
{
	u32 base = FAT_FCB_ADDR + index * 16;
	mips_put_byte(base, fcb.type);
	mips_put_byte(base, fcb.acl);
	mips_put_byte(base, fcb.size);
	mips_put_byte(base, fcb.pointer);
	mips_put_word(base, fcb.date);
}
