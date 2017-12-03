/** 
 * @file IPhoneDesktopItem.hh
 * @synopsis  
 * @author WanZhen
 * @version 0.1
 * @date 2010-08-02
 * Copyright © 2002-2009 Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef IPHONEDESKTOPITEM_H
#define IPHONEDESKTOPITEM_H

#include "GraphicsView.hh"
#include <string>

using namespace GraphicsView;

class IPhoneDesktopItem : public Item {
public:
    enum {
        ICON_NORMAL = 0,
        ICON_LEFT,
        ICON_RIGHT,
        MAXICONSTATUS
    };
    
    IPhoneDesktopItem(const std::string& name, const std::string& sign, const std::string& iconPath);
    ~IPhoneDesktopItem();

    const char *name() const { return m_name.c_str(); }
    void onDraw(HDC dc);
    void setStatus(int index) { m_status = index; }
    int status ()const { return m_status; }
    void setScale (float scale) {m_scale = scale;}
    void setTransparency (int value);
    void setHilite(bool enable) { 
        m_isHilite = enable;
        if (m_isHilite) {
            SetMemDCAlpha (m_hdc[0], MEMDC_FLAG_SRCALPHA, 0xd6);
        } else {
            SetMemDCAlpha (m_hdc[0], 0, 0);
            SetMemDCAlpha (m_hdc[0], MEMDC_FLAG_SRCPIXELALPHA, 0);
        }
    }

private:
    int getPicturesName (const char *iconPath);
    void getRandColor (Uint8& red, Uint8& green, Uint8& blue, Uint8& alpha);

private:
    std::string m_name;
    std::string m_signature;
    std::string m_icons[MAXICONSTATUS];
    int m_status;
    float m_scale;
    int m_transparency;
    HDC m_hdc[MAXICONSTATUS];
    HDC m_textHdc[MAXICONSTATUS];
    PLOGFONT m_nameFont;
    bool m_isHilite;
};

#endif /* IPHONEDESKTOPITEM_H */
