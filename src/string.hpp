/* == STRING.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Miscaelennias string utility functions.                             ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfString {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfStd;                 // Using std namespace
/* -- Some helpful globals so not to repeat anything ----------------------- */
static const string     strBlank, strSpace{" "};     // Blank C++ String
static const char*const cpBlank = strBlank.c_str(),  // Blank C String
  *const cpTimeFormat = "%a %b %d %H:%M:%S %Y %z";   // Default time format
/* -- Locale singleton ----------------------------------------------------- */
static const locale &GetLocaleSingleton(void)
  { static const locale lLocaleCurrent{ strBlank }; return lLocaleCurrent; }
/* -- Append final parameter (uses copy elision) --------------------------- */
static void AppendParam(ostringstream&) { }
/* -- Append a parameter (uses copy elision) ------------------------------- */
template<typename AnyType, typename... VarArgsType>
  static void AppendParam(ostringstream &osS, const AnyType &atVal,
    const VarArgsType... vatArgs)
      { osS << atVal; AppendParam(osS, vatArgs...); }
/* -- Append main function ------------------------------------------------- */
template<typename... V>static const string Append(const V... vV)
{ // Stream to write to
  ostringstream osS;
  // Build string
  AppendParam(osS, vV...);
  // Return string
  return osS.str();
}
/* -- Append with formatted numbers ---------------------------------------- */
template<typename... V>static const string AppendImbued(const V... vV)
{ // Stream to write to
  ostringstream osS;
  // Imbue current locale
  osS.imbue(GetLocaleSingleton());
  // Build string
  AppendParam(osS, vV...);
  // Return appended string
  return osS.str();
}
/* -- Append final parameter (uses copy elision) --------------------------- */
static void FormatParam(ostringstream &osS, const char *cpPos)
  { if(*cpPos) osS << cpPos; }
/* -- Process any value ------------------------------------------------- -- */
template<typename T, typename... V>
  static void FormatParam(ostringstream &osS, const char *cpPos,
    const T& tVal, const V... vV)
{ // Find the mark that will be replaced by this parameter and if we
  // find the character?
  if(const char*const cpNewPos = strchr(cpPos, '$'))
  { // How far did we find the new position
    switch(const size_t stNum = static_cast<size_t>(cpNewPos - cpPos))
    { // One character? Just copy one character and move ahead two to skip
      // over the '$' we just processed.
      case 1: osS << *cpPos; cpPos += 2; break;
      // More than one character? Copy characters and stride over the '$'
      // we just processed. Better than storing single characters.
      default: osS << std::move(string{ cpPos, stNum });
               cpPos += stNum + 1;
               break;
      // Did not move? This can happen at the start of the string. Just
      // move over the first '$'.
      case 0: ++cpPos; break;
    } // Push the value we are supposed to replace the matched '$' with.
    osS << tVal;
    // Process more parameters if we can.
    FormatParam(osS, cpPos, vV...);
  } // Return the rest of the string.
  else FormatParam(osS, cpPos);
}
/* -- Prepare message from c-string format --------------------------------- */
template<typename... V>
  static const string Format(const char*const cpP, const V... vV)
{ // Return if string empty of invalid
  if(!IfUtf::IsCStringValid(cpP)) return {};
  // Stream to write to
  ostringstream osS;
  // Format the text
  FormatParam(osS, cpP, vV...);
  // Return formated text
  return osS.str();
}
/* == Format a number ====================================================== */
template<typename T>
  static const string FormatNumber(const T tV, const int iP=0)
    { return std::move(AppendImbued(fixed, setprecision(iP), tV)); }
/* -- Trim specified characters from string -------------------------------- */
static const string Trim(const string &strS, const char cC)
{ // Return empty string if source string is empty
  if(strS.empty()) return strS;
  // Calculate starting misoccurance of character. Return original if not found
  const size_t stBegin = strS.find_first_not_of(cC);
  if(stBegin == string::npos) return strS;
  // Calculate ending misoccurance of character then copy and return the string
  return strS.substr(stBegin, strS.find_last_not_of(cC) - stBegin + 1);
}
/* -- Convert integer to string with padding and precision ----------------- */
template<typename IntType>static const string ToString(const IntType itV,
  const int iW=0, const int iPrecision=numeric_limits<IntType>::digits10)
    { return std::move(Append(setw(iW), fixed, setprecision(iPrecision),
        itV)); }
/* -- Quickly convert numbered string to integer --------------------------- */
template<typename IntType=int64_t>
  static const IntType ToNumber(const string &strValue)
{ // Value to store into
  IntType itN;
  // Put value into input string stream
  istringstream isS{ strValue };
  // Push value into integer
  isS >> itN;
  // Return result
  return itN;
}
/* -- Quickly convert hex string to integer ------------------------------== */
template<typename IntType=int64_t>
  static const IntType HexToNumber(const string &strValue)
{ // Value to store into
  IntType itN;
  // Put value into input string stream
  istringstream isS{ strValue };
  // Push value into integer
  isS >> hex >> itN;
  // Return result
  return itN;
}
/* -- Convert hex to string with zero padding ------------------------------ */
template<typename T>static const string ToHex(const T tV, const int iP=0)
  { return std::move(Append(setfill('0'), hex, setw(iP), tV)); }
