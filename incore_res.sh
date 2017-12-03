#! /bin/sh

INNER_RES_PATH="inner-res"
FILE_LST="files.lst"

if [ "x$1" = "xclear" ]; then
    echo "clean incore res...."
    cd ${INNER_RES_PATH}
    rm -rf *.png.c *.bmp.c *.gif.c *.jpg.c _pmp_inner_res.c
else
    echo "make incore res...." 
    #find . -name \*.png -or -name \*.bmp -or -name \*.gif  -or -name \*.jpg | grep -v res/gvfb | grep -v test > ${FILE_LST}
    find . -regex ".*\.\(jpg\|png\|jpg\|gif\)" | grep -v res/gvfb | grep -v test > ${FILE_LST}
    sed  's/\.\///' ${FILE_LST} -i
    ${INNER_RES_PATH}/inner-res-trans -u -m _MGINCORE_RES -c minigui/common.h -i . -o ./inner-res -p __mgpmp -n _pmp_inner_res -l ${FILE_LST}

    echo "make incore mgetc.c...."
    ${INNER_RES_PATH}/mgcfg-trans -o mgetc.c ./MiniGUI.cfg
    #sed 's/#include\ "common.h"/#include\ "mgetc\.h"/' -i ${INNER_RES_PATH}/mgetc.c
    #sed 's/#include\ "minigui.h"//' -i ${INNER_RES_PATH}/mgetc.c
    #sed 's/__mg_get_mgetc/CellphoneGetInnerEtc/' -i ${INNER_RES_PATH}/mgetc.c

    echo "!!!Warning: Copy ./mgetc.c to minigui/src/sysres/, and rebuild minigui."
    echo 
fi
