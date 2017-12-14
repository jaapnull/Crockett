#include <CCore/Types.h>
#include <CMath/Math.h>
#include <CCore/String.h>
#include <CCore/Frame.h>
#include <CFile/File.h>
#include <CUtils/Sort.h>
#include <CUtils/StreamUtils.h>

int gAdvent1a()
{
	const char* c = "494751136895345894732582362629576539599184296195318162664695189393364372585778868512194863927652788149779748657989318645936221887731542718562643272683862627537378624843614831337441659741281289638765171452576466381314558821636595394981788588673443769343597851883955668818165723174939893841654914556681324133667446412138511724424292394454166623639872425168644336248217213826339741267546823779383343362789527461579565822966859349777937921933694912369552152772735167832762563719664315456987186713541153781499646178238762644186484381142249926194743713139262596264878458636595896487362658672224346241358667234115974528626523648311919886566497837217169673923935143386823757293148719377821517314629812886912412829924484513493885672343964151252433622341141661523814465991516961684511941471572895453711624986269342398786175846925783918686856442684489873327497698963658862856336682422797551251489126661954848572297228765445646745256499679451426358865477844467458533962981852292513358871483321161973583245698763531598395467675529181496911117769834127516441369261275244225978893617456524385518493112272169767775861256649728253754964675812534546226295535939697352141217337346738553495616832783757866928174519145357234834584788253893618549484385733283627199445369658339175644484859385884574943219267922729967571943843794565736975716174727852348441254492886794362934343868643337828637454277582276962353246357835493338372219824371517526474283541714897994127864461433627894831268659336264234436872715374727211764167739169341999573855627775114848275268739159272518673316753672995297888734844388928439859359992475637439771269232916542385876779616695129412366735112593669719335783511355773814685491876721452994714318863716542473187246351548626157775143333161422867924437526253865859969947366972895674966845993244925218766937543487875485647329995285821739359369998935331986126873726737672159265827566443794515755939813676194755474477224152139987944419463371386499841415227734673733555261543871359797796529847861748979527579985757964742667473767269248335229836818297477665453189662485548925521497365877771665365728224394427883312135322325169141784";
	int total = 0;
	for (int i = 0; c[i] != 0; i++)
		if (c[i] == (c[i + 1] == 0 ? c[0] : c[i + 1]))
			total += c[i] - '0';
	return total;
}

int gAdvent1b()
{
	String s = "494751136895345894732582362629576539599184296195318162664695189393364372585778868512194863927652788149779748657989318645936221887731542718562643272683862627537378624843614831337441659741281289638765171452576466381314558821636595394981788588673443769343597851883955668818165723174939893841654914556681324133667446412138511724424292394454166623639872425168644336248217213826339741267546823779383343362789527461579565822966859349777937921933694912369552152772735167832762563719664315456987186713541153781499646178238762644186484381142249926194743713139262596264878458636595896487362658672224346241358667234115974528626523648311919886566497837217169673923935143386823757293148719377821517314629812886912412829924484513493885672343964151252433622341141661523814465991516961684511941471572895453711624986269342398786175846925783918686856442684489873327497698963658862856336682422797551251489126661954848572297228765445646745256499679451426358865477844467458533962981852292513358871483321161973583245698763531598395467675529181496911117769834127516441369261275244225978893617456524385518493112272169767775861256649728253754964675812534546226295535939697352141217337346738553495616832783757866928174519145357234834584788253893618549484385733283627199445369658339175644484859385884574943219267922729967571943843794565736975716174727852348441254492886794362934343868643337828637454277582276962353246357835493338372219824371517526474283541714897994127864461433627894831268659336264234436872715374727211764167739169341999573855627775114848275268739159272518673316753672995297888734844388928439859359992475637439771269232916542385876779616695129412366735112593669719335783511355773814685491876721452994714318863716542473187246351548626157775143333161422867924437526253865859969947366972895674966845993244925218766937543487875485647329995285821739359369998935331986126873726737672159265827566443794515755939813676194755474477224152139987944419463371386499841415227734673733555261543871359797796529847861748979527579985757964742667473767269248335229836818297477665453189662485548925521497365877771665365728224394427883312135322325169141784";
	uint l = (uint32) s.GetLength();
	int total = 0;
	for (uint i0 = 0; i0 < l; i0++)
	{
		uint i1 = (i0 + (l / 2))%l;
		if (s[i0] == s[i1])
			total += s[i0] - '0';
	}
	return total;
}