/* -- Return if specified string has numbers ------------------------------- */
static bool IsAlpha(const string &strValue)
{ // Return if any value isn't an letter
  for(const char cValue : strValue)
    if(!isalpha(static_cast<int>(cValue))) return false;
  // Success
  return true;
}
/* -- Return if specified string has numbers ------------------------------- */
static bool IsAlphaNumeric(const string &strValue)
{ // Return if any value isn't an letter
  for(const char cValue : strValue)
    if(!isalnum(static_cast<int>(cValue))) return false;
  // Success
  return true;
}
/* -- Return if specified string is a valid integer ------------------------ */
template<typename IntType=int64_t>static bool IsNumber(const string &strValue)
{ // Get string stream
  istringstream isS{ strValue };
  // Test with string stream
  IntType itV; isS >> noskipws >> itV;
  // Return if succeeded
  return isS.eof() && !isS.fail();
}
/* -- Return if specified string is a valid float -------------------------- */
static bool IsFloat(const string &strValue)
  { return IsNumber<double>(strValue); }
/* -- Returns if the specified number is a power of two -------------------- */
template<typename IntType=int64_t>static bool IsPow2(const IntType itVal)
    { return !((itVal & (itVal - 1)) && itVal); }
/* -- Return absolute number ----------------------------------------------- */
template<typename IntType=int64_t>
  static IntType Absolute(const IntType itVal)
{ // Check if supplied argument is signed and negate it if true
  if constexpr(is_signed_v<IntType>) return (itVal < 0) ? -itVal : itVal;
  // Else just return the value without any processing
  else return itVal;
}
/* -- Return true if string is a value number to the power of 2 ------------ */
static bool IsNumberPOW2(const string &strValue)
  { return !strValue.empty() && IsPow2(Absolute(ToNumber(strValue))); }
