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
	String s("A AB ABB B BB ABBC C");
	Array<String> ss;
	gExplodeString(ss, s, String(" "));

	struct { bool operator()(const String& inLeft, const String& inRight) { return gOrderString(inLeft, inRight) < 0; } } smf;
	gSort(ss.begin(), ss.end(), smf);
	return 0;
}
