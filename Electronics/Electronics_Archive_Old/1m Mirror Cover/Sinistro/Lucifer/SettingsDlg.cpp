//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  CDlgControl.CPP
// PURPOSE: 
// AUTHOR:  Vasiliy Popov
//

#include "wincpp.hpp"
#include "SettingsDlg.hpp"
#include "resource.h"

const char c_szAppName[] = "Lucifer";
const char c_szExt[]     = "scs";      //settings file extension
const int  c_nSaturation = 65535;      //scope saturation level

DECLARE_STR(Lucifer);
DECLARE_STR(ProtocolPath);
DECLARE_STR(Saturation);
DECLARE_STR(AutosavePath);
DECLARE_STR(Autosave);
DECLARE_STR(Video);
DECLARE_STR(ReadMode);
DECLARE_STR(TDI);
DECLARE_STR(ExposureMS);
DECLARE_STR(DelayMS);
DECLARE_STR(Loops);
DECLARE_STR(BinningX);
DECLARE_STR(BinningY);
DECLARE_STR(ROI1);
DECLARE_STR(ROI2);
DECLARE_STR(ROI3);
DECLARE_STR(ptMin1); 
DECLARE_STR(ptMax1); 
DECLARE_STR(ptMin2); 
DECLARE_STR(ptMax2); 
DECLARE_STR(ptMin3); 
DECLARE_STR(ptMax3); 
DECLARE_STR(AspNum);   
DECLARE_STR(bmCCD);
DECLARE_STR(ClkNum);
DECLARE_STR(PrescanCol);
DECLARE_STR(FlushRow);
DECLARE_STR(FlipY);

// ASP _STR
const char c_szGain[]         = "Gain";    
const char c_szFactorJJ[]     = "FactorJJ";
const char c_szResetBlank[]   = "ResetBlank";
const char c_szAspOffset[]    = "AspOffset"; 
// CLK _STR
const char c_szClkBias[]      = "ClkBias"; 
const char c_szClkBiasOn[]    = "ClkBiasOn"; 
const char c_szClkPhaseH[]    = "ClkPhaseH"; 
const char c_szClkPhaseL[]    = "ClkPhaseL"; 
// TIMING _STR
DECLARE_STR(AdcReadNum);
const char* c_szSerial[]      = {"ts1","ts2","ts3","ts4","ts5","ts6","ts7","tSG","tADC"}; //[9]
const char* c_szParallel[]    = {"tp1","tp2","tp3","tp4","tp5","tp6","tp7","tMPP","tTDI"};//[9]

////////////////////////////////////////////////////////////
// Defaults
////////////////////////////////////////////////////////////
// General tab
const BOOL c_bAutoSaveDef     = false;
const BOOL c_bVideoDef        = false; 
const UINT c_uExposureDef     = 1000; //ms
const UINT c_uDelayMSDef      = 500; //ms
const UINT c_uLoopsDef        = 1; 
const UINT c_uBinningXDef     = 1; 
const UINT c_uBinningYDef     = 1; 
const BOOL c_bROI1Def         = true;
const BOOL c_bROI2Def         = false;
const BOOL c_bROI3Def         = false;
const POINT c_ptPointMinDef   = { 1,1 }; 
const POINT c_ptPointMaxDef   = { CCD486_WIDTH, CCD486_HEIGHT };
const UINT  c_nFlushRowDef    = ( CCD486_WIDTH != CCD486_HEIGHT)? (int)CAM_Flush : (int)CAM_NoFlush; 
const UINT  c_uPrescanColDef  = 0;

//ASP tab defaults
const UINT c_uReadModeDef     = 1;
const int c_nGainDef          = (int)CAM_Gain1x;
const UINT c_uFactorJJDef     = 2;
const BOOL c_bResetBlankDef   = false; 
const int c_nAspOffsetDef     = 0; //dn
//CLK tab defaults
const float c_afClkBias [eCLKBIAS_NUM]      = {24.0f, 17.0f, -2.5f,+15.0f};
const float c_afClkPhaseHigh[eCLKPHASE_NUM] = {+5.0f, +3.0f, +4.5f,+10.0f, +4.5f, +5.0f};
const float c_afClkPhaseLow [eCLKPHASE_NUM] = {-5.0f, -8.0f, -6.0f,  0.0f, -6.0f, -5.0f};
//Timing defaults
const UINT  c_uReadNum        = 8; // [2...256], even only
const float c_afSerDef[eSTIME_NUM]     = {1.0f, 1.0f, 1.0f, 1.0f,
                                         1.0f, 1.0f, 1.0f, 
                                         0.03f, //tsg 
                                         0.3f,  //tadc
                                         };
const float c_afParDef[ePTIME_NUM] = {25.0f, 25.0f, 25.0f, 25.0f,
                                         25.0f, 25.0f, 25.0f, 5.0f,  //tmpp
                                         0.0f   //ttdi
                                         };
// Range of controls
const UINT c_uBinningMin         =  1;
const UINT c_uBinningMax         =  8;
const UINT c_uExposureMinMS      =  0;     //don't open shutter?
const UINT c_uExposureMaxMS      =  65535; 
const UINT c_uShutterDelayMinMS  =  0;
const UINT c_uShutterDelayMaxMS  =  4096; 
const UINT c_uLoopsMin           =  1;
const UINT c_uLoopsMax           =  16384;

// Timing Range
const float c_fSerTimeQuantaUS   =  0.01f;   //[us] 10ns
const float c_fSerTimeMinUS      =  0.01f;   //[us] 10ns
const float c_fSerTimeMaxUS      =  81.92f;  //[us] 
const float c_fSerTimeAdcMinUS   =  0.25f;   //[us] 250ns
const float c_fSerTimeAdcMaxUS   =  2.56f;   //[us] 

const float c_fTimeTDI_QuantaMS  =  0.1f;   //[ms]   
const float c_fTimeTDI_MinMS     =  0.0f;   //[ms] 
const float c_fTimeTDI_MaxMS     =  409.5f; //[ms] 409.5 ms

const float c_fParTimeQuantaUS   =  0.1f;    //[us] 100ns
const float c_fParTimeMinUS      =  0.1f;    //[us] 
const float c_fParTimeMaxUS      =  819.2f;  //[us] 

// # ADC reads per pixel
const UINT c_uAdcReadNumQuanta  =  2;
const UINT c_uAdcReadNumMin     =  2;
const UINT c_uAdcReadNumMax     =  256;

//###############################################################################################
// commment out if you don't use a global hInstance in your project
extern HINSTANCE g_hInstance;


const UINT c_uAspTimerID     = 668;
const UINT c_uClkTimerID     = 669;
const UINT c_uAspTimerPeriod = 500;
const UINT c_uClkTimerPeriod = 500;

//===============================================================================================
// FUNCTION: ShowDeviceError
// PURPOSE:  Utility function for error handling.
//
HWND s_hwndParent = NULL;

static bool ShowDeviceError(const CAM_Device* pDevice, int nError=0)
{
   if (!nError && pDevice)
      nError = pDevice->GetLastError();
   if (!nError)
      return false;
   if (!s_hwndParent)
      return false;
   char sz[256] = "";
   pDevice->GetLastErrorText(sz, ELEMENTS_IN(sz));
   MessageBox( s_hwndParent, sz, "Error", MB_OK);
   return false;
}


LuciferSettings::LuciferSettings() 
{
   ////////////////////////////////////////////
   // Initialize Lucifier settings to defaults
   ////////////////////////////////////////////

   memset(this, 0, sizeof(*this)); 

   ubmCCD = 0x001;   //use CCD#1 (bitmap)
   wSaturation = c_nSaturation; //scope saturation level 65535

   // general
   uReadMode   = c_uReadModeDef;                     //ASP readout mode 1-19
   bTDI        = false;
   uExposureMS = c_uExposureDef;
   uDelayMS    = c_uDelayMSDef;
   uLoops      = c_uLoopsDef;
   uBinningX   = c_uBinningXDef;
   uBinningY   = c_uBinningYDef;
   bROI1       = c_bROI1Def;
   bROI2       = c_bROI2Def;
   bROI3       = c_bROI3Def;
   
   ptMin1.x    = 1;         ptMin1.y = 2; 
   ptMax1.x    = CCD486_WIDTH; ptMax1.y = CCD486_HEIGHT;
   
   ptMin2      = c_ptPointMinDef; ptMax2 = c_ptPointMaxDef; 
   ptMin3      = c_ptPointMinDef; ptMax3 = c_ptPointMaxDef; 
   bAutosave   = c_bAutoSaveDef;
   bVideo      = c_bVideoDef;
   eFlushRow   = (CAM_FlushRow)c_nFlushRowDef;
   bFlipY      = FALSE;   
   uPrescanCol = c_uPrescanColDef;  //#prescan columns
   // asp
   ubmCCD   = 0;                                      //Bit flags for enabled CDD  
   uAspNum  = 1;    // last edited ASP  
   for (int i=0; i<CAM_MAX_CCD; i++)
   {
      aeGain[i]       = (CAM_Gain)c_nGainDef;               //ASP gain 0=1x, 1=4x
      auFactorJJ[i]   = c_uFactorJJDef;                     //ASP filtering JJ 1-32
      abResetBlank[i] = c_bResetBlankDef;                   //reset blanking: 0=off, 1=on (same as RG)
      for (int j=0; j<CAM_MAX_CHANNELS;j++)
         anAspOffset[i][j] = c_nAspOffsetDef;  //ASP channel offsets +/-[DN]
   }
   // clk
   uClkNum = 1; 
   for (int i=0; i<CAM_MAX_CCD; i++)
   {
      bClkBiasOn[CAM_MAX_CCD] = false;

      for (int j=0; j<(int)eCLKBIAS_NUM;j++)
         afClkBias [i][j] = 0.0f; 
      
      for (int j=0; j<(int)eCLKPHASE_NUM;j++)
      {
         afClkPhaseHigh[i][j]; 
         afClkPhaseLow [i][j];
      }
   }
   //timing
   uReadNum = c_uReadNum;
   for (int i=0; i<ELEMENTS_IN(adSerTimeUS);i++)
      adSerTimeUS[i] = c_afSerDef[i];
   for (int i=0; i<ELEMENTS_IN(adParTimeUS);i++)
      adParTimeUS[i] = c_afParDef[i];
   
   AXU_GetDefaultDataDirectory(szAutosavePath, ELEMENTS_IN(szAutosavePath));
}

