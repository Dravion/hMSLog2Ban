#pragma once


// ChMSLog2BanDlg dialog
class ChMSLog2BanDlg : public CDialogEx
{

// Construction
public:
	ChMSLog2BanDlg(CWnd* pParent = nullptr);	// standard constructor
	CString start_date_time;

	
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HMSLOG2BAN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	hMSUtils* obj = new hMSUtils();
	CString current_log;
	CListBox VAR_ED_LOGFILE;
	CStatic ED_VAR_LB_INFO;
	CButton MainFrm;
	afx_msg void OnBnClickedCancel();	
	afx_msg void OnBnClickedBtScan();
	CButton var_ck_unknown_user;
	int Search_in_File(char* fname, CString search_pattern);
	void start_fw();
	CString lookupips(int index); 
	CListBox ed_attacker_ips;
	CButton bt_scan_var;	
	afx_msg void OnBnClickedBtStop();
	afx_msg void OnStnClickedLbFwStatus();
	CStatic var_lb_fw_status;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CButton var_bt_stop;
	CButton var_bt_scan;
	afx_msg void OnClose();
	CStatic m_strTimer;

	CEdit ed_timer_var;
	CEdit var_ed_timing;
};
