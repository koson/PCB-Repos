//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// USB2DevDlg.h : header file
//
#include "wincpp.hpp"

#include "afxcmn.h"

#define CUSB2DevDlg_BASE ADM_Dialog

/////////////////////////////////////////////////////////////////////////////
// CUSB2DevDlg dialog

class CUSB2DevDlg : public CUSB2DevDlg_BASE
{
private:
// Attributes
   CAM_Info       m_DeviceInfo;
   CAM_Device*    m_pDevice;
   bool           m_bInit;

	ADM_ValidatedText	m_VRRequestHex;
	ADM_ValidatedText	m_VRValueHex;
	//ADM_ValidatedText	m_VRIndexHex;
	ADM_IntegerText	m_VRIndex;
   ADM_ValidatedText m_EEStartHex;

   ADM_IntegerText	m_VRTimeout;
	ADM_IntegerText	m_VRBytes;
	ADM_IntegerText	m_EEBytes;
   ADM_IntegerText	m_BulkBytes;

	ADM_Text	         m_sVRData;
   ADM_Text	         m_sEEData;
	ADM_Text          m_sBulkData;
   
	ADM_Text          m_sName;
   ADM_StaticText    m_Info; 

   CProgressCtrl     m_Progress;
   char              m_szPath[_MAX_PATH*2];

public:    // Public interface

   // ADM base classes should be created using a parent window
   CUSB2DevDlg(HWND hwndParent, CAM_Device* pDevice );

   // Destructor
   virtual ~CUSB2DevDlg();

   // override this to do pre-initialization
   virtual BOOL   OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam );

   // override this to process notifications from controls
   virtual void   Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData );

   // override this to apply changes in the dialog when the user hits OK
   virtual void   Apply();

   // override this to enable/disable controls at each user change of the user interface
   virtual void   UIUpdate();

   // override this to catch any of your own registered windows messages
   virtual BOOL   ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

   virtual void   OnDestroy( HWND hwnd );

   bool SetProgress(int nPercent);

private:   // Private functions
	void OnVrRead();
	void OnVrWrite();
	void OnEepromRead();
	void OnEepromWrite();
	void OnBulkRead();
	void OnDownloadEeprom();
	void OnReset();
	void OnReLoad();

   bool ShowDeviceError(int nError=0);
   void ShowDeviceInfo(const CAM_Info *pInfo);
   void StringToBytes(LPCSTR psz, UINT uBytes);
   CString BytesToString(UINT uBytes);
};
