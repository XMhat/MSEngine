/* == CRYPT.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This file handles all the cryptographic and co/decompression        ## */
/* ## routines and sets up the environment for OpenSSL library.           ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfCrypt {                    // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace Library::OpenSSL;      // Using OpenSSL library functions
using namespace Library::SevenZip;     // Using 7Zip library functions
using namespace IfMemory;              // Using memory namespace
using namespace IfLog;                 // Using log namespace
using namespace IfUtf;                 // Using utf namespace
using namespace IfVars;                // Using vars namespace
using namespace IfSystem;              // Using system namespace
/* -- Convert the specified character to hexadecimal ----------------------- */
static char CryptHex2Char(const uint8_t ucChar)
{ // Hex lookup table
  static const array<const char,256> caLookup{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //   0 - 15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  16 - 31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  32 - 47
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, //  48 - 63   (48 = '0')
    0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  64 - 79   (65 = 'A')
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  80 - 95
    0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  96 - 111  (97 = 'a')
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 112 - 127
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 128 - 143
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 144 - 159
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160 - 175
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 176 - 191
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 192 - 207
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 208 - 223
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 224 - 239
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 240 - 255
  }; // Return character
  return caLookup[ucChar];
}
/* -- Convert the specified hexadecimanl string to 8-bit array ----------- */
static void CryptHexDecodePtr(const char*const cpSrc, const size_t stSrcLen,
  char*const cpDst)
{ // Build 8-bit value from two ASCII characters
  for(size_t stInPos = 0, stOutPos = 0;
             stInPos < stSrcLen;
             stInPos += 2, ++stOutPos)
    cpDst[stOutPos] = static_cast<char>
      ((CryptHex2Char(static_cast<uint8_t>(cpSrc[stInPos])) << 4) +
        CryptHex2Char(static_cast<uint8_t>(cpSrc[stInPos + 1])));
}
/* -- Convert the specified hexadecimanl string to 8-bit array ----------- */
static Memory CryptHexDecodeA(const string &strSrc)
{ // Must not be empty and a multiple of two to comply
  if(strSrc.empty() || strSrc.size() % 2) return {};
  // The memory to output. We know what size will be
  Memory mbDst{ strSrc.size() / 2 };
  // Build 8-bit value from two ASCII characters
  CryptHexDecodePtr(strSrc.c_str(), strSrc.length(), mbDst.Ptr<char>());
  // Return memory
  return mbDst;
}
/* -- Convert the specified hexadecimanl string to string ------------------ */
static string CryptHexDecodeStr(const string &strSrc)
{ // Must not be empty and a multiple of two to comply
  if(strSrc.empty() || strSrc.size() % 2) return {};
  // The memory to output. We know what size will be
  string strDst; strDst.resize(strSrc.size() / 2);
  // Build 8-bit value from two ASCII characters
  CryptHexDecodePtr(strSrc.c_str(), strSrc.length(),
    ToNonConstCast<char*>(strDst.data()));
  // Return memory
  return strDst;
}
/* -- Convert the specified 8-bit char to a uppercase hex string ----------- */
static const auto &CryptChar2HexU(const uint8_t ucP)
{ // Lookup table for uppercase conversion. Don't know how to do this in C++ :(
  static const array<const array<const char,2>,256>cHexTabUC{
  { {'0','0'},{'0','1'},{'0','2'},{'0','3'},{'0','4'},{'0','5'},{'0','6'},
    {'0','7'},{'0','8'},{'0','9'},{'0','A'},{'0','B'},{'0','C'},{'0','D'},
    {'0','E'},{'0','F'},{'1','0'},{'1','1'},{'1','2'},{'1','3'},{'1','4'},
    {'1','5'},{'1','6'},{'1','7'},{'1','8'},{'1','9'},{'1','A'},{'1','B'},
    {'1','C'},{'1','D'},{'1','E'},{'1','F'},{'2','0'},{'2','1'},{'2','2'},
    {'2','3'},{'2','4'},{'2','5'},{'2','6'},{'2','7'},{'2','8'},{'2','9'},
    {'2','A'},{'2','B'},{'2','C'},{'2','D'},{'2','E'},{'2','F'},{'3','0'},
    {'3','1'},{'3','2'},{'3','3'},{'3','4'},{'3','5'},{'3','6'},{'3','7'},
    {'3','8'},{'3','9'},{'3','A'},{'3','B'},{'3','C'},{'3','D'},{'3','E'},
    {'3','F'},{'4','0'},{'4','1'},{'4','2'},{'4','3'},{'4','4'},{'4','5'},
    {'4','6'},{'4','7'},{'4','8'},{'4','9'},{'4','A'},{'4','B'},{'4','C'},
    {'4','D'},{'4','E'},{'4','F'},{'5','0'},{'5','1'},{'5','2'},{'5','3'},
    {'5','4'},{'5','5'},{'5','6'},{'5','7'},{'5','8'},{'5','9'},{'5','A'},
    {'5','B'},{'5','C'},{'5','D'},{'5','E'},{'5','F'},{'6','0'},{'6','1'},
    {'6','2'},{'6','3'},{'6','4'},{'6','5'},{'6','6'},{'6','7'},{'6','8'},
    {'6','9'},{'6','A'},{'6','B'},{'6','C'},{'6','D'},{'6','E'},{'6','F'},
    {'7','0'},{'7','1'},{'7','2'},{'7','3'},{'7','4'},{'7','5'},{'7','6'},
    {'7','7'},{'7','8'},{'7','9'},{'7','A'},{'7','B'},{'7','C'},{'7','D'},
    {'7','E'},{'7','F'},{'8','0'},{'8','1'},{'8','2'},{'8','3'},{'8','4'},
    {'8','5'},{'8','6'},{'8','7'},{'8','8'},{'8','9'},{'8','A'},{'8','B'},
    {'8','C'},{'8','D'},{'8','E'},{'8','F'},{'9','0'},{'9','1'},{'9','2'},
    {'9','3'},{'9','4'},{'9','5'},{'9','6'},{'9','7'},{'9','8'},{'9','9'},
    {'9','A'},{'9','B'},{'9','C'},{'9','D'},{'9','E'},{'9','F'},{'A','0'},
    {'A','1'},{'A','2'},{'A','3'},{'A','4'},{'A','5'},{'A','6'},{'A','7'},
    {'A','8'},{'A','9'},{'A','A'},{'A','B'},{'A','C'},{'A','D'},{'A','E'},
    {'A','F'},{'B','0'},{'B','1'},{'B','2'},{'B','3'},{'B','4'},{'B','5'},
    {'B','6'},{'B','7'},{'B','8'},{'B','9'},{'B','A'},{'B','B'},{'B','C'},
    {'B','D'},{'B','E'},{'B','F'},{'C','0'},{'C','1'},{'C','2'},{'C','3'},
    {'C','4'},{'C','5'},{'C','6'},{'C','7'},{'C','8'},{'C','9'},{'C','A'},
    {'C','B'},{'C','C'},{'C','D'},{'C','E'},{'C','F'},{'D','0'},{'D','1'},
    {'D','2'},{'D','3'},{'D','4'},{'D','5'},{'D','6'},{'D','7'},{'D','8'},
    {'D','9'},{'D','A'},{'D','B'},{'D','C'},{'D','D'},{'D','E'},{'D','F'},
    {'E','0'},{'E','1'},{'E','2'},{'E','3'},{'E','4'},{'E','5'},{'E','6'},
    {'E','7'},{'E','8'},{'E','9'},{'E','A'},{'E','B'},{'E','C'},{'E','D'},
    {'E','E'},{'E','F'},{'F','0'},{'F','1'},{'F','2'},{'F','3'},{'F','4'},
    {'F','5'},{'F','6'},{'F','7'},{'F','8'},{'F','9'},{'F','A'},{'F','B'},
    {'F','C'},{'F','D'},{'F','E'},{'F','F'} }
  };
  // Return the string. Remember there is no null terminator
  return cHexTabUC[ucP];
}
/* -- Convert the specified 8-bit array to a hexadecmial string (upcase) -- */
static const string CryptBin2Hex(const uint8_t*const ucStr,
  const size_t stSize)
{ // The output string and we know what the size of the output will be so
  // we do not need to use an ostringstream object.
  string strOut; strOut.reserve(stSize * 2);
  // For each character in the string convert it to hex and push it onto
  // the string. The CHAR2HEX function returns only 2 bytes, so we must make
  // sure STL only processes 2 bytes!
  for(const uint8_t *ucPtr = const_cast<uint8_t*>(ucStr),
              *const ucPtrEnd = ucPtr + stSize;
                     ucPtr < ucPtrEnd;
                   ++ucPtr)
    strOut.append(CryptChar2HexU(*ucPtr).data(), 2);
  // We're done. return the string!
  return strOut;
}
static const string CryptBin2Hex(const DataConst &dcIn)
  { return CryptBin2Hex(dcIn.Ptr<uint8_t>(), dcIn.Size()); }
