CC = iccavr
CFLAGS =  -e -D__ICC_VERSION="7.07" -DATMEGA -DATMega16 -DF_CPU=7372800 -l -g -Mavr_enhanced -I../../lib
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -g -ucrtatmega.o -bfunc_lit:0x54.0x4000 -dram_end:0x45f -bdata:0x60.0x45f -dhwstk_size:16 -beeprom:0.512 -fihx_coff -S2
FILES = main.o 

sensor-station-temperature:	$(FILES)
	$(CC) -o sensor-station-temperature $(LFLAGS) main.o -lcatmega
main.o:	main.c
	$(CC) -c $(CFLAGS) main.c

clean:
	rm -f *.o *.lis *.s *.cof *.dbg *.lst *.mp *.hex *.i
