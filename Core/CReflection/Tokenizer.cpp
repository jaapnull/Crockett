#include <CReflection/Tokenizer.h>

TokenReader::TokenReader() :
	mStream(0),
	mNextCharPointer(cBufferSize),
	mEOFPointer(0)
{
}

void TokenReader::SetStream(Stream& inStream)
{
	mStream = &inStream;
	IncreaseNextCharPointer();
}

void TokenReader::IncreaseNextCharPointer()
{
	assert(mNextCharPointer != mEOFPointer);
	mNextCharPointer++;
	if (mNextCharPointer >= cBufferSize)
	{
		size64 read = mStream->GetBytes(&mNextCharBuffer[0], cBufferSize);
		assert(read != Stream::cStreamError);
		mEOFPointer = int(read);
		mNextCharPointer = 0;
	}
}

bool TokenReader::PeekNextChar(char& outChar) const
{
	if (mNextCharPointer == mEOFPointer) return false;
	outChar = mNextCharBuffer[mNextCharPointer];
	return true;
}

int TokenReader::GetToken(EStreamTokenType& outType, char* outText, int inMaxLength)
{
	assert(mStream);
	int64 read_count = 0;
	char first_char;
	EStreamTokenType initial_type = sstUnknown;
	do
	{
		bool p = PeekNextChar(first_char);
		if (p == false)
		{
			outType = sstEOF;
			return 0;
		}
		IncreaseNextCharPointer();
	} while (sIsWhitespace(first_char) || sIsLineEnding(first_char));
	// copy over the first character
	switch (first_char)
	{
	case ('=') : initial_type = sstAssign; break;
	case (':') : initial_type = sstColon; break;
	case ('@') : initial_type = sstAt; break;
	case ('.') : initial_type = sstDot; break;
	case (',') : initial_type = sstComma; break;
	case ('!') : initial_type = sstBang; break;
	case (';') : initial_type = sstSemicolon; break;
	case ('[') : initial_type = sstListBegin; break;
	case (']') : initial_type = sstListEnd; break;
	case ('{') : initial_type = sstTypeBegin; break;
	case ('}') : initial_type = sstTypeEnd; break;
	case ('/') : initial_type = sstPath; break;
	case ('\\') : initial_type = sstPath; break;
	case ('\"') : initial_type = sstStringLiteral; break;
	case ('?') : initial_type = sstHeader; break;
	case ('<') : initial_type = sstRefBegin; break;
	case ('>') : initial_type = sstRefEnd; break;
	default:		initial_type = sIsAlpha(first_char) ? sstIdentifier : sIsNumeric(first_char) ? sstNumberLiteral : sstUnknown;
	}

	switch (initial_type)
	{
		// keep going until you find a non-alphanumeric
		case (sstIdentifier) :
		{
			outText[0] = first_char;
			char next_char;
			bool p = PeekNextChar(next_char);
			int t = 1;
			while (p == true && (sIsAlphaNumeric(next_char)))
			{
				outText[t++] = next_char;
				IncreaseNextCharPointer();
				p = PeekNextChar(next_char);
			}
			// check for certain keywords
			if (t == 5 && memcmp(outText, "false", 5) == 0)
			{
				outType = sstNumberLiteral;
			}
			else if (t == 4 && memcmp(outText, "true", 4) == 0)
			{
				outType = sstNumberLiteral;
			}
			else outType = sstIdentifier;
			return t;
		}

		// keep going until you find a non-alphanumeric
		case (sstHeader) :
		{
			outText[0] = first_char;
			char next_char;
			bool p = PeekNextChar(next_char);
			int t = 1;
			while (p == true && (sIsAlphaNumeric(next_char)))
			{
				outText[t++] = next_char;
				IncreaseNextCharPointer();
				p = PeekNextChar(next_char);
			}
			outType = sstHeader;
			return t;
		}

		// keep going until you find a non-numeric, except for a single point
		case (sstNumberLiteral) :
		{
			outText[0] = first_char;
			bool found_point = false;
			char next_char;
			bool p = PeekNextChar(next_char);
			int t = 1;
			while (p == true && (sIsNumeric(next_char)) || (next_char == '.' && !found_point))
			{
				found_point |= next_char == '.';
				outText[t++] = next_char;
				IncreaseNextCharPointer();
				p = PeekNextChar(next_char);
			}
			outType = sstNumberLiteral;
			return t;
		}

		// keep going until you find a second " or a linebreak (will turn into unknown token)
		case (sstStringLiteral) :
		{
			char next_char;
			bool p = PeekNextChar(next_char);
			int t = 0;
			while (p == true && next_char != '\"' && !sIsLineEnding(next_char))
			{
				outText[t++] = next_char;
				IncreaseNextCharPointer();
				p = PeekNextChar(next_char);
			}
			if (p == false || next_char != '\"')
			{
				// if it breaks on anything but the closing ", we throw an invalid
				outType = sstInvalid;
			}
			else
			{
				// we eat the closing "
				IncreaseNextCharPointer();
				outType = sstStringLiteral;
			}
			return t;
		}

		default:
		{
			// single character tokens
			outType = initial_type;
			outText[0] = first_char;
			return 1;
		}
	}
}
