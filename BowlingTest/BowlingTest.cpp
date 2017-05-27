#include <CCore/String.h>
#include <CMath/Vector.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CMath/Math.h>
#include <unordered_map>

#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>
#include <WCommon/Font.h>
#include <WCommon/Mouse.h>
#include <WCommon/Keyboard.h>


#ifdef WIN32_IS_INCLUDED
#error Windows Header Slip
#endif

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}


struct State
{
	static const int cMaxStateSize = 9;
	int mCounts[cMaxStateSize]	= {0};
	
	bool operator==(const State& inState) const 
	{
		for (int i = 0; i < cMaxStateSize; i++)
			if (mCounts[i] != inState.mCounts[i]) 
				return false;
		return true;
	}

	const String ToString() const
	{
		String s;
		for (int i = 0; i < cMaxStateSize; i++)
		{
			if (mCounts[i] > 0)
			{
				String v(mCounts[i], '1'+i);
				s.Append(v);
			}
		}
		if (s.IsEmpty())
			s.Append('0');  
		return s;
	}

	size_t GetHash() const
	{
		size_t seed = 0;
		for (int i = 0; i < cMaxStateSize; i++)
		{
			hash_combine<int>(seed, mCounts[i]);
		}
		return seed;
	}
};

template<>
struct std::hash<State>
{
	std::size_t operator()(const State& k) const
	{
		return k.GetHash();
	}
};

std::unordered_map<State, bool> results;


void sCreateReductionSet(Array<State>& outStates, const State& inState)
{
	// per group size, we create all reduction sets
	for (int v = 0; v < State::cMaxStateSize; v++)
	{
		if (inState.mCounts[v] > 0)
		{
			int size = v+1; // size of the group
			if (size == 1) // for the size 0 group, there is one reduction, removing the group
			{
				State new_state = inState;
				new_state.mCounts[0]--; // size1 group reduction
				outStates.Append(new_state);
			}
			else
			{
				int range_1 =((size-1) / 2);
				int range_2 =((size-2) / 2);
				// do the one-cut
				for (int i = 0; i <= range_1; i++)
				{
					State new_state = inState;
					int left_size = i;
					int right_size =  (size-i-1);
					new_state.mCounts[v]--;
					if (left_size > 0) new_state.mCounts[left_size-1]++;
					if (right_size > 0) new_state.mCounts[right_size-1]++;
					outStates.Append(new_state);
				}

				// do the two-cut
				for (int i = 0; i <= range_2; i++)
				{
					State new_state = inState;
					int left_size = i;
					int right_size =  (size-i-2);
					new_state.mCounts[v]--;
					if (left_size > 0) new_state.mCounts[left_size-1]++;
					if (right_size > 0) new_state.mCounts[right_size-1]++;
					outStates.Append(new_state);
				}
			}
		}
	}
}


State sStateFromIntInput(uint32 inInput)
{
	State s;
	uint32 value = inInput;
	for (int v = 0; v < State::cMaxStateSize; v++)
	{
		int digit = value%10;
		if (digit > 0)
		{
			s.mCounts[digit-1]++;
		}
		value /= 10;
	}
	return s;
}

static uint32 sEvaluateStateRuns = 0;

bool sEvaluateState(const State& inState)
{
	sEvaluateStateRuns++;
	std::unordered_map<State, bool>::iterator i = results.find(inState);
	if (i != results.end())
	{
		return i->second;
	}

	Array<State> reductions;
	sCreateReductionSet(reductions, inState);

	String output = inState.ToString();
	output.Append("->");
	bool winnable = false;
	bool first = true;
	for (const State& r : reductions)
	{
		bool w = sEvaluateState(r);
		winnable |= (!w);

		if (!first) output.Append(' ');
		first = false;
		output.Append(r.ToString());
		output.Append(w ? 'W' : 'L');
		if (winnable) 
			break;
	}
	output.Append("->");
	output.Append(winnable ? 'W' : 'L');
	//std::cout << output << std::endl;
	results.insert(std::pair<State, bool>(inState, winnable));
	return winnable;
}


int main()
{
	State s;
	results.insert(std::pair<State, bool>(s, false));
	uint32 value = 0;

	do
	{
		do
		{
			std::cin.clear();
			std::cout << "S>";
			std::cin >> value;
			std::cin.ignore(uint64(-1), '\n');
		}
		while (std::cin.fail());

		State begin_state = sStateFromIntInput(value);
		bool w = sEvaluateState(begin_state);
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << begin_state.ToString() << "->" << (w ? 'W' : 'L') << std::endl;
		std::cout << "cached " << results.size() << " entries" << std::endl;
		std::cout << "evaulated  " << sEvaluateStateRuns << " states" << std::endl;
		std::cout << "---------------------------------------------"<< std::endl;
	} while(value != 0);

	return 0;
}

