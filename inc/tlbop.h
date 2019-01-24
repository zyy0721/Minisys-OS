#pragma once

#include <inc/types.h>

typedef unsigned int tlbhi_t;
typedef unsigned int tlblo_t;

// Returns the size of the TLB.
extern int mips_tlb_size (void);
 // Probes the TLB for an entry matching hi, and if present invalidates it.
extern void mips_tlbinval (tlbhi_t hi);

// Invalidate the whole TLB.
extern void mips_tlbinvalall (void);

// Reads the TLB entry with specified by index, and returns the EntryHi,
// EntryLo0, EntryLo1 and PageMask parts in *phi, *plo0, *plo1 and *pmsk
// respectively.
extern void mips_tlbri2 (tlbhi_t *phi, tlblo_t *plo0, tlblo_t *plo1, unsigned *pmsk,
		  int index);

// Writes hi, lo0, lo1 and msk into the TLB entry specified by index.
extern void mips_tlbwi2 (tlbhi_t hi, tlblo_t lo0, tlblo_t lo1, unsigned msk,
		  int index);

// Writes hi, lo0, lo1 and msk into the TLB entry specified by the
// Random register.
extern void mips_tlbwr2 (tlbhi_t hi, tlblo_t lo0, tlblo_t lo1, unsigned msk);

// Probes the TLB for an entry matching hi and returns its index, or -1 if
// not found. If found, then the EntryLo0, EntryLo1 and PageMask parts of the
// entry are also returned in *plo0, *plo1 and *pmsk respectively
extern int mips_tlbprobe2 (tlbhi_t hi, tlblo_t *plo0, tlblo_t *plo1, unsigned *pmsk);

// Probes the TLB for an entry matching hi and if present rewrites that entry,
// otherwise updates a random entry. A safe way to update the TLB.
extern int mips_tlbrwr2 (tlbhi_t hi, tlblo_t lo0, tlblo_t lo1, unsigned msk);

void print_tlb();
