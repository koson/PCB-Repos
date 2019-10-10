//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  LuciferDlg.cpp
// PURPOSE: 
// AUTHOR:  BHI  Aug 2004
//

#include "wincpp.hpp"
#include "resource.h"
#include "LuciferDlg.h"
#include "USB2DevDlg.h"

#include <vector>
#include <string>
#include <algorithm>

// Set the following to 1 to save as multi-image TIFF files.
#define USE_MULTI_IMAGE_TIFF 1

extern HINSTANCE g_hInstance;

const char c_szAppName[] = "Lucifer";

DECLARE_STR(Lucifer);
DECLARE_STR(Path);
DECLARE_STR(Brightness);
DECLARE_STR(Contrast);
DECLARE_STR(Name);
DECLARE_STR(NA);
DECLARE_STR(Exposure);
DECLARE_STR(MultiGain);

const UINT c_uTimerID     = 666;
const UINT c_uTimerPeriod = 20;

const UINT WM_OPEN_DEVICE   = WM_APP+333;
const UINT WM_NEWSCANREGION = WM_APP+334;

const int c_nNmPerPixel  = 1000; 

static int EnableList[] = { IDC_SNAP };

const int c_nQUANTA = 100; //contrast/brightness

static float InxToValue(int nInx)
{
   return -float(nInx*2)/c_nQUANTA;
}
static int ValueToInx(float fValue)
{
   return Round(-fValue*c_nQUANTA/2);
}

static int ValueToPercentage(float fValue)
{
   return Round(fValue*50.0+50.0);
}
static LPCSTR GetDriverName(CAM_DEVICE_TYPE eDeviceType)
{
   switch (eDeviceType)
   {
      case CAM_DEVICE_DEMO:
         return "SinistroDemo.dll";
      case CAM_DEVICE_REAL:
         return "Sinistro.dll";
   }
   ERRORMSG1("Unexpected device type (%d)", eDeviceType);
   return "Sinistro.dll";
}

//===============================================================================================
// CLASS:   CRunAnimation
// PURPOSE: Auto-object to run the animation.
//
class CRunAnimation
{
private:
   CLuciferDlg *m_pDlg;

public:
   CRunAnimation(CLuciferDlg *pDlg) { m_pDlg = pDlg; m_pDlg->StartAnimation(IDR_BUSY); }
   ~CRunAnimation()                  { if (m_pDlg)    m_pDlg->StopAnimation();                 }
   void Release()                    { m_pDlg = NULL;                                          }
};

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CLuciferDlg::CLuciferDlg(HWND hwndParent, UINT uSerial, CAM_DEVICE_TYPE eDeviceType, UINT uFlags)
   : CLuciferDlg_BASE(g_hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), hwndParent)
{
   MEMBERASSERT();
   m_uSerial         = uSerial;
   m_eDeviceType     = eDeviceType;
   m_uFlags          = uFlags;

   m_pDevice         = NULL;
   m_bInit           = false;
   m_bAcquiring      = false;
   m_bQuiet          = ((uFlags & LUCIDLG_FLAG_QUIET) != 0);
   m_bIgnoreScopeMsg = false;
   m_uActiveChannel  = 0;
   m_bInTimerMsg     = false;

   m_pControlDlg     = NULL;
   m_pUsbDevDlg      = NULL;

   static ADM_Window_Position Pos;
   SetPositionStorage(&Pos);

   m_bRegisterOkButton     = FALSE; // Base class flag for whether to register the Ok button
   m_bRegisterCancelButton = FALSE; // Base class flag for whether to register the Cancel button

   RegisterControl(IDC_SNAP,             &m_Snap);
   RegisterControl(IDC_CONTROLS,         &m_Controls);
   RegisterControl(IDC_ANIMATION,        &m_Animation);

   RegisterControl(IDC_CURSORPOS,        &m_CursorPos);
   RegisterControl(IDC_CONTRAST_VALUE,   &m_ContrastValue);
   RegisterControl(IDC_CONTRAST,         &m_Contrast);
   RegisterControl(IDC_BRIGHTNESS_VALUE, &m_BrightnessValue);
   RegisterControl(IDC_BRIGHTNESS,       &m_Brightness);
   RegisterControl(IDC_MODEPARAMS,       &m_ModeParams);

   m_Channel.RegisterControl(this, IDC_CHANNEL, IDC_CHANNEL_SPINNER);

   LoadFromRegistry();
}

//===============================================================================================
// FUNCTION: Destructor
// PURPOSE:  Object cleanup.
//
CLuciferDlg::~CLuciferDlg()
{
   MEMBERASSERT();
   Stop();

   if (m_pControlDlg) 
   {
		// destroy modeless control dialog
      m_pControlDlg->DestroyWindow();
		delete m_pControlDlg;
      m_pControlDlg =	0;
   }

   if (m_pUsbDevDlg) 
   {
		// destroy modeless control dialog
      m_pUsbDevDlg->DestroyWindow();
		delete m_pUsbDevDlg;
      m_pUsbDevDlg =	0;
   }

   if (IsWindow(m_hWnd))
      DestroyWindow();
}

//===============================================================================================
// FUNCTION: LoadFromRegistry
// PURPOSE:  Serialise from registry.
//
void CLuciferDlg::LoadFromRegistry()
{
   m_szPath[0]  = '\0';

   char szDefault[_MAX_PATH] = "";
   AXU_GetDefaultDataDirectory(szDefault, ELEMENTS_IN(szDefault));
   AXU_GetPrivateProfileString(USE_STR(Lucifer), USE_STR(Path), szDefault, m_szPath, sizeof(m_szPath), c_szAppName);
   
   //Load camera settings
   m_Settings.Load();

   m_ImageScope.SetSaturationColor(65535);//m_Settings.wSaturation);

   SaveToRegistry();
}

//===============================================================================================
// FUNCTION: SaveToRegistry
// PURPOSE:  Serialise to registry.
//
void CLuciferDlg::SaveToRegistry()
{
   MEMBERASSERT();
   AXU_WritePrivateProfileString(USE_STR(Lucifer), USE_STR(Path), m_szPath, c_szAppName);
}

