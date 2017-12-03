#ifndef _CALLHISTORY_ACTIVITY_HH_
#define _CALLHISTORY_ACTIVITY_HH_
#include "NCSActivity.hh"
#include "global.h"

//ctrl id
#define CH_IDC_CTNR 100

//max item per page
#define CH_ITEM_MAX   8

//view info
#define CH_TITLE_W   NAVIGATIONBAR_W
#define CH_TITLE_H   NAVIGATIONBAR_H
#define CH_SEPARATOR_H 2

#define CH_TABLE_W ACTIVITY_W
#define CH_TABLE_H (ACTIVITY_H - CH_TITLE_H - CH_TOOLBAR_H)

#define CH_TOOLBAR_W ACTIVITY_W
#define CH_TOOLBAR_H 49

#define CH_ITEM_W CH_TABLE_W   
#define CH_ITEM_H ((CH_TABLE_H - (CH_ITEM_MAX - 1) * CH_SEPARATOR_H) / CH_ITEM_MAX)

//navigation btn info
#define CH_BTN_ALLCALLS_X       5
#define CH_BTN_ALLCALLS_W       70
#define CH_BTN_MISSEDCALLS_X    (CH_BTN_ALLCALLS_X+CH_BTN_ALLCALLS_W-1)
#define CH_BTN_MISSEDCALLS_W    140
#define CH_BTN_CLEAR_W          60
#define CH_BTN_CLEAR_X          (CH_TITLE_W - CH_BTN_CLEAR_W - CH_BTN_ALLCALLS_X)
#define CH_BTN_Y    5
#define CH_BTN_H NAVIGATIONBAR_DEFAULT_BUTTON_H 
//btn preporty
#define CH_BTN_TEXT_H 16
#define CH_BTN_COLOR 0xc0192952 
//saparator
#define CH_BTN_SEPARATOR_X  (CH_BTN_MISSEDCALLS_X-1)
#define CH_BTN_SEPARATOR_Y  (CH_BTN_Y + ((CH_BTN_H - CH_BTN_SEPARATOR_H) / 2))
#define CH_BTN_SEPARATOR_W  1
#define CH_BTN_SEPARATOR_H  (CH_BTN_H - 2)

typedef enum callhistory_win_type_e {
    TYPE_ALL_CALLS_WND,
    TYPE_MISSED_CALLS_WND,
    TYPE_MAX,
} callhistory_win_type;

typedef enum callhistory_btn_e {
    BTN_ALLCALLS,
    BTN_MISSEDCALLS,
    BTN_CLEAR,
    BTN_NUM,
} callhistory_btn;

class CallHistoryActivity : public NCSActivity {
public:
    CallHistoryActivity();
    ~CallHistoryActivity();

    //create wnd element
    void createNavBtns(mWidget*);
    void createBody(mWidget*, mContainerCtrl *);

    mHotPiece *createTitle(mWidget*);
    mHotPiece *createTable(mWidget*);
    mHotPiece *createToolbar(mWidget*);

    void initResource(void);
    void releaseResource(void); 

    int callDeleteRow(CallLogService::CallLog *data,int row)
    {
        std::vector<CallLogItem>::iterator iter;
        assert(row < (signed)data->size());
        iter = data->begin();
        data->erase(iter + row);
        return 0;
    }

public:
    mHotPiece* m_editButton;
    static mHotPiece *m_tableview;
    static Intent *m_intent;
    mHotPiece *m_btn[BTN_NUM];

private:
    mHotPiece *m_separator;
    PLOGFONT m_btnFont;
};

#endif/*end*/

