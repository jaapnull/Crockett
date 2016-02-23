#include "stdafx.h"
#include "Serializing.h"



bool ObjectStreamer::WriteNamedInstance(const TypedPointer& inTypedPointer, const String& inIdentifier)
{
	WriteIdentifier(inTypedPointer, inIdentifier);
	mOutStream.PutBytes("=", 1);
	return WriteInstance(inTypedPointer);
}

bool ObjectStreamer::WriteInstance(const TypedPointer& inTypedPointer)
{
	if (inTypedPointer.mType.mModifiers.IsEmpty())
	{
		if (inTypedPointer.mType.mBaseType == etClass)
		{
			const ClassReflectionInfo* info = ReflectionHost::sGetInstance().FindClassInfo(inTypedPointer.mType.mName);
			if (info == 0)
			{
				assert(false);
				return 0; // unknown class type defined
			}
			WriteBeginComplexClass(*info);
			for (uint m = 0; m < info->mMembers.GetLength(); m++)
			{
				ClassMember tm = info->mMembers[m];
				TypedPointer tp(tm.mType, (void*)(((byte*)inTypedPointer.mPointer) + tm.mOffset));
				WriteMember(tm);
				WriteInstance(tp);
			}
			WriteEndComplexClass();
			return true;
		}
		else
		{
			void* ptr = inTypedPointer.mPointer;
			switch (inTypedPointer.mType.mBaseType)
			{
				case etInt:		WritePrimitive(*(int*)ptr); return true;
				case etFloat:	WritePrimitive(*(float*)ptr); return true;
				case etShort:	WritePrimitive(*(short*)ptr); return true;
				case etUShort:	WritePrimitive(*(ushort*)ptr); return true;
				case etChar:	WritePrimitive(*(char*)ptr); return true;
				case etUInt:	WritePrimitive(*(uint*)ptr); return true;
				case etUChar:	WritePrimitive(*(uchar*)ptr); return true;
				case etDouble:	WritePrimitive(*(double*)ptr); return true;
				case etBool:	WritePrimitive(*(bool*)ptr); return true;
				case etWChar:	WritePrimitive(*(wchar_t*)ptr); return true;
				default:		return false;
			}
		}
	}
	else
	{
		ETypeModifier outer_container = inTypedPointer.mType.mModifiers.Back();
		// Write dependency reference
		if (outer_container == ctPointer)
		{
			if (!inTypedPointer.Dereferenced().IsValidPointer())
			{
				WriteNullReference();
			}
			else
			{
				const Dependency& dep = AddDependency(inTypedPointer.Dereferenced());
				if (dep.mLocation.Equals(GetTargetLocation()))
				{
					WriteInternalReference(dep);
				}
				else
				{
					WriteExternalReference(dep);
				}
			}
			return true;
		}
		TypedPointer dereffed = inTypedPointer.Dereferenced();
		size64 elem_count = inTypedPointer.GetElementCount();
		size64 elem_size = dereffed.mType.GetSize();
		byte* base_data = (byte*)dereffed.mPointer;
		if (outer_container == ctArray)
		{
			WriteBeginArray(inTypedPointer);
			for (size64 c = 0; c < elem_count; c++)
			{
				dereffed.mPointer = (base_data + c * elem_size);
				WriteInstance(dereffed);
			}
			WriteEndArray();
			return true;
		}
		else // ctString:
		{
			if (dereffed.mType.mBaseType == etChar  && dereffed.mType.mModifiers.IsEmpty()) { WritePrimitive(*((String*)inTypedPointer.mPointer)); return 1; }
			if (dereffed.mType.mBaseType == etWChar && dereffed.mType.mModifiers.IsEmpty()) { WritePrimitive(*((WString*)inTypedPointer.mPointer)); return 1; }

			WriteBeginString(inTypedPointer);
			for (uint c = 0; c < elem_count; c++)
			{
				dereffed.mPointer = (base_data + c * elem_size);
				WriteInstance(dereffed);
			}
			WriteEndString();
			return true;
		}
	}
	return false;
}


