#include "pch.h"
#include "framework.h"
#include "hMSLog2Ban.h"
#include "hMSLog2BanDlg.h"
#include "afxdialogex.h"
#include "hMSUtils.h"
#include "PacketFilter.h"
#include <string>
#include <algorithm>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PacketFilter pktFilter;

// ChMSLog2BanDlg dialog
ChMSLog2BanDlg::ChMSLog2BanDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HMSLOG2BAN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ChMSLog2BanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, lb_info, ED_VAR_LB_INFO);
	DDX_Control(pDX, IDCANCEL, MainFrm);
	DDX_Control(pDX, ck_unknown_user, var_ck_unknown_user);
	DDX_Control(pDX, IDC_ATTACKER_IPS, ed_attacker_ips);
	DDX_Control(pDX, IDC_BT_STOP, var_bt_stop);
	DDX_Control(pDX, IDC_BT_SCAN, var_bt_scan);
	DDX_Control(pDX, IDC_ED_TIMING, var_ed_timing);
}

BEGIN_MESSAGE_MAP(ChMSLog2BanDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &ChMSLog2BanDlg::OnBnClickedCancel)			
	ON_BN_CLICKED(IDC_BT_SCAN, &ChMSLog2BanDlg::OnBnClickedBtScan)
	ON_BN_CLICKED(IDC_BT_STOP, &ChMSLog2BanDlg::OnBnClickedBtStop)
	ON_WM_TIMER()	
	ON_WM_CLOSE()	
END_MESSAGE_MAP()

// ChMSLog2BanDlg message handlers
BOOL ChMSLog2BanDlg::OnInitDialog()
{
	// initialization
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);	  // Set big icon
	SetIcon(m_hIcon, FALSE);  // Set small icon		

	int hMSServiceState = obj->gethMSServiceStatus();
		
	if (obj->gethMSServiceStatusText(hMSServiceState) == "OK") {

		AfxGetMainWnd()->SetWindowText(L"hMSLog2Ban - (hMailServer " +obj->gethMSBuildArch() +L" Service detected)");			
		
		if (obj->gethMSCurrentLogFile().GetLength() != 0) {
			current_log = obj->gethMSLogPathFromIni() + "\\" + obj->gethMSCurrentLogFile();
			SetDlgItemTextW(lb_info, L"Current Logfile: " + current_log);
			var_ck_unknown_user.SetCheck(1);
			this->var_ed_timing.SetWindowTextW(L"5");
		}
		else {
			MessageBox(L"No hMailServer log files found. Run hMailAdmin, enable all Logfile options and restart hMailServer first and try again.",L"Warning",MB_OK);
			AfxPostQuitMessage(0);
		}
	}	
	else {
		AfxMessageBox(obj->gethMSServiceStatusText(hMSServiceState));
		AfxPostQuitMessage(0);
	}

	GetDlgItem(IDC_FW)->ShowWindow(SW_HIDE);

	return TRUE;  
}

void ChMSLog2BanDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}

HCURSOR ChMSLog2BanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void ChMSLog2BanDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

int ChMSLog2BanDlg::Search_in_File(char* fname, CString search_pattern)
{	
	this->ed_attacker_ips.ResetContent();
	FILE* stream;
	int count = 0;
	int maximumLineLength = 128;
	char* lineBuffer = (char*)malloc(sizeof(char) * maximumLineLength);

	if ((stream = _fsopen(fname, "r", _SH_DENYNO)) != NULL) 
	{			
		char line[256];
		while (fgets(line, sizeof(line), stream)) {
				
			CString bfind = search_pattern;
			CString str = CString(line, 256);
								
				if (str.Find(bfind) != -1)
				 this->ed_attacker_ips.AddString(str);		
		}
		fclose(stream);
	}
	return 0;
}

