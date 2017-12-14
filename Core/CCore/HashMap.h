#pragma once

#include <CCore/Array.h>
#include <CCore/BitField.h>



/*
FLOP based HashMap

FLOP: Filled|Last|Overflow|Present

.... = |NUL| Empty entry, no entry with this hash
FLO. = |FLO| Filled and last overflow entry of some hash
F.O. = |FO| Filled and overflow entry of some hash
...P = |P| Value present but not in the entry, which is empty (due to deletes)
F..P = |FP| Filled and value present, not overflow (so this is the value) but there are more entries (not O)
FL.P = |FLP| Filled, Present, last of the values (single, unique entry on the right spot!)
F.OP = |FOP| Filled, Present but entry is overflow. The value is out there, but not here; entry is overflow from another hash
FLOP = |FLOP| Filled and present, but the value here is actually the last entry of another hash's overflow

Invalid states and the rules they break
F... = |INV| Entry is filled, and it is not an overflow entry, but no hash with this value present? INVALID		{ F -> P|O}
.L.. = |INV| Last of overflow, but not filled in? INVALID														{ L -> O }
FL.. = |INV| Filled and Last, but no overflow and no value of this hash present INVALID							{ F -> P|O}, { L- > O }
..O. = |INV| Overflow with no other flags? INVALID																{ O -> F }
.LO. = |INV| Last Overflow, but not filled ? INVALID															{ O -> F }
.L.P = |INV| Last and present, but no overflow INVALID															{ L -> O }, { O -> F }
..OP = |INV| Present, overflow (of some other value) but not filled? INVALID									{ O -> F }
.LOP = |INV| Last overflow but not filled? INVALID																{ O -> F }

*/


/// default hash functor just calls .GetHash()
template <typename T>
struct HashFunctor
{
	uint32 operator()(const T& inObject) const
	{
		return (uint32) inObject.GetHash();
	}
};


template<class TKey, class TValue>
class HashMap
{
public:
	typedef Tuple<TKey, TValue> Entry;

	class Iterator
	{
	public:
		Iterator(HashMap<TKey, TValue>& inMap, uint inHash) : mMap(&inMap), mHash(inHash) {}
		operator Entry*() const
		{
			return &mMap->mEntries[mHash];
		}

		bool operator==(const Iterator& inOther) const			{ return mMap == inOther.mMap && mHash == inOther.mHash; }
		bool operator!=(const Iterator& inOther) const
		{
			bool b = mMap != inOther.mMap || mHash != inOther.mHash; 
			return b;
		}
		void operator++()										
		{
			mHash = mMap->mFilled.FindFirstOne(mHash+1);
			if(mHash == cMaxUint) 
				mHash = (uint)mMap->mEntries.GetLength(); 
		}
	private:
		HashMap<TKey, TValue>*		mMap	= nullptr;
		uint						mHash	= 0;
	};


	HashMap(uint inMapSize)
	{
		gAssert(gIsPowerOf2(inMapSize));
		mEntries.Resize(inMapSize);

		mFilled.Resize(inMapSize);
		mPresent.Resize(inMapSize);
		mLastEntry.Resize(inMapSize);
		mOverflow.Resize(inMapSize);
	}


	inline uint32 GetIndex(const TKey& inKey)
	{
		HashFunctor<TKey> functor;
		uint full_hash = functor(inKey);
		return full_hash & (mEntries.GetLength() - 1);
	}


	Entry* Find(const TKey& inKey)
	{
		uint32 index = GetIndex(inKey);

		if (mPresent.GetBit(index) == false)
			return nullptr;

		for (uint ovf_index = index; ovf_index != cMaxUint; ovf_index = mOverflow.FindFirstOne(ovf_index + 1, index))
		{
			if (inKey == mEntries[ovf_index].mFirst)
			{
				return &mEntries[ovf_index];
			}
			uint32 ovf_orig_index = GetIndex(mEntries[ovf_index].mFirst);
			if (ovf_orig_index == index && mLastEntry.GetBit(ovf_index))
				return nullptr;
		}

		gAssert(false); // should always have found the end of the overflow group when going through the entire map
		return nullptr;
	}


