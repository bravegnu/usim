#!/bin/bash

read-0() {
    while [ "$1" ]; do
        IFS=$'\0' read -r -d '' "$1" || return 1
        shift
    done
}

count=0

for file in *.yml
do
    base=$(basename $file .yml)
    cat ${file} | shyaml get-value when > ${base}.s
    echo "bkpt" >> ${base}.s
    arm-none-eabi-gcc -nostdlib -Ttext=0x0 -mcpu=cortex-m0 ${base}.s -o ${base}.elf > ${base}.build 2>&1
    arm-none-eabi-objcopy ${base}.elf -O binary test.bin >> ${base}.build 2>&1
    cat ${file} | shyaml key-values given | ../src/core-test > ${base}.output
    cat ${file} | shyaml key-values-0 then |
    	while read-0 key value; do
	    grep "${key} ${value}" ${base}.output > /dev/null ||
	        echo "FAILED:" $(shyaml get-value name ${file} < ${file})
	done

    let count=count+1
done

echo Ran ${count} tests.
