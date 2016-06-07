#if !defined(AFX_LINECOLORLISTCTRL_H__92EB1CD4_CD33_4E02_A098_367A8FEFF38E__INCLUDED_)  
#define AFX_LINECOLORLISTCTRL_H__92EB1CD4_CD33_4E02_A098_367A8FEFF38E__INCLUDED_  
  
#if _MSC_VER > 1000  
#pragma once  
#endif // _MSC_VER > 1000  
// LineColorListCtrl.h : header file  
//  
  
#include <afxtempl.h>  
  
  
typedef struct    
{  
    COLORREF colText;  
    COLORREF colTextBk;  
}TEXT_BK;  
  
/////////////////////////////////////////////////////////////////////////////  
// CLineColorListCtrl window  
  
class CLineColorListCtrl : public CListCtrl  
{  
public:  
    void SetItemColor(DWORD iItem, COLORREF TextColor, COLORREF TextBkColor);   //����ĳһ�е�ǰ��ɫ�ͱ���ɫ  
    void SetAllItemColor(DWORD iItem, COLORREF TextColor, COLORREF TextBkColor);//����ȫ���е�ǰ��ɫ�ͱ���ɫ  
    void ClearColor();                                                          //�����ɫӳ���  
  
// Construction  
public:  
    CLineColorListCtrl();  
  
// Attributes  
public:  
    CMap<DWORD, DWORD&, TEXT_BK, TEXT_BK&> MapItemColor;  
      
  
// Operations  
public:  
  
// Overrides  
    // ClassWizard generated virtual function overrides  
    //{{AFX_VIRTUAL(CLineColorListCtrl)  
    //}}AFX_VIRTUAL  
  
// Implementation  
public:  
    virtual ~CLineColorListCtrl();  
  
    // Generated message map functions  
protected:  
    //{{AFX_MSG(CLineColorListCtrl)  
        // NOTE - the ClassWizard will add and remove member functions here.  
    //}}AFX_MSG  
    void CLineColorListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);  
  
    DECLARE_MESSAGE_MAP()  
};  
  
/////////////////////////////////////////////////////////////////////////////  
  
//{{AFX_INSERT_LOCATION}}  
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.  
  
#endif // !defined(AFX_LINECOLORLISTCTRL_H__92EB1CD4_CD33_4E02_A098_367A8FEFF38E__INCLUDED_) 
