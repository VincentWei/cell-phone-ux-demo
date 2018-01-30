#ifndef MSHUTTERPIECE_H
#define MSHUTTERPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mShutterPieceClass mShutterPieceClass;
typedef struct _mShutterPiece mShutterPiece;

#define mShutterPieceClassHeader(clss, superCls) \
	mTextPieceClassHeader(clss, superCls) \

struct _mShutterPieceClass
{
	mShutterPieceClassHeader(mShutterPiece, mTextPiece)
};

MGNCS_EXPORT extern mShutterPieceClass g_stmShutterPieceCls;

#define mShutterPieceHeader(clss) \
	mTextPieceHeader(clss) \
    int shutter_width;    \
    int shutter_height;    \
    int offset_x;       \
    int offset_y;       \

struct _mShutterPiece
{
	mShutterPieceHeader(mShutterPiece)
};

enum mShutterPieceProps {
	NCSP_SHUTTERPIECE_WIDTH     = USER_PIECE_PROP_BEGIN + 10,
	NCSP_SHUTTERPIECE_HEIGHT,
};


#ifdef __cplusplus
}
#endif

#endif

