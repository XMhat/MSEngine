/* == LLUTIL.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Util' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Util
/* ------------------------------------------------------------------------- */
// ! This contains miscellenious utility functions such as string, number and
// ! time manipulation functions.
/* ========================================================================= */
namespace LLUtil {                     // Util namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArgs;                 using namespace IClock::P;
using namespace IStd::P;               using namespace IString::P;
using namespace IUrl::P;               using namespace IUtf;
using namespace IUtil::P;              using namespace IUuId::P;
/* ========================================================================= **
** ######################################################################### **
** ## Util.* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Util.AscTime
// > Timestamp:integer=The timestamp to convert to string
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AscTime, 1,
  LCPUSHVAR(StrFromTimeTT(LCGETINT(StdTimeT, 1, "Timestamp"))));
/* ========================================================================= */
// $ Util.AscTimeUTC
// > Timestamp:integer=The timestamp to convert to string
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AscTimeUTC, 1,
  LCPUSHVAR(StrFromTimeTTUTC(LCGETINT(StdTimeT, 1, "Timestamp"))));
/* ========================================================================= */
// $ Util.AscNTime
// < Date:string=The timestamp converted to a string.
// ? Converts the current time into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AscNTime, 1, LCPUSHVAR(cmSys.FormatTime()));
/* ========================================================================= */
// $ Util.AscNTimeUTC
// < Date:string=The timestamp converted to a string.
// ? Converts the current time into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AscNTimeUTC, 1, LCPUSHVAR(cmSys.FormatTimeUTC()));
/* ========================================================================= */
// $ Util.FormatTime
// > Timestamp:integer=The timestamp to convert to string
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the specified formatted string. There may be
// ? slight differences across different operating systems. Note that time
// ? will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatTime, 1, LCPUSHVAR(StrFromTimeTT(
  LCGETINT(StdTimeT, 1, "Timestamp"), LCGETSTRING(char, 2, "StrFormat"))));
/* ========================================================================= */
// $ Util.FormatTimeUTC
// > Timestamp:integer=The timestamp to convert to string
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the specified formatted string. There may be
// ? slight differences across different operating systems. Note that time
// ? will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatTimeUTC, 1, LCPUSHVAR(StrFromTimeTTUTC(
  LCGETINT(StdTimeT, 1, "Timestamp"), LCGETSTRING(char, 2, "StrFormat"))));
/* ========================================================================= */
// $ Util.FormatNTime
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the current timestamp into the specified formatted string.
// ? There may be slight differences across different operating systems.
// ? Note that time will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatNTime, 1,
  LCPUSHVAR(cmSys.FormatTime(LCGETSTRING(char, 1, "StrFormat"))));
/* ========================================================================= */
// $ Util.FormatNTimeUTC
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the current timestamp into the specified formatted string. There
// ? may be slight differences across different operating systems. Note that
// ? time will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatNTimeUTC, 1,
  LCPUSHVAR(cmSys.FormatTimeUTC(LCGETSTRING(char, 1, "StrFormat"))));
/* ========================================================================= */
// $ Util.ParseTime
// > Format:string=The ISO 8601 formatted string.
// < Timestamp:integer=The timestamp converted from the string.
// ? Converts the specified ISO 8601 formatted string to a numerical timestamp.
// ? Only the format '2015-09-04T14:26:16Z' is supported at the moment.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ParseTime, 1,
  LCPUSHVAR(StrParseTime(LCGETCPPSTRINGNE(1, "ISO8601"))));
/* ========================================================================= */
// $ Util.ParseTimeEx
// > Timestamp:string=The specially formatted date string.
// > Format:string=The customised syntax of the formatted string.
// < Timestamp:integer=The timestamp converted from the string.
// ? Converts the specified specially formatted string to a numerical
// ? timestamp.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ParseTimeEx, 1,
  LCPUSHVAR(StrParseTime(LCGETCPPSTRINGNE(1, "Timestamp"),
    LCGETSTRING(char, 2, "StrFormat"))));
