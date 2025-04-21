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

#include "AdapterTypes/WwiseWrapperTypes.h"

struct WwiseDatabaseMediaIdKey
{
	unsigned int MediaId = 0;
	unsigned int SoundBankId = 0;

	WwiseDatabaseMediaIdKey()
	{}
	WwiseDatabaseMediaIdKey(unsigned int InMediaId, unsigned int InSoundBankId) :
		MediaId(InMediaId),
		SoundBankId(InSoundBankId)
	{}
	bool operator==(const WwiseDatabaseMediaIdKey& Rhs) const
	{
		return MediaId == Rhs.MediaId
			&& SoundBankId == Rhs.SoundBankId;
	}
	bool operator<(const WwiseDatabaseMediaIdKey& Rhs) const
	{
		return (MediaId < Rhs.MediaId)
			|| (MediaId == Rhs.MediaId && SoundBankId < Rhs.SoundBankId);
	}
};

struct WwiseDatabaseLocalizableIdKey
{
	static constexpr unsigned int GENERIC_LANGUAGE = 0;

	unsigned int Id = 0;
	unsigned int LanguageId = 0;
	unsigned int SoundBankId = 0;

	WwiseDatabaseLocalizableIdKey()
	{}
	WwiseDatabaseLocalizableIdKey(unsigned int InId, unsigned int InLanguageId) :
		Id(InId),
		LanguageId(InLanguageId)
	{}
	WwiseDatabaseLocalizableIdKey(unsigned int InId, unsigned int InLanguageId, unsigned int InSoundBankId) :
		Id(InId),
		LanguageId(InLanguageId),
		SoundBankId(InSoundBankId)
	{}
	bool operator==(const WwiseDatabaseLocalizableIdKey& Rhs) const
	{
		return Id == Rhs.Id
			&& LanguageId == Rhs.LanguageId
			&& SoundBankId == Rhs.SoundBankId;
	}
	bool operator<(const WwiseDatabaseLocalizableIdKey& Rhs) const
	{
		return (Id < Rhs.Id)
			|| (Id == Rhs.Id && LanguageId < Rhs.LanguageId)
			|| (Id == Rhs.Id && LanguageId == Rhs.LanguageId && SoundBankId < Rhs.SoundBankId);
	}
};

struct WwiseDatabaseGroupValueKey
{

	unsigned int GroupId = 0;
	unsigned int Id = 0;

	WwiseDatabaseGroupValueKey()
	{}
	WwiseDatabaseGroupValueKey(unsigned int InGroupId, unsigned int InId) :
		GroupId(InGroupId),
		Id(InId)
	{}
	bool operator==(const WwiseDatabaseGroupValueKey& Rhs) const
	{
		return GroupId == Rhs.GroupId
			&& Id == Rhs.Id;
	}
	bool operator<(const WwiseDatabaseGroupValueKey& Rhs) const
	{
		return (GroupId < Rhs.GroupId)
			|| (GroupId == Rhs.GroupId && Id < Rhs.Id);
	}
};

struct WwiseDatabaseLocalizableGroupValueKey
{

	static constexpr unsigned int GENERIC_LANGUAGE = 0;

	WwiseDatabaseGroupValueKey GroupValue;
	unsigned int LanguageId = 0;

	WwiseDatabaseLocalizableGroupValueKey()
	{}
	WwiseDatabaseLocalizableGroupValueKey(unsigned int InGroup, unsigned int InId, unsigned int InLanguageId) :
		GroupValue(InGroup, InId),
		LanguageId(InLanguageId)
	{}
	WwiseDatabaseLocalizableGroupValueKey(WwiseDatabaseGroupValueKey InGroupValue, unsigned int InLanguageId) :
		GroupValue(InGroupValue),
		LanguageId(InLanguageId)
	{}
	bool operator==(const WwiseDatabaseLocalizableGroupValueKey& Rhs) const
	{
		return GroupValue == Rhs.GroupValue
			&& LanguageId == Rhs.LanguageId;
	}
	bool operator<(const WwiseDatabaseLocalizableGroupValueKey& Rhs) const
	{
		return (GroupValue < Rhs.GroupValue)
			|| (GroupValue == Rhs.GroupValue && LanguageId < Rhs.LanguageId);
	}
};

