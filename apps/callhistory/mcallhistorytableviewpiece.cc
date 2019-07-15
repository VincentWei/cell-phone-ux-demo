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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "global.h"
#include "register.h"


#include "CallLogService.hh"
#include "ContactService.hh"
#include "CallHistoryActivity.hh"
#include "mcallhistorytableviewpiece.hh"
#include "TimeService.hh"

enum timeDisplayType{
    CURRENT_TIME,
    BACK_WEEK_TIME,
    FAR_AWAY_TIME,
};
static enum timeDisplayType GetTimeType(time_t t); 
static void FormatTime(char *time,int len,time_t t);
    
static void mCallHistoryTableViewPiece_construct(mCallHistoryTableViewPiece *self, DWORD add_data)
{
    Class(mTableViewPiece).construct((mTableViewPiece*)self, NCS_TABLEVIEW_INDEX_STYLE);
    self->ismissed = FALSE;
    self->isSelect = TRUE;
    self->data = GET_CALLLOG_SERVICE()->getAllCallLog();
    self->t = (char (*)[TIME_MAX_LEN])malloc(self->data->size() * TIME_MAX_LEN);
    if(self->t == NULL){
        fprintf(stderr, "malloc error\n");
        return;
    }
    self->activity = NULL;
    self->name = new std::string[self->data->size()];
    assert(NULL != self->name);
}

static int mCallHistoryTableViewPiece_numberOfSections(mCallHistoryTableViewPiece* self)
{
    return 1;
}
static int mCallHistoryTableViewPiece_numberOfRowsInSection(mCallHistoryTableViewPiece* self, int section)
{
    return self->data->size();
}

static mItemPiece *CreateUserPiece(mCallHistoryTableViewPiece* self,const mIndexPath* indexpath)
{
    int row = indexpath->row;
    CallLogItem cl = self->data->at(row);
    mItemPiece *item;
    std::string *tmp;
    const char *title;

    FormatTime(self->t[row], TIME_MAX_LEN, cl.m_time);
    
    tmp = self->name + row;
    *tmp = GET_CONTACTSERVICE()->getNameByDialNum(cl.m_dial);
    if(tmp->empty()) {
        title = cl.m_dial.c_str();
    } else {
        title = tmp->c_str();
    }
    item = CreateDetailBtnItem(title, self->t[row], 1);

    return item;
}

static mTableViewItemPiece* mCallHistoryTableViewPiece_createItemForRow( mCallHistoryTableViewPiece* self, const mIndexPath* indexpath)
{
    mTableViewItemPiece *item;
    mItemPiece *chitem;
    RECT rc;

    SetRect(&rc, 0, 0, CH_ITEM_W, CH_ITEM_H);

    //create user piece
    chitem = CreateUserPiece(self,indexpath);
    _c(chitem)->setRect(chitem,&rc);

    //create tableview item
    item = NEWPIECEEX(mTableViewItemPiece,0);
    _c(item)->setRect(item,&rc);
    _c(item)->setUserPiece(item,(mHotPiece*)chitem);

    return item;
}

static void mCallHistoryTableViewPiece_rowDidSelectAtIndexPath( mCallHistoryTableViewPiece* self, const mIndexPath* indexpath) {
    int row;
    CallLogItem cl;
    std::string str_temp;

    row = indexpath->row;
    cl = self->data->at(row);

    str_temp = GET_CONTACTSERVICE()->getNameByDialNum(cl.m_dial);
    if (str_temp.empty())
    {
        CallHistoryActivity::m_intent->putExtra("number",cl.m_dial);
    }
    else
    {
        CallHistoryActivity::m_intent->putExtra("number",
                GET_CONTACTSERVICE()->getNameByDialNum(cl.m_dial));
    }
    ACTIVITYSTACK->switchTo("PhoneCallActivity", CallHistoryActivity::m_intent);
}

static void mCallHistoryTableViewPiece_onCommitDeleteRowAtIndexPath(mCallHistoryTableViewPiece* self, 
        const mIndexPath* indexpath) {
    //add:del data with row in db

    self->activity->callDeleteRow(self->data,indexpath->row);
    _c(self)->deleteRowAtIndexPath(self, indexpath, NCS_TABLEVIEW_ANIMATIONZOOM);
}

