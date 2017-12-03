#ifndef _SETTING_ACTIVITY_HH_
#define _SETTING_ACTIVITY_HH_
#include "NCSActivity.hh"
#include "global.h"

#define SETTING_IDC_CTNR   100
#define SETTING_TIMER   108
#define SETTING_ITEM_MAX   8

#define SETTING_TITLE_H   NAVIGATIONBAR_H
#define SETTING_SEPARATOR_H 2
#define SETTING_ITEM_H ((ACTIVITY_H - SETTING_TITLE_H - \
        (SETTING_ITEM_MAX - 1) * SETTING_SEPARATOR_H)/SETTING_ITEM_MAX)

#define SETTING_ITEM_W ACTIVITY_W   
#define SETTING_TABLE_H (ACTIVITY_H - SETTING_TITLE_H)

#define SETTING_OK_BTN_W 60
#define SETTING_OK_BTN_H NAVIGATIONBAR_DEFAULT_BUTTON_H 

#define SETTING_OK_BTN_TEXT_H 16
#define SETTING_ITEM_TEXT_H 18

#define SETTING_OK_BTN_COLOR 0xff192952 

#include "SettingService.hh"
typedef enum setting_win_type_e{
    TYPE_MAIN_WIN_UI,
    TYPE_SUB_WIN_UI,
    TYPE_CLOCK_WIN_UI,
    TYPE_WIN_MAX_NUM
}setting_win_type;

#define SETTING_STR_MAX_LEN 32

class SettingActivity : public NCSActivity {
    public:
        SettingActivity();
        ~SettingActivity();
        void createNavigatorBtns(mNavigationItem* self);
        void createCtnrBody(mContainerCtrl* ctnr,mNavigationItem *rootItem);
        void createUIMainWin(HWND hwnd);
        void createUISubWin(const char *name);
        void createUIClockWin(HWND hwnd);
        void initClockWin(HWND hwnd);
        void releaseClockView(HWND hwnd);
        static mNavigationPanelPiece *getControllerHandle()
        {
            return m_nav;
        }
        static mNavigationItem *getNavItem(setting_win_type type)
        {
            if (type >= TYPE_MAIN_WIN_UI && type < TYPE_WIN_MAX_NUM)
                return m_navItems[type];
            return NULL;
        }
        virtual int onStop();
        void initResource(void);
        void releaseResource(void); 

    private:
        mHotPiece *createMainWinView(void);
        mHotPiece *createSubWinView(const char *name);
        mHotPiece *createClockView(HWND hwnd);
    public:
        static StringArray m_settingData;
        static mHotPiece *m_settingMainWinPiece;
        static mHotPiece *m_settingSubWinPiece;

protected:
    int onBack();

    private:
        static mNavigationPanelPiece *m_nav;
        static mNavigationItem *m_navItems[TYPE_WIN_MAX_NUM];    
        mHotPiece *m_okBtn;
        mHotPiece *m_barBack;
        PLOGFONT m_btnFont;
        PLOGFONT m_itemTextFont;
};
#endif/*end*/
