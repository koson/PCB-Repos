//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  ScanRegionList.HPP
// PURPOSE: 
// AUTHOR:  BHI  Jun 1998
//

#ifndef INC_SCANREGIONLIST_HPP
#define INC_SCANREGIONLIST_HPP

class AXLIST_CRegionList;

class CScanRegionList
{
private:   // Attributes
   AXLIST_CRegionList *m_pRegionList;

private:   // Private functions
   BOOL CleanUp();

private:   // Unimplemented copy functions.
   CScanRegionList(const CScanRegionList &);
   const CScanRegionList &operator=(const CScanRegionList &);

public:    // Public interface
   CScanRegionList();
   ~CScanRegionList();

   operator AXLIST_CRegionList *() { return m_pRegionList; }

   BOOL Create(const SIZE *pImageSize);
   void Clear();

   void GetRect(RECT *pRect) const;
   void SetRect(const RECT *pRect);
   void Freeze(BOOL bFreeze=TRUE);
};

#endif      // INC_SCANREGIONLIST_HPP