void LuciferSettings::Load(LPCSTR pszPath)
{
   BOOL bUseINI = AXU_UseINIFile(pszPath!=NULL);
   if (!pszPath)
      pszPath = c_szAppName;

   char szDefault[_MAX_PATH] = "";
   AXU_GetDefaultDataDirectory(szDefault, ELEMENTS_IN(szDefault));

   // general tab
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(ReadMode),      c_uReadModeDef,   &uReadMode, pszPath);
   AXU_GetPrivateProfileBOOL(USE_STR(Lucifer),  USE_STR(TDI),           false,            &bTDI,     pszPath );
   AXU_GetPrivateProfileBOOL(USE_STR(Lucifer),  USE_STR(Video),         c_bVideoDef,      &bVideo,    pszPath);
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(ExposureMS),    c_uExposureDef,   &uExposureMS, pszPath);
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(DelayMS),       c_uDelayMSDef,    &uDelayMS,  pszPath);
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(Loops),         c_uLoopsDef,      &uLoops,    pszPath );
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(BinningX),      c_uBinningXDef,   &uBinningX, pszPath );
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(BinningY),      c_uBinningYDef,   &uBinningY, pszPath );
   AXU_GetPrivateProfileBOOL(USE_STR(Lucifer),  USE_STR(ROI1),          c_bROI1Def,       &bROI1,     pszPath );
   AXU_GetPrivateProfileBOOL(USE_STR(Lucifer),  USE_STR(ROI2),          c_bROI2Def,       &bROI2,     pszPath );
   AXU_GetPrivateProfileBOOL(USE_STR(Lucifer),  USE_STR(ROI3),          c_bROI3Def,       &bROI3,     pszPath );
   AXU_GetPrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMin1),        &c_ptPointMinDef, &ptMin1,    pszPath ); 
   AXU_GetPrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMin2),        &c_ptPointMinDef, &ptMin2,    pszPath ); 
   AXU_GetPrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMin3),        &c_ptPointMinDef, &ptMin3,    pszPath ); 
   AXU_GetPrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMax1),        &c_ptPointMaxDef, &ptMax1,    pszPath ); 
   AXU_GetPrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMax2),        &c_ptPointMaxDef, &ptMax2,    pszPath ); 
   AXU_GetPrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMax3),        &c_ptPointMaxDef, &ptMax3,    pszPath ); 
   AXU_GetPrivateProfileBOOL(USE_STR(Lucifer),  USE_STR(Autosave),      c_bAutoSaveDef,   &bAutosave, pszPath);
   AXU_GetPrivateProfileString(USE_STR(Lucifer),USE_STR(AutosavePath),  szDefault,        szAutosavePath, ELEMENTS_IN(szAutosavePath), pszPath);
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(PrescanCol),    c_uPrescanColDef, &uPrescanCol, pszPath);
   int nFlush;
   AXU_GetPrivateProfileInt(USE_STR(Lucifer),   USE_STR(FlushRow),      c_nFlushRowDef,   &nFlush, pszPath);
   eFlushRow = (CAM_FlushRow)nFlush;
   AXU_GetPrivateProfileBOOL(USE_STR(Lucifer),  USE_STR(FlipY),      FALSE,   &bFlipY, pszPath);

   //ASP
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(AspNum),    1,   &uAspNum, pszPath); //last ASP
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(bmCCD),   0x0F,  &ubmCCD, pszPath );

   char szTag[32];
   for (int nCCD=0; nCCD<CAM_MAX_CCD; nCCD++)
   {
      AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d", c_szGain, nCCD+1);
      int nGain;
      AXU_GetPrivateProfileInt(USE_STR(Lucifer),  szTag, c_nGainDef, &nGain, pszPath);
      aeGain[nCCD] = (CAM_Gain)nGain;

      AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d", c_szFactorJJ, nCCD+1);
      AXU_GetPrivateProfileUINT(USE_STR(Lucifer), szTag, c_uFactorJJDef, &auFactorJJ[nCCD], pszPath);

      AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d", c_szResetBlank, nCCD+1);
      AXU_GetPrivateProfileBOOL(USE_STR(Lucifer),  szTag,  c_bResetBlankDef, &abResetBlank[nCCD], pszPath);
      
      for (int i=1; i<=CAM_MAX_CHANNELS; i++) 
      {
         AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d_%d", c_szGain, nCCD+1, i+1);
         AXU_GetPrivateProfileInt(USE_STR(Lucifer),  szTag, c_nAspOffsetDef, &anAspOffset[nCCD][i], pszPath);
      }
   }
   //CLK
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(ClkNum),    1,   &uClkNum, pszPath); //last CLK
   for (int i=0; i<CAM_MAX_CCD; i++)
   {
      AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d", c_szClkBiasOn, i+1);
      AXU_GetPrivateProfileBOOL(USE_STR(Lucifer),  szTag,  false, &bClkBiasOn[i], pszPath);

      for (int j=0; j<(int)eCLKBIAS_NUM;j++)
      {
         AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d_%d", c_szClkBias, i+1, j+1);
         AXU_GetPrivateProfileFloat(USE_STR(Lucifer), szTag, c_afClkBias[i], &afClkBias[i][j], pszPath);
      }      
      for (int j=0; j<(int)eCLKPHASE_NUM;j++)
      {
         //afClkPhaseHigh[i][j]; 
         AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d_%d", c_szClkPhaseH, i+1, j+1);
         AXU_GetPrivateProfileFloat(USE_STR(Lucifer), szTag, c_afClkPhaseHigh[i], &afClkPhaseHigh[i][j], pszPath);
         AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d_%d", c_szClkPhaseL, i+1, j+1);
         AXU_GetPrivateProfileFloat(USE_STR(Lucifer), szTag, c_afClkPhaseLow[i], &afClkPhaseLow[i][j], pszPath);
      }
   }

   //timing
   AXU_GetPrivateProfileUINT(USE_STR(Lucifer),  USE_STR(AdcReadNum),    c_uReadNum,   &uReadNum, pszPath);
   
   for (int i=0; i<ELEMENTS_IN(adSerTimeUS);i++)
      AXU_GetPrivateProfileDouble(USE_STR(Lucifer), c_szSerial[i], c_afSerDef[i], &adSerTimeUS[i], pszPath);

   for (int i=0; i<ELEMENTS_IN(adParTimeUS);i++)
      AXU_GetPrivateProfileDouble(USE_STR(Lucifer), c_szParallel[i], c_afParDef[i], &adParTimeUS[i], pszPath);

   
   /*
   AXU_GetPrivateProfileBool(USE_STR(Lucifer), USE_STR(ZoomX2),   false, &bZoom2x, pszPath);
*/
   int nSaturation = c_nSaturation; //65535;
   AXU_GetPrivateProfileInt(USE_STR(Lucifer), USE_STR(Saturation), nSaturation, &nSaturation, pszPath);
   wSaturation = WORD(nSaturation);
     
   AXU_UseINIFile(bUseINI);
}

