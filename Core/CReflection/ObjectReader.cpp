#include <CReflection/ObjectReader.h>
#include <CCore/String.h>
#include <CUtils/StringUtils.h>


bool ObjectReader::ReadToken(Token& outToken)
{
	char buffer[256];
	int length = mTokenReader.GetToken(outToken.mType, &(buffer[0]), 256);
	outToken.mText.Set(&(buffer[0]), length);
	return outToken.mType != sstEOF;
}

bool ObjectReader::ExpectToken(EStreamTokenType inType, Token& outToken)
{
	bool ret = ReadToken(outToken);
	gAssert(outToken.mType == inType);
	return ret;
}

bool ObjectReader::ExpectToken(EStreamTokenType inType)

{
	Token temp;
	bool ret = ReadToken(temp);
	gAssert(temp.mType == inType);
	return ret;
}

bool ObjectReader::ReadFile(Stream& inStream, Array<TypedPointer>& outObjects)
{
	//// A) File					FH <Object>*
	mStream = &inStream;
	mTokenReader.SetStream(inStream);
	gAssert(mObjects.IsEmpty());

//	HEADER CHECK
//	Token token;
//	ExpectToken(sstHeader, token);
//	std::cout << "Header: " << token.mText << std::endl;


	// Read all objects in file
	TypedPointer new_object;
	while(ReadRootObject(new_object) == true)
	{
		outObjects.Append(new_object);
		mObjects.Append(new_object);
		new_object.Clear();
	}
	mObjects.Clear();
	return true;
}


bool ObjectReader::ReadRootObject(TypedPointer& outObject)
{
	// Expect outObject to be completely empty; to be filled in by this function
	gAssert(!outObject.IsValid());

	// B) Object				CI : NI = <Item>
	// O) Object				CI = <Item>

	
	Token type;
	Token name;

	Token first_token;
	Token second_token;
	ReadToken(first_token);
	if (first_token.mType == sstEOF) 
		return false;
	type = first_token;

	ReadToken(second_token);
	if (second_token.mType == sstColon)			//B
	{
		bool r = ExpectToken(sstIdentifier, name) && ExpectToken(sstAssign);
		if (!r) return false;
	}
	else if (second_token.mType == sstAssign)	// O
	{
		// directly to <Item>
	}

	// Assume a root object is always a known compound (class) object
	const CompoundReflectionInfo* refl_info = ReflectionHost::sGetReflectionHost().FindClassInfo(ClassName(type.mText));
	TypedCompoundPointer new_obj(TypeDecl(refl_info), refl_info->mInstanceFunction(nullptr));
	outObject = new_obj;
	
	mRootObject = new_obj;
	mRootPath.Clear();

	if (name.mType != sstInvalid)
	{
		gAssert(name.mType == sstIdentifier);
		String* name_member = new_obj.GetCompoundMember<String>("!name");
		if (name_member != nullptr)
		{
			name_member->Set(name.mText);
		}
	}

	bool return_value = ReadItem(outObject);
	return return_value;
}


static void sReadStringLiteral(TypedPointer& ioObject, const String& inLiteralString)
{
	gAssert(ioObject.mType.IsCharString());
	static_cast<String*>(ioObject.mPointer)->Set(inLiteralString);
}


static void sReadNumberLiteral(TypedPointer& ioObject, const String& inLiteralString)
{
	gAssert(ioObject.mType.IsNakedPrimitive());
	switch (ioObject.mType.mNakedType)
	{
		case etFloat: *static_cast<float*>(ioObject.mPointer) = gStringToFloat(inLiteralString); return;
		case etInt:   *static_cast<int*>(ioObject.mPointer) = gStringToInt(inLiteralString); return;
		case etUInt:  *static_cast<uint*>(ioObject.mPointer) = gStringToUInt(inLiteralString); return;
		case etBool:  *static_cast<bool*>(ioObject.mPointer) = gStringToBool(inLiteralString); return;
	default:
		gAssert(false); // readLiteral only works on float,int,uint,bool
	}
}


bool ObjectReader::ReadItem(TypedPointer& ioObject)
{
	// Object is already set up externally
	gAssert(ioObject.IsValid());

	// C) Item					NL	
	// D) Item					SL
	// E) Item					{ <CmpMembers>
	// F) Item					[ <List>
	Token first_token;
	ReadToken(first_token);
	switch (first_token.mType)
	{
	case sstStringLiteral:
	{
		gAssert(ioObject.mType.IsNakedString());
		sReadStringLiteral(ioObject, first_token.mText);
		return true;
	}
	case sstNumberLiteral:
	{
		gAssert(ioObject.mType.IsNakedPrimitive());
		sReadNumberLiteral(ioObject, first_token.mText);
		return true;
	}
	case sstRefBegin:
	{
		gAssert(ioObject.mType.GetOuterDecoration() == ctPointerTo);
		return ReadRef(ioObject);
	}
	case sstTypeBegin:
	{
		gAssert(ioObject.mType.IsNakedCompound());
		return ReadCmpMembers(ioObject);
	}
	case sstListBegin:
	{
		gAssert(ioObject.mType.GetOuterDecoration() == ctArrayOf);
		return ReadList(ioObject, true);
	}
	default:
		// unexpected type
		gAssert(false);
		return false;
	}
}