int gAdvent2a()
{
	File f;
	f.Open(".\\advent_2_input.txt", fomRead);
	uint64 l = f.GetLength();
	String s;
	s.Resize(l);
	f.GetBytes(s.GetData(), l);

	String z;
	int max = 0x80000000;
	int min = 0x7FFFFFFF;
	int checksum = 0;
	for (char c : s)
	{
		if (c == '\t' || c == '\n')
		{
			int i = gStringToInt(z);
			max = gMax<int>(i, max);
			min = gMin<int>(i, min);
			std::cout << "new " << z << " min: " << min << " max: " << max << std::endl;
			z.Clear();
		}
		if (c == '\n')
		{
			checksum += (max - min);
			std::cout << "line min " << min << " max " << max << " checksum " << checksum << std::endl;

			max = 0x80000000;
			min = 0x7FFFFFFF;
			z.Clear();
			continue;
		}
		if (c == '\r' || c == '\t' || c == '\n')
			continue;
		z.Append(c);
		
	}
	return checksum;
}



int gAdvent2b()
{
	File f;
	f.Open(".\\advent_2_input.txt", fomRead);
	uint64 l = f.GetLength();
	String s;
	s.Resize(l);
	f.GetBytes(s.GetData(), l);

	String z;
	Array<int> integers;
	int min = 0x7FFFFFFF;
	int checksum = 0;
	for (char c : s)
	{
		if (c == '\t' || c == '\n')
		{
			int i = gStringToInt(z);
			integers.Append(i);
			z.Clear();
		}
		if (c == '\n')
		{
			gSort(integers.begin(), integers.end());
			bool found = false;
			for (int i0 = 0; i0 < integers.GetLength(); i0++)
			{
				for (int i1 = i0 + 1; i1 < integers.GetLength(); i1++)
				{
					gAssert(integers[i0] <= integers[i1]);

					if ((integers[i1]/integers[i0]) * integers[i0] == integers[i1])
					{
						std::cout << "found " << integers[i0] << " and " << integers[i1] << ", divisor: " << float(integers[i1]) / float(integers[i0]) << std::endl;
						checksum += (integers[i1] / integers[i0]);
						gAssert(found == false);
						found = true;
					}
				}
			}
			gAssert(found == true);
			integers.Clear();
			continue;
		}
		if (c == '\r' || c == '\t' || c == '\n')
			continue;
		z.Append(c);

	}
	return checksum;
}



int gAdvent3a()
{
	int x = 0;
	int y = 0;
	int target = 312051;
	int steps = 1;
	int dir = 0;
	int size = 1;
	int z = 1;
	do
	{
		if (steps == 0)
		{
			dir = (dir + 1)&3;
			if (dir == 2 || dir == 0) size++;
			steps = size;
		}
		if (dir == 0) { x++; }
		if (dir == 1) { y++; }
		if (dir == 2) { x--; }
		if (dir == 3) { y--; }
		steps--;
		z++;
	} while (z != target);

	std::cout << "X : " << x << " Y : " << y << std::endl;
	return gAbs(x) + gAbs(y);
}


int gAdvent3b() 
{
	DataFrame<int> map;
	map.Resize(400, 400);
	map.SetAll(0);

	int x = 200;
	int y = 200;
	int target = 312051;
	int steps = 1;
	int dir = 0;
	int size = 1;
	int z = 1;
	int sum = 0;
	map.Set(x, y, 1);
	do
	{
		if (steps == 0)
		{
			dir = (dir + 1) & 3;
			if (dir == 2 || dir == 0) size++;
			steps = size;
		}
		if (dir == 0) { x++; }
		if (dir == 1) { y++; }
		if (dir == 2) { x--; }
		if (dir == 3) { y--; }
		steps--;
		
		sum = map.Get(x, y);
		sum += map.Get(x+1, y  );
		sum += map.Get(x-1, y  );
		sum += map.Get(x  , y-1);
		sum += map.Get(x+1, y-1);
		sum += map.Get(x-1, y-1);
		sum += map.Get(x  , y+1);
		sum += map.Get(x+1, y+1);
		sum += map.Get(x-1, y+1);
		map.Set(x, y, sum);
		

		std::cout << "X : " << x << " Y : " << y << " HAS " << sum << std::endl;
	} while (sum < target);

	std::cout << "X : " << x << " Y : " << y << std::endl;
	return gAbs(x) + gAbs(y);

}