/* -- Convert the specified 8-bit char to a lowercase hex string ----------- */
static const auto &CryptChar2HexL(const uint8_t ucP)
{ // Lookup table for lowercase conversion
  static const array<const array<const char,2>,256>cHexTabLC{
  { {'0','0'},{'0','1'},{'0','2'},{'0','3'},{'0','4'},{'0','5'},{'0','6'},
    {'0','7'},{'0','8'},{'0','9'},{'0','a'},{'0','b'},{'0','c'},{'0','d'},
    {'0','e'},{'0','f'},{'1','0'},{'1','1'},{'1','2'},{'1','3'},{'1','4'},
    {'1','5'},{'1','6'},{'1','7'},{'1','8'},{'1','9'},{'1','a'},{'1','b'},
    {'1','c'},{'1','d'},{'1','e'},{'1','f'},{'2','0'},{'2','1'},{'2','2'},
    {'2','3'},{'2','4'},{'2','5'},{'2','6'},{'2','7'},{'2','8'},{'2','9'},
    {'2','a'},{'2','b'},{'2','c'},{'2','d'},{'2','e'},{'2','f'},{'3','0'},
    {'3','1'},{'3','2'},{'3','3'},{'3','4'},{'3','5'},{'3','6'},{'3','7'},
    {'3','8'},{'3','9'},{'3','a'},{'3','b'},{'3','c'},{'3','d'},{'3','e'},
    {'3','f'},{'4','0'},{'4','1'},{'4','2'},{'4','3'},{'4','4'},{'4','5'},
    {'4','6'},{'4','7'},{'4','8'},{'4','9'},{'4','a'},{'4','b'},{'4','c'},
    {'4','d'},{'4','e'},{'4','f'},{'5','0'},{'5','1'},{'5','2'},{'5','3'},
    {'5','4'},{'5','5'},{'5','6'},{'5','7'},{'5','8'},{'5','9'},{'5','a'},
    {'5','b'},{'5','c'},{'5','d'},{'5','e'},{'5','f'},{'6','0'},{'6','1'},
    {'6','2'},{'6','3'},{'6','4'},{'6','5'},{'6','6'},{'6','7'},{'6','8'},
    {'6','9'},{'6','a'},{'6','b'},{'6','c'},{'6','d'},{'6','e'},{'6','f'},
    {'7','0'},{'7','1'},{'7','2'},{'7','3'},{'7','4'},{'7','5'},{'7','6'},
    {'7','7'},{'7','8'},{'7','9'},{'7','a'},{'7','b'},{'7','c'},{'7','d'},
    {'7','e'},{'7','f'},{'8','0'},{'8','1'},{'8','2'},{'8','3'},{'8','4'},
    {'8','5'},{'8','6'},{'8','7'},{'8','8'},{'8','9'},{'8','a'},{'8','b'},
    {'8','c'},{'8','d'},{'8','e'},{'8','f'},{'9','0'},{'9','1'},{'9','2'},
    {'9','3'},{'9','4'},{'9','5'},{'9','6'},{'9','7'},{'9','8'},{'9','9'},
    {'9','a'},{'9','b'},{'9','c'},{'9','d'},{'9','e'},{'9','f'},{'a','0'},
    {'a','1'},{'a','2'},{'a','3'},{'a','4'},{'a','5'},{'a','6'},{'a','7'},
    {'a','8'},{'a','9'},{'a','a'},{'a','b'},{'a','c'},{'a','d'},{'a','e'},
    {'a','f'},{'b','0'},{'b','1'},{'b','2'},{'b','3'},{'b','4'},{'b','5'},
    {'b','6'},{'b','7'},{'b','8'},{'b','9'},{'b','a'},{'b','b'},{'b','c'},
    {'b','d'},{'b','e'},{'b','f'},{'c','0'},{'c','1'},{'c','2'},{'c','3'},
    {'c','4'},{'c','5'},{'c','6'},{'c','7'},{'c','8'},{'c','9'},{'c','a'},
    {'c','b'},{'c','c'},{'c','d'},{'c','e'},{'c','f'},{'d','0'},{'d','1'},
    {'d','2'},{'d','3'},{'d','4'},{'d','5'},{'d','6'},{'d','7'},{'d','8'},
    {'d','9'},{'d','a'},{'d','b'},{'d','c'},{'d','d'},{'d','e'},{'d','f'},
    {'e','0'},{'e','1'},{'e','2'},{'e','3'},{'e','4'},{'e','5'},{'e','6'},
    {'e','7'},{'e','8'},{'e','9'},{'e','a'},{'e','b'},{'e','c'},{'e','d'},
    {'e','e'},{'e','f'},{'f','0'},{'f','1'},{'f','2'},{'f','3'},{'f','4'},
    {'f','5'},{'f','6'},{'f','7'},{'f','8'},{'f','9'},{'f','a'},{'f','b'},
    {'f','c'},{'f','d'},{'f','e'},{'f','f'} }
  };
  // Return the string. Remember there is no null terminator
  return cHexTabLC[ucP];
}
/* -- Convert the specified 8-bit array to a hexadecmial string (lwcase) --- */
static const string CryptBin2HexL(const uint8_t*const ucStr,
  const size_t stSize)
{ // The output string and we know what the size of the output will be so we
  // don't need to use an ostringstream object.
  string strOut; strOut.reserve(stSize * 2);
  // For each character in the string convert it to hex and push it onto
  // the string. The CHAR2HEX function returns only 2 bytes, so we must make
  // sure STL only processes 2 bytes!
  for(const uint8_t *ucPtr = const_cast<uint8_t*>(ucStr),
                    *const ucPtrEnd = ucPtr + stSize;
                     ucPtr < ucPtrEnd;
                   ++ucPtr)
    strOut.append(CryptChar2HexL(*ucPtr).data(), 2);
  // We're done. return the string!
  return strOut;
}
static const string CryptBin2HexL(const DataConst &dcIn)
  { return CryptBin2HexL(dcIn.Ptr<uint8_t>(), dcIn.Size()); }