void LuciferSettings::Save(LPCSTR pszPath)
{
   BOOL bUseINI = AXU_UseINIFile(pszPath!=NULL);
   if (!pszPath)
      pszPath = c_szAppName;
   
   // General tab
   AXU_WritePrivateProfileBOOL(USE_STR(Lucifer), USE_STR(Video),         bVideo,    pszPath );
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer), USE_STR(ReadMode),      uReadMode, pszPath);
   AXU_WritePrivateProfileBOOL(USE_STR(Lucifer), USE_STR(TDI),           bTDI,      pszPath );
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer), USE_STR(ExposureMS),    uExposureMS, pszPath );
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer), USE_STR(DelayMS),       uDelayMS,  pszPath );
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer), USE_STR(Loops),         uLoops,    pszPath );
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer), USE_STR(BinningX),      uBinningX, pszPath );
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer), USE_STR(BinningY),      uBinningY, pszPath );
   AXU_WritePrivateProfileBOOL(USE_STR(Lucifer), USE_STR(ROI1),          bROI1,     pszPath );
   AXU_WritePrivateProfileBOOL(USE_STR(Lucifer), USE_STR(ROI2),          bROI2,     pszPath );
   AXU_WritePrivateProfileBOOL(USE_STR(Lucifer), USE_STR(ROI3),          bROI3,     pszPath );
   AXU_WritePrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMin1),        &ptMin1,    pszPath ); 
   AXU_WritePrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMin2),        &ptMin2,    pszPath ); 
   AXU_WritePrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMin3),        &ptMin3,    pszPath ); 
   AXU_WritePrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMax1),        &ptMax1,    pszPath ); 
   AXU_WritePrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMax2),        &ptMax2,    pszPath ); 
   AXU_WritePrivateProfilePoint(USE_STR(Lucifer), USE_STR(ptMax3),        &ptMax3,    pszPath ); 
   AXU_WritePrivateProfileBOOL(USE_STR(Lucifer), USE_STR(Autosave),      bAutosave, pszPath );
   AXU_WritePrivateProfileString(USE_STR(Lucifer), USE_STR(AutosavePath),  szAutosavePath, pszPath );
   AXU_WritePrivateProfileInt(USE_STR(Lucifer), USE_STR(Saturation), wSaturation, pszPath);
   AXU_WritePrivateProfileInt(USE_STR(Lucifer),   USE_STR(FlushRow), (int)eFlushRow, pszPath);
   AXU_WritePrivateProfileBOOL(USE_STR(Lucifer),  USE_STR(FlipY),     bFlipY, pszPath);
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer),  USE_STR(PrescanCol),  uPrescanCol,  pszPath);

   //ASP
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer),  USE_STR(AspNum), uAspNum, pszPath); //last ASP
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer),  USE_STR(bmCCD),  ubmCCD, pszPath );
   char szTag[32];
   for (int nCCD=0; nCCD<CAM_MAX_CCD; nCCD++)
   {
      AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d", c_szGain, nCCD+1);
      AXU_WritePrivateProfileInt(USE_STR(Lucifer),  szTag, (int)aeGain[nCCD], pszPath);

      AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d", c_szFactorJJ, nCCD+1);
      AXU_WritePrivateProfileUINT(USE_STR(Lucifer), szTag, auFactorJJ[nCCD], pszPath);

      AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d", c_szResetBlank, nCCD+1);
      AXU_WritePrivateProfileBOOL(USE_STR(Lucifer),  szTag,  abResetBlank[nCCD], pszPath);
      
      for (int i=1; i<=CAM_MAX_CHANNELS; i++) 
      {
         AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d_%d", c_szGain, nCCD+1, i+1);
         AXU_WritePrivateProfileInt(USE_STR(Lucifer),  szTag, anAspOffset[nCCD][i], pszPath);
      }
   }
   //CLK
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer),  USE_STR(ClkNum),  uClkNum, pszPath); //last CLK
   for (int i=0; i<CAM_MAX_CCD; i++)
   {
      AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d", c_szClkBiasOn, i+1);
      AXU_WritePrivateProfileBOOL(USE_STR(Lucifer),  szTag,  bClkBiasOn[i], pszPath);

      for (int j=0; j<(int)eCLKBIAS_NUM;j++)
      {
         AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d_%d", c_szClkBias, i+1, j+1);
         AXU_WritePrivateProfileFloat(USE_STR(Lucifer), szTag, afClkBias[i][j], pszPath);
      }      
      for (int j=0; j<(int)eCLKPHASE_NUM;j++)
      {
         //afClkPhaseHigh[i][j]; 
         AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d_%d", c_szClkPhaseH, i+1, j+1);
         AXU_WritePrivateProfileFloat(USE_STR(Lucifer), szTag, afClkPhaseHigh[i][j], pszPath);
         AXU_sprintf(szTag, ELEMENTS_IN(szTag), "%s_%d_%d", c_szClkPhaseL, i+1, j+1);
         AXU_WritePrivateProfileFloat(USE_STR(Lucifer), szTag, afClkPhaseLow[i][j], pszPath);
      }
   }

   //timing
   AXU_WritePrivateProfileUINT(USE_STR(Lucifer),  USE_STR(AdcReadNum), uReadNum, pszPath); 
   
   for (int i=0; i<ELEMENTS_IN(adSerTimeUS);i++)
      AXU_WritePrivateProfileDouble(USE_STR(Lucifer), c_szSerial[i], adSerTimeUS[i], pszPath);

   for (int i=0; i<ELEMENTS_IN(adParTimeUS);i++)
      AXU_WritePrivateProfileDouble(USE_STR(Lucifer), c_szParallel[i], adParTimeUS[i], pszPath);

   /*
   AXU_WritePrivateProfileBool(USE_STR(Lucifer), USE_STR(ZoomX2),        bZoom2x, pszPath);
   */

   AXU_UseINIFile(bUseINI);
}

//###############################################################################################
//###############################################################################################
//###
//###  CLASS: CTabGeneral
//###
//###############################################################################################
//###############################################################################################

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CTabGeneral::CTabGeneral(HWND hwndParent, const CAM_Device *pDevice, LuciferSettings *pSettings)
   : CTabGeneral_BASE(g_hInstance, MAKEINTRESOURCE(IDD_GENERAL_TAB), "General")
{
   MEMBERASSERT();
   m_pDevice   = pDevice;
   m_pSettings = pSettings;
   
   s_hwndParent = hwndParent;

   VERIFY(m_pDevice->GetDeviceInfo(&m_DeviceInfo));

   // Readout mode
   m_ReadMode.RegisterControl(this,   IDC_RMODE,   IDC_SPINNER_RMODE);
   m_ReadMode.SetRange(1, CAM_MODES);
   // TDI
   RegisterControl(IDC_TDI,      &m_TDI);
   // Flush 1st row
   UINT auList[] = { IDC_FLUSH_NONE, IDC_FLUSH };
   m_FlushRow.SetRadioList(this, ELEMENTS_IN(auList), auList);
   //m_FlushRow.EnableWindow(0, TRUE);
   m_FlushRow.EnableWindow(1, TRUE);
   
   // Flipped in Y-dir
   RegisterControl(IDC_FLIPY,   &m_FlipY);

   // # Prescan columns
   m_PrescanCol.RegisterControl(this,   IDC_PRESCAN,   IDC_SPINNER_PRESCAN);
   m_PrescanCol.SetRange(0, CCD486_PRESCAN);  

   // Binning
   m_BinningX.RegisterControl(this,   IDC_BINX,   IDC_SPINNER_BINX);
   m_BinningY.RegisterControl(this,   IDC_BINY,   IDC_SPINNER_BINY);
   m_BinningX.SetRange(c_uBinningMin, c_uBinningMax);
   m_BinningY.SetRange(c_uBinningMin, c_uBinningMax);
   
   // Exposure
   m_ExposureMS.RegisterControl(this, IDC_EXPOSURE, IDC_SPINNER_EXPOSURE);
   m_ExposureMS.SetRange(c_uExposureMinMS, c_uExposureMaxMS);
   
   // Shutter delay
   m_DelayMS.RegisterControl(this,   IDC_DELAYMS,   IDC_SPINNER_DELAYMS);
   m_DelayMS.SetRange (c_uShutterDelayMinMS, c_uShutterDelayMaxMS);
   
   // Repeat exposure/readout process
   m_Loops.RegisterControl(this,     IDC_LOOPS,   IDC_SPINNER_LOOPS);
   m_Loops.SetRange (c_uLoopsMin, c_uLoopsMax);

   RegisterControl(IDC_ROI1,      &m_ROI1);
   m_Xmin1.RegisterControl(this,   IDC_XMIN1,   IDC_SPINNER_XMIN1);
   m_Xmax1.RegisterControl(this,   IDC_XMAX1,   IDC_SPINNER_XMAX1);
   m_Ymin1.RegisterControl(this,   IDC_YMIN1,   IDC_SPINNER_YMIN1);
   m_Ymax1.RegisterControl(this,   IDC_YMAX1,   IDC_SPINNER_YMAX1);
   //Note: optical origin is [1,1]; 
   m_Xmin1.SetRange(1, 8191);  
   m_Xmax1.SetRange(1, 8191); 
   m_Ymin1.SetRange(1, 8191); 
   m_Ymax1.SetRange(1, 8191); 
   
   RegisterControl(IDC_ROI2,      &m_ROI2); 
   m_Xmin2.RegisterControl(this,   IDC_XMIN2,   IDC_SPINNER_XMIN2);
   m_Xmax2.RegisterControl(this,   IDC_XMAX2,   IDC_SPINNER_XMAX2);
   m_Ymin2.RegisterControl(this,   IDC_YMIN2,   IDC_SPINNER_YMIN2);
   m_Ymax2.RegisterControl(this,   IDC_YMAX2,   IDC_SPINNER_YMAX2);
   //Note: optical origin is [1,1];
   m_Xmin2.SetRange(1, 8191);  
   m_Xmax2.SetRange(1, 8191); 
   m_Ymin2.SetRange(1, 8191); 
   m_Ymax2.SetRange(1, 8191); 

   RegisterControl(IDC_ROI3,      &m_ROI3);
   m_Xmin3.RegisterControl(this,   IDC_XMIN3,   IDC_SPINNER_XMIN3);
   m_Xmax3.RegisterControl(this,   IDC_XMAX3,   IDC_SPINNER_XMAX3);
   m_Ymin3.RegisterControl(this,   IDC_YMIN3,   IDC_SPINNER_YMIN3);
   m_Ymax3.RegisterControl(this,   IDC_YMAX3,   IDC_SPINNER_YMAX3);
   //Note: optical origin is [1,1];
   m_Xmin3.SetRange(1, (FW_SRAM_SIZE-1));  
   m_Xmax3.SetRange(1, (FW_SRAM_SIZE-1)); 
   m_Ymin3.SetRange(1, (FW_SRAM_SIZE-1)); 
   m_Ymax3.SetRange(1, (FW_SRAM_SIZE-1)); 
   
   // Video Mode on/off
   RegisterControl(IDC_VIDEO,    &m_Video);
   
   // Auto save images
   RegisterControl(IDC_AUTOSAVE,         &m_Autosave);
   RegisterControl(IDC_AUTOSAVE_PATH,    &m_AutosavePath);
}

void CTabGeneral::LoadDialog()
{
   m_ReadMode     = m_pSettings->uReadMode;  // CCD readout mode 1-19
   m_TDI          = m_pSettings->bTDI;       // TDI
   m_PrescanCol   = m_pSettings->uPrescanCol;
   m_FlushRow     = m_pSettings->eFlushRow;
   m_FlipY        = m_pSettings->bFlipY;
   m_ExposureMS   = m_pSettings->uExposureMS;
   m_DelayMS      = m_pSettings->uDelayMS;
   m_Loops        = m_pSettings->uLoops;
   m_BinningX     = m_pSettings->uBinningX;
   m_BinningY     = m_pSettings->uBinningY;
   m_ROI1         = m_pSettings->bROI1;
   m_Xmin1        = m_pSettings->ptMin1.x;
   m_Xmax1        = m_pSettings->ptMax1.x;
   m_Ymin1        = m_pSettings->ptMin1.y;
   m_Ymax1        = m_pSettings->ptMax1.y;
   m_ROI2         = m_pSettings->bROI2;
   m_Xmin2        = m_pSettings->ptMin2.x;
   m_Xmax2        = m_pSettings->ptMax2.x;
   m_Ymin2        = m_pSettings->ptMin2.y;
   m_Ymax2        = m_pSettings->ptMax2.y;
   m_ROI3         = m_pSettings->bROI3;
   m_Xmin3        = m_pSettings->ptMin3.x;
   m_Xmax3        = m_pSettings->ptMax3.x;
   m_Ymin3        = m_pSettings->ptMin3.y;
   m_Ymax3        = m_pSettings->ptMax3.y;
   m_Video        = m_pSettings->bVideo;
   m_Autosave     = m_pSettings->bAutosave;
   m_AutosavePath = m_pSettings->szAutosavePath;

   OnReadMode();  // update TDI 
   
   SetDirty();
}

