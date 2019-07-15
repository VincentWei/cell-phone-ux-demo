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
 * @file MusicPlayerToolbar.hh 
 * @Synopsis  
 * @author DengMin
 * @version 1.0
 * @date 10/11/2010 17:17:38 
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef MUSICPLAYERTOOLBAR_HH
#define MUSICPLAYERTOOLBAR_HH

#include "global.h"

#include <cstring>

class MusicPlayerToolbar {
    public:
        MusicPlayerToolbar(HWND parent);
        virtual ~MusicPlayerToolbar();

        // add event listener
        void addBtnEventListener(int btnIdx, int message, NCS_CB_ONPIECEEVENT handler);

        // switch play/pause
        int switchPlayState() {
            mImagePiece* btn = m_toolbarBtnPiece[TB_PLAY];

            if (isPlay()) {
                m_playState = PAUSE;

                // switch to PAUSE, so change the image to PLAY
                _c(btn)->setProperty(btn, NCSP_IMAGEPIECE_IMAGE, 
                        (DWORD)m_toolbarBmp[TB_PLAY]);
            }
            else {
                m_playState = PLAY;

                // switch to PLAY, so change the image to PAUSE
                _c(btn)->setProperty(btn, NCSP_IMAGEPIECE_IMAGE, 
                        (DWORD)m_pauseBmp);
            }
            UpdateWindow(m_toolbarCtrl->hwnd, FALSE);

            return 0;
        }

        BOOL isPlay() {
            return m_playState == PLAY;
        }

        enum TB_INDEX {
            TB_PREV = 0,
            TB_PLAY,
            TB_NEXT,

            TB_MAX
        };

        enum PLAY_STATE {
            PLAY = 0,
            PAUSE,
        };

    private:
        void loadRes();
        void unloadRes();

        void initComponent();

    private:
        // parent window
        HWND m_parent;

        // playOrPause
        int m_playState;

        // component
        mContainerCtrl* m_toolbarCtrl;
        mPanelPiece* m_toolbarContainerShell;
        mPanelPiece* m_toolbarContainer;
        mButtonPanelPiece* m_toolbarSubContainer[TB_MAX];
        mImagePiece* m_toolbarBtnPiece[TB_MAX];

        mHotPiece* m_pausePiece;

        // resource
        // bitmap
        PBITMAP m_toolbarBmp[TB_MAX];
        PBITMAP m_pauseBmp;
};


#endif // MUSICPLAYERTOOLBAR_HH
