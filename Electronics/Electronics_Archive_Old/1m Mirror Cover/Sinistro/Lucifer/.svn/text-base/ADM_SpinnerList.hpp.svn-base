//***********************************************************************************************
//
//    Copyright (c) 2001 Axon Instruments.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  ADM_SpinnerList.HPP
// PURPOSE: 
// AUTHOR:  BHI  Oct 2001
//

#ifndef INC_ADM_SPINNERLIST_HPP
#define INC_ADM_SPINNERLIST_HPP

#pragma once
#include "\AxonDev\Comp\ADMExtensions\ADMExtensions.hpp"
#include <vector>

#define ADM_SpinnerList_BASE   ADM_StaticSpinner

class ADM_SpinnerList : public ADM_SpinnerList_BASE
{
private:   // Attributes
   bool m_bWrap;
   int  m_nValue;
   std::vector<LPSTR> m_List;

private:   // Unimplemented copy functions.
   ADM_SpinnerList(const ADM_SpinnerList &);
   const ADM_SpinnerList &operator=(const ADM_SpinnerList &);

public:    // Public interface
   ADM_SpinnerList()  
   {
      m_bWrap  = false;
      m_nValue = 0;
   }
   ~ADM_SpinnerList()
   {
      Clear(); 
   }

   void AllowWrap()
   {
      m_bWrap = true;
   }

   void Clear()
   {
      for (UINT i=0; i<m_List.size(); i++)
         free(m_List[i]);
      m_List.clear();
      Step(0);
   }

   void Step(int nStep=1)
   {
      m_nValue += nStep;
      if (m_nValue < 0)
      {
         if (m_bWrap)
            m_nValue = m_List.size()-1;
         else
            m_nValue = 0;
      }
      else if (m_nValue >= (int)m_List.size())
      {
         if (m_bWrap)
            m_nValue = 0;
         else
            m_nValue = m_List.size()-1;
      }
      bool bBadVal = (m_nValue < 0) || (m_nValue >= (int)m_List.size());
      SetFormatItem(0, bBadVal ? "" : m_List[m_nValue]);
   }

   void AddString(LPCSTR psz)
   {
      m_List.push_back(strdup(psz));
      if (m_nValue < 0)
         Step(0);
   }

   void SetValue(int nValue)
   {
      m_nValue = nValue;
      Step(0);
   }
   int GetValue()             { return m_nValue; }

   void operator=(int nValue) { SetValue(nValue);  }
   operator int()             { return GetValue(); }

   void Notification( ADM_Wnd *pWnd, UINT uID, int nNotificationCode, LONG lData )
   {
      if (pWnd == &m_Spinner)
         Step(nNotificationCode == ADM_NOTIFY_GAUGE_LINEUP ? -1 : 
              nNotificationCode == ADM_NOTIFY_GAUGE_LINEDOWN ? 1 : 0);

      // Send the notifications back to the parent for handling
      ADM_SpinnerList_BASE::Notification(pWnd, uID, nNotificationCode, lData);
   }

};

#endif      // INC_ADM_SPINNERLIST_HPP