/* ------------------------------------------------------------------------- */
static void CryptAddEntropy(void)
{ // Grab some data from the timer subsystem
  const uint64_t qMicro = cLog->CCDeltaUS();
  // Add it as entropy to openssl
  RAND_seed(&qMicro, sizeof(qMicro));
}
/* ------------------------------------------------------------------------- */
static void CryptRandomPtr(void*const vpDst, const size_t stSize)
{ // Add more entropy to the RNG
  CryptAddEntropy();
  // Fill data with random data
  RAND_bytes(reinterpret_cast<unsigned char*>(vpDst),
             static_cast<int>(stSize));
}
/* ------------------------------------------------------------------------- */
template<typename AnyType>static const AnyType CryptRandom(void)
{ // Do the randomisation into the requested type and return it
  AnyType atData;
  CryptRandomPtr(&atData, sizeof(atData));
  return atData;
}
/* -- URL encode the specified c-string ------------------------------------ */
static const string CryptURLEncode(const char*const cpURL)
{ // Bail if null
  if(!cpURL) return {};
  // We will use a ostringstream to build this as we do not know what the
  // size of the output will be.
  ostringstream osS;
  // For each character
  for(const char*cpSrc = const_cast<char*>(cpURL); *cpSrc; ++cpSrc)
  { // Get character
    const uint8_t ucC = static_cast<uint8_t>(*cpSrc);
    // Normal character? Append to string
    if(isalnum(ucC)) osS << static_cast<char>(ucC);
    // Test individual character
    else switch(ucC)
    { // A character we don't need to encode? Just add them 'as-is'.
      case '-': case '.':
      case '_': case '~': osS << static_cast<char>(ucC); break;
      // Any other character? Encode it!
      default: osS << '%' << CryptChar2HexU(ucC)[0]
                          << CryptChar2HexU(ucC)[1]; break;
    }
  } // Done
  return osS.str();
}
/* -- URL encode the specified string -------------------------------------- */
static const string CryptURLEncode(const string &strS)
  { return CryptURLEncode(strS.c_str()); }
