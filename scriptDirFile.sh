#!/bin/bash
filname=$1
if test -f $1
then 
    touch $1 
    echo -e "\nFile $1 already exists and was updated successfully\n"
    else
    touch $1
    echo -e "\nFile $1 was created successfully\n"
fi
