
// DeskTopCaptureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DesktopCapture.h"
#include "DesktopCaptureDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool autoCapture = true;//是否自动定时截图

///利用GDI对象对当前屏幕进行截图保存
using namespace Gdiplus;
int GetEncoderClsid(const WCHAR* format, CLSID& _Clsid)
{
    UINT num = 0;  // number of image encoders
    UINT size = 0; // size of the image encoder array in bytes
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1; // Failure

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) {
        free(pImageCodecInfo);
        return -1;     //Failure
    }

    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j)
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            _Clsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;     //Success
        }
    free(pImageCodecInfo);
    return -1;     //Failure
}

CString captureImgPath = "";
#include<direct.h>	//_mkdir
//将当前屏幕保存到文件
void CaptureScreen(char *filename = "")
{
    static const CString defaultDir = "D:/ScreenCapture/"; //默认目录
    char folder[500] = "";
    GetPrivateProfileString("ScreenCapture", "fatherFolder", defaultDir, folder, 500, ".\\config.ini");
    if (folder[strlen(folder) - 1] != '/' && folder[strlen(folder) - 1] != '\\')
        strcat(folder, "/"); //添加目录分隔符
    CString lastFolder = folder;
    if (!PathFileExists(folder)) { //文件不存在则需要创建
        _mkdir(folder);  //建立父级目录
        if (!PathFileExists(folder)) {//如果文件夹仍然不存在,即创建失败
            strcpy(folder, defaultDir);
            _mkdir(folder);  //建立父级目录
            WritePrivateProfileString("ScreenCapture", "#温馨提示"
                                      , "设定目录不可写,已自动设置为默认目录", ".\\config.ini");
        }
        WritePrivateProfileString("ScreenCapture", "fatherFolder", folder, ".\\config.ini");
    }
    CString fullPathName = filename;
    //如果没有传递文件名的参数则用配置文件中的路径和当前日期和时间生成文件名
    if (filename == 0 || filename[0] == 0) {
        time_t now_time = time(NULL);
        struct tm *newtime = localtime(&now_time);
        char _date[128], _time[128];
        strftime(_date, 128, "%Y%m%d", newtime);
        strftime(_time, 128, "%H%M%S", newtime);
        CString now = "";
        now.Format("%s_%s", _date, _time);
        captureImgPath = (CString)folder + _date;
        _mkdir(captureImgPath);//建立子级目录，目录名为日期
        SYSTEMTIME currentTime;
        GetSystemTime(&currentTime); //获取当前时间的毫秒数
        fullPathName.Format("%s/%s-%d.png", captureImgPath, now, currentTime.wMilliseconds);
    }
    //开始获取屏幕内容
    CDC *pDC = CDC::FromHandle(::GetDC(NULL));//获取当前整个屏幕DC
    int Width = pDC->GetDeviceCaps(HORZRES);
    int Height = pDC->GetDeviceCaps(VERTRES);

    CDC memDC;//内存DC
    memDC.CreateCompatibleDC(pDC);

    CBitmap memBitmap;
    memBitmap.CreateCompatibleBitmap(pDC, Width, Height);//建立和屏幕兼容的bitmap
    CBitmap *oldmemBitmap = memDC.SelectObject(&memBitmap);//将memBitmap选入内存DC
    memDC.BitBlt(0, 0, Width, Height, pDC, 0, 0, SRCCOPY);//复制屏幕图像到内存DC
    //以下代码保存memDC中的位图到png文件
    Bitmap mbitmap(HBITMAP(memBitmap), 0);//从CBitmap中得到HBitmap
    CLSID imgCode = { 0 }; //图像编码
    if (-1 != GetEncoderClsid(L"image/png", imgCode)) { //选择编码
        USES_CONVERSION;
        CStringW  ws = A2W(fullPathName);
        mbitmap.Save(ws.GetBuffer(), &imgCode);//保存
    }
    memDC.SelectObject(oldmemBitmap);
    //弹出通知栏气泡
    CString ps = "\n您可按下Ctrl+Alt+1打开截图所在文件夹哟"; //附加信息
    if (lastFolder != folder)    //如果用户设置的目录不可写则提示
        ps = "\n啊哦,设定目录<" + CString(lastFolder) + ">不可写,已自动设置为默认目录:" + folder;
    static CDesktopCaptureDlg* pDlg = (CDesktopCaptureDlg*)AfxGetMainWnd();
    pDlg->showTips("截图成功", "已将截图文件保存到:" + fullPathName + ps);
}

