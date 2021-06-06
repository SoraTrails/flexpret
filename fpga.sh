#!/bin/bash

if [ "${1}z" == "z" ]; then
    target=os
else
    target=$1
fi
cp src/os/${target}.data.mem fpga/
cp src/os/${target}.inst.mem fpga/
rm -f fpga/fpga.tar
rm -rf fpga/dir*
cd fpga && bash tomem.sh ${target}.inst.mem && bash tomem.sh ${target}.data.mem && cd -
for i in {0..7}; do
    echo -n "patching fpga/dir_${target}.data.mem/${i}_${target}.data.mem ... "
    for j in {0..2000}; do 
        echo "0000" >> fpga/dir_${target}.data.mem/${i}_${target}.data.mem
    done
    echo " done."
done
tar cf fpga/fpga.tar fpga/dir*
cp fpga/fpga.tar /data/
echo $target