/* ========================================================================= */
// $ Util.ParseTime2
// > Format:string=The specially formatted date string.
// < Timestamp:integer=The timestamp converted from the string.
// ? Converts the specified specially formatted string to a numerical
// ? timestamp. Only the format 'Mon May 04 00:05:00 +0000 2009' is supported
// ? at the moment.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ParseTime2, 1,
  LCPUSHVAR(StrParseTime2(LCGETCPPSTRINGNE(1, "StrFormat"))));
/* ========================================================================= */
// $ Util.FormatNumber
// > Value:number=A number value, it will be converted to a string.
// > Precision:integer=Maximum number of floating point digits
// < Value:string=The formatted string.
// ? Uses the current OS locale settings to cleanly format a number with
// ? thousand separators.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatNumber, 1, LCPUSHVAR(StrReadableFromNum(
  LCGETNUM(double, 1, "Value"), LCGETINT(int, 2, "Digits"))));
/* ========================================================================= */
// $ Util.FormatNumberI
// > Value:integer=A integer value, it will be converted to a string.
// > Precision:integer=Maximum number of floating point digits
// < Value:string=The formatted string.
// ? Uses the current OS locale settings to cleanly format a number with
// ? thousand separators.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatNumberI, 1,
  LCPUSHVAR(StrReadableFromNum(LCGETINT(uint64_t, 1, "Value"), 0)));
/* ========================================================================= */
// $ Util.Round
// > Value:string=A number value
// > Precision:integer=Maximum number of floating point digits
// < Value:number=The rounded value
// ? Rounds the specified number to the specified number of floating point
// ? digits up or down to the nearest zero.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Round, 1, LCPUSHVAR(UtilRound<lua_Number>(
  LCGETNUM(double, 1, "Value"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.RoundInt
// > Value:string=A number value
// > Precision:integer=Maximum number of floating point digits
// < Value:integer=The rounded value
// ? Rounds the specified number to the specified number of floating point
// ? digits up or down to the nearest zero.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RoundInt, 1,
  LCPUSHVAR(UtilRound<lua_Integer>(LCGETNUM(double, 1, "Value"), 0)));
/* ========================================================================= */
// $ Util.Explode
// > Text:string=The text to tokenise.
// > Separator:string=The separator.
// < Table:table=The returned table.
// ? Splits the specified string into table entries on each matching interation
// ? of the specified separator.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Explode, 1, LuaUtilExplode(lS));
/* ========================================================================= */
// $ Util.ExplodeEx
// > Text:string=The text to tokenise.
// > Separator:string=The separator.
// > Maximum:integer=Maximum tokens.
// < Table:table=The returned table.
// ? Splits the specified string into table entries on each matching interation
// ? of the specified separator. The maximum value is the maximum number of
// ? tokens allowed so further occurences of the separator are not tokenised
// ? and included in the final tokens string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ExplodeEx, 1, LuaUtilExplodeEx(lS));
/* ========================================================================= */
// $ Util.Implode
// > Table:table=The table to convert to a string.
// > Separator:string=The separator.
// < String:string=The returned string.
// ? Appends each entry in the specified table to a string separated by the
// ? specified separator string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Implode, 1, LuaUtilImplode(lS));
/* ========================================================================= */
// $ Util.ImplodeEx
// > Table:table=The table to convert to a string.
// > Separator:string=The separator.
// > LastSep:string=The final separator (i.e. ' or' or ' and').
// < String:string=The returned string.
// ? Appends each entry in the specified table to a string separated by a
// ? 'Separator' and uses the specified 'LastSep' as the final separator to
// ? form a human reable string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ImplodeEx, 1, LuaUtilImplodeEx(lS));
/* ========================================================================= */
// $ Util.IfBlank
// > String:string=The string to test if empty.
// > Alternative:string=The string to use if blank.
// < Return:string=The returned string.
// ? If 'String' is blank or invalid, then 'StringAlt' is returned.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IfBlank, 1, LuaUtilIfBlank(lS));
/* ========================================================================= */
// $ Util.IsASCII
// > String:string=The string to test if empty.
// < Result:boolean=String is all ASCII.
// ? Returns if all characters in string are all less than 0x80. This call is
// ? UTF8 compatible.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IsASCII, 1,
  LCPUSHVAR(UtfDecoder(LCGETSTRING(char, 1, "String")).IsASCII()));
