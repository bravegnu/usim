#!/bin/bash

read-0() {
    while [ "$1" ]; do
        IFS=$'\0' read -r -d '' "$1" || return 1
        shift
    done
}

for file in *.yml
do
    cat ${file} | shyaml get-value when > test.s
    echo "bkpt" >> test.s
    arm-none-eabi-gcc -nostdlib -Ttext=0x0 -mcpu=cortex-m0 test.s -o test.elf > test.build 2>&1
    arm-none-eabi-objcopy test.elf -O binary test.bin >> test.build 2>&1
    cat ${file} | shyaml key-values given | ../src/core-test > test.output
    cat ${file} | shyaml key-values-0 then |
    	while read-0 key value; do
	    grep "${key} ${value}" test.output > /dev/null ||
		echo "FAILED:" $(shyaml get-value name ${file} < ${file})
	done
done
