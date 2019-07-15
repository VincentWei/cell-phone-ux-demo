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
#ifndef ACTIVITYPICTUREPREVIEWER_HH
#define ACTIVITYPICTUREPREVIEWER_HH

#include "global.h"
#include "NCSActivity.hh"

class ActivityPicturePreviewer : public NCSActivity {
    public:
        ActivityPicturePreviewer();
        ~ActivityPicturePreviewer();
        PLOGFONT getTitleFont() {return m_titleFont;}
        PLOGFONT getTitleBtnFont() {return m_titleBtnFont;}
    protected:
        void loadResource();
        void unloadResource();
    public:
       Intent * m_intent;
    private:
       PLOGFONT m_titleFont; 
       PLOGFONT m_titleBtnFont; 
};

#endif /* ACTIVITYPICTUREPREVIEWER_HH */