int gAdvent4()
{
	File f;
	f.Open(".\\advent_4_input.txt", fomRead);

	LineReader line_reader(f);
	String line;
	int valid_count = 0;
	int line_no = 0;
	while (line_reader.ReadLine(line))
	{
		line_no++;
		Array<String> passwords;
		gExplodeString(passwords, line, String(" "));
		
		// ADDED FOR 4B
		for (String& s : passwords)
		{
			gSort(s.begin(), s.end());
		}
		// END 4B

		struct { bool operator()(const String& inLeft, const String& inRight) { return gOrderString(inLeft, inRight) < 0; } } smf;
		gSort(passwords.begin(), passwords.end(), smf);



		bool is_valid = true;
		for (int i = 1; i < passwords.GetLength(); i++)
		{
			if (passwords[i] == passwords[i - 1])
			{
				std::cout << "INVALID ON LINE " << line_no << " : " << passwords[i] << std::endl;
				is_valid = false;
			}
		}
		if (is_valid)
			valid_count++;

	}
	return valid_count;
}


int gAdvent5()
{
	File f;
	f.Open(".\\advent_5_input.txt", fomRead);

	LineReader line_reader(f);
	String line;
	int valid_count = 0;
	int line_no = 0;

	Array<int> instructions;

	while (line_reader.ReadLine(line))
	{
		instructions.Append(gStringToInt(line));
	}

	int ip = 0;
	int steps = 0;
	do
	{
		int next_ip = ip + instructions[ip];

		// PUT IN FOR B
		if (instructions[ip] >= 3)
			instructions[ip]--;
		else
		// END FOR B
			instructions[ip]++;
		ip = next_ip;
		steps++;
	}
	while (ip > 0 && ip < instructions.GetLength());

	return steps;
}



int gAdvent6()
{


	struct State
	{
		bool operator==(const State& s) const
		{
			for (int c = 0; c < 16; c++)
				if (s.mBlock[c] != mBlock[c])
					return false;
			return true;
		}
		bool operator!=(const State& s) const
		{
			return !operator==(s);
		}

		State(uint8 inBlocks[16])
		{
			for (int c = 0; c < 16; c++)
				mBlock[c] = inBlocks[c];
		}
		State() {}

		uint8 mBlock[16];
	};

	uint8 s[16] = { 0,5,10,0,11,14,13,4,11,8,8,7,1,4,12,11 };
	State current_state(s);
	int cycle = 0;
	Array<State> found_states;

	uint64 f = 0;
	found_states.Append(current_state);
	do
	{
		// find smallest entry
		uint max_entry = 0;
		int max = -1;
		for (int n = 0; n < 16; n++)
		{
			int entry = (int) current_state.mBlock[n];
			if (entry > max) { max_entry = n; max = entry; }
		}
		
		current_state.mBlock[max_entry] = 0;
		for (int n = 0; n < max; n++)
		{
			max_entry++;
			current_state.mBlock[max_entry % 16]++;
		}
		f = found_states.Find(current_state);
		found_states.Append(current_state);
		cycle++;
		
		std::cout << "state : " << cycle << '\t' <<
			(int)current_state.mBlock[0] << '\t' <<
			(int)current_state.mBlock[1] << '\t' <<
			(int)current_state.mBlock[2] << '\t' <<
			(int)current_state.mBlock[3] << '\t' <<
			(int)current_state.mBlock[4] << '\t' <<
			(int)current_state.mBlock[5] << '\t' <<
			(int)current_state.mBlock[6] << '\t' <<
			(int)current_state.mBlock[7] << '\t' <<
			(int)current_state.mBlock[8] << '\t' <<
			(int)current_state.mBlock[9] << '\t' <<
			(int)current_state.mBlock[10] << '\t' <<
			(int)current_state.mBlock[11] << '\t' <<
			(int)current_state.mBlock[12] << '\t' <<
			(int)current_state.mBlock[13] << '\t' <<
			(int)current_state.mBlock[14] << '\t' <<
			(int)current_state.mBlock[15] << std::endl;

	} while (f == cMaxSize64);
	
	return 0;
}




