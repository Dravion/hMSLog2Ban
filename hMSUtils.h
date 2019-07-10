#pragma once

#include "pch.h"
#include "PacketFilter.h"

using namespace std;

class hMSUtils
{
public:
	hMSUtils();	
	int gethMSServiceStatus();	
	CString gethMSServicePath();	
	CString gethMSLogPathFromIni();
	CString gethMSServiceStatusText(int code);
	CString gethMSBuildArch();	
	CString gethMSCurrentLogFile();
	CString getLockedFile(CString fname);

};