void CTabGeneral::SaveDialog()
{
   m_pSettings->uReadMode   = m_ReadMode;  //CCD readout mode 1-19
   m_pSettings->bTDI        = m_TDI;       // TDI
   m_pSettings->uPrescanCol = m_PrescanCol;
   m_pSettings->eFlushRow   = (CAM_FlushRow)((int)m_FlushRow);
   m_pSettings->bFlipY      = m_FlipY;
   
   m_pSettings->uExposureMS = m_ExposureMS;
   m_pSettings->uDelayMS   = m_DelayMS;
   m_pSettings->uLoops     = m_Loops;
   m_pSettings->uBinningX  = m_BinningX;
   m_pSettings->uBinningY  = m_BinningY;
   m_pSettings->bROI1      = m_ROI1;
   m_pSettings->ptMin1.x   = m_Xmin1;
   m_pSettings->ptMax1.x   = m_Xmax1;
   m_pSettings->ptMin1.y   = m_Ymin1;
   m_pSettings->ptMax1.y   = m_Ymax1;
   m_pSettings->bROI2      = m_ROI2;
   m_pSettings->ptMin2.x   = m_Xmin2;
   m_pSettings->ptMax2.x   = m_Xmax2;
   m_pSettings->ptMin2.y   = m_Ymin2;
   m_pSettings->ptMax2.y   = m_Ymax2;
   m_pSettings->bROI3      = m_ROI3;
   m_pSettings->ptMin3.x   = m_Xmin3;
   m_pSettings->ptMax3.x   = m_Xmax3;
   m_pSettings->ptMin3.y   = m_Ymin3;
   m_pSettings->ptMax3.y   = m_Ymax3;
   m_pSettings->bVideo     = m_Video;
   m_pSettings->bAutosave = m_Autosave;
   AXU_STRNCPYZ(m_pSettings->szAutosavePath, m_AutosavePath);
}

//===============================================================================================
// FUNCTION: OnInitDialog
// PURPOSE:  Responds to the dialog initialization message.
//
BOOL CTabGeneral::OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam )
{
   MEMBERASSERT();

   // Pass the dialog initialization back to the parent (very important)
   BOOL bRval = CTabGeneral_BASE::OnInitDialog(hWnd, hwndFocus, lParam);

   LoadDialog();
   SetDirty();
   return bRval;
}

//===============================================================================================
// FUNCTION: OnBinning
// PURPOSE:  .
//
void CTabGeneral::OnBinning(int nNotificationCode)
{
   MEMBERASSERT();
   /*
   if (nNotificationCode == ADM_NOTIFY_GAUGE_LINEDOWN)
         m_nBinning--;
   if (nNotificationCode == ADM_NOTIFY_GAUGE_LINEUP)
         m_nBinning++;
   m_nBinning = max(1, m_nBinning);
   m_nBinning = min(8, m_nBinning);
   */
   SetDirty();
   PostUpdateEvent();
}

void CTabGeneral::OnReadMode()
{
   MEMBERASSERT();
   CAM_ReadModeDesc RM = {0};
   UINT uMode = m_ReadMode;
   if (!m_pDevice->GetReadoutCapabilities(uMode, &RM))
   {
      ShowDeviceError(m_pDevice);
      return;
   }   
   
   // enable TDI if a single quadrant is used
   bool bTDI = (RM.bmActQuad) &&
           (RM.bmActQuad == bmQ1 || RM.bmActQuad == bmQ2 ||
            RM.bmActQuad == bmQ3 || RM.bmActQuad == bmQ4 );
   if (!bTDI)
      m_TDI = false;
   
   // enable/disable TDI button  
   m_TDI.EnableWindow(bTDI);

   // change ROI max/min
   /*
   if (RM.eRtype==eHOLD) 
   {  // HOLD modes
      UINT uHeight  = max( RM.uHeigthQ12, RM.uHeigthQ34 );
      UINT y = (RM.uWidthQ34 == 0)? uHeight : 1;
      m_Ymax1.SetRange(y, y+uHeight); 
   }
   else {
      //min = optical origin is [1,1]; 
      m_Ymax1.SetRange(1, CCD486_HEIGHT); 
   }
   m_Xmax1.SetRange(1, 8192); 
   */
}

//===============================================================================================
// FUNCTION: OnSetPath
// PURPOSE:  .
//
void CTabGeneral::OnSetPath()
{
   MEMBERASSERT();

   char szPath[_MAX_PATH*2];
   if (AXU_ChooseDirectoryEx(m_hWnd, "Autosave Path", m_AutosavePath, szPath, ELEMENTS_IN(szPath)))
      m_AutosavePath = szPath;   
}

//===============================================================================================
// FUNCTION: Notification
// PURPOSE:  Responds to notifications from the controls on the window.
//
void CTabGeneral::Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
{
   MEMBERASSERT();

   if (uID == IDC_SETPATH)
      OnSetPath();
   else if (uID == IDC_SPINNER_BINX || uID == IDC_SPINNER_BINY)
      OnBinning(nNotificationCode);
   else if (uID == IDC_SPINNER_RMODE || (pWnd == &m_ReadMode))
      OnReadMode();
   // Send the notifications back to the parent for handling
   CTabGeneral_BASE::Notification(pWnd, uID, nNotificationCode, lData);
}

//===============================================================================================
// FUNCTION: UIUpdate
// PURPOSE:  .
//
void CTabGeneral::UIUpdate()
{
   MEMBERASSERT();

   BOOL bFlushEnable = FALSE;

   UINT uModeNum = m_ReadMode;
   CAM_ReadModeDesc ModeDesc;
   if (m_pDevice) {
      m_pDevice->GetReadoutCapabilities (uModeNum, &ModeDesc);   
      if (ModeDesc.uHeigthQ12 && ModeDesc.uHeigthQ34 && 
          (ModeDesc.uHeigthQ12 != ModeDesc.uHeigthQ34))
      {
         bFlushEnable = TRUE; //enable flush 1st line
      }
      else {
         m_FlushRow = 0;
      }
   }
   m_FlushRow.EnableWindow(1, bFlushEnable);

   if (m_TDI)
   {  //TDI: keep only ROI#1 enabled
      m_ROI1 = true; m_ROI2 = false; m_ROI3 = false;
   }   
   BOOL bROI1 = m_ROI1;   
   BOOL bROI2 = m_ROI2;   
   BOOL bROI3 = m_ROI3;   
   if (!bROI1 && !bROI2 && !bROI3) {
      bROI1 = TRUE; // keep atleast ROI#1 enabled
      m_ROI1 = bROI1;
   }
   m_Xmin1.EnableWindow(bROI1);
   m_Xmax1.EnableWindow(bROI1);
   m_Ymin1.EnableWindow(bROI1);
   m_Ymax1.EnableWindow(bROI1);

   m_Xmin2.EnableWindow(bROI2);
   m_Xmax2.EnableWindow(bROI2);
   m_Ymin2.EnableWindow(bROI2);
   m_Ymax2.EnableWindow(bROI2);

   m_Xmin3.EnableWindow(bROI3);
   m_Xmax3.EnableWindow(bROI3);
   m_Ymin3.EnableWindow(bROI3);
   m_Ymax3.EnableWindow(bROI3);

   m_AutosavePath.EnableWindow(m_Autosave);
   
}

//===============================================================================================
// FUNCTION: Apply
// PURPOSE:  Applies the changes in the dialog.
//
void CTabGeneral::Apply()
{
   MEMBERASSERT();
   // let base class Apply first
   CTabGeneral_BASE::Apply();

   SaveDialog();
}

//===============================================================================================
// FUNCTION: Validate
// PURPOSE:  Validates the user's values before Apply is called.
//
BOOL CTabGeneral::Validate( BOOL bSilent /*=FALSE*/, ADM_Wnd **ppBadWindow /*=NULL*/)
{
   MEMBERASSERT();

   // let base class Validate first
   if (!CTabGeneral_BASE::Validate(bSilent, ppBadWindow))
      return FALSE;

   // TODO: perform validation actions here

   // if you have an error, you should so something like this:
   /*
   if (user entered something wrong)
   {
      if ( ppBadWindow )
         *ppBadWindow = &m_controlInError;

      if ( !bSilent )
         AMD_MessageBox( GetDialogHandle(), m_hInstance, MAKEINTRESOURCE( IDS_ERROR_STRING ), MB_OK | AMD_ERROR | MB_APPLMODAL );

      return FALSE;
   }
   */

   return TRUE;
}