/* ========================================================================= */
// $ Util.IsExtASCII
// > String:string=The string to test if empty.
// < Result:boolean=String is all extended ASCII.
// ? Returns if all characters in string are all less than 0xFF. This call is
// ? UTF8 compatible.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IsExtASCII, 1,
  LCPUSHVAR(UtfDecoder(LCGETSTRING(char, 1, "String")).IsExtASCII()));
/* ========================================================================= */
// $ Util.Capitalise
// > String:string=The string to capitalise
// < Result:string=The same string in memory capitalised
// ? Returns the same string capitalised
/* ------------------------------------------------------------------------- */
LLFUNCEX(Capitalise, 1,
  LCPUSHVAR(StrCapitalise(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Util.ParseArgs
// > String:string=The arguments to parse.
// < Table:table=A table of arguments.
// ? Treats the specified string as a command line and returns a table entry
// ? for each argument.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ParseArgs, 1,
  LCTOTABLE(ArgsBuildSafe(LCGETCPPSTRING(1, "Arguments"))));
/* ========================================================================= */
// $ Util.ParseUrl
// > String:string=The url to parse
// < Result:integer=The result code of the parse
// < Protocol:string=The protocol string 'http' or 'https'
// < Secure:boolean=The conneciton would need to use SSL?
// < Host:string=The hostname to connect to
// < Port:integer=The port number to connect to
// < Resource:string=The resource to request
// < Bookmark:string=The bookmark part of the request
// < Username:string=The username part of the request
// < Password:string=The password part of the request
// ? Parses the specified url and returns all the information about it
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(ParseUrl)
  const Url uParsed{ LCGETCPPSTRING(1, "Url") };
  LCPUSHVAR(uParsed.GetResult());
  if(uParsed.GetResult() != Url::R_GOOD) return 1;
  LCPUSHVAR(uParsed.GetProtocol(), uParsed.GetSecure(),
            uParsed.GetHost(), uParsed.GetPort(),
            uParsed.GetResource(), uParsed.GetBookmark(),
            uParsed.GetUsername(), uParsed.GetPassword());
LLFUNCENDEX(9)
/* ========================================================================= */
// $ Util.Position
// > Position:integer=The position to convert to human readable form.
// < Result:string=The resulting string
// ? Converts the specified number into a positional number. i.e. 1=1st,
// ? 2="2nd", 11="11th", 98="98th", etc.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Position, 1,
  LCPUSHVAR(StrFromPosition(LCGETINT(uint64_t, 1, "Position"))));
/* ========================================================================= */
// $ Util.RelTime
// > Seconds:integer=The unix timestamp.
// < Result:string=The resulting string
// ? Subtracts the specified time with the current time and converts it to a
// ? human readable string showing the time duration between the two.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RelTime, 1,
  LCPUSHVAR(cmSys.ToDurationRel(LCGETINT(StdTimeT, 1, "Timestamp"))));
/* ========================================================================= */
// $ Util.RelTimeEx
// > Seconds:integer=The unix timestamp.
// > Count:integer=The maximum number of compomnents.
// < Result:string=The resulting string
// ? Subtracts the specified time with the current time and converts it to a
// ? human readable string showing the time duration between the two whilst
// ? giving the option to keep the string short and simple by limiting the
// ? specified number of components to show.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RelTimeEx, 1, LCPUSHVAR(cmSys.ToDurationRel(
  LCGETINT(StdTimeT,   1, "Timestamp"),
  LCGETINT(unsigned int, 2, "Components"))));
/* ========================================================================= */
// $ Util.Pluralise
// > Count:integer=The number to check
// > Singular:string=The word to suffix if the number is singular
// > Plural:string=The word to suffix if the number is plural
// < Result:string=The resulting number plus the suffixed text
// ? Checks the number and suffixes the appropriate word depending on if the
// ? supplied number is singular or plural.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Pluralise, 1, LCPUSHVAR(StrPluraliseNum(
  LCGETINT   (uint64_t, 1, "Value"), LCGETSTRING(char, 2, "Singular"),
  LCGETSTRING(char,     3, "Plural"))));
