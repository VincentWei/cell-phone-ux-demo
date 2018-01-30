#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "global.h"
#include "register.h"

#include "SettingActivity.hh"
#include "SettingMainWinPiece.hh"
#include "SettingSubWinPiece.hh"
#include "TimeService.hh"

static BOOL updateClockValue(void* target, TimeService *ts, int eventId, DWORD param)
{
    mSettingMainWinPiece *self = (mSettingMainWinPiece *)target;
    _c(self)->myreloadData(self,TYPE_CLOCK);
    return TRUE;
}

static const char *getStringFromRow(mSettingMainWinPiece *self,int row)
{
    std::vector<std::string>::iterator it;
    for(it = self->data->begin();it != self->data->end();it ++,row --)
    {
        if (row == 0)
            break;
    }
    if (it != self->data->end())
    {
        //return *it;
        return (*it).c_str();
    }
    else
    {
        assert(0);
        return NULL;
    }
}

static void mSettingMainWinPiece_construct(mSettingMainWinPiece *self, DWORD add_data)
{
    Class(mTableViewPiece).construct((mTableViewPiece*)self, NCS_TABLEVIEW_INDEX_STYLE);
    self->act = (SettingActivity *)add_data;
    self->data = &(SettingActivity::m_settingData); 
    self->cur_data = (char (*)[SETTING_STR_MAX_LEN])malloc(self->data->size()*SETTING_STR_MAX_LEN);
    assert(self->cur_data);
    assert(self->act);
    TIMESERVICE->addEventListener((void *)self,TimeService::MINUTE,updateClockValue);
    TIMESERVICE->addEventListener((void *)self,TimeService::HOUR,updateClockValue);
}

static int mSettingMainWinPiece_numberOfSections(mSettingMainWinPiece* self)
{
    return 1;
}
static int mSettingMainWinPiece_numberOfRowsInSection(mSettingMainWinPiece* self, int section)
{
    return self->data->size();
}

static BOOL tableitem_event_cb(mWidget *_self, mHotPiece *piece,
        int event_id, DWORD param)
{
    unsigned int row = (unsigned int)param;
    mSettingMainWinPiece *self = (mSettingMainWinPiece *)_self;

    ExtraType extra_type;
    const char *rowdata;

    assert(row < self->data->size());
    rowdata = getStringFromRow(self,row);

    extra_type = GET_SETTINGSERVICE()->getItemType(rowdata);
    if (extra_type == TYPE_TEXT)
    {
    }
    else if (extra_type == TYPE_CHECKBOX)
    {
        if (event_id == NCSN_ITEMPIECE_CHECKBOX_ON)
        {
            GET_SETTINGSERVICE()->setCurrent(rowdata,"on");
        }
        else if (event_id == NCSN_ITEMPIECE_CHECKBOX_OFF)
        {
            GET_SETTINGSERVICE()->setCurrent(rowdata,"off");
        }
    }
    else if (extra_type == TYPE_INDICATOR)
    {
    /*
        mSettingSubWinPiece *subpiece;
        mNavigationItem *nextItem;
        mNavigationPanelPiece *nav = SettingActivity::getControllerHandle();

        self->act->createUISubWin(rowdata);
        subpiece = (mSettingSubWinPiece *)self->act->m_settingSubWinPiece;
        assert(subpiece);
        subpiece->itemname = rowdata.c_str(); 
        assert(!rowdata.empty());
        subpiece->candidacy = GET_SETTINGSERVICE()->getCandidacy(rowdata); 
        _c(subpiece)->reloadData(subpiece); 

        nextItem = SettingActivity::getNavItem(TYPE_SUB_WIN_UI);
        assert(nextItem);
        _c(nav)->push(nav,nextItem);
    */
    }
    else if (extra_type == TYPE_EDIT)
    {
    }
    return TRUE;
}

static mItemPiece *settingCreateUserPiece(mSettingMainWinPiece* self,const mIndexPath* indexpath)
{
    unsigned int row = (unsigned)indexpath->row;
    mItemPiece *setitem = NULL;
    
    ExtraType extra_type;
    
    const char* rowdata;

    assert(row < self->data->size());
    rowdata = getStringFromRow(self,row);
    extra_type = GET_SETTINGSERVICE()->getItemType(rowdata);


    if (extra_type == TYPE_TEXT)
    {
        setitem = CreateEditItem(rowdata,self->cur_data[row]);
    }
    else if (extra_type == TYPE_CHECKBOX)
    {
        int event_ids[] = {NCSN_ITEMPIECE_CHECKBOX_ON ,NCSN_ITEMPIECE_CHECKBOX_OFF , 0};

        if (GET_SETTINGSERVICE()->getCurrent(rowdata) == "on")
        {
            setitem = CreateSwitchCtrlItemEx(rowdata,NULL, 1,row);
        }
        else
        {
            setitem = CreateSwitchCtrlItemEx(rowdata,NULL, 0,row);
        }

        ncsAddEventListeners((mObject*)setitem, (mObject*)self,
                (NCS_CB_ONPIECEEVENT)tableitem_event_cb, event_ids);
    }
    else if (extra_type == TYPE_INDICATOR)
    {
        setitem = CreateDetailBtnItemEx(rowdata,self->cur_data[row], 1,row);
    /*
        ncsAddEventListener((mObject*)setitem, (mObject*)self,
                (NCS_CB_ONPIECEEVENT)tableitem_event_cb,NCSN_ITEMPIECE_BUTTON_CLICKED);
    */
    }
    else if (extra_type == TYPE_EDIT)
    {
        setitem = CreateEditItem(rowdata,self->cur_data[row]);
    }
    else
    {
        printf("========%s======TYPE error\n",__FUNCTION__);
        setitem = CreateEditItem(rowdata,"data error");
        /*assert(0);*/
    }

    return setitem;
}

