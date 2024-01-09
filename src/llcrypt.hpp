/* == LLCRYPT.HPP ========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Crypt' namespace and methods for the guest to use in   ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Crypt
/* ------------------------------------------------------------------------- */
// ! The crypt class allows access to cryptographic, compression and
// ! decompression functions.
/* ========================================================================= */
namespace LLCrypt {                    // Crypt namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace ICrypt::P;
/* ========================================================================= **
** ######################################################################### **
** ## Crypt.* namespace functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ Crypt.CRC
// > Data:string=The string to hash.
// < Hash:integer=The calculated CRC hash.
// ? Calculates a CRC32 hash from a string. Insecure, only use for super fast
// ? hashing of insignificant strings. The benefit of CRC hashing though is
// ? that the function returns an integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CRC, 1, LCPUSHVAR(CryptToCRC32(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.CRCA
// > Data:array=The data to hash.
// < Hash:integer=The calculated CRC hash.
// ? Calculates a CRC32 hash from an array class. Insecure, only use for super
// ? fast hashing of insignificant strings. The benefit of CRC hashing though
// ? is that the function returns an integer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CRCA, 1, LCPUSHVAR(CryptToCRC32(*LCGETPTR(1, Asset))));
/* ========================================================================= */
// $ Crypt.SHA1
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA1 hash.
// ? Calculates a SHA1 hash from a string. This hashing algorithm is obsolete
// ? and insecure and should no longer be used and is here only for
// ? compatibility reasons only.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA1, 1,
  LCPUSHVAR(SHA1functions::HashStr(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.SHA1A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA1 hash.
// ? Calculates a SHA1 hash from an array class. Insecure, only use for
// ? hashing of insignificant data.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA1A, 1, LCPUSHVAR(SHA1functions::HashMB(*LCGETPTR(1, Asset))));
/* ========================================================================= */
// $ Crypt.SHA224
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA224 hash from a string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA224, 1,
  LCPUSHVAR(SHA224functions::HashStr(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.SHA224A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA224 hash from an array class.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA224A, 1,
  LCPUSHVAR(SHA224functions::HashMB(*LCGETPTR(1, Asset))));
/* ========================================================================= */
// $ Crypt.SHA256
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA256 hash from a string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA256, 1,
  LCPUSHVAR(SHA256functions::HashStr(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.SHA256A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA256 hash from an array class.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA256A, 1,
  LCPUSHVAR(SHA256functions::HashMB(*LCGETPTR(1, Asset))));
/* ========================================================================= */
// $ Crypt.SHA384
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA384 hash.
// ? Calculates a SHA384 hash from a string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA384, 1,
  LCPUSHVAR(SHA384functions::HashStr(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.SHA384A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA384 hash.
// ? Calculates a SHA384 hash from an array class.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA384A, 1, LCPUSHVAR(SHA384functions::HashMB(*LCGETPTR(1, Asset))));
/* ========================================================================= */
// $ Crypt.SHA512
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA512 hash.
// ? Calculates a SHA512 hash from a string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA512, 1,
  LCPUSHVAR(SHA512functions::HashStr(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.SHA512A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA512 hash.
// ? Calculates a SHA384 hash from an array class.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SHA512A, 1, LCPUSHVAR(SHA512functions::HashMB(*LCGETPTR(1, Asset))));
/* ========================================================================= */
// $ Crypt.HMSHA1
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA1 hash.
// ? Calculates a SHA1 hash from a string using the specifiy entropy. Insecure,
// ? only use for hashing of insignificant data.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HMSHA1, 1, LCPUSHVAR(SHA1functions::HashStr(
  LCGETCPPSTRING(1, "Key"), LCGETCPPSTRING(2, "Data"))));
/* ========================================================================= */
// $ Crypt.HMSHA224
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA224 hash.
// ? Calculates a SHA224 hash from a string using the specifiy entropy.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HMSHA224, 1, LCPUSHVAR(SHA224functions::HashStr(
  LCGETCPPSTRING(1, "Key"), LCGETCPPSTRING(2, "Data"))));
/* ========================================================================= */
// $ Crypt.HMSHA256
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA256 hash from a string using the specifiy entropy.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HMSHA256, 1, LCPUSHVAR(SHA256functions::HashStr(
  LCGETCPPSTRING(1, "Key"), LCGETCPPSTRING(2, "Data"))));
/* ========================================================================= */
// $ Crypt.HMSHA384
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA384 hash.
// ? Calculates a SHA384 hash from a string using the specifiy entropy.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HMSHA384, 1, LCPUSHVAR(SHA384functions::HashStr(
  LCGETCPPSTRING(1, "Key"), LCGETCPPSTRING(2, "Data"))));
/* ========================================================================= */
// $ Crypt.HMSHA512
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA512 hash.
// ? Calculates a SHA512 hash from a string using the specifiy entropy.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HMSHA512, 1, LCPUSHVAR(SHA512functions::HashStr(
  LCGETCPPSTRING(1, "Key"), LCGETCPPSTRING(2, "Data"))));
