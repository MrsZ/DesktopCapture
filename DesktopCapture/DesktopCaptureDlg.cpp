
// DeskTopCaptureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DesktopCapture.h"
#include "DesktopCaptureDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool autoCapture = true;//�Ƿ��Զ���ʱ��ͼ

///����GDI����Ե�ǰ��Ļ���н�ͼ����
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
//����ǰ��Ļ���浽�ļ�
void CaptureScreen(char *filename = "")
{
    static const CString defaultDir = "D:/ScreenCapture/"; //Ĭ��Ŀ¼
    char folder[500] = "";
    GetPrivateProfileString("ScreenCapture", "fatherFolder", defaultDir, folder, 500, ".\\config.ini");
    if (folder[strlen(folder) - 1] != '/' && folder[strlen(folder) - 1] != '\\')
        strcat(folder, "/"); //���Ŀ¼�ָ���
    CString lastFolder = folder;
    if (!PathFileExists(folder)) { //�ļ�����������Ҫ����
        _mkdir(folder);  //��������Ŀ¼
        if (!PathFileExists(folder)) {//����ļ�����Ȼ������,������ʧ��
            strcpy(folder, defaultDir);
            _mkdir(folder);  //��������Ŀ¼
            WritePrivateProfileString("ScreenCapture", "#��ܰ��ʾ"
                , "�趨Ŀ¼����д,���Զ�����ΪĬ��Ŀ¼", ".\\config.ini");
        }
        WritePrivateProfileString("ScreenCapture", "fatherFolder", folder, ".\\config.ini");
    }
    CString fullPathName = filename;
    //���û�д����ļ����Ĳ������������ļ��е�·���͵�ǰ���ں�ʱ�������ļ���
    if (filename == 0 || filename[0] == 0) {
        time_t now_time = time(NULL);
        struct tm *newtime = localtime(&now_time);
        char _date[128], _time[128];
        strftime(_date, 128, "%Y%m%d", newtime);
        strftime(_time, 128, "%H%M%S", newtime);
        CString now = "";
        now.Format("%s_%s", _date, _time);
        captureImgPath = (CString)folder + _date;
        _mkdir(captureImgPath);//�����Ӽ�Ŀ¼��Ŀ¼��Ϊ����
        SYSTEMTIME currentTime;
        GetSystemTime(&currentTime); //��ȡ��ǰʱ��ĺ�����
        fullPathName.Format("%s/%s-%d.png", captureImgPath, now, currentTime.wMilliseconds);
    }
    //��ʼ��ȡ��Ļ����
    CDC *pDC = CDC::FromHandle(::GetDC(NULL));//��ȡ��ǰ������ĻDC
    int Width = pDC->GetDeviceCaps(HORZRES);
    int Height = pDC->GetDeviceCaps(VERTRES);

    CDC memDC;//�ڴ�DC
    memDC.CreateCompatibleDC(pDC);

    CBitmap memBitmap;
    memBitmap.CreateCompatibleBitmap(pDC, Width, Height);//��������Ļ���ݵ�bitmap
    CBitmap *oldmemBitmap = memDC.SelectObject(&memBitmap);//��memBitmapѡ���ڴ�DC
    memDC.BitBlt(0, 0, Width, Height, pDC, 0, 0, SRCCOPY);//������Ļͼ���ڴ�DC
                                                          //���´��뱣��memDC�е�λͼ��png�ļ�
    Bitmap mbitmap(HBITMAP(memBitmap), 0);//��CBitmap�еõ�HBitmap
    CLSID imgCode = { 0 }; //ͼ�����
    if (-1 != GetEncoderClsid(L"image/png", imgCode)) { //ѡ�����
        USES_CONVERSION;
        CStringW  ws = A2W(fullPathName);
        mbitmap.Save(ws.GetBuffer(), &imgCode);//����
    }
    memDC.SelectObject(oldmemBitmap);
    //����֪ͨ������
    CString ps = "\n���ɰ���Ctrl+Alt+1�򿪽�ͼ�����ļ���Ӵ"; //������Ϣ
    if (lastFolder != folder)    //����û����õ�Ŀ¼����д����ʾ
        ps = "\n��Ŷ,�趨Ŀ¼<" + CString(lastFolder) + ">����д,���Զ�����ΪĬ��Ŀ¼:" + folder;
    static CDesktopCaptureDlg* pDlg = (CDesktopCaptureDlg*)AfxGetMainWnd();
    pDlg->showTips("��ͼ�ɹ�", "�ѽ���ͼ�ļ����浽:" + fullPathName + ps);
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


// CDesktopCaptureDlg �Ի���

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
    ON_MESSAGE(WM_HOTKEY, OnHotkey) //��Ӵ˾�ʵ��ȫ�ֿ�ݼ�
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CDesktopCaptureDlg ��Ϣ�������

BOOL CDesktopCaptureDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);		// ����Сͼ��
    ShowWindow(SW_HIDE);//����ʾ����
    ModifyStyle(WS_THICKFRAME, NULL);//����ʾ�߿�
    ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);//������������ʾ
    MoveWindow(-9, 700, 10, 10);

    // TODO: �ڴ���Ӷ���ĳ�ʼ������
    //ע��ȫ�ֽ�ͼ��ݼ�
    ::RegisterHotKey(m_hWnd, ID_HKCAPTURE, MOD_CONTROL | MOD_ALT, '0');
    //ע��ȫ�ִ򿪽�ͼ�ļ���
    ::RegisterHotKey(m_hWnd, ID_HKOPENFOLDER, MOD_CONTROL | MOD_ALT, '1');
    //ע��ȫ���Զ���ͼ���� / ֹͣ��ݼ�
    ::RegisterHotKey(m_hWnd, ID_HKChangeAutoCap, MOD_CONTROL | MOD_ALT, '2');
    //ע��ȫ���˳���ݼ�
    ::RegisterHotKey(m_hWnd, ID_HKExit, MOD_CONTROL | MOD_ALT, '9');
    AfxBeginThread(catchScreenThread, 0); //���������߳��Զ���ʱ��ͼ
    //���������Ϣ
    ndMain.cbSize = sizeof(NOTIFYICONDATA);
    ndMain.hWnd = m_hWnd;
    ndMain.uFlags = NIF_INFO | NIF_ICON | NIF_MESSAGE | NIF_TIP;
    ndMain.hIcon = m_hIcon;
    strcpy(ndMain.szTip, "��ͼС����");
    showTips("��ܰ��ʾ", "��ͼ�������������У�����Ctrl+Alt+0���Խ�ͼŶ", NIM_ADD);
    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDesktopCaptureDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // ʹͼ���ڹ����������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    }
    else {
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

//�򿪴�Ž�ͼ���ļ���
void CDesktopCaptureDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (captureImgPath == "")
        CaptureScreen();
    ShellExecute(this->GetSafeHwnd(), "open", captureImgPath, 0, 0, SW_SHOW);
    CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CDesktopCaptureDlg::showTips(const CString& title, const CString& info, int op)
{
    strcpy(ndMain.szInfoTitle, title);//���ݱ���
    strcpy(ndMain.szInfo, info);//��������  
    Shell_NotifyIcon(op, &ndMain);
}
