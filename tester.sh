#!/bin/bash

clear

PROG_NAME='./mini'
phase2='./testphase2'

# Set the color variable
green='\033[0;32m'
red='\033[0;31m'
clear='\033[0m'

rm 'output.txt'
rm 'pyout.txt'

for file in $phase1 $phase2;
do
    find $file -type f;
done > list.txt

list='list.txt'
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

rm 'list.txt'

# exit the script
exit 0