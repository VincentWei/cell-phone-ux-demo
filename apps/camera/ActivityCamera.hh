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
#ifndef ACTIVITYCAMERA_HH
#define ACTIVITYCAMERA_HH

#include "global.h"

#include "NCSActivity.hh"

class ActivityCamera : public NCSActivity {
    public:
        ActivityCamera();
        ~ActivityCamera();
        mButtonPanelPiece *playPiece(void)const { return m_playPiece; }
        mButtonPanelPiece *shooterPiece(void)const { return m_shooterPiece; }
        void setPlayPiece(mButtonPanelPiece *piece) { m_playPiece = piece; }
        void setShooterPiece(mButtonPanelPiece *piece) { m_shooterPiece = piece; }
        void createBody(mWidget*);
        mPanelPiece *createView(mWidget *);
        mPanelPiece* createToolbar(mWidget *);
        void createAnimation(mWidget *);
        void onView(mWidget *, int);
        HDC getImage(void){return m_image;}
        void setAnim(MGEFF_ANIMATION anim){ m_anim = anim; }
        MGEFF_ANIMATION getAnim(void){return m_anim;}
        virtual int onResume();
    protected:
        void loadResource();
        void unloadResource();
    public:
        Intent * m_intent;
        int m_count;
        BOOL m_isShooting;
    private:
        mButtonPanelPiece *m_playPiece;
        mButtonPanelPiece *m_shooterPiece;
        HDC m_image;
        MGEFF_ANIMATION m_anim;
};

#endif /* ACTIVITYCAMERA_HH */
