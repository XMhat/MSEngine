/* == LLUTIL.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Util' namespace and methods for the guest to use in    ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Util
/* ------------------------------------------------------------------------- */
// ! This contains miscellenious utility functions such as string, number and
// ! time manipulation functions.
/* ========================================================================= */
LLNAMESPACEBEGIN(Util)                 // Util namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfClock;               // Using clock namespace
using namespace IfGlFW;                // Using glfw namespace
using namespace IfUuId;                // Using uuid namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Util.AscTime
// > Timestamp:integer=The timestamp to convert to string
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AscTime, 1,
  LCPUSHXSTR(FormatTimeTT(LCGETINT(StdTimeT, 1, "Timestamp"))));
/* ========================================================================= */
// $ Util.AscTimeUTC
// > Timestamp:integer=The timestamp to convert to string
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AscTimeUTC, 1,
  LCPUSHXSTR(FormatTimeTTUTC(LCGETINT(StdTimeT, 1, "Timestamp"))));
/* ========================================================================= */
// $ Util.AscNTime
// < Date:string=The timestamp converted to a string.
// ? Converts the current time into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AscNTime, 1, LCPUSHXSTR(cmSys.FormatTime()));
/* ========================================================================= */
// $ Util.AscNTimeUTC
// < Date:string=The timestamp converted to a string.
// ? Converts the current time into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(AscNTimeUTC, 1, LCPUSHXSTR(cmSys.FormatTimeUTC()));
/* ========================================================================= */
// $ Util.FormatTime
// > Timestamp:integer=The timestamp to convert to string
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the specified formatted string. There may be
// ? slight differences across different operating systems. Note that time
// ? will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatTime, 1, LCPUSHXSTR(FormatTimeTT(
  LCGETINT(StdTimeT, 1, "Timestamp"), LCGETSTRING(char, 2, "Format"))));
/* ========================================================================= */
// $ Util.FormatTimeUTC
// > Timestamp:integer=The timestamp to convert to string
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the specified formatted string. There may be
// ? slight differences across different operating systems. Note that time
// ? will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatTimeUTC, 1, LCPUSHXSTR(FormatTimeTTUTC(
  LCGETINT(StdTimeT, 1, "Timestamp"), LCGETSTRING(char, 2, "Format"))));
/* ========================================================================= */
// $ Util.FormatNTime
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the current timestamp into the specified formatted string.
// ? There may be slight differences across different operating systems.
// ? Note that time will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatNTime, 1,
  LCPUSHXSTR(cmSys.FormatTime(LCGETSTRING(char, 1, "Format"))));
/* ========================================================================= */
// $ Util.FormatNTimeUTC
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the current timestamp into the specified formatted string. There
// ? may be slight differences across different operating systems. Note that
// ? time will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatNTimeUTC, 1,
  LCPUSHXSTR(cmSys.FormatTimeUTC(LCGETSTRING(char, 1, "Format"))));
/* ========================================================================= */
// $ Util.ParseTime
// > Format:string=The ISO 8601 formatted string.
// < Timestamp:integer=The timestamp converted from the string.
// ? Converts the specified ISO 8601 formatted string to a numerical timestamp.
// ? Only the format '2015-09-04T14:26:16Z' is supported at the moment.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ParseTime, 1, LCPUSHINT(ParseTime(LCGETCPPSTRINGNE(1, "ISO8601"))));
/* ========================================================================= */
// $ Util.ParseTimeEx
// > Timestamp:string=The ISO 8601 formatted string.
// > Format:string=The customised syntax of the formatted string.
// < Timestamp:integer=The timestamp converted from the string.
// ? Converts the specified ISO 8601 formatted string to a numerical timestamp.
// ? Only the format '2015-09-04T14:26:16Z' is supported at the moment.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ParseTimeEx, 1, LCPUSHINT(ParseTime(LCGETCPPSTRINGNE(1, "Timestamp"),
  LCGETSTRING(char, 2, "Format"))));
