
#ifndef _NCSCTRL_ANIMATINEDIT_H
#define _NCSCTRL_ANIMATINEDIT_H

#include <mgncs4touch/mgncs4touch.h>

#define NCSCTRL_ANIMATIONEDIT   NCSCLASSNAME("animationedit")

enum mAnimationEditProp {
    NCSP_ANIMATIONEDIT_RADIUS = NCSP_WIDGET_MAX + 1,
    NCSP_ANIMATIONEDIT_BKCOLOR,
    NCSP_ANIMATIONEDIT_BORDERSIZE,
    NCSP_ANIMATIONEDIT_BORDERCOLOR,
    NCSP_ANIMATIONEDIT_FILLMODE,
    NCSP_ANIMATIONEDIT_CORNERFLAG,
    NCSP_ANIMATIONEDIT_TEXTCOLOR
};

#define NCSS_ANIMATIONEDIT_BKGRADIENT (1<<NCSS_WIDGET_SHIFT)

DECLARE_OBJECT(mAnimationEdit)

#define mAnimationEditHeader(clsName) \
        mWidgetHeader(clsName) \
        char *text; /*save string*/ \
        int content_length; \
        int text_offest; \
        PLOGFONT *pFont; \
        ARGB text_color;

#define mAnimationEditClassHeader(clsName, parentClass) \
        mWidgetClassHeader(clsName, parentClass) \
        void (*setContent)(mAnimationEdit *self, const char* str, const char *effectorname); \
        int (*getContent)(mAnimationEdit *self, char *strbuff, int buf_len); \
        void (*append)(mAnimationEdit *self, const char* str); \
        void (*tailDelete)(mAnimationEdit *self, int num); \
        HBRUSH (*getBKBrush)(mAnimationEdit *self); \
        BOOL (*setGradientFillColors)(mAnimationEdit *self, ARGB *colors, int num);

typedef mWidgetRenderer mAnimationEditRenderer;

DEFINE_OBJECT(mAnimationEdit, mWidget)


#endif /*_NCSCTRL_ANIMATINEDIT_H*/
