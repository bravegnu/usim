#!/bin/bash

read-0() {
    while [ "$1" ]; do
        IFS=$'\0' read -r -d '' "$1" || return 1
        shift
    done
}

let count=0
let passed=0

for file in *.yml
do
    err=0
    failed=0

    base=$(basename $file .yml)
    shyaml get-value when < ${file} > ${base}.s 2> ${base}.err&&
	echo "bkpt" >> ${base}.s &&
	arm-none-eabi-gcc -nostdlib -Ttext=0x0 -mcpu=cortex-m0 ${base}.s -o ${base}.elf > ${base}.build 2> ${base}.err &&
	arm-none-eabi-objcopy ${base}.elf -O binary ${base}.bin >> ${base}.build 2> ${base}.err &&
	shyaml key-values given < ${file} > ${base}.input 2> ${base}.err &&
	../src/core-test ${base}.bin < ${base}.input > ${base}.output 2> ${base}.err ||
	    err=1

    if [ ${err} -eq 0 ]; then
	cat ${file} | shyaml key-values-0 then |
    	    while read-0 key value; do
		grep "${key} ${value}" ${base}.output > /dev/null
		if [ $? -ne 0 ]; then
		    echo -n "Expected ${key}=${value} "
		    echo -n "Got "
		    grep "${key} " ${base}.output | tr " " "="
		    
		    exit 1
		fi
	    done > ${base}.expected
	failed=$?
	expected=$(cat ${base}.expected)
    fi

    if [ ${err} -eq 0 -a ${failed} -eq 0 ]; then
	rm -f ${base}.s
	rm -f ${base}.bin
	rm -f ${base}.build
	rm -f ${base}.output
	rm -f ${base}.input
	rm -f ${base}.expected
	rm -f ${base}.err
	
	let passed=passed+1
    else
	if [ ${err} -ne 0 ]; then
	    echo ${file}: "ERROR: Check ${base}.err for more details"
	    cat -n ${base}.err | tail -n5 | sed -e "s/^/    /"
	else
	    echo -n ${file}: "FAILED: "
	    echo ${expected}	    
	fi
    fi

    let count=count+1
done

echo Ran ${count} tests. ${passed} passed.
