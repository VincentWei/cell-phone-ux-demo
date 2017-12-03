#ifndef _MGNCS_SLIDESWITCHPIECE_H
#define _MGNCS_SLIDESWITCHPIECE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _mSlideSwitchPiece mSlideSwitchPiece;
typedef struct _mSlideSwitchPieceClass mSlideSwitchPieceClass;

enum mSlideSwitchPieceState {
    NCSN_SLIDESWITCHPIECE_OFF = (USER_PIECE_EVENT_BEGIN + 0x300),
    NCSN_SLIDESWITCHPIECE_ON,
};

enum mSlideSwitchPieceProps {
    NCSP_SLIDESWITCH_RADIUS = USER_PIECE_PROP_BEGIN + 0x100,
};

#define mSlideSwitchPieceHeader(clss) \
	mPanelPieceHeader(clss) \
    unsigned radius; \
    enum mSlideSwitchPieceState state; \
    mHotPiece *leftBackgroundPiece; \
    mHotPiece *leftTextPiece; \
    mHotPiece *rightBackgroundPiece;\
    mHotPiece *rightTextPiece; \
    mHotPiece *buttonPiece;

struct _mSlideSwitchPiece
{
	mSlideSwitchPieceHeader(mSlideSwitchPiece)
};

#define mSlideSwitchPieceClassHeader(clss, superCls) \
	mPanelPieceClassHeader(clss, superCls) \
    int (*getState)(clss*); \
    void (*setState)(clss*, enum mSlideSwitchPieceState state); \
    void (*changeState)(clss*); \
    void (*resetState)(clss*); \
    void (*setLeftBkgnd)(clss*, int, int, mColorTable*, mColorTable*); \
    void (*setRightBkgnd)(clss*, int, int, mColorTable*, mColorTable*); \
    void (*setButton)(clss*, int, int, mColorTable*, mColorTable*); \
    void (*setLeftText)(clss*, const char*, PLOGFONT, ARGB); \
    void (*setRightText)(clss*, const char*, PLOGFONT, ARGB);

struct _mSlideSwitchPieceClass
{
	mSlideSwitchPieceClassHeader(mSlideSwitchPiece, mPanelPiece)
};

MGNCS_EXPORT extern mSlideSwitchPieceClass g_stmSlideSwitchPieceCls;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MGNCS_SLIDESWITCHPIECE_H */
