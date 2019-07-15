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
#include <assert.h>
#include <stdio.h>
#include "global.h"
#include "IPhoneDesktopLayout.hh"
#include <minigui/gdi.h>
#include <minigui/window.h>

IPhoneDesktopLayout::IPhoneDesktopLayout(Scene *scene, HWND hwnd)
    : m_scene(scene), m_hwnd(hwnd), m_pages(0), m_curpage(0),
    m_column(COLUMNS_PER_PAGE), m_row(ROWS_PER_PAGE)
{
    RECT rc;

    GetClientRect(hwnd, &rc);
    m_width = RECTW(rc);
    m_height = RECTH(rc);
    m_iconSize = m_width/(m_column * 2);
    m_pageBarCircleDistance = PAGEBAR_DISTANCE;
}

IPhoneDesktopLayout::~IPhoneDesktopLayout() {
}

int IPhoneDesktopLayout::nextPage() {
    if (++m_curpage >= m_pages) {
        m_curpage = m_pages-1;
    }
    return m_curpage;
}

int IPhoneDesktopLayout::prevPage() {
    if (--m_curpage < 0) {
        m_curpage = 0;
    }
    return m_curpage;
}

void IPhoneDesktopLayout::index2Rect(RECT &rc, int page, int index) const {
    POINT pt;
    index2Point(pt, page, index);

    rc.left = pt.x - m_iconSize/2;
    rc.right = rc.left + m_iconSize;
    rc.top = pt.y - m_iconSize/2;
    rc.bottom = rc.top + m_iconSize;
}

void IPhoneDesktopLayout::index2Point(POINT &point, int page, int index) const {
    point.x = page * m_width + (index % m_column) * (m_width / m_column);
    point.y = (index / m_column) * (m_height / m_row);

    point.x += m_width / (m_column * 2);
    point.y += m_height / (m_row * 2);
}

Item* IPhoneDesktopLayout::index2Item(int page, int index) const {
    POINT pt;
    index2Point(pt, page, index);
//    printf("-------------------page=%d, index=%d, pt = (%d, %d).\n", page, index, pt.x, pt.y);
    return m_scene->itemAt(pt);
}

int IPhoneDesktopLayout::point2Index(const POINT &point, int &page, int &index) const {
    int index_w, index_h;

    page = m_curpage;

    if (point.y < m_height) {
        index_w = (point.x % m_width) * m_column / m_width;
        index_h = point.y * m_row / m_height;
        index = m_column * index_h + index_w;
//        page = point.x / m_width;
        return 0;
    } else {
        return -1;
    }
}

Item* IPhoneDesktopLayout::point2Item(const POINT &point) const {
    int page, index;
    if (point2Index(point, page, index) == -1) {
        return NULL;
    }
    return index2Item(page, index);
}

int IPhoneDesktopLayout::getVacancy(int page) const {
    int i;
    int total = getItemTotal();
    for (i = 0; i < total; i++) {
        if (index2Item(page, i) == NULL) {
            return i;
        }
    }
    return -1;
}

bool IPhoneDesktopLayout::pageFull(int page) const {
    return getVacancy(page) == -1;
}

void IPhoneDesktopLayout::reLayout() {
    Scene::ItemList::iterator i;
    Scene::ItemList &items = m_scene->items();
    int index = 0;
    int page = 0;
    POINT pt;

    m_pages = 1;
    for (i=items.begin(); i!=items.end(); ++i) {
        index2Point(pt, page, index);
        (*i)->moveTo(pt);
        if (++index >= getItemTotal()) {
            ++page;
            newPage();
            index = 0;
        }
    }
}

int IPhoneDesktopLayout::setColunm(int col)
{
    if (col > 0) {
        m_column = col;
        return 0;
    }
    return -1;
}

int IPhoneDesktopLayout::setRow(int row)
{
    if (row > 0) {
        m_row = row;
        return 0;
    }
    return -1;
}
