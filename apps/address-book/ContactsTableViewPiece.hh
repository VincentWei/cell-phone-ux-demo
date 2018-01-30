

#ifndef _CONTACTSTABLEVIEW_H
#define _CONTACTSTABLEVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mContactsTableViewPieceClass mContactsTableViewPieceClass;
typedef struct _mContactsTableViewPiece mContactsTableViewPiece;

#define CONTACTTABLEVIEW_GROUPMAX 26

#define mContactsTableViewPieceHeader(clss) \
    mTableViewPieceHeader(clss) \
    int key_stack_len; \
    PhoneContactsActivity *activity;

#define mContactsTableViewPieceClassHeader(clss, superCls) \
    mTableViewPieceClassHeader(clss, superCls)

struct _mContactsTableViewPiece
{   
    mContactsTableViewPieceHeader(mContactsTableViewPiece)
};

struct _mContactsTableViewPieceClass
{
    mContactsTableViewPieceClassHeader(mContactsTableViewPiece, mTableViewPiece)
};

MGNCS_EXPORT extern mContactsTableViewPieceClass g_stmContactsTableViewPieceCls;


#ifdef __cplusplus
}
#endif
#endif /* _CONTACTSTABLEVIEW_H */
