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
#ifndef __CONTACTSDETAIL_HH__
#define __CONTACTSDETAIL_HH__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mDetailTableViewPieceClass mDetailTableViewPieceClass;
typedef struct _mDetailTableViewPiece mDetailTableViewPiece;

#define mDetailTableViewPieceClassHeader(clss, superCls)    \
    mTableViewPieceClassHeader(clss, superCls) \
    void (*setContactsData) (mDetailTableViewPiece *self, ContactsInfo *data);

struct _mDetailTableViewPieceClass
{
    mDetailTableViewPieceClassHeader(mDetailTableViewPiece, mTableViewPiece)
};

#define mDetailTableViewPieceHeader(clss)   \
    mTableViewPieceHeader(clss) \
    ContactsInfo *data; \
    DWORD act;

struct _mDetailTableViewPiece
{
    mDetailTableViewPieceHeader(mDetailTableViewPiece)
};

MGNCS_EXPORT extern mDetailTableViewPieceClass g_stmDetailTableViewPieceCls;

#ifdef __cplusplus
}
#endif

#endif
