#!/bin/bash

copy_files ()
{
    release_dir="$1"
    files_list="$2"

    # copy files
    echo "Copy ..."
    line_no=0

    for i in `cat $files_list`; do
        ((line_no++))
        if [ $line_no -eq 1 ];then
            test ! -e $i && echo "Directory $i not exist!\n" && exit 1
            cd $i
            continue
        fi
        file_dir=${i%/*}
        target_dir=$release_dir/$file_dir
        test ! -e "$target_dir" && mkdir -p "$target_dir" >/dev/null && chmod 755 "$target_dir"
        cp $i $target_dir -a
    done
}

strip_files ()
{
    release_dir=$1
    # strip
    echo "Strip ..."
    which $STRIP > /dev/null
    if [ $? -ne 0 ]; then
        echo "  error: strip not found."
        return
    fi
    test -e $release_dir/lib && $STRIP $release_dir/lib/* 2>/dev/null
    test -e $release_dir/lib && $STRIP $release_dir/bin/* 2>/dev/null
}

usage ()
{
    echo "Get the list file from \"genlist.sh\"."
    echo "Usage:"
    echo "  ./gen-release.sh <release dir> <list file>"
} 

check_args ()
{
    for i in $*
    do
        if [ "x$i" == "-h" ] || [ "x$i" == "--help" ];then
            usage 
            exit 1
        fi
    done

    test ! $# -eq 2 && usage && exit 1
}

main ()
{
    check_args $*

    release_dir=$1
    files_list=$2

    # clean
    echo "Clean ..."
    rm -rf $release_dir
    mkdir $release_dir
    chmod 755 $release_dir

    cd $release_dir
    release_dir=$PWD
    cd - > /dev/null

    copy_files $release_dir $files_list
    strip_files $release_dir
   
    #others
    mkdir $release_dir/tmp
    chmod 777 $release_dir/tmp

    echo "Done."
}
main $*

