
#ifndef _SMSLIST_PIECE_H_
#define _SMSLIST_PIECE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SMSListData_t{
    char name[SMS_NAME_MAX_LEN];
    char smsbody[SMS_SIMPLE_MESSAGE_MAX_LEN];
    char time[SMS_NAME_MAX_LEN];
    struct SMSListData_t *next;
}SMSListData;

typedef struct _mSMSListPieceClass mSMSListPieceClass;
typedef struct _mSMSListPiece mSMSListPiece;

#define mSMSListPieceHeader(clss) \
    mTableViewPieceHeader(clss) \
    SMSActivity *act;\
    SMSListData *list_head; \
    SMSListData *list_end; \
    BOOL editMode;

#define mSMSListPieceClassHeader(clss, superCls) \
    mTableViewPieceClassHeader(clss, superCls)

struct _mSMSListPiece
{   
    mSMSListPieceHeader(mSMSListPiece)
};

struct _mSMSListPieceClass
{   
    mSMSListPieceClassHeader(mSMSListPiece, mTableViewPiece)
};

MGNCS_EXPORT extern mSMSListPieceClass g_stmSMSListPieceCls;

#ifdef __cplusplus
}
#endif
#endif /*_SMSLIST_PIECE_H_*/