int gAdvent7()
{
	struct Entry
	{
		String			mName;
		Array<Entry*>	mChildren;
		uint			mWeight			= 0;
		Entry*			mParent			= nullptr;

		uint			CheckWeights() const
		{
			uint w = mWeight;
			if (mChildren.IsEmpty())
				return w;

			uint cw0 = mChildren[0]->CheckWeights();
			uint cw = cw0;
			for (int i = 1; i < mChildren.GetLength(); i++)
			{
				Entry* e = mChildren[i];
				uint cwn = e->CheckWeights();
				if (cwn != cw0)
				{
					std::cout << mName << " has weird children; " << e->mName << " has weight " << cwn << ", " << std::endl;
				}
				cw += cwn;
			}
			std::cout << mName << " has weight " << mWeight << " + " << cw << " = " << w + cw << std::endl;
			return w + cw;

		}

		bool operator==(const Entry& inEntry) const
		{
			return mName == inEntry.mName; 
		}
	};

	Array<Entry> entries;
	entries.Reserve(100000);

	File f;
	f.Open(".\\advent_7_input.txt", fomRead);
	LineReader line_reader(f);
	String line;

	while (line_reader.ReadLine(line))
	{
		Array<String> parts;
		gExplodeString(parts, line, String("() ,->"));
		Entry e;
		e.mName = parts[0];
		size64 idx = entries.Find(e);
		if (idx == cMaxSize64)
		{
			entries.Append(e);
			idx = entries.GetLastIndex();
		}
		gAssert(entries[idx].mChildren.IsEmpty());
		gAssert(entries[idx].mWeight == 0);
		entries[idx].mWeight = gStringToUInt(parts[1]);
		for (uint c = 2; c < parts.GetLength(); c++)
		{
			Entry ce;
			ce.mName = parts[c];
			size64 cidx = entries.Find(ce);
			if (cidx == cMaxSize64)
			{
				entries.Append(ce);
				cidx = entries.GetLastIndex();
			}
			entries[cidx].mParent = &entries[idx];
			entries[idx].mChildren.Append(&entries[cidx]);
		}
	}


	for (Entry& e : entries)
	{
		if (e.mParent == nullptr)
		{
			uint v = e.CheckWeights();
			std::cout << e.mName << " <<<< root : " << v << std::endl;
			
		}
	}

	return 0;
}





#include <CCore/HashMap.h>

int gAdvent8()
{
	File f;
	f.Open(".\\advent_8_input.txt", fomRead);
	LineReader line_reader(f);
	String line;
	HashMap<String, int> variables(1024);
	int all_time_max = cMinInt;
	while (line_reader.ReadLine(line))
	{
		Array<String> line_bits;
		gExplodeString(line_bits, line, String(" "));
		HashMap<String, int>::Entry* assignment_var		= variables.FindOrCreate(line_bits[0], 0);
		bool is_inc										= line_bits[1] == String("inc");
		int delta_val									= gStringToInt(line_bits[2]);
		gAssert(line_bits[3] == "if");
		HashMap<String, int>::Entry* test_var			= variables.FindOrCreate(line_bits[4], 0);
		String comperator								= line_bits[5];
		int test_val									= gStringToInt(line_bits[6]);
		
		bool test_result = false;
		if		(comperator == ">" )	{ test_result = (test_var->mSecond >  test_val); }
		else if (comperator == "<" )	{ test_result = (test_var->mSecond <  test_val); }
		else if (comperator == "==")	{ test_result = (test_var->mSecond == test_val); }
		else if (comperator == ">=")	{ test_result = (test_var->mSecond >= test_val); }
		else if (comperator == "<=")	{ test_result = (test_var->mSecond <= test_val); }
		else if (comperator == "!=")	{ test_result = (test_var->mSecond != test_val); }
		else { gAssert(false); }
		if (test_result)
		{
			assignment_var->mSecond += (is_inc ? delta_val : -delta_val);	
			all_time_max = gMax(all_time_max, assignment_var->mSecond);
		}
	}
	int max_value = cMinInt;
	for (HashMap<String, int>::Entry it : variables)
		max_value = gMax(it.mSecond, max_value);

	// return max_value;		A
	return all_time_max;	//	B
}


