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
 * @file Activity.hh 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

// refactored @ 2010-11-22 by min

#ifndef  ACTIVITY_INC
#define  ACTIVITY_INC

#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgeff/mgeff.h>
#include <mgplus/mgplus.h>
#include <mgncs/mgncs.h>

#include "global.h"
#include "utility.h"

#include "ContentValues.hh"

#define ACTIVITY_NO_SECONDARYDC 1

class ContentResolver;

class Activity
{
public:
    // constructor & destructor
    Activity();
    virtual ~Activity();

    // get activity pointer from window handle
    static Activity* getActivityFromHWND(HWND hwnd)
    {
        return (Activity*)GetWindowAdditionalData(hwnd);
    }

    void setHWNDSecondaryDC(HWND hwnd);
    void deleteHWNDSecondaryDC();

    // create this activity, including creating window, setting additional data, etc.
    int create();

    // show the window of this activity
    virtual void show();

    // hide the window of this activity
    virtual void hide();

    // get window handle of this activity
    HWND hwnd() const { return m_hwnd; }

    // get snapshot of this activity
    HDC snapshot() const;

    // get style of this activity
    int style() const { return m_style; }

    // determine whether I need the switch effect provide by the framework
    bool needSwitchEffect() const { return m_needSwitchEffect; }

    enum STYLE {
        STYLE_PUSH = 0x01,
        STYLE_ZOOM,
        STYLE_ALPHA,
    };

    // index for push style
    int m_pushIndex;

protected:

    virtual int onStart() { return 0; }
    virtual int onStop() { return 0; }

    // Give you a chance to do some special things when switching to another activity.
    // You can override this behavior in your own inherited class.
    virtual int onPause() { return 0; }

    // Give you a chance to do some special things when switching back from another activity.
    // You can override this behavior in your own inherited class.
    virtual int onResume() { return 0; }

    virtual int onBack() { return 0; }
    virtual int onHome() {return 0;}

    // Create window for this activity, you must override this method to create your own 
    // window for your activity.
    virtual HWND createHWND() = 0;

protected:

    // store window handle of this activity
    HWND m_hwnd;

    // indicate style of activity, default is STYLE_NONE
    int m_style;

    // indicate if this activity need a switch effect or not, default is TRUE
    bool m_needSwitchEffect;

    friend class ActivityStack;
};

class Intent
{
    public:
        static Intent* newIntent(int action = ACTION_MAIN, std::string uri="")
        {
            return new Intent(action, uri);
        }
        static void deleteIntent(Intent* intentPtr)
        {
            assert(intentPtr);
            delete intentPtr;
        }

    public:
        int action() const
        {
            return m_action;
        }
        const std::string& uri() const
        {
            return m_uri;
        }

        void putExtra(const std::string& key, const boost::any& value)
        {
            m_extras[key] = value;
        }

        const boost::any& getExtra(const std::string& key) const
        {
            ExtraMap::const_iterator found = m_extras.find(key);
            static boost::any empty;

            if (found == m_extras.end()) {
                return empty;
            }
            else {
                return found->second;
            }
        }

        int getInt(const std::string& key) const
        {
            ExtraMap::const_iterator found = m_extras.find(key);

            if (found == m_extras.end()) {
                return 0;
            }
            else {
                return boost::any_cast<int>(found->second);
            }
        }

        const char* getCString(const std::string& key) const
        {
            ExtraMap::const_iterator found = m_extras.find(key);

            if (found == m_extras.end()) {
                return "";
            }
            else {
                return boost::any_cast<const char*>(found->second);
            }
        }

        const std::string& getString(const std::string& key) const
        {
            ExtraMap::const_iterator found = m_extras.find(key);
            static std::string empty("");

            if (found == m_extras.end()) {
                return empty;
            }
            else {
                return boost::any_cast<const std::string&>(found->second);
            }
        }

    protected:
        Intent(int action = ACTION_MAIN, std::string uri = "")
            : m_action(action), m_uri(uri) {}
        virtual ~Intent() {}

    protected:

        int m_action;
        std::string m_uri;
        ExtraMap m_extras;
};


#include <vector>
class ActivityFactory {
public:
    static ActivityFactory *singleton() {
       if (! s_single) {
           s_single = new ActivityFactory();
       }
       return s_single;
    }
    int registerActivity(const char *name, Activity *(*create)(void)) {
        ActivityInfo *info = (ActivityInfo *)malloc(sizeof(*info));
        std::vector<ActivityInfo *>::iterator i;

        info->name = strdup(name);
        info->create = create;

        for (i=m_activities.begin(); i!=m_activities.end(); ++i) {
            if (strcmp(name, (*i)->name) <= 0) {
                break;
            }
        }
        m_activities.insert(i, info);
        return 0;
    }
    Activity *create(const char* name) {
        for (std::vector<ActivityInfo *>::const_iterator i=m_activities.begin(); i!=m_activities.end(); ++i) {
            if (strcmp(name, (*i)->name) == 0) {
                Activity *ins = (*i)->create();
                if (ins) {
                    if (ins->create() == 0) {
                        return ins;
                    }else{
                        delete ins;
                    }
                }
                return NULL;
            }
        }
        return NULL;
    }

    void list(void) {
        printf("All registered activities (total %lu):\n", (unsigned long)m_activities.size());
        for (std::vector<ActivityInfo *>::const_iterator i=m_activities.begin(); i!=m_activities.end(); ++i) {
            printf("  %s (%p)\n", (*i)->name, (*i)->create);
        }
    }

    void queryNames(std::vector<std::string> &names) {
        for (std::vector<ActivityInfo *>::const_iterator i=m_activities.begin(); i!=m_activities.end(); ++i) {
            names.push_back((*i)->name);
        }
    }
private:
    ActivityFactory() { }
private:
    typedef struct {
        char *name;
        Activity *(*create)(void);
    } ActivityInfo;
    std::vector<ActivityInfo *> m_activities;

    static ActivityFactory *s_single;
};

#define REGISTER_ACTIVITY(_class) \
static Activity * my_create() \
{ \
    return new _class(); \
} \
void realRegister##_class (void) \
{ \
    ActivityFactory::singleton()->registerActivity(#_class, my_create); \
}

#define DO_REGISTER_ACTIVITY(_class) \
    extern void realRegister##_class (void); \
    realRegister##_class ()

#define AUTO_REGISTER_ACTIVITY(_class) \
    static struct _ActivityFactory_##_class { \
        static Activity *create() { \
            return new _class(); \
        } \
        _ActivityFactory_##_class() { \
            ActivityFactory::singleton()->registerActivity(#_class, create); \
        } \
    } _autoRegister_##_class

#endif   /* ----- #ifndef ACTIVITY_INC  ----- */