/* ------------------------------------------------------------------------- */
template<class T>static const string CryptImplodeMapAndEncode(const T &ssData,
  const string &strSep)
{ // The vector to return
  StrVector svRet;
  // Iterate through each key pair and insert into vector whilst encoding
  transform(ssData.cbegin(), ssData.cend(), back_inserter(svRet),
    [](const auto &vI)
      { return std::move(Append(CryptURLEncode(vI.first), '=',
          CryptURLEncode(vI.second))); });
  // Return vector
  return Implode(svRet, strSep);
}
/* -- Get error reason --------------------------------------------------- */
static string CryptGetErrorReason(const unsigned long ulErr)
{ // Clear and resize error buffer to maximum
  string strError; strError.resize(128);
  // Grab the error string from openssl and resize to correct size. Better to
  // use the non '_n' version so we can do all this in one line. Since
  // OpenSSL won't write more than 120 characters.
  strError.resize(static_cast<size_t>(
    strlen(ERR_error_string(ulErr, const_cast<char*>(strError.data())))));
  // Return error
  return strError;
}
/* -- Get error reason --------------------------------------------------- */
static string CryptGetError(void)
{ // Error to return
  string strError;
  // Process errors... Only show errors we can actually report on
  // See ERR.H for actual error codes.
  while(const unsigned long ulErr = ERR_get_error())
  { // If the operating system has the reason?
    if(ERR_GET_LIB(ulErr) == ERR_LIB_SYS ||
       ERR_GET_REASON(ulErr) == ERR_R_SYS_LIB)
    { // Get system error and if no error set? Store what OpenSSL actually sent
      strError = SysError();
      if(strError.empty()) strError = CryptGetErrorReason(ulErr);
      // Remove the rest of the errors because system errors are best
      else ERR_clear_error();
    } // An error we don't need to specially process
    else
    { // Clear the error and make room for the error message
      strError = CryptGetErrorReason(ulErr);
      // If theres a colon in it, delete everything up to that colon
      const size_t stColon = strError.find_last_of(':');
      if(stColon != string::npos)
        strError = Capitalise(strError.substr(stColon + 1));
      // Free unused memory since the logevity of this value can be a while
      strError.shrink_to_fit();
    }
  } // Return the reason
  return strError;
}
/* ------------------------------------------------------------------------- */
static const string CryptPTRtoB64(const void*const vpIn, const size_t stIn)
{ // To clean up when leaving scope unexpectedliy
  typedef unique_ptr<BIO, function<decltype(BIO_free_all)>> BioPtr;
  // Create base 64 filter and if succeeded?
  if(BioPtr bB64{ BIO_new(BIO_f_base64()), BIO_free_all })
  { // Disable new line
    BIO_set_flags(bB64.get(), BIO_FLAGS_BASE64_NO_NL);
    // Create RAM based buffer filter and if succeeded?
    if(BIO*const bRAMraw = BIO_new(BIO_s_mem()))
    { // Assign RAM buffer to base 64 filter and if succeeded?
      if(BioPtr bRAM{ BIO_push(bB64.get(), bRAMraw), BIO_free_all })
      { // bB64 is now taken over by bRAM so make sure it doesn't destruct
        bB64.release();
        // Do conversion and if succeeded?
        size_t stBytesWritten;
        if(BIO_write_ex(bRAM.get(), vpIn, stIn, &stBytesWritten))
        { // Do flush and if succeeded?
          if(BIO_flush(bRAM.get()))
          { // Get memory pointer (don't use *const, it will crash).
            BUF_MEM *bmPTR = nullptr;
            BIO_get_mem_ptr(bRAM.get(), &bmPTR);
            // Done so return result
            return { bmPTR->data, bmPTR->length };
          } // Failed to flush stream
          XC("Failed to flush base64 encoder stream!",
             "InSize", stIn, "OutSize", stBytesWritten,
             "Reason", CryptGetError());
        } // Failed to decode base64
        XC("Failed to decode base64 stream!",
           "InSize", stIn, "Reason", CryptGetError());
      } // Failed to decode base64
      XC("Failed to assign RAM buffer to decode base64 stream!",
         "InSize", stIn, "Reason", CryptGetError());
    } // Failed to create RAM based buffer
    XC("Failed to create RAM based buffer to decode base64 stream!",
       "InSize", stIn, "Reason", CryptGetError());
  } // Failed to create base64 decoder context
  XC("Failed to create base64 decoder context!",
     "InSize", stIn, "Reason", CryptGetError());
}
/* ------------------------------------------------------------------------- */
static size_t CryptB64toPTR(void*const vpIn, const size_t stIn,
  void*const vpOut, const size_t stOut)
{ // To clean up when leaving scope unexpectedliy
  typedef unique_ptr<BIO, function<decltype(BIO_free_all)>> BioPtr;
  // Create base 64 filter, clean up if succeeded?
  if(BioPtr bB64{ BIO_new(BIO_f_base64()), BIO_free_all })
  { // Create RAM based buffer filter and clean up if succeeded?
    if(BIO*const bRAMraw = BIO_new_mem_buf(vpIn, static_cast<int>(stIn)))
    { // Assign RAM buffer to base 64 filter and if succeeded?
      if(BioPtr bRAM{ BIO_push(bB64.get(), bRAMraw), BIO_free_all })
      { // bB64 is now taken over by bRAM so make sure it is not released
        bB64.release();
        // Disable new line
        BIO_set_flags(bRAM.get(), BIO_FLAGS_BASE64_NO_NL);
        // Do conversion and return bytes read
        size_t stBytesRead;
        if(BIO_read_ex(bRAM.get(), vpOut, stOut, &stBytesRead))
          return stBytesRead;
        // Failure so raise exception
        XC("Failed to decode base64 data!",
           "InSize", stIn, "OutSize", stOut, "Reason", CryptGetError());
      } // Failure to assign RAM buffer
      XC("Failed to assign RAM buffer for base64 decoding!",
         "InSize", stIn, "OutSize", stOut, "Reason", CryptGetError());
    } // Failure to create RAM buffer filter
    XC("Failed to allocate memory for base64 decoding!",
       "InSize", stIn, "OutSize", stOut, "Reason", CryptGetError());
  }  // Failure to create base 64 filter
  XC("Failed to create base64 filter!",
     "InSize", stIn, "OutSize", stOut, "Reason", CryptGetError());
}
/* ------------------------------------------------------------------------- */
static const string CryptMBtoB64(const DataConst &dcIn)
  { return CryptPTRtoB64(dcIn.Ptr<void>(), dcIn.Size()); }
/* ------------------------------------------------------------------------- */
static const string CryptStoB64(const string &strIn)
  { return CryptPTRtoB64(ToNonConstCast<void*>(strIn.data()),
      strIn.length()); }
