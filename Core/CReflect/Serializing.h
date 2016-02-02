#pragma once
#include "Stdafx.h"
#include "Singleton.h"
#include "types.h"
#include "array.h"
#include "Reflection.h"
#include "Streams.h"


// Grammar of a serialized object; this should be a proper CFG (?); at least compatible with a 1-lookahead parser (LL1?)
// 
// FileBegin			FH <FileCont>
// FileCont				<RootObject> <FileCont> | 0 
// RootObject			CI : <Assignment>
// Assignment			NI = <Item> ;
// Item					<Literal> | <List> | <Complex> | <Reference>
// Literal				SL | NL
// ListBegin			[ <ListCont>
// ListCont				<Item> <ListCont> | ]
// ComplexBegin			{ <ComplexCont>
// ComplexCont			<Member> <ComplexCont> | }
// RefBegin				< NI <RefEnd>
// RefEnd				@ PATH > | >

// expanded for LL1, also made sure lists cannot have mixed types
// 
// File					FH <Object>
// Object				CI : NI = <Item> <Object>
// Object				0
// Item					NL
// Item					SL
// Item					{ <CmpMembers>
// Item					[ <List>

// List					]
// List					NL <NumList>
// List					SL <StrList>
// List					< <RefList>
// List					{ <CpxList>
// List					[ <List>

// CmpMembers				NI = <Item> <CmpMembers>
// CmpMembers				}

// RefList				NI <RefEnd> <RefList>
// RefList				]
// CpxList				NI = <Item> <CmpMembers> <CpxList>
// CpxList				]
// NumList				NL <NumList>
// NumList				]
// StrList				SL <StrList>
// StrList				]
// RefEnd				>
// RefEnd				@ PATH >



// any object that needs to be serialized to a file needs to be derived from this class
class Serializable
{
private:
	String					mName;							///< Name of the object; root object in text file
	Path					mLocation;						///< Location of object; filename, streamname or none for run-time objects
public:
	const String&			GetName()			const		{ return mName; }
	const String&			GetLocation()		const		{ return mLocation; }
	void SetName(const String& inName)						{ mName = inName; }
	void SetLocation(const Path& inLocation)				{ mLocation = inLocation; }
	virtual void			JustMakeItVirtual() const		{ }

	ClassName						GetClassName() const	{ return ClassName(typeid(*this)); }
	const ClassReflectionInfo*		GetClassInfo() const;	///< see cpp for implementation


};

// Dependency between serialized files or between files in the same source file
// 
struct Dependency
{
	TypedPointer			mObject;						// Pointer of the object that has the dependency
	String					mName;							// 
	Path					mLocation;
};


// set of objects to be serialized into a file
class StreamSet : public Array<Serializable*>
{
	// straight inheritance from an simple array
};


// serializing grammar
enum EStreamTokenType
{
	sstUnknown = 0,			// Everything else UNKNW
	sstInvalid,				// Invalid token, like a string literal without closing tags
	sstIdentifier,			// NI / abcABC
	sstStringLiteral,		// SL / "xxxxx"
	sstNumberLiteral,		// NL / 1231.5234
	sstHeader,				// FH / ?xxxxx
	sstEOF,					// End of File
	// all token-to-char 1-to-1
	sstPath,				// /
	sstAssign,				// =
	sstDot,					// .
	sstColon,				// :
	sstAt,					// @
	sstSemicolon,			// ;
	sstListBegin,			// [
	sstListEnd,				// ]
	sstTypeBegin,			// {
	sstTypeEnd,				// }
	sstRefBegin,			// <
	sstRefEnd				// >
};


class TokenReader
{
public:

							TokenReader();
	void					SetStream(Stream& inStream);
	inline void				IncreaseNextCharPointer();
	inline bool				PeekNextChar(char& outChar) const;
	int						GetToken(EStreamTokenType& outType, char* outText, int inMaxLength);

private:
	static inline bool		sIsWhitespace(char inChar)	{ return inChar == ' ' || inChar == '\t'; }
	static inline bool		sIsLineEnding(char inChar)	{ return inChar == '\n' || inChar == '\r'; }
	static inline bool		sIsNumeric(char inChar)		{ return inChar >= '0' && inChar <= '9'; }
	static inline bool		sIsAlpha(char inChar)		{ return (inChar >= 'A' && inChar <= 'Z') || (inChar >= 'a' && inChar <= 'z') || inChar == '_'; }
	static inline bool		sIsAlphaNumeric(char inChar){ return sIsAlpha(inChar) || sIsNumeric(inChar); }

