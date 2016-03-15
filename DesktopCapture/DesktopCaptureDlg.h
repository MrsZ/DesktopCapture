
// DeskTopCaptureDlg.h : ͷ�ļ�
//

#pragma once


// CDesktopCaptureDlg �Ի���
class CDesktopCaptureDlg : public CDialogEx
{
    // ����
public:
    CDesktopCaptureDlg(CWnd* pParent = NULL);
    ~CDesktopCaptureDlg();
    //��֪ͨ����ʾ��ʾ��
    void showTips(const CString& title, const CString& info, int op = NIM_MODIFY);

// �Ի�������
    enum { IDD = IDD_DESKTOPCAPTURE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
    HICON m_hIcon;
    NOTIFYICONDATA ndMain; //֪ͨ��Ϣ�Ľṹ�����

    // ���ɵ���Ϣӳ�亯��
    virtual BOOL OnInitDialog();
    virtual void OnOk();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg long OnHotkey(WPARAM wParam, LPARAM lParam);//ȫ�ֿ�ݼ�
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};
