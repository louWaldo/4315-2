#!/bin/bash

# make a list of all files in testpahse folders

PROG_NAME='./mini'
phase2='./testphase2'

# Set the color variable
green='\033[0;32m'
red='\033[0;31m'
# Clear the color after that
clear='\033[0m'

rm 'output.txt'
rm 'pyout.txt'

for file in $phase1 $phase2;
do
    find $file -type f;
done > list.txt

list='list.txt'
# compare the outputs, if they are the same show color green else show red
while read line; do
    name=$(echo $line )
    actual=$(python3 $line)
    out=$($PROG_NAME $line)
    printf "%s\n %s \n\n" "$name" "$actual" >> pyout.txt
    printf "%s\n %s \n\n" "$name" "$out" >> output.txt

    if [ out = actual ]
    then
        echo -e "Test $name ${green}Passed${clear}"
    else
        echo -e "Test $name ${red}Failed${clear}"
    fi

done < $list



# delete the list file
rm list

# delete the output file


# exit the script
exit 0




#     # # get the expected output
#     # 
#     # expected=$(echo "pyout.txt")
#     # # get the actual output
#     # actual=$(echo "output.txt")
#     # # compare the expected and actual outputs
#     # if [ "$actual" = "$expected" ]
#     # then
#     #     echo "Test $name passed"
#     # else
#     #     echo "Test $name failed"
#     # fi
# done < "$list"