//###############################################################################################
//###############################################################################################
//###
//###  CLASS: CTabAsp
//###
//###############################################################################################
//###############################################################################################

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CTabAsp::CTabAsp(HWND hwndParent, CAM_Device *pDevice, LuciferSettings *pSettings)
   : CTabAsp_BASE(g_hInstance, MAKEINTRESOURCE(IDD_ASP_TAB), "ASP")
{
   MEMBERASSERT();
   m_pDevice   = pDevice;
   m_pSettings = pSettings;

   VERIFY(m_pDevice->GetDeviceInfo(&m_DeviceInfo));
   // get bitmap of available ASP
   m_ubmAvailableCCD = m_DeviceInfo.ubmAvailableASP;
   
   //ASP#
   m_uAspNum = 1; //default#
   m_AspNum.RegisterControl(this,   IDC_ASPNUM,   IDC_SPINNER_ASPNUM);
   m_AspNum.SetRange( 1, CAM_MAX_CCD);
   
   //Enabled CCD
   UINT auBoards[] = { IDC_ASP1, IDC_ASP2, IDC_ASP3, IDC_ASP4 };
   m_CCDs.SetCheckList( this, auBoards, ELEMENTS_IN(auBoards) );
   m_CCDs.EnableWindow (m_ubmAvailableCCD);

   // Gain
   UINT auList[] = { IDC_GAIN_X1, IDC_GAIN_X4 };
   m_Gain.SetRadioList(this, ELEMENTS_IN(auList), auList);
   
   // Digital Filter (JJ)
   m_FactorJJ.RegisterControl(this,   IDC_JJ,   IDC_SPINNER_JJ);
   m_FactorJJ.SetRange(1, 32);
   
   // Reset blanking
   RegisterControl(IDC_RESET_BLANK,      &m_ResetBlank);
   
   // Adjust Analog offsets (DN)
   m_Offset[0].RegisterControl(this,   IDC_ADJ1,   IDC_SPINNER_ADJ1);
   m_Offset[1].RegisterControl(this,   IDC_ADJ2,   IDC_SPINNER_ADJ2);
   m_Offset[2].RegisterControl(this,   IDC_ADJ3,   IDC_SPINNER_ADJ3);
   m_Offset[3].RegisterControl(this,   IDC_ADJ4,   IDC_SPINNER_ADJ4);
   for (UINT i = 0; i<ELEMENTS_IN(m_Offset); i++)
      m_Offset[i].SetRange(-3316, 3316, 2); //granularity = 2
   
   // Monitor DC levels (Volt)   
   UINT uIdc[] = {IDC_MON_ADJ1, IDC_MON_ADJ2, IDC_MON_ADJ3, IDC_MON_ADJ4};
   for (UINT i=0; i<ELEMENTS_IN(m_Level); i++)
   {
      RegisterControl(uIdc[i], m_Level+i);
      m_Level[i].SetDigitsBehindDecimal(2);
   }
   // ASP temperature
   RegisterControl( IDC_ASP_TEMPERATURE , &m_Temperature);
   // Poll ASP button
   RegisterControl( IDC_POLL_ASP, &m_Poll);
}


void CTabAsp::LoadDialog(UINT uBoardNum)
{
   int nIndex  =  (uBoardNum > CAM_MAX_CCD)? 0 : (uBoardNum > 0)? (uBoardNum - 1) : 0; 
   m_AspNum    =  (UINT)nIndex+1;        //active ASP board# 

   TRACE1("Load: ASP#%d\n", nIndex+1);

   m_CCDs      =  m_pSettings->ubmCCD & m_ubmAvailableCCD;

   m_Gain      =  (int)m_pSettings->aeGain[nIndex];   //ASP gain 0=1x, 1=4x
   m_FactorJJ  =  m_pSettings->auFactorJJ[nIndex];    //ASP filtering JJ 1-32
   m_ResetBlank = m_pSettings->abResetBlank[nIndex];  //reset blanking: 0=off, 1=on (same as RG)
   for (UINT i=0; i<ELEMENTS_IN(m_Offset); i++) //ASP channel offsets +/-[DN]
      m_Offset[i] = m_pSettings->anAspOffset[nIndex][i];

   m_uAspNum = uBoardNum; //active
   
   SetDirty();
}

void CTabAsp::ShowDcLevels(bool bShowError /*true*/)
{
   CAM_AspHealth Params;
   if (!m_pDevice->MonitorAspHealth (m_uAspNum, &Params))
   {
      if (bShowError)
         ShowDeviceError(m_pDevice);
      // stop polling
      m_Poll = false;
      m_Timer.Stop();
      return;
   }   
   for (int i=0; i<ELEMENTS_IN(m_Level); i++) 
   {
      m_Level[i] = Params.afAspVolt[i];
   }
   m_Temperature.SetFormatItem(0, Params.fDegreeC, 2);
}

static int nEvenOffset(int nOffset) 
{
   if (nOffset % 2) {
      if (nOffset > 0)
         nOffset++; 
      else if (nOffset < 0)
         nOffset--;
   }
   return nOffset;
}

void CTabAsp::SaveDialog(UINT uBoardNum)
{
   int nIndex  =  (uBoardNum > CAM_MAX_CCD)? 0 : (uBoardNum > 0)? (uBoardNum - 1) : 0; 

   TRACE1("Save: ASP#%d\n", nIndex+1);

   m_pSettings->uAspNum = nIndex+1; // active 
   m_pSettings->ubmCCD  = m_CCDs & m_ubmAvailableCCD;
   
   m_pSettings->aeGain[nIndex]       = (CAM_Gain)(int)m_Gain;  //ASP gain 0=1x, 1=4x
   m_pSettings->auFactorJJ[nIndex]   = m_FactorJJ;             //ASP filtering JJ 1-32
   m_pSettings->abResetBlank[nIndex] = m_ResetBlank;           //reset blanking: 0=off, 1=on (same as RG)

   for (UINT i=0; i<ELEMENTS_IN(m_Offset); i++) {               //ASP channel offsets +/-[DN]
      int nOffset = m_Offset[i]; //Round offsets to even number (1->2)
      m_pSettings->anAspOffset[nIndex][i] = nEvenOffset(nOffset);
   }
}

//===============================================================================================
// FUNCTION: OnInitDialog
// PURPOSE:  Responds to the dialog initialization message.
//
BOOL CTabAsp::OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam )
{
   MEMBERASSERT();

   // Pass the dialog initialization back to the parent (very important)
   BOOL bRval = CTabAsp_BASE::OnInitDialog(hWnd, hwndFocus, lParam);

   LoadDialog(m_pSettings->uAspNum); //load last ASP
   SetDirty();

   return bRval;
}

//===============================================================================================
// FUNCTION: OnBoardSelect
// PURPOSE:  .
//
void CTabAsp::OnBoardSelect()
{
   MEMBERASSERT();
   UINT uCCD    = m_CCDs;
   UINT uAspNum = m_AspNum;
   
   if (uAspNum != m_uAspNum)
   {
      // save old settings before updating ones
      SaveDialog(m_uAspNum);
      // Load new settings
      LoadDialog(uAspNum);
   }
   //enable/disable the board controls
   bool bOn = (uCCD & (1<<(uAspNum-1))) != 0;
   
   for (UINT i=0; i<ELEMENTS_IN(m_Offset); i++)
      m_Offset[i].EnableWindow(bOn);
   
   m_FactorJJ.EnableWindow(bOn);
   m_Gain.EnableWindow(bOn);
   m_ResetBlank.EnableWindow(bOn);
}

//===============================================================================================
// FUNCTION: OnSet
// PURPOSE:  Set ASP voltages.
//
void CTabAsp::OnSet() 
{
   if (!m_pDevice)
   {
      MessageBeep(MB_ICONERROR);
      return;
   }
   // Set ASP parameters (analog offsets, gain, ResetBlank)
   UINT uBoardNum = m_AspNum;
   // Save current settings 
   SaveDialog(uBoardNum);

   CAM_AspParams Params;
   UINT uIndex = (uBoardNum==0)? 0 : (uBoardNum-1);

   // ASP offsets
   for (int i=0; i<ELEMENTS_IN(Params.anOffsDN); i++)
      Params.anOffsDN[i] = m_pSettings->anAspOffset[uIndex][i]; //CCD#1
   // Gain
   Params.eGain       = m_pSettings->aeGain[uIndex]; 
   // ResetBlank   
   Params.bResetBlank = m_pSettings->abResetBlank[uIndex];

   if (!m_pDevice->SetAspParameters ( uBoardNum, &Params))
   {
      ShowDeviceError(m_pDevice);
      return;
   }   
}

//===============================================================================================
// FUNCTION: OnGet
// PURPOSE:  Read ASP voltages and temperature.
//
void CTabAsp::OnGet() 
{
   if (!m_pDevice)
   {
      MessageBeep(MB_ICONERROR);
      return;
   }
   ShowDcLevels(); 
}

//===============================================================================================
// FUNCTION: Notification
// PURPOSE:  Responds to notifications from the controls on the window.
//
void CTabAsp::Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
{
   MEMBERASSERT();

   if (uID == IDC_SPINNER_ASPNUM)
      OnBoardSelect();
   else if (uID == IDC_ASP_SET_BN)
      OnSet();
   else if (uID == IDC_ASP_GET_BN)
      OnGet();
   else if (uID == IDC_POLL_ASP)
      OnPollButton();

   // Send the notifications back to the parent for handling
   CTabAsp_BASE::Notification(pWnd, uID, nNotificationCode, lData);
}

//===============================================================================================
// FUNCTION: UIUpdate
// PURPOSE:  .
//
void CTabAsp::UIUpdate()
{
   MEMBERASSERT();

   if (m_CCDs == 0) {
      // keep at least ASP#1 checked
      m_CCDs = 0x01;   //(bitmap) 
   }
}

//===============================================================================================
// FUNCTION: Apply
// PURPOSE:  Applies the changes in the dialog.
//
void CTabAsp::Apply()
{
   MEMBERASSERT();

   // let base class Apply first
   CTabAsp_BASE::Apply();
   
   UINT uBoardNum = m_AspNum;       //ASP board#
   SaveDialog(uBoardNum);


}

//===============================================================================================
// FUNCTION: Validate
// PURPOSE:  Validates the user's values before Apply is called.
//
BOOL CTabAsp::Validate( BOOL bSilent /*=FALSE*/, ADM_Wnd **ppBadWindow /*=NULL*/)
{
   MEMBERASSERT();

   // let base class Validate first
   if (!CTabAsp_BASE::Validate(bSilent, ppBadWindow))
      return FALSE;

   // TODO: perform validation actions here

   // if you have an error, you should so something like this:
   /*
   if (user entered something wrong)
   {
      if ( ppBadWindow )
         *ppBadWindow = &m_controlInError;

      if ( !bSilent )
         AMD_MessageBox( GetDialogHandle(), m_hInstance, MAKEINTRESOURCE( IDS_ERROR_STRING ), MB_OK | AMD_ERROR | MB_APPLMODAL );

      return FALSE;
   }
   */

   return TRUE;
}

//===============================================================================================
// FUNCTION: OnPollButton
// PURPOSE:  .
//
void CTabAsp::OnPollButton()
{
   if (m_Poll) 
   {
      VERIFY(m_Timer.Start( m_hWnd, c_uAspTimerID, c_uAspTimerPeriod, false));
   }
   else
   {
      m_Timer.Stop();
   }
}

//===============================================================================================
// FUNCTION: OnTimer
// PURPOSE:  .
//
void CTabAsp::OnTimer()
{
   MEMBERASSERT();
   if (m_pDevice)
      ShowDcLevels(false); //don't show error
   else {
      m_Timer.Stop();
   }
   m_Timer.Acknowledge();
}