/* ========================================================================= */
// $ Util.PluraliseEx
// > Count:integer=The number to check
// > Singular:string=The word to suffix if the number is singular
// > Plural:string=The word to suffix if the number is plural
// < Result:string=The resulting number plus the suffixed text
// ? Checks the number and suffixes the appropriate word depending on if the
// ? supplied number is singular or plural. Uses the systems local settings
// ? to make sure the number is formatted properly.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PluraliseEx, 1, LCPUSHVAR(StrPluraliseNumEx(
  LCGETINT   (uint64_t, 1, "Value"), LCGETSTRING(char, 2, "Singular"),
  LCGETSTRING(char,     3, "Plural"))));
/* ========================================================================= */
// $ Util.Bytes
// > Count:integer=The number to convert.
// > Precision:integer=The precision after the decimal point to use
// < Result:string=The resulting string.
// ? Converts the specified number of bytes into a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Bytes, 1, LCPUSHVAR(StrToReadableBytes(
  LCGETINT(uint64_t, 1, "Bytes"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.Bits
// > Count:integer=The number to convert.
// > Precision:integer=The precision after the decimal point to use
// < Result:string=The resulting string.
// ? Converts the specified number of bits into a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Bits, 1, LCPUSHVAR(StrToReadableBits(
  LCGETINT(uint64_t, 1, "Bits"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.Grouped
// > Count:integer=The number to convert.
// > Precision:integer=The precision after the decimal point to use
// < Result:string=The resulting string.
// ? Converts the specified number into a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Grouped, 1, LCPUSHVAR(StrToReadableGrouped(
  LCGETINT(uint64_t, 1, "Bits"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.PlusOrMinus
// > Count:number=The number to convert.
// < Result:string=The resulting string.
// ? Prefixes the specified number with + or -.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PlusOrMinus, 1, LCPUSHVAR(StrPrefixPosNeg(
  LCGETNUM(double, 1, "Value"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.PlusOrMinusEx
// > Count:number=The number to convert.
// < Result:string=The resulting string.
// ? Prefixes the specified number with + or - and formats the number.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PlusOrMinusEx, 1, LCPUSHVAR(StrPrefixPosNegReadable(
  LCGETNUM(double, 1, "Value"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.WordWrap
// > Text:string=The text to word wrap
// > Maximum:integer=The maximum number of characters per line.
// > Indent:integer=The indentation to wrap at.
// < Lines:table=The resulting indexed table of lines.
// ? Word wraps the specified string into a table of lines.
/* ------------------------------------------------------------------------- */
LLFUNCEX(WordWrap, 1, LCTOTABLE(UtfWordWrap(
  LCGETCPPSTRING(1, "String"), LCGETINT(size_t, 2, "Width"),
  LCGETINT(size_t, 3, "Indent"))));
/* ========================================================================= */
// $ Util.Duration
// > Seconds:number=The number of seconds.
// > Precision:integer=The maximum number of floating point digits.
// < Result:string=The human readable duration string.
// ? Converts the specified number of seconds into a human readable duration
// ? string in the format of 000:00:00:00.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Duration, 1, LCPUSHVAR(StrShortFromDuration(
  LCGETNUM(double, 1, "Seconds"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.LDuration
// > Seconds:integer=The number of seconds.
// < Result:string=The human readable duration string.
// ? Converts the specified number of seconds into a human readable duration
// ? string in the format of e.g. 0 years 0 weeks 0 days 0 hours 0 mins 0 secs.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LDuration, 1,
  LCPUSHVAR(StrLongFromDuration(LCGETINT(StdTimeT, 1, "Seconds"))));
/* ========================================================================= */
// $ Util.LDurationEx
// > Seconds:integer=The number of seconds.
// > Components:integer=The maximum number of components
// < Result:string=The human readable duration string.
// ? Converts the specified number of seconds into a human readable duration
// ? string in the format of e.g. 0 years 0 weeks 0 days 0 hours 0 mins 0 secs.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LDurationEx, 1, LCPUSHVAR(StrLongFromDuration(
  LCGETINT(StdTimeT,   1, "Seconds"),
  LCGETINT(unsigned int, 2, "Components"))));
/* ========================================================================= */
// $ Util.GetRatio
// > Width:integer=The width number.
// > Height:integer=The height number
// < Result:string=The ratio between the two numbers.
// ? Caclulates the ratio between the two numbers and returns a string in the
// ? format of "n:n".
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetRatio, 1, LCPUSHVAR(StrFromRatio(
  LCGETINTLGE(double, 1, 1, UINT_MAX, "Width"),
  LCGETINTLGE(double, 2, 1, UINT_MAX, "Height"))));
/* ========================================================================= */
// $ Util.Clamp
// > Value:number=The current number.
// > Minimum:number=The minimum number.
// > Maximum:number=The maximum number.
// < Result:number=The clamped number.
// ? Makes sure the specified number does not go under the minimum or maximum
// ? numbers specified.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Clamp, 1, LCPUSHVAR(UtilClamp(
  LCGETNUM(lua_Number, 1, "Value"), LCGETNUM(lua_Number, 2, "Minimum"),
  LCGETNUM(lua_Number, 3, "Maximum"))));
