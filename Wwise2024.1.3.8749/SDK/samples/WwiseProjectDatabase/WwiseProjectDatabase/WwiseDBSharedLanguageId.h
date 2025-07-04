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

#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "WwiseDatabaseIdentifiers.h"
#include "AdapterTypes/WwiseWrapperTypes.h"

enum class WwiseDBLanguageRequirement
{
	IsDefault,
	IsOptional,
	SFX
};

struct WwiseDBLanguageId
{
	WwiseDBShortId LanguageId = 0;
	WwiseDBString LanguageName;

	WwiseDBLanguageId() = default;

	WwiseDBLanguageId(WwiseDBShortId InLanguageId, const WwiseDBString& InLanguageName) :
		LanguageId(InLanguageId), LanguageName(InLanguageName)
	{
	}

	bool operator==(const WwiseDBLanguageId& Rhs) const
	{
		return LanguageId == Rhs.LanguageId;
	}

	bool operator!=(const WwiseDBLanguageId& Rhs) const
	{
		return LanguageId != Rhs.LanguageId;
	}

	bool operator>=(const WwiseDBLanguageId& Rhs) const
	{
		return LanguageId >= Rhs.LanguageId;
	}

	bool operator>(const WwiseDBLanguageId& Rhs) const
	{
		return LanguageId > Rhs.LanguageId;
	}

	bool operator<=(const WwiseDBLanguageId& Rhs) const
	{
		return LanguageId <= Rhs.LanguageId;
	}

	bool operator<(const WwiseDBLanguageId& Rhs) const
	{
		return LanguageId < Rhs.LanguageId;
	}
};

struct WwiseDBSharedLanguageId
{

	static const WwiseDBSharedLanguageId Sfx;

	WwiseDBSharedPtr<WwiseDBLanguageId> Language;
	
	WwiseDBLanguageRequirement LanguageRequirement = WwiseDBLanguageRequirement::SFX;

	WwiseDBSharedLanguageId():
		Language(new WwiseDBLanguageId()),
		LanguageRequirement(WwiseDBLanguageRequirement::IsOptional)
	{
	}

	WwiseDBSharedLanguageId(WwiseDBShortId InLanguageId, const WwiseDBString& InLanguageName, WwiseDBLanguageRequirement InLanguageRequirement) :
		Language(new WwiseDBLanguageId(InLanguageId, InLanguageName)),
		LanguageRequirement(InLanguageRequirement)
	{
	}

	WwiseDBSharedLanguageId(const WwiseDBLanguageId& InLanguageId, WwiseDBLanguageRequirement InLanguageRequirement):
		Language(new WwiseDBLanguageId(InLanguageId)),
		LanguageRequirement(InLanguageRequirement)
	{
	}

	~WwiseDBSharedLanguageId()
	{
	}


	WwiseDBShortId GetLanguageId() const
	{
		return Language->LanguageId;
	}

	const WwiseDBString& GetLanguageName() const
	{
		return Language->LanguageName;
	}

	bool operator==(const WwiseDBSharedLanguageId& Rhs) const
	{
		return (!Language.IsValid() && !Rhs.Language.IsValid())
			|| (Language.IsValid() && Rhs.Language.IsValid() && *Language == *Rhs.Language);
	}

	bool operator!=(const WwiseDBSharedLanguageId& Rhs) const
	{
		return (Language.IsValid() != Rhs.Language.IsValid())
			|| (Language.IsValid() && Rhs.Language.IsValid() && *Language != *Rhs.Language);
	}

	bool operator>=(const WwiseDBSharedLanguageId& Rhs) const
	{
		return (!Language.IsValid() && !Rhs.Language.IsValid())
			|| (Language.IsValid() && Rhs.Language.IsValid() && *Language >= *Rhs.Language);
	}

	bool operator>(const WwiseDBSharedLanguageId& Rhs) const
	{
		return (Language.IsValid() && !Rhs.Language.IsValid())
			|| (Language.IsValid() && Rhs.Language.IsValid() && *Language > *Rhs.Language);
	}

	bool operator<=(const WwiseDBSharedLanguageId& Rhs) const
	{
		return (!Language.IsValid() && !Rhs.Language.IsValid())
			|| (Language.IsValid() && Rhs.Language.IsValid() && *Language <= *Rhs.Language);
	}

	bool operator<(const WwiseDBSharedLanguageId& Rhs) const
	{
		return (!Language.IsValid() && Rhs.Language.IsValid())
			|| (Language.IsValid() && Rhs.Language.IsValid() && *Language < *Rhs.Language);
	}
};

inline WwiseDBShortId GetTypeHash(const WwiseDBSharedLanguageId& Id)
{
	return GetTypeHash(Id.Language->LanguageId);
}
