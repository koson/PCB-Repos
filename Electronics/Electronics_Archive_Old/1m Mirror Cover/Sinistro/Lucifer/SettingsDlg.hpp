//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  SettingsDlg.hpp
// PURPOSE: Sinistro camera settings 
// AUTHOR:  Vasiliy Popov
//
#ifndef INC_SETTINGSDLG_HPP
#define INC_SETTINGSDLG_HPP

#pragma once

#include "wincpp.hpp"

struct LuciferSettings //: public CAM_Protocol
{
   // Lucifer protocol settings
   char   szAutosavePath[_MAX_PATH*2];
   WORD   wSaturation; //scope saturation level 65535
   BOOL   bAutosave;
   BOOL   bVideo;
   UINT   uAspNum;    // last edited ASP  
   UINT   uClkNum; 
   BOOL   bClkBiasOn[CAM_MAX_CCD];
   // Sinistro image acquisition settings 
 
   // general
   UINT   uReadMode;       //CCD readout mode 1-19
   BOOL   bTDI;            //TDI mode 
   CAM_FlushRow eFlushRow; //Flush 1st row
   BOOL   bFlipY;          //CCD is flipped in vertical direction
   UINT   uPrescanCol;     //# prescan columns
   UINT   uExposureMS;
   UINT   uDelayMS;
   UINT   uLoops;
   UINT   uBinningX;
   UINT   uBinningY;
   BOOL   bROI1;
   BOOL   bROI2;
   BOOL   bROI3;
   POINT  ptMin1, ptMax1; 
   POINT  ptMin2, ptMax2; 
   POINT  ptMin3, ptMax3; 
   // asp
   UINT     ubmCCD;                                      //Bit flags for enabled CDD  
   CAM_Gain aeGain[CAM_MAX_CCD];                         //ASP gain 0=1x, 1=4x
   UINT     auFactorJJ[CAM_MAX_CCD];                     //ASP filtering JJ 1-32
   BOOL     abResetBlank[CAM_MAX_CCD];                   //reset blanking: 0=off, 1=on (same as RG)
   int      anAspOffset[CAM_MAX_CCD][CAM_MAX_CHANNELS];  //ASP channel offsets +/-[DN]
   // clk
   float    afClkBias     [CAM_MAX_CCD][eCLKBIAS_NUM]; 
   float    afClkPhaseHigh[CAM_MAX_CCD][eCLKPHASE_NUM]; 
   float    afClkPhaseLow [CAM_MAX_CCD][eCLKPHASE_NUM]; 
   // timing
   UINT     uReadNum;                   // Number of ADC reads per pixel 
   double   adSerTimeUS[eSTIME_NUM]; // Serial shift timing
   double   adParTimeUS[ePTIME_NUM]; // Parallel shift timing

   LuciferSettings();

   void Load(LPCSTR pszPath=NULL);
   void Save(LPCSTR pszPath=NULL);
};


#define CTabGeneral_BASE ADM_TabSubDialog

class CTabGeneral : public CTabGeneral_BASE
{
private:   // Attributes
   CAM_Info          m_DeviceInfo;
   const CAM_Device *m_pDevice;
   LuciferSettings  *m_pSettings;

   ADM_IntegerSpinner m_ReadMode;
   ADM_Button         m_TDI;
   ADM_RadioButtons   m_FlushRow;
   ADM_Button         m_FlipY;
   ADM_IntegerSpinner m_PrescanCol;

   ADM_IntegerSpinner m_BinningX;
   ADM_IntegerSpinner m_BinningY;
   ADM_IntegerSpinner m_ExposureMS;
   ADM_IntegerSpinner m_DelayMS; //between shutter close command and start of CCD readout
   ADM_IntegerSpinner m_Loops;

   ADM_Button         m_ROI1;
   ADM_IntegerSpinner m_Xmin1;
   ADM_IntegerSpinner m_Xmax1;
   ADM_IntegerSpinner m_Ymin1;
   ADM_IntegerSpinner m_Ymax1;

   ADM_Button         m_ROI2;
   ADM_IntegerSpinner m_Xmin2;
   ADM_IntegerSpinner m_Xmax2;
   ADM_IntegerSpinner m_Ymin2;
   ADM_IntegerSpinner m_Ymax2;

   ADM_Button         m_ROI3;
   ADM_IntegerSpinner m_Xmin3;
   ADM_IntegerSpinner m_Xmax3;
   ADM_IntegerSpinner m_Ymin3;
   ADM_IntegerSpinner m_Ymax3;

   ADM_Button         m_Video;
   ADM_Button         m_Autosave;
   ADM_ValidatedText  m_AutosavePath;

private:   // Unimplemented constructor/copy functions.
   CTabGeneral();
   CTabGeneral(const CTabGeneral &);
   const CTabGeneral &operator=(const CTabGeneral &);

private:   // Private functions
   void OnSetPath();
   void OnBinning(int nNotificationCode);
   void OnReadMode();

public:    // Public interface

