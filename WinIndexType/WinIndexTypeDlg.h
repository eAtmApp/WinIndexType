
// WinIndexTypeDlg.h: 头文件
//

#pragma once
#include <iostream>
#include <windows.h>
#include <CommCtrl.h>
#include <vector>
#include <array>
#include <string>

#define _READ_ERROR "_read_error_34122"

// CWinIndexTypeDlg 对话框
class CWinIndexTypeDlg: public CDialog
{
    // 构造
public:
    CWinIndexTypeDlg(CWnd* pParent = nullptr);	// 标准构造函数
    ~CWinIndexTypeDlg();

    // 对话框数据
    #ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_WININDEXTYPE_DIALOG };
    #endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:

    struct _DATA_TYPE {
        int     id;
        char    ext[512];
        char    desc[512];
        bool    check;
        bool    init_check; //初始选择
    };

    //max 5000
    std::array<_DATA_TYPE, 5000>* m_pArrayItem = nullptr;

    std::array<_DATA_TYPE*, 5000> m_curItem = {nullptr};
    
    //总数
    int m_nItemCount=0;

    //显示的数目
    int m_nShowCount=0;

    _DATA_TYPE& get_item(int index)
    {
        return *m_curItem[index];
    }
    void ToggleCheckBox(int item);

    //过滤显示列表
    void    FilterShowList(bool force=false);

    //查找窗口句柄
    HWND    FindListWnd();
    
    HWND    m_hLastListWnd=nullptr;

    // 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnOdfinditemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnOdcachehintList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);

    DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_list;
    afx_msg void OnReadList();

    afx_msg void OnSelectAll();
    afx_msg void OnChangeFilter();
private:
    CString m_strFilter;
    CString m_strLastFilter;
public:
    afx_msg void OnWriteList();
    afx_msg void OnUnSelectAll();
    int m_nShowType=0;
    int m_nLastShowType=0;
    afx_msg void OnBnClickedRadio();
};