all: mzero add.bin

mzero: memory.o sysbus.o clock.o arm.o main.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -std=c++0x -c $<

%.elf: %.s
	arm-none-eabi-gcc -nostdlib -Ttext=0x0 -mcpu=cortex-m0 $< -o $@

%.bin: %.elf
	arm-none-eabi-objcopy $< -O binary $@

clean:
	rm -f *.o
	rm -f mzero
	rm -f add.bin
	rm -f add.elf
