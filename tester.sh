#!/bin/bash

# make a list of all files in testpahse folders

PROG_NAME='./mini'
phase1='./testphase1'
phase2='./testphase2'


for file in $phase1 $phase2;
do
    find $file -type f;
done > list.txt

list='list.txt'
# compare the outputs, if they are the same show color green else show red
while read line; do
    echo $line
    python3 $line
    ./mini $line
done < $list


# while read -r line;
# do
#     $PROG_NAME $line > out.txt
#     diff -q out.txt $line.out
#     if [ $? -eq 0 ]; then
#         echo "Test passed for $line"
#     else
#         echo "Test failed for $line"
#     fi
# done < $list
# # remove list.txt
# rm list.txt

# for each file in the list
while read line
do
    # run the program on the file
    $PROG_NAME $line >> output.txt
    # get the name of the file
    name=$(echo $line )
    # get the expected output
    python3 $line >> pyout.txt
    expected=$(cat pyout.txt)
    # get the actual output
    actual=$(cat output.txt)
    # compare the expected and actual outputs
    if [ "$actual" = "$expected" ]
    then
        echo "Test $name passed"
    else
        echo "Test $name failed"
    fi
done < "$list"

# delete the list file


# delete the output file


# exit the script
exit 0