/* ========================================================================= */
// $ Util.ClampInt
// > Value:integer=The current number.
// > Minimum:integer=The minimum number.
// > Maximum:integer=The maximum number.
// < Result:integer=The clamped number.
// ? Makes sure the specified number does not go under the minimum or maximum
// ? numbers specified.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ClampInt, 1, LCPUSHVAR(UtilClamp(
  LCGETINT(lua_Integer, 1, "Value"), LCGETINT(lua_Integer, 2, "Minimum"),
  LCGETINT(lua_Integer, 3, "Maximum"))));
/* ========================================================================= */
// $ Util.Compact
// > Value:string=The string to compact.
// < Result:integer=The compacted string.
// ? Removes all leading, trailing and grouped whitespaces from string so that
// ? each word has only one whitespace in between them.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Compact, 1, LCPUSHVAR(StrCompact(LCGETSTRING(char, 1, "String"))));
/* ========================================================================= */
// $ Util.Replace
// > String:string=The string to search from
// > Search:string=The string to find
// > Replace:string=The string to replace with
// < Result:string=The resulting string
// ? A simple search/replace text mechanism without having to worry about
// ? errors with escape sequences with string:gsub. Uses standard c++ string
// ? class functions for functionality.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Replace, 1, LCPUSHVAR(StrReplace(LCGETCPPSTRING(1, "String"),
  LCGETCPPSTRING(2, "Search"), LCGETCPPSTRING(3, "Replace"))));
/* ========================================================================= */
// $ Util.ReplaceEx
// > String:string=The string to search from
// > Data:table=Object with the replacement data
// < Result:string=The resulting string
// ? Same as Replace but takes an object table as argument which has key
// ? value pairs which are the text to replace (as key) and the text to replace
// ? to (as value).
/* ------------------------------------------------------------------------- */
LLFUNCEX(ReplaceEx, 1, LuaUtilReplaceMulti(lS));
/* ========================================================================= */
// $ Util.Trim
// > String:string=The string to modify
// > Search:integer=The ASCII character to remove
// < Result:string=The trimmed string
// ? Removes the specified character that are prefixed and suffixed to the
// ? specified string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Trim, 1, LCPUSHVAR(StrTrim(LCGETCPPSTRING(1, "String"),
  static_cast<char>(LCGETINTLGE(unsigned char, 2, 0, 255, "Char")))));
/* ========================================================================= */
// $ Util.StretchInner
// > OuterWidth:number=Outer width of the rectangle to adjust
// > OuterHeight:number=Outer height of the rectangle to adjust
// > InnerWidth:number=Inner width of the rectangle to adjust
// > InnerHeight:number=Inner height of the rectangle to adjust
// < OuterWidth:number=Outer width of the rectangle to adjust
// < OuterHeight:number=Outer height of the rectangle to adjust
// < InnerWidth:number=Inner width of the rectangle to adjust
// < InnerHeight:number=Inner height of the rectangle to adjust
// ? Scales the specified 2D inner rectangle to the outer bounds while
// ? preserving aspect ratio.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(StretchInner)
  double dOW = LCGETNUM(double, 1, "OuterWidth"),
         dOH = LCGETNUM(double, 2, "OuterHeight"),
         dIW = LCGETNUM(double, 3, "InnerWidth"),
         dIH = LCGETNUM(double, 4, "InnerHeight");
  UtilStretchToInner(dOW, dOH, dIW, dIH);
  LCPUSHVAR(dOW, dOH, dIW, dIH);
