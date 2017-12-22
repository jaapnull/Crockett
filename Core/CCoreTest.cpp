
#include <CCore/Integers.h>
#include <CCore/Bitfield.h>
#include <CCore/HashMap.h>
#include <CCore/String.h>
#include <CUtils/StringUtils.h>
#include <CMath/Math.h>

template<typename T>
bool sTest(const String& inTestName, const T& inValue, const T& inExpected)
{
	if (inValue != inExpected)
	{
		std::cout << "Test Failed! Name: " << inTestName << "; Got: " << inValue << ", Expected: " << inExpected << std::endl;
		gAssert(inValue == inExpected);
		return false;
	}
	return true;
}


uint gHashMapTest()
{

	const uint cMapSize = 512;

	HashMap<String, int> map(cMapSize);

	for (int loop = 1; loop < 32; loop++)
	{
		gAssert(map.GetFilledEntryCount() == 0);
		for (int c = 0; c < cMapSize; c++)
		{
			if ((c % loop) == 0) // don't add every nth value
				continue;

			String s = gToString(c);
			s.Append("_testkey");
			map.Add(s, map.GetIndex(s));
			gAssert(map.Find(s) != nullptr);
		}


		for (int c = 0; c < cMapSize; c++)
		{
			String s = gToString(c);
			s.Append("_testkey");
			HashMap<String, int>::Entry* e = map.Find(s);
			if ((c % loop) == 0)
			{
				gAssert(e == nullptr);
			}
			else
			{
				gAssert(e != nullptr);
				gAssert(e->mSecond == map.GetIndex(s));
			}
		}


		// removing itmes
		for (int c = 0; c < cMapSize; c++)
		{
			String s = gToString(c);
			s.Append("_testkey");
			HashFunctor<String> hf;
			int hash = hf(s);
			HashMap<String, int>::Entry* e = map.Find(s);
			if ((c % loop) == 0)
			{
				gAssert(e == nullptr);
				bool was_there = map.Remove(s);
				gAssert(!was_there);
				gAssert(!map.Find(s));
			}
			else
			{
				gAssert(e != nullptr);
				bool was_there = map.Remove(s);
				gAssert(was_there);
				gAssert(!map.Find(s));
			}
		}
	}
	return 0;
}


void gBitRangeTest()
{
	for (int x = 0; x < 1000; x++)
	{
		uint64 c = gRand64();
		int highest_bit = gRandRange(0, 63);

		c >>= (63 - highest_bit);
		c |= (1ll << highest_bit);

		gAssert(highest_bit == gGetHighestBitSet(c));
	}


	for (int x = 0; x < 1000; x++)
	{
		uint64 c = gRand64();
		int lowest_bit = gRandRange(0, 63);

		c <<= lowest_bit;
		c |= (1ll << lowest_bit);

		gAssert(lowest_bit == gGetLowestBitSet(c));
	}

	BitField bf;
	bf.Resize(123);

	gAssert(bf.FindFirstOne(0) == cMaxUInt);


	for (int x = 0; x < 1000; x++)
	{
		bf.Clear();
		uint first_bit = gRandRange(0, bf.GetBitCount() - 1);
		uint first_test = gRandRange(0, bf.GetBitCount() - 1);

		bf.SetBit(first_bit, true);
		for (uint b = first_bit + 1; b < bf.GetBitCount(); b++)
		{
			bf.SetBit(b, true);
		}

		for (uint b = 0; b < bf.GetBitCount(); b++)
		{
			gAssert(bf.GetBit(b) == (b >= first_bit));
		}

		sTest("FindFirstOne test", bf.FindFirstOne(first_test), gMax(first_bit, first_test));
		bf.Invert();
		sTest("FindFirstZero test", bf.FindFirstZero(first_test), gMax(first_bit, first_test));

		for (uint b = 0; b < bf.GetBitCount(); b++)
		{
			gAssert(bf.GetBit(b) != (b >= first_bit));
		}
	}
}


void gCoreUnitTest()
{

	gBitRangeTest();
	gHashMapTest();
}



