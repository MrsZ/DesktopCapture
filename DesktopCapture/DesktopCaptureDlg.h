
// DeskTopCaptureDlg.h : 头文件
//

#pragma once


// CDesktopCaptureDlg 对话框
class CDesktopCaptureDlg : public CDialogEx
{
public:
    explicit CDesktopCaptureDlg(CWnd* pParent = NULL);
    ~CDesktopCaptureDlg();
    //在通知栏显示提示框
    void showTips(const CString& title, const CString& info, int op = NIM_MODIFY);
    void OnCapture();
    void OnOpenFolder();

    // 对话框数据
    enum { IDD = IDD_DESKTOPCAPTURE_DIALOG };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

    HICON m_hIcon;
    NOTIFYICONDATA ndMain; //通知消息的结构体变量

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    virtual void OnOk();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg long OnHotkey(WPARAM wParam, LPARAM lParam);//全局快捷键
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg LRESULT OnNotifyIconMsg(WPARAM wParam, LPARAM lParam);//处理通知栏消息
    DECLARE_MESSAGE_MAP()
};
