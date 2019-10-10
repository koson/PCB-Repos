//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  ImageScope.CPP
// PURPOSE: 
// AUTHOR:  BHI  Oct 1998
//

#include "wincpp.hpp"
#include "ImageScope.hpp"

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CImageScope::CImageScope()
{
   MEMBERASSERT();
   m_hwndScope   = NULL;
   m_nLastError  = 0;
   m_wSaturation = 65535;
}

//===============================================================================================
// FUNCTION: Destructor
// PURPOSE:  Object cleanup.
//
CImageScope::~CImageScope()
{
   MEMBERASSERT();
   if (IsWindow(m_hwndScope))
      DestroyWindow(m_hwndScope);
   m_hwndScope = NULL;
}

//===============================================================================================
// FUNCTION: Create
// PURPOSE:  Create the scope.
//
HWND CImageScope::Create(HWND hwndParent, int nControlID)
{
   MEMBERASSERT();
   ASSERT(m_hwndScope == NULL);

   // Create the scope.
   DWORD dwStyle  = 0;
   RECT *prWindow = NULL;

   m_hwndScope = IMAGESCOPE_CreateImageScope(hwndParent, dwStyle, prWindow, 10, &m_DataSource);
   if (!m_hwndScope)
      return FALSE;
   AXU_ReplaceControl(hwndParent, nControlID, m_hwndScope);

   IMAGESCOPE_PROPS isProps;
   if (!IMAGESCOPE_GetProps(m_hwndScope, &isProps, &m_nLastError))
      return FALSE;
         
   isProps.uFlags |=  IMAGESCOPE_FLAGS_NODRAWOPTIMIZE;  // Do not optimize display if the scaling is integral.

   AXLIST_GetRectRegionName(isProps.cRegionName, sizeof(isProps.cRegionName));

   isProps.uFlags |=  IMAGESCOPE_FLAGS_VCENTER;         // TRUE= scope is centered vertically in the window
   isProps.uFlags |=  IMAGESCOPE_FLAGS_HCENTER;         // TRUE= scope is centered horizontally in the window
   isProps.uFlags &= ~IMAGESCOPE_FLAGS_BORDER;          // turn off border
   isProps.uFlags &= ~IMAGESCOPE_FLAGS_WINDOWINTSCALE;  // turn off integer scaling   
   isProps.uFlags &= ~IMAGESCOPE_FLAGS_ALWAYSCURRENT;   // turn off lock video      
   isProps.uFlags &= ~IMAGESCOPE_FLAGS_INFODISPLAYED;   // turn off info
   isProps.uFlags &= ~IMAGESCOPE_FLAGS_AVERAGEPIXELS;   // turn off averaging
   isProps.uFlags &= ~IMAGESCOPE_FLAGS_ZOOMTOWINDOW;    // 
   isProps.uFlags &= ~IMAGESCOPE_FLAGS_QUICKLUT;
   isProps.uFlags &= ~IMAGESCOPE_FLAGS_DISTORTASPECT;   // turn off aspect distortion
   isProps.uFlags |=  IMAGESCOPE_FLAGS_NOCLIPREGION;    // TRUE= regions will not clip to edges when dragged

   isProps.eMouseState = IMAGESCOPE_ZOOMSTATE;          // start in zoom state

   if (!IMAGESCOPE_SetProps(m_hwndScope, &isProps, &m_nLastError))
      return FALSE;

   IMAGESCOPE_CScopeCLUT ScopeCLUT;
   ScopeCLUT.m_pRed[1]   = CPoint(65535,255);
   ScopeCLUT.m_pGreen[1] = CPoint(65535,255);
   ScopeCLUT.m_pBlue[1]  = CPoint(65534,255);//CPoint(65534,0);

   ScopeCLUT.m_nRed   = 2;
   ScopeCLUT.m_nGreen = 2;
   ScopeCLUT.m_nBlue  = 2;
   VERIFY(SetCLUT(&ScopeCLUT));

   IMAGESCOPE_GetDisplayParameters( m_hwndScope, &m_DisplayParameters);

   IMAGESCOPE_SelectRegions(m_hwndScope, NULL, FALSE);
   return m_hwndScope;
}

