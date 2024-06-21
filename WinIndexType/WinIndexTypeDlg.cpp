
// WinIndexTypeDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "WinIndexType.h"
#include "WinIndexTypeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinIndexTypeDlg::CWinIndexTypeDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_WININDEXTYPE_DIALOG, pParent)
    , m_pArrayItem(new std::array<_DATA_TYPE, 5000>)
    , m_strFilter(_T(""))
    , m_nShowType(0)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CWinIndexTypeDlg::~CWinIndexTypeDlg()
{
    delete[]m_pArrayItem;
}

void CWinIndexTypeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list);
    DDX_Text(pDX, IDC_EDIT1, m_strFilter);
    DDX_Radio(pDX, IDC_RADIO1, m_nShowType);
}

BEGIN_MESSAGE_MAP(CWinIndexTypeDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()

    ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, OnGetdispinfoList)
    //ON_NOTIFY(LVN_ODFINDITEM, IDC_LIST1, OnOdfinditemList)
    //ON_NOTIFY(LVN_ODCACHEHINT, IDC_LIST1, OnOdcachehintList)
    ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, OnKeydownList)
    ON_BN_CLICKED(IDC_BUTTON1, &CWinIndexTypeDlg::OnReadList)
    ON_BN_CLICKED(IDC_BUTTON2, &CWinIndexTypeDlg::OnSelectAll)
    ON_EN_CHANGE(IDC_EDIT1, &CWinIndexTypeDlg::OnChangeFilter)
    ON_BN_CLICKED(IDC_BUTTON4, &CWinIndexTypeDlg::OnWriteList)
    ON_BN_CLICKED(IDC_BUTTON3, &CWinIndexTypeDlg::OnUnSelectAll)
    ON_BN_CLICKED(IDC_RADIO1, &CWinIndexTypeDlg::OnBnClickedRadio)
    ON_BN_CLICKED(IDC_RADIO2, &CWinIndexTypeDlg::OnBnClickedRadio)
    ON_BN_CLICKED(IDC_RADIO3, &CWinIndexTypeDlg::OnBnClickedRadio)
END_MESSAGE_MAP()

BOOL CWinIndexTypeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    UpdateData(FALSE);

    //exe sln rar 7z zip iso img vmx gz

    std::vector<CString> vecFilter;

    //ListView_SetExtendedListViewStyle(m_list.m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

    auto exStyle = m_list.GetExStyle();
    m_list.SetExtendedStyle(exStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

    //m_list.InsertColumn(0, "id", LVCFMT_LEFT, 60);
    m_list.InsertColumn(1, "扩展名", LVCFMT_LEFT, 150);
    m_list.InsertColumn(2, "筛选器描述", LVCFMT_LEFT, 250);

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);
    return TRUE;
}

void CWinIndexTypeDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}
HCURSOR CWinIndexTypeDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

//This function is called when the list needs data. This is the most
//critical function when working with virtual lists.
void CWinIndexTypeDlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

    //Create a pointer to the item
    LV_ITEM* pItem = &(pDispInfo)->item;

    //Which item number?
    int itemid = pItem->iItem;

    auto item = get_item(itemid);

    //Do the list need text information?
    if (pItem->mask & LVIF_TEXT)
    {
        if (pItem->iSubItem == 0)
        {
            strcpy_s(pItem->pszText, pItem->cchTextMax, item.ext);
        }
        else if (pItem->iSubItem == 1)
        {
            strcpy_s(pItem->pszText, pItem->cchTextMax, item.desc);
        }
    }

    if (pItem->mask & LVIF_IMAGE)
    {
        pItem->mask |= LVIF_STATE;
        pItem->stateMask = LVIS_STATEIMAGEMASK;

        if (item.check)
        {
            pItem->state = INDEXTOSTATEIMAGEMASK(2);
        }
        else
        {
            pItem->state = INDEXTOSTATEIMAGEMASK(1);
        }
    }

    *pResult = 0;
}

