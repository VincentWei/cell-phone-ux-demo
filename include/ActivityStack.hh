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
/*!============================================================================
 * @file ActivityStack.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

// refactored @ 2010-11-22 by min

#ifndef  _ACTIVITYSTACK_H
#define  _ACTIVITYSTACK_H

#include <string>
#include <vector>
#include "Activity.hh"

/** 
 * @synopsis  A stack for activities manager.
 */
class ActivityStack
{
public:
    typedef std::pair<std::string, Activity*> ActivityEntry;
    typedef std::vector<ActivityEntry> ActivityCollection;
        
    // get the single instance of ActivityStack
    static ActivityStack* singleton();

    // get the top activity in stack
    Activity* top() const;

    // return activity name of current app
    const char* currentAppName() const;

    // Create a new activity and bring it to the top of the stack, 
    // using appName to specify which Activity will be created,
    // using intentPtr to specify your intend (optionally).
    bool push(const char* appName, Intent* intentPtr = NULL);

    // Remove the top activity from the stack, and send MSG_CLOSE message to 
    // the window of the activity, instead of destroying it directly.
    bool pop();

    // Back to previous view
    bool back();

    // Return to home
    bool home();

    // get depth of the stack
    int depth() const { return m_activities.size(); }

    // This method will find the first Activity named 'appName' in the stack from top to bottom,
    // if the Activity is existed, then pop to that Activity by calling popTo method;
    // if the Activity is not existed, then create a new Activity named appName, and push it into the stack.
    void switchTo(const char* appName, Intent* intentPtr = NULL);
    
private:

    // constructor & desctructor, for internal use only
    ActivityStack();
    virtual ~ActivityStack();

    Activity* innerPush(const char* appName, Intent* intentPtr);
    bool innerPop();
    
    // Do the same thing as pop(), but supply a parameter "which" to specify 
    // which activity you want to show up, and all activities above it will be poped.
    // Notice that, if you call this function with parameter NULL, the stack will be cleared.
    // return how much activities has been poped up
    int popTo(Activity* which, Intent* intentPtr = NULL);
    
    // get the activity under the top
    Activity* _underTop() const;

    void _doSwitchEffect(Activity* prev, Activity* next, BOOL switchTo=TRUE);

    // search the first Activity named 'appName' in the stack in the order from top to bottom
    Activity* searchActivityByName(std::string appName);
    

    // store all activities
    ActivityCollection m_activities;

    int m_status;
    enum STATUS {
        READY = 0,
        PUSH,
        POP,
    };

    // store the single instance of ActivityStack
    static ActivityStack* s_single;
};

#define ACTIVITYSTACK   ActivityStack::singleton()

#endif   /* ----- #ifndef ACTIVITYSTACK_INC  ----- */

