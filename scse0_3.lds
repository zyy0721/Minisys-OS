OUTPUT_ARCH(mips)
__entry = DEFINED(__reset_vector) ? 0xbfc00000 : _start;
ENTRY(__entry)
/*
 * The start of flash must be set if including boot code.  By default
 * the use of boot code will mean that application code is copied
 * from flash to RAM at runtime before being executed.
 */
PROVIDE (__flash_start = DEFINED(__reset_vector) ? 0xbfc00000 : __app_start);
PROVIDE (__tlb_stride_length = 1);

SECTIONS
{
    /* Start of bootrom */
  .bootrom 0x9fc00000 : /* Runs uncached (from 0x9fc00000) until I$ is
			   initialized. */
  AT (__flash_start)
  {
    *(.reset)		/* Reset entry point. */
    *(.boot)		/* Boot code. */
    . = ALIGN(8);
  } = 0

  . = 0x80000000;
  .except_vec3 : {
	*(.text.exc_vec3)
  }
  . = 0x80010000;
  .text : {
    *(.text)
        }

  .data : {
	*(.data)
	}

  .sdata : {
  	*(.sdata)
  }

  _fbss = .;
  .bss  : {
	*(.bss)
  }
   
   . = 0x80400000;

   end = . ;
  _end = . ;
}
