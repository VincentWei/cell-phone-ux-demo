#ifndef _mWidgetHostPiece_H_
#define _mWidgetHostPiece_H_

#define WIDGETHOST_FROM_DESKTOP 0
typedef struct _mWidgetHostPiece mWidgetHostPiece;
typedef struct _mWidgetHostPieceClass mWidgetHostPieceClass;

#define mWidgetHostPieceHeader(clss)	\
	mPanelPieceHeader(clss)	\
	HDC memDC;	     \
    mWidget* hosted; \
    mObject *hostedPiece;   \
    BOOL bCaptureMouse; \
	NCS_MNWND_TEMPLATE *tmpl;\
    HWND main_hwnd;

#define mWidgetHostPieceClassHeader(clss, superCls)	\
	mPanelPieceClassHeader(clss, superCls)	\
	mWidget *(*getHosted)(mWidgetHostPiece *);  \
    void *(*setHosted)(clss*, NCS_WND_TEMPLATE*);

struct _mWidgetHostPiece
{
	mWidgetHostPieceHeader(mWidgetHostPiece)
};

struct _mWidgetHostPieceClass
{
	mWidgetHostPieceClassHeader(mWidgetHostPiece, mPanelPiece)
};

MGNCS_EXPORT extern mWidgetHostPieceClass g_stmWidgetHostPieceCls;


#endif