static BOOL mCallHistoryTableViewPiece_willSelectRowAtIndexPath(mCallHistoryTableViewPiece *self, const mIndexPath *indexpath){
    return (self->isSelect) ? (TRUE) : (FALSE);
}

static void mCallHistoryTableViewPiece_destroy(mCallHistoryTableViewPiece *self) {
    delete [] self->name;
    delete self->data;
    free(self->t);
    Class(mTableViewPiece).destroy((mTableViewPiece*)self);
}

static int mCallHistoryTableViewPiece_processMessage(mCallHistoryTableViewPiece* self, int message, WPARAM wParam, LPARAM lParam, mObject *owner)
{
    if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_RBUTTONDBLCLK) {
        if (message == MSG_LBUTTONUP && self->activity) {
            float v_x, v_y;
            mHotPiece * hotPiece = self->activity->m_editButton;
            QueryMouseMoveVelocity(&v_x, &v_y);
            printf("%s:v_x=%.2f, v_y=%.2f, set to 10000 forcely\n", __FUNCTION__, v_x, v_y);
            if (hotPiece && v_x > 200.0 && abs(v_y) < 100.0) {
                _c(hotPiece)->processMessage(hotPiece, MSG_LBUTTONDOWN, 0, 0, owner);
                _c(hotPiece)->processMessage(hotPiece, MSG_LBUTTONUP, 0, 0, owner);
                //return 0;
            }
        }
    }

    return Class(mTableViewPiece).processMessage((mTableViewPiece*)self, message, wParam, lParam, owner);
}


BEGIN_MINI_CLASS(mCallHistoryTableViewPiece, mTableViewPiece)
	CLASS_METHOD_MAP(mCallHistoryTableViewPiece, construct)
	CLASS_METHOD_MAP(mCallHistoryTableViewPiece, processMessage)
	CLASS_METHOD_MAP(mCallHistoryTableViewPiece, numberOfSections)
	CLASS_METHOD_MAP(mCallHistoryTableViewPiece, numberOfRowsInSection)
	CLASS_METHOD_MAP(mCallHistoryTableViewPiece, createItemForRow)
	CLASS_METHOD_MAP(mCallHistoryTableViewPiece, rowDidSelectAtIndexPath)
	CLASS_METHOD_MAP(mCallHistoryTableViewPiece, onCommitDeleteRowAtIndexPath)
	CLASS_METHOD_MAP(mCallHistoryTableViewPiece, willSelectRowAtIndexPath)
	CLASS_METHOD_MAP(mCallHistoryTableViewPiece, destroy)
END_MINI_CLASS

static enum timeDisplayType GetTimeType(time_t t) 
{
    time_t temp;
    int cur_h,cur_m,cur_s;
    cur_h = TIMESERVICE->get(TimeService::HOUR);
    cur_m = TIMESERVICE->get(TimeService::MINUTE);
    cur_s = TIMESERVICE->get(TimeService::SECOND);

    temp = TIMESERVICE->get(TimeService::SEC_FROM_EPOCH) - \
           (cur_h * 60 * 60 + cur_m * 60 + cur_s); 

    if (t >= temp )
        return CURRENT_TIME;
    else if (t < temp && t > temp - (6*24*60*60))
        return BACK_WEEK_TIME;

    return FAR_AWAY_TIME;
}

static void FormatTime(char *time,int len,time_t t)
{
    TimeSnapshot timesnap(t);
    enum timeDisplayType type = GetTimeType(t);

    if (type == CURRENT_TIME)
    {
        timesnap.format(time,TIME_MAX_LEN,"%H:%M %p"); 
    }
    else if (type == BACK_WEEK_TIME)
    {
        timesnap.format(time,TIME_MAX_LEN,"%A"); 
    }
    else
    {
        timesnap.format(time,TIME_MAX_LEN,"%Y-%m-%d"); 
    }
}