int gAdvent9()
{
	File f;
	f.Open(".\\advent_9_input.txt", fomRead);
	
	char c;
	int indent = 0;
	int score = 0;
	bool intrash = 0;
	int trash = 0;
	while (f.GetBytes(&c, 1))
	{
		
		if (c == '>' && intrash)	{ intrash = false; }
		if (c == '!' && intrash)	{ f.GetBytes(&c, 1); continue; }

		if (intrash) trash++;

		if (c == '<' && !intrash)	{ intrash = true; }		
		if (c == '{' && !intrash)	{ indent++; }
		if (c == '}' && !intrash)	{ score += indent;indent--; }
	}
	gAssert(indent == 0);
	return trash; // A = score
}





template <uint TSize>
struct KnotHash
{
	KnotHash()
	{ 
		for (int c = 0; c < TSize; c++) 
			mKnots[c] = c; 
	}

	void Shift(uint inShift)
	{
		inShift %= TSize;
		Array<int> k;
		for (uint i = 0; i < inShift; i++)			k.Append(mKnots[i]);
		for (uint i = 0; i < TSize - inShift; i++)	mKnots[i] = mKnots[i + inShift];
		for (uint i = 0; i < inShift; i++)			mKnots[i + TSize - inShift] = k[i];
		mShift += inShift;
		mShift %= TSize;
	}

	void HashString(const String& inString)
	{
		String z = inString;
		z.Append(String("\x11\x1F\x49\x2f\x17"));
		int skip = 0;
		int cur = 0;
		for (int rounds = 0; rounds < 64; rounds++)
			for (int c = 0; c < z.GetLength(); c++)
			{
				Swap(cur, (uchar)z[c]);
				cur += skip + (uchar)z[c];
				skip++;
			}
	}

	void Swap(uint inOffset, uint inLength)
	{
		for (uint c = 0; c < inLength / 2; c++)
			gSwap(mKnots[(c+ inOffset)%TSize], mKnots[(inLength - 1 - c+ inOffset) % TSize]);
	}

	void Swap(uint inLength)
	{
		for (uint c = 0; c < inLength / 2; c++)
			gSwap(mKnots[c], mKnots[inLength - 1 - c]);
	}

	void Print()
	{
		for (int c = 0; c < TSize; c++)
		{
			std::cout << '[' << std::hex << (int) mKnots[c] << ']';
		}
		std::cout << ">> " << mShift << std::endl;
	}

	void PrintDenseHash()
	{
		for (int s = 0; s < 16; s++)
		{
			int dense_hash = 0;
			for (int d = 0; d < 16; d++)
				dense_hash ^= mKnots[d + s * 16];
			std::cout << std::hex << dense_hash;
		}
	}


	void Straighten()
	{
		Shift(TSize - mShift);
	}

	int		mShift = 0;
	int		mKnots[TSize];
};

int gAdvent10()
{
	//"\x17\x31\x73\x47\x23"
	String data("83,0,193,1,254,237,187,40,88,27,2,255,149,29,42,100\x11\x1F\x49\x2f\x17");// { 83, 0, 193, 1, 254, 237, 187, 40, 88, 27, 2, 255, 149, 29, 42, 100 };
	KnotHash<256> r;
	r.Print();

	int skip = 0;
	for (int rounds = 0; rounds < 64; rounds++)
	for (int c = 0; c < data.GetLength(); c++)
	{
		r.Swap((uchar) data[c]);
		r.Shift(skip + (uchar)data[c]);
		skip++;
	}
	r.Straighten();

	for (int s = 0; s < 16; s++)
	{
		int dense_hash = 0;
		for (int d = 0; d < 16; d++)
			dense_hash ^= r.mKnots[d + s * 16];
		std::cout << std::hex << dense_hash;
	}
	std::cout << std::endl;


	//String data("83,0,193,1,254,237,187,40,88,27,2,255,149,29,42,100\x11\x1F\x49\x2f\x17");// { 83, 0, 193, 1, 254, 237, 187, 40, 88, 27, 2, 255, 149, 29, 42, 100 };
	String data2("83,0,193,1,254,237,187,40,88,27,2,255,149,29,42,100");
	KnotHash<256> r2;
	r2.HashString(data2);
//
//	skip = 0;
//	int cur = 0;
//	for (int rounds = 0; rounds < 64; rounds++)
//		for (int c = 0; c < data.GetLength(); c++)
//		{
//			r2.Swap(cur, (uchar)data[c]);
//			//r.Shift(skip + (uchar)data[c]);
//			cur += skip + (uchar)data[c];
//			skip++;
//
//		}
	//r.Straighten();

	for (int s = 0; s < 16; s++)
	{
		int dense_hash = 0;
		for (int d = 0; d < 16; d++)
			dense_hash ^= r2.mKnots[d + s * 16];
		std::cout << std::hex << dense_hash;
	}
	std::cout << std::endl;


	return 0;
}



