#! /bin/bash
# set -x
set -e

ROOT_DIR=/opt/flexpret
MAP_DIR=/data/flexpret
# RES_DIR=${ROOT_DIR}/emulator/generated-src/4tf-16i-16d-ti
RES_DIR=${ROOT_DIR}/emulator/generated-src/8tf-64i-64d-ti

showHelp() {
cat << EOF  
Usage: ./run.sh [-h] [-s <before|after>] [-c [test]] [-r [test]] [-C]
Sync, compile, run flexpret simulator

-h,  --help                    Display help
-C,  --clean                    Clean file
-s,  --sync <before|after>     Sync before: copy source file from docker mapped region to container
                               Sync after:  copy source file from container to docker mapped region
-c,  --compile [target]        Compile source file to elf and generate inst&data file, default target is os.
-r,  --run [target]            Compile testbench and run it using inst&data file, default target is os.
EOF
}

cleanFile() {
    WORK_DIR=${ROOT_DIR}/src/os
    cd $WORK_DIR && make cleanall
}

syncFile() {
    # FILE_DIR=emulator/generated-src/4tf-16i-16d-ti
    FILE_DIR=emulator/generated-src/8tf-64i-64d-ti
    if [ "$1"z == "beforez" ]; then
        list=(`cd $MAP_DIR && git --no-pager diff HEAD | grep '^+++ b/' | sed 's/+++ b\///'`)
        for i in ${list[*]}; do 
            echo -n "copy  ${MAP_DIR}/$i  to  ${ROOT_DIR}/$i ... "
            cp ${MAP_DIR}/$i ${ROOT_DIR}/$i
            echo "done."
        done
    elif [ "$1"z == "afterz" ]; then
        gzip -c ${RES_DIR}/log > ${RES_DIR}/log.tar.gz
        list=(
            ${FILE_DIR}/asm
            ${FILE_DIR}/asm_func
            ${FILE_DIR}/config
            ${FILE_DIR}/log.tar.gz
            ${FILE_DIR}/raw_log
            ${FILE_DIR}/cmp_log
            tests/examples/sboth.inst.mem.ins
            src/os/os.dump
            src/os/test.dump
            src/os/cmsis.dump
            src/os/rtthread.dump
        )
        for i in ${list[*]}; do 
            echo -n "cp ${ROOT_DIR}/$i ${MAP_DIR}/$i ... "
            if [ ! -f ${ROOT_DIR}/$i ]; then
                echo "not found, skip."
            else
                cp ${ROOT_DIR}/$i ${MAP_DIR}/$i
                echo "done."
            fi
        done
        echo -n "cp ${FILE_DIR}/*.raw_log to ${MAP_DIR}/../wcet_log ..."
        cp ${FILE_DIR}/*.raw_log ${MAP_DIR}/../wcet_log
        echo "done."
    else
        echo "bad args for sync file"
    fi
}

compileFile() {
    if [ "$1"z == "sbothz" ]; then 
        WORK_DIR=${ROOT_DIR}/tests/examples
    else
        WORK_DIR=${ROOT_DIR}/src/os
    fi
    if [ "$1"z == "osz" -o "$1"z == "sbothz" ]; then
        cd ${WORK_DIR} && make $1_all
        cp ${WORK_DIR}/$1.inst.mem.ins ${RES_DIR}/asm.tmp
        grep -E '[0-9a-f]+:' ${RES_DIR}/asm.tmp | grep -v 'elf' | awk '{$1=substr($1,0,length($1)-1);$2="";print $0}' > ${RES_DIR}/asm
        grep -E '^[0-9a-f]+ <[0-9a-zA-Z_]+>:' ${RES_DIR}/asm.tmp | awk '{$2=substr($2,2,length($2)-3); print $0}' > ${RES_DIR}/asm_func
        rm -f ${RES_DIR}/asm.tmp 
    elif [ "$1"z == "testz" -o "$1"z == "cmsisz" ]; then
        cd ${WORK_DIR} && make $1
        cp ${WORK_DIR}/$1.inst.mem.ins ${RES_DIR}/asm.tmp
        grep -E '[0-9a-f]+:' ${RES_DIR}/asm.tmp | grep -v 'elf' | awk '{$1=substr($1,0,length($1)-1);$2="";print $0}' > ${RES_DIR}/asm
        grep -E '^[0-9a-f]+ <[0-9a-zA-Z_]+>:' ${RES_DIR}/asm.tmp | awk '{$2=substr($2,2,length($2)-3); print $0}' > ${RES_DIR}/asm_func
        rm -f ${RES_DIR}/asm.tmp
    elif [ "$1"z == "wcetz" ]; then
        cd ${WORK_DIR} && make wcet
    elif [ "$1"z == "rtthreadz" ]; then
        RTTHREAD_DIR=/data/rtthread/rt-thread-3.1.3
        cd ${RTTHREAD_DIR} && make
        cp ${RTTHREAD_DIR}/rtthread.dump ${WORK_DIR}/
        cp ${RTTHREAD_DIR}/rtthread.inst.mem.ins ${WORK_DIR}/
        cp ${RTTHREAD_DIR}/rtthread.inst.mem ${WORK_DIR}/
        cp ${RTTHREAD_DIR}/rtthread.data.mem ${WORK_DIR}/
        cp ${WORK_DIR}/rtthread.inst.mem.ins ${RES_DIR}/asm.tmp
        grep -E '[0-9a-f]+:' ${RES_DIR}/asm.tmp | grep -v 'elf' | awk '{$1=substr($1,0,length($1)-1);$2="";print $0}' > ${RES_DIR}/asm
        grep -E '^[0-9a-f]+ <[0-9a-zA-Z_]+>:' ${RES_DIR}/asm.tmp | awk '{$2=substr($2,2,length($2)-3); print $0}' > ${RES_DIR}/asm_func
        rm -f ${RES_DIR}/asm.tmp 
    fi
}

runWCETTestbench() {
    WORK_DIR=${ROOT_DIR}/src/os
    TESTBENCH_FILE=${ROOT_DIR}/emulator/testbench/Core-tb-os.cpp
    GEN_TESTBENCH_FILE=${ROOT_DIR}/emulator/testbench/Core-tb-os-gen.cpp
    cp ${TESTBENCH_FILE} ${GEN_TESTBENCH_FILE}
    sed -i "s/FORMAT_TO_BE_MODIFIED/\"\"/" ${GEN_TESTBENCH_FILE}
    sed -i "s/VAR_TO_BE_MODIFIED/\"\"/" ${GEN_TESTBENCH_FILE}
    echo "compiling testbench Core ... (complie log location: ${RES_DIR}/cmp_log)"
    rm -f ${WORK_DIR}/Core-os.o
    cd ${WORK_DIR} && make Core > ${RES_DIR}/cmp_log 2>&1

    for i in `ls ${WORK_DIR}/test/malardalen_wcet/*.c`; do
        name=`basename ${i}`
        name=${name%.*}
        echo "running testbench Core (target: wcet, case: ${name})... (log location: ${RES_DIR}/${name}_wcet.raw_log)"
        ${WORK_DIR}/Core --maxcycles=1000000 --ispm=${WORK_DIR}/test/malardalen_wcet/${name}_wcet.inst.mem --dspm=${WORK_DIR}/test/malardalen_wcet/${name}_wcet.data.mem 2> ${RES_DIR}/${name}_wcet.raw_log > /dev/null
    done
    rm -f ${WORK_DIR}/Core-os.o
}

runTestbench() {
    if [ "$1"z == "sbothz" ]; then 
        WORK_DIR=${ROOT_DIR}/tests/examples
        target=sboth
    elif [ "$1"z == "wcetz" ]; then 
        runWCETTestbench ;
        exit 0
    else
        WORK_DIR=${ROOT_DIR}/src/os
        target=os
    fi
    # RES_DIR=${ROOT_DIR}/emulator/generated-src/8tf-64i-64d-ti
    # RES_DIR=${ROOT_DIR}/emulator/generated-src/4tf-16i-16d-ti
    TESTBENCH_FILE=${ROOT_DIR}/emulator/testbench/Core-tb-os.cpp
    GEN_TESTBENCH_FILE=${ROOT_DIR}/emulator/testbench/Core-tb-os-gen.cpp

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
    echo "compiling testbench Core ... (complie log location: ${RES_DIR}/cmp_log)"
    cd ${WORK_DIR} && make Core > ${RES_DIR}/cmp_log 2>&1
    # run
    echo "running testbench Core (target: ${1})... (log location: ${RES_DIR}/raw_log)"
    # echo "${WORK_DIR}/Core --maxcycles=100000 --ispm=${1}.inst.mem --dspm=${1}.data.mem 2> ${RES_DIR}/raw_log > ${RES_DIR}/log"
    ${WORK_DIR}/Core --maxcycles=180000 --ispm=${WORK_DIR}/${1}.inst.mem --dspm=${WORK_DIR}/${1}.data.mem 2> ${RES_DIR}/raw_log > ${RES_DIR}/log
    echo "Core return $?."
    grep "\*\*\*" /opt/flexpret/emulator/generated-src/8tf-64i-64d-ti/raw_log
    # gzip -c ${RES_DIR}/log > ${RES_DIR}/log.tar.gz
}

TEMP=`getopt -o hs:c::r::C --long help,sync:,compile::,run::,clean \
     -n 'run.bash' -- "$@"`

if [ $? != 0 ] ; then echo "bad args..." ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

while true ; do
    case "$1" in
        -C|--clean) cleanFile ; exit 0 ;; 
        -h|--help) showHelp ; exit 0 ;;
        -s|--sync) syncFile $2 ; exit 0 ;;
        -c|--compile) 
            case "$2" in
                "") compileFile os; exit 0 ;;
                *)  compileFile $2 ; exit 0 ;;
            esac ;;
        -r|--run) 
            case "$2" in
                "") runTestbench os; exit 0 ;;
                *)  runTestbench $2 ; exit 0 ;;
            esac ;;
        --) shift ; break;;
        *) shift 1 ; exit 0 ;;
    esac
    shift
done

exit 0