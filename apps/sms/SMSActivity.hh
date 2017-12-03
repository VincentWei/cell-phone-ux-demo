#ifndef _SMS_ACTIVITY_HH_
#define _SMS_ACTIVITY_HH_
#include "NCSActivity.hh"
#include "global.h"
#include "SmsService.hh"
#include "msmsdetailview.h"

#define SMS_IDC_CTNR   100
#define SMS_ITEM_MAX   7

#define SMS_TITLE_H   NAVIGATIONBAR_H
#define SMS_SEPARATOR_H 2
#define SMS_ITEM_H ((ACTIVITY_H - SMS_TITLE_H - \
        (SMS_ITEM_MAX - 1) * SMS_SEPARATOR_H)/SMS_ITEM_MAX)

#define SMS_ITEM_W ACTIVITY_W   
#define SMS_TABLE_H (ACTIVITY_H - SMS_TITLE_H)

#define SMS_BTN_W 60
#define SMS_BTN_H NAVIGATIONBAR_DEFAULT_BUTTON_H 

#define SMS_BTN_TEXT_H 16

#define SMS_BTN_COLOR 0xff192952 

#define SMS_MESSAGE_MAX_LEN 256
#define SMS_NAME_MAX_LEN 32
#define SMS_SIMPLE_MESSAGE_MAX_LEN 32

enum {
    SMS_LIST_ITEM,
    SMS_DETAIL_ITEM,
    SMS_MAX_ITEM,
};

class SMSActivity : public NCSActivity {
public:
    SMSActivity();
    ~SMSActivity();
    void createNavigatorBtns(mNavigationItem *self);

    void createCtnrBody(mContainerCtrl* ctnr,mNavigationItem *rootItem);

    void createSMSListWin(void);

    void createSMSDetailWin(char *name);

    static mNavigationPanelPiece *getNavigationHandle()
        {
            return m_nav;
        }
    mNavigationItem *getNavItem(int type)
        {
            if (type >= SMS_LIST_ITEM && type < SMS_MAX_ITEM)
                return m_navItems[type];
            return NULL;
        }

    void listEnterDetailWin(const mIndexPath* indexpath);

    int smsGetListNumberNo(void)
    {
        return m_keys.size();
    }
    int smsDeleteRow(int row)
    {
        std::vector<std::string>::iterator iter;
        assert(row < (signed)m_keys.size());
        iter = m_keys.begin();
        m_keys.erase(iter + row);
        return 0;
    }
    void smsGetListData(char *name,char *detail,char *time,int row);
    void initResource(void);
    void releaseResource(void);

public:
    mWidget* self;
    mHotPiece *m_editBtn;

    char m_contactName[SMS_NAME_MAX_LEN];
    SmsService::SmsSession m_session;
    Intent* m_phoneCallIntent;

protected:
    int onBack();

private:
    mHotPiece *createSMSListView(void);
    mHotPiece *createSMSDetailView(void);
    static mNavigationPanelPiece *m_nav;
    mNavigationItem *m_navItems[SMS_MAX_ITEM];    
    StringArray m_keys; 

    mHotPiece *m_smsListPiece;
    mHotPiece *m_smsDetailPiece;
    mHotPiece *m_barBack;
    PLOGFONT m_btnFont;
};
#endif/*end*/