/* -- Return true if string is a value number to the power of 2 ------------ */
static bool IsNumberPOW2Zero(const string &strValue)
{ // Failed if empty
  if(strValue.empty()) return false;
  // Convert string to number and return positive if value is zero or power 2
  const int64_t qwVal = Absolute(ToNumber(strValue));
  return !qwVal || IsPow2(qwVal);
}
/* -- Set error number ----------------------------------------------------- */
static void SetErrNo(const int iValue) { errno = iValue; }
/* -- Get error number ----------------------------------------------------- */
static int GetErrNo(void) { return errno; }
/* -- Is error number equal to --------------------------------------------- */
static bool IsErrNo(const int iValue) { return GetErrNo() == iValue; }
/* -- Is error number not equal to ----------------------------------------- */
static bool IsNotErrNo(const int iValue) { return !IsErrNo(iValue); }
/* -- Convert error number to string --------------------------------------- */
static const string LocalError(const int iErrNo=errno)
{ // Buffer to store error message into
  string strErr; strErr.resize(128);
  // Windows?
#if defined(WINDOWS)
  // 'https://msdn.microsoft.com/en-us/library/51sah927.aspx' says:
  // "Your string message can be, at most, 94 characters long."
  if(strerror_s(const_cast<char*>(strErr.c_str()), strErr.capacity(), iErrNo))
    strErr.assign(Append("Error ", iErrNo));
  // OSX?
#elif defined(MACOS)
  // Grab the error result and if failed? Just put in the error number continue
  if(strerror_r(iErrNo, const_cast<char*>(strErr.c_str()), strErr.capacity()))
    strErr.assign(Append("Error ", iErrNo));
  // Linux?
#elif defined(LINUX)
  // Grab the error result and if failed? Set a error and continue
  const char*const cpResult =
    strerror_r(iErrNo, const_cast<char*>(strErr.c_str()), strErr.capacity());
  if(!cpResult) strErr = Append("Error ", iErrNo);
  // We got a message but if was not put in our buffer just return as is
  else if(cpResult != strErr.c_str()) return cpResult;
#endif
  // Resize and compact the buffer
  strErr.resize(strlen(strErr.c_str()));
  strErr.shrink_to_fit();
  // Have to do this because the string is still actually 94 bytes long
  return strErr;
}
/* -- Helper plugin for C runtime errno checking --------------------------- */
struct ErrorPluginStandard final
{ /* -- Exception class helper macro for C runtime errors ------------------ */
#define XCL(r,...) throw Error<ErrorPluginStandard>(r, ## __VA_ARGS__)
  /* -- Constructor to add C runtime error code ---------------------------- */
  explicit ErrorPluginStandard(ostringstream &osS)
    { osS << "\n+ Reason<" << GetErrNo() << "> = \""
          << LocalError() << "\"."; }
};/* ----------------------------------------------------------------------- */
/* -- Convert special formatted string to unix timestamp ------------------- */
static StdTimeT ParseTime2(const string &strS)
{ // Time structure
  StdTMStruct tData;
  // Scan timestamp into time structure (Don't care about day name). We'll
  // store the timezone in tm_isdst and we'll optimise this by storing the
  // month string in the actual month integer var (4 or 8 bytes so safe).
  // Fmt: %3s %3s %02d %02d:%02d:%02d %05d %04d
  // Test example to just quickly copy and paste in the engine...
  // lexec 'Console.Write(Util.ParseTime2("Mon Mar 14 00:00:00 -0800 2017"));'
  istringstream isS{ strS };
  isS >> get_time(&tData, "%a %b %d %T");
  if(isS.fail()) return 0;
  isS >> tData.tm_wday;
  isS >> get_time(&tData, "%Y");
  if(isS.fail()) return 0;
  // No daylight savings
  tData.tm_isdst = 0;
  // Return timestamp and adjust for specified timezone if neccesary
  return StdMkTime(&tData) + (!tData.tm_wday ? 0 : ((tData.tm_wday < 0 ?
    ((tData.tm_wday % 100) * 60) : -((tData.tm_wday % 100) * 60)) +
    ((tData.tm_wday / 100) * 3600)));
}
/* -- Convert ISO 8601 string to unix timestamp ---------------------------- */
static StdTimeT ParseTime(const string &strS,
  const char*const cpF="%Y-%m-%dT%TZ")
{ // Time structure
  StdTMStruct tData;
  // Create static input stringstream (safe and fast in c++11)
  istringstream isS{ strS };
  // Scan timestamp into time structure
  isS >> get_time(&tData, cpF);
  if(isS.fail()) return 0;
  // Fill in other useless junk in the struct
  tData.tm_isdst = 0;
  // Return timestamp
  return StdMkTime(&tData);
}
/* -- Convert writable reference string to uppercase ----------------------- */
static string &ToUpperRef(string &strStr)
{ // If string is not empty
  if(!strStr.empty())
    MYTRANSFORM(par_unseq, strStr.begin(), strStr.end(),
      strStr.begin(), [](char cChar)->char
        { return static_cast<char>(toupper(static_cast<int>(cChar))); });
  // Return output
  return strStr;
}
/* -- Convert writable referenced string to lowercase ---------------------- */
static string &ToLowerRef(string &strStr)
{ // If string is not empty
  if(!strStr.empty())
    MYTRANSFORM(par_unseq, strStr.begin(), strStr.end(),
      strStr.begin(), [](char cChar)->char
        { return static_cast<char>(tolower(static_cast<int>(cChar))); });
  // Return output
  return strStr;
}
/* -- Basic multiple replace of text in string ----------------------------- */
template<class ListType=StrPairList>
  static string &ReplaceEx(string &strDest, const ListType &ltList)
{ // Return original string if empty
  if(strDest.empty() || ltList.empty()) return strDest;
  // Current index to scan
  size_t stPos = 0;
  // Repeat...
  do
  { // Enumerate each occurence to find...
    for(const auto &spItem : ltList)
    { // Get string to find
      const string &strWhat = spItem.first;
      // Last cut position and current character index
      if(strncmp(strDest.c_str()+stPos, strWhat.data(), strWhat.length()))
        continue;
      // Get string to replace with
      const string &strWith = spItem.second;
      // Replace the occurence with the specified text
      strDest.replace(stPos, strWhat.length(), strWith);
      // Go forward so we can search for the next occurence
      stPos += strWith.length();
      // Start again
      goto NextCharacter;
    } // Occurences not found
    ++stPos;
    // Occurence found
    NextCharacter:;
  } // ...until no more characters left
  while(stPos < strDest.length());
  // Return the string we build
  return strDest;
}
/* -- Basic replace of text in string -------------------------------------- */
static string &Replace(string &strStr, const char cWhat, const char cWith)
{ // Return original string if empty
  if(strStr.empty()) return strStr;
  // For each occurence of 'strWhat' with 'strWith'.
  for(size_t stPos  = strStr.find(cWhat, 0);
             stPos != string::npos;
             stPos  = strStr.find(cWhat, stPos)) strStr[stPos++] = cWith;
  // Return string
  return strStr;
}
/* -- Basic replace of text in string -------------------------------------- */
static string Replace(const string &strStr, const char cWhat, const char cWith)
  { string strDst{ strStr }; return Replace(strDst, cWhat, cWith); }
/* ------------------------------------------------------------------------- */
static string &Replace(string &strDest, const string &strWhat,
  const string &strWith)
{ // Return original string if empty
  if(strDest.empty()) return strDest;
  // For each occurence of 'strWhat' with 'strWith'.
  for(size_t stPos  = strDest.find(strWhat,0);
             stPos != string::npos;
             stPos  = strDest.find(strWhat, stPos))
  { // Replace occurence
    strDest.replace(stPos, strWhat.length(), strWith);
    // Push position forward so we don't risk infinite loop
    stPos += strWith.length();
  } // Return string
  return strDest;
}
/* -- Basic replace of text in string -------------------------------------- */
static string Replace(const string &strIn, const string &strWhat,
  const string &strWith)
    { string strOut{ strIn }; return Replace(strOut, strWhat, strWith); }
