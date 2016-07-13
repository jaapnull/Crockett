#include <CReflection/ObjectReader.h>
#include <CCore/String.h>
#include <CUtils/StringUtils.h>


ObjectReader::ObjectReader(Stream& inStream) : mStream(&inStream)
{
	mTokenReader.SetStream(inStream);
}

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
	gAssert(mStack.IsEmpty());
	gAssert(mObjects.IsEmpty());

	Token token;
	ExpectToken(sstHeader, token);
	std::cout << "Header: " << token.mText << std::endl;

	// First add to stack, then call function to fill it further
	mStack.Append(TypedPointer());
	
	// Read all objects in file
	while (ReadRootObject(mStack.Back()) == true)
	{
		outObjects.Append(mStack.Back());
		mObjects.Append(mStack.Back());
		mStack.Pop();
	}
	mStack.Clear();
	mObjects.Clear();
	return true;
}


bool ObjectReader::ReadRootObject(TypedPointer& outObject)
{
	// Expect outObject to be completely empty; to be filled in by this function
	gAssert(!outObject.IsValid());

	// B) Object				CI : NI = <Item>
	Token name;
	Token type;
	Token first_token;
	ReadToken(first_token);
	if (first_token.mType == sstEOF) 
		return false;
	type = first_token;
	bool r = ExpectToken(sstColon) && ExpectToken(sstIdentifier, name) && ExpectToken(sstAssign);
	if (!r) 
		return false;

	// Assume a root object is always a known compound (class) object
	const CompoundReflectionInfo* refl_info = ReflectionHost::sGetReflectionHost().FindClassInfo(ClassName(type.mText));
	TypedCompoundPointer new_obj(TypeDecl(refl_info), refl_info->mInstanceFunction(nullptr));
	outObject = new_obj;
	String* name_member = new_obj.GetCompoundMember<String>("!name");
	if (name_member != nullptr)
	{
		name_member->Set(name.mText);
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
		std::cout << "Item is String: " << first_token.mText << std::endl;
		sReadStringLiteral(ioObject, first_token.mText);
		return true;
	}
	case sstNumberLiteral:
	{
		gAssert(ioObject.mType.IsNakedPrimitive());
		std::cout << "Item is Number: " << first_token.mText << std::endl;
		sReadNumberLiteral(ioObject, first_token.mText);
		return true;
	}
	case sstRefBegin:
	{
		gAssert(ioObject.mType.GetOuterDecoration() == ctPointerTo);
		std::cout << "Item is Reference: " << std::endl;
		return ReadRef(ioObject);
	}
	case sstTypeBegin:
	{
		gAssert(ioObject.mType.IsNakedCompound());
		std::cout << "Item is Complex Type " << std::endl;
		return ReadCmpMembers(ioObject);
	}
	case sstListBegin:
	{
		gAssert(ioObject.mType.GetOuterDecoration() == ctArrayOf);
		std::cout << "Item is List: " << first_token.mText << std::endl;
		return ReadList(ioObject);
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
			std::cout << "FOUND LINK " << path << std::endl;
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

bool ObjectReader::ReadCmpMembers(TypedPointer& ioObject)
{
	gAssert(ioObject.mType.IsNakedCompound());
	Token first_token;
	ReadToken(first_token);
	switch (first_token.mType)
	{
		// M) CmpMembers				NI = <Item> <CmpMembers>
	case sstIdentifier:
	{
		TypedPointer member_ptr = TypedCompoundPointer(ioObject).GetCompoundMember(first_token.mText);
		if (!ExpectToken(sstAssign)) return false;
		bool rv = ReadItem(member_ptr);
		return rv && ReadCmpMembers(ioObject);
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

bool ObjectReader::ReadList(TypedPointer& outObject)
{
	gAssert(outObject.mType.IsNakedArray());
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
		std::cout << "Complex end" << std::endl;
		return true;
	}
	// G) List					]
	case sstNumberLiteral:
	{
		TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
		sReadNumberLiteral(element_ptr, first_token.mText);
		std::cout << "NumeredLiteralList, entry: " << first_token.mText << std::endl;
		return ReadList(outObject);
	}
	case sstStringLiteral:
	{
		TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
		sReadStringLiteral(element_ptr, first_token.mText);
		std::cout << "StringLiteralList, entry: " << first_token.mText << std::endl;
		return ReadList(outObject);
	}
	case sstRefBegin:
	{
		TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
		std::cout << "RefList, entry: " << first_token.mText << std::endl;
		return ReadRef(element_ptr) && ReadList(outObject);
	}
	case sstTypeBegin:
	{
		TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
		std::cout << "TypeList, entry: " << first_token.mText << std::endl;
		return ReadCmpMembers(element_ptr) && ReadList(outObject);
	}
	case sstListBegin:
	{
		TypedPointer element_ptr = TypedArrayPointer(outObject).CreateNewArrayItem();
		std::cout << "ListList, entry: " << first_token.mText << std::endl;
		return ReadList(element_ptr) && ReadList(outObject);
	}
	default:
		// unexpected type
		gAssert(false);
		return false;
	}
}