/* ------------------------------------------------------------------------- */
static Memory CryptB64toMB(const string &strIn)
{ // Output buffer
  Memory aData{ strIn.length() };
  // Do conversion and resize string after
  aData.Resize(CryptB64toPTR(ToNonConstCast<void*>(strIn.data()),
    strIn.length(), aData.Ptr(), aData.Size()));
  // Return data
  return aData;
}
/* ------------------------------------------------------------------------- */
static const string CryptB64toS(const string &strIn)
{ // Create output buffer with enough size for output
  string strOut; strOut.resize(strIn.length());
  // Do conversion and resize string after
  strOut.resize(CryptB64toPTR(ToNonConstCast<void*>(strIn.data()),
    strIn.length(), ToNonConstCast<void*>(strOut.data()), strOut.length()));
  // Return string
  return strOut;
}
/* -- Encode XML/HTML entities into string (crude but effective) ----------- */
static const string CryptEntEncode(const string &strS)
{ // Done if empty
  if(strS.empty()) return {};
  // Create sting to return and reserve memory. We will use a ostringstream
  // because we do not know what the size is going to be and we can make
  // use of hex which will work with our utf8 decoder.
  ostringstream osS; osS << hex;
  // For each entity. Find it in the string
  // Until null character. Which control token?
  for(Decoder utfSrc{ strS }; unsigned int uiChar = utfSrc.Next();)
  { // Characters not these character ranges will be encoded
    if(uiChar <= 31                  || // Control characters
      (uiChar >= 33 && uiChar <= 47) || // Symbols before numbers
      (uiChar >= 58 && uiChar <= 64) || // Symbols before capitals
       uiChar >= 123)                   // Extended characters
      osS << "&#x" << uiChar << ';';
    // Character is usable as is
    else osS << static_cast<char>(uiChar);
  } // Return string
  return osS.str();
}
/* ------------------------------------------------------------------------- */
static const string CryptStoHEX(const string &strIn)
  { return CryptBin2Hex(reinterpret_cast<const uint8_t*>(strIn.data()),
      strIn.length()); }
/* ------------------------------------------------------------------------- */
static const string CryptStoHEXL(const string &strIn)
  { return CryptBin2HexL(reinterpret_cast<const uint8_t*>(strIn.data()),
      strIn.length()); }
/* -- Helper for mutliple hashing types from OpenSSL ----------------------- */
static Memory CryptHMACCall(const EVP_MD*const fFunc,
  const void         *const vpSalt, const size_t stSaltSize,
  const unsigned char*const cpSrc,  const size_t stSrcSize)
{ // Check sizes to make sure they can be converted to integer
  if(IntWillOverflow<int>(stSaltSize))
    XC("Size of salt data too big!",
       "Requested", stSaltSize, "Maximum", numeric_limits<int>::max());
  if(IntWillOverflow<int>(stSrcSize))
    XC("Size of source data to hash too big!",
       "Requested", stSrcSize,  "Maximum", numeric_limits<int>::max());
  // Create output for HMAC-SHA hash
  Memory aData{ EVP_MAX_MD_SIZE };
  // For storage of size. We really need to know the output size.
  unsigned int uiLen = 0;
  // Get hash. Remember that HMAC returns as binary as we need to send the
  // whole buffer to Base64 and NOT a null-terminated string.
  if(!HMAC(fFunc, vpSalt, static_cast<int>(stSaltSize), cpSrc, stSrcSize,
    aData.Ptr<unsigned char>(), &uiLen))
      XC("Failed to perform salted-hash on source data!",
         "Function",    fFunc != nullptr,
         "SaltAddress", vpSalt != nullptr, "SaltSize", stSaltSize,
         "SrcAddress",  cpSrc != nullptr,  "SrcSize", stSrcSize,
         "OutLength",   uiLen);
  // Resize string
  aData.Resize(uiLen);
  // Return memory block
  return aData;
}
/* -- Hashing functions ---------------------------------------------------- */
#define DEFINE_HASH_FUNCS(x, s, f) \
  namespace x ## functions { \
    static Memory HashPtrRaw(const unsigned char*const ucpIn, \
      const size_t stLen) \
        { Memory mbOut{ s }; \
          x(ucpIn, stLen, mbOut.Ptr<unsigned char>()); \
          return mbOut; } \
    static const string HashPtr(const unsigned char*const ucpIn, \
      const size_t stLen) \
        { return CryptBin2Hex(std::move(HashPtrRaw(ucpIn, stLen))); } \
    static const string HashStr(const string &strIn) \
      { return HashPtr(reinterpret_cast<const unsigned char*>(strIn.data()), \
          strIn.length()); } \
    static const string HashMB(const DataConst &dcIn) \
      { return HashPtr(dcIn.Ptr<unsigned char>(), dcIn.Size()); } \
    static Memory HashPtrRaw(const void*const vpSalt, const size_t stSaltSize,\
      const unsigned char*const cpDest, const size_t stDestSize) \
        { return CryptHMACCall(f(), vpSalt, stSaltSize, cpDest, stDestSize); }\
    static Memory HashStrRaw(const string &strKey, const string &strIn) \
      { return HashPtrRaw( \
          reinterpret_cast<const void*>(strKey.data()), \
          strKey.size(), \
          reinterpret_cast<const unsigned char*>(strIn.data()),\
          strIn.size()); } \
    static const string HashStr(const string &strKey, const string &strIn) \
      { return CryptBin2Hex(std::move(HashStrRaw(strKey, strIn))); } \
  };
DEFINE_HASH_FUNCS(MD5,    MD5_DIGEST_LENGTH,    EVP_md5);    // Insecure
DEFINE_HASH_FUNCS(SHA1,   SHA_DIGEST_LENGTH,    EVP_sha1);   // Insecure
DEFINE_HASH_FUNCS(SHA224, SHA224_DIGEST_LENGTH, EVP_sha224); // Secure
DEFINE_HASH_FUNCS(SHA256, SHA256_DIGEST_LENGTH, EVP_sha256); // Secure
DEFINE_HASH_FUNCS(SHA384, SHA384_DIGEST_LENGTH, EVP_sha384); // Very secure
DEFINE_HASH_FUNCS(SHA512, SHA512_DIGEST_LENGTH, EVP_sha512); // Really secure
#undef DEFINE_HASH_FUNCS
/* -- Create CRC32 hash of specified string using LZMA API ----------------- */
static unsigned int CryptToCRC32(const string &strIn)
  { return CrcCalc(strIn.data(), strIn.length()); }
