//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  Image.CPP
// PURPOSE: 
// AUTHOR:  BHI  May 1998
//

#include "wincpp.hpp"
#include "Image.hpp"

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CImage::CImage()
{
   MEMBERASSERT();      
   m_nNmPerPixel = 0;
   m_szName[0]   = '\0';
}

//===============================================================================================
// FUNCTION: Destructor
// PURPOSE:  Object cleanup.
//
CImage::~CImage()
{
   MEMBERASSERT();
   Discard();   
}

//===============================================================================================
// FUNCTION: Discard
// PURPOSE:  deletes the images for this channel
//
void CImage::Discard()
{
   MEMBERASSERT();
   m_Image.Free();
   m_rBounds.SetRectEmpty();
}

//===============================================================================================
// FUNCTION: SetProps
// PURPOSE:  creates the images for this channel
//
bool CImage::SetProps(LPCSTR pszName, int nNmPerPixel, LPCRECT prScan)
{
   MEMBERASSERT();
   Discard();
   m_nNmPerPixel = nNmPerPixel;
   if (prScan)
      m_rBounds  = *prScan;
   AXU_STRNCPYZ(m_szName, pszName);

   // Resize the image storage.
   if (!m_Image.Alloc(m_rBounds.Width()*m_rBounds.Height()))
   {
      Discard();
      return false;
   }
   return true;
}

//===============================================================================================
// FUNCTION: GetDataValue
// PURPOSE:  Gets a data value from the array.
//
WORD CImage::GetDataValue(POINT pt, WORD wMissing) const
{
   if (!m_rBounds.PtInRect(pt))
      return wMissing;

   CPoint ptRel = pt;
   ptRel -= m_rBounds.TopLeft();
   return m_Image[ptRel.y * m_rBounds.Width() + ptRel.x];
}

static void XorIt(LPWORD pw, const CRect &rBounds, const CRect &rXor)
{
   UINT uScanLine = rBounds.Width();
   LPWORD pTL = pw + rXor.top * uScanLine + rXor.left;

   for (int y=0; y<rXor.Height(); y++)
      for (int x=0; x<rXor.Width(); x++)
         pTL[y*uScanLine+x] ^= 0x8000;
}

//===============================================================================================
// FUNCTION: XorRect
// PURPOSE:  .
//
void CImage::XorRect(LPCRECT pr, int nBorder)
{
   MEMBERASSERT();

   // Get intersection with pixels.
   CRect rIntersect;
   if (!rIntersect.IntersectRect(m_rBounds, pr))
      return;

   // Offset to be relative to TL.
   rIntersect.OffsetRect(-m_rBounds.left, -m_rBounds.top);

   if (!nBorder)
   {
      XorIt(m_Image, m_rBounds, rIntersect);
      return;
   }

   CRect r1=rIntersect, r2=rIntersect, r3=rIntersect, r4=rIntersect;
   r1.bottom = r1.top    + nBorder;
   r3.top    = r3.bottom - nBorder;

   r2.top    = r1.bottom;
   r2.bottom = r3.top;
   r2.left   = r2.right - nBorder;

   r4.top    = r1.bottom;
   r4.bottom = r3.top;
   r4.right  = r4.left + nBorder;

   XorIt(m_Image, m_rBounds, r1);
   XorIt(m_Image, m_rBounds, r2);
   XorIt(m_Image, m_rBounds, r3);
   XorIt(m_Image, m_rBounds, r4);
}
