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
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "GraphicsView.hh"

using namespace GraphicsView;

/*
 * Item
 */
Item::Item()
    : m_scale(1.0), m_transparency(1.0)
{
    memset(&m_rc, 0, sizeof(m_rc));
}

Item::~Item() {
}

void Item::getCenterPoint(POINT &point) const {
    point.x = (m_rc.left + m_rc.right)/2;
    point.y = (m_rc.top + m_rc.bottom)/2;
}

void Item::getRect(RECT &rc) const {
    memcpy(&rc, &m_rc, sizeof(m_rc));
}

bool Item::isIn(const RECT &rc) const {
#if 0
    POINT pt;
    getCenterPoint(pt);
    return PtInRect(&rc, pt.x, pt.y);
#else
    RECT rc1;
    return IntersectRect(&rc1, &rc, &m_rc) == TRUE;
#endif
}

void Item::moveTo(const POINT &point) {
    int w, h;
    w = RECTW(m_rc);
    h = RECTH(m_rc);
    m_rc.left   = point.x - w/2;
    m_rc.top    = point.y - h/2;
    m_rc.right  = point.x + w/2;
    m_rc.bottom = point.y + h/2;
}

void Item::moveTo(const RECT &rc) {
    // NormalizeRect(&rc); Can't Normalize the Rect because it constant.
    if (rc.top <= rc.bottom && rc.left <= rc.right)
        memcpy(&m_rc, &rc, sizeof(m_rc));
}

/*
 * Scene
 */

Scene::Scene() : m_focusItem(NULL) {
}

Scene::~Scene() {
    while (! m_items.empty()) {
        Item *item = m_items.back();
        m_items.pop_back();
        delete item;
    }
}

void Scene::addItem(Item *item) {
    m_items.push_back(item);
}

void Scene::removeItem(Item *item) {
    ItemList::iterator i;
    for (i=m_items.begin(); i!=m_items.end(); ++i) {
        if (*i == item) {
            m_items.erase(i);
            /* XXX: don't delete item */
            return;
        }
    }
}

Item *Scene::itemAt(const POINT &pos) const {
    ItemList::const_iterator i;
    POINT itemPos;
    for (i=m_items.begin(); i!=m_items.end(); ++i) {
        (*i)->getCenterPoint(itemPos);
        if (itemPos.x == pos.x && itemPos.y == pos.y) {
            return *i;
        }
    }
    return NULL;
}

View::View(Scene *scene, HWND hwnd, bool doubleBuffer)
    : m_scene(scene), m_hwnd(hwnd), m_doubleBuffer(doubleBuffer), m_eraseBackgroundCallback(NULL), m_eraseBackgroundParam(NULL)
{
    printf("%s hwnd=%p\n", __FUNCTION__, m_hwnd);
    m_bgColor = RGBA2Pixel(HDC_SCREEN, 0x00, 0x00, 0x00, 0xff);
}

View::~View() {
}

void View::SceneToView(const POINT &src, POINT &dst) const {
    dst.x = src.x - m_rc.left;
    dst.y = src.y - m_rc.top;
}

void View::ViewToScene(const POINT &src, POINT &dst) const {
    dst.x = src.x + m_rc.left;
    dst.y = src.y + m_rc.top;
}

void View::SceneToView(const RECT &src, RECT &dst) const {
    memcpy(&dst, &src, sizeof(dst));
    OffsetRect(&dst, -m_rc.left, -m_rc.top);
}

void View::ViewToScene(const RECT &src, RECT &dst) const {
    memcpy(&dst, &src, sizeof(dst));
    OffsetRect(&dst, m_rc.left, m_rc.top);
}

void View::onDrawBackground(HDC dc, const RECT &rc) {
    if (! m_eraseBackgroundCallback) {
        SetBrushColor(dc, m_bgColor);
        FillBox(dc, rc.left, rc.top, RECTW(rc), RECTH(rc));
    }else{
        m_eraseBackgroundCallback(dc, rc, m_eraseBackgroundParam);
    }
}

void View::onDraw(HDC dc) {
    RECT rc;

    rc.left = 0;
    rc.top = 0;
    rc.right = RECTW(m_rc);
    rc.bottom = RECTH(m_rc);
    onDraw(dc, rc);
}

/*
static void printPoint(const char *prefix, const POINT &pt) {
    printf("%s: %d,%d ", prefix, pt.x, pt.y);
}

static void printRect(const char *prefix, const RECT &rc) {
    printf("%s: %d,%d,%d,%d\n", prefix, rc.left, rc.top, rc.right, rc.bottom);
}
*/

void View::onDraw(HDC dst_dc, const RECT &rc) {
    RECT scene_rc, item_rc;
    POINT scene_pt, view_pt;
    Scene::ItemList::iterator i;
    Scene::ItemList &items = m_scene->items();
    HDC memdc = HDC_INVALID, dc;
    Item *focus = m_scene->getFocusItem();

    if (m_doubleBuffer) {
        memdc = CreateCompatibleDC(dst_dc);
        dc = memdc;
    }else{
        dc = dst_dc;
    }

    onDrawBackground(dc, rc);

    ViewToScene(rc, scene_rc);

    for (i=items.begin(); i!=items.end(); ++i) {
        if ((*i)!=focus && (*i)->isIn(scene_rc)) {
            (*i)->getRect(item_rc);
            scene_pt.x = item_rc.left;
            scene_pt.y = item_rc.top;
            SceneToView(scene_pt, view_pt);
            SetMapMode(dc, MM_ANISOTROPIC);
            SetViewportOrg(dc, &view_pt);
            (*i)->onDraw(dc);
        }
    }

    if (focus && focus->isIn(scene_rc)) {
        focus->getRect(item_rc);
        scene_pt.x = item_rc.left;
        scene_pt.y = item_rc.top;
        SceneToView(scene_pt, view_pt);
        SetMapMode(dc, MM_ANISOTROPIC);
        SetViewportOrg(dc, &view_pt);
        focus->onDraw(dc);
    }

    if (m_doubleBuffer) {
        BitBlt(memdc, 0, 0, 0, 0, dst_dc, 0, 0, -1);
        DeleteMemDC(memdc);
    }
}

void View::moveTo(const RECT &rc) {
    memcpy(&m_rc, &rc, sizeof(m_rc));
}

void View::setEraseBackgroundCallback(void (*callback)(HDC, const RECT &, void *), void *param) {
    m_eraseBackgroundCallback = callback;
    m_eraseBackgroundParam = param;
}