const Dependency& ObjectStreamer::AddDependency(const TypedPointer& inTypedPointer)
{
	gAssert(inTypedPointer.IsValidPointer());
	Dependency dependency_entry;
	dependency_entry.mObject = inTypedPointer;
	const Serializable* serializable = static_cast<Serializable*>(inTypedPointer.mPointer);
	if (serializable)
	{
		dependency_entry.mName = serializable->GetName();
		if (dependency_entry.mName.IsEmpty())
		{
			dependency_entry.mName = inTypedPointer.mType.ToString().Append("_123");
		}
		dependency_entry.mLocation = serializable->GetLocation();
	}
	else
	{
		dependency_entry.mName = inTypedPointer.mType.mName;
		dependency_entry.mName.Append('_');
		dependency_entry.mName.Append(gToString((uint)mDependencies.GetLength()));
	}

	if (dependency_entry.mLocation.IsEmpty())
	{
		dependency_entry.mLocation = GetTargetLocation();
	}

	mDependencies.Append(dependency_entry);
	return mDependencies.Back();
}


const ClassReflectionInfo* Serializable::GetClassInfo() const
{
	return ReflectionHost::sGetInstance().FindClassInfo(GetClassName());
}

bool ObjectStreamer::WriteStreamSet(const StreamSet& inSet)
{
	assert(mDependencies.IsEmpty());
	
	mOutStream.PutBytes("?Writer1_0\n", 11);

	for (uint o = 0; o < inSet.GetLength(); o++)
	{
		// convert stream set to TypedPointers
		const ClassReflectionInfo* root_desc = inSet[o]->GetClassInfo();
		if (root_desc == 0) return false;
		FullType d;
		d.mName = root_desc->mName;
		d.mBaseType = etClass;
		TypedPointer typed_pointer(d, (void*)inSet[o]);
		
		// set location of object
		inSet[o]->SetLocation(GetTargetLocation());

		WriteNamedInstance(typed_pointer, inSet[o]->GetName());
	}

	for (uint i = 0; i < mDependencies.GetLength(); i++)
	{
		Dependency& d(mDependencies[i]);
		if (d.mLocation.Equals(GetTargetLocation())) // we have a dependency that is not stored anywhere (or is not a resource); save it inside the file
		{
			WriteNamedInstance(d.mObject, d.mName);
		}
		else
		{
			// we assume the dependency exists and will ignore for now
		}
	}

	return true;
}




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
	case ('=') :	initial_type = sstAssign; break;
	case (':') :	initial_type = sstColon; break;
	case ('@') :	initial_type = sstAt; break;
	case ('.') :	initial_type = sstDot; break;
	case (';') :	initial_type = sstSemicolon; break;
	case ('[') :	initial_type = sstListBegin; break;
	case (']') :	initial_type = sstListEnd; break;
	case ('{') :	initial_type = sstTypeBegin; break;
	case ('}') :	initial_type = sstTypeEnd; break;
	case ('/') :	initial_type = sstPath; break;
	case ('\\') : initial_type = sstPath; break;
	case ('\"') :	initial_type = sstStringLiteral; break;
	case ('?') :	initial_type = sstHeader; break;
	case ('<') :	initial_type = sstRefBegin; break;
	case ('>') :	initial_type = sstRefEnd; break;
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




FileReader::FileReader() : mStream(0)
{
}

bool FileReader::ReadToken(Token& outToken)
{
	char buffer[256];
	int length = mTokenReader.GetToken(outToken.mType, &(buffer[0]), 256);
	outToken.mText.Set(&(buffer[0]), length);
	return outToken.mType != sstEOF;
}

bool FileReader::ExpectToken(EStreamTokenType inType, Token& outToken)
{
	bool ret = ReadToken(outToken);
	gAssert(outToken.mType == inType);
	return ret;
}

bool FileReader::ExpectToken(EStreamTokenType inType)
{
	Token temp;
	bool ret = ReadToken(temp);
	gAssert(temp.mType == inType);
	return ret;
}