/* ========================================================================= */
// $ Util.ParseTime2
// > Format:string=The specially formatted date string.
// < Timestamp:integer=The timestamp converted from the string.
// ? Converts the specified specially formatted string to a numerical
// ? timestamp. Only the format 'Mon May 04 00:05:00 +0000 2009' is supported
// ? at the moment.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ParseTime2, 1, LCPUSHINT(ParseTime2(LCGETCPPSTRINGNE(1, "Format"))));
/* ========================================================================= */
// $ Util.FormatNumber
// > Value:number=A number value, it will be converted to a string.
// > Precision:integer=Maximum number of floating point digits
// < Value:string=The formatted string.
// ? Uses the current OS locale settings to cleanly format a number with
// ? thousand separators.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatNumber, 1, LCPUSHXSTR(FormatNumber(
  LCGETNUM(double, 1, "Value"), LCGETINT(int, 2, "Digits"))));
/* ========================================================================= */
// $ Util.FormatNumber
// > Value:integer=A integer value, it will be converted to a string.
// > Precision:integer=Maximum number of floating point digits
// < Value:string=The formatted string.
// ? Uses the current OS locale settings to cleanly format a number with
// ? thousand separators.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FormatNumberI, 1,
  LCPUSHXSTR(FormatNumber(LCGETINT(uint64_t, 1, "Value"), 0)));
/* ========================================================================= */
// $ Util.Round
// > Value:string=A number value
// > Precision:integer=Maximum number of floating point digits
// < Value:number=The rounded value
// ? Rounds the specified number to the specified number of floating point
// ? digits up or down to the nearest zero.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Round, 1, LCPUSHNUM(Round<lua_Number>(
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
  LCPUSHINT(Round<lua_Integer>(LCGETNUM(double, 1, "Value"), 0)));
/* ========================================================================= */
// $ Util.Explode
// > Text:string=The text to tokenise.
// > Separator:string=The separator.
// < Table:table=The returned table.
// ? Splits the specified string into table entries on each matching interation
// ? of the specified separator.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Explode, 1, Explode(lS));
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
LLFUNCEX(ExplodeEx, 1, ExplodeEx(lS));
/* ========================================================================= */
// $ Util.Implode
// > Table:table=The table to convert to a string.
// > Separator:string=The separator.
// < String:string=The returned string.
// ? Appends each entry in the specified table to a string separated by the
// ? specified separator string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Implode, 1, Implode(lS));
/* ========================================================================= */
// $ Util.ImplodeReadable
// > Table:table=The table to convert to a string.
// > Separator:string=The separator.
// > LastSep:string=The final separator (i.e. ' or' or ' and').
// < String:string=The returned string.
// ? Appends each entry in the specified table to a string separated by a
// ? 'Separator' and uses the specified 'LastSep' as the final separator to
// ? form a human reable string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ImplodeEx, 1, ImplodeEx(lS));
/* ========================================================================= */
// $ Util.IfBlank
// > String:string=The string to test if empty.
// > StringAlt:string=The string to use if blank.
// < StringRet:string=The returned string.
// ? If 'String' is blank or invalid, then 'StringAlt' is returned.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IfBlank, 1, IfBlank(lS));
/* ========================================================================= */
// $ Util.IsASCII
// > String:string=The string to test if empty.
// < Result:boolean=String is all ASCII.
// ? Returns if all characters in string are all less than 0x80. This call is
// ? UTF8 compatible.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IsASCII, 1,
  LCPUSHBOOL(Decoder(LCGETSTRING(char, 1, "String")).IsASCII()));
/* ========================================================================= */
// $ Util.IsExtASCII
// > String:string=The string to test if empty.
// < Result:boolean=String is all extended ASCII.
// ? Returns if all characters in string are all less than 0xFF. This call is
// ? UTF8 compatible.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IsExtASCII, 1,
  LCPUSHBOOL(Decoder(LCGETSTRING(char, 1, "String")).IsExtASCII()));
