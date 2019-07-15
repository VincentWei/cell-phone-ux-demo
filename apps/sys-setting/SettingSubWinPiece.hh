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
#ifndef _SETTINGSUBWINPIECE_HH_
#define _SETTINGSUBWINPIECE_HH_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mSettingSubWinPieceClass mSettingSubWinPieceClass;
typedef struct _mSettingSubWinPiece mSettingSubWinPiece;

#define mSettingSubWinPieceClassHeader(clss, superCls)  \
    mTableViewPieceClassHeader(clss, superCls)

struct _mSettingSubWinPieceClass
{
    mSettingSubWinPieceClassHeader(mSettingSubWinPiece, mTableViewPiece)
};

#define mSettingSubWinPieceHeader(clss) \
    mTableViewPieceHeader(clss) \
    const char *itemname; \
    StringArray candidacy;

struct _mSettingSubWinPiece
{
    mSettingSubWinPieceHeader(mSettingSubWinPiece)
};

MGNCS_EXPORT extern mSettingSubWinPieceClass g_stmSettingSubWinPieceCls;
#ifdef __cplusplus
}
#endif

#endif