/* -- Replace all occurences of whitespace with plus ----------------------- */
static const string SpaceEncode(const string &strText)
  { return Replace(strText, ' ', '+'); }
/* ------------------------------------------------------------------------- */
static const char *IfNull(const char*const cpIn,
  const char*const cpAlt = cpBlank)
    { return cpIn != nullptr ? cpIn : cpAlt; }
static const char *IfBlank(const string &strIn,
  const char*const cpAlt = cpBlank)
    { return strIn.empty() ? cpAlt : strIn.c_str(); }
/* ------------------------------------------------------------------------- */
template<typename T>static const char *Pluralise(const T tCount,
  const char*const cpSingular, const char*const cpPlural)
    { return tCount == 1 ? cpSingular : cpPlural; }
/* ------------------------------------------------------------------------- */
template<typename T>static const string PluraliseNum(const T tCount,
  const char*const cpSingular, const char*const cpPlural)
   { return std::move(Append(tCount, ' ',
       std::move(Pluralise(tCount, cpSingular, cpPlural)))); }
/* ------------------------------------------------------------------------- */
template<typename T>static const string PluraliseNumEx(const T tCount,
  const char*const cpSingular, const char*const cpPlural)
   { return std::move(Append(FormatNumber(tCount), ' ',
       std::move(Pluralise(tCount, cpSingular, cpPlural)))); }
/* -- Convert time to long duration ---------------------------------------- */
static const string ToDuration(const StdTimeT tDuration,
  unsigned int uiCompMax = numeric_limits<unsigned int>::max())
{ // Time buffer
  StdTMStruct tD;
  // Lets convert the duration as a time then it will be properly formated
  // in terms of leap years, proper days in a month etc.
  StdGMTime(&tD, &tDuration);
  // Output string
  ostringstream osS;
  // If failed? Manually do it
  if(tD.tm_year == -1)
  { // Clear years and months since we can't realiably calculate that.
    tD.tm_year = tD.tm_mon = 0;
    // Set days, hours, minutes and seconds
    tD.tm_mday = static_cast<int>(tDuration / 86400);
    tD.tm_hour = static_cast<int>(tDuration / 3600 % 24);
    tD.tm_min = static_cast<int>(tDuration / 60 % 60);
    tD.tm_sec = static_cast<int>(tDuration % 60);
  } // Succeeded, subtract 70 as it returns as years past 1900.
  else tD.tm_year -= 70;
  // Add years?
  if(tD.tm_year && uiCompMax > 0)
  { // Do add years
    osS << PluraliseNum(tD.tm_year, "year", "years");
    --uiCompMax;
  } // Add months?
  if(tD.tm_mon && uiCompMax > 0)
  { // Do add months
    osS << (osS.tellp() ? strSpace : strBlank)
        << PluraliseNum(tD.tm_mon, "month", "months");
    --uiCompMax;
  } // Add days? (removing the added 1)
  if(--tD.tm_mday && uiCompMax > 0)
  { // Do add days
    osS << (osS.tellp() ? strSpace : strBlank)
        << PluraliseNum(tD.tm_mday, "day", "days");
    --uiCompMax;
  } // Add hours?
  if(tD.tm_hour && uiCompMax > 0)
  { // Do add hours
    osS << (osS.tellp() ? strSpace : strBlank)
        << PluraliseNum(tD.tm_hour, "hour", "hours");
    --uiCompMax;
  } // Add Minutes?
  if(tD.tm_min && uiCompMax > 0)
  { // Do add minutes
    osS << (osS.tellp() ? strSpace : strBlank)
        << PluraliseNum(tD.tm_min, "min", "mins");
    --uiCompMax;
  } // Check seconds
  if((tD.tm_sec || !tDuration) && uiCompMax > 0)
    osS << (osS.tellp() ? strSpace : strBlank)
        << PluraliseNum(tD.tm_sec, "sec", "secs");
  // Return string
  return osS.str();
}
/* ------------------------------------------------------------------------- */
static const char *ToPosition(const uint64_t qPosition)
{ // Get value as base 100
  const uint64_t qVb100 = qPosition % 100;
  // Number not in teens? Compare value as base 10 instead
  if(qVb100 <= 10 || qVb100 >= 20) switch(qPosition % 10)
  { case 1: return "st"; case 2: return "nd"; case 3: return "rd";
    default: break;
  } // Everything else is 'th'
  return "th";
} /* -- Get position of number as a string --------------------------------- */
static const string ToPositionStr(const uint64_t qPosition)
  { return Append(qPosition, ToPosition(qPosition)); }
