
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgncs/mgncs.h>
#include "register.h"

#include "SettingActivity.hh"
#include "museritem.h"
#include "mnavigationpanelpiece.h"
#include "SettingMainWinPiece.hh"
#include "SettingSubWinPiece.hh"

#define MSG_SETTING_SUB_WIN 1234

static mItemPiece *subWinCreateItemPiece(mSettingSubWinPiece *self, int row);
static int getCurItemPieceRow(StringArray candidacy,std::string cur);

static void mSettingSubWinPiece_construct(mSettingSubWinPiece *self, DWORD add_data)
{
    Class(mTableViewPiece).construct((mTableViewPiece *)self, NCS_TABLEVIEW_INDEX_STYLE);

    self->itemname = (char *)add_data;
    self->candidacy = GET_SETTINGSERVICE()->getCandidacy(self->itemname);
}

static int mSettingSubWinPiece_numberOfRowsInSection(mSettingSubWinPiece *self, int section)
{
    return self->candidacy.size();
}

static mTableViewItemPiece *mSettingSubWinPiece_createItemForRow(mSettingSubWinPiece *self, const mIndexPath *indexpath){
    int row;
    RECT rc;

    row = indexpath->row;
    mTableViewItemPiece *item = NEWPIECEEX(mTableViewItemPiece,0);
    SetRect(&rc, 0, 0, SETTING_ITEM_W, SETTING_ITEM_H);

    _c(item)->setRect(item, &rc);
    _c(item)->setUserPiece(item, (mHotPiece *)subWinCreateItemPiece(self, row));

    return item;
}

static void onClickReloadData(mWidget* _self, int message, WPARAM wParam, LPARAM lParam) 
{
    mSettingSubWinPiece *self = (mSettingSubWinPiece *)wParam;
    int row = (int)lParam;

    assert(message == MSG_SETTING_SUB_WIN);

    mSettingMainWinPiece *mainpiece = 
        (mSettingMainWinPiece *)SettingActivity::m_settingMainWinPiece;

    GET_SETTINGSERVICE()->setCurrent(self->itemname,self->candidacy[row]);
    //_c(self)->reloadData(self);
    _c(mainpiece)->myreloadData(mainpiece,TYPE_ALL); 
    return ;
}

static void mSettingSubWinPiece_rowDidSelectAtIndexPath(mSettingSubWinPiece *self, const mIndexPath *indexpath){
    mNavigationPanelPiece *nav = SettingActivity::getControllerHandle();

    int row = indexpath->row;

    assert(self->candidacy.size() > 0);
    std::string cur = GET_SETTINGSERVICE()->getCurrent(self->itemname);

    if(getCurItemPieceRow(self->candidacy,cur) != row)
    {
        mWidget* _w = _c(self)->getOwner(self);

#if 1
        ncsSetComponentHandler((mComponent*)_w, MSG_SETTING_SUB_WIN, (void *)onClickReloadData);
        PostMessage(_w->hwnd, MSG_SETTING_SUB_WIN, (WPARAM)self, (LPARAM)row);
#else
        mSettingMainWinPiece *mainpiece = 
            (mSettingMainWinPiece *)SettingActivity::m_settingMainWinPiece;

        GET_SETTINGSERVICE()->setCurrent(self->itemname,self->candidacy[row]);
        _c(self)->reloadData(self);
        _c(mainpiece)->myreloadData(mainpiece,TYPE_ALL); 
#endif
    }

    //fix me:pop when lbuttonup
    _c(nav)->pop(nav);
}

static void mSettingSubWinPiece_destroy(mSettingSubWinPiece *self)
{
    Class(mTableViewPiece).destroy((mTableViewPiece*)self);
}

static mItemPiece *subWinCreateItemPiece(mSettingSubWinPiece *self, int row){
    mItemPiece *item;
    int defaultvalue = 0;

    std::string cur = GET_SETTINGSERVICE()->getCurrent(self->itemname);
    assert(self->candidacy.size() > 0);

    if(getCurItemPieceRow(self->candidacy,cur) == row)
        defaultvalue = 1;

    item = CreateRadioItem(self->candidacy[row].c_str(), NULL,
            defaultvalue,0,NCS_ALIGN_LEFT);
    return item;
}

static int getCurItemPieceRow(StringArray candidacy,std::string cur){
    unsigned int i;

    for (i=0;i<candidacy.size();i++)
    {
        if (candidacy[i] == cur)
            return i;
    }
    return 0;
}

BEGIN_MINI_CLASS(mSettingSubWinPiece, mTableViewPiece)
    CLASS_METHOD_MAP(mSettingSubWinPiece, construct)
    CLASS_METHOD_MAP(mSettingSubWinPiece, numberOfRowsInSection)
    CLASS_METHOD_MAP(mSettingSubWinPiece, createItemForRow)
    CLASS_METHOD_MAP(mSettingSubWinPiece, rowDidSelectAtIndexPath)  
    CLASS_METHOD_MAP(mSettingSubWinPiece, destroy)
END_MINI_CLASS
