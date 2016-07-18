#pragma once

#include <CCore/Assert.h>
#include <CCore/Streams.h>

// Tokeinzer: class for tokenizing string using the Object grammar


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
	sstComma,				// ,
	sstBang,				// !
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
	static inline bool		sIsWhitespace(char inChar) { return inChar == ' ' || inChar == '\t'; }
	static inline bool		sIsLineEnding(char inChar) { return inChar == '\n' || inChar == '\r'; }
	static inline bool		sIsNumeric(char inChar) { return inChar >= '0' && inChar <= '9'; }
	static inline bool		sIsAlpha(char inChar) { return (inChar >= 'A' && inChar <= 'Z') || (inChar >= 'a' && inChar <= 'z') || inChar == '_'; }
	static inline bool		sIsAlphaNumeric(char inChar) { return sIsAlpha(inChar) || sIsNumeric(inChar); }

	Stream*					mStream;
	static const int		cBufferSize = 512;							// Buffer of 512
	int						mNextCharPointer;							// Pointer within the buffer
	int						mEOFPointer;								// -1 if no EOF within buffer
	char					mNextCharBuffer[cBufferSize];				// Buffer that keeps token in there

};