/* -- Capitalise a string -------------------------------------------------- */
static const string Capitalise(const string &strStr)
{ // Capitalise first character if string not nullptr or empty
  if(strStr.empty()) return strStr;
  // Duplicate the string anad uppercase the first character
  string strNew{ strStr };
  strNew[0] = static_cast<char>(toupper(strStr.front()));
  // Return provided string
  return strNew;
}
/* -- Evaluate a list of booleans and return a character value ------------- */
static const string
  EvaluateTokens(const vector<pair<const bool,const char>> &etData)
    { return etData.empty() ? strBlank :
      accumulate(etData.cbegin(), etData.cend(), strBlank,
        [](const string &strOut, const auto &bcpPair)
          { return bcpPair.first ? Append(strOut,
            bcpPair.second) : strOut; }); }
/* -- Convert time to short duration --------------------------------------- */
static const string ToShortDuration(const double fdDuration,
  const int iPrecision=6)
{ // Output string
  ostringstream osS;
  // Get duration ceiled and if negative?
  double fdInt, fdFrac = modf(fdDuration, &fdInt);
  if(fdInt < 0)
  { // Set negative symbol and negate the duration
    osS << '-';
    fdInt = -fdInt;
    fdFrac = -fdFrac;
  } // Set floating point precision with zero fill
  osS << fixed << setfill('0') << setprecision(0);
  // Have days?
  if(fdInt >= 86400)
    osS <<            floor(fdInt/86400)          << ':'
        << setw(2) << fmod(floor(fdInt/3600), 24) << ':'
        << setw(2) << fmod(floor(fdInt/60),   60) << ':' << setw(2);
  // No days, but hours?
  else if(fdInt >= 3600)
    osS <<            fmod(floor(fdInt/3600), 24) << ':'
        << setw(2) << fmod(floor(fdInt/60),   60) << ':' << setw(2);
  // No hours, but minutes?
  else if(fdInt >= 60)
    osS << fmod(floor(fdInt/60), 60) << ':' << setw(2);
  // No minutes so no zero padding
  else osS << setw(0);
  // On the seconds part, we have a problem where having a precision
  // of zero is causing stringstream to round so we'll just convert it to an
  // int instead to fix it.
  osS << fmod(fdInt, 60);
  if(iPrecision > 0)
    osS << '.' << setw(iPrecision) <<
      static_cast<unsigned int>(fabs(fdFrac) * pow(10.0, iPrecision));
  // Return string
  return osS.str();
}
/* -- Return true of false ------------------------------------------------- */
static const char *TrueOrFalse(const bool bCondition)
  { return bCondition ? "true" : "false"; }
static const char *YesOrNo(const bool bCondition)
  { return bCondition ? "X" : "-"; }
/* ------------------------------------------------------------------------- */
[[maybe_unused]] static size_t FindCharForwards(const string &strS,
  size_t stStart, const size_t stEnd, const char*const cpChars)
{ // Ignore if invalid parameter
  if(!cpChars) return string::npos;
  // Until we've reached the limit
  while(stStart < stEnd && stStart != string::npos)
  { // Return position if we find the character
    for(const char*cpPtr = cpChars; *cpPtr != '\0'; ++cpPtr)
      if(strS[stStart] == *cpPtr) return stStart;
    // Goto next index and try again
    ++stStart;
  } // Failed so return so
  return string::npos;
}
/* -- Count occurence of string -------------------------------------------- */
static size_t CountOccurences(const string &strStr, const string &strWhat)
{ // Zero if string is empty
  if(strStr.empty()) return 0;
  // Matching occurences
  size_t stCount = 0;
  // Find occurences
  for(size_t stIndex = strStr.find(strWhat);
             stIndex != string::npos;
             stIndex = strStr.find(strWhat, stIndex + 1)) ++stCount;
  // Return occurences
  return stCount;
} /* -- Get return character format of text string ------------------------- */
static const string GetTextFormat(const string &strIn)
{ // Enumerate each character if there are characters to check
  if(!strIn.empty())
    for(string::const_iterator ciC{ strIn.cbegin() };
                               ciC != strIn.cend();
                             ++ciC)
  { // Test character
    switch(*ciC)
    { // Carriage-return found
      case '\r':
        return find(ciC, strIn.cend(), '\n') != strIn.cend() ? "\r\n" : "\r";
      // Line-feed found
      case '\n':
        return find(ciC, strIn.cend(), '\r') != strIn.cend() ? "\n\r" : "\n";
      // Anything else is ignored
      default: break;
    }
  } // Nothing found
  return {};
} /* -- Implode a stringdeque to a single string --------------------------- */
template<typename T>
  static const string Implode(const T&sdL, const string &strSep=strSpace)
{ // Done if empty or begin position is invalid
  if(sdL.empty()) return {};
  // Create output only string stream which stays cached (safe in c++11)
  ostringstream osS;
  // Build command string from vector (this and next line don't seem optimal)
  const typename T::const_iterator tLastButOne{ prev(sdL.cend(), 1) };
  copy(sdL.cbegin(), tLastButOne,
    ostream_iterator<string>{ osS, strSep.c_str() });
  osS << *tLastButOne;
  // Done
  return osS.str();
}
/* -- Converts the key/value pairs to a stringvector ----------------------- */
static const string ImplodeMap(const StrNCStrMap &ssmSrc,
  const string &strLineSep=strSpace, const string &strKeyValSep="=",
  const string &strValEncaps="\"")
{ // Done if empty
  if(ssmSrc.empty()) return {};
  // Make string vector to implode and reserve memory for items.
  // Insert each value in the map with the appropriate seperators.
  StrVector svRet; svRet.reserve(ssmSrc.size());
  transform(ssmSrc.cbegin(), ssmSrc.cend(), back_inserter(svRet),
    [&strKeyValSep, &strValEncaps](const auto &vIter)
      { return std::move(Append(vIter.first, strKeyValSep,
          strValEncaps, vIter.second, strValEncaps)); });
  // Return vector imploded into a string
  return Implode(svRet, strLineSep);
} /* -- Implode a stringdeque to a single string --------------------------- */
template<typename T>static const string Implode(const T sdL,
  const size_t &stBegin=0, const string &strSep=strSpace)
{ // Done if empty or begin position is invalid
  if(sdL.empty() || stBegin >= sdL.size()) return {};
  // If we have only one item, just return its string
  if(sdL.size()-stBegin == 1) return sdL[stBegin];
  // Create output only string stream which stays cached (safe in c++11)
  ostringstream osS;
  // Build command string from vector
  copy(next(sdL.cbegin(), static_cast<ssize_t>(stBegin)), prev(sdL.cend(), 1),
    ostream_iterator<string>(osS, strSep.c_str()));
  // Add last item without a separator
  osS << sdL.back();
  // Done
  return osS.str();
}
/* ------------------------------------------------------------------------- */
template<typename T>
  static const string PlusOrMinus(const T tVal, const int iPrecision)
    { return std::move(Append(showpos, fixed, setprecision(iPrecision),
        tVal)); }