/* -- Create CRC32 hash of specified memory block using LZMA API ----------- */
static unsigned int CryptToCRC32(const DataConst &dcIn)
  { return CrcCalc(dcIn.Ptr<void>(), dcIn.Size()); }
/* -- URL decode the specified c-string ------------------------------------ */
static const string CryptURLDecode(const char*const cpURL)
{ // Bail if null
  if(!cpURL) return {};
  // We will use a ostringstream to build this as we do not know what the
  // size of the output will be.
  ostringstream osS;
  // For each character
  for(char *cpPtr = const_cast<char*>(cpURL); *cpPtr; ++cpPtr)
  { // Get and test character
    switch(unsigned char &ucC = *reinterpret_cast<unsigned char*>(cpPtr))
    { // % symbol denoting a custom value
      case '%':
      { // Convert hex to number and if not valid? Next character
        unsigned char &uc1 = *reinterpret_cast<unsigned char*>(++cpPtr);
        if(!isxdigit(uc1)) continue;
        // Get second hex character and if not valid? Next character
        unsigned char &uc2 = *reinterpret_cast<unsigned char*>(++cpPtr);
        if(!isxdigit(uc2)) continue;
        // Is lowecase 'a'
        if(uc1 >= 'a') uc1 -= 'a'-'A';
        uc1 -= uc1 >= 'A' ? ('A' - 10) : '0';
        if(uc2 >= 'a') uc2 -= 'a'-'A';
        uc2 -= uc2 >= 'A' ? ('A' - 10) : '0';
        // Set new character and fall through to default
        ucC = 16 * uc1 + uc2;
        // Fall through
        [[fallthrough]];
      } // Normal character? (Do not move this as '%' falls through to this)
      default:
      { // Push normal character
        osS << ucC;
        // Done
        break;
      }
    }
  } // Done
  return osS.str();
}
/* ------------------------------------------------------------------------- */
static const Memory CryptRandomBlock(const size_t stSize)
{ // Memory to hold data
  Memory aData{ stSize };
  // Fill data with random data
  CryptRandomPtr(aData.Ptr(), aData.Size());
  // Return array
  return aData;
}
/* -- Crypt manager class -------------------------------------------------- */
static class Crypt final :
  /* -- Base classes ------------------------------------------------------- */
  public IHelper                       // The crypto manager class
{ /* -------------------------------------------------------------- */ private:
  const StrStrMap  csmEntList;         // Html entity decoding lookup table
  /* ----------------------------------------------------------------------- */
  static void *OSSLAlloc(size_t stSize, const char*const, const int)
    { return MemAlloc<void>(stSize); }
  /* ----------------------------------------------------------------------- */
  static void *OSSLReAlloc(void*const vpPtr, size_t stSize,
    const char*const, const int)
      { return MemReAlloc(vpPtr, stSize); }
  /* ----------------------------------------------------------------------- */
  static void OSSLFree(void*const vpPtr, const char*const, const int)
    { MemFree(vpPtr); }
  /* -- Private keys ----------------------------------------------- */ public:
#define PK_KEY_COUNT                 4 // Number of quads in key (256bits)
#define PK_IV_COUNT                  2 // Number of quads in iv key (128bits)
#define PK_TOTAL_COUNT                 (PK_KEY_COUNT + PK_IV_COUNT)
  /* ----------------------------------------------------------------------- */
  typedef array<uint64_t, PK_KEY_COUNT>   QPKey;
  typedef array<uint64_t, PK_IV_COUNT>    QIVKey;
  typedef array<uint64_t, PK_TOTAL_COUNT> QKeys;
  /* ----------------------------------------------------------------------- */
  union PrivateKey                     // Private key data
  { // --------------------------------------------------------------------- */
    struct Parts                       // Parts of the private key
    { // ------------------------------------------------------------------- */
      QPKey        qKey;               // Private key (256bits)
      QIVKey       qIV;                // IV key (128bits)
      // ------------------------------------------------------------------ */
    } p;                               // Access to important parts
    // --------------------------------------------------------------------- */
    QKeys          qKeys;              // Access to all parts of key
    // --------------------------------------------------------------------- */
  } pkDKey, pkKey;                     // Default and loaded private key data
  /* -- Set a new private key ---------------------------------------------- */
  void ResetPrivateKey(void)
    { CryptRandomPtr(&pkKey.qKeys, sizeof(pkKey.qKeys)); }
  /* -- Update private key mainly for use with protected cvars ------------- */
  void WritePrivateKey(const size_t stId, const uint64_t qVal)
    { pkKey.qKeys[stId] = qVal; }
  /* -- Set default private key -------------------------------------------- */
  void SetDefaultPrivateKey(void) { pkKey = pkDKey; }
  /* -- Read part of the private key --------------------------------------- */
  uint64_t ReadPrivateKey(const size_t stId) { return pkKey.qKeys[stId]; }
  /* -- Iterator is the last entitiy? -------------------------------------- */
  bool IsLastEntity(const StrStrRefMapConstIt &ssrmicIt)
    { return ssrmicIt == csmEntList.cend(); }
  /* -- Find entity in the entity list ------------------------------------- */
  const StrStrRefMapConstIt FindEntity(const string &strWhat)
    { return csmEntList.find(strWhat); }
  /* -- Encode XML/HTML entities into string ------------------------------- */
  const string EntDecode(string strS)
  { // Done if empty
    if(strS.empty()) return {};
    // Loop until we don't find anymore entities to decode
    for(size_t stAPos = strS.find('&');
               stAPos != string::npos;
               stAPos = strS.find('&', stAPos))
    { // Get semi-colon position, break if not found
      const size_t stSPos = strS.find(';', stAPos+1);
      if(stSPos == string::npos) break;
      // Copy out the entity
      const string strT{ strS.substr(stAPos+1, stSPos-stAPos-1) };
      // Cut out the entity
      strS.erase(stAPos, stSPos-stAPos+1);
      // Is a unicode number?
      if(strT.front() == '#')
      { // Don't have at least two characters? Ignore, goto next entity
        if(strT.length() <= 1) continue;
        // Value to convert
        unsigned int uiVal;
        // Have more than 2 characters and hex character specified?
        if(strT.length() > 2 && (strT[1] == 'x' || strT[1] == 'X'))
          uiVal = HexToNumber<unsigned int>
            (strT.substr(2, string::npos));
        // Not hex but is a number? Normal number
        else if(isdigit(static_cast<unsigned char>(strT[1])))
          uiVal = ToNumber<unsigned int>
            (strT.substr(1, string::npos));
        // Shouldn't be anything else. Ignore insertations, goto next entity
        else continue;
        // Encoder character
        const EncoderEx utfCode{ EncodeEx(uiVal) };
        // Insert utf-8 string
        strS.insert(stAPos, utfCode.u.c);
        // Go forward the number of unicode bytes written
        stAPos += utfCode.l;
        // Find another entity
        continue;
      } // Find string to decode, ignore further insertation if no match
      const StrStrRefMapConstIt csmItem{ FindEntity(strT) };
      if(IsLastEntity(csmItem)) continue;
      // Insert result
      strS.insert(stAPos, csmItem->second);
      // Go forward
      stAPos += csmItem->second.length();
    } // Return string
    return strS;
  }
  /* -- Constructor -------------------------------------------------------- */
  Crypt(void) :
    /* -- Initialisation of members ---------------------------------------- */
    IHelper{ __FUNCTION__ },
    csmEntList{
      { "Agrave",  "\xC0"           }, { "Aacute",  "\xC1"           },
      { "Acirc",   "\xC2"           }, { "Atilde",  "\xC3"           },
      { "Auml",    "\xC4"           }, { "Aring",   "\xC5"           },
      { "AElig",   "\xC6"           }, { "Ccedil",  "\xC7"           },
      { "Egrave",  "\xC8"           }, { "Eacute",  "\xC9"           },
      { "Ecirc",   "\xCA"           }, { "Euml",    "\xCB"           },
      { "Igrave",  "\xCC"           }, { "Iacute",  "\xCD"           },
      { "Icirc",   "\xCE"           }, { "Iuml",    "\xCF"           },
      { "ETH",     "\xD0"           }, { "Ntilde",  "\xD1"           },
      { "Ograve",  "\xD2"           }, { "Oacute",  "\xD3"           },
      { "Ocirc",   "\xD4"           }, { "Otilde",  "\xD5"           },
      { "Ouml",    "\xD6"           }, { "Oslash",  "\xD8"           },
      { "Ugrave",  "\xD9"           }, { "Uacute",  "\xDA"           },
      { "Ucirc",   "\xDB"           }, { "Uuml",    "\xDC"           },
      { "Yacute",  "\xDD"           }, { "THORN",   "\xDE"           },
      { "aacute",  "\xE1"           }, { "acirc",   "\xE2"           },
      { "acute",   "\xB4"           }, { "aelig",   "\xE6"           },
      { "agrave",  "\xE0"           }, { "alefsym", strBlank         },
      { "alpha",   "a"              }, { "amp",     "&"              },
      { "and",     strBlank         }, { "ang",     strBlank         },
      { "apos",    "'"              }, { "aring",   "\xE5"           },
      { "asymp",   "\x98"           }, { "atilde",  "\xE3"           },
      { "auml",    "\xE4"           }, { "bdquo",   "\x84"           },
      { "beta",    "\xDF"           }, { "brvbar",  "\xA6"           },
      { "bull",    "\x95"           }, { "cap",     "n"              },
      { "ccedil",  "\xE7"           }, { "cedil",   "\xB8"           },
      { "cent",    "\xA2"           }, { "chi",     "X"              },
      { "circ",    "\x88"           }, { "clubs",   strBlank         },
      { "cong",    strBlank         }, { "copy",    "\xA9"           },
      { "crarr",   strBlank         }, { "cup",     strBlank         },
      { "curren",  "\xA4"           }, { "dArr",    strBlank         },
      { "dagger",  "\x86"           }, { "darr",    strBlank         },
      { "deg",     "\xB0"           }, { "delta",   "d"              },
      { "diams",   strBlank         }, { "divide",  "\xF7"           },
      { "eacute",  "\xE9"           }, { "ecirc",   "\xEA"           },
      { "egrave",  "\xE8"           }, { "empty",   "\xD8"           },
      { "emsp",    strSpace         }, { "ensp",    strSpace         },
      { "epsilon", "e"              }, { "equiv",   "="              },
      { "eta",     strBlank         }, { "eth",     "\xF0"           },
      { "euml",    "\xEB"           }, { "euro",    "\x80"           },
      { "exist",   strBlank         }, { "fnof",    "\x83"           },
      { "forall",  strBlank         }, { "frac12",  "\xBD"           },
      { "frac14",  "\xBC"           }, { "frac34",  "\xBE"           },
      { "frasl",   "/"              }, { "gamma",   strBlank         },
      { "ge",      "="              }, { "gt",      ">"              },
      { "hArr",    strBlank         }, { "harr",    strBlank         },
      { "hearts",  strBlank         }, { "hellip",  "\x85"           },
      { "iacute",  "\xED"           }, { "icirc",   "\xEE"           },
      { "iexcl",   "\xA1"           }, { "igrave",  "\xEC"           },
      { "image",   "I"              }, { "infin",   "8"              },
      { "int",     strBlank         }, { "iota",    strBlank         },
      { "iquest",  "\xBF"           }, { "isin",    strBlank         },
      { "iuml",    "\xEF"           }, { "kappa",   strBlank         },
      { "lArr",    strBlank         }, { "lambda",  strBlank         },
      { "lang",    "<"              }, { "laquo",   "\xAB"           },
      { "larr",    strBlank         }, { "lceil",   strBlank         },
      { "ldquo",   "\x93"           }, { "le",      "="              },
      { "lfloor",  strBlank         }, { "lowast",  "*"              },
      { "loz",     strBlank         }, { "lrm",     "\xE2\x80\x8E"   },
      { "lsaquo",  "\x8B"           }, { "lsquo",   "\x91"           },
      { "lt",      "<"              }, { "macr",    "\xAF"           },
      { "mdash",   "\x97"           }, { "micro",   "\xB5"           },
      { "middot",  "\xB7"           }, { "minus",   "-"              },
      { "mu",      "\xB5"           }, { "nabla",   strBlank         },
      { "nbsp",    strSpace         }, { "ndash",   "\x96"           },
      { "ne",      strBlank         }, { "ni",      strBlank         },
      { "not",     "\xAC"           }, { "notin",   strBlank         },
      { "nsub",    strBlank         }, { "ntilde",  "\xF1"           },
      { "nu",      strBlank         }, { "oacute",  "\xF3"           },
      { "ocirc",   "\xF4"           }, { "oelig",   "\x9C"           },
      { "ograve",  "\xF2"           }, { "oline",   strBlank         },
      { "omega",   strBlank         }, { "omicron", strBlank         },
      { "oplus",   strBlank         }, { "or",      strBlank         },
      { "ordf",    "\xAA"           }, { "ordm",    "\xBA"           },
      { "oslash",  "\xF8"           }, { "otilde",  "\xF5"           },
      { "otimes",  strBlank         }, { "ouml",    "\xF6"           },
      { "para",    "\xB6"           }, { "part",    strBlank         },
      { "permil",  "\x89"           }, { "perp",    strBlank         },
      { "phi",     "f"              }, { "pi",      "p"              },
      { "piv",     strBlank         }, { "plusmn",  "\xB1"           },
      { "pound",   "\xA3"           }, { "prime",   "'"              },
      { "prod",    strBlank         }, { "prop",    strBlank         },
      { "psi",     strBlank         }, { "quot",    "\""             },
      { "rArr",    strBlank         }, { "radic",   "v"              },
      { "rang",    ">"              }, { "raquo",   "\xBB"           },
      { "rarr",    strBlank         }, { "rceil",   strBlank         },
      { "rdquo",   "\x94"           }, { "real",    "R"              },
      { "reg",     "\xAE"           }, { "rfloor",  strBlank         },
      { "rho",     strBlank         }, { "rlm",     "\xE2\x80\x8F"   },
      { "rsaquo",  "\x9B"           }, { "rsquo",   "\x92"           },
      { "sbquo",   "\x82"           }, { "scaron",  "\x9A"           },
      { "sdot",    "\xB7"           }, { "sect",    "\xA7"           },
      { "shy",     "\xC2\xAD"       }, { "sigma",   "s"              },
      { "sigmaf",  strBlank         }, { "sim",     "~"              },
      { "spades",  strBlank         }, { "sub",     strBlank         },
      { "sube",    strBlank         }, { "sum",     strBlank         },
      { "sup",     strBlank         }, { "sup1",    "\xB9"           },
      { "sup2",    "\xB2"           }, { "sup3",    "\xB3"           },
      { "supe",    strBlank         }, { "szlig",   "\xDF"           },
      { "tau",     "t"              }, { "there4",  strBlank         },
      { "theta",   strBlank         }, { "thetasym",strBlank         },
      { "thinsp",  strBlank         }, { "thorn",   "\xFE"           },
      { "tilde",   "\x98"           }, { "times",   "\xD7"           },
      { "trade",   "\x99"           }, { "uArr",    strBlank         },
      { "uacute",  "\xFA"           }, { "uarr",    strBlank         },
      { "ucirc",   "\xFB"           }, { "ugrave",  "\xF9"           },
      { "uml",     "\xA8"           }, { "upsih",   strBlank         },
      { "upsilon", "Y"              }, { "uuml",    "\xFC"           },
      { "weierp",  "P"              }, { "xi",      strBlank         },
      { "yacute",  "\xFD"           }, { "yen",     "\xA5"           },
      { "yuml",    "\xFF"           }, { "zeta",    "Z"              },
      { "zwj",     "\xE2\x80\x8D"   }, { "zwnj",    "\xE2\x80\x8C"   },
    },                                 // Default private
    pkDKey{ { { 0x9F7C1E39CA3935CA, 0x71F2A9630EF11A98,
                0xA2AB924A293A01FB, 0x2BA92A197F3AD1A9 },
              { 0x109CF37A284B8910, 0x89FE280958CFD102 } } },
    pkKey(pkDKey)                      // and IV key
    /* -- Code ------------------------------------------------------------- */
    { // Use sql to allocate memory?
      if(!CRYPTO_set_mem_functions(OSSLAlloc, OSSLReAlloc, OSSLFree))
        XC("Failed to setup allocator for crypto interface!");
      // Generate CRC table (for lzma lib)
      CrcGenerateTable();
      // Init openSSL
      OPENSSL_init();
      // Class initialised
      IHInitialise();
      // Loop until...
      do
      { // Get some random entropy from the system hardware
        const Memory aData{ cSystem->GetEntropy() };
        // Set seed from system class to opensl
        RAND_seed(aData.Ptr<void>(), aData.Size<int>());
        // Make a simple request to initialise more entropy
        CryptRandom<int>();
      } // ...PRNG is ready
      while(!RAND_status());
    }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPERBEGIN(~Crypt)
  // De-initialise openssl
  if(IHDeInitialise()) OPENSSL_cleanup();
  // Overwrite loaded private key
  SetDefaultPrivateKey();
  // Done
  DTORHELPEREND(~Crypt)
  /* -- Macros ---------------------------------------------------- */ private:
  DELETECOPYCTORS(Crypt);              // Do not need defaults
  /* ----------------------------------------------------------------------- */
} *cCrypt = nullptr;                   // Pointer to static class
/* -- URL decode the specified string -------------------------------------- */
static const string CryptURLDecode(const string &strS)
  { return CryptURLDecode(strS.c_str()); }
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