	Entry* FindOrCreate(const TKey& inKey, const TValue& inDefault)
	{
		Entry* e = Find(inKey);
		if (e == nullptr)
		{
			return Add(inKey, inDefault);
		}
		else
		{
			return e;
		}
	}


	void Remove(const TKey& inKey)
	{
		gAssert(false);
	}


	Entry* Add(const TKey& inKey, const TValue& inValue)
	{
		uint32 index = GetIndex(inKey);
		bool used = mFilled.GetBit(index);
		bool present = mPresent.GetBit(index);
		if (!used)
		{
			// not used yet! if it is present, this will never be the last entry
			// if it is not present, this becomes the only entry
			mFilled.SetBit(index, true);
			mOverflow.SetBit(index, false);
			mLastEntry.SetBit(index, !present);
			mPresent.SetBit(index, true);
		}
		else if (used && !present)
		{
			// entry used but it is some overflow entry from another hash, the hash itself is not used yet
			gAssert(mOverflow.GetBit(index));
			// loop around zero lookup
			uint free_overflow_idx = mFilled.FindFirstZero(index + 1, index);
			if (free_overflow_idx == cMaxUint)
				return nullptr; // map full!

			mPresent.SetBit(index, true);
			index = free_overflow_idx;
			mFilled.SetBit(index, true);
			mLastEntry.SetBit(index, true);
			mOverflow.SetBit(index, true);
		}
		else if (used && present)
		{
			// entry used and there are already other entries hiding in overflow somewhere (hard case!)
			// we need to do two things: find an empty spot, and find any other overflows we cross with a LastEntry flag and see if they are
			// of this hash, if so, take flag and use it for new entry

			// if the initial entry contains the last entry of a hash, and it is not an overflow entry, it is by definition the last entry of the hash we are looking for
			bool found_last_entry = mLastEntry.GetBit(index) && !mOverflow.GetBit(index);

			// first: find our target spot to add our entry
			uint free_overflow_idx = mFilled.FindFirstZero(index + 1, index);
			if (free_overflow_idx == cMaxUint)
				return nullptr; // map full
			gAssert(mFilled.GetBit(free_overflow_idx) == false);

			// second: move across all overflows with the last_entry flag and see if it is our hash
			// if we already found the last entry in the initial lookup, reset the L bit and skip search
			if (found_last_entry)
			{
				mLastEntry.SetBit(index, false);
			}
			else
			{
				// search until we arrive at our new entry or until the end if we are looping
				// we are looking for L =1;O = 1 entries; we iterate over the most sparse one (overflow)
				for (uint overflow = index + 1; overflow != cMaxUint; overflow = mOverflow.FindFirstOne(overflow + 1, free_overflow_idx))
				{
					if (mLastEntry.GetBit(overflow))
					{
						uint32 ovf_index = GetIndex(mEntries[overflow].mFirst);
						if (ovf_index == index) // hash is the same!
						{
							mLastEntry.SetBit(overflow, false);
							found_last_entry = true;
							break;
						}
					}
				}
			}

			mPresent.SetBit(index, true);
			index = free_overflow_idx;
			gAssert(mFilled.GetBit(index) == false);
			mFilled.SetBit(index, true);
			mLastEntry.SetBit(index, found_last_entry);
			mOverflow.SetBit(index, true);
		}

		mEntries[index].mFirst = inKey;
		mEntries[index].mSecond = inValue;
		return &mEntries[index];
	}

	Iterator begin()	{ return Iterator(*this, mFilled.FindFirstOne(0)); }
	Iterator end()		{ return Iterator(*this, (uint) mEntries.GetLength()); }

private:

	BitField		mFilled;				// Fn == 1 means that mEntries[n] is used
	BitField		mLastEntry;				// Ln == 1 means that all entries[k] where hash(key[k]) == hash(key[n]) between hash(key[n]) and [n]
	BitField		mOverflow;				// On == 1 means that entries[n] holds an overflow entry, not a direct entry
	BitField		mPresent;				// Pn == 1 means that at least one hash value [n] is in the map
	Array<Entry>	mEntries;
};



