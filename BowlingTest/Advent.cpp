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
	DataFrame<int> map(400,400, 0);
	ivec2 pos(200, 200);
	map.Set(pos, 1);
	int steps = 1; int dir = 0; int size = 1; int sum = 0;
	while (sum <= 312051)
	{
		if (steps == 0) // turning corner
		{
			dir = (dir + 1) & 3;
			if (dir == 2 || dir == 0) size++;
			steps = size;
		}
		if (dir == 0) { pos.x++; }
		if (dir == 1) { pos.y++; }
		if (dir == 2) { pos.x--; }
		if (dir == 3) { pos.y--; }
		steps--;
		
		sum = 0;
		for (int x = -1; x <= 1; x++)
			for (int y = -1; y <= 1; y++)
				sum += map.Get(pos.x+x, pos.y+y);
		map.Set(pos, sum);
	}
	return sum;
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
	File f; f.Open(".\\advent_9_input.txt", fomRead);
	char c; int indent = 0; int score = 0; bool intrash = 0; int trash = 0;
	while (f.GetBytes(&c, 1))
	{
		if (c == '>' && intrash)	{ intrash = false; }
		if (c == '!' && intrash)	{ f.GetBytes(&c, 1); continue; }
		if (intrash) trash++;
		if (c == '<' && !intrash)	{ intrash = true; }		
		if (c == '{' && !intrash)	{ indent++; }
		if (c == '}' && !intrash)	{ score += indent;indent--; }
	}
	return trash; // A = return score
}





template <uint TSize>
struct KnotHash
{
	KnotHash()							{ for (uint c = 0; c < TSize; c++) mData[c] = c; }
	void Swap(uint inO, uint inL)		{ for (uint c = 0; c < inL / 2; c++) gSwap(mData[(c + inO) % TSize], mData[(inL - 1 - c + inO) % TSize]); }
	void GetDenseHash(uint8* outHash)	{ for (uint s = 0; s < TSize / 16; s++) { outHash[s] = 0; for (int d = 0; d < 16; d++) outHash[s] ^= mData[d + s * 16]; } }

	void HashString(const String& inString, uint inRounds)
	{
		String z = inString; int skip = 0; int cur = 0;
		z.Append(String("\x11\x1F\x49\x2f\x17"));
		for (uint rounds = 0; rounds < inRounds; rounds++)
			for (uint c = 0; c < z.GetLength(); c++)
			{
				Swap(cur, (uchar)z[c]);
				cur += (skip++) + (uchar)z[c];
			}
	}
	uint8	mData[TSize];
};

