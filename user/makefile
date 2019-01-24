INCLUDES = -I ../

CC = mips-mti-elf-gcc
LD = mips-mti-elf-ld
OD = mips-mti-elf-objdump
OC = mips-mti-elf-objcopy
SZ = mips-mti-elf-size

USERLIB := libos.o \
	string.o \
	syscall_lib.o \

CFLAGS = -EL -g -march=m14kc -O1 -G0
LDFLAGS = -EL -G0

CFLAGS += -nostdlib -static
LDFLAGS += -static -n -nostdlib -T ./user.lds

all: $(USERLIB) entry.o syscall_wrap.o mytest.x mytest.b mytest.b.c

%.x: %.b.c
	echo cc1 $<
	$(CC) $(CFLAGS) -c -o $@ $<

%.b.c: %.b
	echo create $@
	echo bintoc $<
	./bintoc.exe $<

%.b: entry.o syscall_wrap.o %.o $(USERLIB)
	echo ld $@
	$(LD) -o $@ $(LDFLAGS) $^

%.o: %.c
	echo user1 cc $<
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.S
	echo as $<
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: lib.h

clean:
	rm -rf *.o *.b.c *.x *.b