/* ========================================================================= */
// $ Crypt.B64E
// > Data:string=The string to encode.
// < Hash:string=The return string encoded to base64.
// ? Encodes the specified string to base64.
/* ------------------------------------------------------------------------- */
LLFUNCEX(B64E, 1, LCPUSHVAR(CryptStoB64(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.B64EA
// > Data:array=The data to encode.
// < Hash:string=The return string encoded to bas64.
// ? Encodes the specified data array to base64.
/* ------------------------------------------------------------------------- */
LLFUNCEX(B64EA, 1, LCPUSHVAR(CryptMBtoB64(*(LCGETPTR(1, Asset)))));
/* ========================================================================= */
// $ Crypt.B64D
// > Text:string=The string to decode.
// < Text:string=The base64 decoded string.
// ? Decodes the specified string from base64.
/* ------------------------------------------------------------------------- */
LLFUNCEX(B64D, 1, LCPUSHVAR(CryptB64toS(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.B64DA
// > Text:string=The string to decode.
// < Data:Asset=The base64 decoded data array.
// ? Decodes the specified string from base64 to a data array class. Useful
// ? if you're handling binary data and/or not sure if the data is binary/text
// ? and need to check.
/* ------------------------------------------------------------------------- */
LLFUNCEX(B64DA, 1, LCCLASSCREATE(Asset)->SwapMemory(
  CryptB64toMB(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.HexEncode
// > Text:string=The string.
// < Text:string=The hex encoded string.
// ? Encodes the specified text to uppercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HexEncode, 1, LCPUSHVAR(CryptStoHEX(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.HexEncodeA
// > Data:Asset=The data array class to encode.
// < Text:string=The hex encoded string.
// ? Encodes the specified data array to uppercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HexEncodeA, 1, LCPUSHVAR(CryptBin2Hex(*(LCGETPTR(1, Asset)))));
/* ========================================================================= */
// $ Crypt.HexEncodeL
// > Text:string=The string.
// < Text:string=The hex encoded string.
// ? Encodes the specified text to lowercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HexEncodeL, 1, LCPUSHVAR(CryptStoHEXL(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.HexEncodeLA
// > Data:Asset=The data array class to encode.
// < Text:string=The hex encoded string.
// ? Encodes the specified data array to lowercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HexEncodeLA, 1, LCPUSHVAR(CryptBin2HexL(*(LCGETPTR(1, Asset)))));
/* ========================================================================= */
// $ Crypt.HexDecode
// > Text:string=The hex encoded string to decode.
// < Text:Asset=The text decoded string.
// ? Encodes the specified text to uppercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HexDecode, 1,
  LCPUSHVAR(CryptHexDecodeStr(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.HexDecodeA
// > Text:string=The hex encoded string to decode.
// < Data:Asset=The decoded data array class.
// ? Decodes the specified hexadecimal string to binary data.
/* ------------------------------------------------------------------------- */
LLFUNCEX(HexDecodeA, 1, LCCLASSCREATE(Asset)->
  SwapMemory(CryptHexDecodeA(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.UrlEncode
// > Text:string=The URL string to encode.
// < Text:string=The URL encoded string.
// ? URL encodes the specified string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(UrlEncode, 1,
  LCPUSHVAR(CryptURLEncode(LCGETSTRING(char, 1, "String"))));
/* ========================================================================= */
// $ Crypt.UrlDecode
// > Text:string=The URL string to decode.
// < Text:string=The URL decoded string.
// ? URL decodes the specified string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(UrlDecode, 1,
  LCPUSHVAR(CryptURLDecode(LCGETSTRING(char, 1, "String"))));
/* ========================================================================= */
// $ Crypt.EntDecode
// > Text:string=The URL string to remove HTML entities from.
// < Text:string=The URL decoded string.
// ? Decodes HTML entities from the spcified string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(EntDecode, 1,
  LCPUSHVAR(cCrypt->EntDecode(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= */
// $ Crypt.EntEncode
// > Text:string=The URL string to insert HTML entities into.
// < Text:string=The URL encoded string.
// ? Encodes HTML entities into the spcified string.
/* ------------------------------------------------------------------------- */
LLFUNCEX(EntEncode, 1,
  LCPUSHVAR(CryptEntEncode(LCGETCPPSTRING(1, "String"))));
/* ========================================================================= **
** ######################################################################### **
** ## Crypt.* namespace functions structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Crypt.* namespace functions begin
  LLRSFUNC(B64D),       LLRSFUNC(B64DA),       LLRSFUNC(B64E),
  LLRSFUNC(B64EA),      LLRSFUNC(CRC),         LLRSFUNC(CRCA),
  LLRSFUNC(EntDecode),  LLRSFUNC(EntEncode),   LLRSFUNC(HexDecode),
  LLRSFUNC(HexDecodeA), LLRSFUNC(HexEncode),   LLRSFUNC(HexEncodeA),
  LLRSFUNC(HexEncodeL), LLRSFUNC(HexEncodeLA), LLRSFUNC(HMSHA1),
  LLRSFUNC(HMSHA224),   LLRSFUNC(HMSHA256),    LLRSFUNC(HMSHA384),
  LLRSFUNC(HMSHA512),   LLRSFUNC(SHA1),        LLRSFUNC(SHA1A),
  LLRSFUNC(SHA224),     LLRSFUNC(SHA224A),     LLRSFUNC(SHA256),
  LLRSFUNC(SHA256A),    LLRSFUNC(SHA384),      LLRSFUNC(SHA384A),
  LLRSFUNC(SHA512),     LLRSFUNC(SHA512A),     LLRSFUNC(UrlDecode),
  LLRSFUNC(UrlEncode),
LLRSEND                                // Crypt.* namespace functions end
/* ========================================================================= */
}                                      // End of Crypt namespace
/* == EoF =========================================================== EoF == */
