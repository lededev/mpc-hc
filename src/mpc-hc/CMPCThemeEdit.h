#pragma once
#include <afxwin.h>
#include "CMPCThemeScrollBarHelper.h"
#include <imm.h>

class CMPCThemeEdit : public CEdit
    , public CMPCThemeScrollable
{
public:
    DECLARE_DYNAMIC(CMPCThemeEdit)
    CMPCThemeEdit();
    virtual ~CMPCThemeEdit();
    void PreSubclassWindow();
    void setBuddy(CWnd* buddyWindow) { this->buddy = buddyWindow; };
    void setFileDialogChild(bool set) { isFileDialogChild = set; };
    void SetFixedWidthFont(CFont& f);
    bool IsScrollable();
protected:
    CWnd* buddy;
    CMPCThemeScrollBarHelper* themedSBHelper;
    CFont font;
    bool isFileDialogChild;
    void SetCompWindowPos(HIMC himc, UINT start);

    DECLARE_MESSAGE_MAP()
    
    afx_msg LRESULT ResizeSupport(WPARAM wParam, LPARAM lParam);
    afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
    afx_msg void OnNcPaint();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};


