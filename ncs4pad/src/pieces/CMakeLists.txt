mg_add_source_files(
    mabstractitemmanager.c
    manimationeditpiece.c
    mbuttonpanelpiece.c
    mexseparatorpiece.c
    mitemiterator.c
    mlistitemmanager.c
    mlistitemiterator.c
    mgroupitemiterator.c
    mgroupitemmanager.c
    mgrouppieceitem.c
    mpanelpiece.c
    mitempiece.c
    mwrappaintpiece.c
    mtableviewpiece.c
    mtableviewitempiece.c
    mphoneseparatorpiece.c
    mphonestaticrdrpiece.c
    mpieceitem.c
    mrotateswitchpiece.c
    mslideswitchpiece.c
    mcheckmarkpiece.c
    mradiopiece.c
    mindicatorpiece.c
    mindicatorbuttonpiece.c
    mtextpiece.c
    mtransroundpiece.c
    mindexlocatepiece.c
    mnavigationitem.c
    mnavigationbarpiece.c
    mnavigationpanelpiece.c
    m3dbuttonpiece.c
    mwidgethostpiece.c
)

# mtablepanelitempiece.c
# mtablepanelpiece.c

IF (enable_scrollview)
    mg_add_source_files(mscrollviewpiece.c)
ENDIF (enable_scrollview)
    
mg_commit_source_files()
