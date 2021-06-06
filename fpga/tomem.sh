
if [ ! -f $1 ]; then
    echo "$1 not exist"
    exit 0
fi
rm -rf dir_$1 && mkdir -p dir_$1
touch dir_$1/0_$1
touch dir_$1/1_$1
touch dir_$1/2_$1
touch dir_$1/3_$1
touch dir_$1/4_$1
touch dir_$1/5_$1
touch dir_$1/6_$1
touch dir_$1/7_$1

linenum=0
while read line; do
    val=(`echo "$line" | awk 'BEGIN{FS="";OFS=" "} {print $1,$2,$3,$4,$5,$6,$7,$8}'`)
    for i in {0..7}; do
        caseval=${val[i]}
        case $caseval in
            0) echo "0000" >> dir_$1/${i}_$1;;
            1) echo "0001" >> dir_$1/${i}_$1;;
            2) echo "0010" >> dir_$1/${i}_$1;;
            3) echo "0011" >> dir_$1/${i}_$1;;
            4) echo "0100" >> dir_$1/${i}_$1;;
            5) echo "0101" >> dir_$1/${i}_$1;;
            6) echo "0110" >> dir_$1/${i}_$1;;
            7) echo "0111" >> dir_$1/${i}_$1;;
            8) echo "1000" >> dir_$1/${i}_$1;;
            9) echo "1001" >> dir_$1/${i}_$1;;
            A) echo "1010" >> dir_$1/${i}_$1;;
            B) echo "1011" >> dir_$1/${i}_$1;;
            C) echo "1100" >> dir_$1/${i}_$1;;
            D) echo "1101" >> dir_$1/${i}_$1;;
            E) echo "1110" >> dir_$1/${i}_$1;;
            F) echo "1111" >> dir_$1/${i}_$1;;
            *) >&2 echo "bad val" ;;
        esac
    done
    linenum=$(($linenum+1))
done < $1

echo $linenum