bool ObjectReader::ReadRef(TypedPointer& ioObject)
{
	// can only read a reference into a pointer
	gAssert(ioObject.mType.IsNakedPointer());
	String location;
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
			location.Append(token.mText); break;
		case sstRefEnd:
		{
			if (!location.IsEmpty())
			{
				UnresolvedLink ul;
				ul.mReflectionPath = mRootPath;
				ul.mObjectLocation = location;
				mLinks.Append(ul);
				ioObject.mPointer = (void*)0; // set nullpointer
			}
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

bool ObjectReader::ReadCmpMembers(TypedPointer& ioObject)
{
	gAssert(ioObject.mType.IsNakedCompound());
	Token first_token;
	ReadToken(first_token);

	String member_name = first_token.mText;
	if (first_token.mType == sstBang)
	{
		ReadToken(first_token);
		gAssert(first_token.mType == sstIdentifier);
		member_name.Append(first_token.mText);
	}

	switch (first_token.mType)
	{
		// M) CmpMembers				NI = <Item> <CmpMembers>
		case sstIdentifier:
		{
			TypedPointer member_ptr = TypedCompoundPointer(ioObject).GetCompoundMember(member_name);
			if (!ExpectToken(sstAssign)) 
				return false;


			int32 member_idx = TypedCompoundPointer(ioObject).GetCompoundMemberIndex(member_name);
			mRootPath.Append(ReflectPathPart(member_idx));
			bool rv = ReadItem(member_ptr);
			mRootPath.Pop();
			rv &= ReadCmpMembers(ioObject);
			return rv;
		}
		// N) CmpMembers				}
		case sstTypeEnd:
		{
			return true;
		}
		default:
		// unexpected type
		gAssert(false);
		return false;
	}
}

bool ObjectReader::ReadList(TypedPointer& outObject, bool inFirstEntry)
{
	gAssert(outObject.mType.IsNakedArray());
	// G) List					]
	// H) List		[,]			NL <List>
	// I) List		[,]			SL <List>
	// J) List		[,]			< <Path> > <List>
	// K) List		[,]			{ <CmpMembers> <List>
	// L) List		[,]			[ <List> <List>
	Token first_token;
	ReadToken(first_token);

	if (!inFirstEntry)
	{
		if (first_token.mType == sstListEnd)
		{
			return true;
		}
		else if (first_token.mType == sstComma)
		{
			ReadToken(first_token);
		}
		else
		{
			gAssert(false);
			return false;
		}
	}

	switch (first_token.mType)
	{
		// G) List					]
		case sstListEnd:
		{
			return true;
		}

		// H) List					NL <List>
		case sstNumberLiteral:
		{
			TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
			sReadNumberLiteral(element_ptr, first_token.mText);
			return ReadList(outObject, false);
		}

		// I) List					SL <List>
		case sstStringLiteral:
		{
			TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
			sReadStringLiteral(element_ptr, first_token.mText);
			return ReadList(outObject, false);
		}

		// J) List					< <Path> > <List>
		case sstRefBegin:
		{
			mRootPath.Append(ReflectPathPart(uint32(TypedArrayPointer(outObject).GetContainerElementCount())));
			TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
			bool r = ReadRef(element_ptr);
			if (!r)
				return false;
			mRootPath.Pop();
			return ReadList(outObject, false);
		}

		// K) List					{ <CmpMembers> <List>
		case sstTypeBegin:
		{
			mRootPath.Append(ReflectPathPart(uint32(TypedArrayPointer(outObject).GetContainerElementCount())));
			TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
			bool r = ReadCmpMembers(element_ptr);
			if (!r)
				return false;
			mRootPath.Pop();
			return ReadList(outObject, false);
		}

		// L) List					[ <List> <List>
		case sstListBegin:
		{
			mRootPath.Append(ReflectPathPart(uint32(TypedArrayPointer(outObject).GetContainerElementCount())));
			TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
			bool r = ReadList(element_ptr, true);
			if (!r)
				return false;
			mRootPath.Pop();
			return ReadList(outObject, false);
		}
	
		default:
		// unexpected type
		gAssert(false);
		return false;
	}
}