UINT catchScreenThread(void*)
{
    while (1) {
        int getTime = GetPrivateProfileInt("ScreenCapture", "Sleep", 3000, ".\\config.ini");
        Sleep(getTime);
        if (autoCapture)
            CaptureScreen();
    }
    return 0;
}


// CDesktopCaptureDlg 对话框

CDesktopCaptureDlg::CDesktopCaptureDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CDesktopCaptureDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDesktopCaptureDlg::~CDesktopCaptureDlg()
{
    showTips("", "", NIM_DELETE);
    UnregisterHotKey(m_hWnd, ID_HKCAPTURE);
    UnregisterHotKey(m_hWnd, ID_HKOPENFOLDER);
    UnregisterHotKey(m_hWnd, ID_HKChangeAutoCap);
    UnregisterHotKey(m_hWnd, ID_HKExit);
}

void CDesktopCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDesktopCaptureDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_HOTKEY, OnHotkey) //添加此句实现全局快捷键
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CDesktopCaptureDlg 消息处理程序

BOOL CDesktopCaptureDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标
    ShowWindow(SW_HIDE);//不显示窗口
    ModifyStyle(WS_THICKFRAME, NULL);//不显示边框
    ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);//不在任务栏显示
    MoveWindow(-8, 700, 10, 10);

    // TODO: 在此添加额外的初始化代码
    //注册全局截图快捷键
    ::RegisterHotKey(m_hWnd, ID_HKCAPTURE, MOD_CONTROL | MOD_ALT, '0');
    //注册全局打开截图文件夹
    ::RegisterHotKey(m_hWnd, ID_HKOPENFOLDER, MOD_CONTROL | MOD_ALT, '1');
    //注册全局自动截图开启 / 停止快捷键
    ::RegisterHotKey(m_hWnd, ID_HKChangeAutoCap, MOD_CONTROL | MOD_ALT, '2');
    //注册全局退出快捷键
    ::RegisterHotKey(m_hWnd, ID_HKExit, MOD_CONTROL | MOD_ALT, '9');
    AfxBeginThread(catchScreenThread, 0); //开启辅助线程自动定时截图
    //添加托盘消息
    ndMain.cbSize = sizeof(NOTIFYICONDATA);
    ndMain.hWnd = m_hWnd;
    ndMain.uFlags = NIF_INFO | NIF_ICON | NIF_MESSAGE | NIF_TIP;
    ndMain.hIcon = m_hIcon;
    strcpy(ndMain.szTip, "截图小工具");
    showTips("温馨提示", "截图工具已正常运行，按下Ctrl+Alt+0可以截图哦", NIM_ADD);
    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDesktopCaptureDlg::OnPaint()
{
    if (IsIconic()) {
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
    } else {
        CDialogEx::OnPaint();
    }
}

HCURSOR CDesktopCaptureDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

long CDesktopCaptureDlg::OnHotkey(WPARAM wParam, LPARAM lParam)
{
    switch (wParam) {
    case ID_HKCAPTURE:
        CaptureScreen();
        break;
    case ID_HKExit:
        PostQuitMessage(-1);
        break;
    case ID_HKOPENFOLDER:
        OnLButtonDblClk(0, 0);
        break;
    case ID_HKChangeAutoCap:
        autoCapture = !autoCapture;
        break;
    default:
        break;
    }
    return 0;
}

void CDesktopCaptureDlg::OnOk()
{
}

//打开存放截图的文件夹
void CDesktopCaptureDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (captureImgPath == "") //路径为空表示还没保存过截图,先保存一张然后打开该文件夹
        CaptureScreen();
    else if (!PathFileExists(captureImgPath)) //路径不为空还需要保证文件夹存在,因为外部可能主动删除
        _mkdir(captureImgPath);
    ShellExecute(GetSafeHwnd(), "open", captureImgPath, 0, 0, SW_SHOW);
    CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CDesktopCaptureDlg::showTips(const CString& title, const CString& info, int op)
{
    strcpy(ndMain.szInfoTitle, title);//气泡标题
    strcpy(ndMain.szInfo, info);//气泡内容
    Shell_NotifyIcon(op, &ndMain);
}