struct WwiseDatabaseLocalizableGuidKey
{

	static constexpr unsigned int GENERIC_LANGUAGE = WwiseDatabaseLocalizableIdKey::GENERIC_LANGUAGE;

	WwiseDBGuid Guid;
	unsigned int LanguageId = 0;		// 0 if no Language

	WwiseDatabaseLocalizableGuidKey()
	{}
	WwiseDatabaseLocalizableGuidKey(WwiseDBGuid InGuid, unsigned int InLanguageId) :
	Guid(InGuid),
	LanguageId(InLanguageId)
	{}
	bool operator==(const WwiseDatabaseLocalizableGuidKey& Rhs) const
	{
		return Guid == Rhs.Guid
			&& LanguageId == Rhs.LanguageId;
	}
	bool operator<(const WwiseDatabaseLocalizableGuidKey& Rhs) const
	{
		return (Guid < Rhs.Guid)
			|| (Guid == Rhs.Guid && LanguageId < Rhs.LanguageId);
	}
};

struct WwiseDatabaseLocalizableNameKey
{

	static constexpr unsigned int GENERIC_LANGUAGE = WwiseDatabaseLocalizableIdKey::GENERIC_LANGUAGE;

	WwiseDBString Name;
	unsigned int LanguageId = 0;		// 0 if no Language

	WwiseDatabaseLocalizableNameKey()
	{}
	WwiseDatabaseLocalizableNameKey(WwiseDBString InName, unsigned int InLanguageId) :
		Name(*InName),
		LanguageId(InLanguageId)
	{}

	bool operator==(const WwiseDatabaseLocalizableNameKey& Rhs) const
	{
		return Name == Rhs.Name
			&& LanguageId == Rhs.LanguageId;
	}
	bool operator<(const WwiseDatabaseLocalizableNameKey& Rhs) const
	{
		return (Name < Rhs.Name)
			|| (Name == Rhs.Name && LanguageId < Rhs.LanguageId);
	}
};

unsigned int GetTypeHash(const WwiseDatabaseMediaIdKey& FileId);
unsigned int GetTypeHash(const WwiseDatabaseLocalizableIdKey& LocalizableId);
unsigned int GetTypeHash(const WwiseDatabaseGroupValueKey& LocalizableGroupValue);
unsigned int GetTypeHash(const WwiseDatabaseLocalizableGroupValueKey& LocalizableGroupValue);
unsigned int GetTypeHash(const WwiseDatabaseLocalizableIdKey& EventId);
unsigned int GetTypeHash(const WwiseDatabaseLocalizableGuidKey& LocalizableGuid);
unsigned int GetTypeHash(const WwiseDatabaseLocalizableNameKey& LocalizableName);

inline unsigned int GetTypeHash(const WwiseDBShortId& ShortId)
{
	return ShortId;
}

inline unsigned int WwiseDBHashCombine(unsigned int A, unsigned int C)
{
	unsigned int B = 0x9e3779b9;
	A += B;

	A -= B; A -= C; A ^= (C>>13);
	B -= C; B -= A; B ^= (A<<8);
	C -= A; C -= B; C ^= (B>>13);
	A -= B; A -= C; A ^= (C>>12);
	B -= C; B -= A; B ^= (A<<16);
	C -= A; C -= B; C ^= (B>>5);
	A -= B; A -= C; A ^= (C>>3);
	B -= C; B -= A; B ^= (A<<10);
	C -= A; C -= B; C ^= (B>>15);

	return C;
}

typedef WwiseDatabaseLocalizableGuidKey LocalizableGuidKey;
typedef WwiseDatabaseLocalizableIdKey LocalizableIdKey;
typedef WwiseDatabaseLocalizableNameKey LocalizableNameKey;
typedef WwiseDatabaseMediaIdKey MediaIdKey;
typedef WwiseDatabaseLocalizableGroupValueKey LocalizableGroupValueKey;
typedef WwiseDatabaseGroupValueKey GroupValueKey;
