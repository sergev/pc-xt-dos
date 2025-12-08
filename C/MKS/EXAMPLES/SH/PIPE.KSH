#!/bin/sh

#
# pipe  --  multi-input, multi-output pipelining
#
#           Terry Jones 19/10/87 (tcjones@watdragon)
#
#-------------------------------------------------------------------------------
#             Department Of Computer Science, University Of Waterloo
#             Waterloo Ontario Canada N2L 3G1
#
#{ihnp4,allegra,decvax,utzoo,utcsri,clyde}!watmath!watdragon!tcjones
#tcjones@dragon.waterloo.{cdn,edu} tcjones@WATER.bitnet
#tcjones%watdragon@waterloo.csnet [from oz, tcjones@dragon.waterloo.cdn@munnari]
#-------------------------------------------------------------------------------
#

myname=`basename $0`

if [ $# -eq 0 ]
then
    while read line
    do
        echo $line
    done
    exit 1
fi

def_shell=/bin/sh

if [ -z "$SHELL" ]
then
    echo No '$SHELL' variable set, using $def_shell
    SHELL=$def_shell
fi

cum_in=/tmp/${USER}_pipe_$$

if [ -f $cum_in -a ! -w $cum_in ]
then
    echo ${myname}: could not use temporary ${cum_in} - try again.
    exit 1
fi

>$cum_in
tty=`tty`
#echo rm $cum_in >> rmt

IN=1
SOME_IN=0
SOME_OUT=0
ANY_OUT=0
while [ -n "$1" ]
do
    case $1 in
        -in|-i) 
            IN=1
            SOME_IN=0

            if [ "$SOME_OUT" = "0" ]
            then
                SOME_OUT=1
                cat $cum_in
            fi

            shift;;
        -out|-o)
            IN=0
            SOME_OUT=0
            ANY_OUT=1

            if [ "$SOME_IN" = "0" ]
            then
                SOME_IN=1
                while read line
                do
                    echo $line >> $cum_in
                done
            fi
            shift;;
        *) 
            if [ "$IN" = "1" ]
            then
                SOME_IN=1
                if [ "$1" = "-" ]
                then
                    while read line 
                    do
                        echo $line >> $cum_in
                    done
                else
                    echo $1 | $SHELL >> $cum_in
                fi
            else
                SOME_OUT=1
                if [ "$1" = "-" ]
                then
                    cat $cum_in
                else
                    eval "cat $cum_in" "$1"
                fi
            fi

            shift;;
    esac
done

if [ "$SOME_OUT" = "0" -o "$ANY_OUT" = "0" ]
then
    cat $cum_in
fi

/bin/rm -f $cum_in
