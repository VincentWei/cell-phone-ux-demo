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
/** 
 * @file GraphicsView.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef GRAPHICSVIEW_INC
#define GRAPHICSVIEW_INC

#include <vector>
#include <minigui/common.h>
#include <minigui/gdi.h>
using std::vector;

namespace GraphicsView {

    /** 
     * @synopsis  Item
     */
class Item {
    public:
        virtual ~Item();
        virtual void onDraw(HDC dc) = 0;
        void getCenterPoint(POINT &point) const;
        void getRect(RECT &rc) const;
        bool isIn(const RECT &rc) const;
        void moveTo(const POINT &point);
        void moveTo(const RECT &rc);
        void setScale(float scale) { m_scale = scale; }
        void setTransparency(float transparency) { m_transparency = transparency; }
        float scale() const { return m_scale; }
        float transparency() const { return m_transparency; }
    protected:
        Item();
    protected:
        RECT m_rc;
        float m_scale;
        float m_transparency;
};

class View;

/** 
 * @synopsis  Scene
 */
class Scene {
    public:
        Scene();
        ~Scene();
        void addItem(Item *item);
        void removeItem(Item *item);
        Item *itemAt(const POINT &pos) const; 
        Item *getFocusItem() { return m_focusItem;} 
        void setFocusItem(Item *item) { m_focusItem = item;} 
        typedef vector<Item *> ItemList;
        ItemList &items() { return m_items; }
        void debug() {
            ItemList::const_iterator i;
            for (i=m_items.begin(); i!=m_items.end(); ++i) {
                POINT pt;
                (*i)->getCenterPoint(pt);
                printf("(%d,%d)\n", pt.x, pt.y);
            }
        }
    private:
        ItemList m_items;
        Item* m_focusItem;
        /* vector<View *> m_observers; */
        // friend class View;
};

/** 
 * @synopsis  View
 */
class View {
    public:
        virtual ~View();

        void moveTo(const RECT &rc);
        const RECT& getRect() const { return m_rc; } 

        /* Coordinate Mapping */
        void SceneToView(const POINT &src, POINT &dst) const;
        void ViewToScene(const POINT &src, POINT &dst) const;
        void SceneToView(const RECT &src, RECT &dst) const;
        void ViewToScene(const RECT &src, RECT &dst) const;

        /* Painting */
        void onDraw(HDC dc);
        void onDraw(HDC dc, const RECT &rc);

#if 0
        /* Event handling */
        virtual int onLButtonDown(const POINT &pos) = 0;
        virtual int onLButtonUp(const POINT &pos) = 0;
        virtual int onMouseMove(const POINT &pos) = 0;
#endif
        void setBgColor(gal_pixel pixel) { m_bgColor = pixel;}
        gal_pixel getBgColor() { return m_bgColor;}
        void setEraseBackgroundCallback(void (*callback)(HDC, const RECT &, void *), void *param);
    protected:
        View(Scene *, HWND hwnd, bool doubleBuffer=false);
        virtual void onDrawBackground(HDC dc, const RECT &rc);
    protected:
        Scene *m_scene;
        RECT m_rc;
        HWND m_hwnd;
        bool m_doubleBuffer;
        gal_pixel m_bgColor;
        void (*m_eraseBackgroundCallback)(HDC, const RECT &, void *);
        void *m_eraseBackgroundParam;
};

};

#endif /* GRAPHICSVIEW_INC */
