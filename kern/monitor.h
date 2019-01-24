#pragma once

struct Trapframe;

// Activate the kernel monitor,
// optionally providing a trap frame indicating the current state
// (NULL if none).
void monitor(struct Trapframe *tf);

// Functions implementing monitor commands.
int mon_help(int argc, char **argv, struct Trapframe *tf);
int mon_kerninfo(int argc, char **argv, struct Trapframe *tf);
int mon_play(int argc, char **argv, struct Trapframe *tf);
int mon_game(int argc, char **argv, struct Trapframe *tf);
int mon_tlb(int argc, char **argv, struct Trapframe *tf);
int mon_sw(int argc, char **argv, struct Trapframe *tf);
int mon_ls(int argc, char **argv, struct Trapframe *tf);
int mon_touch(int argc, char **argv, struct Trapframe *tf);
int mon_mkdir(int argc, char **argv, struct Trapframe *tf);
int mon_rm(int argc, char **argv, struct Trapframe *tf);
int mon_about(int argc, char **argv, struct Trapframe *tf);
int mon_sw(int argc, char **argv, struct Trapframe *tf);