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

#include "Ref/WwiseRefPlatformInfo.h"
#include "Ref/WwiseRefProjectInfo.h"

class WwiseRefPlatform : public WwiseRefPlatformInfo
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::Platform;

	// The reference does contain supplemental information, such as Path.
	WwiseRefProjectInfo ProjectInfo;
	WwiseRefIndexType ProjectInfoPlatformReferenceIndex;

	WwiseRefPlatform() :
		ProjectInfo(),
		ProjectInfoPlatformReferenceIndex(-1)
	{}
	WwiseRefPlatform(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		const WwiseMetadataSharedRootFileConstPtr& InProjectInfoRootMediaRef, const WwiseDBString& InProjectInfoJsonFilePath,
		WwiseRefIndexType InProjectInfoPlatformReferenceIndex) :
		WwiseRefPlatformInfo(InRootMediaRef, InJsonFilePath),
		ProjectInfo(InProjectInfoRootMediaRef, InProjectInfoJsonFilePath),
		ProjectInfoPlatformReferenceIndex(InProjectInfoPlatformReferenceIndex)
	{}
	WwiseRefPlatform(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath) :
		WwiseRefPlatformInfo(InRootMediaRef, InJsonFilePath),
		ProjectInfo(),
		ProjectInfoPlatformReferenceIndex()
	{}
	WwiseRefPlatform(const WwiseMetadataSharedRootFileConstPtr& InProjectInfoRootMediaRef, const WwiseDBString& InProjectInfoJsonFilePath,
		WwiseRefIndexType InProjectInfoPlatformReferenceIndex) :
		WwiseRefPlatformInfo(),
		ProjectInfo(InProjectInfoRootMediaRef, InProjectInfoJsonFilePath),
		ProjectInfoPlatformReferenceIndex(InProjectInfoPlatformReferenceIndex)
	{}
	void Merge(WwiseRefPlatform&& InOtherPlatform);

	const WwiseMetadataPlatform* GetPlatform() const;
	const WwiseMetadataPlatformReference* GetPlatformReference() const;

	const WwiseDBGuid* PlatformGuid() const;
	const WwiseDBString* PlatformName() const;
	WwiseDBGuid BasePlatformGuid() const;
	WwiseDBString BasePlatformName() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
};
