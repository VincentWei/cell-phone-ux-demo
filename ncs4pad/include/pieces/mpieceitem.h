/*!============================================================================
 * @file mpieceitem.h 
 * @Synopsis mPieceItem 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  MPIECEITEM_INC
#define  MPIECEITEM_INC

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mNormalVerctor
{
    float angle;
    int x;
    int y;
    int z;
} mNormalVector;

typedef struct _mPieceItemClass mPieceItemClass;
typedef struct _mPieceItem mPieceItem;

#define mPieceItemHeader(clss) \
    mObjectHeader(clss) \
    list_t list;    \
    int x;  \
    int y;  \
    mHotPiece *piece; \
    float wscalefactor; \
    float hscalefactor; \
    int alpha; \
    mNormalVector normalVector; \
    HDC cacheDC; \
    BOOL isEnableCache; \
    BOOL underLayout;\
    int type;

#define mPieceItemClassHeader(clss, superCls) \
    mObjectClassHeader(clss, superCls)  \
    void (*setPiece)(clss*, mHotPiece*); \
    mHotPiece* (*getPiece)(clss*); \
    void (*setX)(clss*, int);    \
    void (*setY)(clss*, int);    \
    int  (*getX)(clss*);    \
    int  (*getY)(clss*);    \
    int  (*getType)(clss*);    \
    void (*setType)(clss*, int);

struct _mPieceItem
{   
    mPieceItemHeader(mPieceItem)
};

struct _mPieceItemClass
{   
    mPieceItemClassHeader(mPieceItem, mObject)
};

MGNCS_EXPORT extern mPieceItemClass g_stmPieceItemCls;

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef MPIECEITEM_INC  ----- */

