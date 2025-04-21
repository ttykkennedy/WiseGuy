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

#include "AdapterTypes/Standard/WwiseStandardArray.h"
#include "AdapterTypes/WwiseMap.h"
#include "AdapterTypes/Standard/WwiseStandardTuple.h"

#include <map>

template <typename KeyType, typename ObjectType, typename StandardMap = std::map<KeyType, ObjectType>>
class WwiseStandardMap : public IWwiseMap<StandardMap>
{
public:
	WwiseStandardMap(){}
	
	WwiseStandardMap(const StandardMap& Map)
		: IWwiseMap<StandardMap>(Map)
	{
	}

	unsigned int Size() const override
	{
		return (unsigned int)this->Map.size();
	}

	void Empty(unsigned int ExpectedUsage = 0) override
	{
		this->Map.clear();		
	}

	void Append(const WwiseStandardMap<KeyType, ObjectType>& T)
	{
		for(auto Elem : T.Map)
		{
			this->Map.emplace(Elem.first, Elem.second);	
		}
	}

	void Add(const KeyType& Key, const ObjectType& Object)
	{
		this->Map.insert({Key, Object});
	}
	
	void Add(const KeyType& Key, ObjectType&& Object)
	{
		this->Map.try_emplace(Key, Object);
	}

	void Add(const WwiseStandardTuple<KeyType, ObjectType>& Tuple)
	{
		this->Map.insert_or_assign(Tuple.template Get<0>(), Tuple.template Get<1>());
	}
	
	void AddOrReplace(const KeyType& Key, ObjectType&& Object)
	{
		this->Map.insert_or_assign(Key, Object);
	}

	ObjectType* Find(KeyType Key)
	{
		if(this->Map.find(Key) == this->Map.end())
		{
			return nullptr;
		}
		return &this->Map.at(Key);
	}

	const ObjectType* Find(KeyType Key) const
	{
		if(this->Map.find(Key) == this->Map.end())
		{
			return nullptr;
		}
		return &this->Map.at(Key);
	}

	void GenerateValueArray(WwiseStandardArray<ObjectType>& OutArray) const
	{
		for(auto& Element : this->Map)
		{
			OutArray.Add(Element.second);
		}
	}

	bool Contains(const KeyType& Key) const
	{
		return this->Map.find(Key) != this->Map.end();
	}
	
	ObjectType FindRef(const KeyType& Key) const
	{
		auto object = this->Map.find(Key);
		if(object == this->Map.end())
		{
			return ObjectType();
		}
		return object->second;
	}

	ObjectType& FindChecked(const KeyType& Key)
	{
		return this->Map[Key];
	}
	
	ObjectType operator[](KeyType Object) const
	{
		if(!this->Map.contains(Object))
		{
			return ObjectType();
		}
		return this->Map.at(Object);
	}

	std::pair<KeyType, ObjectType> operator()(const std::pair<KeyType, ObjectType>& Object) const
	{
		return Object;
	}
};