	Stream*					mStream;
	static const int		cBufferSize					= 512;			// buffer of 512
	int						mNextCharPointer;							// pointer within the buffer
	int						mEOFPointer;								// -1 if no EOF within buffer
	char					mNextCharBuffer[cBufferSize];

};






// expanded for LL1, also made sure lists cannot have mixed types
// 
// A) File					FH <Object>*
// B) Object				ID : ID = <Item> ;
// C) Item					NL
// D) Item					SL
// E) Item					{ <CmpMembers>
// F) Item					[ <List>
// Z) Item					< <Path> >
//    Path					ID PathCont
//    PathCont				>
//    PathCont				/ ID PathCont
//    PathCont				: ID


// Cheap (comma seperated) list!
// G') List				<Item> <ListCont>
// I') List				]
// H') ListCont			] 
// H') ListCont			, <Item> <ListCont>


// Or type checked list
// G) List					]
// H) List					NL <List>
// I) List					SL <List>
// J) List					< <Path> > <List>
// K) List					{ <CmpMembers> <List>
// L) List					[ <List> <List>

// M) CmpMembers			NI = <Item> <CmpMembers>
// N) CmpMembers			}


struct UnresolvedLink
{
	String					mLinkPath;		///< What is the path to the unresolved link (should be pointer type)
	String					mTargetPath;	///< What is the full path to the target
};

class FileReader
{
public:

	struct Token
	{

		String				mText;			
		EStreamTokenType	mType;			
	};

								FileReader();

	bool						ReadToken(Token& outToken);
	bool						ExpectToken(EStreamTokenType inType, Token& outToken);
	bool						ExpectToken(EStreamTokenType inType);
	bool						ReadFile(Stream& inStream, Array<Serializable*>& outObjects);
	bool						ReadObject(TypedPointer& outObject);
	bool						ReadItem(TypedPointer& outObject);
	bool						ReadRef(TypedPointer& outObject);
	bool						ReadCmpMembers(TypedPointer& outObject);
	bool						ReadList(TypedPointer& outObject);

	void						AddUnresolvedLink(const String& inLinkPath, const String& inTargetPath);

private:
	Array<UnresolvedLink>		mLinks;
	Array<String>				mTIP;
	Stream*						mStream;
	TokenReader					mTokenReader;
};



// Helper class that writes out reflected objects to (file) stream
class ObjectStreamer
{
public:
						ObjectStreamer(Stream& outStream) : mOutStream(outStream)		{}
	
	const Path&			GetTargetLocation()												{ return mOutStream.GetPath(); }

	bool				WriteInstance(const TypedPointer& inTypedPointer);
	bool				WriteNamedInstance(const TypedPointer& inTypedPointer, const String& inIdentifier);
	bool				WriteStreamSet(const StreamSet& inSet);
	
protected:
	
	const Dependency&	AddDependency(const TypedPointer& inTypedPointer);

	virtual void		WriteIdentifier(const TypedPointer& inTypedPointer, const String& inIdentifier) = 0;
	virtual void		WriteBeginComplexClass(const ClassReflectionInfo& inClassInfo) = 0;
	virtual void		WriteEndComplexClass() = 0;
	virtual void		WriteMember(const ClassMember& inMember) = 0;
	
	virtual void		WriteInternalReference(const Dependency& inDependency) = 0;
	virtual void		WriteExternalReference(const Dependency& inDependency) = 0;
	virtual void		WriteNullReference() = 0;

	virtual void		WriteBeginArray(const TypedPointer& inTypedPointer) = 0;
	virtual void		WriteEndArray() = 0;
	virtual void		WriteBeginString(const TypedPointer& inTypedPointer) = 0;
	virtual void		WriteEndString() = 0;

	virtual void		WritePrimitive(const String& inPrim) = 0;
	virtual void		WritePrimitive(const WString& inPrim) = 0;
	virtual void		WritePrimitive(const int& inPrim) = 0;
	virtual void		WritePrimitive(const float& inPrim) = 0;
	virtual void		WritePrimitive(const short& inPrim) = 0;
	virtual void		WritePrimitive(const ushort& inPrim) = 0;
	virtual void		WritePrimitive(const char& inPrim) = 0;
	virtual void		WritePrimitive(const uint& inPrim) = 0;
	virtual void		WritePrimitive(const uchar& inPrim) = 0;
	virtual void		WritePrimitive(const double& inPrim) = 0;
	virtual void		WritePrimitive(const bool& inPrim) = 0;
	virtual void		WritePrimitive(const wchar_t& inPrim) = 0;

	Array<Dependency>	mDependencies;
	Stream&				mOutStream;

};