//===============================================================================================
// FUNCTION: OverrideSaturationColor
// PURPOSE:  Override the last CLUt value to show white for saturation.
//
void CImageScope::OverrideSaturationColor()
{
   MEMBERASSERT();
   int nLUTSize = IMAGESCOPE_GetFinalLUTSize( m_hwndScope );
   ASSERT(nLUTSize > 0);

   // set the high value of the LUT to always be white
   if (nLUTSize == (USHRT_MAX+1))
   {
      for (int i = m_wSaturation; i < nLUTSize; i++)
        IMAGESCOPE_SetFinalLUTColor( m_hwndScope, i, 255, 255, 255);
   }
   else
   {
      IMAGESCOPE_SetFinalLUTColor( m_hwndScope, nLUTSize-1, 255, 255, 255);
   }
   Redraw();
}

//===============================================================================================
// FUNCTION: Copy
// PURPOSE:  Copies the current image to the clipboard.
//
void CImageScope::Copy()
{
   MEMBERASSERT();
   IMAGESCOPE_DoClipboardAction(m_hwndScope, IMAGESCOPE_CLIPBOARD_COPY, NULL, NULL);
}

//===============================================================================================
// FUNCTION: SetDisplayParameters
// PURPOSE:  Set the brightness, contrast and gamma settings.
//
BOOL CImageScope::SetDisplayParameters(const IMAGESCOPE_DisplayParameters * pDisplayParameters)
{
   MEMBERASSERT();
   if (!m_hwndScope)
      return true;

   if (!IMAGESCOPE_SetDisplayParameters( m_hwndScope, pDisplayParameters, &m_nLastError))
      return FALSE;

   OverrideSaturationColor();
   return TRUE;
}

//===============================================================================================
// FUNCTION: SetCLUT
// PURPOSE:  Set the color lookup table for the image.
//
BOOL CImageScope::SetCLUT(const IMAGESCOPE_CScopeCLUT *pCLUT)
{
   MEMBERASSERT();
   if (!IMAGESCOPE_SetCLUT( m_hwndScope, pCLUT, &m_nLastError))
      return FALSE;

   OverrideSaturationColor();
   return TRUE;
}

//===============================================================================================
// FUNCTION: SetImages
// PURPOSE:  Set the channel data used by the data source.
//
void CImageScope::SetImages(CImages *pImages, const RECT &rect)
{
   MEMBERASSERT();
   CRect r    = rect;
   CSize size = r.Size();
   m_ScanRegionList.Create(&size);

   m_DataSource.SetImages(pImages, rect, m_ScanRegionList);

   if (m_hwndScope)
   {
      VERIFY(IMAGESCOPE_RegisterDataSource( m_hwndScope, &m_DataSource, &m_nLastError));
      OverrideSaturationColor();
   }
}

//===============================================================================================
// FUNCTION: SetActive
// PURPOSE:  .
//
void CImageScope::SetActive(UINT uImage)
{
   MEMBERASSERT();
   VERIFY(IMAGESCOPE_SetActiveImageNum( m_hwndScope, uImage, 0, NULL));
}

//======================================================================================
// FUNCTION:   SetMouseState
// PURPOSE:    Checks that the scope is in the appropriate mouse state.
//
void CImageScope::SetMouseState(IMAGESCOPE_MOUSESTATE eState)
{
   MEMBERASSERT();

   // Get the current scope properties.
   IMAGESCOPE_PROPS ISProps;
   VERIFY( IMAGESCOPE_GetProps(m_hwndScope, &ISProps, NULL) );

   if (ISProps.eMouseState != eState)
   {
      ISProps.eMouseState = eState;
      ISProps.uROIType    = AXLIST_ROI_ROI;
      VERIFY( IMAGESCOPE_SetProps(m_hwndScope, &ISProps, NULL) );
   }

//   BOOL bCreateRegion = (ISProps.eMouseState == IMAGESCOPE_SELECTSTATE) && bROIMode;
//   IMAGESCOPE_SetSelectionCursor(m_hwndScope, bCreateRegion ? g_curCreateRegion : g_curDefault, 
//                                 g_curMoveRegion, g_curCopyRegion);
}

