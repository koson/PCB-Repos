//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  Image.HPP
// PURPOSE: 
// AUTHOR:  BHI  May 1998
//

#ifndef INC_IMAGE_HPP
#define INC_IMAGE_HPP

class CImage
{
private:
   int                  m_nNmPerPixel;
   CRect                m_rBounds;
   CArrayPtrEx<WORD>    m_Image;
   char                 m_szName[32];

private:   
   const CImage &operator=(const CImage &S);  // Disable the default operator=()
   CImage(const CImage &CS);                  // Disable the default copy constructor

public:   
   CImage();
   ~CImage();

   bool SetProps(LPCSTR pszName, int nNmPerPixel, LPCRECT prScan);

   int     GetPixelCount() const    { return m_Image.GetCount(); }
   LPCRECT GetBounds() const        { return m_rBounds;  }
   int     GetNmPerPixel() const    { return m_nNmPerPixel; }

   LPCWORD GetPtr() const           { return m_Image;    }
   LPWORD  GetPtr()                 { return m_Image;    }

   WORD GetDataValue(POINT pt, WORD nMissing=0) const;
   LPCSTR GetName() const           { return m_szName;   }

   // deletes the images for this channel
   void Discard();
   void Clear()                     { m_Image.Zero();    }

   // Debugging
   void XorRect(LPCRECT pr, int nBorder=0);
};

class CImages
{
private:
   int                  m_nNmPerPixel;
   CRect                m_rBounds;
   CArrayPtrEx<CImage>  m_Images;

private:   
   const CImages &operator=(const CImages &S);  // Disable the default operator=()
   CImages(const CImages &CS);                  // Disable the default copy constructor

public:   
   CImages()  {}
   ~CImages() {}

   bool SetProps(const LPCSTR *pNames, UINT uImages, int nNmPerPixel, LPCRECT prScan)
   {
      if (!m_Images.Alloc(uImages))
         return false;
      for (UINT i=0; i<m_Images.GetCount(); i++)
         m_Images[i].SetProps(pNames[i], nNmPerPixel, prScan);
      return true;
   }

   UINT    GetCount() const          { return m_Images.GetCount(); }
   CImage *GetImage(UINT uImage)     { return m_Images + uImage;   }
   LPCSTR  GetImageName(UINT uImage) { return m_Images[uImage].GetName(); }

   void Discard()                    { m_Images.Free();            }
   void Clear()                      
   { 
      for (UINT i=0; i<m_Images.GetCount(); i++)
         m_Images[i].Clear();
   }
};

#endif      // INC_IMAGE_HPP
