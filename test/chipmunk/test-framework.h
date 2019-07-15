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
#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

void setupChipmunk(cpSpace *space);
int proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
void timer_callback(cpSpace *space);

#endif /* TEST_FRAMEWORK_H */