//======================================================================================
// FUNCTION:   StartZoom
// PURPOSE:    Forces the scope to start zooming.
//
void CImageScope::StartZoom()
{
   MEMBERASSERT();

   SetMouseState(IMAGESCOPE_ZOOMSTATE);

   // Unfortunately, setting the state to ZOOM_MODE is not enough.
   // The Scope window receives the WM_LBUTTONDOWN message and passes it up to the image tab.
   // When the Left Button is clicked and the WM_LBUTTONDOWN message is processed by the
   // scope window, it is not in zoom mode.  So we have to set it to ZOOM_MODE, (by calling
   // SetMouseState(c_nZOOM_MODE); ) and then resend the WM_LBUTTONDOWN message.
   // Nasty, but it works.

   POINT pt2;
   GetCursorPos(&pt2);
   ScreenToClient(m_hwndScope, &pt2);

   ::SendMessage(m_hwndScope, WM_LBUTTONDOWN, WPARAM(MK_LBUTTON), MAKELPARAM(pt2.x, pt2.y));
}

//===============================================================================================
// FUNCTION: Redraw
// PURPOSE:  .
//
void CImageScope::Redraw()
{
   MEMBERASSERT();
   if (m_hwndScope)
      IMAGESCOPE_Redraw(m_hwndScope, NULL);
}

BOOL CImageScope::FullScale()
{
   MEMBERASSERT();

   IMAGESCOPE_PROPS Props;
   VERIFY(IMAGESCOPE_GetProps(m_hwndScope, &Props, NULL));

   return Zoom(Props.rImageSize);
}

BOOL CImageScope::Zoom(const RECT &rect)
{
   CRect rZoom = rect;

   BOOL bChanged = FALSE;
   if (!IMAGESCOPE_DoZoom(m_hwndScope, rZoom.left, rZoom.top, 
                          rZoom.Width(), rZoom.Height(), &bChanged, NULL))
   {
      rZoom.InflateRect(5, 5);
      VERIFY(IMAGESCOPE_DoZoom(m_hwndScope, rZoom.left, rZoom.top, 
                               rZoom.Width(), rZoom.Height(), &bChanged, NULL));
   }

   // Redraw the scope with the new channel etc.
   Redraw();
   return bChanged;
}

//===============================================================================================
// FUNCTION: ScreenToImage
// PURPOSE:  .
//
BOOL CImageScope::ScreenToImage( POINT *ppt)
{
   MEMBERASSERT();
   POINT ptCursor = *ppt;
   ScreenToClient(m_hwndScope, &ptCursor);

   return IMAGESCOPE_ConvertClientToDestination( m_hwndScope, ptCursor, ppt);
}

//===============================================================================================
// FUNCTION: ShowLines
// PURPOSE:  .
//
void CImageScope::ShowLines(int nLines)
{
   MEMBERASSERT();
   
   const IMAGESCOPE_CDataChannel *pC = m_DataSource.GetChannel(0);
   int nNumLines = 0;
   if (!pC)
      return;
   const AXLIST_LINE *pLine = pC->GetRLEMask(0, &nNumLines);

   if (nLines < nNumLines)
      nNumLines = 0;
   int nNewLines = nLines - nNumLines;
   if (!nLines)
      return;
   CRect rChanged(0, nNumLines, pLine->nLength, nLines);
   m_DataSource.AddLines(nNewLines);
   IMAGESCOPE_Refresh(m_hwndScope, rChanged);
}