LLFUNCENDEX(4)
/* ========================================================================= */
// $ Util.StretchOuter
// > OuterWidth:number=Outer width of the rectangle to adjust
// > OuterHeight:number=Outer height of the rectangle to adjust
// > InnerWidth:number=Inner width of the rectangle to adjust
// > InnerHeight:number=Inner height of the rectangle to adjust
// < OuterWidth:number=Outer width of the rectangle to adjust
// < OuterHeight:number=Outer height of the rectangle to adjust
// < InnerWidth:number=Inner width of the rectangle to adjust
// < InnerHeight:number=Inner height of the rectangle to adjust
// ? Scales the specified 2D inner rectangle to the outer bounds while
// ? preserving aspect ratio.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(StretchOuter)
  double dOW = LCGETNUM(double, 1, "OuterWidth"),
         dOH = LCGETNUM(double, 2, "OuterHeight"),
         dIW = LCGETNUM(double, 3, "InnerWidth"),
         dIH = LCGETNUM(double, 4, "InnerHeight");
  UtilStretchToOuter(dOW, dOH, dIW, dIH);
  LCPUSHVAR(dOW, dOH, dIW, dIH);
LLFUNCENDEX(4)
/* ========================================================================= */
// $ Util.MkUTF8Char
// > Value:integer=Value to cast to a string
// < ByteCode:string=The 'value' cast as a string.
// ? Casts the specified 24-bit integer directly to a string. This is useful
// ? as a quicker way to create unicode characters. All bits above 24 are
// ? automatically stripped by a bitwise operation for safety.
/* ------------------------------------------------------------------------- */
LLFUNCEX(UTF8Char, 1,
  LCPUSHVAR(UtfDecodeNum(LCGETINT(uint32_t, 1, "Value"))));
/* ========================================================================= */
// $ Util.CountOf
// > Source:string=Source string to search
// > What:string=String to search for
// < Count:integer=Number of occurences in the string
// ? A fast way of counting the number of occurences in a string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CountOf, 1, LCPUSHVAR(StrCountOccurences(
  LCGETCPPSTRING(1, "Source"), LCGETCPPSTRING(2, "What"))));
/* ========================================================================= */
// $ Util.Hex
// > Value:integer=Integer to be converted to hex
// < Value:string=Same number but in hexadecimal
// ? Convert integer to hexadecimal
/* ------------------------------------------------------------------------- */
LLFUNCEX(Hex, 1, LCPUSHVAR(StrHexFromInt(LCGETINT(lua_Integer, 1, "Value"))));
/* ========================================================================= */
// $ Util.TableSize
// > Table:table=A key/value pairs table
// < Count:integer=Number of items in a table
// ? This function is only provided for convenience and should not normally
// ? be used. It enumerates a key/value pairs table and returns the number of
// ? items in it. It will also subtract the number of actual 1-indexed items
// ? if there are any present. It is recommended to manually record the table
// ? size yourself whilst building the key/value pairs table instead of using
// ? this.
/* ------------------------------------------------------------------------- */
LLFUNCEX(TableSize, 1, LCPUSHVAR(LuaUtilGetKeyValTableSize(lS)));
/* ========================================================================= */
// $ Util.Blank
// ? Just a blank function that does nothing
/* ------------------------------------------------------------------------- */
LLFUNC(Blank,);
/* ========================================================================= */
// $ Util.DecodeUUID
// > High64:integer=The high-order 128-bit integer.
// > Low64:integer=The low-order 128-bit integer.
// < Result:string=The decoded UUID.
// ? Decodes the specified UUID integers to a string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(DecodeUUID, 1, LCPUSHVAR(UuId{ LCGETINT(uint64_t, 1, "High"),
  LCGETINT(uint64_t, 2, "Low") }.UuIdToString()));
/* ========================================================================= */
// $ Util.EncodeUUID
// > UUID:string=The UUID string to recode.
// < High64:integer=The high-order 128-bit integer
// < Low64:integer=The low-order 128-bit integer
// ? Encodes the specified UUID string to two 64-bit integers.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(EncodeUUID)
  const UuId uuidData{ LCGETCPPSTRING(1, "String") };
  LCPUSHVAR(uuidData.d.qwRandom[0], uuidData.d.qwRandom[1]);