//===============================================================================================
// FUNCTION: OnInitDialog
// PURPOSE:  Responds to the dialog initialization message.
//
BOOL CLuciferDlg::OnInitDialog( HWND hWnd, HWND hWndFocus, LPARAM lParam )
{
   MEMBERASSERT();
   //CWaitCursor Wait;

   // Pass the dialog initialization back to the parent (very important)
   BOOL bRval = CLuciferDlg_BASE::OnInitDialog(hWnd, hWndFocus, lParam);

	m_Animation.Open(IDR_BUSY);
   m_ImageScope.Create(hWnd, IDC_IMAGE);
   m_Contrast.SetPageSteps( -1, 1 );
   m_Contrast.SetRange( -c_nQUANTA/2, c_nQUANTA/2 );
   m_Brightness.SetPageSteps( -1, 1 );
   m_Brightness.SetRange( -c_nQUANTA/2, c_nQUANTA/2 );

   float fContrast   = m_ImageScope.GetContrast();
   float fBrightness = m_ImageScope.GetBrightness();

   AXU_GetPrivateProfileFloat(USE_STR(Lucifer), USE_STR(Brightness), fBrightness, &fBrightness, c_szAppName);
   AXU_GetPrivateProfileFloat(USE_STR(Lucifer), USE_STR(Contrast),   fContrast,   &fContrast,   c_szAppName);

   SetContrast(fContrast);
   SetContrastText(fContrast);

   SetBrightness(fBrightness);
   SetBrightnessText(fBrightness);

   m_ImageScope.SetContrast(fContrast);
   m_ImageScope.SetBrightness(fBrightness);
   m_ImageScope.SetMouseState(IMAGESCOPE_ZOOMSTATE);

//   SetDlgItemText(m_hWnd, IDC_LINE_COUNT, "");
   OnSize();
   m_ImageScope.FullScale();

   m_aszChannels[0] = "L";
   m_aszChannels[1] = "H";
   m_uActiveChannel = 0;


   SetDlgItemText(m_hWnd, IDC_NAME,     "");
   SetDlgItemText(m_hWnd, IDC_VERSION,  "");
   SetDlgItemText(m_hWnd, IDC_FIRMWARE, "");
   SetDlgItemText(m_hWnd, IDC_SERIAL,   "");

   return bRval;
}

//===============================================================================================
// FUNCTION: ShowDeviceError
// PURPOSE:  Utility function for error handling.
//
bool CLuciferDlg::ShowDeviceError(int nError)
{
   if (!nError && m_pDevice)
      nError = m_pDevice->GetLastError();
   if (!nError)
      return false;

   char sz[256] = "";
   m_Driver.GetErrorText(nError, sz, ELEMENTS_IN(sz));
   MessageBox(sz, "Error", MB_OK);
   return false;
}

//===============================================================================================
// FUNCTION: ShowCloseError
// PURPOSE:  .
//
bool CLuciferDlg::ShowCloseError()
{
   int nError = (m_pDevice ? m_pDevice->GetLastError() : 0);
   if (!nError)
      return true;

   char sz[256] = "";
   m_Driver.GetErrorText(nError, sz, ELEMENTS_IN(sz));
   AXU_STRNCATZ(sz, "\n\nExit anyway?");
   return MessageBox(sz, "Error", MB_OKCANCEL)==IDOK;
}

//===============================================================================================
// FUNCTION: GetContrast
// PURPOSE:  .
//
float CLuciferDlg::GetContrast()
{
   MEMBERASSERT();
   return InxToValue(m_Contrast.GetPos());
}

//===============================================================================================
// FUNCTION: GetBrightness
// PURPOSE:  .
//
float CLuciferDlg::GetBrightness()
{
   MEMBERASSERT();
   return InxToValue(m_Brightness.GetPos());
}

//===============================================================================================
// FUNCTION: SetContrast
// PURPOSE:  .
//
void CLuciferDlg::SetContrast(float fContrast)
{
   MEMBERASSERT();
   m_Contrast.SetPos(ValueToInx(fContrast));
}

//===============================================================================================
// FUNCTION: SetBrightness
// PURPOSE:  .
//
void CLuciferDlg::SetBrightness(float fBrightness)
{
   MEMBERASSERT();
   m_Brightness.SetPos(ValueToInx(fBrightness));
}

//===============================================================================================
// FUNCTION: SetContrastText
// PURPOSE:  .
//
void CLuciferDlg::SetContrastText(float fContrast)
{
   MEMBERASSERT();
   m_ContrastValue.SetFormatItem(0, ValueToPercentage(fContrast));
}

//===============================================================================================
// FUNCTION: SetBrightnessText
// PURPOSE:  .
//
void CLuciferDlg::SetBrightnessText(float fBrightness)
{
   MEMBERASSERT();
   m_BrightnessValue.SetFormatItem(0, ValueToPercentage(fBrightness));
}

//==============================================================================================
// FUNCTION: OnContrastChange
// PURPOSE:  Sets the contrast parameter for the current scope.
//
BOOL CLuciferDlg::OnContrastChange(int nNotificationCode)
{
   MEMBERASSERT();

   BOOL bTracking = (nNotificationCode == ADM_NOTIFY_GAUGE_THUMBPOS);
   if ((nNotificationCode == ADM_NOTIFY_GAUGE_CHANGED) || bTracking)
   {
      float fContrast = GetContrast();
      m_ImageScope.SetContrast(fContrast);
      SetContrastText(fContrast);

      Redraw();
      return true;
   }
   return false;
}

//==============================================================================================
// FUNCTION: OnBrightnessChange
// PURPOSE:  Sets the contrast parameter for the current scope.
//
BOOL CLuciferDlg::OnBrightnessChange(int nNotificationCode)
{
   MEMBERASSERT();

   BOOL bTracking = (nNotificationCode == ADM_NOTIFY_GAUGE_THUMBPOS);
   if ((nNotificationCode == ADM_NOTIFY_GAUGE_CHANGED) || bTracking)
   {
      float fBrightness = GetBrightness();
      m_ImageScope.SetBrightness(fBrightness);
      SetBrightnessText(fBrightness);

      Redraw();
      return true;
   }
   return false;
}

//##############################################################################################
// CLASS:   CTiffFileDlg
// PURPOSE: Common file dialog for saving images.
//
class CTiffFileDlg : public ADM_FileDialog
{
private:
   CTiffFileDlg( const CTiffFileDlg &);
   const CTiffFileDlg &operator=( const CTiffFileDlg &);

public:
   CTiffFileDlg(HWND hwndParent, LPCSTR pszPath, bool bTiff)
      : ADM_FileDialog(hwndParent)
   {
      SetDialogType( eSave );

      // Change the title of the parent dialog.
      if (bTiff)
      {
         SetTitle("Save TIFF Image");
         SetFilter("Tiff Files (*.tif)|*.tif|All Files (*.*)|*.*||");
      }
      else
      {
         SetTitle("Save Raw Images");
         SetFilter("Raw Files (*.raw)|*.raw|All Files (*.*)|*.*||");
      }

      // Set the required flags.
      SetFlags( OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT );

      // Set the initial directory.
      SetInitialDir(pszPath);
   }
};

