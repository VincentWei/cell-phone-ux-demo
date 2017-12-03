#! /bin/bash

filelist=`find res | grep -v svn | sort`

for file in ${filelist}; do
    if [ -d ${file} ]; then
        echo "    /* ${file} */"
    else
        echo "    { \"${file}\", RES_TYPE_IMAGE, 16, 1, 1 },"
    fi
done

