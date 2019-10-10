//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  ChooseDeviceDlg.CPP
// PURPOSE: 
// AUTHOR:  BHI  May 2004
//

#include "wincpp.hpp"
#include "ChooseDeviceDlg.hpp"
#include "resource.h"

// commment out if you don't use a global hInstance in your project
extern HINSTANCE g_hInstance;

const LPCSTR c_aszColHeading[] = 
{
   "Serial", 
   "Firmware", 
   "Name", 
};

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
// NOTES:    Replace IDD_MY_DIALOG to use your own dialog resource
//           Assumes a global g_hInstance is defined
//
CChooseDeviceDlg::CChooseDeviceDlg(HWND hWndParent, CAM_Info *pInfo, UINT uInfo)
   : CChooseDeviceDlg_BASE(g_hInstance, MAKEINTRESOURCE(IDD_CHOOSE_DEVICE), hWndParent)
{
   MEMBERASSERT();
   m_pInfo   = pInfo;
   m_uInfo   = uInfo;
   m_uSerial = 0;

   // Register control classes.
   RegisterControl(IDC_LIST, &m_List);
}

//===============================================================================================
// FUNCTION: Destructor
// PURPOSE:  Object cleanup.
//
CChooseDeviceDlg::~CChooseDeviceDlg()
{
   MEMBERASSERT();
}

//===============================================================================================
// FUNCTION: OnInitDialog
// PURPOSE:  Responds to the dialog initialization message.
//
BOOL CChooseDeviceDlg::OnInitDialog( HWND hWnd, HWND hWndFocus, LPARAM lParam )
{
   MEMBERASSERT();

   // Pass the dialog initialization back to the parent (very important)
   BOOL bRval = CChooseDeviceDlg_BASE::OnInitDialog(hWnd, hWndFocus, lParam);

   DWORD dwExStyle = GetWindowExStyle(m_List.m_hWnd);
   SetWindowExStyle(m_List.m_hWnd, dwExStyle | LVS_EX_FULLROWSELECT);

	for (UINT i=0; i<ELEMENTS_IN(c_aszColHeading); i++)
      m_List.InsertColumn(i, c_aszColHeading[i]);
	
	for (i=0; i<ELEMENTS_IN(c_aszColHeading); i++)
      m_List.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

   for (i=0; i<m_uInfo; i++)
   {
      char szSerialNumber[32];
      itoa(m_pInfo[i].uSerialNumber, szSerialNumber, 10);

      // Insert the item
      m_List.InsertItem(i, szSerialNumber);
	   m_List.SetItemData(i, m_pInfo[i].uSerialNumber);

      m_List.SetItemText(i, 1, m_pInfo[i].szFirmwareVersion);
      m_List.SetItemText(i, 2, m_pInfo[i].szName);
   }
	for (i=0; i<ELEMENTS_IN(c_aszColHeading); i++)
      m_List.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

   return bRval;
}

//===============================================================================================
// FUNCTION: Notification
// PURPOSE:  Responds to notifications from the controls on the window.
//
void CChooseDeviceDlg::Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
{
   MEMBERASSERT();

   // TODO: trap your notifications here
   if (uID == ADM_NOTIFY_DBLCLICK)
      OnOk();

   // Send the notifications back to the parent for handling
   CChooseDeviceDlg_BASE::Notification(pWnd, uID, nNotificationCode, lData);
}

//===============================================================================================
// FUNCTION: Apply
// PURPOSE:  Applies the changes in the dialog.
//
void CChooseDeviceDlg::Apply()
{
   MEMBERASSERT();

   // let base class Apply first
   CChooseDeviceDlg_BASE::Apply();

   m_uSerial = m_List.GetItemData(m_List.GetNextItem(-1, LVNI_SELECTED));
}

//===============================================================================================
// FUNCTION: Validate
// PURPOSE:  Validates the user's values before Apply is called.
//
BOOL CChooseDeviceDlg::Validate( BOOL bSilent /*=FALSE*/, ADM_Wnd **ppBadWindow /*=NULL*/)
{
   MEMBERASSERT();

   // let base class Validate first
   if (!CChooseDeviceDlg_BASE::Validate(bSilent, ppBadWindow))
      return FALSE;

   if (!m_List.GetSelectedCount())
   {
      if ( ppBadWindow )
         *ppBadWindow = &m_List;
      if ( !bSilent )
         MessageBeep(UINT(-1));
      return false;
   }
   return TRUE;
}

//===============================================================================================
// FUNCTION: UIUpdate
// PURPOSE:  Updates the user interface of the dialog.
//
void CChooseDeviceDlg::UIUpdate()
{
   MEMBERASSERT();

   // let base class Apply first
   CChooseDeviceDlg_BASE::UIUpdate();

   // TODO: perform UIUpdate actions here
}

//===============================================================================================
// FUNCTION: ProcessEvent
// PURPOSE:  Processes the messages of the dialog.
//
BOOL CChooseDeviceDlg::ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   MEMBERASSERT();

   // TODO: catch your messages here
   // Note: uMsg will be your normal WM_message

   // let base class process any remaining events
   return CChooseDeviceDlg_BASE::ProcessEvent(hWnd, uMsg, wParam, lParam);
}

