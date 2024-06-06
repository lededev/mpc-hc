#include "stdafx.h"
#include "CMPCThemeResizablePropertySheet.h"
#include "CMPCTheme.h"
#include "mplayerc.h"

CMPCThemeResizablePropertySheet::CMPCThemeResizablePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CResizableSheet(nIDCaption, pParentWnd, iSelectPage)
    ,isModal(false)
{
}

CMPCThemeResizablePropertySheet::CMPCThemeResizablePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CResizableSheet(pszCaption, pParentWnd, iSelectPage)
    ,isModal(false)
{
}

CMPCThemeResizablePropertySheet::~CMPCThemeResizablePropertySheet()
{
}

IMPLEMENT_DYNAMIC(CMPCThemeResizablePropertySheet, CResizableSheet)
BEGIN_MESSAGE_MAP(CMPCThemeResizablePropertySheet, CResizableSheet)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CMPCThemeResizablePropertySheet::OnInitDialog()
{
    BOOL bResult = __super::OnInitDialog();
    fulfillThemeReqs();
    return bResult;
}

void CMPCThemeResizablePropertySheet::fulfillThemeReqs()
{
    CMPCThemeUtil::enableWindows10DarkFrame(this);
    if (AppIsThemeLoaded()) {
        SetSizeGripBkMode(TRANSPARENT); //fix for gripper in mpc theme
    }
    CMPCThemeUtil::fulfillThemeReqs((CWnd*)this);
}

HBRUSH CMPCThemeResizablePropertySheet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (AppNeedsThemedControls()) {
        LRESULT lResult;
        if (pWnd->SendChildNotifyLastMsg(&lResult)) {
            return (HBRUSH)lResult;
        }
        pDC->SetTextColor(CMPCTheme::TextFGColor);
        pDC->SetBkColor(CMPCTheme::ControlAreaBGColor);
        return controlAreaBrush;
    } else {
        HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
        return hbr;
    }
}

INT_PTR CMPCThemeResizablePropertySheet::DoModal() {
    isModal = true;
    PreDoModalRTL(&m_psh);
    return __super::DoModal();
}

//override for CResizableSheet::CalcSizeExtra

BOOL CMPCThemeResizablePropertySheet::CalcSizeExtra(HWND /*hWndChild*/, const CSize& sizeChild, CSize& sizeExtra) {
    CTabCtrl* pTab = GetTabControl();
    if (!pTab)
        return FALSE;

    // get margins of tabcontrol
    CRect rectMargins;
    if (!GetAnchorMargins(pTab->m_hWnd, sizeChild, rectMargins))
        return FALSE;

    // get margin caused by tabcontrol
    CRect rectTabMargins(0, 0, 0, 0);

    // get tab position after resizing and calc page rect
    CRect rectPage, rectSheet;
    GetTotalClientRect(&rectSheet);

    if (!GetAnchorPosition(pTab->m_hWnd, rectSheet, rectPage))
        return FALSE; // no page yet

    CRect rectSave;

    if (lastPageRect == rectPage) {
        rectTabMargins = lastMarginRect;
    } else {
        lastPageRect = rectPage;
        pTab->GetWindowRect(rectSave);
        ::MapWindowPoints(NULL, m_hWnd, (LPPOINT)&rectSave, 2);
        pTab->SetRedraw(FALSE);
        pTab->MoveWindow(rectPage, FALSE);
        pTab->AdjustRect(TRUE, &rectTabMargins);
        lastMarginRect = rectTabMargins;
        pTab->MoveWindow(rectSave, FALSE);
        pTab->SetRedraw(TRUE);
    }

    // add non-client size
    ::AdjustWindowRectEx(&rectTabMargins, GetStyle(), !(GetStyle() & WS_CHILD) &&
        ::IsMenu(GetMenu()->GetSafeHmenu()), GetExStyle());

    // compute extra size
    sizeExtra = rectMargins.TopLeft() + rectMargins.BottomRight() +
        rectTabMargins.Size();
    return TRUE;
}

BOOL CMPCThemeResizablePropertySheet::ArrangeLayoutCallback(LAYOUTINFO& layout) const {
    if (layout.nCallbackID != m_nCallbackID)	// we only added 1 callback
        return CResizableLayout::ArrangeLayoutCallback(layout);

    // set layout info for active page
    layout.hWnd = PropSheet_GetCurrentPageHwnd(m_hWnd);
    if (!::IsWindow(layout.hWnd))
        return FALSE;

    // set margins
    if (IsWizard())	// wizard mode
    {
        // use pre-calculated margins
        layout.marginTopLeft = m_sizePageTL;
        layout.marginBottomRight = m_sizePageBR;
    } else	// tab mode
    {
        CTabCtrl* pTab = GetTabControl();
        ASSERT(pTab != NULL);

        // get tab position after resizing and calc page rect
        CRect rectPage, rectSheet;
        GetTotalClientRect(&rectSheet);

        if (!GetAnchorPosition(pTab->m_hWnd, rectSheet, rectPage))
            return FALSE; // no page yet

        // temporarily resize the tab control to calc page size
        if (lastPageRect == rectPage) {
            rectPage = lastAdjustedPageRect;
        } else {
            lastPageRect = rectPage;
            CRect rectSave;
            pTab->GetWindowRect(rectSave);
            ::MapWindowPoints(NULL, m_hWnd, (LPPOINT)&rectSave, 2);
            pTab->SetRedraw(FALSE);
            pTab->MoveWindow(rectPage, FALSE);
            pTab->AdjustRect(FALSE, &rectPage);
            pTab->MoveWindow(rectSave, FALSE);
            pTab->SetRedraw(TRUE);
            lastAdjustedPageRect = rectPage;
        }

        // set margins
        layout.marginTopLeft = rectPage.TopLeft() - rectSheet.TopLeft();
        layout.marginBottomRight = rectPage.BottomRight() - rectSheet.BottomRight();
    }

    // set anchor types
    layout.anchorTopLeft = TOP_LEFT;
    layout.anchorBottomRight = BOTTOM_RIGHT;

    // use this layout info
    return TRUE;
}