/* ------------------------------------------------------------------------- */
template<typename T>
  static const string PlusOrMinusEx(const T tVal, const int iPrecision)
    { return std::move(AppendImbued(showpos, fixed,
        setprecision(iPrecision), tVal)); }
/* ------------------------------------------------------------------------- */
template<typename OutType, typename InType, class SuffixClass>
  static OutType MakeNumberReadable(const InType itValue,
    const char**const cpSuffix, int &iPrecision, const SuffixClass &scLookup,
    const char*const cpDefault=cpBlank)
{ // Check types
  static_assert(is_floating_point_v<OutType>, "OutType not floating point!");
  static_assert(is_integral_v<InType>, "InType not integral!");
  // Value to return
  OutType otReturn;
  // Discover the best measurement to show by testing each unit from the
  // lookup table to see if it is divisible and if it is not?
  if(!any_of(scLookup.cbegin(), scLookup.cend(),
    [&otReturn, itValue, &cpSuffix](const auto &aItem)
  { // Calculate best measurement to show
    if(itValue < aItem.vValue) return false;
    // Set suffix that was sent. Just helps us just one line ToBits/Bytes
    *cpSuffix = aItem.cpSuf;
    otReturn = static_cast<OutType>(itValue) / aItem.vValue;
    // Success
    return true;
  }))
  { // Not found any matches so precision will now be zero
    iPrecision = 0;
    // Suffix is default suffix
    *cpSuffix = cpDefault;
    // Convert the input value to the output value
    otReturn = static_cast<OutType>(itValue);
  } // Return result
  return otReturn;
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static double ToBytes(const IntType itBytes, const char**const cpSuffix,
    int &iPrecision)
{ // A test to perform
  struct ByteValue { const IntType vValue; const char*const cpSuf; };
  // If input value is 64-bit?
  if constexpr(sizeof(IntType) == 8)
  { // Tests lookup table. This is all we can fit in a 64-bit integer
    static const array<const ByteValue,6> bvLookup{ {
      { 0x1000000000000000, "EB" }, { 0x0004000000000000, "PB" },
      { 0x0000010000000000, "TB" }, { 0x0000000040000000, "GB" },
      { 0x0000000000100000, "MB" }, { 0x0000000000000400, "KB" }
    } };
    // Return result
    return MakeNumberReadable<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // If input value is 32-bit?
  if constexpr(sizeof(IntType) == 4)
  { // Tests lookup table. This is all we can fit in a 32-bit integer
    static const array<const ByteValue,3> bvLookup{ {
      { 0x40000000, "GB" }, { 0x00100000, "MB" }, { 0x00000400, "KB" }
    } };
    // Return result
    return MakeNumberReadable<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // If input value is 16-bit?
  if constexpr(sizeof(IntType) == 2)
  { // Tests lookup table. This is all we can fit in a 16-bit integer
    static const array<const ByteValue,1> bvLookup{ { { 0x0400, "KB" } } };
    // Return result
    return MakeNumberReadable<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // Input value is not 64, 32 nor 16 bit? Use a empty table
  static const array<const ByteValue,0> bvLookup{ { } };
  // Show error
  return MakeNumberReadable<double>(itBytes,
    cpSuffix, iPrecision, bvLookup, "B");
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string ToBytesStr(const IntType itBytes, int iPrecision=2)
{ // Process a human readable value for the specified number of bytes
  const char *cpSuffix = nullptr;
  const double dVal = ToBytes<IntType>(itBytes, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return Append(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string ToImbuedBytesStr(const IntType itBytes,
  int iPrecision=2)
{ // Process a human readable value for the specified number of bytes
  const char *cpSuffix = nullptr;
  const double dVal = ToBytes<IntType>(itBytes, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return AppendImbued(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static double ToBits(const IntType itBits, const char**const cpSuffix,
  int &iPrecision)
{ // A test to perform
  struct BitValue { const IntType vValue; const char*const cpSuf; };
  // If input value is 64-bit?
  if constexpr(sizeof(IntType) == 8)
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const array<const BitValue,6> bvLookup{ {
      { 1000000000000000000, "Eb" }, { 1000000000000000, "Pb" },
      {       1000000000000, "Tb" }, {       1000000000, "Gb" },
      {             1000000, "Mb" }, {             1000, "Kb" },
    } };
    // Return result
    return MakeNumberReadable<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  } // If input value is 32-bit?
  if constexpr(sizeof(IntType) == 4)
  { // Tests lookup table. This is all we can fit in a 32-bit integer.
    static const array<const BitValue,3> bvLookup{ {
      { 1000000000, "Gb" }, { 1000000, "Mb" }, { 1000, "Kb" },
    } };
    // Return result
    return MakeNumberReadable<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  } // If input value is 16-bit?
  if constexpr(sizeof(IntType) == 2)
  { // Tests lookup table. This is all we can fit in a 16-bit integer.
    static const array<const BitValue,6> bvLookup{ { { 1000, "Kb" } } };
    // Return result
    return MakeNumberReadable<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  } // Input value is not 64, 32 nor 16 bit? Use a empty table
  static const array<const BitValue,0> bvLookup{ { } };
  // Show error
  return MakeNumberReadable<double>(itBits,
    cpSuffix, iPrecision, bvLookup, "b");
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string ToBitsStr(const IntType itBits, int iPrecision=2)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal = ToBits<IntType>(itBits, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return Append(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string ToImbuedBitsStr(const IntType itBits, int iPrecision)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal = ToBits<IntType>(itBits, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return AppendImbued(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static double ToGrouped(const IntType itValue, const char**const cpSuffix,
    int &iPrecision)
{ // A test to perform
  struct Value { const IntType vValue; const char*const cpSuf; };
  // If input value is 64-bit?
  if constexpr(sizeof(IntType) == 8)
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const array<const Value,4> vLookup{ {
      { 1000000000000, "T" }, { 1000000000, "B" },
      { 1000000,       "M" }, { 1000,       "K" }
    } };
    // Return result
    return MakeNumberReadable<double>(itValue, cpSuffix, iPrecision, vLookup);
  } // If input value is 32-bit?
  if constexpr(sizeof(IntType) == 4)
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const array<const Value,3> vLookup{ {
      { 1000000000, "B" }, { 1000000, "M" }, { 1000, "K" }
    } };
    // Return result
    return MakeNumberReadable<double>(itValue, cpSuffix, iPrecision, vLookup);
  } // If input value is 16-bit?
  if constexpr(sizeof(IntType) == 2)
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const array<const Value,1> vLookup{ { { 1000, "K" } } };
    // Return result
    return MakeNumberReadable<double>(itValue, cpSuffix, iPrecision, vLookup);
  } // Input value is not 64, 32 nor 16 bit? Use a empty table
  static const array<const Value,0> vLookup{ { } };
  // Show error
  return MakeNumberReadable<double>(itValue, cpSuffix, iPrecision, vLookup);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string ToImbuedGroupedStr(const IntType itValue, int iPrecision)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal = ToGrouped<IntType>(itValue, &cpSuffix, iPrecision);
  // Move the FORMATTED stringstreams output string into the return value.
  return AppendImbued(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
static size_t FindCharForwards(const string &strS, size_t stStart,
  const size_t stEnd, const char cpChar)
{ // Until we've reached the limit
  while(stStart < stEnd && stStart != string::npos)
  { // Return position if we find the character
    if(strS[stStart] == cpChar) return stStart;
    // Goto next index and try again
    ++stStart;
  } // Failed so return so
  return string::npos;
}
/* ------------------------------------------------------------------------- */
[[maybe_unused]] static size_t FindCharBackwards(const string &strS,
  size_t stStart, const size_t stEnd, const char cpChar)
{ // Until we've reached the limit
  while(stStart >= stEnd && stStart != string::npos)
  { // Return position if we find the character
    if(strS[stStart] == cpChar) return stStart;
    // Goto next index and try again
    --stStart;
  } // Failed so return so
  return string::npos;
}
/* ------------------------------------------------------------------------- */
[[maybe_unused]] static size_t FindCharNotForwards(const string &strS,
  size_t stStart, const size_t stEnd, const char cpChar)
{ // Until we've reached the limit
  while(stStart < stEnd && stStart != string::npos)
  { // Return position if we find the character
    if(strS[stStart] != cpChar) return stStart;
    // Goto next index and try again
    ++stStart;
  } // Failed so return so
  return string::npos;
}
/* ------------------------------------------------------------------------- */
static size_t FindCharNotForwards(const string &strS, size_t stStart,
  const size_t stEnd)
{ // Until we've reached the limit
  while(stStart < stEnd && stStart != string::npos)
  { // Return position if we find a non-control character
    if(strS[stStart] > ' ') return stStart;
    // We could not match any character
    ++stStart;
  } // Failed so return so
  return string::npos;
}
/* ------------------------------------------------------------------------- */
[[maybe_unused]] static size_t FindCharNotBackwards(const string &strS,
  size_t stStart, const size_t stEnd, const char cpChar)
{ // Until we've reached the limit
  while(stStart >= stEnd && stStart != string::npos)
  { // Return position if we find the character
    if(strS[stStart] != cpChar) return stStart;
    // Goto next index and try again
    --stStart;
  } // Failed so return so
  return string::npos;
}
/* ------------------------------------------------------------------------- */
static size_t FindCharNotBackwards(const string &strS, size_t stStart,
  const size_t stEnd)
{ // Until we've reached the limit
  while(stStart >= stEnd && stStart != string::npos)
  { // Return position if we find a non-control character
    if(strS[stStart] > ' ') return stStart;
    // We could not match any character
    --stStart;
  } // Failed so return so
  return string::npos;
}
/* -- Do convert the specified structure to string ------------------------= */
static const string FormatTimeTM(const StdTMStruct &tmData,
  const char*const cpF) { return Append(put_time(&tmData, cpF)); }
/* -- Convert specified timestamp to string -------------------------------- */
static const string FormatTimeTT(const StdTimeT ttTimestamp,
  const char*const cpFormat = cpTimeFormat)
{ // Convert it to local time in a structure
  StdTMStruct tmData; StdLocalTime(&tmData, &ttTimestamp);
  // Do the parse and return the string
  return FormatTimeTM(tmData, cpFormat);
}
/* -- Remove suffixing carriage return and line feed ----------------------- */
static string &Chop(string &strStr)
{ // Error message should have a carriage return/line feed so remove it
  while(!strStr.empty() && (strStr.back() == '\r' || strStr.back() == '\n'))
    strStr.pop_back();
  // Return string
  return strStr;
}
/* -- Convert specified timestamp to string (UTC) -------------------------- */
static const string FormatTimeTTUTC(const StdTimeT ttTimestamp,
  const char*const cpFormat = cpTimeFormat)
{ // Convert it to local time
  StdTMStruct tmData; StdGMTime(&tmData, &ttTimestamp);
  // Do the parse and return the string
  return FormatTimeTM(tmData, cpFormat);
}
/* ------------------------------------------------------------------------- */
template<typename FloatType>
  static const string ToRatio(const FloatType ftLeft, const FloatType ftRight)
{ // Convert to double if neccesary
  const double fdLeft = static_cast<double>(ftLeft),
               fdRight = static_cast<double>(ftRight);
  // Return if invalid number or the below loop can infinitely enumerate
  if(fdLeft <= 0.0 || fdRight <= 0.0) return "N/A";
  // Divisor to use
  double fdDivisor;
  // Loop until common denominator found
  for(double fdNumerator = fdLeft, fdDenominator = fdRight; ; )
  { // Find the lowest numerator and break if we find it
    fdNumerator = fmod(fdNumerator, fdDenominator);
    if(fdNumerator == 0.0) { fdDivisor = fdDenominator; break; }
    // Find the lowest denominator and break if we find it
    fdDenominator = fmod(fdDenominator, fdNumerator);
    if(fdDenominator == 0.0) { fdDivisor = fdNumerator; break; }
  } // Return lowest numerator and denominator
  return Append(fixed, setprecision(0), ceil(fdLeft / fdDivisor), ':',
    ceil(fdRight / fdDivisor));
}
/* -- Convert string to lower case ----------------------------------------- */
[[maybe_unused]] static const string ToLower(const string &strSrc)
{ // Create memory for destination string and copy the string over
  string strDst; strDst.resize(strSrc.length());
  for(size_t stI = 0; stI < strSrc.size(); ++stI)
    strDst[stI] = static_cast<char>(tolower(static_cast<int>(strSrc[stI])));
  // Return copied string
  return strDst;
}
/* -- Convert string to upper case ----------------------------------------- */
[[maybe_unused]] static const string ToUpper(const string &strSrc)
{ // Create memory for destination string and copy the string over
  string strDst; strDst.resize(strSrc.length());
  for(size_t stI = 0; stI < strSrc.size(); ++stI)
    strDst[stI] = static_cast<char>(toupper(static_cast<int>(strSrc[stI])));
  // Return copied string
  return strDst;
}
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