int gAdvent11()
{

	/*
	
	  \ n  /
	nw +--+ ne
	  /    \
	-+      +-
	  \    /
	sw +--+ se
	  / s  \

	*/

	File f;
	f.Open(".\\advent_11_input.txt", fomRead);
	size64 len = f.GetLength();
	String str = "se,sw,se,sw,sw";
	
	str. Resize(len); f.GetBytes(str.GetData(), len);

	Array<String> directions; gExplodeString(directions, str, String(",\r\n"));
	
	ivec2 pos(0, 0);

	int max = 0;

	for (String d : directions)
	{
		if (d == "s")			{ pos.y += 2; }
		else if (d == "n")		{ pos.y -= 2; }
		else if (d == "ne")		{ pos.y -= 1; pos.x += 1; }
		else if (d == "nw") { pos.y -= 1; pos.x -= 1; }
		else if (d == "se") { pos.y += 1; pos.x += 1; }
		else if (d == "sw") { pos.y += 1; pos.x -= 1; }
		else gAssert(false);

		int steps = 0;
		int x = gAbs(pos.x);
		int y = gAbs(pos.y);
		if (x > y)
		{
			steps = x;
		}
		else
		{
			int hrz = x;
			int vert_todo = y - hrz;
			steps = hrz + vert_todo / 2;
		}
		max = gMax(max, steps);
	}




	return max; //steps = A
}



int gAdvent12()
{
	File f;
	f.Open(".\\advent_12_input.txt", fomRead);
	LineReader reader(f);
	Array < Array<int> > entries;
	entries.Resize(2000);

	String line;
	while (reader.ReadLine(line))
	{
		Array<String> parts;
		gExplodeString(parts, line, String("<-> ,"));
		int idx = gStringToInt(parts[0]);
		for (int n = 1; n < parts.GetLength(); n++)
			entries[idx].Append(gStringToInt(parts[n]));
	}

	BitField visited;
	visited.Resize(2000);

	int g = 0;
	for (uint first = 0; first != cMaxUint; first = visited.FindFirstZero(first + 1))
	{
		std::cout << "Group " << g << " starts with " << first << std::endl;
		Array<int> stack;
		stack.Append(first);
		for (int c = 0; c < stack.GetLength(); c++)
		{
			gAssert(visited[stack[c]] == false);
			visited.SetBit(stack[c], true);
			for (int e : entries[stack[c]])
			{
				if (visited[e] == false)
				{
					stack.Append(e);
				}
			}
		}
		std::cout << "Group " << g << " had " << stack.GetLength() << " entries " << std::endl;
		g++;
	}
	return g; // <- answ for B; A = length of group 0
}




int gAdvent13()
{
	File f;
	f.Open(".\\advent_13_input.txt", fomRead);
	LineReader reader(f);
	String line;

	typedef Tuple<int, int> Gate;
	Array<Gate> gates;
	while (reader.ReadLine(line))
	{
		Array<String> parts;
		gExplodeString(parts, line, String(": "));
		gates.Append(Gate(gStringToInt(parts[0]), gStringToInt(parts[1])));
	}

	int level = 0;
	int delay = 0;
	bool caught = 0;
	do
	{
		caught = false;
		level = 0;
		delay++;
		for (Gate g : gates)
		{
			int cycle = (g.mSecond * 2 - 2);
			if ((g.mFirst + delay) % cycle == 0)
			{
				level += g.mSecond * g.mFirst;
				caught = true;
			}
		}
	} while (caught);

	return delay; // delay for B, level for A
}


int gAdvent14()
{
	KnotHash<256> knot;
	knot.HashString("flqrgnkx-0");

	knot.PrintDenseHash();

	return 0;
}