//This functions is called when the user "writes" in the list box to find an item.
void CWinIndexTypeDlg::OnOdfinditemList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // pNMHDR has information about the item we should find
    // In pResult we should save which item that should be selected
    NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;

    /* pFindInfo->iStart is from which item we should search.
       We search to bottom, and then restart at top and will stop
       at pFindInfo->iStart, unless we find an item that match
     */

     // Set the default return value to -1
     // That means we didn't find any match.
    *pResult = -1;

    //Is search NOT based on string?
    if ((pFindInfo->lvfi.flags & LVFI_STRING) == 0)
    {
        //This will probably never happend...
        return;
    }

    /*
        Let's look on a sample list;

          Name
          Anders
        * Anna
          Annika
          Bob
          Emma
          Emmanuel

        Anna is selected.
        If "A" is written, Annika should be selected.
        If "AND" is written, Anders should be selected.
        If "ANNK" is written, the selection should stay on Anna.
        If "E" is written, Emma should be selected.

    */

    //This is the string we search for
    CString searchstr = pFindInfo->lvfi.psz;

    //	TRACE(_T("Find: %s\n"), searchstr);

    int startPos = pFindInfo->iStart;
    //Is startPos outside the list (happens if last item is selected)
    if (startPos >= m_list.GetItemCount())
        startPos = 0;

    int currentPos = startPos;

    //Let's search...
/*
    do
    {
        //Do this word begins with all characters in searchstr?
        if (_tcsnicmp(m_database[currentPos].m_name, searchstr, searchstr.GetLength()) == 0)
        {
            //Select this item and stop search.
            *pResult = currentPos;
            break;
        }

        //Go to next item
        currentPos++;

        //Need to restart at top?
        if (currentPos >= m_list.GetItemCount())
            currentPos = 0;

        //Stop if back to start
    } while (currentPos != startPos);*/
}

//This is called to give you a chance to catch data. Useless in most cases :-)
void CWinIndexTypeDlg::OnOdcachehintList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCACHEHINT* pCacheHint = (NMLVCACHEHINT*)pNMHDR;

    /*	TRACE(	_T("Chache item %d to item %d\n"),
                pCacheHint->iFrom,
                pCacheHint->iTo );*/

                //... cache the data pCacheHint->iFrom to pCacheHint->iTo ...

    *pResult = 0;
}

/*
void CWinIndexTypeDlg::ToggleCheckBox(int item)
{
    //Change check box
    m_database[item].m_checked = !m_database[item].m_checked;

    //And redraw
    m_list.RedrawItems(item, item);
}*/

void CWinIndexTypeDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    LVHITTESTINFO hitinfo = {0};
    hitinfo.pt = pNMListView->ptAction;

    int item = m_list.HitTest(&hitinfo);
    if (item != -1)
    {
        if ((hitinfo.flags == LVHT_ONITEMSTATEICON) != 0)
        {
            auto iSubItem = hitinfo.iSubItem;
            ToggleCheckBox(item);
        }
    }
    *pResult = 0;
}

void CWinIndexTypeDlg::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;

    //If user press space, toggle flag on selected item
    if (pLVKeyDown->wVKey == VK_SPACE && m_list.GetSelectionMark() != -1)
    {
        ToggleCheckBox(m_list.GetSelectionMark());
    }

    *pResult = 0;
}

#define verify_break(exp,msg) if(!(exp)){err_str=msg;break;}
#define verify_ret(exp,msg) if(!(exp)){if(!err_str.empty()){err_str=msg;}return false;}

HWND CWinIndexTypeDlg::FindListWnd()
{
    HWND hListView = ::FindWindowA("#32770", "高级选项");
    if (hListView) hListView = ::FindWindowExA(hListView, 0, "#32770", "文件类型");
    if (hListView) hListView = ::FindWindowExA(hListView, 0, "SysListView32", NULL);
    return hListView;
}

