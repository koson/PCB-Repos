//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  ImageScope.HPP
// PURPOSE: 
//

#ifndef INC_IMAGESCOPE_HPP
#define INC_IMAGESCOPE_HPP

#include "wincpp.hpp"

#include "ScanRegionList.hpp"
#include "Image.hpp"

class CImageChannel : public IMAGESCOPE_CDataChannel
{
private:
   CImage                      *m_pImage;
   IMAGESCOPE_CDataChannelProps m_Props;
   CArrayPtrEx<AXLIST_LINE>     m_Lines;
   UINT                         m_uValidLines;
   CRect                        m_rImage;
   CRect                        m_rROI;

   CImageChannel(const CImageChannel &);
   const CImageChannel &operator=(const CImageChannel &);

public:
   CImageChannel()
   {
      m_pImage                = NULL;
      m_Props.nWidth          = 100;
      m_Props.nHeight         = 100;
      m_Props.nScanLine       = 100 * sizeof(WORD);
      m_Props.nBPP            = 16;
      m_Props.nBytesPerPixel  = sizeof(WORD);
      m_uValidLines           = 0;
   }

   void SetImage(CImage *pImage, const RECT &rect, AXLIST_CRegionList *pRegionList)
   {
      m_pImage      = pImage;
      m_uValidLines = 0;

      m_rImage = rect;
      m_rROI   = pImage->GetBounds();

      memset(&m_Props, 0, sizeof(m_Props));
      m_Props.nBPP            = 16;
      m_Props.nBytesPerPixel  = sizeof(WORD);
      m_Props.nWidth          = m_rImage.IsRectEmpty() ? 100 : m_rImage.Width();
      m_Props.nHeight         = m_rImage.IsRectEmpty() ? 100 : m_rImage.Height();
      m_Props.nScanLine       = m_Props.nWidth * m_Props.nBytesPerPixel;
      m_Props.pRegionList     = pRegionList;

      AXLIST_LINE Line = { 0 };
      Line.xStart  = m_rROI.left;
      Line.nLength = m_rROI.Width();
      m_Lines.Alloc(m_rROI.Height());

      for (Line.yStart=m_rROI.top; Line.yStart<m_rROI.bottom; Line.yStart++)
      {
         m_Lines[Line.yStart] = Line;
         Line.nPixelsBefore  += Line.nLength;
      }
   }
   const WORD *GetImage(SIZE &size) const                           
   {
      size.cx = m_Lines ? m_Lines[0].nLength : 0;
      size.cy = m_Lines.GetCount();

      return m_pImage->GetPtr(); 
   }
   int GetNmPerPixel() const { return m_pImage ? m_pImage->GetNmPerPixel() : 0; }

   void AddLines(UINT uLines) 
   {
      m_uValidLines = min(m_Lines.GetCount(), m_uValidLines + uLines);
   }

   // retrieves name of channel
   virtual void GetName(char * pChannelName, int nBufSize) const        { AXU_strncpyz(pChannelName, "ImageChannel", nBufSize); }

   // retrieves channel info structure 
   virtual void GetProps(IMAGESCOPE_CDataChannelProps * pProps) const   { *pProps = m_Props;    }

   // retrieves the number of images in the channel
   virtual int  GetNumImages() const                                    { return 1; }
   
   // given an image index, what is the number of this image, use the value returned from this function for all other
   // image number parameters
   virtual int  GetImageNumber(int nImageIndex) const                   { return nImageIndex; }

   // retrieves image ptr for given frame, the ptr is always the start of the image buffer   
   // nLockRLELineStart = starting RLE line that needs to be access by this call (all RLE lines before are guaranteed not be accessed)
   // nNumRLELines = # of RLEs needed to be locked from nLockRLELineStart downward
   virtual const BYTE * GetImagePtr(int nImageNum, int nLockRLELineStart, int nNumRLELines) const 
   { return (const BYTE *)m_pImage->GetPtr(); }

   // releases the image ptr returned by the previous GetImagePtr
   // ReleaseImagePtr will always be called before another GetImagePtr is called
   virtual void ReleaseImagePtr(int nImageNum) const {}
      