void ChMSLog2BanDlg::start_fw()
{
	int ItemCount = 0;
	ItemCount = this->ed_attacker_ips.GetCount();

	for (int i = 0; i <= ItemCount - 1; i++) {
		pktFilter.AddToBlockList(CT2A(this->lookupips(i)));

		//// Start firewall.			
		if (pktFilter.StartFirewall()) {
			GetDlgItem(IDC_BT_SCAN)->EnableWindow(FALSE);
			GetDlgItem(IDC_BT_STOP)->EnableWindow(true);
			GetDlgItem(IDC_FW)->ShowWindow(SW_SHOW);
			
		}
		else {
			MessageBox(L"Error starting Firewall!", L"Error", MB_OK);
			GetDlgItem(IDC_BT_SCAN)->EnableWindow(true);
			GetDlgItem(IDC_FW)->ShowWindow(SW_HIDE);
		}
	}	
}

void ChMSLog2BanDlg::OnBnClickedBtScan()
{
	CString interval;
	var_ed_timing.GetWindowText(interval);
	int var_timing = _wtoi(interval);
	var_timing = var_timing * 1000;
	SetTimer(1, var_timing, NULL);
	GetDlgItem(IDC_BT_SCAN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_STOP)->EnableWindow(true);
	GetDlgItem(IDC_FW)->ShowWindow(SW_SHOW);
	
}

/* Iterates line by line trough CListBox and extract the ip addresses */
CString ChMSLog2BanDlg::lookupips(int index)
{
	CString item;
	this->ed_attacker_ips.GetText(index, item);
	std::string x = CT2A(item.GetString());
	x.erase(std::remove(x.begin(), x.end(), '"'), x.end());
	x.erase(std::remove(x.begin(), x.end(), '\t'), x.end());
	
	string temp = x;
	temp.erase(0, 33);
	temp.erase(temp.length()-23);	
	//MessageBox((CString)temp.c_str(), L"Debug", MB_OK);
	return (CString)temp.c_str();
}

/* Stop button */
void ChMSLog2BanDlg::OnBnClickedBtStop()
{
	KillTimer(1);
	start_date_time.Empty();
	pktFilter.StopFirewall();
	GetDlgItem(IDC_BT_SCAN)->EnableWindow(true);
	GetDlgItem(IDC_BT_STOP)->EnableWindow(false);	
	GetDlgItem(IDC_FW)->ShowWindow(SW_HIDE);	
}

void ChMSLog2BanDlg::OnTimer(UINT_PTR nIDEvent)
{
	Search_in_File(CT2A(current_log), L"SENT: 550 Unknown user");

	CTime CurrentTime = CTime::GetCurrentTime();
	int iHours = CurrentTime.GetHour();
	int iMinutes = CurrentTime.GetMinute();
	int iSeconds = CurrentTime.GetSecond();
	CString strHours, strMinutes, strSeconds;

	if (iHours < 10)
		strHours.Format(_T("0%d"), iHours);
	else
		strHours.Format(_T("%d"), iHours);

	if (iMinutes < 10)
		strMinutes.Format(_T("0%d"), iMinutes);
	else
		strMinutes.Format(_T("%d"), iMinutes);

	if (iSeconds < 10)
		strSeconds.Format(_T("0%d"), iSeconds);
	else
		strSeconds.Format(_T("%d"), iSeconds);
	
	if (start_date_time.IsEmpty())
	     start_date_time.Format(_T("%s:%s:%s"), strHours, strMinutes, strSeconds);

	CString now_date_time;
	now_date_time.Format(_T("%s:%s:%s"), strHours, strMinutes, strSeconds);

	CString fw_status = L"ACTIVE ";

	GetDlgItem(IDC_FW)->SetWindowTextW(fw_status + L" Start: " + start_date_time +
		" / Last log scan: " + now_date_time);

	UpdateData(FALSE);
	CDialogEx::OnTimer(nIDEvent);

	// If everything is prepared, start blocking
	start_fw();

}


void ChMSLog2BanDlg::OnClose()
{
	pktFilter.StopFirewall();
	CDialogEx::OnClose();
}