bool FileReader::ReadFile(Stream& inStream, Array<Serializable*>& outObjects)
{
	//// A) File					FH <Object>*
	mStream = &inStream;
	mTokenReader.SetStream(inStream);
	mTIP.Clear();
	Token token;
	ExpectToken(sstHeader, token);
	std::wcout << "Header: " << token.mText << std::endl;

	Array<TypedPointer> found_objects;
	TypedPointer new_object;
	while (ReadObject(new_object) == true)
	{ 
		found_objects.Append(new_object);
		outObjects.Append((Serializable*) new_object.mPointer);
	}

	return true;
}


bool FileReader::ReadObject(TypedPointer& outObject)
{
	// B) Object				CI : NI = <Item>
	Token name;
	Token type;
	Token first_token;
	ReadToken(first_token);
	if (first_token.mType == sstEOF) return false;
	type = first_token;
	bool r = ExpectToken(sstColon) &&
		ExpectToken(sstIdentifier, name) &&
		ExpectToken(sstAssign);
	if (!r) return false;

	mTIP.Append(name.mText);
	outObject = ReflectionHost::sGetInstance().CreateClass(ClassName(type.mText), 0);
	static_cast<Serializable*>(outObject.mPointer)->SetName(name.mText);
	std::wcout << "Object Found " << name.mText << ':' << type.mText << std::endl;
	
	bool return_value =  ReadItem(outObject);
	mTIP.Pop();
	return return_value;
}


static void sReadStringLiteral(TypedPointer& ioObject, const String& inLiteralString)
{
	gAssert(ioObject.mType.mModifiers.IsOnly(ctString));
	if (ioObject.mType.mBaseType == etChar)
	{
		static_cast<String*>(ioObject.mPointer)->Set(inLiteralString);
	}
	else if (ioObject.mType.mBaseType == etWChar)
	{
		static_cast<WString*>(ioObject.mPointer)->Set(L"YeahDontSupportWStringYet");
	}
	else
	{
		gAssert(false); // should be string of chars or wchars
	}
}


static void sReadNumberLiteral(TypedPointer& ioObject, const String& inLiteralString)
{
	gAssert(ioObject.mType.mModifiers.IsOnly(ctNone));
	switch (ioObject.mType.mBaseType)
	{

		case etFloat: *static_cast<float*>(ioObject.mPointer)	= gStringToFloat(inLiteralString); return;
		case etInt:   *static_cast<int*>(ioObject.mPointer)		= gStringToInt(inLiteralString); return;
		case etUInt:  *static_cast<uint*>(ioObject.mPointer)	= gStringToUInt(inLiteralString); return;
		case etBool:  *static_cast<bool*>(ioObject.mPointer)	= gStringToBool(inLiteralString); return;
		default:
			gAssert(false); // readLiteral only works on float,int,uint,bool
	}
}


bool FileReader::ReadItem(TypedPointer& ioObject)
{
	// C) Item					NL	
	// D) Item					SL
	// E) Item					{ <CmpMembers>
	// F) Item					[ <List>
	Token first_token;
	ReadToken(first_token);
	switch (first_token.mType)
	{	case sstStringLiteral:
	{
		std::wcout << "Item is String: " << first_token.mText << std::endl;
		sReadStringLiteral(ioObject, first_token.mText);
		return true;
	}
	case sstNumberLiteral:
	{
		std::wcout << "Item is Number: " << first_token.mText << std::endl;
		sReadNumberLiteral(ioObject, first_token.mText);
		return true;
	}
	case sstRefBegin:
	{
		std::wcout << "Item is Reference: " << std::endl;
		return ReadRef(ioObject);
	}
	case sstTypeBegin:
	{
		std::wcout << "Item is Complex Type " << std::endl;
		return ReadCmpMembers(ioObject);
	}
	case sstListBegin:
	{
		std::wcout << "Item is List: " << first_token.mText << std::endl;
		//ioObject.ClearList();
		return ReadList(ioObject);
	}
	default:
		// unexpected type
		gAssert(false);
		return false;
	}
}



