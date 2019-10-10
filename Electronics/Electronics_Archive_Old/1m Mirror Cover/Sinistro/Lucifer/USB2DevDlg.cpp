//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// USB2DevDlg.cpp : implementation file
//

#include "wincpp.hpp"
#include "USB2DevDlg.h"
#include "resource.h"	// main symbols

#include "..\common\AxoDefn.h"
#include "..\common\AxoDebug.h"
#include "..\common\WaitCursor.hpp"
#include "..\inc\AxoUtils32.h"

#include <math.h>

#include "ChooseDeviceDlg.hpp"

// commment out if you don't use a global hInstance in your project
extern HINSTANCE g_hInstance;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BYTE s_Data[65536];

const UINT WM_OPEN_DEVICE = WM_APP+333;

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
// NOTES:    Replace IDD_MY_DIALOG to use your own dialog resource
//           Assumes a global g_hInstance is defined
//
CUSB2DevDlg::CUSB2DevDlg(HWND hwndParent, CAM_Device* pDevice)
   : CUSB2DevDlg_BASE(g_hInstance, MAKEINTRESOURCE(IDD_USB2DEV_DIALOG), hwndParent)
   ,m_pDevice (pDevice)
   ,m_bInit   (false)

{
   MEMBERASSERT();
   // Register control classes here.
   RegisterControl( IDC_VR_REQUEST, &m_VRRequestHex);
	RegisterControl( IDC_VR_VALUE,   &m_VRValueHex);
	RegisterControl( IDC_VR_INDEX, 	&m_VRIndex);
	RegisterControl( IDC_VR_TIMEOUT, &m_VRTimeout);
	RegisterControl( IDC_VR_BYTES,   &m_VRBytes);
	RegisterControl( IDC_VR_DATA,    &m_sVRData);

   RegisterControl( IDC_EEPROM_BYTES,	&m_EEBytes);

	RegisterControl( IDC_EEPROM_START, &m_EEStartHex);
	RegisterControl( IDC_EEPROM_DATA, &m_sEEData);

   RegisterControl( IDC_BULK_DATA, &m_sBulkData);
	RegisterControl( IDC_BULK_BYTES,	&m_BulkBytes);

	RegisterControl( IDC_NAME, &m_sName);
	RegisterControl( IDC_INFO, &m_Info);

	m_VRRequestHex = "70"; //VR_REG
	m_VRValueHex   = "0";  //Device ID = ASP/CLK #1
	m_VRIndex      =  0;  //MSP timer reg 
   m_EEStartHex   = "0";

   m_VRTimeout = 1000;
	m_VRBytes   = sizeof(long);
	m_EEBytes   = 0;
   m_BulkBytes = 0;
}

CUSB2DevDlg::~CUSB2DevDlg()
{
   MEMBERASSERT();
}

/////////////////////////////////////////////////////////////////////////////
// CUSB2DevDlg message handlers

BOOL CUSB2DevDlg::OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam )
{
   MEMBERASSERT();

   // Pass the dialog initialization back to the parent (very important)
   BOOL bRval = CUSB2DevDlg_BASE::OnInitDialog(hWnd, hwndFocus, lParam);

   m_bInit = false;

   ShowDeviceInfo(NULL);
   if (m_pDevice) {
      if (m_pDevice->GetDeviceInfo(&m_DeviceInfo))
         ShowDeviceInfo(&m_DeviceInfo);
   }
   
   //m_Progress.SetRange(0, 100);

   return bRval;  // return TRUE  unless you set the focus to a control
}

bool CUSB2DevDlg::ShowDeviceError(int nError)
{
   if (m_pDevice)
   {
      DWORD dwError = m_pDevice->GetLastError();
      char sz[256] = "";
      AXU_sprintf(sz, ELEMENTS_IN(sz), "CAM ERROR: 0x%08X  ", dwError);
      m_pDevice->GetLastErrorText(sz+strlen(sz), ELEMENTS_IN(sz)-strlen(sz));

      MessageBox(sz, "Error", MB_OK);
      
	   m_VRBytes   = sizeof(long);   
   }
   return false;
}

void CUSB2DevDlg::StringToBytes(LPCSTR psz, UINT uBytes)
{
   CString s(psz);
   LPSTR p = NULL;
   LPSTR pByte = strtok(s.LockBuffer(), " ");
   UINT uCount = 0;
   while (pByte)
   {
      if (uCount>=uBytes)
         break;
      s_Data[uCount++] = (BYTE)strtol(pByte, &p, 16);
      pByte = strtok(NULL, " ");
   }
   while (uCount < uBytes)
      s_Data[uCount++] = 0;
   s.UnlockBuffer();
}

CString CUSB2DevDlg::BytesToString(UINT uBytes)
{
   CString rval;
   CString s;
   for (UINT i=0; i<uBytes; i++)
   {
      s.Format("%02X", s_Data[i]);
      if (i > 0)
         rval += " ";
      rval += s;
   }
   return rval;
}

