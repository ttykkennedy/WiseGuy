/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CPerforceApp
// See Perforce.cpp for the implementation of this class
//

class CPerforceApp : public CWinApp
{
public:
	CPerforceApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