bool FileReader::ReadRef(TypedPointer& ioObject)
{
	// can only read a reference into a pointer
	gAssert(ioObject.mType.mModifiers.IsPointer());
	String path;
	Token token;
	while (ReadToken(token))
	{
		switch (token.mType)
		{
			// all valid path tokens
		case sstDot:
		case sstIdentifier:
		case sstPath:
		case sstAt:
		case sstColon:
			path.Append(token.mText); break;
		case sstRefEnd:
		{
			std::wcout << "Ref; Target is " << path << std::endl;
			for (const String& s : mTIP)
			{
				std::wcout << s << '.';
			}
			std::wcout << std::endl;
//			AddUnresolvedLink(path, mTIP)
			ioObject.mPointer = (void*)0; // set nullpointer
			return true;
		}
		default:
			// unexpected type
			gAssert(false);
			return false;
		}

	}
	return false;
}

bool FileReader::ReadCmpMembers(TypedPointer& ioObject)
{
	gAssert(ioObject.mType.mModifiers.IsOnly(ctNone));
	gAssert(!ioObject.mType.IsPrimitiveType());
	Token first_token;
	ReadToken(first_token);
	switch (first_token.mType)
	{
		// M) CmpMembers				NI = <Item> <CmpMembers>
	case sstIdentifier:
	{
							std::wcout << "Complex Member: " << first_token.mText << std::endl;
							TypedPointer member_ptr = ioObject.GetMember(first_token.mText);
							if (!ExpectToken(sstAssign)) return false;
							mTIP.Append(first_token.mText);
							bool rv = ReadItem(member_ptr);
							mTIP.Pop();
							return rv && ReadCmpMembers(ioObject);
	}
		// N) CmpMembers				}
	case sstTypeEnd:
	{
						std::wcout << "Complex end" << std::endl;
						return true;
	}
	default:
		// unexpected type
		gAssert(false);
		return false;
	}
}

bool FileReader::ReadList(TypedPointer& outObject)
{
	gAssert(outObject.mType.mModifiers.IsArray());
	// G) List					]
	// H) List					NL <List>
	// I) List					SL <List>
	// J) List					< <Path> > <List>
	// K) List					{ <CmpMembers> <List>
	// L) List					[ <List> <List>
	Token first_token;
	ReadToken(first_token);
	switch (first_token.mType)
	{
		// G) List					]
	case sstListEnd:
	{
						std::wcout << "Complex end" << std::endl;
						return true;
	}
		// G) List					]
	case sstNumberLiteral:
	{
								TypedPointer element_ptr = outObject.PushNewArrayItem();
								sReadNumberLiteral(element_ptr, first_token.mText);
								std::wcout << "NumeredLiteralList, entry: " << first_token.mText << std::endl;
								return ReadList(outObject);
	}
	case sstStringLiteral:
	{
								TypedPointer element_ptr = outObject.PushNewArrayItem();
								sReadStringLiteral(element_ptr, first_token.mText);
								std::wcout << "StringLiteralList, entry: " << first_token.mText << std::endl;
								return ReadList(outObject);
	}
	case sstRefBegin:
	{
								TypedPointer element_ptr = outObject.PushNewArrayItem();
								std::wcout << "RefList, entry: " << first_token.mText << std::endl;
								return ReadRef(element_ptr) && ReadList(outObject);
	}
	case sstTypeBegin:
	{
								TypedPointer element_ptr = outObject.PushNewArrayItem();
								std::wcout << "TypeList, entry: " << first_token.mText << std::endl;
								return ReadCmpMembers(element_ptr) && ReadList(outObject);
	}
	case sstListBegin:
	{
								TypedPointer element_ptr = outObject.PushNewArrayItem();
								std::wcout << "ListList, entry: " << first_token.mText << std::endl;
								return ReadList(element_ptr) && ReadList(outObject);
	}
	default:
		// unexpected type
		gAssert(false);
		return false;
	}
}



