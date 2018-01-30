
/*!============================================================================
 * @file TimeSettingActivity.hh 
 * @Synopsis  
 * @author DengMin
 * @version 1.0
 * @date 04/11/2010 17:06:35 
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef TIMESETTINGACTIVITY_HH
#define TIMESETTINGACTIVITY_HH

#include "NCSActivity.hh"

class TimeSettingActivity : public NCSActivity {

    public:
        // constructor & destructor
        TimeSettingActivity();
        ~TimeSettingActivity();

    //private:
        // private method----------------------------------------------------

        // initialize helper functions
        void loadMyResource();
        void unloadMyResource();
        void initNavbar();
        void initSurround();
        void initDatePicker();
        void initTimePicker();

        // actions
        void switchToTimePicker();
        void switchToDatePicker();
        void switchWidget(mWidget* from, mWidget* to);
        void enterSettings();

    //private:
        // private data----------------------------------------------------

        // resource
        // font
        PLOGFONT m_navbarTitleFont;
        PLOGFONT m_navbarBtnFont;
        PLOGFONT m_btnNameFont;
        PLOGFONT m_btnContentFont;
        PLOGFONT m_datePickerTitleFont;
        PLOGFONT m_datePickerNumFont;
        PLOGFONT m_timePickerTitleFont;
        PLOGFONT m_timePickerNumFont;

        enum BTNBAR_INDEX {
            BTNBAR_DATE = 0,
            BTNBAR_TIME,

            BTNBAR_NUM,
        };

        enum BTNLABEL_INDEX {
            BTNLABEL_NAME = 0,
            BTNLABEL_CONTENT,

            BTNLABEL_NUM,
        };

        // main window
        mWidget* self;

        // control & piece
        mContainerCtrl* m_navbarCtrl;
        mPanelPiece* m_navbarContainer;
        mHotPiece* m_navbarBtnPiece;

        mContainerCtrl* m_surroundCtrl;
        mPanelPiece* m_surroundContainer;

        //mPanelPiece* m_btnBar[BTNBAR_NUM];
        mButtonPanelPiece* m_btnBar[BTNBAR_NUM];
        mTextPiece* m_btnLabel[BTNBAR_NUM][BTNLABEL_NUM];

        mContainerCtrl* m_datePickerCtrl;
        mPanelPiece* m_datePickerContainer;
        mDatePicker* m_datePicker;

        mContainerCtrl* m_timePickerCtrl;
        mPanelPiece* m_timePickerContainer;
        mTimePicker* m_timePicker;

        enum State {
            IN_DATE_PICKER = 0,
            IN_TIME_PICKER,
            IN_PICKER_SWITCH,
        };
        int m_state;

        time_t m_currentTime;
};

#endif