void CWinIndexTypeDlg::OnReadList()
{
    __int64 rows, cols;
    DWORD ProcessID = 0;
    HANDLE hProcess = nullptr;
    LVITEM* lpItem = nullptr;
    LPSTR pTextBuffer = nullptr;

    std::string err_str;
    bool result = false;

    m_nItemCount = 0;
    m_nShowCount = 0;

    m_list.SetItemCount(0);

    auto& itemdata = *m_pArrayItem;

    do
    {
        int nItemCount = 0;

        HWND hListView = FindListWnd();
        verify_break(hListView, "没有找到窗口句柄");

        m_hLastListWnd = hListView;

        HWND hHeaderWnd = (HWND)::SendMessageA(hListView, LVM_GETHEADER, 0, 0);
        verify_break(hHeaderWnd, "LVM_GETHEADER失败");

        rows = ::SendMessageA(hListView, LVM_GETITEMCOUNT, 0, 0);       //总行数
        cols = ::SendMessageA(hHeaderWnd, HDM_GETITEMCOUNT, 0, 0);      //总列数
        verify_break(rows && cols, "LVM_GETITEMCOUNT/HDM_GETITEMCOUNT失败");

        GetWindowThreadProcessId(hListView, &ProcessID);
        verify_break(ProcessID, "获取窗口对应进程ID失败");

        hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, ProcessID);
        verify_break(hProcess != INVALID_HANDLE_VALUE, "打开进程句柄失败");

        //LVITEM
        lpItem = (LVITEM*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
        verify_break(lpItem, "申请远进程内存失败1");

        //Text Buffer
        pTextBuffer = (LPSTR)VirtualAllocEx(hProcess, NULL, 512, MEM_COMMIT, PAGE_READWRITE);
        verify_break(pTextBuffer, "申请远进程内存失败2");

        LVITEM vItem = {0};
        char ItemBuf[512] = {0};

        for (int i = 0; i < rows; i++)
        {
            itemdata[i].check = (bool)ListView_GetCheckState(hListView, i);
            itemdata[i].init_check = itemdata[i].check;

            auto read_item_text = [&, this](int col)
            {
                vItem.mask = LVIF_TEXT;    //说明pszText是有效的
                vItem.iItem = i;            //行号
                vItem.iSubItem = col;        //列号
                vItem.cchTextMax = 512;    //所能存储的最大的文本为256字节
                vItem.pszText = pTextBuffer;

                verify_ret(WriteProcessMemory(hProcess, lpItem, &vItem, sizeof(LVITEM), NULL), "写进程内存失败");
                verify_ret(::SendMessageA(hListView, LVM_GETITEMA, (WPARAM)i, (LPARAM)lpItem), "LVM_GETITEMA失败");
                verify_ret(ReadProcessMemory(hProcess, pTextBuffer, ItemBuf, 512, NULL), "读进程内存失败");

                return true;
            };

            if (!read_item_text(0)) break;
            strcpy_s(itemdata[i].ext, 512, ItemBuf);

            if (!read_item_text(1)) break;
            strcpy_s(itemdata[i].desc, 512, ItemBuf);

            nItemCount++;
        }

        m_nItemCount = nItemCount;

        result = true;
    } while (FALSE);

    if (pTextBuffer) VirtualFreeEx(hProcess, pTextBuffer, 0, MEM_RELEASE);
    if (lpItem) VirtualFreeEx(hProcess, lpItem, 0, MEM_RELEASE);
    if (hProcess != INVALID_HANDLE_VALUE) CloseHandle(hProcess);//关闭打开的进程对象

    if (!result || !err_str.empty())
    {
        AfxMessageBox(err_str.c_str(), MB_OK | MB_ICONERROR);
        return;
    }

    if (m_nItemCount)
    {
        FilterShowList(true);
    }
}

void CWinIndexTypeDlg::ToggleCheckBox(int item)
{
    m_curItem[item]->check = !m_curItem[item]->check;
    m_list.RedrawItems(item, item);
    m_nShowType = -1;
    UpdateData(FALSE);
}

void CWinIndexTypeDlg::FilterShowList(bool force/*=false*/)
{
    CString filterStr(m_strFilter);
    {
        filterStr.Replace("\r", "");
        filterStr.Replace("\n", "");
        filterStr.Replace("\t", "");
        filterStr.Replace(",", ";");
        filterStr.Replace("，", ";");
        filterStr.Replace("|", ";");
        filterStr.Replace("；", ";");
        filterStr.Replace("　", " ");
        filterStr.Replace("  ", " ");
        filterStr.Replace("  ", " ");
        filterStr.Replace("  ", " ");
        filterStr.Replace("  ", " ");
        filterStr.Replace(" ", ";");
        filterStr.Trim();
    }

    if (!force && filterStr == m_strLastFilter)
    {
        return;
    }

    std::vector<CString> vecFilter;
    if (!filterStr.IsEmpty())
    {
        int pos = 0;
        while (true) {
            CString token = filterStr.Tokenize(";", pos);
            token.Replace(" ", "");
            token.Replace("　", "");
            token.Trim();
            if (token.IsEmpty()) { break; }
            vecFilter.push_back(token);
        }
    }

    m_strLastFilter = filterStr;
    m_nLastShowType = m_nShowType;

    m_nShowCount = 0;
    auto& itemdata = *m_pArrayItem;
    for (int i = 0; i < m_nItemCount; i++)
    {
        if ((m_nShowType == 1 && !itemdata[i].check) || (m_nShowType == 2 && itemdata[i].check))
        {
            continue;
        }

        bool isok = false;
        if (vecFilter.empty())
        {
            isok = true;
        }
        else {
            auto extname = itemdata[i].ext;
            for (auto item : vecFilter)
            {
                if (item.CompareNoCase(extname) == 0)
                {
                    isok = true;
                }
            }
        }
        if (!isok) continue;

        m_curItem[m_nShowCount++] = &itemdata[i];
    }
    m_list.SetItemCount(m_nShowCount);
    m_list.RedrawItems(0, m_nShowCount - 1);

    CString tmpstr;
    tmpstr.Format("Filter (%d/%d)", m_nShowCount, m_nItemCount);
    SetDlgItemTextA(IDC_STATIC3, tmpstr);

}