void CUSB2DevDlg::ShowDeviceInfo(const CAM_Info *pInfo)
{
   if (pInfo)
   {
      char sz[16];
      m_sName.SetWindowText(m_DeviceInfo.szName);
      AXU_sprintf(sz, ELEMENTS_IN(sz), "%04X", m_DeviceInfo.wVendorID);
      m_Info.SetFormatItem(0, sz );
      AXU_sprintf(sz, ELEMENTS_IN(sz), "%04X", m_DeviceInfo.wProductID);
      m_Info.SetFormatItem(1, sz );
      m_Info.SetFormatItem(2, m_DeviceInfo.szFirmwareVersion);
      m_Info.SetFormatItem(3, (long)m_DeviceInfo.uSerialNumber);
   }
   else
   {
      m_sName.SetWindowText("");
      m_Info.SetFormatItem(0, 0 );
      m_Info.SetFormatItem(1, "");
      m_Info.SetFormatItem(2, 0);
   }
}

void CUSB2DevDlg::OnDestroy( HWND hwnd )
{
   MEMBERASSERT();
//   CloseDevice();
}

//===============================================================================================
// FUNCTION: ProcessEvent
// PURPOSE:  Low level message hook.
//
BOOL CUSB2DevDlg::ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   MEMBERASSERT();

   switch (uMsg)
   {
      case WM_PAINT:
         if (!m_bInit)
            ::PostMessage(m_hWnd, WM_OPEN_DEVICE, 0, 0);
         m_bInit = true;
         break;

      case WM_OPEN_DEVICE:
      {
  //    if (m_wProductID && m_uDevice!=UINT(-1))
  //       OpenDevice(m_wProductID, m_uDevice, m_szFirmware, m_szFPGA);
  //    if (!m_pDevice)
  //       OnOpenDevice();
  //    if (!m_pDevice)
  //       OnCancel();
         break;
      }
   }
   return CUSB2DevDlg_BASE::ProcessEvent( hWnd, uMsg, wParam, lParam );
}


class CProgress : public CAM_ProgressCallback
{
   CUSB2DevDlg *m_pDlg;
public:
   CProgress(CUSB2DevDlg *pDlg) { m_pDlg = pDlg; }
   ~CProgress()                 { m_pDlg->SetProgress(0); }

   virtual bool Progress(int nPercent)
   {
      return m_pDlg->SetProgress(nPercent);
   }
};

bool CUSB2DevDlg::SetProgress(int nPercent)
{
   if (nPercent==0)
      m_Progress.ShowWindow(SW_HIDE);
   else if (!m_Progress.IsWindowVisible())
      m_Progress.ShowWindow(SW_SHOWNORMAL);
   m_Progress.SetPos(nPercent);
   return true;
}

//##############################################################################################
// CLASS:   COpenFileDlg
// PURPOSE: Common file dialog for opening EEPROM files.
//
class COpenFileDlg : public ADM_FileDialog
{
private:
   COpenFileDlg( const COpenFileDlg &);
   const COpenFileDlg &operator=( const COpenFileDlg &);

public:
   COpenFileDlg(HWND hwndParent, LPCSTR pszPath)
      : ADM_FileDialog(hwndParent)
   {
      SetDialogType( eOpen );

      // Change the title of the parent dialog.
      SetTitle("Open USB EEPROM file");
      SetFilter("I2C Files (*.iic)|*.iic|All Files (*.*)|*.*||");

      // Set the required flags.
      SetFlags( OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR );

      // Set the initial directory.
      SetInitialDir(pszPath);
   }
};

void CUSB2DevDlg::OnReLoad() 
{
   if (!m_pDevice)
      return;

//   CWaitCursor Wait;

   ShowDeviceInfo(NULL);

   // download firmware if required
   if (!m_pDevice->ReopenDevice())
   {
      ShowDeviceError();
      return;
   }
   if (m_pDevice->GetDeviceInfo(&m_DeviceInfo))
      ShowDeviceInfo(&m_DeviceInfo);
}

void CUSB2DevDlg::OnDownloadEeprom() 
{
   if (!m_pDevice)
      return;

   COpenFileDlg Dlg(m_hWnd, m_szPath);
   if (!Dlg.CreateModal())
      return;
   
   //CWaitCursor Wait;
   CProgress Callback(this);
   if (!m_pDevice->DownloadEEPROM(m_szPath, &Callback))
      ShowDeviceError();
}