   // retrieves RLE structure for given image
   // returns the bitmask for the image to reconstruct it, it will never be NULL if the image ptr != NULL
   virtual const AXLIST_LINE * GetRLEMask(int nImageNum, int * pnNumLines) const
   {
      if (pnNumLines)
         *pnNumLines = int(m_uValidLines);
      return m_Lines;
   }
   
   // retrieves time stamp for image 
   virtual DWORD  GetTimeStamp(int nImageNum) const                     { return 0; }
};

class CAutoSource : public IMAGESCOPE_CDataSource
{
private:
   CArrayPtrEx<CImageChannel> m_Channels;

private:   // Unimplemented copy functions.
   CAutoSource(const CAutoSource &);
   const CAutoSource &operator=(const CAutoSource &);

public:         

   CAutoSource()          {}
   virtual ~CAutoSource() {} 
                  
   // retrieves name of datasource
   virtual void  GetName(char * pName, int nBufSize) const    { AXU_strncpyz(pName, "AutoSource", nBufSize); }

   // Get a read-only pointer to a channel object.
   virtual const IMAGESCOPE_CDataChannel *GetChannel(int nChannel) const { return m_Channels + nChannel; }
      
   // returns the number of logical channels that are available in the system        
   virtual int GetNumChannels() const { return m_Channels.GetCount(); }

   bool SetImages(CImages *pImages, const RECT &rect, AXLIST_CRegionList *pRegionList)
   {
      if (!m_Channels.Alloc(pImages->GetCount()))
         return false;

      for (UINT i=0; i<m_Channels.GetCount(); i++)
         m_Channels[i].SetImage(pImages->GetImage(i), rect, pRegionList); 
      return true;
   }

   void AddLines(UINT uLines) 
   { 
      for (UINT i=0; i<m_Channels.GetCount(); i++)
         m_Channels[i].AddLines(uLines); 
   }
};

class CImageScope
{
private:   // Attributes
   HWND                         m_hwndScope;
   CAutoSource                  m_DataSource;
   int                          m_nLastError;
   IMAGESCOPE_DisplayParameters m_DisplayParameters;
   CScanRegionList              m_ScanRegionList;
   WORD                         m_wSaturation;

private:   // Unimplemented copy functions.
   CImageScope(const CImageScope &);
   const CImageScope &operator=(const CImageScope &);

private:   // Private functions
   void OverrideSaturationColor();

public:    // Public interface
   CImageScope();
   ~CImageScope();

   HWND Create(HWND hwndParent, int nScopeID);
   BOOL SetDisplayParameters(const IMAGESCOPE_DisplayParameters * pDisplayParameters);
   BOOL SetCLUT(const IMAGESCOPE_CScopeCLUT *pCLUT);
   void SetImages(CImages *pImages, const RECT &rect);
   void SetMouseState(IMAGESCOPE_MOUSESTATE eState);
   void StartZoom();
   void SetScanRect(LPCRECT prScan) { m_ScanRegionList.SetRect(prScan); }
   void SetActive(UINT uImage);
   BOOL ScreenToImage( POINT *ppt);
   
   void Redraw();
   HWND GetHWND()                  { return m_hwndScope; }

   void ShowLines(int uLines);
   void SetSaturationColor(WORD wSaturation)
   {
      m_wSaturation = wSaturation;
      SetDisplayParameters(&m_DisplayParameters);
   }

   WORD GetSaturationColor() const { return m_wSaturation; }

   void SetContrast(float fContrast)
   {
      m_DisplayParameters.fContrast = fContrast;
      SetDisplayParameters(&m_DisplayParameters);
   }
   void SetBrightness(float fBrightness)
   {
      m_DisplayParameters.fBrightness = fBrightness;
      SetDisplayParameters(&m_DisplayParameters);
   }

   float GetContrast() const          { return m_DisplayParameters.fContrast; }
   float GetBrightness() const        { return m_DisplayParameters.fBrightness; }

   BOOL Zoom(const RECT &rect);
   BOOL FullScale();
   void Copy();

   void OnScopeMessage(UINT uMsg, LPARAM lParam);
};

#endif      // INC_IMAGESCOPE_HPP