LLFUNCENDEX(2)
/* ========================================================================= */
// $ Util.RandUUID
// < High64:integer=The high-order 128-bit integer
// < Low64:integer=The low-order 128-bit integer
// ? Generates a random UUIDv4
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(RandUUID)
  const UuId uuidData;
  LCPUSHVAR(uuidData.d.qwRandom[0], uuidData.d.qwRandom[1]);
LLFUNCENDEX(2)
/* ========================================================================= */
// $ Util.MakeWord
// > High:integer=The high-order 8-bit range integer
// > Low:integer=The low-order 8-bit range integer
// < Result:integer=The resulting 16-bit range integer
// ? Joins two integers in the 8-bit range to return one integer in the 16-bit
// ? range.
/* ------------------------------------------------------------------------- */
LLFUNCEX(MakeWord, 1, LCPUSHVAR(UtilMakeWord(LCGETINT(uint8_t, 1, "High"),
                                             LCGETINT(uint8_t, 2, "Low"))));
/* ========================================================================= */
// $ Util.MakeDWord
// > High:integer=The high-order 16-bit range integer
// > Low:integer=The low-order 16-bit range integer
// < Result:integer=The resulting 32-bit range integer
// ? Joins two integers in the 16-bit range to return one integer in the 32-bit
// ? range.
/* ------------------------------------------------------------------------- */
LLFUNCEX(MakeDWord, 1, LCPUSHVAR(UtilMakeDWord(LCGETINT(uint16_t, 1, "High"),
                                               LCGETINT(uint16_t, 2, "Low"))));
/* ========================================================================= */
// $ Util.MakeQWord
// > High:integer=The high-order 32-bit range integer
// > Low:integer=The low-order 32-bit range integer
// < Result:integer=The resulting 64-bit range integer
// ? Joins two integers in the 32-bit range to return one integer in the 64-bit
// ? range.
/* ------------------------------------------------------------------------- */
LLFUNCEX(MakeQWord, 1, LCPUSHVAR(UtilMakeQWord(LCGETINT(uint32_t, 1, "High"),
                                               LCGETINT(uint32_t, 2, "Low"))));
/* ========================================================================= */
// $ Util.LowByte
// > Value:integer=The 16-bit ranged value to extract the lowest 8-bit value
// < Result:integer=The resulting 8-bit ranged value
// ? Extracts the lowest 8-bits from a 16-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LowByte, 1, LCPUSHVAR(UtilLowByte(LCGETINT(uint16_t, 1, "Low"))));
/* ========================================================================= */
// $ Util.LowWord
// > Value:integer=The 32-bit ranged value to extract the lowest 16-bit value
// < Result:integer=The resulting 16-bit ranged value
// ? Extracts the lowest 16-bits from a 32-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LowWord, 1, LCPUSHVAR(UtilLowWord(LCGETINT(uint32_t, 1, "Low"))));
/* ========================================================================= */
// $ Util.LowDWord
// > Value:integer=The 64-bit ranged value to extract the lowest 32-bit value
// < Result:integer=The resulting 32-bit ranged value
// ? Extracts the lowest 32-bits from a 64-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LowDWord, 1, LCPUSHVAR(UtilLowDWord(LCGETINT(uint64_t, 1, "Low"))));
/* ========================================================================= */
// $ Util.HighByte
// > Value:integer=The 16-bit ranged value to extract the highest 8-bit value
// < Result:integer=The resulting 8-bit ranged value
// ? Extracts the highest 8-bits from a 16-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HighByte, 1, LCPUSHVAR(UtilHighByte(LCGETINT(uint16_t, 1, "High"))));
/* ========================================================================= */
// $ Util.HighWord
// > Value:integer=The 32-bit ranged value to extract the highest 16-bit value
// < Result:integer=The resulting 16-bit ranged value
// ? Extracts the highest 16-bits from a 32-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HighWord, 1, LCPUSHVAR(UtilHighWord(LCGETINT(uint32_t, 1, "High"))));
/* ========================================================================= */
// $ Util.HighDWord
// > Value:integer=The 64-bit ranged value to extract the lowest 32-bit value
// < Result:integer=The resulting 32-bit ranged value
// ? Extracts the lowest 32-bits from a 64-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HighDWord, 1,
  LCPUSHVAR(UtilHighDWord(LCGETINT(uint64_t, 1, "High"))));
