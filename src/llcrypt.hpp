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
using namespace IMemory::P;            using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Crypt.* namespace functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ Crypt.B64D
// > Text:string=The string to decode.
// < Text:string=The base64 decoded string.
// ? Decodes the specified string from base64.
/* ------------------------------------------------------------------------- */
LLFUNC(B64D, 1, LuaUtilPushVar(lS, CryptB64toS(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.B64DA
// > Text:string=The string to decode.
// < Data:Asset=The base64 decoded data array.
// ? Decodes the specified string from base64 to a data array class. Useful
// ? if you're handling binary data and/or not sure if the data is binary/text
// ? and need to check.
/* ------------------------------------------------------------------------- */
LLFUNC(B64DA, 1,
  const AgString aString{lS, 1};
  AcAsset{lS}().MemSwap({ CryptB64toMB(aString) }))
/* ========================================================================= */
// $ Crypt.B64E
// > Data:string=The string to encode.
// < Hash:string=The return string encoded to base64.
// ? Encodes the specified string to base64.
/* ------------------------------------------------------------------------- */
LLFUNC(B64E, 1, LuaUtilPushVar(lS, CryptStoB64(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.B64EA
// > Data:array=The data to encode.
// < Hash:string=The return string encoded to bas64.
// ? Encodes the specified data array to base64.
/* ------------------------------------------------------------------------- */
LLFUNC(B64EA, 1, LuaUtilPushVar(lS, CryptMBtoB64(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Crypt.CRC
// > Data:string=The string to hash.
// < Hash:integer=The calculated CRC hash.
// ? Calculates a CRC32 hash from a string. Insecure, only use for super fast
// ? hashing of insignificant strings. The benefit of CRC hashing though is
// ? that the function returns an integer.
/* ------------------------------------------------------------------------- */
LLFUNC(CRC, 1, LuaUtilPushVar(lS, CryptToCRC32(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.CRCA
// > Data:array=The data to hash.
// < Hash:integer=The calculated CRC hash.
// ? Calculates a CRC32 hash from an array class. Insecure, only use for super
// ? fast hashing of insignificant strings. The benefit of CRC hashing though
// ? is that the function returns an integer.
/* ------------------------------------------------------------------------- */
LLFUNC(CRCA, 1, LuaUtilPushVar(lS, CryptToCRC32(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Crypt.EntDecode
// > Text:string=The URL string to remove HTML entities from.
// < Text:string=The URL decoded string.
// ? Decodes HTML entities from the spcified string.
/* ------------------------------------------------------------------------- */
LLFUNC(EntDecode, 1, LuaUtilPushVar(lS, cCrypt->EntDecode(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.EntEncode
// > Text:string=The URL string to insert HTML entities into.
// < Text:string=The URL encoded string.
// ? Encodes HTML entities into the spcified string.
/* ------------------------------------------------------------------------- */
LLFUNC(EntEncode, 1, LuaUtilPushVar(lS, CryptEntEncode(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.HexDecode
// > Text:string=The hex encoded string to decode.
// < Text:Asset=The text decoded string.
// ? Encodes the specified text to uppercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexDecode, 1, LuaUtilPushVar(lS, CryptHexDecodeStr(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.HexDecodeA
// > Text:string=The hex encoded string to decode.
// < Data:Asset=The decoded data array class.
// ? Decodes the specified hexadecimal string to binary data.
/* ------------------------------------------------------------------------- */
LLFUNC(HexDecodeA, 1,
  const AgString aString{lS, 1};
  AcAsset{lS}().MemSwap({ CryptHexDecodeA(aString) }))
/* ========================================================================= */
// $ Crypt.HexEncode
// > Text:string=The string.
// < Text:string=The hex encoded string.
// ? Encodes the specified text to uppercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexEncode, 1, LuaUtilPushVar(lS, CryptStoHEX(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.HexEncodeA
// > Data:Asset=The data array class to encode.
// < Text:string=The hex encoded string.
// ? Encodes the specified data array to uppercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexEncodeA, 1, LuaUtilPushVar(lS, CryptBin2Hex(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Crypt.HexEncodeL
// > Text:string=The string.
// < Text:string=The hex encoded string.
// ? Encodes the specified text to lowercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexEncodeL, 1, LuaUtilPushVar(lS, CryptStoHEXL(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.HexEncodeLA
// > Data:Asset=The data array class to encode.
// < Text:string=The hex encoded string.
// ? Encodes the specified data array to lowercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexEncodeLA, 1, LuaUtilPushVar(lS, CryptBin2HexL(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Crypt.HMSHA1
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA1 hash.
// ? Calculates a SHA1 hash from a string using the specifiy entropy. Insecure,
// ? only use for hashing of insignificant data.
/* ------------------------------------------------------------------------- */
LLFUNC(HMSHA1, 1,
  const AgNeString aKey{lS, 1};
  const AgString aValue{lS, 2};
  LuaUtilPushVar(lS, SHA1functions::HashStr(aKey, aValue)))
/* ========================================================================= */
// $ Crypt.HMSHA224
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA224 hash.
// ? Calculates a SHA224 hash from a string using the specifiy entropy.
/* ------------------------------------------------------------------------- */
LLFUNC(HMSHA224, 1,
  const AgNeString aKey{lS, 1};
  const AgString aValue{lS, 2};
  LuaUtilPushVar(lS, SHA224functions::HashStr(aKey, aValue)))
/* ========================================================================= */
// $ Crypt.HMSHA256
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA256 hash from a string using the specifiy entropy.
/* ------------------------------------------------------------------------- */
LLFUNC(HMSHA256, 1,
  const AgNeString aKey{lS, 1};
  const AgString aValue{lS, 2};
  LuaUtilPushVar(lS, SHA256functions::HashStr(aKey, aValue)))
/* ========================================================================= */
// $ Crypt.HMSHA384
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA384 hash.
// ? Calculates a SHA384 hash from a string using the specifiy entropy.
/* ------------------------------------------------------------------------- */
LLFUNC(HMSHA384, 1,
  const AgNeString aKey{lS, 1};
  const AgString aValue{lS, 2};
  LuaUtilPushVar(lS, SHA384functions::HashStr(aKey, aValue)))
/* ========================================================================= */
// $ Crypt.HMSHA512
// > Key:string=The entropy to use.
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA512 hash.
// ? Calculates a SHA512 hash from a string using the specifiy entropy.
/* ------------------------------------------------------------------------- */
LLFUNC(HMSHA512, 1,
  const AgNeString aKey{lS, 1};
  const AgString aValue{lS, 2};
  LuaUtilPushVar(lS, SHA512functions::HashStr(aKey, aValue)))
/* ========================================================================= */
// $ Crypt.SHA1
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA1 hash.
// ? Calculates a SHA1 hash from a string. This hashing algorithm is obsolete
// ? and insecure and should no longer be used and is here only for
// ? compatibility reasons only.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA1, 1, LuaUtilPushVar(lS, SHA1functions::HashStr(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.SHA1A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA1 hash.
// ? Calculates a SHA1 hash from an array class. Insecure, only use for
// ? hashing of insignificant data.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA1A, 1, LuaUtilPushVar(lS, SHA1functions::HashMB(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Crypt.SHA224
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA224 hash from a string.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA224, 1,
  LuaUtilPushVar(lS, SHA224functions::HashStr(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.SHA224A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA224 hash from an array class.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA224A, 1,
  LuaUtilPushVar(lS, SHA224functions::HashMB(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Crypt.SHA256
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA256 hash from a string.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA256, 1,
  LuaUtilPushVar(lS, SHA256functions::HashStr(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.SHA256A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA256 hash.
// ? Calculates a SHA256 hash from an array class.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA256A, 1,
  LuaUtilPushVar(lS, SHA256functions::HashMB(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Crypt.SHA384
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA384 hash.
// ? Calculates a SHA384 hash from a string.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA384, 1,
  LuaUtilPushVar(lS, SHA384functions::HashStr(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.SHA384A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA384 hash.
// ? Calculates a SHA384 hash from an array class.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA384A, 1,
  LuaUtilPushVar(lS, SHA384functions::HashMB(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Crypt.SHA512
// > Data:string=The string to hash.
// < Hash:string=The calculated SHA512 hash.
// ? Calculates a SHA512 hash from a string.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA512, 1,
  LuaUtilPushVar(lS, SHA512functions::HashStr(AgString{lS, 1})))
/* ========================================================================= */
// $ Crypt.SHA512A
// > Data:array=The data to hash.
// < Hash:string=The calculated SHA512 hash.
// ? Calculates a SHA384 hash from an array class.
/* ------------------------------------------------------------------------- */
LLFUNC(SHA512A, 1,
  LuaUtilPushVar(lS, SHA512functions::HashMB(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Crypt.UrlDecode
// > Text:string=The URL string to decode.
// < Text:string=The URL decoded string.
// ? URL decodes the specified string.
/* ------------------------------------------------------------------------- */
LLFUNC(UrlDecode, 1, LuaUtilPushVar(lS, CryptURLDecode(AgCStringChar{lS, 1})))
/* ========================================================================= */
// $ Crypt.UrlEncode
// > Text:string=The URL string to encode.
// < Text:string=The URL encoded string.
// ? URL encodes the specified string.
/* ------------------------------------------------------------------------- */
LLFUNC(UrlEncode, 1, LuaUtilPushVar(lS, CryptURLEncode(AgCStringChar{lS, 1})))
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
