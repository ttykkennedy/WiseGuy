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

template <typename T>
class IWwiseGuid
{
public:
	virtual ~IWwiseGuid() = default;

	IWwiseGuid()
	{
		Guid = T();
	}
	
	IWwiseGuid(const T& InGuid)
	{
		Guid = InGuid;
	}
	
	T Guid;
	friend bool operator==(const IWwiseGuid& X, const IWwiseGuid& Y)
	{
		return X.Guid == Y.Guid;
	}

	/**
	 * Compares two GUIDs for inequality.
	 *
	 * @param X The first GUID to compare.
	 * @param Y The second GUID to compare.
	 * @return true if the GUIDs are not equal, false otherwise.
	 */
	friend bool operator!=(const IWwiseGuid& X, const IWwiseGuid& Y)
	{
		return X.Guid != Y.Guid;
	}

	/**
	 * Compares two GUIDs.
	 *
	 * @param X The first GUID to compare.
	 * @param Y The second GUID to compare.
	 * @return true if the first GUID is less than the second one.
	 */
	friend bool operator<(const IWwiseGuid& X, const IWwiseGuid& Y)
	{
		return X.Guid < Y.Guid;
	}

	virtual bool IsValid() const = 0;
};