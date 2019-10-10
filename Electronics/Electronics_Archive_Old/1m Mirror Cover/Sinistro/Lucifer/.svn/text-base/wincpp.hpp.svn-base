//***********************************************************************************************
//
//    Copyright (c) 2009 LCOGT.
//    All rights reserved.
//
//***********************************************************************************************
#ifndef _LUCIFER_WINCPP_HPP_INCLUDED_
#define  _LUCIFER_WINCPP_HPP_INCLUDED_

// wincpp.hpp : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#ifndef WINVER
#define WINVER 0x0400
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "..\Common\AxoDefn.h"
#include "..\Common\AxoDebug.h"
#include "..\Common\ArrayPtr.hpp"
#include "..\Common\ThreadName.h"
#include "..\Common\AcqTimer.hpp"

#include "..\Common\FileIO.hpp"
#include "..\Common\FilePath.h"
#include "..\Common\FindFile.hpp"
//#include "..\Common\ProgressDlg.hpp"
#include "..\Common\WaitCursor.hpp"
#include "..\Common\AxList_Line.h"
#include "..\Common\AxList_CRegion.hpp"
#include "..\Common\ImageScope_CDataSource.hpp"
#include "..\Common\ADM.hpp"

//lib
#include "..\inc\AxoUtils32.h"
#include "..\inc\AxImageIO.h"
#include "..\ADMExtensions\ADMExtensions.hpp"


#include "..\inc\AxImageScope32.h"
#include "..\inc\AxImageRegion32.h"


// LumiCam hardware constants.
#include "..\..\Firmware\SinistroUSB\SinistroUSB.h"
// c++ driver dll interface
#include "..\Sinistro\Sinistro.h"

#include <math.h>

inline int Round(double dVal)
{
   return (int)floor(dVal+0.5);
}

#endif //_LUCIFER_WINCPP_HPP_INCLUDED_