void CUSB2DevDlg::OnVrRead() 
{
   if (!m_pDevice)
      return;
   
   char sz[32] = "";
   AXU_STRNCPYZ(sz, m_VRRequestHex);
   BYTE byVR = (BYTE)strtoul(sz, NULL, 16);
      
   AXU_STRNCPYZ(sz, m_VRValueHex);
   WORD wValue = (WORD)strtoul(sz, NULL, 16);
	
   WORD wIndex = (WORD)m_VRIndex;

   WORD wReturned = 0;
   if (!m_pDevice->VendorReadRequest(byVR, wValue, wIndex, UINT(m_VRTimeout), 
                                     s_Data, WORD(m_VRBytes), &wReturned))
      ShowDeviceError();
   else
      m_VRBytes  = wReturned;
   CString sData = BytesToString(WORD(m_VRBytes));
   m_sVRData.SetWindowText( sData.GetBuffer(1024) );
}

void CUSB2DevDlg::OnVrWrite() 
{
   if (!m_pDevice)
      return;
   
   char szText[1024];
   m_sVRData.GetWindowText(szText, ELEMENTS_IN(szText));
   StringToBytes(szText, (BYTE)m_VRBytes);

   char sz[32] = "";
   AXU_STRNCPYZ(sz, m_VRRequestHex);
   BYTE byVR = (BYTE)strtoul(sz, NULL, 16);
      
   AXU_STRNCPYZ(sz, m_VRValueHex);
   WORD wValue = (WORD)strtoul(sz, NULL, 16);
	
   WORD wIndex = (WORD)m_VRIndex;

   if (!m_pDevice->VendorWriteRequest(byVR, wValue, wIndex, UINT(m_VRTimeout),
                                      s_Data, WORD(m_VRBytes)))
      ShowDeviceError();
}

void CUSB2DevDlg::OnEepromRead() 
{
   if (!m_pDevice)
      return;

   char sz[32] = "";
   AXU_STRNCPYZ(sz, m_EEStartHex);
   WORD wEEStart = (WORD)strtoul(sz, NULL, 16);

   if (!m_pDevice->ReadFromEEPROM(wEEStart, s_Data, (BYTE)m_EEBytes))
   {
      m_sEEData.SetWindowText("");
      ShowDeviceError();
   }
   else {
      CString szText = BytesToString((WORD)m_EEBytes); 
      m_sEEData.SetWindowText(szText.GetBuffer(1024));
   }
}

void CUSB2DevDlg::OnEepromWrite() 
{
   if (!m_pDevice)
      return;

   char sz[1024] = "";
   AXU_STRNCPYZ(sz, m_EEStartHex);
   WORD wEEStart = (WORD)strtoul(sz, NULL, 16);

   m_sVRData.GetWindowText(sz, ELEMENTS_IN(sz));
   StringToBytes(sz, (WORD)m_EEBytes); //-> s_Data

   if (!m_pDevice->WriteToEEPROM(wEEStart, s_Data, (WORD)m_EEBytes))
      ShowDeviceError();
}

void CUSB2DevDlg::OnBulkRead() 
{
   if (!m_pDevice)
      return;

   if (!m_pDevice->ReadFromPipe(s_Data, (WORD)m_BulkBytes))
   {
      m_sBulkData.SetWindowText("");
      ShowDeviceError();
   }
   else {
      CString sz = BytesToString((WORD)m_BulkBytes);
      m_sBulkData.SetWindowText(sz.GetBuffer(1024));
   }
}

void CUSB2DevDlg::OnReset() 
{
   if (!m_pDevice)
      return;

   if (!m_pDevice->Reset())
      ShowDeviceError();
}

//===============================================================================================
// FUNCTION: UIUpdate
// PURPOSE:  Updates the user interface of the dialog.
//
void CUSB2DevDlg::UIUpdate()
{
   MEMBERASSERT();

   // let base class Apply first
   CUSB2DevDlg_BASE::UIUpdate();

   // TODO: perform UIUpdate actions here
}


//===============================================================================================
// FUNCTION: Apply
// PURPOSE:  Applies the changes in the dialog.
//
void CUSB2DevDlg::Apply()
{
   MEMBERASSERT();

   // let base class Apply first
   CUSB2DevDlg_BASE::Apply();

   // TODO: perform Apply actions here
}

//===============================================================================================
// FUNCTION: Notification
// PURPOSE:  Responds to notifications from the controls on the window.
//
void CUSB2DevDlg::Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
{
   MEMBERASSERT();
   
   if (uID == IDC_VR_READ)
	   OnVrRead();
   else if (uID == IDC_VR_WRITE)
	   OnVrWrite();
   else if (uID == IDC_EEPROM_READ)
	   OnEepromRead();
   else if (uID == IDC_EEPROM_WRITE)
	   OnEepromWrite();
   else if (uID == IDC_BULK_READ)
	   OnBulkRead();
   else if (uID == IDC_DOWNLOAD_EEPROM)
	   OnDownloadEeprom();
   else if (uID == IDC_RESET)
	   OnReset();
   else if (uID == IDC_RELOAD)
	   OnReLoad();

   // Send the notifications back to the parent for handling
   CUSB2DevDlg_BASE::Notification(pWnd, uID, nNotificationCode, lData);
}


