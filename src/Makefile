all: usim core-test add.bin

objs = memory.o sysbus.o clock.o arm.o decoder.o alu.o debug.o

usim: main.o $(objs)
	g++ -o $@ $^

core-test: core-test.o $(objs)
	g++ -o $@ $^

%.o: %.cpp
	g++ -std=c++0x -c $<

%.elf: %.s
	arm-none-eabi-gcc -nostdlib -Ttext=0x0 -mcpu=cortex-m0 $< -o $@

%.bin: %.elf
	arm-none-eabi-objcopy $< -O binary $@

clean:
	rm -f *.o
	rm -f usim
	rm -f add.bin
	rm -f add.elf