//===============================================================================================
// FUNCTION: OnSaveTiff
// PURPOSE:  .
//
void CLuciferDlg::OnSaveTiff()
{
   MEMBERASSERT();
   
   CTiffFileDlg Dlg(m_hWnd, m_szPath, true);

   if (!Dlg.CreateModal())
      return;

   AXU_STRNCPYZ(m_szPath, Dlg.GetFile());

   if (GetFileAttributes(m_szPath)!=UINT(-1))
      DeleteFile(m_szPath);
   SaveTiff(m_szPath);
}

//===============================================================================================
// FUNCTION: OnSaveRaw
// PURPOSE:  .
//
void CLuciferDlg::OnSaveRaw()
{
   MEMBERASSERT();
   
   CTiffFileDlg Dlg(m_hWnd, m_szPath, false);

   if (!Dlg.CreateModal())
      return;

   AXU_STRNCPYZ(m_szPath, Dlg.GetFile());
   SaveRaw(m_szPath);
}

//===============================================================================================
// FUNCTION: SetDefaultImages
// PURPOSE:  Sets up some default images.
//
void CLuciferDlg::SetDefaultImages()
{
   MEMBERASSERT();

   m_rFrame.right  = m_DeviceInfo.SensorInfo.uWidth;
   m_rFrame.bottom = m_DeviceInfo.SensorInfo.uHeight;

   LPCSTR aszNames[2] = { "1", "2" };
   m_Images.SetProps(aszNames, ELEMENTS_IN(aszNames), c_nNmPerPixel, NULL);

   m_ImageScope.SetImages(&m_Images, m_rFrame); 
   SetChannel(0); // Doesn't set channel zero, the param is a step value.
}

//===============================================================================================
// FUNCTION: SetChannel
// PURPOSE:  .
//
void CLuciferDlg::SetChannel(int nStep)
{
   MEMBERASSERT();

   int nChannel = int(m_uActiveChannel) + nStep;
   if (nChannel >= (int)m_Images.GetCount())
      m_uActiveChannel = 0;
   else if (nChannel < 0)
      m_uActiveChannel = m_Images.GetCount()-1;
   else
      m_uActiveChannel = nChannel;

   LPCSTR pszName = m_Images.GetImageName(m_uActiveChannel);
   m_Channel.SetFormatItem(0, pszName);

   m_ImageScope.SetActive(m_uActiveChannel);
   Redraw();
}

//==============================================================================================
// FUNCTION: OnSize
// PURPOSE:  Called in response to a resize action.
//
void CLuciferDlg::OnSize()
{
   HWND hScope = m_ImageScope.GetHWND();
   CRect rScope, rClient;
   ::GetWindowRect(hScope, rScope);

   GetClientRect(rClient);
   ClientToScreen(m_hWnd, &rClient.BottomRight());
   ClientToScreen(m_hWnd, &rClient.TopLeft());

   rScope.BottomRight() = rClient.BottomRight();

   AXU_SetWindowRect(hScope, rScope);
   m_ImageScope.Redraw();
}

//===============================================================================================
// FUNCTION: NewScanRegion
// PURPOSE:  .
//
void CLuciferDlg::NewScanRegion(LPCRECT prScan)
{
   MEMBERASSERT();
   m_ImageScope.SetScanRect(prScan);
   Redraw();
}

//==============================================================================================
// FUNCTION: SaveTiff
// PURPOSE:  Saves the current image to a tiff file.
//
bool CLuciferDlg::SaveTiff(LPCSTR pszFilename)
{
#if USE_MULTI_IMAGE_TIFF
   if (GetFileAttributes(pszFilename)!=UINT(-1))
   {
      char szMsg[512] = "";
      AXU_sprintf(szMsg, ELEMENTS_IN(szMsg), "Do you wish to overwrite file '%s'.\n", pszFilename);
      if (MessageBox(szMsg, "Error", MB_OKCANCEL)==IDCANCEL)
         return false;
   }
   int nError = 0;
   
   AXIIO_FileProperties FileProps;
   FileProps.eFormat = AXIIO_TIFF;
   AXU_STRNCPYZ(FileProps.szComment, "Lucifer debugging");

   HAXIIO hImageIO = AXIIO_WriteOpen(pszFilename, &FileProps, &nError);
   if (!hImageIO)
      return false;

   bool bOK = true;
   for (UINT i=0; i<m_Images.GetCount(); i++)
   {
      CImage *pImage  = m_Images.GetImage(i);
      int nImageBytes = pImage->GetPixelCount() * sizeof(WORD);

      CRect r = pImage->GetBounds();

      AXIIO_ImageProperties ImageProps;
      ImageProps.nWidth       = r.Width();
      ImageProps.nHeight      = r.Height();
      ImageProps.nFullWidth   = r.Width();
      ImageProps.nFullHeight  = r.Height();

      ImageProps.eFormat = AXIIO_GREYSCALE_16;
      ImageProps.nMaxSampleValue = 65535;
      ImageProps.nMinSampleValue = 0;
      ImageProps.dMicronsPerPixel= pImage->GetNmPerPixel() ? pImage->GetNmPerPixel()/1E3 : 10.0;

      AXU_sprintf(ImageProps.szComment, ELEMENTS_IN(ImageProps.szComment), "Channel %d", i);

      if (!AXIIO_WriteImage(hImageIO, &ImageProps, pImage->GetPtr(), nImageBytes, &nError))
         bOK = false;
   }

   AXIIO_Close(hImageIO);
#else
   int nError = 0;
   
   AXIIO_FileProperties FileProps;
   FileProps.eFormat = AXIIO_TIFF;
   AXU_STRNCPYZ(FileProps.szComment, "Lucifer debugging");

   bool bOK = true;
   for (UINT i=0; i<m_Images.GetCount(); i++)
   {
      CFilePath Path(pszFilename);
      char szName[_MAX_PATH*2];
      AXU_sprintf(szName, ELEMENTS_IN(szName), "%s_w%d", Path.GetFileName(), i+1);
      Path.SetFileName(szName);

      if (GetFileAttributes(Path)!=UINT(-1))
      {
         char szMsg[512] = "";
         AXU_sprintf(szMsg, ELEMENTS_IN(szMsg), "Do you wish to overwrite file '%s'.\n", Path.GetPath());
         if (MessageBox(szMsg, "Error", MB_OKCANCEL)==IDCANCEL)
            return false;
      }

      HAXIIO hImageIO = AXIIO_WriteOpen(Path, &FileProps, &nError);
      if (!hImageIO)
      {
         bOK = false;
         continue;
      }

      CImage *pImage  = m_Images.GetImage(i);
      int nImageBytes = pImage->GetPixelCount() * sizeof(WORD);

      CRect r = pImage->GetBounds();

      AXIIO_ImageProperties ImageProps;
      ImageProps.nWidth       = r.Width();
      ImageProps.nHeight      = r.Height();
      ImageProps.nFullWidth   = r.Width();
      ImageProps.nFullHeight  = r.Height();

      ImageProps.eFormat = AXIIO_GREYSCALE_16;
      ImageProps.nMaxSampleValue = 65535;
      ImageProps.nMinSampleValue = 0;
      ImageProps.dMicronsPerPixel= pImage->GetNmPerPixel() ? pImage->GetNmPerPixel()/1E3 : 10.0;

      AXU_sprintf(ImageProps.szComment, ELEMENTS_IN(ImageProps.szComment), "Channel %d", i);

      if (!AXIIO_WriteImage(hImageIO, &ImageProps, pImage->GetPtr(), nImageBytes, &nError))
         bOK = false;

      AXIIO_Close(hImageIO);
   }

#endif
   return bOK;
}

