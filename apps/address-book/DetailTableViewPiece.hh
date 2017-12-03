#ifndef __CONTACTSDETAIL_HH__
#define __CONTACTSDETAIL_HH__

#ifdef __cplusplus
extern "C" {
#endif

#include "mtableviewpiece.h"

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
