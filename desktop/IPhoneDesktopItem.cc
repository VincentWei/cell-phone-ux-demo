#include <assert.h>
#include <string.h>
#include "IPhoneDesktopItem.hh"
#include <minigui/gdi.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgeff/mgeff.h>
#include <mgncs/mgncs.h>
#include <mgncs4touch/mgncs4touch.h>

#include "ResourceManager.hh"

#define SUBSCRIPT_RISE_LEN 20
#define SUBSCRIPT_TEXT_H 15
#define SUBSCRIPT_BMP_TEXT_SPACE (SUBSCRIPT_RISE_LEN - SUBSCRIPT_TEXT_H)
#define ICONSCOLORKEY(dc) RGBA2Pixel(dc, 181, 170, 181, 0x0)

int IPhoneDesktopItem::getPicturesName (const char *iconPath)
{
    int i;
    const char *SUFFIX[MAXICONSTATUS-1] = {
        "_left",
        "_right",
    };

    if (!iconPath) {
        printf ("iconPath error!\n");
        return -1;
    }

    m_icons[0] = iconPath;
    size_t found = m_icons[0].find_last_of('.');
    
    for (i=1; i<MAXICONSTATUS; ++i) {
        m_icons[i] = m_icons[0].substr(0, found) + SUFFIX[i-1] + m_icons[0].substr(found);
    }

    return 0;
}
#define ITEM_TEXT_FONT_H 12
//#define ITEM_BACK_COLOR 0xc01b2d01//0x7f2b3d01
#define ITEM_BACK_COLOR 0x88000000
static void IPhoneDesktopCreateLabel(HDC hdc,const char *name,PLOGFONT font,RECT rect)
{
    mHotPiece* backpiece;
    HDC dc;
    SIZE size;
    int w,x;
    RECT backrc = {0,0,RECTW(rect), RECTH(rect)};

    assert(font);
    dc = GetSubDC(hdc,rect.left,rect.top, RECTW(rect), RECTH(rect));
    backpiece = (mHotPiece*)NEWPIECE(mShapeTransRoundPiece);
    _c(backpiece)->setRect (backpiece, &backrc);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_BKCOLOR, ITEM_BACK_COLOR);
    _c(backpiece)->setProperty (backpiece, NCSP_TRANROUND_RADIUS,4);
    _c(backpiece)->setProperty (backpiece, 
            NCSP_TRANROUND_PAINTMODE,TRANROUND_PAINTMODE_GRAPHICSAVE);
    _c(backpiece)->paint(backpiece,dc,NULL,0);

    SelectFont(dc,font);
    SetBkMode(dc,BM_TRANSPARENT);
    SetTextColor(dc, RGB2Pixel(dc, 0xff, 0xff, 0xff));

    GetTextExtent (dc, name, strlen(name), &size);
    w = GetGDCapability (dc, GDCAP_MAXX) + 1;
    x = (w - size.cx)>>1;
    if (x < 0||x > w)x=0;
    TextOut(dc,x,0,name);

    ReleaseDC(dc);
    DELPIECE(backpiece);
}