//===============================================================================================
// FUNCTION: ProcessEvent
// PURPOSE:  Processes the messages of the dialog.
//
BOOL CTabAsp::ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   MEMBERASSERT();

   switch (uMsg)
   {
      case WM_TIMER:
         OnTimer();
         return true;
      case WM_DESTROY:
         m_Timer.Stop();
         break;
   }

   // let base class process any remaining events
   return CTabAsp_BASE::ProcessEvent(hWnd, uMsg, wParam, lParam);
}


//###############################################################################################
//###############################################################################################
//###
//###  CLASS: CTabClk
//###
//###############################################################################################
//###############################################################################################

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CTabClk::CTabClk(HWND hwndParent, CAM_Device *pDevice, LuciferSettings *pSettings)
   : CTabClk_BASE(g_hInstance, MAKEINTRESOURCE(IDD_CLK_TAB), "Clock")
{
   MEMBERASSERT();
   m_pDevice   = pDevice;
   m_pSettings = pSettings;

   VERIFY(m_pDevice->GetDeviceInfo(&m_DeviceInfo));

   //CLK#
   m_uClkNum = 1; //default#
   m_ClkNum.RegisterControl(this,   IDC_CLKNUM,   IDC_SPINNER_CLKNUM);
   m_ClkNum.SetRange(1, 4);

   // BIAS Voltages
   m_VOD.RegisterControl(this,   IDC_VOD,   IDC_SPINNER_VOD);
   m_VOD.SetRange(14.5f, 27.3f, 0.1f);
   m_VRD.RegisterControl(this,   IDC_VRD,   IDC_SPINNER_VRD);
   m_VRD.SetRange(6.0f, 18.0f, 0.1f);
   m_VOG.RegisterControl(this,   IDC_VOG,   IDC_SPINNER_VOG);
   m_VOG.SetRange(-7.0f, 5.0f, 0.1f);
   m_VDD.RegisterControl(this,   IDC_VDD,   IDC_SPINNER_VDD);
   m_VDD.SetRange(13.0f, 17.0f, 0.1f);

   // Monitor Vxx levels    
   UINT uIdc[] = {IDC_MON_VOD, IDC_MON_VRD, IDC_MON_VOG, IDC_MON_VDD};
   for (UINT i=0; i<ELEMENTS_IN(m_BiasMon); i++) {
      RegisterControl(uIdc[i], m_BiasMon+i);
      m_BiasMon[i].SetDigitsBehindDecimal(2);
   }
   // DC BIAS ON/OFF
   RegisterControl( IDC_BIAS_ON, &m_BiasON);

   // DC Phases
   UINT uPH[] = {IDC_S13H, IDC_P12H, IDC_P3H, IDC_RGH, IDC_TGH, IDC_SGH};
   UINT uPL[] = {IDC_S13L, IDC_P12L, IDC_P3L, IDC_RGL, IDC_TGL, IDC_SGL};
   UINT uSH[] = {IDC_SPINNER_S13H, IDC_SPINNER_P12H, IDC_SPINNER_P3H, IDC_SPINNER_RGH, IDC_SPINNER_TGH, IDC_SPINNER_SGH};
   UINT uSL[] = {IDC_SPINNER_S13L, IDC_SPINNER_P12L, IDC_SPINNER_P3L, IDC_SPINNER_RGL, IDC_SPINNER_TGL, IDC_SPINNER_SGL};
   UINT uMH[] = {IDC_MON_S13H, IDC_MON_P12H, IDC_MON_P3H, IDC_MON_RGH, IDC_MON_TGH, IDC_MON_SGH};
   UINT uML[] = {IDC_MON_S13L, IDC_MON_P12L, IDC_MON_P3L, IDC_MON_RGL, IDC_MON_TGL, IDC_MON_SGL};
   
   for (UINT i=0; i<ELEMENTS_IN(m_PhaseHigh); i++) 
   {
      //phases
      m_PhaseHigh[i].RegisterControl(this,   uPH[i],   uSH[i]);
      m_PhaseHigh[i].SetRange(0.0f, +10.0f, 0.1f);
      m_PhaseLow[i].RegisterControl(this,    uPL[i],   uSL[i]);
      m_PhaseLow[i].SetRange (-10.0f, 0.0f, 0.1f);
      //monitors
      RegisterControl(uMH[i], m_PhaseHighMon+i);
      m_PhaseHighMon[i].SetDigitsBehindDecimal(2);
      RegisterControl(uML[i], m_PhaseLowMon+i);
      m_PhaseLowMon[i].SetDigitsBehindDecimal(2);
   }
   // CLK temperature
   RegisterControl( IDC_CLOCK_TEMPERATURE , &m_Temperature);

   RegisterControl( IDC_POLL_CLK, &m_Poll);
}

void CTabClk::ShowClkMonitors(bool bShowError /*true*/)
{
   CAM_ClkVolt ClkVolt;
   if (!m_pDevice->MonitorClkHealth (m_uClkNum, &ClkVolt))
   {
      if (bShowError)
         ShowDeviceError(m_pDevice);
      // stop polling
      m_Poll = false;
      m_Timer.Stop();
      return;
   }   
   // show 4 BIAS voltages
   for (int i=0; i<ELEMENTS_IN(m_BiasMon); i++) 
   {
      m_BiasMon[i] = ClkVolt.afBiasVolt[i];
   }
   // Show 6 high/low phase monitors
   for (int i=0; i<ELEMENTS_IN(m_PhaseHighMon); i++) 
   {
      m_PhaseHighMon[i] = ClkVolt.afPhaseHighVolt[i];
      m_PhaseLowMon[i]  = ClkVolt.afPhaseLowVolt[i]; 
   }
   // CLK temperature
   m_Temperature.SetFormatItem(0, ClkVolt.fDegreeC, 2);
}

void CTabClk::LoadDialog(UINT uBoardNum)
{
   int nIndex  =  (uBoardNum > CAM_MAX_CCD)? 0 : (uBoardNum > 0)? (uBoardNum - 1) : 0; 
   m_ClkNum    =  (UINT)nIndex+1;        //active CLK board# 

   TRACE1("Load: CLK#%d\n", nIndex+1);
   
   m_VOD = m_pSettings->afClkBias[nIndex][eCAM_VOD]; 
   m_VRD = m_pSettings->afClkBias[nIndex][eCAM_VRD]; 
   m_VOG = m_pSettings->afClkBias[nIndex][eCAM_VOG]; 
   m_VDD = m_pSettings->afClkBias[nIndex][eCAM_VDD]; 

   for (UINT i=0; i<ELEMENTS_IN(m_PhaseHigh); i++) {
      m_PhaseHigh[i] = m_pSettings->afClkPhaseHigh[nIndex][i];
      m_PhaseLow[i]  = m_pSettings->afClkPhaseLow[nIndex][i];
   }
   m_BiasON = m_pSettings->bClkBiasOn[nIndex];   

   m_uClkNum = uBoardNum; //active

   ShowClkMonitors();
   SetDirty();
}

void CTabClk::SaveDialog(UINT uBoardNum)
{
   int nIndex  =  (uBoardNum > CAM_MAX_CCD)? 0 : (uBoardNum > 0)? (uBoardNum - 1) : 0; 

   TRACE1("Save: CLK#%d\n", nIndex+1);

   m_pSettings->uClkNum = nIndex+1; // save active 
   m_pSettings->afClkBias[nIndex][eCAM_VOD] = m_VOD; 
   m_pSettings->afClkBias[nIndex][eCAM_VRD] = m_VRD; 
   m_pSettings->afClkBias[nIndex][eCAM_VOG] = m_VOG; 
   m_pSettings->afClkBias[nIndex][eCAM_VDD] = m_VDD; 

   for (UINT i=0; i<ELEMENTS_IN(m_PhaseHigh); i++) {
      m_pSettings->afClkPhaseHigh[nIndex][i] = m_PhaseHigh[i];
      m_pSettings->afClkPhaseLow[nIndex][i]  = m_PhaseLow[i];
   }
   m_pSettings->bClkBiasOn[nIndex] = m_BiasON;   
}

//===============================================================================================
// FUNCTION: OnBoardSelect
// PURPOSE:  .
//
void CTabClk::OnBoardSelect()
{
   MEMBERASSERT();
   UINT uClkNum = m_ClkNum;
   
   if (uClkNum != m_uClkNum)
   {
      // save old settings before updating new ones
      SaveDialog(m_uClkNum);
      // Load new settings
      LoadDialog(uClkNum);
   }
}

//===============================================================================================
// FUNCTION: OnInitDialog
// PURPOSE:  Responds to the dialog initialization message.
//
BOOL CTabClk::OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam )
{
   MEMBERASSERT();

   // Pass the dialog initialization back to the parent (very important)
   BOOL bRval = CTabClk_BASE::OnInitDialog(hWnd, hwndFocus, lParam);

   LoadDialog(m_pSettings->uClkNum);
   SetDirty();
   return bRval;
}

//===============================================================================================
// FUNCTION: Notification
// PURPOSE:  Responds to notifications from the controls on the window.
//
void CTabClk::Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
{
   MEMBERASSERT();

   if (uID == IDC_SPINNER_CLKNUM)
      OnBoardSelect();
   else if (uID == IDC_CLK_GET_BN)
      OnGet();
   else if (uID == IDC_BIAS_ON)
      OnBiasOn();
   else if (uID == IDC_POLL_CLK)
      OnPollButton();
   else if (uID == IDC_CLK_SET_BN)
      OnSet();

   // Send the notifications back to the parent for handling
   CTabClk_BASE::Notification(pWnd, uID, nNotificationCode, lData);
}

//===============================================================================================
// FUNCTION: OnBiasOn
// PURPOSE:  Turn CLK BIAS voltages ON/OFF.
//
void CTabClk::OnBiasOn() 
{
   UINT uBoardNum = m_ClkNum;
   long lBiasON = (m_BiasON)? 1 : 0; //on/off
   if (!m_pDevice->WriteRegister ( (WORD)uBoardNum, CLKBIAS_ON_REG, &lBiasON ))
   {
      m_BiasON = !(m_BiasON); 
      ShowDeviceError(m_pDevice);
      return;
   }   
}

