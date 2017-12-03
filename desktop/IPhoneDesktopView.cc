#include <stdio.h>
#include "global.h"
#include "IPhoneDesktopView.hh"
#include "IPhoneDesktopController.hh"
#include "ActivityStack.hh"
#include "DesktopToolbar.hh"
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgeff/mgeff.h>

IPhoneDesktopView::IPhoneDesktopView(Scene *scene, HWND hwnd)
    : View(scene, hwnd, true), m_lButtonDown(false)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    moveTo(rc);
}

IPhoneDesktopView::~IPhoneDesktopView() {
}

void IPhoneDesktopView::invalidateRect(const RECT *rc) {
#if 0
    InvalidateRect(m_hwnd, rc, FALSE);
    //UpdateWindow(m_hwnd, TRUE);
#else
    HDC sec_dc;
    HDC dst_dc;
    HWND hmainwnd;

    /* if is main window(not control), do not need to release secondary dc */
    hmainwnd = GetMainWindowHandle(m_hwnd);
    sec_dc = GetSecondaryDC(hmainwnd);
    if (HDC_INVALID != sec_dc && HDC_SCREEN != sec_dc) {
        SetSecondaryDC(hmainwnd, sec_dc, ON_UPDSECDC_DONOTHING);

        UpdateWindow(hmainwnd, TRUE);
        SetSecondaryDC(hmainwnd, sec_dc, ON_UPDSECDC_DEFAULT);

        dst_dc = GetClientDC(hmainwnd); 
        BitBlt(sec_dc, 0, 0, 0, 0, dst_dc, 0, 0, 0);
        ReleaseDC(dst_dc);
    } else {
        assert(0);
    }
#endif
}

void IPhoneDesktopView::callRedraw(MGEFF_ANIMATION animation, void *target, int id, void *pValue) {
    IPhoneDesktopView *view = (IPhoneDesktopView *)target;
    view->invalidateRect(id ? &view->getAnimationArea() : NULL);
}

void IPhoneDesktopView::onDrawBackground(HDC dc, const RECT &rc)
{
    View::onDrawBackground(dc, rc);

    /* draw the pagebar information */
#ifdef HAVE_PAGESCROLL
    IPhoneDesktopController* controller = IPhoneDesktopController::getControllerFromHwnd(m_hwnd);
    IPhoneDesktopLayout* layout = &(controller->getLayout());
    int total_page = layout->totalPage();
    int cur_page = layout->currentPage();
    int distance = layout->getPageBarCircleDistance();
    int offset_x = 0;
    int offset_y = PAGEBAR_Y + (distance - PAGEBAR_DISTANCE) * PAGEBAR_DISTANCE;
    int sum = -distance;
    gal_pixel old_color;

    int i;
    for (i = 0; i < total_page; ++i) {
        sum += PAGEBAR_R * 2 + distance;
    }
    
    offset_x = layout->getWidth() / 2 - sum / 2;

    old_color = SetBrushColor(dc, RGB2Pixel(dc, 0x3f, 0x3f, 0x3f));
    for (i = 0; i < total_page; i++) {
        if (i == cur_page) {
            gal_pixel normal_color = SetBrushColor(dc, RGB2Pixel(dc, 0xff, 0xff, 0xff));
            FillCircle(dc, offset_x, offset_y, PAGEBAR_R);
            SetBrushColor(dc, normal_color);
        } else {
            FillCircle(dc, offset_x, offset_y, PAGEBAR_R);
        }
        offset_x += PAGEBAR_R * 2 + distance;
    }
    SetBrushColor(dc, old_color);
#endif /* HAVE_PAGESCROLL */
}


