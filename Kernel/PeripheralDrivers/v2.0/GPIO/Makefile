all: test1.elf test2.elf

test1.elf: test1.c gpio.c
	gcc test1.c gpio.c -o test1.elf

test2.elf: test2.c gpio.c
	gcc test2.c gpio.c -o test2.elf

clear:
	rm test1.elf
	rm test2.elf

