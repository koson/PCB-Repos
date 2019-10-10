//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  LuciferDlg.h
// PURPOSE: 

#ifndef INC_DLGAUTOFOCUSREVIEW_HPP
#define INC_DLGAUTOFOCUSREVIEW_HPP

#pragma once

#include "wincpp.hpp"

#include "ImageScope.hpp"
#include "Image.hpp"
#include "SettingsDlg.hpp"

#include "..\Sinistro\driver.hpp"  // c++ driver dll interface

const UINT LUCIDLG_FLAG_QUIET     = 0x00000001;

enum CAM_DEVICE_TYPE
{
   CAM_DEVICE_DEMO,
   CAM_DEVICE_REAL,
};

#define CLuciferDlg_BASE ADM_Dialog

class CLuciferDlg : public CLuciferDlg_BASE //, private CActionCallback
{
private:   // Attributes
   UINT              m_uSerial;
   CAM_DEVICE_TYPE   m_eDeviceType;
   UINT              m_uFlags;

   CCamDriver        m_Driver;
   CAM_Device        *m_pDevice;
   CAM_Info          m_DeviceInfo;
   CRect             m_rFrame;
   LPCSTR            m_aszChannels[CAM_DISPLAYS];
   LuciferSettings   m_Settings;

   CAcqTimer         m_Timer;
   bool              m_bInTimerMsg;
   bool              m_bInit;
   bool              m_bAcquiring;
   bool              m_bQuiet;
   bool              m_bIgnoreScopeMsg;
   UINT              m_uActiveChannel;

   CImageScope       m_ImageScope;
   CImages           m_Images;

   ADM_StaticText    m_CursorPos;
   ADM_StaticText    m_ContrastValue;
   ADM_StaticText    m_BrightnessValue;
   ADM_TrackBar      m_Contrast;
   ADM_TrackBar      m_Brightness;
   ADM_Button        m_Snap;
   ADM_Button        m_Controls;
   ADM_StaticSpinner m_Channel;
   ADM_StaticText    m_ModeParams;

   ADM_AnimateCtrl   m_Animation;
   char              m_szPath[_MAX_PATH];

   //ADM_IntegerText   m_ScanLines;
   //CProgressBar      m_ProgressBar;
   ADM_Dialog        *m_pControlDlg;  // modeless dialog
   ADM_Dialog        *m_pUsbDevDlg;

private:   // Unimplemented constructor/copy functions.
   CLuciferDlg();
   CLuciferDlg(const CLuciferDlg &);
   const CLuciferDlg &operator=(const CLuciferDlg &);

private:   // Private functions
   void LoadFromRegistry();
   void SaveToRegistry();

   float GetContrast();
   float GetBrightness();
   void SetContrast(float fContrast);
   void SetBrightness(float fBrightness);
   void SetContrastText(float fContrast);
   void SetBrightnessText(float fBrightness);
   BOOL OnContrastChange(int nNotificationCode);
   BOOL OnBrightnessChange(int nNotificationCode);
   BOOL OnPinHoleChange(int nNotificationCode);
   void ShowPinHolePos(int nPinHolePos);
   void RoundToNearestQuanta();
   void OnSize();
   bool OnClose();
   void OnSaveTiff();
   void OnSaveRaw();
   void OnLeftClick();
   BOOL GetMousePos(POINT &pt);
   void OnMouseMove();
   void OnSelectionCompleted(IMAGESCOPE_AREANOTIFY *pAreaNotify);
   void NewScanRegion(LPCRECT prScan);
   void SetDefaultImages();
   bool ShowDeviceError(int nError=0);
   bool ShowCloseError();
   void On2Byte();
   void RewriteDataFile();
   void OnDeviceReset();
   void OnTimer();
   void OnSnap();
   void OnUsbDevDlg();

   bool OpenDevice();
   void SetChannel(int nStep);
   
   bool ValidateSettings();
   void EditSettings();

   bool SetupFrame();
   bool Snap();
   void Stop(int nError=0);

   bool SaveTiff(LPCSTR pszFilename);
   bool SaveRaw(LPCSTR pszFilename);
   void StartTimer();

   void SetDlgItemUINT(UINT uID, UINT uVal) { ::SetDlgItemInt(m_hWnd, uID, uVal, false);        }
   UINT GetDlgItemUINT(UINT uID) const      { return ::GetDlgItemInt(m_hWnd, uID, NULL, false); }

   void OnScopeMessage(UINT uMsg, LPARAM lParam);

   // CActionCallback interface
   virtual void ShowLines(UINT uLines, bool bUpdateScope);
   virtual void OnMovie();

public:    // Public interface
   // ADM base classes should be created using a parent window
   CLuciferDlg(HWND hwndParent, UINT uSerial, CAM_DEVICE_TYPE eDeviceType, UINT uFlags);
   ~CLuciferDlg();

   // override this to do pre-initialization
   virtual BOOL   OnInitDialog( HWND hWnd, HWND hWndFocus, LPARAM lParam );

   // override this to process notifications from controls
   virtual void   Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData );

   virtual BOOL   ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
   virtual void   OnDestroy( HWND hwnd );
   virtual HBRUSH OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int nType);

   void StartAnimation(int nAnimation);
   void StopAnimation();
   BOOL Redraw();
};

//##############################################################################################
// CLASS:   COpenFileDlg
// PURPOSE: Common file dialog for opening images.
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
      SetTitle("Open Bin Image");
      SetFilter("Raw Files (*.bin)|*.bin|All Files (*.*)|*.*||");

      // Set the required flags.
      SetFlags( OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR );

      // Set the initial directory.
      SetInitialDir(pszPath);
   }
};

#endif      // INC_DLGAUTOFOCUSREVIEW_HPP

