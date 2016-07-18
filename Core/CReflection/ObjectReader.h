#include <CCore/String.h>
#include <CCore/Streams.h>
#include <CReflection/Reflection.h>
#include <CReflection/Serializing.h>
#include <CReflection/Tokenizer.h>

// expanded for LL1, also made sure lists cannot have mixed types
// 
// A) File					FH <Object>*
// B) Object				ID : ID = <Item> ;
// O) Object				ID = <Item> ;
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
	String					mObjectLocation;	///< What is the path to the unresolved link (should be pointer type)
	Array<ReflectPathPart>	mReflectionPath;	///< What is the full path to the target
};


class ObjectReader
{
public:

	struct Token
	{
		String				mText;
		EStreamTokenType	mType = sstInvalid;
	};

	bool						ReadToken(Token& outToken);
	bool						ExpectToken(EStreamTokenType inType, Token& outToken);
	bool						ExpectToken(EStreamTokenType inType);
	bool						ReadFile(Stream& inStream, Array<TypedPointer>& outObjects);
	bool						ReadRootObject(TypedPointer& outObject);
	bool						ReadItem(TypedPointer& outObject);
	bool						ReadRef(TypedPointer& outObject);
	bool						ReadCmpMembers(TypedPointer& outObject);
	bool						ReadList(TypedPointer& outObject, bool inFirstEntry);

	const Array<UnresolvedLink>& GetLinks() const { return mLinks; }

private:
	Array<UnresolvedLink>		mLinks;			///< List of all unresolved links, where to find them and what to link them to
	
	TypedPointer				mRootObject;	///< Current root object that is being read
	Array<ReflectPathPart>		mRootPath;		///< Path within current root object (used for unresolved links)
	
	Array<TypedPointer>			mObjects;		///< List of all created objects (popped off mStack during course of read)
	Stream*						mStream;		///< Stream that supplies text to mTokenReader
	TokenReader					mTokenReader;	///< Token Reader used to generate the tokens during read
};

