
#ifndef _ACTIVITYCONTACTS_HH
#define _ACTIVITYCONTACTS_HH

#include "ContactsLoadHelper.hh"
#include "DetailTableViewPiece.hh"
#include "NCSActivity.hh"
#include "../common/mactivitytoolbarpiece.h"

#define TITLE_NAME_LEN 8
#define GROUPSIZE 26
#define CONTACT_DEFAULT_PHOTO "res/address-book/address_book_unknow.png"

#define IDC_NAV 200

#define VIEW_H (ACTIVITY_H - NAVIGATIONBAR_H)
#define VIEW_W ACTIVITY_W
#define TOOLBAR_H 49 

#define DETAIL_PHONENUM_Y  (192 - NAVIGATIONBAR_H)
#define DETAIL_PHOTO_Y  (88 - NAVIGATIONBAR_H)
#define DETAIL_PHOTO_H  88
#define DETAIL_PHOTO_W  88

#define DETAIL_FONT_SIZE  26

#define CONSTACT_DETAILTABLE_ITEM_H ((VIEW_H - DETAIL_PHONENUM_Y - TOOLBAR_H) / 5) 

typedef struct{
    std::string titlename;
    ContactsGroup group;
}ContactsSection;

typedef  std::vector<ContactsSection*> ContactsData;

class PhoneContactsActivity : public NCSActivity {

    public:
        PhoneContactsActivity();
        ~PhoneContactsActivity();

        int initContacts ();
        int uninitContacts ();
        int createNavigation (HWND ctrl_hwnd);
        int destroyNavigation (HWND ctrl_hwnd);
        void updateDetailNavItem (ContactsInfo *info);

        mNavigationPanelPiece *getNavigationController() const{return m_nav;}
        mNavigationItem *getContactNavItem () const{ return m_navItems[NAVITEM_CONTACT]; }
        mNavigationItem *getDedailNavItem () const{ return m_navItems[NAVITEM_DETAIL]; }

        ContactsData *getContactsData() {return &m_contactsData; }
        Intent *getIntent () const { return m_intent; }

    private:

        void loadContactGroupFromLib ();
        void unloadContactsGroup();
        void loadContactsPhoto();
        void unloadContactsPhoto();
        void loadResource();
        void unloadResource();

        mPanelPiece *createContactView();
        mPanelPiece *createDetailView();
        mActivityToolbarPiece *createToolbar ();

    protected:
        int onBack();

    private:
        enum {
            NAVITEM_CONTACT,
            NAVITEM_DETAIL,
            NAVITEM_MAX,
        };

        Intent *m_intent;
        mNavigationPanelPiece *m_nav;
        mNavigationItem *m_navItems [NAVITEM_MAX];
        mPanelPiece *m_viewPiece[NAVITEM_MAX];
        ContactsData m_contactsData;

        mItemPiece *m_detailNameItem;
        mDetailTableViewPiece *m_detailTableViewPiece;

        PLOGFONT m_font;
};

#endif /* _ACTIVITYCONTACTS_HH */
