all: armsim add.bin

armsim: memory.o sysbus.o clock.o arm.o main.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -std=c++11 -c $<

%.elf: %.s
	arm-none-eabi-gcc -nostdlib -Ttext=0x0 -mcpu=cortex-m0 $< -o $@

%.bin: %.elf
	arm-none-eabi-objcopy $< -O binary $@

clean:
	rm -f *.o
	rm -f armsim
	rm -f add.bin
	rm -f add.elf
