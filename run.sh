#! /bin/bash
# set -x
set -e
ROOT_DIR=/opt/flexpret
MAP_DIR=/data/flexpret
WORK_DIR=${ROOT_DIR}/tests/examples
RES_DIR=${ROOT_DIR}/emulator/generated-src/4tf-16i-16d-ti
TESTBENCH_FILE=${ROOT_DIR}/emulator/testbench/Core-tb-my.cpp
GEN_TESTBENCH_FILE=${ROOT_DIR}/emulator/testbench/Core-tb-my-gen.cpp

if [ "$1"z == "cmpz" ]; then
    cd ${WORK_DIR} && make sboth_all
    cp ${WORK_DIR}/sboth.inst.mem.ins ${RES_DIR}/asm.tmp
    grep -E '[0-9a-f]+:' ${RES_DIR}/asm.tmp | grep -v 'elf' | awk '{$1="";print $0}' > ${RES_DIR}/asm
    rm -f ${RES_DIR}/asm.tmp
elif [ "$1"z == "runz" ]; then
    # gen config
    # dat_t -- signal; mem_t -- memory
    grep -E '^  dat_t<[0-9]+> Core' ${RES_DIR}/Core.h | grep -v -E '__prev' | grep -v -E '(scheduler|control)__R[0-9]+' | awk '{match($1,/dat_t<([0-9]+)>/,a); print a[1], substr($2,0,length($2)-1)}' > ${RES_DIR}/config
    # modify main
    format=""
    while read line; do
        line=(${line})
        if [ ${line[0]} -eq 1 ]; then
            format=${format}" %1d"
        elif [ ${line[0]} -le 32 ]; then
            format=${format}" %08x"
        else 
            format=${format}" %016x"
        fi
    done < ${RES_DIR}/config
    format="\"${format}\\\n\""
    # echo $format
    cp ${TESTBENCH_FILE} ${GEN_TESTBENCH_FILE}
    sed -i "s/FORMAT_TO_BE_MODIFIED/$format/" ${GEN_TESTBENCH_FILE}
    var=`awk '{printf("c\\->%s.lo_word(),", $2);}' ${RES_DIR}/config`
    var=${var%?}
    # echo $var
    sed -i "s/VAR_TO_BE_MODIFIED/$var/" ${GEN_TESTBENCH_FILE}
    # compile
    echo "compiling testbench Core-my ... (complie log location: ${RES_DIR}/cmp_log)"
    cd ${WORK_DIR} && make Core-my > ${RES_DIR}/cmp_log 2>&1
    # run
    echo "running testbench Core-my ... (log location: ${RES_DIR}/raw_log)"
    ${WORK_DIR}/Core-my --maxcycles=150000 --ispm=sboth.inst.mem --dspm=sboth.data.mem --trace 2> ${RES_DIR}/raw_log > ${RES_DIR}/log
    echo "Core-my return $?."
    gzip -c ${RES_DIR}/log > ${RES_DIR}/log.tar.gz
elif [ "$1"z == "copybeforez" ]; then
    list=(`cd $MAP_DIR && git --no-pager diff HEAD~1 | grep '^+++ b/' | sed 's/+++ b\///'`)
    for i in ${list[*]}; do 
        echo -n "copy  ${MAP_DIR}/$i  to  ${ROOT_DIR}/$i ... "
        cp ${MAP_DIR}/$i ${ROOT_DIR}/$i
        echo "done."
    done
elif [ "$1"z == "copyafterz" ]; then
    list=(
        emulator/generated-src/4tf-16i-16d-ti/asm
        emulator/generated-src/4tf-16i-16d-ti/config
        emulator/generated-src/4tf-16i-16d-ti/log.tar.gz
        emulator/generated-src/4tf-16i-16d-ti/raw_log
        emulator/generated-src/4tf-16i-16d-ti/cmp_log
        tests/examples/sboth.inst.mem.ins
    )
    for i in ${list[*]}; do 
        echo -n "cp ${ROOT_DIR}/$i ${MAP_DIR}/$i ... "
        cp ${ROOT_DIR}/$i ${MAP_DIR}/$i
        echo "done."
    done
else 
    echo "need args"
fi
