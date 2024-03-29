#pragma once

// Standard includes.

#include "pch.h"
#include <list>
using namespace std;

//#pragma comment(lib, "ws2_32.lib")

// Firewall sub-layer names.
#define FIREWALL_SUBLAYER_NAME  "MyVistaFirewall"
#define FIREWALL_SUBLAYER_NAMEW L"MyVistaFirewall"
#define FIREWALL_SERVICE_NAMEW  FIREWALL_SUBLAYER_NAMEW

// Byte array IP address length
#define BYTE_IPADDR_ARRLEN    4

// String format IP address length
#define STR_IPADDR_LEN        32

// Vista subnet mask
#define VISTA_SUBNET_MASK   0xffffffff

// Structure to store IP address filter.
typedef struct _IPFILTERINFO {
    BYTE bIpAddrToBlock[BYTE_IPADDR_ARRLEN];
    ULONG uHexAddrToBlock;
    UINT64 u64VistaFilterId;
} IPFILTERINFO, *PIPFILTERINFO;

// List of filters.
typedef std::list<IPFILTERINFO> IPFILTERINFOLIST;

class PacketFilter
{
private:
    // Firewall engine handle.
    HANDLE m_hEngineHandle;

    // Firewall sublayer GUID.
    GUID m_subLayerGUID;

    // List of filters.
    IPFILTERINFOLIST m_lstFilters;

    // Method to get byte array format and hex format IP address from string format.
    	bool ParseIPAddrStringEx(char* szIpAddr, UINT nStrLen, BYTE* pbHostOrdr, UINT nByteLen, ULONG& uHexAddr);

    // Method to create/delete packet filter interface.
    DWORD CreateDeleteInterface( bool bCreate );

    // Method to bind/unbind to/from packet filter interface.
    DWORD BindUnbindInterface( bool bBind );

    // Method to add/remove filter.
    DWORD AddRemoveFilter( bool bAdd );

public:

    // Constructor.
    PacketFilter();

    // Destructor.
    ~PacketFilter();

    // Method to add IP addresses to m_lstFilters list.
    void AddToBlockList( char* szIpAddrToBlock );

    // Method to start packet filter.
    BOOL StartFirewall();

    // Method to stop packet filter.
    BOOL StopFirewall();
	
};