//==============================================================================================
// FUNCTION: SaveRaw
// PURPOSE:  Saves the current image to a tiff file.
//
bool CLuciferDlg::SaveRaw(LPCSTR pszFilename)
{
   UINT uFirst = 0;
   UINT uCount = m_Images.GetCount();

   if (uCount == 0)
      return true;

   // If the shift key is down, only save the last channel if it is channel 8 (FIFO test).
   if (GetKeyState(VK_SHIFT) < 0)
   {
      UINT uLast  = uCount-1;
      if (strcmp(m_Images.GetImage(uLast)->GetName(), "8") == 0)
      {
         uFirst = uLast;
         uCount = 1;
      }
   }

   for (UINT i=0; i<uCount; i++)
   {
      CImage *pImage  = m_Images.GetImage(i + uFirst);

      CFilePath Path(pszFilename);

      char sz[256];
      AXU_sprintf(sz, ELEMENTS_IN(sz), "%s_%s", Path.GetFileName(), pImage->GetName());
      Path.SetFileName(sz);

      CFileIO File;
      if (!File.Create(Path, false))
         return false;

      File.Write(pImage->GetPtr(), pImage->GetPixelCount() * sizeof(WORD));
   }
   return true;
}

//===============================================================================================
// FUNCTION: OnDestroy
// PURPOSE:  .
//
void CLuciferDlg::OnDestroy( HWND hwnd )
{
   MEMBERASSERT();
   
   SaveToRegistry();

   if (m_pDevice)
   {
      if (m_Timer.IsRunning())
         Stop();
      m_Driver.CloseDevice(m_pDevice);
      m_Driver.UnloadDriver();
      m_pDevice = NULL;
   }

   float fContrast   = m_ImageScope.GetContrast();
   float fBrightness = m_ImageScope.GetBrightness();

   AXU_WritePrivateProfileFloat(USE_STR(Lucifer), USE_STR(Brightness), fBrightness, c_szAppName);
   AXU_WritePrivateProfileFloat(USE_STR(Lucifer), USE_STR(Contrast),   fContrast,   c_szAppName);
}

//===============================================================================================
// FUNCTION: Redraw
// PURPOSE:  .
//
BOOL CLuciferDlg::Redraw()
{
   MEMBERASSERT();
   m_ImageScope.Redraw();
   return true;
}
//===============================================================================================
// FUNCTION: OpenDevice
// PURPOSE:  Opens the device.
//
bool CLuciferDlg::OpenDevice()
{
   CRunAnimation Run(this);

   LPCSTR pszDriver = GetDriverName(m_eDeviceType);

   if (!m_Driver.LoadDriver(pszDriver))
   {
      char sz[256] = "";
      AXU_sprintf(sz, ELEMENTS_IN(sz), "Could not load driver %s.", pszDriver);
      MessageBox(sz, "Error", MB_OK);
      return false;
   }

   UINT uOpenFlags = 0;
   if (m_uFlags & CAM_FLAG_DEMO)
      uOpenFlags |= CAM_FLAG_DEMO;

   int nError = 0;
   m_pDevice = m_Driver.OpenDevice(m_uSerial, uOpenFlags, &nError);
   if (!m_pDevice)
      return ShowDeviceError(nError);

   if (!m_pDevice->GetDeviceInfo(&m_DeviceInfo))
      return ShowDeviceError();

   SetDlgItemText(m_hWnd, IDC_NAME,     m_DeviceInfo.szName);
   SetDlgItemText(m_hWnd, IDC_VERSION,  m_DeviceInfo.szDriverVersion);
   SetDlgItemText(m_hWnd, IDC_FIRMWARE, m_DeviceInfo.szFirmwareVersion);
   SetDlgItemUINT(IDC_SERIAL, m_DeviceInfo.uSerialNumber);

//   if (!m_pDevice->GetProtocol(&m_Protocol))
//      return ShowDeviceError();

   return true;
}

//===============================================================================================
// FUNCTION: ValidateSettings
// PURPOSE:  .
//
bool CLuciferDlg::ValidateSettings()
{
   MEMBERASSERT();

   UINT ubmCCD = m_DeviceInfo.ubmAvailableASP;
   //VERIFY(m_pDevice->GetAvailableCCD(ubmCCD));
   
   if ((m_Settings.ubmCCD & ~ubmCCD) )
   {
      if (MessageBox("CCD have been selected that are not currently available.\n\n"
                     "Edit settings now?", "Error", MB_OKCANCEL)==IDOK)
         EditSettings();
      
      return false;
   }
   return true;
}

//===============================================================================================
// FUNCTION: EditSettings
// PURPOSE:  .
//
void CLuciferDlg::EditSettings()
{
   MEMBERASSERT();
   if (!m_pDevice)
      return;

   m_Settings.wSaturation = m_ImageScope.GetSaturationColor();
   CDlgSettings Dlg(m_hWnd, m_pDevice, &m_Settings);

   if (Dlg.CreateModal() == IDCANCEL)
      return;

   m_ImageScope.SetSaturationColor(65535);//m_Settings.wSaturation);

   SaveToRegistry();

   SetupFrame();
}