   // ADM base classes should be created using a parent window
   CTabGeneral(HWND hwndParent, const CAM_Device *pDevice, LuciferSettings *pSettings);

   void LoadDialog();
   void SaveDialog();

   // override this to do pre-initialization
   virtual BOOL   OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam );

   // override this to enable/disable controls at each user change of the user interface
   virtual void   UIUpdate();

   // override this to process notifications from controls
   virtual void   Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData );

   // override this to apply changes in the dialog when the user hits OK
   virtual void   Apply();

   // override this to validate the user's values before Apply is called
   virtual BOOL   Validate( BOOL bSilent = FALSE, ADM_Wnd **ppBadWindow = NULL );
};

#define CTabAsp_BASE ADM_TabSubDialog

class CTabAsp : public CTabAsp_BASE
{
private:   // Attributes
   CAM_Info           m_DeviceInfo;
   CAM_Device        *m_pDevice;
   LuciferSettings   *m_pSettings;
   UINT               m_ubmAvailableCCD; 
   UINT               m_uAspNum; // selected board number

   ADM_CheckBoxes     m_CCDs;
   ADM_IntegerSpinner m_AspNum;
   ADM_RadioButtons   m_Gain;
   ADM_IntegerSpinner m_FactorJJ;

   ADM_IntegerSpinner m_Offset[CAM_MAX_CHANNELS];
   ADM_FloatText      m_Level [CAM_MAX_CHANNELS]; 
   ADM_Button         m_ResetBlank;
   ADM_StaticText     m_Temperature;

   CAcqTimer          m_Timer;
   ADM_Button         m_Poll;

private:   // Unimplemented constructor/copy functions.
   CTabAsp();
   CTabAsp(const CTabAsp &);
   const CTabAsp &operator=(const CTabAsp &);

private:   // Private functions
   void OnBoardSelect();
   void ShowDcLevels(bool bShowError=true);
   void OnSet();
   void OnGet();
   void OnTimer();
   void OnPollButton();

public:    // Public interface

   // ADM base classes should be created using a parent window
   CTabAsp(HWND hwndParent, CAM_Device *pDevice, LuciferSettings *pSettings);

   void LoadDialog(UINT uBoardNum);
   void SaveDialog(UINT uBoardNum);

   // override this to do pre-initialization
   virtual BOOL   OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam );

   // override this to enable/disable controls at each user change of the user interface
   virtual void   UIUpdate();

   // override this to process notifications from controls
   virtual void   Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData );

   // override this to apply changes in the dialog when the user hits OK
   virtual void   Apply();

   // override this to validate the user's values before Apply is called
   virtual BOOL   Validate( BOOL bSilent = FALSE, ADM_Wnd **ppBadWindow = NULL );

   // override this to catch any of your own registered windows messages
   virtual BOOL   ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

};

#define CTabClk_BASE ADM_TabSubDialog

class CTabClk : public CTabClk_BASE
{
private:   // Attributes
   CAM_Info          m_DeviceInfo;
   CAM_Device        *m_pDevice;
   LuciferSettings   *m_pSettings;

   UINT               m_uClkNum; // active CLK board 
   
   // CLK Board# 
   ADM_IntegerSpinner m_ClkNum;
   //DC BIAS
   ADM_FloatSpinner   m_VOD;
   ADM_FloatSpinner   m_VRD;
   ADM_FloatSpinner   m_VOG;
   ADM_FloatSpinner   m_VDD;
   ADM_FloatText      m_BiasMon[eCLKBIAS_NUM]; 
   ADM_Button         m_BiasON;

   //DC phase
   ADM_FloatSpinner   m_PhaseHigh[eCLKPHASE_NUM];
   ADM_FloatSpinner   m_PhaseLow [eCLKPHASE_NUM];
   ADM_FloatText      m_PhaseHighMon[eCLKPHASE_NUM]; 
   ADM_FloatText      m_PhaseLowMon[eCLKPHASE_NUM]; 
   ADM_StaticText     m_Temperature;

   ADM_Button         m_Poll;
   CAcqTimer          m_Timer;

private:   // Unimplemented constructor/copy functions.
   CTabClk();
   CTabClk(const CTabClk &);
   const CTabClk &operator=(const CTabClk &);

private:   // Private functions
   void ShowClkMonitors(bool bShowError = true);
   void OnBoardSelect();
   void OnBiasOn();
   void OnGet();
   void OnSet(); //debug
   void OnTimer();
   void OnPollButton();

public:    // Public interface
   // ADM base classes should be created using a parent window
   CTabClk(HWND hwndParent, CAM_Device *pDevice, LuciferSettings *pSettings);

