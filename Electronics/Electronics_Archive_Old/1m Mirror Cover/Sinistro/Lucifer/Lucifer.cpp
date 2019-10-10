// Lucifer.cpp : Defines the class behaviors for the application.
//
#include "wincpp.hpp"
#include "LuciferDlg.h"
#include "..\Common\ThreadName.h"

#ifdef _DEBUG
// To use AxoTrace, simply #include this file in your main 
// .cpp file, the one that contains your CWinApp-derived app, and call 
// MfxTraceInit();
// from your app's InitInstance function. You also need the AXOTRACE.EXE applet itself. 
// NOTE:
// Since AxoTrace.h contains code, you should only #include it in once place in
// your app. (Don't #include it in stdafx.h.)
   #include "..\Common\AxoTrace.h"
#endif

HINSTANCE g_hInstance;

static HWND s_hwndActiveDialog = NULL;

//==============================================================================================
// FUNCTION: GP_SetActiveWindow
// PURPOSE:  Static function to make the HWND of the active modeless dialog box 
//           available to the message pump.
//
void GP_SetActiveWindow(HWND hWnd)
{
   s_hwndActiveDialog = hWnd;
}

//==============================================================================================
// FUNCTION: SetActiveWindow
// PURPOSE:  Static function to make the HWND of the active modeless dialog box 
//           available to the message pump.
//
HWND GP_GetActiveWindow()
{
   return s_hwndActiveDialog;
}

//==============================================================================================
// FUNCTION: GP_ActivateWindow
// PURPOSE:  Avtivate a given top level window.
//
void GP_ActivateWindow(HWND hWnd)
{
   // If noi window, just get out.
   if (!hWnd)
      return;

   // Restore and activate the window.
   BringWindowToTop(hWnd);
   SetForegroundWindow(hWnd);

   // If iconic, restore the main window
   if (IsIconic(hWnd)) 
      ShowWindow( hWnd, SW_RESTORE );      

   // If there are popups, bring them along too!
   HWND hwndChild = GetLastActivePopup(hWnd);
   if (hwndChild != hWnd) 
   {
      if (IsIconic(hwndChild)) 
         ShowWindow( hwndChild, SW_RESTORE );      
      BringWindowToTop(hwndChild);
      SetForegroundWindow(hwndChild);
   }
}      

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
   g_hInstance = hInstance;
   ADM_Init(hInstance);
   AXODBG_SetDebugFlag(AXODBG_BREAK_ON_ASSERT);  
   
#ifdef _DEBUG
   MfxTraceInit();   // Redirect TRACE to AxoTrace
#endif
   // AXU_SetupStackDump();
   AXU_SetMemoryPolicy();
   SetThreadName("GUI Thread");

   UINT uSerial		= CAM_ANY_DEVICE; // Any device.
   UINT uHeight		= 0;               // 0 = default height (square)
   UINT uFlags       = 0;
   CAM_DEVICE_TYPE  eDeviceType = CAM_DEVICE_REAL;

   if (__argc > 1)
   {
      int    argc = __argc - 1;
      char **argv = __argv + 1;

      TRACE("\n*****************************************************\n");

      while (argc--)
      {
         LPCSTR psz = *argv++;
         if (psz[0]=='-' || psz[0]=='/')
         {
            switch (toupper(psz[1]))
            {
               case 'S':
                  uSerial = atoi(psz+2);
                  TRACE1("CmdLine: Serial = %d\n", uSerial);
                  break;
               case 'H':
                  uHeight = atoi(psz+2);
                  TRACE1("CmdLine: Height = %d\n", uHeight);
                  break;
               case 'D':
                  eDeviceType = CAM_DEVICE_DEMO;
                  TRACE("CmdLine: Demo Mode\n");
                  break;
            }
         }
      }
      TRACE("*****************************************************\n");
   }

   CLuciferDlg Dlg(NULL, uSerial, eDeviceType, uFlags);
   Dlg.CreateModeless();

   GP_ActivateWindow(Dlg.m_hWnd);

   MSG msg; 
   while (GetMessage(&msg, NULL, 0, 0))
   {
      HWND hwndActive = GP_GetActiveWindow();
      if (!hwndActive || !IsDialogMessage(hwndActive, &msg))   // || lRval==HOOK_TRANSLATE_KEY 
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   } 

   ADM_Shutdown( hInstance );
	return 0;
}