//===============================================================================================
// FUNCTION: SetupFrame
// PURPOSE:  Sets up the protocol structure for a snap.
//
bool CLuciferDlg::SetupFrame()
{
   if (!m_pDevice || m_Timer.IsRunning())
      return false;

   SaveToRegistry();

   // update mode and session from settings 
   CAM_Mode    m_Mode;
   CAM_Session m_Session;

   m_Mode.uReadModeNum    = m_Settings.uReadMode;    //Readout mode 1...CAM_MODES
   m_Mode.ubmCCD          = m_Settings.ubmCCD;       //CCD sensors to use (bitmap) : only 1 CCD
   m_Mode.uBinningX       = m_Settings.uBinningX; 
   m_Mode.uBinningY       = m_Settings.uBinningY; 
   m_Mode.uPrescanCol     = m_Settings.uPrescanCol;  //Number of prescan columns
   m_Mode.eFlushRow       = m_Settings.eFlushRow;    //Skip first row of upper half (if required) 
   m_Mode.bFlipY          = m_Settings.bFlipY;

   //Regions of interest  
   if (m_Settings.bROI1) {
      m_Mode.rRegion[0].left    = m_Settings.ptMin1.x;
      m_Mode.rRegion[0].top     = m_Settings.ptMin1.y;  
      m_Mode.rRegion[0].right   = m_Settings.ptMax1.x;
      m_Mode.rRegion[0].bottom  = m_Settings.ptMax1.y;  
   }
   if (m_Settings.bROI2) {
      m_Mode.rRegion[1].left    = m_Settings.ptMin2.x;
      m_Mode.rRegion[1].top     = m_Settings.ptMin2.y;  
      m_Mode.rRegion[1].right   = m_Settings.ptMax2.x;
      m_Mode.rRegion[1].bottom  = m_Settings.ptMax2.y;  
   }
   if (m_Settings.bROI3) {
      m_Mode.rRegion[2].left    = m_Settings.ptMin3.x;
      m_Mode.rRegion[2].top     = m_Settings.ptMin3.y;  
      m_Mode.rRegion[2].right   = m_Settings.ptMax3.x;
      m_Mode.rRegion[2].bottom  = m_Settings.ptMax3.y;  
   }
   
   m_Session.uExposureMS           = m_Settings.uExposureMS;
   m_Session.uShutterCloseMS       = m_Settings.uDelayMS;
   m_Session.bGain4x               = ((int)m_Settings.aeGain[0] == (int)CAM_Gain4x)? TRUE:FALSE;

   ///////////////////////////////////
   // Set camera mode
   if (!m_pDevice->SetMode(&m_Mode))
      return ShowDeviceError();
      
   ///////////////////////////////////
   // Setup scope

   // Get frame from active mode info
   CAM_ModeInfo Mode;
   m_pDevice->GetMode (&Mode);

   char szBmQuad[8] = {0};
   AXU_sprintf( szBmQuad, ELEMENTS_IN(szBmQuad), "%04X", Mode.ubmActQuad);
   m_ModeParams.SetFormatItem(0, (int)Mode.uModeNum); //%0
   m_ModeParams.SetFormatItem(1, szBmQuad);           //[0x%1]
   m_ModeParams.SetFormatItem(2, (int)Mode.uWidth);   //%2
   m_ModeParams.SetFormatItem(3, (int)Mode.uHeight);  //%3

   // Frame size
   CRect rImage(0, 0, 0, 0);
   rImage.right  = Mode.uWidth;
   rImage.bottom = Mode.uHeight;
   m_rFrame      = rImage;
   m_uActiveChannel %= CAM_DISPLAYS;
  
   TRACE3("Set image channels: %d, [%d x %d]\n", 
         CAM_DISPLAYS, m_rFrame.Size().cx, m_rFrame.Size().cy );
      /*RECT r = {0};
   r.left = 0; r.top = 0;
   r.right  = Mode.uWidth; r.bottom = Mode.uHeight;*/
   if (!m_Images.SetProps(m_aszChannels, ELEMENTS_IN(m_aszChannels), c_nNmPerPixel, &m_rFrame))
   {
      MessageBox("Insufficient memory available!", "Error", MB_OK);
      return false;
   }
   m_ImageScope.SetImages(&m_Images, m_rFrame); 
   SetChannel(0);


   ///////////////////////////////////
   // Set camera session parameters
   ///////////////////////////////////
   for (UINT i=0; i<CAM_DISPLAYS; i++)
   {
      m_Session.pImages[i]    = m_Images.GetImage(i)->GetPtr();
      m_Session.uPixels[i]    = m_Images.GetImage(i)->GetPixelCount();
   }

   // Set camera timing
   CAM_Timing* pTiming = &m_Session.Timing;
   pTiming->uReadNum   = m_Settings.uReadNum;
   for (int i=0; i<ELEMENTS_IN(pTiming->adSerTimeUS); i++)
      pTiming->adSerTimeUS[i] = m_Settings.adSerTimeUS[i];

   for (int i=0; i<ELEMENTS_IN(pTiming->adParTimeUS); i++)
      pTiming->adParTimeUS[i] = m_Settings.adParTimeUS[i];
   // TDI time [ms]
   pTiming->dParTimeTdiMS = m_Settings.adParTimeUS[eCAM_TTDI];
   
   // Set ASP parameters: offsets, gain, resetBlank
   CAM_AspParams* pAsp = &m_Session.AspParams;
   for (int i=0; i<ELEMENTS_IN(pAsp->anOffsDN); i++)
      pAsp->anOffsDN[i] = m_Settings.anAspOffset[0][i]; //CCD#1
   // Gain
   pAsp->eGain = m_Settings.aeGain[0]; //CCD#1
   // ResetBlank   
   pAsp->bResetBlank = m_Settings.abResetBlank[0];

   // Set camera session
   if (!m_pDevice->SetSession(&m_Session))
      return ShowDeviceError();

   return true;
}

//===============================================================================================
// FUNCTION: Snap
// PURPOSE:  Snap a single image.
//
bool CLuciferDlg::Snap()
{
   m_Images.Clear();

   m_ImageScope.SetImages(&m_Images, m_rFrame); 
   SetChannel(0);
  
   // Snap a frame
   if (!m_pDevice->Snap()) {
      Stop(m_pDevice->GetLastError());
      return false;
   }
   m_bAcquiring = true;

   //SetPercent(0);
   StartTimer();
   return true;
}