   void LoadDialog(UINT uBoardNum);
   void SaveDialog(UINT uBoardNum);

   // override this to do pre-initialization
   virtual BOOL   OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam );

   // override this to enable/disable controls at each user change of the user interface
   virtual void   UIUpdate();

   // override this to process notifications from controls
   virtual void   Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData );

   // override this to apply changes in the dialog when the user hits OK
   virtual void   Apply();

   // override this to validate the user's values before Apply is called
   virtual BOOL   Validate( BOOL bSilent = FALSE, ADM_Wnd **ppBadWindow = NULL );

   // override this to catch any of your own registered windows messages
   virtual BOOL   ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

};


//////////////////////////////////////////////////////////////////////////////////
#define CTabTiming_BASE ADM_TabSubDialog

class CTabTiming : public CTabTiming_BASE
{
private:   // Attributes
   CAM_Info          m_DeviceInfo;
   const CAM_Device  *m_pDevice;
   LuciferSettings   *m_pSettings;

   ADM_IntegerSpinner  m_ReadNum;
   ADM_DoubleSpinner   m_SerTimeUS[eSTIME_NUM];
   ADM_DoubleSpinner   m_ParTimeUS[ePTIME_NUM];


private:   // Unimplemented constructor/copy functions.
   CTabTiming();
   CTabTiming(const CTabTiming &);
   const CTabTiming &operator=(const CTabTiming &);

private:   // Private functions

public:    // Public interface

   // ADM base classes should be created using a parent window
   CTabTiming(HWND hwndParent, const CAM_Device *pDevice, LuciferSettings *pSettings);

   void LoadDialog();
   void SaveDialog();

   // override this to do pre-initialization
   virtual BOOL   OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam );

   // override this to enable/disable controls at each user change of the user interface
   virtual void   UIUpdate();

   // override this to process notifications from controls
   virtual void   Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData );

   // override this to apply changes in the dialog when the user hits OK
   virtual void   Apply();

   // override this to validate the user's values before Apply is called
   virtual BOOL   Validate( BOOL bSilent = FALSE, ADM_Wnd **ppBadWindow = NULL );
};

////////////////////////////////////////////////////////////////////////////////

#define CTabPower_BASE ADM_TabSubDialog

class CTabPower : public CTabPower_BASE
{
private:   // Attributes
   CAM_Info          m_DeviceInfo;
   const CAM_Device  *m_pDevice;
   LuciferSettings   *m_pSettings;

  // ADM_FloatText      m_Offset[8];
  // ADM_Button         m_SequentialScan;

private:   // Unimplemented constructor/copy functions.
   CTabPower();
   CTabPower(const CTabPower &);
   const CTabPower &operator=(const CTabPower &);

private:   // Private functions

public:    // Public interface

   // ADM base classes should be created using a parent window
   CTabPower(HWND hwndParent, const CAM_Device *pDevice, LuciferSettings *pSettings);

   void LoadDialog();

   // override this to do pre-initialization
   virtual BOOL   OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam );

   // override this to enable/disable controls at each user change of the user interface
   virtual void   UIUpdate();

   // override this to process notifications from controls
   virtual void   Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData );

   // override this to apply changes in the dialog when the user hits OK
   virtual void   Apply();

   // override this to validate the user's values before Apply is called
   virtual BOOL   Validate( BOOL bSilent = FALSE, ADM_Wnd **ppBadWindow = NULL );
};


////////////////////////////////////////////////////////////////////////////////

#define CDlgSettings_BASE ADM_TabbedDialog

class CDlgSettings : public CDlgSettings_BASE
{
private:   // Attributes
   LuciferSettings     *m_pSettings;
   char                 m_szProtocolPath[_MAX_PATH];

   CTabGeneral          m_TabGeneral;
   CTabAsp              m_TabAsp;
   CTabClk              m_TabClk;
   CTabTiming           m_TabTiming;
   CTabPower            m_TabPower;

private:   // Unimplemented constructor/copy functions.
   CDlgSettings();
   CDlgSettings(const CDlgSettings &);
   const CDlgSettings &operator=(const CDlgSettings &);

   void OnLoad();
   void OnSave();
   void OnInitialize();

public:    // Public interface

   // ADM base classes should be created using a parent window
   CDlgSettings(HWND hwndParent, CAM_Device *pDevice, LuciferSettings *pSettings);

   // override this to do pre-initialization
   virtual BOOL   OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam );

   // override this to enable/disable controls at each user change of the user interface
   virtual void   UIUpdate();

   // override this to process notifications from controls
   virtual void   Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData );

   // override this to apply changes in the dialog when the user hits OK
   virtual void   Apply();
};


#endif //INC_SETTINGSDLG_HPP