static mTableViewItemPiece* mSettingMainWinPiece_createItemForRow(
                mSettingMainWinPiece* self, const mIndexPath* indexpath)
{
    mTableViewItemPiece *item = NEWPIECEEX(mTableViewItemPiece,0);

    RECT rc = {0,0,SETTING_ITEM_W,SETTING_ITEM_H};

    assert(item);
    _c(item)->setRect(item,&rc);

    mItemPiece *setitem  = settingCreateUserPiece(self,indexpath);
    if (setitem == NULL)
    {
        printf("===%s==create settting item error row = %d\n",__FUNCTION__,indexpath->row);
        return NULL;
    }
    _c(setitem)->setRect(setitem,&rc);

    _c(item)->setUserPiece(item,(mHotPiece*)setitem);
    return item;
}

static void mSettingMainWinPiece_rowDidSelectAtIndexPath(
        mSettingMainWinPiece* self, const mIndexPath* indexpath)
{
    unsigned int row = (unsigned)indexpath->row;
    
    ExtraType extra_type;
    const char* rowdata;

    assert(row < self->data->size());
    rowdata = getStringFromRow(self,row);
    
    extra_type = GET_SETTINGSERVICE()->getItemType(rowdata);
    
    if (extra_type == TYPE_INDICATOR)
    {
        mSettingSubWinPiece *subpiece;
        mNavigationItem *nextItem;
        mNavigationPanelPiece *nav = SettingActivity::getControllerHandle();

        self->act->createUISubWin(rowdata);
        subpiece = (mSettingSubWinPiece *)self->act->m_settingSubWinPiece;
        assert(subpiece);
        subpiece->itemname = rowdata; 
        subpiece->candidacy = GET_SETTINGSERVICE()->getCandidacy(rowdata); 
        _c(subpiece)->reloadData(subpiece); 

        nextItem = SettingActivity::getNavItem(TYPE_SUB_WIN_UI);
        assert(nextItem);
        _c(nav)->push(nav,nextItem);
    }
    else if (extra_type == TYPE_EDIT)
    {
        if (strcmp(rowdata,"Clock") == 0)
        {
            mNavigationItem *nextItem;
            mNavigationPanelPiece *nav = SettingActivity::getControllerHandle();
            self->act->createUIClockWin(self->act->hwnd());
            nextItem = SettingActivity::getNavItem(TYPE_CLOCK_WIN_UI);
            assert(nextItem);
            _c(nav)->push(nav,nextItem);
        }
    }
}

static BOOL mSettingMainWinPiece_willSelectRowAtIndexPath(
        mSettingMainWinPiece* self, const mIndexPath* indexpath)
{
    unsigned int row = (unsigned)indexpath->row;
    ExtraType extra_type;
    const char* rowdata;

    assert(row < self->data->size());
    rowdata = getStringFromRow(self,row);
    extra_type = GET_SETTINGSERVICE()->getItemType(rowdata);
    if (extra_type == TYPE_CHECKBOX)
    {
        return FALSE;
    }
    return TRUE;
}

static void mSettingMainWinPiece_myreloadData(mSettingMainWinPiece *self,MY_RELOAD_TYPE type)
{
    int i;
    for (i=0;i<(signed)self->data->size();i++)
    {
        memset(self->cur_data[i],0x00,SETTING_STR_MAX_LEN);
        if (i == 0)
        {
            TIMESERVICE->format(self->cur_data[i], SETTING_STR_MAX_LEN, "%H:%M");
            if (type == TYPE_CLOCK)
                break;
        }
        else
        {
            const char *rowdata = getStringFromRow(self,i);
            strncpy(self->cur_data[i],
                GET_SETTINGSERVICE()->getCurrent(rowdata).c_str(),SETTING_STR_MAX_LEN - 1);
        }
    }
    _c(self)->reloadData(self);
    return;
}

static void mSettingMainWinPiece_destroy(mSettingMainWinPiece *self)
{
    TIMESERVICE->removeEventListener((void*)self);
    Class(mTableViewPiece).destroy((mTableViewPiece*)self);
    free(self->cur_data);
}

BEGIN_MINI_CLASS(mSettingMainWinPiece, mTableViewPiece)
	CLASS_METHOD_MAP(mSettingMainWinPiece, construct)
	CLASS_METHOD_MAP(mSettingMainWinPiece, numberOfSections)
	CLASS_METHOD_MAP(mSettingMainWinPiece, numberOfRowsInSection)
	CLASS_METHOD_MAP(mSettingMainWinPiece, createItemForRow)
	CLASS_METHOD_MAP(mSettingMainWinPiece, rowDidSelectAtIndexPath)
	CLASS_METHOD_MAP(mSettingMainWinPiece, willSelectRowAtIndexPath)
	CLASS_METHOD_MAP(mSettingMainWinPiece, myreloadData)
	CLASS_METHOD_MAP(mSettingMainWinPiece, destroy)
END_MINI_CLASS