//===============================================================================================
// FUNCTION: Stop
// PURPOSE:  Stop the current action.
//
void CLuciferDlg::Stop(int nError)
{
   if (!m_pDevice || !m_Timer.IsRunning())   
      return;

   m_bAcquiring = false;

   m_Timer.Stop();
   m_pDevice->Stop();
   StopAnimation();

   //SetPercent(100);
   if (nError)
      ShowDeviceError(nError);
}

//===============================================================================================
// FUNCTION: OnSnap
// PURPOSE:  Start/stop a single frame acqusition.
//
void CLuciferDlg::OnSnap() 
{
   if (!m_pDevice)
   {
      MessageBeep(MB_ICONERROR);
      return;
   }

   if (!m_Timer.IsRunning())
      Snap();  // start
   else
      Stop();
}

//===============================================================================================
// FUNCTION: OnUsbCmdDlg
// PURPOSE:  Open non-modal dialog with low-level USB device controls
//
void CLuciferDlg::OnUsbDevDlg() 
{
   if (!m_pDevice)
   {
      AXODBG_printf("NO USB DEVICE FOUND\n");
      MessageBeep(MB_ICONERROR);
      return;
   }

   // open control dialog
   if (m_pUsbDevDlg)
	{
      BOOL bWasShown	=	::IsWindow(m_pUsbDevDlg->m_hWnd) && m_pUsbDevDlg->IsWindowVisible();

		m_pUsbDevDlg->DestroyWindow();
		delete m_pUsbDevDlg; 	m_pUsbDevDlg	=	NULL;

		if (bWasShown)
		   return;
	}
   // create new modeless dialog
	m_pUsbDevDlg	=	new CUSB2DevDlg(m_hWnd, m_pDevice);
   VERIFY( m_pUsbDevDlg->CreateModeless());

   // Display USB device dialog
   m_pUsbDevDlg->ShowWindow(SW_SHOW);
}


/*
//===============================================================================================
// FUNCTION: OnDeviceReset
// PURPOSE:  .
//
void CLuciferDlg::OnDeviceReset()
{
   MEMBERASSERT();
   if (!m_pDevice)
   {
      MessageBeep(MB_ICONERROR);
      return;
   }

   if ( !m_pDevice->Reset() )
   {
      ShowDeviceError();
      return;
   }
}
*/
//===============================================================================================
// FUNCTION: ShowLines
// PURPOSE:  .
//
void CLuciferDlg::ShowLines(UINT uLines, bool bUpdateScope)
{
   MEMBERASSERT();
   
   if (bUpdateScope && !m_bQuiet)
      m_ImageScope.ShowLines(uLines); 
}

//===============================================================================================
// FUNCTION: OnMovie
// PURPOSE:  .
//
void CLuciferDlg::OnMovie()
{
   MEMBERASSERT();
   m_Images.Clear();
}

//===============================================================================================
// FUNCTION: OnTimer
// PURPOSE:  .
//
void CLuciferDlg::OnTimer()
{
   if (m_bInTimerMsg)
      return;

   m_bInTimerMsg = true;

   if (m_bAcquiring)
   {
      CAM_Status Status;
      if (!m_pDevice->GetStatus(Status))
         Stop(m_pDevice->GetLastError());
      else 
      {   
         if (Status.eState != eCAM_ACQUIRING)
         {
            int nError = 0;
            if (Status.eState == eCAM_ERROR) 
               nError = m_pDevice->GetLastError();
            Stop();

            if (!nError) {
               ShowLines(m_rFrame.Size().cy, true); // DONE OK
            }
         }
      }
      m_Timer.Acknowledge();
   }

   m_bInTimerMsg = false;
}

//===============================================================================================
// FUNCTION: OnLeftClick
// PURPOSE:  .
//
void CLuciferDlg::OnLeftClick()
{
   MEMBERASSERT();
   
   //if ((GetKeyState(VK_MENU)<0) && m_ROIMode)
   //{
   //   m_bIgnoreScopeMsg = true;
   //   m_ImageScope.StartZoom();
   //   m_bIgnoreScopeMsg = false;
   //   m_ROIMode = 0;
   //   return;
   //}
}

//===============================================================================================
// FUNCTION: GetMousePos
// PURPOSE:  Get the current mouse position in image coordinates.
// RETURNS:  TRUE if it is inside the image.
//
BOOL CLuciferDlg::GetMousePos(POINT &pt)
{
   MEMBERASSERT();

   GetCursorPos(&pt);
   return m_ImageScope.ScreenToImage(&pt);
}

void CLuciferDlg::OnMouseMove()
{
   CPoint pt;
   if (!GetMousePos(pt))
   {
      m_CursorPos.SetFormatItem(0, "");
      m_CursorPos.SetFormatItem(1, "");
      m_CursorPos.SetFormatItem(2, "");
      return;
   }

   CImage *pImage = m_Images.GetImage(m_uActiveChannel);

   if (GetKeyState(VK_SHIFT) < 0)
   {
      // show in [um]
      m_CursorPos.SetFormatItem(0, pt.x * c_nNmPerPixel/1000);
      m_CursorPos.SetFormatItem(1, pt.y * c_nNmPerPixel/1000);
   }
   else
   {
      m_CursorPos.SetFormatItem(0, pt.x+1);
      m_CursorPos.SetFormatItem(1, pt.y+1);
   }
   m_CursorPos.SetFormatItem(2, int(pImage->GetDataValue(pt)));
}

//===============================================================================================
// FUNCTION: OnSelectionCompleted
// PURPOSE:  Processes the completion of a rubber-band selection.
//
void CLuciferDlg::OnSelectionCompleted(IMAGESCOPE_AREANOTIFY *pAreaNotify)
{
   MEMBERASSERT();

   // Set the minimum image size to 4x4 pixels.
   static POINT ptMinImage  = { 3, 3 };

   // Set the minimum zoom region to 6x6 display pixels.
   static POINT ptMinClient = { 5, 5 };

   pAreaNotify->ptMinimumImage  = ptMinImage;
   pAreaNotify->ptMinimumClient = ptMinClient;

   CRect rZoom = pAreaNotify->rClientArea;
   BOOL bAreaSelected = (rZoom.Width()  > pAreaNotify->ptMinimumClient.x) &&
                        (rZoom.Height() > pAreaNotify->ptMinimumClient.y);
   bAreaSelected = bAreaSelected; // remove compiler warning.

   //if (m_ROIMode && bAreaSelected)
   //{
   //   static CRect s_rNewScanRegion;
   //   s_rNewScanRegion = pAreaNotify->rImageArea;
   //   ::PostMessage(GetDialogHandle(), WM_NEWSCANREGION, 0, LPARAM(&s_rNewScanRegion));
   //}
}