//===============================================================================================
// FUNCTION: OnSet
// PURPOSE:  DEBUG: Set CLK reference and BIAS voltages.
//
void CTabClk::OnSet() 
{
   if (!m_pDevice)
   {
      MessageBeep(MB_ICONERROR);
      return;
   }
   SaveDialog(m_ClkNum); // Save current settings

   // Set CLK Ref and Bias voltages
   UINT uBoardNum = m_ClkNum;

   // Get CLK voltages from settings
   UINT uIndex  = (uBoardNum==0)? 0 : (uBoardNum-1); //CLK board index
   CAM_ClkVolt Params;
   for (int i=0; i<ELEMENTS_IN(Params.afBiasVolt); i++)
      Params.afBiasVolt[i]      = m_pSettings->afClkBias[uIndex][i];
   for (int i=0; i<ELEMENTS_IN(Params.afPhaseHighVolt); i++)
      Params.afPhaseHighVolt[i] = m_pSettings->afClkPhaseHigh[uIndex][i];
   for (int i=0; i<ELEMENTS_IN(Params.afPhaseLowVolt); i++)
      Params.afPhaseLowVolt[i]  = m_pSettings->afClkPhaseLow[uIndex][i];
   // Set voltages
   if (!m_pDevice->SetClkParameters ( uBoardNum, &Params))
   {
      ShowDeviceError(m_pDevice);
      return;
   }   
}

//===============================================================================================
// FUNCTION: OnGet
// PURPOSE:  Read CLK voltages and temperature.
//
void CTabClk::OnGet() 
{
   if (!m_pDevice)
   {
      MessageBeep(MB_ICONERROR);
      return;
   }
   ShowClkMonitors();
}

//===============================================================================================
// FUNCTION: UIUpdate
// PURPOSE:  .
//
void CTabClk::UIUpdate()
{
   MEMBERASSERT();
   m_BiasON.SetWindowText((m_BiasON) ? "ON" : "OFF");
}

//===============================================================================================
// FUNCTION: Apply
// PURPOSE:  Applies the changes in the dialog.
//
void CTabClk::Apply()
{
   MEMBERASSERT();

   // let base class Apply first
   CTabClk_BASE::Apply();

   UINT uBoardNum = m_ClkNum;       //CLK board#
   SaveDialog(uBoardNum);
}

//===============================================================================================
// FUNCTION: Validate
// PURPOSE:  Validates the user's values before Apply is called.
//
BOOL CTabClk::Validate( BOOL bSilent /*=FALSE*/, ADM_Wnd **ppBadWindow /*=NULL*/)
{
   MEMBERASSERT();

   // let base class Validate first
   if (!CTabClk_BASE::Validate(bSilent, ppBadWindow))
      return FALSE;

   return TRUE;
}

//===============================================================================================
// FUNCTION: OnPollButton
// PURPOSE:  .
//
void CTabClk::OnPollButton()
{
   if (m_Poll) 
   {
      VERIFY(m_Timer.Start( m_hWnd, c_uClkTimerID, c_uClkTimerPeriod, false));
   }
   else
   {
      m_Timer.Stop();
   }
}

//===============================================================================================
// FUNCTION: OnTimer
// PURPOSE:  .
//
void CTabClk::OnTimer()
{
   MEMBERASSERT();
   if (m_pDevice)
      ShowClkMonitors(false); //don't show errors
   else 
      m_Timer.Stop();
   
   m_Timer.Acknowledge();
}

//===============================================================================================
// FUNCTION: ProcessEvent
// PURPOSE:  Processes the messages of the dialog.
//
BOOL CTabClk::ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   MEMBERASSERT();

   switch (uMsg)
   {
      case WM_TIMER:
         OnTimer();
         return true;
      case WM_DESTROY:
         m_Timer.Stop();
         break;
   }

   // let base class process any remaining events
   return CTabAsp_BASE::ProcessEvent(hWnd, uMsg, wParam, lParam);
}

//###############################################################################################
//###############################################################################################
//###
//###  CLASS: CTabTiming
//###
//###############################################################################################
//###############################################################################################

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CTabTiming::CTabTiming(HWND hwndParent, const CAM_Device *pDevice, LuciferSettings *pSettings)
   : CTabTiming_BASE(g_hInstance, MAKEINTRESOURCE(IDD_CLK_TIMING_TAB), "Timing")
{
   MEMBERASSERT();
   m_pDevice   = pDevice;
   m_pSettings = pSettings;
   
   // Serial timing
   UINT auSed[] = {IDC_TS1, IDC_TS2, IDC_TS3, IDC_TS4,
                   IDC_TS5, IDC_TS6, IDC_TS7, IDC_TSG,
                   IDC_TADC };
   UINT auSsp[] = {IDC_SPINNER_TS1, IDC_SPINNER_TS2, IDC_SPINNER_TS3, IDC_SPINNER_TS4,
                   IDC_SPINNER_TS5, IDC_SPINNER_TS6, IDC_SPINNER_TS7, IDC_SPINNER_TSG,
                   IDC_SPINNER_TADC };
   for (int i=0; i<ELEMENTS_IN(m_SerTimeUS);i++)
   {
      m_SerTimeUS[i].RegisterControl(this,   auSed[i],   auSsp[i]);
      m_SerTimeUS[i].SetDigitsBehindDecimal(2); // 0.01us
      if (i == (int)eCAM_TADC)
         m_SerTimeUS[i].SetRange(c_fSerTimeAdcMinUS, c_fSerTimeAdcMaxUS, c_fSerTimeQuantaUS); //tADC
      else { // other serial timing
         m_SerTimeUS[i].SetRange(c_fSerTimeMinUS, c_fSerTimeMaxUS, c_fSerTimeQuantaUS);
      }
   }
   
   //# ADC reads
   m_ReadNum.RegisterControl(this,   IDC_ADCREADS,   IDC_SPINNER_ADCREADS);
   m_ReadNum.SetRange(c_uAdcReadNumMin, c_uAdcReadNumMax, c_uAdcReadNumQuanta);

   // Parallel timing
   UINT auPed[] = {IDC_TP1, IDC_TP2, IDC_TP3, IDC_TP4,
                   IDC_TP5, IDC_TP6, IDC_TP7, IDC_TMPP,
                   IDC_TTDI};
   UINT auPsp[] = {IDC_SPINNER_TP1, IDC_SPINNER_TP2, IDC_SPINNER_TP3, IDC_SPINNER_TP4,
                   IDC_SPINNER_TP5, IDC_SPINNER_TP6, IDC_SPINNER_TP7, IDC_SPINNER_TMPP,
                   IDC_SPINNER_TTDI };
   for (int i=0; i<ELEMENTS_IN(m_ParTimeUS);i++)
   {
      m_ParTimeUS[i].RegisterControl(this,   auPed[i],   auPsp[i]);
      m_ParTimeUS[i].SetDigitsBehindDecimal(1); // 0.1us
      if (i == (int)eCAM_TTDI) {
         // Note: store TDI time in [MS]
         m_ParTimeUS[i].SetRange(c_fTimeTDI_MinMS, c_fTimeTDI_MaxMS, c_fTimeTDI_QuantaMS);
      }
      else {
         // all parallel timing except tTDI [ms]
         m_ParTimeUS[i].SetRange(c_fParTimeMinUS, c_fParTimeMaxUS, c_fParTimeQuantaUS);   
      }
   }
}

void CTabTiming::LoadDialog()
{
   for (UINT i=0; i<ELEMENTS_IN(m_SerTimeUS); i++) {
      m_SerTimeUS[i] = m_pSettings->adSerTimeUS[i];
   }
   for (UINT i=0; i<ELEMENTS_IN(m_ParTimeUS); i++) {
      m_ParTimeUS[i] = m_pSettings->adParTimeUS[i];
   }
   m_ReadNum = m_pSettings->uReadNum;
   SetDirty();
}

void CTabTiming::SaveDialog()
{
   for (UINT i=0; i<ELEMENTS_IN(m_SerTimeUS); i++) {
      m_pSettings->adSerTimeUS[i] = m_SerTimeUS[i];
   }
   for (UINT i=0; i<ELEMENTS_IN(m_ParTimeUS); i++) {
      m_pSettings->adParTimeUS[i] = m_ParTimeUS[i];
   }
   UINT uReads = m_ReadNum;
   if (uReads % 2) 
      uReads++;
   m_pSettings->uReadNum = uReads; //only even numbers
}

//===============================================================================================
// FUNCTION: OnInitDialog
// PURPOSE:  Responds to the dialog initialization message.
//
BOOL CTabTiming::OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam )
{
   MEMBERASSERT();

   // Pass the dialog initialization back to the parent (very important)
   BOOL bRval = CTabTiming_BASE::OnInitDialog(hWnd, hwndFocus, lParam);

   LoadDialog();
   SetDirty();
   return bRval;
}
//===============================================================================================
// FUNCTION: Notification
// PURPOSE:  Responds to notifications from the controls on the window.
//
void CTabTiming::Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
{
   MEMBERASSERT();

   /*if (pWnd == &m_Channels)
      OnChannelsSelect();
   else if (pWnd == &m_Lasers)
      OnLaserSelect();
   */
   // Send the notifications back to the parent for handling
   CTabTiming_BASE::Notification(pWnd, uID, nNotificationCode, lData);
}

//===============================================================================================
// FUNCTION: UIUpdate
// PURPOSE:  .
//
void CTabTiming::UIUpdate()
{
   MEMBERASSERT();
}

//===============================================================================================
// FUNCTION: Apply
// PURPOSE:  Applies the changes in the dialog.
//
void CTabTiming::Apply()
{
   MEMBERASSERT();

   // let base class Apply first
   CTabTiming_BASE::Apply();

   SaveDialog();
}

