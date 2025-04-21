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

#include "WwiseGeneratedFiles.h"

#include "AdapterTypes/IWwiseDirectoryVisitor.h"

class WwiseDirectoryVisitor
{
	friend IWwiseDirectoryVisitor<WwiseDirectoryVisitor>;
public:
	WwiseDirectoryVisitor(const WwiseDBString* InPlatformName = nullptr) :
		PlatformName(InPlatformName)
	{}

	WwiseGeneratedFiles& Get();

protected:
	bool Visit(const WwiseDBString& FilenameOrDirectory, bool bIsDirectory);

private:
	WwiseGeneratedFiles GeneratedDirectory;
	
	class FPlatformRootDirectoryVisitor;

	WwiseDBArray<WwiseDBFuture<FPlatformRootDirectoryVisitor*>> Futures;

	const WwiseDBString* PlatformName;

	class FSoundBankVisitor;
	class FMediaVisitor;
	WwiseDirectoryVisitor& operator=(const WwiseDirectoryVisitor& Rhs) = delete;
	WwiseDirectoryVisitor(const WwiseDirectoryVisitor& Rhs) = delete;
};