//===============================================================================================
// FUNCTION: On2Byte
// PURPOSE:  .
//
void CLuciferDlg::On2Byte()
{
   MEMBERASSERT();
   bool bLowBitOnly = (GetKeyState(VK_SHIFT) < 0);
   BYTE byMask      = BYTE(bLowBitOnly ? 0x01 : 0xFF);
   
   COpenFileDlg Dlg(m_hWnd, m_szPath);
   if (!Dlg.CreateModal())
      return;

   AXU_STRNCPYZ(m_szPath, Dlg.GetFile());
   CFileIO Srce, Dest;
   if (!Srce.Create(m_szPath, true))
      return;

   CFilePath Path(m_szPath);
   Path.SetExtension(bLowBitOnly ? "b0" : "2b");
   if (!Dest.Create(Path, false))
      return;

   BYTE aby[2048];
   WORD aw[2048];
   for (;;)
   {
      DWORD dwRead = 0;
      Srce.Read(aby, sizeof(aby), &dwRead);

      if (!dwRead)
         break;

      for (UINT i=0; i<dwRead; i++)
         aw[i] = WORD(aby[i] & byMask);

      Dest.Write(aw, dwRead*sizeof(WORD));
   }
}

//===============================================================================================
// FUNCTION: OnClose
// PURPOSE:  .
//
bool CLuciferDlg::OnClose()
{
   MEMBERASSERT();
   SaveToRegistry();
   
   DestroyWindow();     // N.B. modeless dialog's only!
   PostQuitMessage(0);
   return true;
}

//===============================================================================================
// FUNCTION: Notification
// PURPOSE:  Responds to notifications from the controls on the window.
//
void CLuciferDlg::Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
{
   MEMBERASSERT();

   if (pWnd == &m_Contrast)
      OnContrastChange(nNotificationCode);
   else if (pWnd == &m_Brightness)
      OnBrightnessChange(nNotificationCode);
   else if (uID == IDC_SAVE_TIFF)
      OnSaveTiff();
   else if (uID == IDC_SAVE_RAW)
      OnSaveRaw();
   else if (pWnd == &m_Snap)
      OnSnap();
   else if (uID == IDC_STOP)
      Stop();
   //else if (uID == IDC_2BYTE)
   //   On2Byte();
   else if (uID == IDC_USBDEV)
      OnUsbDevDlg();
   else if (uID == IDC_EDIT_SETTINGS)
      EditSettings();
   else if (uID == IDC_CHANNEL_SPINNER)
   {
      SetChannel(nNotificationCode == ADM_NOTIFY_GAUGE_LINEUP ? -1 : 
                 nNotificationCode == ADM_NOTIFY_GAUGE_LINEDOWN ? 1 : 0);
   }

   // Send the notifications back to the parent for handling
   CLuciferDlg_BASE::Notification(pWnd, uID, nNotificationCode, lData);
}

//======================================================================================
// METHOD:  OnScopeMessage
// PURPOSE: Process notification messages from the scope.
//
void CLuciferDlg::OnScopeMessage(UINT uMsg, LPARAM lParam)
{
   MEMBERASSERT();

   // Ignore if poorly set up, or if the ignore flag is set.
   if (!m_hWnd || m_bIgnoreScopeMsg)
      return;

   // Fix up the oddball messages that do not pass the scope HWND in the lParam
   HWND                           hwndScope     = NULL;
   IMAGESCOPE_AREANOTIFY         *pAreaNotify   = NULL;
   IMAGESCOPE_REGIONCREATE       *pRegionCreate = NULL;
   IMAGESCOPE_DISPLAYPROP_NOTIFY *pDisplayProp  = NULL;
   IMAGESCOPE_KEYPRESSED         *pKeyPressed   = NULL;

   switch (uMsg)
   {
      case ISCN_AREA_MOUSE_ZOOM_COMPLETED:
      case ISCN_AREA_MOUSE_SELECTION_COMPLETED:
         pAreaNotify = (IMAGESCOPE_AREANOTIFY *)lParam;
         hwndScope   = pAreaNotify->hScopeWnd;
         break;

      case ISCN_CLICK_LMOUSE_REGION:
      case ISCN_DOUBLECLICK_REGION:
      case ISCN_DOUBLECLICK_RMOUSE_REGION:
      case ISCN_CLICK_RMOUSE_REGION:
      case ISCN_REGION_SELECTED:
      case ISCN_REGION_DESELECTED:
      case ISCN_REGION_CREATED:
         pRegionCreate = (IMAGESCOPE_REGIONCREATE *)lParam;
         hwndScope     = pRegionCreate->hScopeWnd;
         break;

      case ISCN_DISPLAYPROP_CHANGE:
         pDisplayProp = (IMAGESCOPE_DISPLAYPROP_NOTIFY *)lParam;
         hwndScope    = pDisplayProp->hScopeWnd;
         break;

      case ISCN_KEYDOWN:
      case ISCN_KEYUP:
         pKeyPressed = (IMAGESCOPE_KEYPRESSED *)lParam;
         hwndScope   = pKeyPressed->hScopeWnd;
         break;

      default:
         hwndScope = HWND(lParam);
         break;
   }

   // Default message return -- 0  (scope does its processing)
   SetWindowLong(m_hWnd, DWL_MSGRESULT, FALSE);

   // TRACE_ScopeMessage(uMsg);
   switch (uMsg)
   {
      // user changed zoom
      case ISCN_ZOOM_CHANGE:
         break;

      // Undo zoom list updated
      case ISCN_UNDO_ZOOM_CHANGE:
         break;

      case ISCN_MODE_CHANGE:
         break;

      // Color look-up table has changed
      case ISCN_CLUT_CHANGE:
         break;

      case ISCN_REGIONUPDATING:
         break;
      
      // region(s) was changed by scope.
      case ISCN_REGIONUPDATE:
         break;
      
      // scope has gained the focus
      case ISCN_SETFOCUS:
         ::SendMessage(hwndScope, WM_QUERYNEWPALETTE, 0, 0);
         break;

      // scope has lost the focus
      case ISCN_KILLFOCUS:
         break;

      // user double clicked with right mouse button on a region
      case ISCN_DOUBLECLICK_RMOUSE_REGION:
         // Return TRUE to prevent the scope putting up its right mouse menu.
//       SetWindowLong(m_hWnd, DWL_MSGRESULT, TRUE);
         break;

      // user clicked with right mouse button on a region
      case ISCN_CLICK_RMOUSE_REGION: // lParam - scope wnd which was clicked in
         // Return TRUE to prevent the scope putting up its right mouse menu.
//       SetWindowLong(m_hWnd, DWL_MSGRESULT, TRUE);

         // Post a message to provoke the right mouse menu so as to avoid re-entrance in ImageScope.
//       PostMessage(m_hWnd, WM_GP_RIGHTMOUSE, 0, REGION_MENU);
         break;

      // user clicked with right mouse button
      case ISCN_CLICK_RMOUSE:        // lParam - scope wnd which was clicked in
         // Return TRUE to prevent the scope putting up its right mouse menu.
//       SetWindowLong(m_hWnd, DWL_MSGRESULT, TRUE);

         // Post a message to provoke the right mouse menu so as to avoid re-entrance in ImageScope.
//       PostMessage(m_hWnd, WM_GP_RIGHTMOUSE, 0, NOREGION_MENU);
         break;

      case ISCN_MOUSE_RELEASED:
         break;

      // user clicked with left mouse button on a region
      case ISCN_CLICK_LMOUSE_REGION: // lParam - scope wnd which was clicked in
         //if (m_ROIMode && (GetKeyState(VK_CONTROL)<0))
         //{
         //   SetWindowLong(m_hWnd, DWL_MSGRESULT, TRUE);
         //   return;
         //}
         OnLeftClick();
         break;

      // user clicked with left mouse button
      case ISCN_CLICK_LMOUSE:        // lParam - scope wnd which was clicked in
         OnLeftClick();
         break;

      // user let the mouse up
      case ISCN_CLICK_LMOUSE_UP:
         break;

      // user double clicked in scope inside a region, that region is now selected
      case ISCN_DOUBLECLICK_REGION:  // lParam - scope wnd which was clicked in
         break;

      // user double clicked in scope  
      case ISCN_DOUBLECLICK:         // lParam - scope wnd which was clicked in
         break;

      // user has moved mouse over scope 
      case ISCN_MOUSEMOVE:           // lParam - scope wnd where mouse has moved over
         OnMouseMove();
         break;

      // user just finished drawing a zoom rectangle or a selection rectangle, lParam is a ptr to IMAGESCOPE_AREANOTIFY struct
      case ISCN_AREA_MOUSE_ZOOM_COMPLETED:
         break;

      // user just finished drawing a lasso selection, lParam is a ptr to IMAGESCOPE_AREANOTIFY struct
      case ISCN_AREA_MOUSE_SELECTION_COMPLETED:
         OnSelectionCompleted(pAreaNotify);
         break;
   }
}