//===============================================================================================
// FUNCTION: Validate
// PURPOSE:  Validates the user's values before Apply is called.
//
BOOL CTabTiming::Validate( BOOL bSilent /*=FALSE*/, ADM_Wnd **ppBadWindow /*=NULL*/)
{
   MEMBERASSERT();

   // let base class Validate first
   if (!CTabTiming_BASE::Validate(bSilent, ppBadWindow))
      return FALSE;

   // TODO: perform validation actions here

   // if you have an error, you should so something like this:
   /*
   if (user entered something wrong)
   {
      if ( ppBadWindow )
         *ppBadWindow = &m_controlInError;

      if ( !bSilent )
         AMD_MessageBox( GetDialogHandle(), m_hInstance, MAKEINTRESOURCE( IDS_ERROR_STRING ), MB_OK | AMD_ERROR | MB_APPLMODAL );

      return FALSE;
   }
   */

   return TRUE;
}

//###############################################################################################
//###############################################################################################
//###
//###  CLASS: CTabPower
//###
//###############################################################################################
//###############################################################################################

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CTabPower::CTabPower(HWND hwndParent, const CAM_Device *pDevice, LuciferSettings *pSettings)
   : CTabPower_BASE(g_hInstance, MAKEINTRESOURCE(IDD_POWER_TAB), "Power")
{
   MEMBERASSERT();
   m_pDevice   = pDevice;
   m_pSettings = pSettings;
}

void CTabPower::LoadDialog()
{
   SetDirty();
}

//===============================================================================================
// FUNCTION: OnInitDialog
// PURPOSE:  Responds to the dialog initialization message.
//
BOOL CTabPower::OnInitDialog( HWND hWnd, HWND hwndFocus, LPARAM lParam )
{
   MEMBERASSERT();

   // Pass the dialog initialization back to the parent (very important)
   BOOL bRval = CTabPower_BASE::OnInitDialog(hWnd, hwndFocus, lParam);

   LoadDialog();
   SetDirty();
   return bRval;
}
//===============================================================================================
// FUNCTION: Notification
// PURPOSE:  Responds to notifications from the controls on the window.
//
void CTabPower::Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
{
   MEMBERASSERT();

   // Send the notifications back to the parent for handling
   CTabPower_BASE::Notification(pWnd, uID, nNotificationCode, lData);
}

//===============================================================================================
// FUNCTION: UIUpdate
// PURPOSE:  .
//
void CTabPower::UIUpdate()
{
   MEMBERASSERT();
}

//===============================================================================================
// FUNCTION: Apply
// PURPOSE:  Applies the changes in the dialog.
//
void CTabPower::Apply()
{
   MEMBERASSERT();

   // let base class Apply first
   CTabPower_BASE::Apply();

   /*m_pSettings->ubmLasers      = m_Lasers;
   m_pSettings->ubmChannels    = m_Channels;
   m_pSettings->bSequentialScan= (m_SequentialScan!=0);

   for (UINT i=0; i<ELEMENTS_IN(m_Power); i++)
      m_pSettings->abyLaserPower[i] = BYTE(Round(m_Power[i]));
   for (i=0; i<ELEMENTS_IN(m_PMT); i++)
   {
      m_pSettings->afPMTVoltage[i] = m_PMT[i];
      m_pSettings->anSeqOffset[i] = Round(m_Offset[i]*1E3);
   }*/
}

//===============================================================================================
// FUNCTION: Validate
// PURPOSE:  Validates the user's values before Apply is called.
//
BOOL CTabPower::Validate( BOOL bSilent /*=FALSE*/, ADM_Wnd **ppBadWindow /*=NULL*/)
{
   MEMBERASSERT();

   // let base class Validate first
   if (!CTabPower_BASE::Validate(bSilent, ppBadWindow))
      return FALSE;

   // TODO: perform validation actions here

   // if you have an error, you should so something like this:
   /*
   if (user entered something wrong)
   {
      if ( ppBadWindow )
         *ppBadWindow = &m_controlInError;

      if ( !bSilent )
         AMD_MessageBox( GetDialogHandle(), m_hInstance, MAKEINTRESOURCE( IDS_ERROR_STRING ), MB_OK | AMD_ERROR | MB_APPLMODAL );

      return FALSE;
   }
   */

   return TRUE;
}

//###############################################################################################
//###############################################################################################
//###
//###  CLASS: CDlgSettings
//###
//###############################################################################################
//###############################################################################################

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CDlgSettings::CDlgSettings(HWND hwndParent, CAM_Device *pDevice, LuciferSettings *pSettings )
   : CDlgSettings_BASE( g_hInstance, MAKEINTRESOURCE(IDD_SETTINGS), hwndParent, IDC_TABCONTROL )
     ,m_TabGeneral(hwndParent, pDevice, pSettings)
     ,m_TabAsp(hwndParent, pDevice, pSettings)
     ,m_TabClk(hwndParent, pDevice, pSettings)
     ,m_TabTiming(hwndParent, pDevice, pSettings)
     ,m_TabPower(hwndParent, pDevice, pSettings)
{
   MEMBERASSERT();

   m_pSettings = pSettings;

   CModulePath Path;
   Path.SetExtension(c_szExt);
   AXU_STRNCPYZ(m_szProtocolPath, Path);
   AXU_GetPrivateProfileString(USE_STR(Lucifer), USE_STR(ProtocolPath), m_szProtocolPath, m_szProtocolPath, ELEMENTS_IN(m_szProtocolPath), c_szAppName);

   //m_pSettings->Load(m_szProtocolPath);

   AddSubDialog( &m_TabGeneral );
   AddSubDialog( &m_TabAsp );
   AddSubDialog( &m_TabClk );
   AddSubDialog( &m_TabTiming );
   AddSubDialog( &m_TabPower );
}

//===============================================================================================
// FUNCTION: OnInitDialog
// PURPOSE:  .
//
BOOL CDlgSettings::OnInitDialog( HWND hWnd, HWND hWndFocus, LPARAM lParam )
{
   MEMBERASSERT();
   
   // Pass the dialog initialization back to the parent (very important)
   BOOL bRval = CDlgSettings_BASE::OnInitDialog(hWnd, hWndFocus, lParam);

   return bRval;
}

//===============================================================================================
// FUNCTION: Notification
// PURPOSE:  Responds to notifications from the controls on the window.
//
void CDlgSettings::Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
{
   MEMBERASSERT();
   if (uID == IDC_LOAD)
      OnLoad();
   else if (uID == IDC_SAVE)
      OnSave();
   else if (uID == IDC_INITIALIZE)
      OnInitialize();

   // Send the notifications back to the parent for handling
   CDlgSettings_BASE::Notification(pWnd, uID, nNotificationCode, lData);
}

//===============================================================================================
// FUNCTION: UIUpdate
// PURPOSE:  .
//
void CDlgSettings::UIUpdate()
{
   MEMBERASSERT();
   CDlgSettings_BASE::UIUpdate();

}

//===============================================================================================
// FUNCTION: Apply
// PURPOSE:  .
//
void CDlgSettings::Apply()
{
   MEMBERASSERT();
   CDlgSettings_BASE::Apply();
}

//##############################################################################################
// CLASS:   CSettingsFileDlg
// PURPOSE: Common file dialog for loading/saving settings.
//
class CSettingsFileDlg : public ADM_FileDialog
{
private:
   CSettingsFileDlg( const CSettingsFileDlg &);
   const CSettingsFileDlg &operator=( const CSettingsFileDlg &);

public:
   CSettingsFileDlg(HWND hwndParent, LPCSTR pszPath, bool bSave)
      : ADM_FileDialog(hwndParent)
   {
      SetDialogType( bSave ? eSave : eOpen );

      // Change the title of the parent dialog.
      SetTitle("Save Settings");
      char sz[256] = "";
      AXU_sprintf(sz, ELEMENTS_IN(sz), "Sinistro Camera Settings Files (*.%s)|*.%s|All Files (*.*)|*.*||", 
         c_szExt, c_szExt);
      SetFilter(sz);

      // Set the required flags.
      UINT uFlags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
      if (bSave)
         uFlags |= OFN_OVERWRITEPROMPT;
      else
         uFlags |= OFN_FILEMUSTEXIST;

      SetFlags( uFlags );

      // Set the initial directory.
      SetInitialDir(pszPath);
   }
};

//===============================================================================================
// FUNCTION: OnLoad
// PURPOSE:  .
//
void CDlgSettings::OnLoad()
{
   MEMBERASSERT();

   CSettingsFileDlg Dlg(m_hWnd, m_szProtocolPath, false);
   if (!Dlg.CreateModal())
      return;

   AXU_STRNCPYZ(m_szProtocolPath, Dlg.GetFile());
   m_pSettings->Load(m_szProtocolPath);
   AXU_WritePrivateProfileString(USE_STR(Lucifer), USE_STR(ProtocolPath), m_szProtocolPath, c_szAppName);
  
   m_TabGeneral.LoadDialog();
   m_TabAsp.LoadDialog(m_pSettings->uAspNum);
   m_TabClk.LoadDialog(m_pSettings->uClkNum);
   m_TabTiming.LoadDialog();
   m_TabPower.LoadDialog();
   
}

//===============================================================================================
// FUNCTION: OnSave
// PURPOSE:  .
//
void CDlgSettings::OnSave()
{
   MEMBERASSERT();
   
   CSettingsFileDlg Dlg(m_hWnd, m_szProtocolPath, true);
   if (!Dlg.CreateModal())
      return;
   
  // Apply();

   AXU_STRNCPYZ(m_szProtocolPath, Dlg.GetFile());
   m_pSettings->Save(m_szProtocolPath);
   AXU_WritePrivateProfileString(USE_STR(Lucifer), USE_STR(ProtocolPath), m_szProtocolPath, c_szAppName);
}

//===============================================================================================
// FUNCTION: OnInitialize
// PURPOSE:  .
//
void CDlgSettings::OnInitialize()
{
   MEMBERASSERT();
  
  *m_pSettings = LuciferSettings();
   
  // load default settings
   m_TabGeneral.LoadDialog();
   m_TabAsp.LoadDialog(m_pSettings->uAspNum);
   m_TabClk.LoadDialog(m_pSettings->uClkNum);
   m_TabTiming.LoadDialog();
   m_TabPower.LoadDialog();
}

