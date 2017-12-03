#!/bin/bash

genlist ()
{
    #find $1 -type f -exec md5sum \{\} \; | sort -k2
    find $1 ! -type d | sort -k2
}

usage ()
{
    echo "Usage:"
    echo "  $0 <target> <list file>"
}

main ()
{
    for i in $*
    do
        if [ "x$i" == "-h" ] || [ "x$i" == "--help" ];then
            usage 
            exit 1
        fi
    done

    test ! $# -eq 2 && usage && exit 1
    test ! -d "$1" && echo "Directory \"$1\" not exist!" && exit 1

    target_root=$1
    list_file=$2
    exec 5>$list_file

    cd $target_root
    echo -e "$PWD\n" >&5

    echo "Getting files list..."
    genlist >&5
    echo "Done."
}

main $*