//===============================================================================================
// FUNCTION: ProcessEvent
// PURPOSE:  Low level message hook.
//
BOOL CLuciferDlg::ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   MEMBERASSERT();

   static UINT WM_IMAGESCOPE = RegisterWindowMessage(IMAGESCOPE_MESSAGE_STR);

   if (uMsg == WM_IMAGESCOPE)
   {
      OnScopeMessage(wParam, lParam);
      return true;
   }

   switch (uMsg)
   {
      case WM_SIZE:
         OnSize();
         break;

      case WM_CLOSE:
         OnClose();
         return TRUE;

      case WM_PAINT:
         if (!m_bInit)
            ::PostMessage(m_hWnd, WM_OPEN_DEVICE, 0, 0);
         m_bInit = true;
         break;

      case WM_OPEN_DEVICE:
      {
         bool bOK = OpenDevice();
         bOK     &= SetupFrame();
         if (!bOK)
            SetDefaultImages();
         break;
      }
      case WM_NEWSCANREGION:
         NewScanRegion(LPCRECT(lParam));
         break;

      case WM_TIMER:
         OnTimer();
         break;
   }
   
   return CLuciferDlg_BASE::ProcessEvent( hWnd, uMsg, wParam, lParam );
}

//===============================================================================================
// FUNCTION: StartAnimation
// PURPOSE:  .
//
void CLuciferDlg::StartAnimation(int nAnimation)
{
   MEMBERASSERT();
   ::ShowWindow(GetDlgItem(m_hWnd, IDC_WAITMSG), SW_SHOWNORMAL);
   m_Animation.Open(nAnimation);
   m_Animation.ShowWindow(SW_SHOWNORMAL);
   m_Animation.Play(0, UINT(-1), UINT(-1));

   for (UINT i=0; i<ELEMENTS_IN(EnableList); i++)
      ::EnableWindow(GetDlgItem(m_hWnd, EnableList[i]), false);
}

//===============================================================================================
// FUNCTION: StopAnimation
// PURPOSE:  .
//
void CLuciferDlg::StopAnimation()
{
   MEMBERASSERT();
   m_Animation.ShowWindow(SW_HIDE);
   ::ShowWindow(GetDlgItem(m_hWnd, IDC_WAITMSG), SW_HIDE);
	m_Animation.Stop();
	m_Animation.Seek(0);

   for (UINT i=0; i<ELEMENTS_IN(EnableList); i++)
      ::EnableWindow(GetDlgItem(m_hWnd, EnableList[i]), true);
}

//===============================================================================================
// FUNCTION: StartTimer
// PURPOSE:  .
//
void CLuciferDlg::StartTimer()
{
   MEMBERASSERT();
   m_bInTimerMsg = false;
   StartAnimation(IDR_BUSY);
   VERIFY(m_Timer.Start(m_hWnd, c_uTimerID, c_uTimerPeriod, false));
}

//===============================================================================================
// FUNCTION: OnCtlColor
// PURPOSE:  .
//
HBRUSH CLuciferDlg::OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int nType)
{
   MEMBERASSERT();
   if (hwnd == m_Animation.m_hWnd)
   {
      COLORREF rgb = GetSysColor(COLOR_BTNFACE);
      SetBkColor(hdc, rgb);
      return ::CreateSolidBrush(rgb);
   }
   return CLuciferDlg_BASE::OnCtlColor(hwnd, hdc, hwndChild, nType);
}
