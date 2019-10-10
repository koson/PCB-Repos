//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  ScanRegionList.CPP
// PURPOSE: 
// AUTHOR:  BHI  Jun 1998
//

#include "wincpp.hpp"
#include "ScanRegionList.hpp"

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CScanRegionList::CScanRegionList()
{
   MEMBERASSERT();
   m_pRegionList = NULL;
}

//===============================================================================================
// FUNCTION: Destructor
// PURPOSE:  Object cleanup.
//
CScanRegionList::~CScanRegionList()
{
   MEMBERASSERT();
   CleanUp();
}

//===============================================================================================
// FUNCTION: Create
// PURPOSE:  Create the images and the region list used by the AcquisitionEngine and the scopes.
//
BOOL CScanRegionList::Create(const SIZE *pImageSize)
{
   MEMBERASSERT();

   // Clear any existing list.
   CleanUp();

   m_pRegionList = AXLIST_CreateNewRegionList( pImageSize->cx, pImageSize->cy, FALSE, NULL);
   WPTRASSERT(m_pRegionList);
   if (!m_pRegionList)
      return FALSE;

   m_pRegionList->ClearRegionTypes();

   AXLIST_CRegion *pScanRegion = AXLIST_CreateRectRegion();
   if (!pScanRegion || !m_pRegionList->AddNewRegionType(pScanRegion))
   {
      if (pScanRegion)
         pScanRegion->Delete();
      return CleanUp();
   }
   return (m_pRegionList!=NULL);
}

//===============================================================================================
// FUNCTION: CleanUp
// PURPOSE:  Delete any held region list.
//
BOOL CScanRegionList::CleanUp()
{
   MEMBERASSERT();
   if (m_pRegionList)
      VERIFY(AXLIST_DestroyRegionList( m_pRegionList, NULL));
   m_pRegionList = NULL;
   return FALSE;  // not a failure -- a convenience!
}

//===============================================================================================
// FUNCTION: Clear
// PURPOSE:  External access to our internal cleanup function.
//
void CScanRegionList::Clear()
{
   MEMBERASSERT();
   CleanUp();
}

//===============================================================================================
// FUNCTION: GetRect
// PURPOSE:  Returns the (GDI) rectangle coordinates of the region.
//
void CScanRegionList::GetRect(RECT *pRect) const
{
   MEMBERASSERT();
   WPTRASSERT(m_pRegionList);
   ASSERT(m_pRegionList->GetCount()<=1);

   if (m_pRegionList->GetCount() < 1)
   {
      CRect rEmpty;
      *pRect = rEmpty;
      return;
   }

   AXLIST_CRegion *pRegion = m_pRegionList->GetFirstRegion();
   CRect r = pRegion->GetBoundingRect(false);      

   // The vertices are inclusive of the right and botton, so adjust the rectangle
   // so that it reflects the GDI convention of exclusive of right and bottom.
   r.right++;
   r.bottom++;

   // Normalize the rectangle in case the user has done something weird dragging ordinates.
   r.NormalizeRect();
   *pRect = r;
}

//===============================================================================================
// FUNCTION: SetRect
// PURPOSE:  Sets the (GDI) rectangle coordinates of the region.
//
void CScanRegionList::SetRect(const RECT *pRect)
{
   MEMBERASSERT();
   UINT uCount = m_pRegionList->GetCount();
   ASSERT(m_pRegionList->GetCount()<=1);

   if (!m_pRegionList)
      return;
   WPTRASSERT(m_pRegionList);

   // Normalize the rectangle in case the user has done something weird dragging ordinates.
   CRect r = *pRect;
   r.NormalizeRect();

   // If the rectangle is empty, delete any held region.
   if (r.IsRectEmpty())
   {
      if (uCount > 0)
      {
         AXLIST_CRegion *pRegion = m_pRegionList->GetFirstRegion();
         m_pRegionList->RemoveRegion(pRegion);  
         m_pRegionList->DeleteRegion(pRegion);
      }
      return;
   }

   // If no regions are held, create one.
   if (uCount == 0)
   {
      AXLIST_CRegion *pScanRegion = AXLIST_CreateRectRegion();
      if (!pScanRegion || !m_pRegionList->AddRegion(pScanRegion))
         return;
   }

   // Set the size of the region.
   AXLIST_CRegion *pRegion = m_pRegionList->GetFirstRegion();
   CPoint Points[2];
   Points[0].x = r.left;
   Points[0].y = r.top;
   Points[1].x = r.right;
   Points[1].y = r.bottom;
   pRegion->SetVertices(Points, ELEMENTS_IN(Points)); 

/*
   // The vertices are inclusive of the right and botton, so adjust the rectangle
   // so that it is inclusive of right and bottom.
   // r.right--;
   // r.bottom--;

   // Set the size of the region.
   AXLIST_CRegion *pRegion = m_pRegionList->GetFirstRegion();
   CPoint Points[4];
   Points[0].x = r.left;
   Points[0].y = r.top;
   Points[1].x = r.right;
   Points[1].y = r.top;
   Points[2].x = r.right;
   Points[2].y = r.bottom;
   Points[3].x = r.left;
   Points[3].y = r.bottom;
   pRegion->SetVertices(Points, ELEMENTS_IN(Points)); 
*/
}

//===============================================================================================
// FUNCTION: Freeze
// PURPOSE:  Freeze the region to prevent any modification of it.
//
void CScanRegionList::Freeze(BOOL bFreeze)
{
   MEMBERASSERT();
   if (m_pRegionList)
      m_pRegionList->SetROIFreeze(bFreeze);
}