/* ========================================================================= */
// $ Util.Capitalise
// > String:string=The string to capitalise
// < Result:string=The same string in memory capitalised
// ? Returns the same string capitalised
/* ------------------------------------------------------------------------- */
LLFUNCEX(Capitalise, 1,
  LCPUSHXSTR(Capitalise(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Util.ParseArgs
// > String:string=The arguments to parse.
// < Table:table=A table of arguments.
// ? Treats the specified string as a command line and returns a table entry
// ? for each argument.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ParseArgs, 1,
  LCTOTABLE(ArgumentsBuildSafe(LCGETCPPSTRING(1, "Arguments"))));
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
// ? Parses the specified url and returns all the information about it
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(ParseUrl)
  const Url uParsed{ LCGETCPPSTRING(1, "Url") };
  LCPUSHINT(uParsed.GetResult());
  if(uParsed.GetResult() != Url::R_GOOD) return 1;
  LCPUSHXSTR(uParsed.GetProtocol());
  LCPUSHBOOL(uParsed.GetSecure());
  LCPUSHXSTR(uParsed.GetHost());
  LCPUSHINT(uParsed.GetPort());
  LCPUSHXSTR(uParsed.GetResource());
  LCPUSHXSTR(uParsed.GetBookmark());
LLFUNCENDEX(7)
/* ========================================================================= */
// $ Util.Position
// > Position:integer=The position to convert to human readable form.
// < Result:string=The resulting string
// ? Converts the specified number into a positional number. i.e. 1=1st,
// ? 2="2nd", 11="11th", 98="98th", etc.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Position, 1,
  LCPUSHXSTR(ToPositionStr(LCGETINT(uint64_t, 1, "Position"))));
/* ========================================================================= */
// $ Util.RelTime
// > Seconds:integer=The unix timestamp.
// < Result:string=The resulting string
// ? Subtracts the specified time with the current time and converts it to a
// ? human readable string showing the time duration between the two.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RelTime, 1,
  LCPUSHXSTR(cmSys.ToDurationRel(LCGETINT(StdTimeT, 1, "Timestamp"))));
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
LLFUNCEX(RelTimeEx, 1, LCPUSHXSTR(cmSys.ToDurationRel(
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
LLFUNCEX(Pluralise, 1, LCPUSHXSTR(PluraliseNum(
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
LLFUNCEX(PluraliseEx, 1, LCPUSHXSTR(PluraliseNumEx(
  LCGETINT   (uint64_t, 1, "Value"), LCGETSTRING(char, 2, "Singular"),
  LCGETSTRING(char,     3, "Plural"))));
/* ========================================================================= */
// $ Util.Bytes
// > Count:integer=The number to convert.
// > Precision:integer=The precision after the decimal point to use
// < Result:string=The resulting string.
// ? Converts the specified number of bytes into a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Bytes, 1, LCPUSHXSTR(ToImbuedBytesStr(
  LCGETINT(uint64_t, 1, "Bytes"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.Bits
// > Count:integer=The number to convert.
// > Precision:integer=The precision after the decimal point to use
// < Result:string=The resulting string.
// ? Converts the specified number of bits into a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Bits, 1, LCPUSHXSTR(ToImbuedBitsStr(
  LCGETINT(uint64_t, 1, "Bits"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.Grouped
// > Count:integer=The number to convert.
// > Precision:integer=The precision after the decimal point to use
// < Result:string=The resulting string.
// ? Converts the specified number into a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Grouped, 1, LCPUSHXSTR(ToImbuedGroupedStr(
  LCGETINT(uint64_t, 1, "Bits"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.PlusOrMinus
// > Count:number=The number to convert.
// < Result:string=The resulting string.
// ? Prefixes the specified number with + or -.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PlusOrMinus, 1, LCPUSHXSTR(PlusOrMinus(
  LCGETNUM(double, 1, "Value"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.PlusOrMinusEx
// > Count:number=The number to convert.
// < Result:string=The resulting string.
// ? Prefixes the specified number with + or - and formats the number.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PlusOrMinusEx, 1, LCPUSHXSTR(PlusOrMinusEx(
  LCGETNUM(double, 1, "Value"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.WordWrap
// > Text:string=The text to word wrap
// > Maximum:integer=The maximum number of characters per line.
// > Indent:integer=The indentation to wrap at.
// < Lines:table=The resulting indexed table of lines.
// ? Word wraps the specified string into a table of lines.
/* ------------------------------------------------------------------------- */
LLFUNCEX(WordWrap, 1, LCTOTABLE(WordWrap(
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
LLFUNCEX(Duration, 1, LCPUSHXSTR(ToShortDuration(
  LCGETNUM(double, 1, "Seconds"), LCGETINT(int, 2, "Precision"))));
/* ========================================================================= */
// $ Util.LDuration
// > Seconds:integer=The number of seconds.
// < Result:string=The human readable duration string.
// ? Converts the specified number of seconds into a human readable duration
// ? string in the format of e.g. 0 years 0 weeks 0 days 0 hours 0 mins 0 secs.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LDuration, 1,
  LCPUSHXSTR(ToDuration(LCGETINT(StdTimeT, 1, "Seconds"))));
/* ========================================================================= */
// $ Util.LDurationEx
// > Seconds:integer=The number of seconds.
// > Components:integer=The maximum number of components
// < Result:string=The human readable duration string.
// ? Converts the specified number of seconds into a human readable duration
// ? string in the format of e.g. 0 years 0 weeks 0 days 0 hours 0 mins 0 secs.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LDurationEx, 1, LCPUSHXSTR(ToDuration(
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
LLFUNCEX(GetRatio, 1, LCPUSHXSTR(ToRatio(
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
LLFUNCEX(Clamp, 1, LCPUSHNUM(Clamp(
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
LLFUNCEX(ClampInt, 1, LCPUSHINT(Clamp(
  LCGETINT(lua_Integer, 1, "Value"), LCGETINT(lua_Integer, 2, "Minimum"),
  LCGETINT(lua_Integer, 3, "Maximum"))));
/* ========================================================================= */
// $ Util.ClipSet
// > Text:string=The text to copy.
// ? Stores the specified text into the operating system's clipboard.
/* ------------------------------------------------------------------------- */
LLFUNC(ClipSet, cGlFW->WinSetClipboard(LCGETSTRING(char, 1, "String")));
/* ========================================================================= */
// $ Util.ClipGet
// < Text:string=The text in the clipboard.
// ? Retrieves text from the operating systems clipboard. Returns a blank
// ? Returns a blank string if there is no string in the clipboard.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ClipGet, 1, LCPUSHSTR(cGlFW->WinGetClipboard()));
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
LLFUNCEX(Replace, 1, LCPUSHXSTR(Replace(LCGETCPPSTRING(1, "String"),
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
LLFUNCEX(ReplaceEx, 1, ReplaceMulti(lS));
/* ========================================================================= */
// $ Util.Trim
// > String:string=The string to modify
// > Search:integer=The ASCII character to remove
// < Result:string=The trimmed string
// ? Removes the specified character that are prefixed and suffixed to the
// ? specified string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Trim, 1, LCPUSHXSTR(Trim(LCGETCPPSTRING(1, "String"),
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
  double fdOW = LCGETNUM(double, 1, "OuterWidth"),
         fdOH = LCGETNUM(double, 2, "OuterHeight"),
         fdIW = LCGETNUM(double, 3, "InnerWidth"),
         fdIH = LCGETNUM(double, 4, "InnerHeight");
  StretchToInnerBounds(fdOW, fdOH, fdIW, fdIH);
  LCPUSHNUM(fdOW);
  LCPUSHNUM(fdOH);
  LCPUSHNUM(fdIW);
  LCPUSHNUM(fdIH);
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
  double fdOW = LCGETNUM(double, 1, "OuterWidth"),
         fdOH = LCGETNUM(double, 2, "OuterHeight"),
         fdIW = LCGETNUM(double, 3, "InnerWidth"),
         fdIH = LCGETNUM(double, 4, "InnerHeight");
  StretchToOuterBounds(fdOW, fdOH, fdIW, fdIH);
  LCPUSHNUM(fdOW);
  LCPUSHNUM(fdOH);
  LCPUSHNUM(fdIW);
  LCPUSHNUM(fdIH);
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
  LCPUSHXSTR(DecodeNum(LCGETINT(uint32_t, 1, "Value"))));
/* ========================================================================= */
// $ Util.CountOf
// > Source:string=Source string to search
// > What:string=String to search for
// < Count:integer=Number of occurences in the string
// ? A fast way of counting the number of occurences in a string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CountOf, 1, LCPUSHINT(CountOccurences(
  LCGETCPPSTRING(1, "Source"), LCGETCPPSTRING(2, "What"))));
/* ========================================================================= */
// $ Util.Hex
// > Value:integer=Integer to be converted to hex
// < Value:string=Same number but in hexadecimal
// ? Convert integer to hexadecimal
/* ------------------------------------------------------------------------- */
LLFUNCEX(Hex, 1, LCPUSHXSTR(ToHex(LCGETINT(lua_Integer, 1, "Value"))));
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
LLFUNCEX(TableSize, 1, LCPUSHINT(GetKeyValueTableSize(lS)));
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
LLFUNCEX(DecodeUUID, 1, LCPUSHXSTR(UuId(LCGETINT(uint64_t, 1, "High"),
  LCGETINT(uint64_t, 2, "Low")).UuIdToString()));
/* ========================================================================= */
// $ Util.EncodeUUID
// > UUID:string=The UUID string to recode.
// < High64:integer=The high-order 128-bit integer
// < Low64:integer=The low-order 128-bit integer
// ? Encodes the specified UUID string to two 64-bit integers.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(EncodeUUID)
  const UuId uuidData{ LCGETCPPSTRING(1, "String") };
  LCPUSHINT(uuidData.d.qwRandom[0]);
  LCPUSHINT(uuidData.d.qwRandom[1]);
LLFUNCENDEX(2)
/* ========================================================================= */
// $ Util.RandUUID
// < High64:integer=The high-order 128-bit integer
// < Low64:integer=The low-order 128-bit integer
// ? Generates a random UUIDv4
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(RandUUID)
  const UuId uuidData;
  LCPUSHINT(uuidData.d.qwRandom[0]);
  LCPUSHINT(uuidData.d.qwRandom[1]);
LLFUNCENDEX(2)
/* ========================================================================= */
// $ Util.MakeWord
// > High:integer=The high-order 8-bit range integer
// > Low:integer=The low-order 8-bit range integer
// < Result:integer=The resulting 16-bit range integer
// ? Joins two integers in the 8-bit range to return one integer in the 16-bit
// ? range.
/* ------------------------------------------------------------------------- */
LLFUNCEX(MakeWord, 1, LCPUSHINT(MakeWord(LCGETINT(uint8_t, 1, "High"),
                                         LCGETINT(uint8_t, 2, "Low"))));
/* ========================================================================= */
// $ Util.MakeDWord
// > High:integer=The high-order 16-bit range integer
// > Low:integer=The low-order 16-bit range integer
// < Result:integer=The resulting 32-bit range integer
// ? Joins two integers in the 16-bit range to return one integer in the 32-bit
// ? range.
/* ------------------------------------------------------------------------- */
LLFUNCEX(MakeDWord, 1, LCPUSHINT(MakeDWord(LCGETINT(uint16_t, 1, "High"),
                                           LCGETINT(uint16_t, 2, "Low"))));
/* ========================================================================= */
// $ Util.MakeQWord
// > High:integer=The high-order 32-bit range integer
// > Low:integer=The low-order 32-bit range integer
// < Result:integer=The resulting 64-bit range integer
// ? Joins two integers in the 32-bit range to return one integer in the 64-bit
// ? range.
/* ------------------------------------------------------------------------- */
LLFUNCEX(MakeQWord, 1, LCPUSHINT(MakeQWord(LCGETINT(uint32_t, 1, "High"),
                                           LCGETINT(uint32_t, 2, "Low"))));
/* ========================================================================= */
// $ Util.LowByte
// > Value:integer=The 16-bit ranged value to extract the lowest 8-bit value
// < Result:integer=The resulting 8-bit ranged value
// ? Extracts the lowest 8-bits from a 16-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LowByte, 1, LCPUSHINT(LowByte(LCGETINT(uint16_t, 1, "Low"))));
/* ========================================================================= */
// $ Util.LowWord
// > Value:integer=The 32-bit ranged value to extract the lowest 16-bit value
// < Result:integer=The resulting 16-bit ranged value
// ? Extracts the lowest 16-bits from a 32-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LowWord, 1, LCPUSHINT(LowWord(LCGETINT(uint32_t, 1, "Low"))));
/* ========================================================================= */
// $ Util.LowDWord
// > Value:integer=The 64-bit ranged value to extract the lowest 32-bit value
// < Result:integer=The resulting 32-bit ranged value
// ? Extracts the lowest 32-bits from a 64-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(LowDWord, 1, LCPUSHINT(LowDWord(LCGETINT(uint64_t, 1, "Low"))));
/* ========================================================================= */
// $ Util.HighByte
// > Value:integer=The 16-bit ranged value to extract the highest 8-bit value
// < Result:integer=The resulting 8-bit ranged value
// ? Extracts the highest 8-bits from a 16-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HighByte, 1, LCPUSHINT(HighByte(LCGETINT(uint16_t, 1, "High"))));
/* ========================================================================= */
// $ Util.HighWord
// > Value:integer=The 32-bit ranged value to extract the highest 16-bit value
// < Result:integer=The resulting 16-bit ranged value
// ? Extracts the highest 16-bits from a 32-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HighWord, 1, LCPUSHINT(HighWord(LCGETINT(uint32_t, 1, "High"))));
/* ========================================================================= */
// $ Util.HighDWord
// > Value:integer=The 64-bit ranged value to extract the lowest 32-bit value
// < Result:integer=The resulting 32-bit ranged value
// ? Extracts the lowest 32-bits from a 64-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HighDWord, 1, LCPUSHINT(HighDWord(LCGETINT(uint64_t, 1, "High"))));
/* ========================================================================= */
// $ Util.RoundMul
// > Value:integer=The integer to round up to the nearest multiple.
// > Multiplier:integer=The nearest multiple in which to round up to.
// < Result:integer=The resulting rounded value.
// ? Rounds the specified integer up to the nearest multiple of the specified
// ? multiplier.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RoundMul, 1,
  LCPUSHINT(Nearest(LCGETINT(lua_Integer, 1, "Value"),
                    LCGETINT(lua_Integer, 2, "Multiple"))));
/* ========================================================================= */
// $ Util.RoundPow2
// > Value:integer=The integer to round up to the nearest power of two
// < Result:integer=The resulting rounded value
// ? Rounds the specified integer up to the nearest power of two.
/* ------------------------------------------------------------------------- */
LLFUNCEX(RoundPow2, 1,
  LCPUSHINT(NearestPow2<lua_Integer>(LCGETINT(lua_Integer, 1, "Value"))));
/* ========================================================================= */
/* ######################################################################### */
/* ## Util.* namespace functions structure                              ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Util.* namespace functions begin
  LLRSFUNC(AscTime),                   // Autoformat specified time
  LLRSFUNC(AscTimeUTC),                // Autoformat specified time as UTC
  LLRSFUNC(AscNTime),                  // Autoformat current time
  LLRSFUNC(AscNTimeUTC),               // Autoformat Current time as UTC
  LLRSFUNC(Bits),                      // Get readable bytes string
  LLRSFUNC(Blank),                     // Just a blank function
  LLRSFUNC(Bytes),                     // Get readable bytes string
  LLRSFUNC(Capitalise),                // Capitalise the specified string
  LLRSFUNC(Clamp),                     // Clamp a number
  LLRSFUNC(ClampInt),                  // Clamp an integer
  LLRSFUNC(ClipGet),                   // Get string from clipboard
  LLRSFUNC(ClipSet),                   // Set string to clipboard
  LLRSFUNC(CountOf),                   // Count occurences in string
  LLRSFUNC(DecodeUUID),                // Decode two UUID ints to string
  LLRSFUNC(Duration),                  // Make short duration string from float
  LLRSFUNC(EncodeUUID),                // Encode UUID string to two UUID ints
  LLRSFUNC(Explode),                   // Explode string to table
  LLRSFUNC(ExplodeEx),                 // Explode string to table limited match
  LLRSFUNC(FormatNumber),              // Format number to locale
  LLRSFUNC(FormatNumberI),             // Format integer to locale
  LLRSFUNC(FormatNTime),               // Format current time
  LLRSFUNC(FormatNTimeUTC),            // Format Current time as UTC
  LLRSFUNC(FormatTime),                // Format specified time
  LLRSFUNC(FormatTimeUTC),             // Format specified time as UTC
  LLRSFUNC(GetRatio),                  // Convert dimensions
  LLRSFUNC(Grouped),                   // Get readable string
  LLRSFUNC(Hex),                       // Convert integer to hexadecimal
  LLRSFUNC(HighByte),                  // Return highest 8-bit of a 16-bit
  LLRSFUNC(HighDWord),                 //   "           32-bit of a 64-bit
  LLRSFUNC(HighWord),                  //   "           16-bit of a 32-bit
  LLRSFUNC(IfBlank),                   // Return altstring if string empty
  LLRSFUNC(IsASCII),                   // Return if all chars <= 0x7F
  LLRSFUNC(IsExtASCII),                // Return if all chars <= 0xFF
  LLRSFUNC(Implode),                   // Implode table to string
  LLRSFUNC(ImplodeEx),                 // Advanced implode table to string
  LLRSFUNC(LDuration),                 // Make long duration string (num | int)
  LLRSFUNC(LDurationEx),               //   " with component limit (num | int)
  LLRSFUNC(LowByte),                   // Return lowest 8-bit of a 16-bit
  LLRSFUNC(LowDWord),                  // Return lowest 32-bit of a 64-bit
  LLRSFUNC(LowWord),                   // Return lowest 16-bit of a 32-bit
  LLRSFUNC(MakeWord),                  // Join two 8-bits to make a 16-bit
  LLRSFUNC(MakeDWord),                 // Join two 16-bits to make a 32-bit
  LLRSFUNC(MakeQWord),                 // Join two 32-bits to make a 64-bit
  LLRSFUNC(ParseArgs),                 // Parse cmd str to table
  LLRSFUNC(ParseTime),                 // Parse ISO8601 time string
  LLRSFUNC(ParseTime2),                // Parse special time string
  LLRSFUNC(ParseTimeEx),               // Parse custom time string
  LLRSFUNC(ParseUrl),                  // Parse an url
  LLRSFUNC(Pluralise),                 // Pluralise specified integer
  LLRSFUNC(PluraliseEx),               //   " and format
  LLRSFUNC(PlusOrMinus),               // Add + if number positive
  LLRSFUNC(PlusOrMinusEx),             //   " and format
  LLRSFUNC(Position),                  // Make position from int
  LLRSFUNC(RandUUID),                  // Return random UUID
  LLRSFUNC(RelTime),                   // Get relative time string
  LLRSFUNC(RelTimeEx),                 //   " with component limit
  LLRSFUNC(Replace),                   // Simple replace text
  LLRSFUNC(ReplaceEx),                 // Advanced replace text
  LLRSFUNC(Round),                     // Round up specified number
  LLRSFUNC(RoundInt),                  // Round specified number to int
  LLRSFUNC(RoundMul),                  // Round up to specified multiplier
  LLRSFUNC(RoundPow2),                 // Round up to nearest power of two
  LLRSFUNC(StretchInner),              // Stretch to inner bounds
  LLRSFUNC(StretchOuter),              // Stretch to outer bounds
  LLRSFUNC(TableSize),                 // Get size of a key/value table
  LLRSFUNC(Trim),                      // Trim off characters at end
  LLRSFUNC(UTF8Char),                  // Makes a UTF8 character from int
  LLRSFUNC(WordWrap),                  // Word wrap a string
LLRSEND                                // Util.* namespace functions end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Util namespace
/* == EoF =========================================================== EoF == */
