//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  ChooseDeviceDlg.HPP
// PURPOSE: 
// AUTHOR:  BHI  May 2004
//

#ifndef INC_CHOOSEDEVICEDLG_HPP
#define INC_CHOOSEDEVICEDLG_HPP

#include "wincpp.hpp"

struct CAM_Info;

#define CChooseDeviceDlg_BASE ADM_Dialog

class CChooseDeviceDlg : public CChooseDeviceDlg_BASE
{
private:   // Attributes
   ADM_ListControl m_List;
   CAM_Info       *m_pInfo;
   UINT            m_uInfo;
   UINT            m_uSerial;

private:   // Unimplemented constructor/copy functions.
   CChooseDeviceDlg();
   CChooseDeviceDlg(const CChooseDeviceDlg &);
   const CChooseDeviceDlg &operator=(const CChooseDeviceDlg &);

private:   // Private functions

public:    // Public interface

   // ADM base classes should be created using a parent window
   CChooseDeviceDlg(HWND hWndParent, CAM_Info *pInfo, UINT uInfo);

   // Destructor
   virtual ~CChooseDeviceDlg();

   UINT GetSerial() const { return m_uSerial; }

   // override this to do pre-initialization
   virtual BOOL   OnInitDialog( HWND hWnd, HWND hWndFocus, LPARAM lParam );

   // override this to process notifications from controls
   virtual void   Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData );

   // override this to apply changes in the dialog when the user hits OK
   virtual void   Apply( void );

   // override this to validate the user's values before Apply is called
   virtual BOOL   Validate( BOOL bSilent = FALSE, ADM_Wnd **ppBadWindow = NULL );

   // override this to enable/disable controls at each user change of the user interface
   virtual void   UIUpdate( void );

   // override this to catch any of your own registered windows messages
   virtual BOOL   ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif      // INC_CHOOSEDEVICEDLG_HPP