IPhoneDesktopItem::IPhoneDesktopItem(const std::string& name, const std::string& sign, const std::string& iconPath)
    : m_name(name), m_signature(sign), m_status(0), m_scale(1.0f), m_transparency(0xff), m_isHilite(false)
{
    int i;
    RECT rc = {0};
    BITMAP *bmp;
    int w = 0, h = 0;

    m_nameFont = CreateLogFontEx ("ttf", "helvetica", "GB2312",
            FONT_WEIGHT_BOOK,
            FONT_SLANT_ROMAN,
            FONT_FLIP_NONE,
            FONT_OTHER_NONE,
            FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
            ITEM_TEXT_FONT_H, 0);

    getPicturesName (iconPath.c_str());
    for (i=0; i<MAXICONSTATUS; ++i) {
        if (!m_icons[i].empty()) {
            bmp = (BITMAP*) Load32Resource (m_icons[i].c_str(), RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
            if (!bmp) {
                printf ("Load32Resource error, name: %s!\n", m_icons[i].c_str());
                continue;
            } else {
                RECT rc;
                w = bmp->bmWidth + SUBSCRIPT_RISE_LEN;
                h = bmp->bmHeight/* + SUBSCRIPT_RISE_LEN*/;
                m_hdc[i] = CreateMyCompatibleDC(Get32MemDC(), w, h);

                SetBrushColor(m_hdc[i], ICONSCOLORKEY(m_hdc[i]));
                if (GetGDCapability(m_hdc[i], GDCAP_AMASK) == 0)
                    SetMemDCColorKey (m_hdc[i], MEMDC_FLAG_SRCCOLORKEY, ICONSCOLORKEY(m_hdc[i]));
                FillBox(m_hdc[i], 0, 0, w, h);
                FillBitmapPartInBox(m_hdc[i], SUBSCRIPT_RISE_LEN / 2, 0, 0, 0, bmp, 0, 0, 0, 0);

                m_textHdc[i] = CreateCompatibleDCEx(Get32SwMemDC(), w, SUBSCRIPT_TEXT_H); 
                SetBrushColor(m_textHdc[i], ICONSCOLORKEY(m_textHdc[i]));
                if (GetGDCapability(m_textHdc[i], GDCAP_AMASK) == 0)
                    SetMemDCColorKey (m_textHdc[i], MEMDC_FLAG_SRCCOLORKEY, ICONSCOLORKEY(m_textHdc[i]));
                FillBox(m_textHdc[i], 0, 0, w, SUBSCRIPT_TEXT_H);
                SetRect(&rc,0,0,w,SUBSCRIPT_TEXT_H);
                IPhoneDesktopCreateLabel(m_textHdc[i], m_signature.c_str(), m_nameFont, rc);
            }
        } else {
            printf ("Load32Resource error. no path info\n");
        }
    }

    rc.right  = GetGDCapability(m_hdc[0], GDCAP_MAXX) + 1;
    rc.bottom = GetGDCapability(m_hdc[0], GDCAP_MAXY) + 1;
    rc.bottom += SUBSCRIPT_RISE_LEN;
    //printf("Bitmap: w=%d,h=%d.\n", RECTW(rc), RECTH(rc));
    moveTo(rc);
}

IPhoneDesktopItem::~IPhoneDesktopItem() 
{
    int i;
    BITMAP *bmp;
    for (i=0; i<MAXICONSTATUS; ++i) {
        if (m_hdc[i] != HDC_INVALID) {
            DeleteMemDC (m_hdc[i]);
        }
        if (m_textHdc[i] != HDC_INVALID) {
            DeleteMemDC (m_textHdc[i]);
        }

        bmp = (BITMAP*)GetResource (Str2Key (m_icons[i].c_str()));
        if (bmp) {
            ReleaseRes (Str2Key (m_icons[i].c_str()));
        }
    }
    DestroyLogFont(m_nameFont);
}

void IPhoneDesktopItem::setTransparency (int value)
{
    if (value<=0xff && value>=0) {
        m_transparency = value;
    } else {
        assert (0);
    }
}

void IPhoneDesktopItem::onDraw(HDC dc) 
{
    int w,h;
    assert(m_status >=0 && m_status < MAXICONSTATUS);

    if (m_scale != 1.0f || m_transparency != 0xff) {
        HDC tmpdc;
        POINT pt;
        int temp_h;
        w = m_scale * GetGDCapability(m_hdc[0], GDCAP_HPIXEL);
        h = m_scale * GetGDCapability(m_hdc[0], GDCAP_VPIXEL);
        tmpdc = CreateCompatibleDCEx (dc, w, h);
        temp_h = h;

        StretchBlt(m_hdc[0], 0, 0, -1, -1, tmpdc, 0, 0, w, h, -1);
        SetMemDCColorKey (tmpdc, MEMDC_FLAG_SRCCOLORKEY, ICONSCOLORKEY(tmpdc));
        SetMemDCAlpha (tmpdc, MEMDC_FLAG_SRCALPHA, m_transparency);
        GetViewportOrg(dc, &pt);
        pt.x -= (w - GetGDCapability(m_hdc[0], GDCAP_HPIXEL)) / 2;
        pt.y -= (h - GetGDCapability(m_hdc[0], GDCAP_VPIXEL)) / 2;
        BitBlt (tmpdc, 0, 0, 0, 0, dc, pt.x, pt.y, -1);
        DeleteMemDC (tmpdc);

        w = m_scale * GetGDCapability(m_textHdc[0], GDCAP_HPIXEL);
        h = m_scale * GetGDCapability(m_textHdc[0], GDCAP_VPIXEL);
        tmpdc = CreateCompatibleDCEx (m_textHdc[0], w, h);

        StretchBlt(m_textHdc[0], 0, 0, -1, -1, tmpdc, 0, 0, w, h, -1);
        SetMemDCColorKey (tmpdc, MEMDC_FLAG_SRCCOLORKEY, ICONSCOLORKEY(tmpdc));
        SetMemDCAlpha (tmpdc, MEMDC_FLAG_SRCALPHA, m_transparency);
        GetViewportOrg(dc, &pt);
        pt.x -= (w - GetGDCapability(m_textHdc[0], GDCAP_HPIXEL)) / 2;
        pt.y -= (h - GetGDCapability(m_textHdc[0], GDCAP_VPIXEL)) / 2;
        BitBlt (tmpdc, 0, 0, 0, 0, dc, pt.x, pt.y+temp_h+SUBSCRIPT_BMP_TEXT_SPACE, -1);
        DeleteMemDC (tmpdc);
    } else if (m_status >= 0 && m_status < MAXICONSTATUS) {
        if (m_hdc[m_status] != HDC_INVALID && m_textHdc[m_status] != HDC_INVALID) {
            POINT pt;
            GetViewportOrg(dc, &pt);
            h = GetGDCapability(m_hdc[0], GDCAP_VPIXEL);
            SetMemDCColorKey (m_hdc[m_status], 
                    MEMDC_FLAG_SRCCOLORKEY, ICONSCOLORKEY(m_hdc[m_status]));
            BitBlt (m_hdc[m_status], 0, 0, 0, 0, dc, pt.x, pt.y, -1);
            BitBlt (m_textHdc[m_status], 0, 0, 0, 0, dc, pt.x, pt.y+h+SUBSCRIPT_BMP_TEXT_SPACE, -1);
        } else {
            Uint8 r,g,b,a;
            getRandColor (r, g, b, a);
            SetBrushColor(dc, RGBA2Pixel (dc, r, g, b, a));
            FillBox(dc, 0, 0, RECTW(m_rc), RECTH(m_rc));
        }
    } else {
        assert (0);
    }
}

void IPhoneDesktopItem::getRandColor (Uint8 &red, Uint8 &green, Uint8 &blue, Uint8 &alpha)
{
    unsigned long radix = (unsigned long)this;
    radix = radix * 1103515245 + 12345;

    red = ((unsigned)(radix/65536) % 32768)%255;
    green = 0;
    blue = ((unsigned)(radix/65536) % 32768)%128;
    alpha = 0xff;
}
