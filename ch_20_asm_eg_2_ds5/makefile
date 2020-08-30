
simple_example.o:  simple_example.s
	armasm --cpu cortex-m4 -o simple_example.o simple_example.s

startup_stm32f4xx.o: startup_stm32f4xx.s
	armasm --cpu cortex-m4 -o startup_stm32f4xx.o startup_stm32f4xx.s

system_stm32f4xx.o: system_stm32f4xx.c
	armcc -c -g -W --cpu cortex-m4 -I CMSIS/Include -I CMSIS/ST/STM32F4xx/Include -o system_stm32f4xx.o system_stm32f4xx.c

simple_example.elf : system_stm32f4xx.o startup_stm32f4xx.o simple_example.o
	armlink --rw_base 0x20000000 --ro_base 0x08000000 simple_example.o startup_stm32f4xx.o system_stm32f4xx.o \
	"--keep=startup_stm32f4xx.o(RESET)" "--first=startup_stm32f4xx.o(RESET)" --entry Reset_Handler --map -o simple_example.elf

simple_example.bin: simple_example.elf
	fromelf --bin --output simple_example.bin simple_example.elf

simple_example.list: simple_example.elf
	fromelf -c --output simple_example.list simple_example.elf
	
all : simple_example.elf simple_example.bin simple_example.list

clean:
	rm simple_example.elf
	rm simple_example.bin
	rm simple_example.list
	rm system_stm32f4xx.o startup_stm32f4xx.o simple_example.o
	