void CWinIndexTypeDlg::OnSelectAll()
{
    for (int i = 0; i < m_nShowCount; i++)
    {
        m_curItem[i]->check = true;
    }
    m_list.RedrawItems(0, m_nShowCount - 1);
    m_nShowType = -1;
    UpdateData(FALSE);
}

void CWinIndexTypeDlg::OnUnSelectAll()
{
    for (int i = 0; i < m_nShowCount; i++)
    {
        m_curItem[i]->check = false;
    }
    m_list.RedrawItems(0, m_nShowCount - 1);

    m_nShowType = -1;
    UpdateData(FALSE);
}

void CWinIndexTypeDlg::OnChangeFilter()
{
    UpdateData(TRUE);
    FilterShowList();
}

void CWinIndexTypeDlg::OnWriteList()
{
    auto hListView = FindListWnd();
    if (hListView != m_hLastListWnd)
    {
        AfxMessageBox("索引高级选项窗口重新打开过,需要重新读取");
        return;
    }

    int nCount = 0;
    for (auto item : *m_pArrayItem)
    {
        if (item.check != item.init_check)
        {
            nCount++;
        }
    }
    if (nCount == 0)
    {
        AfxMessageBox("没有修改任何类型");
        return;
    }

    CString query;
    query.Format("共修改%d个类型,是否写入到列表?", nCount);
    if (AfxMessageBox(query, MB_YESNO) != IDYES)
    {
        return;
    }

    //auto dwCurID = GetCurrentThreadId();
    //DWORD dwForeID = GetWindowThreadProcessId(hListView, nullptr);
    //AttachThreadInput(dwCurID, dwForeID, TRUE);

    std::string err_str;
    HANDLE hProcess = nullptr;

    LVITEM* lpItem = nullptr;

    do
    {
        DWORD ProcessID = 0;
        GetWindowThreadProcessId(hListView, &ProcessID);
        verify_break(ProcessID, "获取窗口对应进程ID失败");

        hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, ProcessID);
        verify_break(hProcess != INVALID_HANDLE_VALUE, "打开进程句柄失败");

        lpItem = (LVITEM*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
        verify_break(lpItem, "申请远进程内存失败1");

        LV_ITEM vItem = {0};

        for (int i = 0; i < m_nItemCount; i++)
        {
            if ((*m_pArrayItem)[i].check == (*m_pArrayItem)[i].init_check) continue;

            vItem.stateMask = LVIS_STATEIMAGEMASK;
            vItem.state = INDEXTOSTATEIMAGEMASK(((*m_pArrayItem)[i].check) ? 2 : 1), LVIS_STATEIMAGEMASK;

            SIZE_T dwWriteSize=0;
            verify_break(WriteProcessMemory(hProcess, lpItem, &vItem, sizeof(LVITEM),&dwWriteSize), "WriteProcessMemory失败");

            auto ret=::SendMessageA(hListView, LVM_SETITEMSTATE,(WPARAM)i, (LPARAM)lpItem);
            verify_break(ret, "LVM_SETITEMSTATE失败");
            
        }

    } while (false);

    if (err_str.empty())
    {
        AfxMessageBox("写入完成!");
    }
    else {
        AfxMessageBox(err_str.c_str(), MB_OK | MB_ICONERROR);
    }


    //AttachThreadInput(dwCurID, dwForeID, FALSE);


}

void CWinIndexTypeDlg::OnBnClickedRadio()
{
    int nLast = m_nShowType;
    UpdateData(TRUE);
    if (nLast == m_nShowType) return;
    FilterShowList(true);
}