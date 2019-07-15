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

#ifndef _CALLHISTORYTABLEVIEWPIECE_H_
#define _CALLHISTORYTABLEVIEWPIECE_H_

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#define TIME_MAX_LEN    26

typedef struct _mCallHistoryTableViewPieceClass mCallHistoryTableViewPieceClass;
typedef struct _mCallHistoryTableViewPiece mCallHistoryTableViewPiece;

#define mCallHistoryTableViewPieceHeader(clss) \
    mTableViewPieceHeader(clss) \
    CallLogService::CallLog *data;  \
    std::string* name;   \
    int ismissed;   \
    BOOL isSelect;  \
    char (*t)[TIME_MAX_LEN];\
    CallHistoryActivity* activity;

#define mCallHistoryTableViewPieceClassHeader(clss, superCls) \
    mTableViewPieceClassHeader(clss, superCls)

struct _mCallHistoryTableViewPiece
{   
    mCallHistoryTableViewPieceHeader(mCallHistoryTableViewPiece)
};

struct _mCallHistoryTableViewPieceClass
{   
    mCallHistoryTableViewPieceClassHeader(mCallHistoryTableViewPiece, mTableViewPiece)
};

MGNCS_EXPORT extern mCallHistoryTableViewPieceClass g_stmCallHistoryTableViewPieceCls;

#ifdef __cplusplus
}
#endif

#endif /*_CALLHISTORYTABLEVIEWPIECE_H_*/