/* ========================================================================= */
// $ Util.RoundMul
// > Value:integer=The integer to round up to the nearest multiple.
// > Multiplier:integer=The nearest multiple in which to round up to.
// < Result:integer=The resulting rounded value.
// ? Rounds the specified integer up to the nearest multiple of the specified
// ? multiplier.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RoundMul, 1,
  LCPUSHVAR(UtilNearest(LCGETINT(lua_Integer, 1, "Value"),
                        LCGETINT(lua_Integer, 2, "Multiple"))));
/* ========================================================================= */
// $ Util.RoundPow2
// > Value:integer=The integer to round up to the nearest power of two
// < Result:integer=The resulting rounded value
// ? Rounds the specified integer up to the nearest power of two.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RoundPow2, 1,
  LCPUSHVAR(UtilNearestPow2<lua_Integer>(LCGETINT(lua_Integer, 1, "Value"))));
/* ========================================================================= **
** ######################################################################### **
** ## Util.* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Util.* namespace functions begin
  LLRSFUNC(AscNTime),      LLRSFUNC(AscNTimeUTC),   LLRSFUNC(AscTime),
  LLRSFUNC(AscTimeUTC),    LLRSFUNC(Bits),          LLRSFUNC(Blank),
  LLRSFUNC(Bytes),         LLRSFUNC(Capitalise),    LLRSFUNC(Clamp),
  LLRSFUNC(ClampInt),      LLRSFUNC(Compact),       LLRSFUNC(CountOf),
  LLRSFUNC(DecodeUUID),    LLRSFUNC(Duration),      LLRSFUNC(EncodeUUID),
  LLRSFUNC(Explode),       LLRSFUNC(ExplodeEx),     LLRSFUNC(FormatNTime),
  LLRSFUNC(FormatNTimeUTC),LLRSFUNC(FormatNumber),  LLRSFUNC(FormatNumberI),
  LLRSFUNC(FormatTime),    LLRSFUNC(FormatTimeUTC), LLRSFUNC(GetRatio),
  LLRSFUNC(Grouped),       LLRSFUNC(Hex),           LLRSFUNC(HighByte),
  LLRSFUNC(HighDWord),     LLRSFUNC(HighWord),      LLRSFUNC(IfBlank),
  LLRSFUNC(Implode),       LLRSFUNC(ImplodeEx),     LLRSFUNC(IsASCII),
  LLRSFUNC(IsExtASCII),    LLRSFUNC(LDuration),     LLRSFUNC(LDurationEx),
  LLRSFUNC(LowByte),       LLRSFUNC(LowDWord),      LLRSFUNC(LowWord),
  LLRSFUNC(MakeDWord),     LLRSFUNC(MakeQWord),     LLRSFUNC(MakeWord),
  LLRSFUNC(ParseArgs),     LLRSFUNC(ParseTime),     LLRSFUNC(ParseTime2),
  LLRSFUNC(ParseTimeEx),   LLRSFUNC(ParseUrl),      LLRSFUNC(Pluralise),
  LLRSFUNC(PluraliseEx),   LLRSFUNC(PlusOrMinus),   LLRSFUNC(PlusOrMinusEx),
  LLRSFUNC(Position),      LLRSFUNC(RandUUID),      LLRSFUNC(RelTime),
  LLRSFUNC(RelTimeEx),     LLRSFUNC(Replace),       LLRSFUNC(ReplaceEx),
  LLRSFUNC(Round),         LLRSFUNC(RoundInt),      LLRSFUNC(RoundMul),
  LLRSFUNC(RoundPow2),     LLRSFUNC(StretchInner),  LLRSFUNC(StretchOuter),
  LLRSFUNC(TableSize),     LLRSFUNC(Trim),          LLRSFUNC(UTF8Char),
  LLRSFUNC(WordWrap),
LLRSEND                                // Util.* namespace functions end
/* ========================================================================= */
}                                      // End of Util namespace
/* == EoF =========================================================== EoF == */