int gAdvent10()
{
	String data2("83,0,193,1,254,237,187,40,88,27,2,255,149,29,42,100");
	KnotHash<256> r;
	r.HashString(data2, 64);
	uint8 dense_hash[16];
	r.GetDenseHash(dense_hash);
	for (uint s = 0; s < 16; s++) std::cout << std::hex << (uint)dense_hash[s];
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
	File f; f.Open(".\\advent_12_input.txt", fomRead);
	Array < Array<int> > entries; entries.Resize(2000);
	LineReader reader(f); String line;
	while (reader.ReadLine(line))
	{
		Array<String> parts; gExplodeString(parts, line, String("<-> ,"));
		int idx = gStringToInt(parts[0]);
		for (int n = 1; n < parts.GetLength(); n++)
			entries[idx].Append(gStringToInt(parts[n]));
	}

	BitField visited; visited.Resize(2000);
	int group_count = 0;
	for (uint first = 0; first != cMaxUInt; first = visited.FindFirstZero(first + 1))
	{
		group_count++;
		Array<int> unvisited_stack;
		unvisited_stack.Append(first);
		for (int c = 0; c < unvisited_stack.GetLength(); c++)
		{
			visited.SetBit(unvisited_stack[c], true);
			for (int e : entries[unvisited_stack[c]]) if (visited[e] == false) unvisited_stack.Append(e);
		}
	}
	return group_count; // <- answ for B; A = length of group 0
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



ivec2 sFindFirst(const DataFrame<uint8>& inMap, uint8 inFindThing)
{
	for (uint y = 0; y < inMap.GetHeight(); y++)
		for (uint x = 0; x < inMap.GetWidth(); x++)
		{
			if (inMap.Get(x, y) == inFindThing)
				return ivec2(x, y);
		}
	return ivec2(-1, -1);
}


void sFloodFill(DataFrame<uint8>& inMap, const ivec2& inStart, uint8 inFillThing)
{
	if (inStart.x < 0 || inStart.y < 0 || inStart.x >= (int)inMap.GetWidth() || inStart.y >= (int)inMap.GetHeight())
	{
		return;
	}
	uint8 c = inMap.Get(inStart.x, inStart.y);
	if (c == inFillThing || c == '.')
		return;

	inMap.Set(inStart.x, inStart.y, inFillThing);

	sFloodFill(inMap, inStart + ivec2(0, 1), inFillThing);
	sFloodFill(inMap, inStart + ivec2(1, 0), inFillThing);
	sFloodFill(inMap, inStart + ivec2(0, -1), inFillThing);
	sFloodFill(inMap, inStart + ivec2(-1, 0), inFillThing);
}



int gAdvent14()
{

	DataFrame<uint8> map;
	map.Resize(128, 128);
	int ones = 0;
	for (int x = 0; x < 128; x++)
	{
		KnotHash<256> knot;
		String str("vbqugkhl-"); //flqrgnkx  vbqugkhl
		str.Append(gToString(x));
		knot.HashString(str, 64);

		uint8 hash[16];
		knot.GetDenseHash(hash);

		for (int b = 0; b < 16; b++)
		{
			map.Set(x, b * 8 + 0, (hash[b] & 0x80) ? '#' : '.');
			map.Set(x, b * 8 + 1, (hash[b] & 0x40) ? '#' : '.');
			map.Set(x, b * 8 + 2, (hash[b] & 0x20) ? '#' : '.');
			map.Set(x, b * 8 + 3, (hash[b] & 0x10) ? '#' : '.');
			map.Set(x, b * 8 + 4, (hash[b] & 0x08) ? '#' : '.');
			map.Set(x, b * 8 + 5, (hash[b] & 0x04) ? '#' : '.');
			map.Set(x, b * 8 + 6, (hash[b] & 0x02) ? '#' : '.');
			map.Set(x, b * 8 + 7, (hash[b] & 0x01) ? '#' : '.');
		}
	}

	ivec2 first;
	int groups = 0;
	do
	{

		first = sFindFirst(map, '#');
		
		if (groups % 100 == 0 || first.x == -1)
		{
			for (int y = 0; y < 128; y++)
			{
				String s;
				for (int b = 0; b < 128; b++)
				{
					s.Append(map.Get(y, b));
				}
				std::cout << s << std::endl;
			}
			std::cout << String(128, '-') << std::endl;
		}


		if (first.x >= 0)
		{
			sFloodFill(map, first, '0' + groups % 10);
			groups++;
		}
		
	} while (first.x >= 0 && first.y >= 0);


	return 0;
}



int gAdvent15()
{
	int64 mul_a = 16807;
	int64 mul_b = 48271;
	int64 div = 2147483647;
	int64 a = 277;
	int64 b = 349;
	int points = 0;
	for (uint64 i = 0; i < 5000000; i++)
	{
		do
		{
			a *= mul_a;
			a %= div;
		} while ((a & 3) != 0);
		
		do
		{
			b *= mul_b;
			b %= div;
		} while ((b & 7) != 0);

		if ((a & 0xFFFF) == (b & 0xFFFF))
		{
			points++;
			std::cout << "a :" << a << std::endl;
			std::cout << "b :" << b << std::endl;
		}
	}
	return points;
}


enum class Type : uint8 { stSpin, stSwap, stPartner };
struct Step
{
	Type	mType;
	uint8	mA;
	uint8	mB;
};


int gAdvent16()
{
	
	String dance("abcdefghijklmnop");
	//String dance("abcde");
	

	File f;
	f.Open(".\\advent_16_input.txt", fomRead);
	LineReader reader(f);
	String line;
	reader.ReadLine(line);
	//line = "s1,x3/4,pe/b";
	Array<String> string_steps;
	gExplodeString(string_steps, line, ',');
	Array<Step> steps;


	std::cout << dance << std::endl;

	for (const String& s : string_steps)
	{
		Step step;
		if (s[0] == 's')
		{
			int count = gStringToInt(s.Substring(1, s.GetLength() - 1));
			count %= 16;

			step.mType = Type::stSpin;
			step.mA = count;
		}
		else if (s[0] == 'x')
		{
			Array<String> params;
			gExplodeString(params, s.Substring(1, s.GetLength() - 1), '/');
			step.mType = Type::stSwap;
			step.mA = gStringToInt(params[0]);
			step.mB = gStringToInt(params[1]);
		}
		else if (s[0] == 'p')
		{
			step.mType = Type::stPartner;
			step.mA = s[1];
			step.mB = s[3];
		}
		else
		{
			gAssert(false);
		}
		steps.Append(step);
	}


	HashMap<String, int> known_thingies(0x100000);

	int step_count = 0;
	char buff[16];
	uint64 count = 1000000000ll % 48ll;
	for (uint64 i = 0; i < count + 48*10; i++)
	{
		for (const Step& s : steps)
		{
			switch (s.mType)
			{
			case Type::stSpin:
				for (int c = 0; c < s.mA; c++)
					buff[c] = dance[c + 16 - s.mA];
				for (int c = 16 - s.mA - 1; c >= 0; c--)
					dance[c + s.mA] = dance[c];
				for (int c = 0; c < s.mA; c++)
					dance[c] = buff[c];
				break;
			case Type::stSwap:
				gSwap(dance[s.mA], dance[s.mB]);
				break;
			case Type::stPartner:
				int i0 = (int)dance.Find(s.mA);
				int i1 = (int)dance.Find(s.mB);
				gSwap(dance[i0], dance[i1]);
				break;
			}
		}

		Tuple<String, int>* f = known_thingies.Find(dance);
		if (f != nullptr)
		{
			std::cout << "duplicate state! " << dance << " encountered on step " << i << " with existing " << f->mSecond << std::endl;
		}
		known_thingies.Add(dance, (int) i);

		if (i % 48 == 0)
		{
			std::cout << dance << std::endl;
		}
	}





	return 0;
}

template <typename T>
class LinkedList
{
public:
	struct Node
	{
		Node() {}
		Node(const T& inValue, uint inPrev, uint inNext) : mValue(inValue), mPrev(inPrev), mNext(inNext) {}
		T		mValue;
		uint	mPrev = 0;
		uint	mNext = 0;
	};

	uint InsertAfterPos(const T& inValue, uint inAfterNodeIndex)
	{
		if (mNodes.IsEmpty())
			gAssert(inAfterNodeIndex == cMaxUInt);
		return InsertAfterNodeIndex(inValue, inAfterNodeIndex == cMaxUint ? cMaxUint : Forward(mFirstNode, inAfterNodeIndex));
	}

	uint InsertAfterNodeIndex(const T& inValue, uint inAfterNodeIndex)
	{
		if (inAfterNodeIndex == cMaxUInt)
		{
			if (mNodes.IsEmpty())
			{
				mNodes.Append(Node(inValue, 0, 0));
				mFirstNode = 0;
				return 0;
			}
			else
			{
				uint last_node = mNodes[mFirstNode].mPrev;
				gAssert(inAfterNodeIndex != cMaxSize64);
				uint new_node = InsertAfterNodeIndex(inValue, last_node);
				mFirstNode = new_node;
				return new_node;
			}
		}
		else
		{
			uint pos = inAfterNodeIndex;// Forward(mFirstNode, inAfterPos);
			uint next = mNodes[pos].mNext;
			mNodes.Append(Node(inValue, pos, next));
			uint new_next = (uint) mNodes.GetLastIndex();
			mNodes[pos].mNext = new_next;
			mNodes[next].mPrev = new_next;
			return new_next;
		}
	}

	void Print()
	{
		int idx = 0;
		for (const Node& n : mNodes)
			std::cout << n.mPrev << '|' << idx++ << '|' << n.mNext << ' ';
		std::cout << std::endl;

		int c = mFirstNode;
		while (c != cMaxUInt)
		{
			std::cout << mNodes[c].mPrev << '|' << c << '|' << mNodes[c].mNext << ' ';
			c = mNodes[c].mNext;
			if (c == mFirstNode)
			{
				c = cMaxUInt;
			}
		}
		std::cout << std::endl;
	}




	uint Forward(uint inStart, uint inSteps)
	{
		uint pos = inStart == cMaxUInt ? mFirstNode : inStart;
		for (uint i = 0; i < inSteps; i++)
		{
			pos = mNodes[pos].mNext;
			gAssert(pos != cMaxUInt);
		}
		return pos;
	}


	uint				mFirstNode = cMaxUInt;
	Array<Node>			mNodes;
};

int gAdvent17()
{
	LinkedList<uint> linked_list;
	uint step =  376;
	uint cur = linked_list.InsertAfterNodeIndex(0, cMaxUInt);
	for (uint i = 0; i < 100000000; i++)
	{
		cur = linked_list.InsertAfterNodeIndex(i, linked_list.Forward(cur, step));

		if (linked_list.mNodes[0].mNext == cur || !(i&0xFFFFF))
			std::cout << "0: " << linked_list.mNodes[0].mNext << "; cur: " << cur << std::endl;
		
	}
	//linked_list.Print();

uint len = 1;
uint pos = 0;
for (uint i = 0; i < 50000000; i++)
{
	pos += step;
	pos %= len++;
	pos++;
	if (pos == 1)
		std::cout << "0: " << i + 1 << "; pos: " << pos << std::endl;

}

return 1;
}



class Machine18
{
public:
	Machine18(int inID, const Array<String>& inProgram, Array<int64>& inInput, Array<int64>& inOutput) :
		mID(inID), mProgram(inProgram), mInput(inInput), mOutput(inOutput), mIP(0) {
		mVariables.Resize(26); mVariables.SetAll(0); GetVar("p") = inID;
	}

	uint RunUntilBlock()
	{
		uint instructions_run = 0;
		for (uint instructions_run = 0; mIP >= 0 && mIP < mProgram.GetLength(); instructions_run++)
		{
			Array<String> parts; gExplodeString(parts, mProgram[mIP], ' ');

			int64 param0_rvalue = sIsVar(parts[1]) ? cMaxSize64 : (int64)gStringToInt(parts[1]);
			int64& param0_lvalue = (param0_rvalue == cMaxSize64) ? GetVar(parts[1]) : param0_rvalue;
			int64 param1 = parts.GetLength() < 3 ? 0 : sIsVar(parts[2]) ? GetVar(parts[2]) : (int64)gStringToInt(parts[2]);

			if (parts[0] == "set") { param0_lvalue = param1; mIP++; }
			else if (parts[0] == "mul") { param0_lvalue *= param1; mIP++; }
			else if (parts[0] == "add") { param0_lvalue += param1; mIP++; }
			else if (parts[0] == "mod") { param0_lvalue %= param1; mIP++; }
			else if (parts[0] == "rcv") { if (mInput.IsEmpty()) break; param0_lvalue = mInput[0]; mInput.PopUnder(); mIP++; }
			else if (parts[0] == "snd") { mOutput.Append(param0_lvalue); mDataSent++; mIP++; }
			else if (parts[0] == "jgz") { if (param0_lvalue > 0) mIP += param1; else mIP++; }
			else { gAssert(false); }
		}
		return instructions_run;
	}

	uint                    GetDataSent() const { return mDataSent; }
private:
	int64&                  GetVar(const String& s) { return mVariables[s[0] - 'a']; }
	static uint             sIsVar(const String& s) { return (s[0] >= 'a'  && s[0] <= 'z'); }
	uint                    mDataSent = 0;
	int                     mID;
	const Array<String>&    mProgram;
	Array<int64>&           mOutput;
	Array<int64>&           mInput;
	uint64                  mIP;
	Array<int64>            mVariables;

};


int gAdvent18()
{
	File f;
	f.Open(".\\advent_18_input.txt", fomRead);
	LineReader reader(f);
	Array<String> lines;
	reader.ReadAllLines(lines);

	Array<int64> a0;
	Array<int64> a1;
	Machine18 m0(0, lines, a0, a1);
	Machine18 m1(1, lines, a1, a0);

	while (m0.RunUntilBlock() > 0 || m1.RunUntilBlock() > 0)
	{
	}

	return m1.GetDataSent();
}




int gAdvent19()
{
	enum Direction { dUp, dDown, dLeft, dRight };
	File f; f.Open(".\\advent_19_input.txt", fomRead);
	Array<String> map; String found; uint steps = 0; Direction dir = dDown;
	LineReader reader(f); reader.ReadAllLines(map);
	ivec2 pos(0, (int) map[0].Find('|'));
	while (true)
	{
		steps++;
		if (dir == dDown)	pos.x++;
		if (dir == dUp)		pos.x--;
		if (dir == dLeft)	pos.y--;
		if (dir == dRight)	pos.y++;
		switch (map[pos.x][pos.y])
		{
			case (' '): // end of the road
				std::cout << found << std::endl;
				return steps;
			case ('|') :
			case ('-') :
				break;
			case ('+') :
				if (dir == dDown || dir == dUp)	dir = (map[pos.x][pos.y + 1] == '-') ? dRight : dLeft;
				else							dir = (map[pos.x + 1][pos.y] == '|') ? dDown : dUp;
				break;
			default:
				found.Append(map[pos.x][pos.y]);
		}
	}
	return -1;
}


int gAdvent20()
{
	struct Particle { ivec3 mP, mV, mA; uint mIdx; };
	Array<Particle> particles;
	File f; f.Open(".\\advent_20_input.txt", fomRead);
	String line; LineReader reader(f);
	while (reader.ReadLine(line))
	{
		Array<String> integers; gExplodeString(integers, line, String(" ,<>=pva"));
		Particle part;
		part.mP.x = gStringToInt(integers[0]); part.mP.y = gStringToInt(integers[1]); part.mP.z = gStringToInt(integers[2]);
		part.mV.x = gStringToInt(integers[3]); part.mV.y = gStringToInt(integers[4]); part.mV.z = gStringToInt(integers[5]);
		part.mA.x = gStringToInt(integers[6]); part.mA.y = gStringToInt(integers[7]); part.mA.z = gStringToInt(integers[8]);
		part.mIdx = (uint)particles.GetLength();
		particles.Append(part);
	}
	gSort(particles, [](const Particle& inA, const Particle& inB) { return inA.mA.GetManhattanLength() < inB.mA.GetManhattanLength(); });

	for (int i = 0; i < 10000; i++)
	{
		for (int p0 = 0; p0 < particles.GetLength(); p0++)
		{
			bool hit = false;
			for (int p1 = p0 + 1; p1 < particles.GetLength(); p1++) if (particles[p0].mP == particles[p1].mP) { particles.SwapRemove(p1); p1--; hit = true; }
			if (hit) { particles.SwapRemove(p0); p0--; }
		}
		for (Particle& part : particles) { part.mV += part.mA; part.mP += part.mV; }
	}
	return (int) particles.GetLength(); // <-- B A --> return particles[0].mIdx;
}



