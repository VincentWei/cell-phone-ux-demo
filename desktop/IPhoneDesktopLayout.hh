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
 * @file IPhoneDesktopLayout.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPLAYOUT_H
#define IPHONEDESKTOPLAYOUT_H

#include "global.h"
#include "GraphicsView.hh"
using namespace GraphicsView;

class IPhoneDesktopLayout {
    public:
        IPhoneDesktopLayout(Scene *, HWND hwnd);
        ~IPhoneDesktopLayout();

        int currentPage() const { return m_curpage; }
        int totalPage() const { return m_pages; }
        int nextPage();
        int prevPage();
        void newPage() { if (++m_pages > MAX_PAGE_NUMBER) --m_pages; }
        void delPage() { --m_pages; }
        void index2Rect(RECT &rc, int page, int index) const;
        void index2Point(POINT &point, int page, int index) const;
        Item* index2Item(int page, int index) const;
        int point2Index(const POINT &point, int &page, int &index) const;
        Item* point2Item(const POINT &point) const;

        int getVacancy(int page) const;
        bool pageFull(int page) const;
        HWND getHwnd () { return m_hwnd; }

        void reLayout();
        int  setColunm(int col);
        int  setRow(int row);
        int  getColunm() const {return m_column;}
        int  getRow() const {return m_row;}
        int  getItemTotal() const {return (m_column * m_row);}
        int  getPageBarCircleDistance() const { return m_pageBarCircleDistance; }
        int  getWidth() const {return m_width; }
        int  getHeight() const {return m_height; }
        void setPageBarCircleDistance(int distance) { m_pageBarCircleDistance = distance; }
        void setWidth(size_t width) { m_width = width; }
        void setHeight(size_t height) { m_height = height; }
    private:
        Scene *m_scene;
        HWND m_hwnd;
        int m_pages;
        int m_curpage;
        int m_pageBarCircleDistance; /* the distance between page circles in the pageBar */
        int m_width;
        int m_height;
        int m_iconSize;
        int m_column;
        int m_row;
};

#endif /* IPHONEDESKTOPLAYOUT_H */
