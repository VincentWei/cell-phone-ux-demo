///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
#ifndef  MSIMPLETABLEVIEW_INC
#define  MSIMPLETABLEVIEW_INC

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mSmsDetailView mSmsDetailView;
typedef struct _mSmsDetailViewClass mSmsDetailViewClass;

#define mSmsDetailViewHeader(clss) \
    mTableViewPieceHeader(clss)	\
    const char* name;	\
    void *act; \
    PBITMAP my_icon;	\
    char *my_icon_path; \
    PBITMAP sender_icon; \
    char *sender_icon_path; 

#define mSmsDetailViewClassHeader(clss, superCls) \
    mTableViewPieceClassHeader(clss, superCls)

struct _mSmsDetailView
{   
    mSmsDetailViewHeader(mSmsDetailView)
};

struct _mSmsDetailViewClass
{   
    mSmsDetailViewClassHeader(mSmsDetailView, mTableViewPiece)
};

MGNCS_EXPORT extern mSmsDetailViewClass g_stmSmsDetailViewCls;

#ifdef __cplusplus
}
#endif

#endif

