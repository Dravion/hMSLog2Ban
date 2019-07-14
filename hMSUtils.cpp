#include "pch.h"

hMSUtils::hMSUtils()
{
}

CString hMSUtils::getLockedFile(CString fname)
{
	return (L"");
}

CString hMSUtils::gethMSServiceStatusText(int code)
{
	CString service_state = nullptr;
	switch (code) {
		case 1001: service_state = "Error, not installed or not accessible";
			break;
		case 1002: service_state = "hMailServer Service exists but is not running";
			break;
		case 1003: service_state = "OK";		
			break;  
		default: std::cout << "FAIL";
	}
	return service_state;
}

int hMSUtils::gethMSServiceStatus()
{
	TCHAR szSvcName[80] = L"hMailServer";
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded;
	int iReturn = 0;

	SC_HANDLE schSCManager = OpenSCManager(
		nullptr,						// local computer
		nullptr,						// ServicesActive database 
		SC_MANAGER_QUERY_LOCK_STATUS);  // full access rights 

	if (nullptr == schSCManager) {
	}

	// Get a handle to the service.
	SC_HANDLE schService = OpenService(
		schSCManager,         // SCM database 
		szSvcName,            // name of service 			
		SERVICE_QUERY_STATUS);

	if (schService == nullptr)
	{
		CloseServiceHandle(schSCManager);
	}

	// Make sure the service is not already stopped.
	if (!QueryServiceStatusEx(
		schService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)& ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded))
	{
		iReturn = 1001; // Error, not installed or not accessible
		
	}

	if (ssp.dwCurrentState == SERVICE_STOPPED) {
		iReturn = 1002; // hMailServer Service exists but not running
	}

	if (ssp.dwCurrentState == SERVICE_RUNNING) {
		iReturn = 1003; // hMailServer Service exists and is running
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return iReturn;
}

CString hMSUtils::gethMSBuildArch()
{
	DWORD dwBinaryType;
	CString strResult = nullptr;
	CString hMSServicePath = gethMSServicePath()+L"\\hMailServer.exe";

	if (GetBinaryType(hMSServicePath, &dwBinaryType)
		&& dwBinaryType == SCS_64BIT_BINARY) {
		strResult = L"64-Bit";	
	}

	if (GetBinaryType(hMSServicePath, &dwBinaryType)
		&& dwBinaryType == SCS_32BIT_BINARY) {
		strResult = L"32-Bit";	
	}
	return strResult;
}

CString hMSUtils::gethMSCurrentLogFile()
{
	CString found;
	WIN32_FIND_DATA ffd;	
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	
	StringCchLength(gethMSLogPathFromIni(), MAX_PATH, &length_of_arg);
	StringCchCopy(szDir, MAX_PATH, gethMSLogPathFromIni());
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));	
	hFind = FindFirstFile(szDir, &ffd);
	   
	// List all the files in the directory

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// dont list directory dots			
		}
		else {
			if (
			   (!wcscmp(ffd.cFileName, L"hmailserver_awstats.log") == 0) &
			   (!wcscmp(ffd.cFileName, L"hmailserver_events.log") == 0) &
			   (!wcscmp(ffd.cFileName, L"hmailserver_backup.log") == 0) 
			 ) {			    
				int x = 1;
				HANDLE hFile;
				hFile = CreateFile(ffd.cFileName,
					GENERIC_READ,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile == INVALID_HANDLE_VALUE) {
					found = ffd.cFileName; 					
				}
				CloseHandle(hFile);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	return found;
}

CString hMSUtils::gethMSServicePath()
{
	HRESULT hres;

	hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hres)) {}

	hres = CoInitializeSecurity(
		nullptr,
		-1,
		nullptr,
		nullptr,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr,
		EOAC_NONE,
		nullptr
	);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemLocator* pLoc = nullptr;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)& pLoc);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemServices* pSvc = nullptr;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		nullptr,                 // User name. NULL = current user
		nullptr,                 // User password. NULL = current
		nullptr,                 // Locale. NULL indicates current
		0,                       // Security flags.
		nullptr,                 // Authority (for example, Kerberos)
		nullptr,                 // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
	}

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		nullptr,                     // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		nullptr,                     // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IEnumWbemClassObject* pEnumerator = nullptr;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT PathName FROM Win32_Service WHERE name ='hmailserver'"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		nullptr,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IWbemClassObject* pclsObj = nullptr;
	ULONG uReturn = 0;

	pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

	VARIANT vtProp;
	pclsObj->Get(L"PathName", 0, &vtProp, nullptr, nullptr);
	CString result = vtProp.bstrVal;
	VariantClear(&vtProp);
	pclsObj->Release();

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();
	   	
	result.Replace(L"RunAsService", L"");	
	result.Delete(0);
	result.Delete(result.GetLength() -2);
	result.Delete(result.GetLength() -1);
	result.Replace(L"\\hMailServer.exe",L"");

	return result;
}

CString hMSUtils::gethMSLogPathFromIni()
{	TCHAR szKeyValue[MAX_PATH];
	CString inipath = gethMSServicePath() + L"\\hmailserver.ini";
	GetPrivateProfileString(L"Directories", L"LogFolder", nullptr, szKeyValue, MAX_PATH, inipath);
	return szKeyValue;
}






