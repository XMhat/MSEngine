/* == LUAUTIL.HPP ========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Lua utility functions. They normally need a state to work.          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaUtil {                   // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IDir::P;               using namespace IError::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IToken::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Variables ------------------------------------------------------------ */
static unsigned int uiLuaPaused = 0;   // Times Lua paused before handling it
/* -- Utility type defs ---------------------------------------------------- */
struct LuaUtilClass { void *vpPtr; };  // Holds a pointer to a class
/* -- Prune stack ---------------------------------------------------------- */
static void LuaUtilPruneStack(lua_State*const lS, const int iParam)
  { lua_settop(lS, iParam); }
/* -- Return items in stack ------------------------------------------------ */
static int LuaUtilStackSize(lua_State*const lS) { return lua_gettop(lS); }
/* -- Return if stack is empty --------------------------------------------- */
static bool LuaUtilIsStackEmpty(lua_State*const lS)
  { return LuaUtilStackSize(lS) == 0; }
/* -- Get length of a table ------------------------------------------------ */
static lua_Unsigned LuaUtilGetSize(lua_State*const lS, const int iParam)
  { return lua_rawlen(lS, iParam); }
/* -- Get and return a C++ string without checking it ---------------------- */
static const string LuaUtilToCppString(lua_State*const lS, const int iParam=-1)
{ // Storage for string length. Do not optimise this because I am not sure
  // what the standard direction is for evaluating expression. Left-to-right
  // or right-to-left, so I will just store the string point first to be safe.
  size_t stLength;
  const char*const cpStr = lua_tolstring(lS, iParam, &stLength);
  return { cpStr, stLength };
}
/* -- Return type of item in stack ----------------------------------------- */
static const string LuaUtilGetStackType(lua_State*const lS, const int iIndex)
{ // What type of variable?
  switch(const int iI = lua_type(lS, iIndex))
  { // Nil?
    case LUA_TNIL: return "nil"; break;
    // A number?
    case LUA_TNUMBER:
    { // If not actually an integer? Write as normal floating-point number
      if(!lua_isinteger(lS, iIndex))
        return StrFromNum(lua_tonumber(lS, iIndex));
      // Get actual integer value and return it and it's hex value
      const lua_Integer liValue = lua_tointeger(lS, iIndex);
      return StrFormat("$ [0x$$]", liValue, hex, liValue);
    } // A boolean?
    case LUA_TBOOLEAN: return StrFromBoolTF(lua_toboolean(lS, iIndex));
    // A string?
    case LUA_TSTRING:
    { // Get value of string and return value with size
      const string strVal{ LuaUtilToCppString(lS, iIndex) };
      return StrFormat("[$] \"$\"", strVal.length(), strVal);
    } // A table?
    case LUA_TTABLE: return StrFormat("<table:$>[$]",
      lua_topointer(lS, iIndex), LuaUtilGetSize(lS, iIndex));
    // Who knows? Function? Userdata?
    default: return StrFormat("<$:$>",
      lua_typename(lS, iI), lua_topointer(lS, iIndex));
  }
}
/* -- Return status of item in stack --------------------------------------- */
static const string LuaUtilGetStackTokens(lua_State*const lS, const int iIndex)
{ // Fill token buffer depending on status
  return StrFromEvalTokens({
    { lua_isboolean(lS, iIndex),       'B' },
    { lua_iscfunction(lS, iIndex),     'C' },
    { lua_isfunction(lS, iIndex),      'F' },
    { lua_islightuserdata(lS, iIndex), 'L' },
    { lua_isnil(lS, iIndex),           'X' },
    { lua_isnone(lS, iIndex),          '0' },
    { lua_isinteger(lS, iIndex),       'I' },
    { lua_isnumber(lS, iIndex),        'N' },
    { lua_isstring(lS, iIndex),        'S' },
    { lua_istable(lS, iIndex),         'T' },
    { lua_isthread(lS, iIndex),        'R' },
    { lua_isuserdata(lS, iIndex),      'U' },
  });
}
/* -- Log the stack -------------------------------------------------------- */
static const string LuaUtilGetVarStack(lua_State*const lS)
{ // If there are variables in the stack?
  if(const int iCount = LuaUtilStackSize(lS))
  { // String to return
    ostringstream osS;
     // For each element (1 is the first item)
    for(int iIndex = 1; iIndex <= iCount; ++iIndex)
      osS << iIndex
          << "["
          << iIndex - iCount - 1
          << "] (" << LuaUtilGetStackTokens(lS, iIndex) << ") "
          << LuaUtilGetStackType(lS, iIndex)
          << cCommon->Lf();
    // Return string
    return osS.str();
  } // No elements in variable stack
  return "<empty stack>";
}
/* -- Set hook ------------------------------------------------------------- */
static void LuaUtilSetHookCallback(lua_State*const lS,
  lua_Hook fcbCb, const int iC)
    { lua_sethook(lS, fcbCb, LUA_MASKCOUNT, iC); }
/* -- Push a table onto the stack ------------------------------------------ */
template<typename IdxIntType=int, typename KeyIntType=int>
  static void LuaUtilPushTable(lua_State*const lS,
    const IdxIntType iitIndexes=0, const KeyIntType kitKeys=0)
{ lua_createtable(lS, UtilIntOrMax<int>(iitIndexes),
                      UtilIntOrMax<int>(kitKeys)); }
/* -- Push a nil onto the stack -------------------------------------------- */
static void LuaUtilPushNil(lua_State*const lS) { lua_pushnil(lS); }
/* -- Push a boolean onto the stack ---------------------------------------- */
template<typename IntType>
  static void LuaUtilPushBool(lua_State*const lS, const IntType itValue)
{ lua_pushboolean(lS, static_cast<bool>(itValue)); }
/* -- Push a number onto the stack ----------------------------------------- */
template<typename IntType>
  static void LuaUtilPushNum(lua_State*const lS, const IntType itValue)
{ lua_pushnumber(lS, static_cast<lua_Number>(itValue)); }
/* -- Push an integer onto the stack --------------------------------------- */
template<typename IntType>
  static void LuaUtilPushInt(lua_State*const lS, const IntType itValue)
{ lua_pushinteger(lS, static_cast<lua_Integer>(itValue)); }
/* -- Push a string onto the stack ----------------------------------------- */
template<typename PtrType>
  static void LuaUtilPushCStr(lua_State*const lS, const PtrType ptValue)
{ lua_pushstring(lS, reinterpret_cast<const char*>(ptValue)); }
/* -- Push a literal string onto the stack --------------------------------- */
template<typename PtrType, typename IntType>
  static void LuaUtilPushLStr(lua_State*const lS, const PtrType ptValue,
    const IntType itSize)
{ lua_pushlstring(lS, reinterpret_cast<const char*>(ptValue),
                      static_cast<size_t>(itSize)); }
/* -- Push a memory block onto the stack as a string ----------------------- */
static void LuaUtilPushMem(lua_State*const lS, const MemConst &mcSrc)
    { LuaUtilPushLStr(lS, mcSrc.MemPtr<char>(), mcSrc.MemSize()); }
/* -- Push a C++ string onto the stack ------------------------------------- */
static void LuaUtilPushStr(lua_State*const lS, const string &strStr)
  { LuaUtilPushLStr(lS, strStr.data(), strStr.length()); }
/* -- Push a C++ string view onto the stack -------------------------------- */
static void LuaUtilPushStrView(lua_State*const lS, const string_view &strvStr)
  { LuaUtilPushLStr(lS, strvStr.data(), strvStr.length()); }
/* -- Push a pointer ------------------------------------------------------- */
static void LuaUtilPushPtr(lua_State*const lS, void*const vpPtr)
  { lua_pushlightuserdata(lS, vpPtr); }
/* -- Push multiple values of different types (use in ll*.hpp sources) ----- */
static void LuaUtilPushVar(lua_State*const) { }
template<typename ...VarArgs, typename AnyType>
  static void LuaUtilPushVar(lua_State*const lS, const AnyType &atVal,
    const VarArgs &...vaVars)
{ // Type is std::string?
  if constexpr(is_same_v<AnyType, string>) LuaUtilPushStr(lS, atVal);
  // Type is boolean?
  else if constexpr(is_same_v<AnyType, bool>) LuaUtilPushBool(lS, atVal);
  // Type is any pointer type (assuming char*, don't send anything else)
  else if constexpr(is_pointer_v<AnyType>) LuaUtilPushCStr(lS, atVal);
  // Type is enum, int, long, short or int64?
  else if constexpr(is_integral_v<AnyType> || is_enum_v<AnyType>)
    LuaUtilPushInt(lS, atVal);
  // Type is float or double?
  else if constexpr(is_floating_point_v<AnyType>) LuaUtilPushNum(lS, atVal);
  // Just push nil otherwise
  else LuaUtilPushNil(lS);
  // Shift to next variable
  LuaUtilPushVar(lS, vaVars...);
}
/* -- Throw error ---------------------------------------------------------- */
static void LuaUtilErrThrow(lua_State*const lS) { lua_error(lS); }
/* -- Push C-String on stack and throw ------------------------------------- */
static void LuaUtilPushErr(lua_State*const lS,
  const char*const cpReason)
{ LuaUtilPushCStr(lS, cpReason); LuaUtilErrThrow(lS); }
/* -- Get human readable name of specified type id ------------------------- */
static const char *LuaUtilGetType(lua_State*const lS, const int iIndex)
  { return lua_typename(lS, lua_type(lS, iIndex)); }
/* -- Remove item from stack ----------------------------------------------- */
static void LuaUtilRmStack(lua_State*const lS, const int iParam=-1)
  { lua_remove(lS, iParam); }
/* -- Get and pop string on top -------------------------------------------- */
static const string LuaUtilGetAndPopStr(lua_State*const lS)
{ // If there is nothing on the stack then return a generic error
  if(lua_isnone(lS, -1)) return "Error signalled with no reason";
  // Not have a string on stack? Set embedded error!
  if(!lua_isstring(lS, -1))
    return StrFormat("Error signalled with invalid '$' reason",
      LuaUtilGetType(lS, -1));
  // Get error string
  const string strError{ LuaUtilToCppString(lS) };
  // Remove the error string
  LuaUtilRmStack(lS);
  // return the error
  return strError;
}
/* -- Return reference ----------------------------------------------------- */
static void LuaUtilGetRefEx(lua_State*const lS, const int iTable=1,
  const lua_Integer liIndex=1) { lua_rawgeti(lS, iTable, liIndex); }
/* -- Return reference ----------------------------------------------------- */
static void LuaUtilGetRef(lua_State*const lS, const int iParam)
  { LuaUtilGetRefEx(lS, LUA_REGISTRYINDEX, static_cast<lua_Integer>(iParam)); }
/* -- Return referenced function ------------------------------------------- */
static bool LuaUtilGetRefFunc(lua_State*const lS, const int iParam)
{ // If context and reference are valid?
  if(lS && iParam != LUA_REFNIL)
  { // Push the userdata onto the stack and return success if successful
    LuaUtilGetRef(lS, iParam);
    if(lua_isfunction(lS, -1)) return true;
    // Failed so remove whatever it was
    LuaUtilRmStack(lS);
  } // Failure
  return false;
}
/* -- Return referenced userdata ------------------------------------------- */
static bool LuaUtilGetRefUsrData(lua_State*const lS, const int iParam)
{ // If context and reference are valid?
  if(lS && iParam != LUA_REFNIL)
  { // Push the userdata onto the stack and return success if successful
    LuaUtilGetRef(lS, iParam);
    if(lua_isuserdata(lS, -1)) return true;
    // Failed so remove whatever it was
    LuaUtilRmStack(lS);
  } // Failure
  return false;
}
/* -- Remove reference to hidden variable without checking ----------------- */
static void LuaUtilRmRefUnsafe(lua_State*const lS, const int iRef)
  { luaL_unref(lS, LUA_REGISTRYINDEX, iRef); }
/* -- Remove reference to hidden variable ---------------------------------- */
static bool LuaUtilRmRef(lua_State*const lS, const int iRef)
{ // Return if no state or reference
  if(!lS || iRef == LUA_REFNIL) return false;
  // Unreference
  LuaUtilRmRefUnsafe(lS, iRef);
  // Success
  return true;
}
/* -- Safely delete a reference -------------------------------------------- */
static bool LuaUtilRmRefSafe(lua_State*const lS,
  int &iRef, const int iSRef=LUA_REFNIL)
{ // Remove state and return failed if failed
  const bool bReturn = LuaUtilRmRef(lS, iRef);
  // Reset the reference
  iRef = iSRef;
  // Success
  return bReturn;
}
/* -- Get a new reference without checking --------------------------------- */
static int LuaUtilRefInitUnsafe(lua_State*const lS)
  { return luaL_ref(lS, LUA_REGISTRYINDEX); }
/* -- Get a new reference -------------------------------------------------- */
static int LuaUtilRefInit(lua_State*const lS)
{ // Return if no state or nothing on the stack
  if(!lS || LuaUtilIsStackEmpty(lS)) return LUA_REFNIL;
  // Return result
  return LuaUtilRefInitUnsafe(lS);
}
/* -- Set a new reference -------------------------------------------------- */
static bool LuaUtilRefInit(lua_State*const lS, int &iRef)
{ // Do the reference
  iRef = LuaUtilRefInit(lS);
  // Done
  return iRef != LUA_REFNIL;
}
/* ------------------------------------------------------------------------- */
static const string LuaUtilStack(lua_State*const lST)
{ // We need the root state so we can iterate through all the threads and will
  // eventually arrive at *lS as the last stack. Most of the time GetState()
  // equals to *lS anyway, just depends if it triggered in a co-routine or not.
  LuaUtilGetRef(lST, LUA_RIDX_MAINTHREAD);
  lua_State *lS = lua_tothread(lST, -1);
  LuaUtilRmStack(lST);
  // Return if state is invalid. Impossible really but just incase.
  if(!lS) return "\n- Could not find main thread!";
  // list of stack traces for coroutines. They are ordered from most recent
  // call to the root call so we need to use this list to reverse them after.
  // Also we (or even Lua) does know how many total calls there has been, we
  // can only enumerate them.
  typedef list<lua_Debug> LuaStack;
  typedef LuaStack::reverse_iterator LuaStackRevIt;
  LuaStack lsStack;
  // Co-routine id so user knows which coroutine sub-level they were at.
  int iCoId = 0;
  // Loop until we've enumerated all the upstates
  do
  { // list of stack traces for this coroutine
    LuaStack lsThread;
    // For each stack
    for(int iParam = 0; ; ++iParam)
    { // Lua debug info container
      lua_Debug ldData;
      // Read stack data
      if(!lua_getstack(lS, iParam, &ldData)) break;
      // Set co-routine id. We're not using this 'event' var and neither does
      // LUA in lua_getinfo() according to ldebug.c.
      ldData.event = iCoId;
      // Insert into list
      lsThread.emplace_front(StdMove(ldData));
    } // Move into lsStack in reverse order
    lsStack.splice(lsStack.cend(), lsThread);
    // If the top item is not a thread? We're done
    if(!lua_isthread(lS, 1)) break;
    // Set parent thread
    lS = lua_tothread(lS, 1);
    // Increment coroutine id
    iCoId++;
  } // Until theres no more upstates
  while(lS);
  // String to return
  ostringstream osS;
  // Stack id that will get decremented to 0 (the root call)
  size_t stId = lsStack.size();
  // For each stack trace
  for(LuaStackRevIt lsriIt{ lsStack.rbegin() };
                    lsriIt != lsStack.rend();
                  ++lsriIt)
  { // Get thread data
    lua_Debug &ldData = *lsriIt;
    // Query stack and ignore if failed or line is invalid and there is no name
    if(!lua_getinfo(lS, "Slnu", &ldData) ||
      (ldData.currentline == -1 && !ldData.name)) continue;
    // Prepare start of stack trace
    osS << "\n- " << --stId << ':' << ldData.event << " = "
        << ldData.short_src;
    // We have line data? StrAppend data to string
    if(ldData.currentline != -1)
      osS << " @ " << ldData.currentline << '['
          << ldData.linedefined << '-' << ldData.lastlinedefined << ']';
    // Write rest of data
    osS << " : " << (ldData.name ? ldData.name : "?") << '('
        << (*ldData.namewhat ? ldData.namewhat : "?") << ';'
        << static_cast<unsigned int>(ldData.nparams) << ';'
        << static_cast<unsigned int>(ldData.nups) << ')';
  } // Return formatted stack string
  return osS.str();
}
/* -- Generic panic handler ------------------------------------------------ */
static int LuaUtilException(lua_State*const lS)
{ // Get error message and stack. Don't one line this because the order of
  // execution is important!
  const string strError{ LuaUtilGetAndPopStr(lS) };
  XC(StrAppend(strError, LuaUtilStack(lS)));
}
/* -- Generic error handler ------------------------------------------------ */
static int LuaUtilErrGeneric(lua_State*const lS)
{ // Get error message and stack. Don't one line this because the order of
  // execution is important!
  const string strError{ LuaUtilGetAndPopStr(lS) };
  LuaUtilPushStr(lS, StrAppend(strError, LuaUtilStack(lS)));
  return 1;
}
/* -- Push a function onto the stack --------------------------------------- */
static void LuaUtilPushCFunc(lua_State*const lS, lua_CFunction cFunc,
  const int iNVals=0)
    { lua_pushcclosure(lS, cFunc, iNVals); }
/* -- Push the above generic error function and return its id -------------- */
static int LuaUtilPushAndGetGenericErrId(lua_State*const lS)
  { LuaUtilPushCFunc(lS, LuaUtilErrGeneric); return LuaUtilStackSize(lS); }
/* == Generate an exception if the specified condition is false ============ */
static void LuaUtilAssert(lua_State*const lS, const bool bCond,
  const int iIndex, const char*const cpName, const char*const cpType)
{ // Ignore if condition is true
  if(bCond) return;
  // Throw error message
  XC("Invalid parameter!",
    "Name",     cpName, "Parameter", iIndex,
    "Required", cpType, "Supplied",  LuaUtilGetType(lS, iIndex));
}
/* -- Check that parameter is a table -------------------------------------- */
static void LuaUtilCheckTable(lua_State*const lS, const int iParam,
  const char*const cpName)
    { LuaUtilAssert(lS, !!lua_istable(lS, iParam), iParam, cpName, "table"); }
/* -- Check that parameter is a string ------------------------------------- */
static void LuaUtilCheckStr(lua_State*const lS, const int iParam,
  const char*const cpName)
    { LuaUtilAssert(lS, !!lua_isstring(lS, iParam),
        iParam, cpName, "string"); }
/* -- Check that parameter is a string and is not empty -------------------- */
static void LuaUtilCheckStrNE(lua_State*const lS, const int iParam,
  const char*const cpName)
{ // Check the string is valid and return if the string is not empty
  LuaUtilCheckStr(lS, iParam, cpName);
  if(lua_rawlen(lS, iParam) > 0) return;
  // Throw error message
  XC("Non-empty string required!", "Name", cpName, "Parameter", iParam);
}
/* -- Get the specified string from the stack ------------------------------ */
template<typename StrType>
  static const StrType *LuaUtilGetStr(lua_State*const lS,
    const int iParam, const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  LuaUtilCheckStr(lS, iParam, cpName);
  // Return the number
  return reinterpret_cast<const StrType*>(lua_tostring(lS, iParam));
}
/* -- Get the specified string from the stack ------------------------------ */
template<typename StrType>
  static const StrType *LuaUtilGetStrNE[[maybe_unused]](lua_State*const lS,
    const int iParam, const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  LuaUtilCheckStrNE(lS, iParam, cpName);
  // Return the number
  return reinterpret_cast<const StrType*>(lua_tostring(lS, iParam));
}
/* -- Get and return a string and throw exception if not a string ---------- */
template<typename StrType>
  static const StrType *LuaUtilGetLStr(lua_State*const lS,
    const int iParam, size_t &stLen, const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  LuaUtilCheckStr(lS, iParam, cpName);
  // Return the number
  return reinterpret_cast<const StrType*>(lua_tolstring(lS, iParam, &stLen));
}
/* -- Helper for LuaUtilGetLStr that makes a memory block ------------------ */
static Memory LuaUtilGetMBfromLStr(lua_State*const lS, const int iParam,
  const char*const cpName)
{ // Storage for size
  size_t stStrLen;
  // Get string and store size in string.
  const char*const cpStr = LuaUtilGetLStr<char>(lS, iParam, stStrLen, cpName);
  // Return as memory block
  return { stStrLen, cpStr };
}
/* -- Get and return a C++ string and throw exception if not a string ------ */
static const string LuaUtilGetCppStr(lua_State*const lS, const int iParam,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  LuaUtilCheckStr(lS, iParam, cpName);
  // Return the constructed string
  return LuaUtilToCppString(lS, iParam);
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
static const string LuaUtilGetCppStrNE(lua_State*const lS, const int iParam,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  LuaUtilCheckStrNE(lS, iParam, cpName);
  // Return the constructed string
  return LuaUtilToCppString(lS, iParam);
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
static const string LuaUtilGetCppFile(lua_State*const lS, const int iParam,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  LuaUtilCheckStr(lS, iParam, cpName);
  // Get the filename and verify that the filename is valid
  const string strFile{ LuaUtilToCppString(lS, iParam) };
  if(const ValidResult vrId = DirValidName(strFile))
    XC("Invalid parameter!",
       "Param",    iParam,                       "File",    strFile,
       "Reason",   cDirBase->VNRtoStr(vrId), "ReasonId", vrId);
  // Return the constructed string
  return strFile;
}
/* -- Get and return a C++ string and throw exception if not a string ------ */
static const string LuaUtilGetCppStrUpper(lua_State*const lS, const int iParam,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  string strStr{ LuaUtilGetCppStr(lS, iParam, cpName) };
  // Return the constructed string converting it to upper case
  return StrToUpCaseRef(strStr);
}
/* -- Check the specified number of parameters are set --------------------- */
static void LuaUtilCheckParams(lua_State*const lS, const int iCount)
{ // Get number of parameters supplied and we're ok if the count is correct
  const int iTop = LuaUtilStackSize(lS);
  if(iCount == iTop) return;
  // Push error message
  XC((iCount < iTop) ? "Too many arguments!" : "Not enough arguments!",
    "Supplied", iTop, "Required", iCount);
}
/* -- Check the specified function is valid -------------------------------- */
static void LuaUtilCheckFunc(lua_State*const lS, const int iIndex,
  const char*const cpName)
    { LuaUtilAssert(lS, !!lua_isfunction(lS, iIndex),
        iIndex, cpName, "function"); }
/* -- Check multiple functions are valid ----------------------------------- */
static void LuaUtilCheckFuncs(lua_State*const) { }
template<typename ...VarArgs>
  static void LuaUtilCheckFuncs(lua_State*const lS, const int iIndex,
    const char*const cpName, const VarArgs &...vaVars)
      { LuaUtilCheckFunc(lS, iIndex, cpName);
        LuaUtilCheckFuncs(lS, vaVars...); }
/* -- Get and return a boolean and throw exception if not a boolean -------- */
static bool LuaUtilGetBool(lua_State*const lS, const int iIndex,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid boolean
  LuaUtilAssert(lS, !!lua_isboolean(lS, iIndex), iIndex, cpName, "boolean");
  // Return the number
  return lua_toboolean(lS, iIndex) == 1;
}
/* -- Try to get and check a valid number not < or >= ---------------------- */
template<typename IntType>
  static IntType LuaUtilGetNum(lua_State*const lS, const int iIndex,
    const char*const cpName)
{ // Test to make sure if supplied parameter is a valid number (float)
  LuaUtilAssert(lS, !!lua_isnumber(lS, iIndex), iIndex, cpName, "number");
  // Return the number
  return static_cast<IntType>(lua_tonumber(lS, iIndex));
}
/* -- Try to get and check a valid number not < or > ----------------------- */
template<typename IntType>
  static IntType LuaUtilGetNumLG(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Throw if value not a number and return if it is in the valid range
  const IntType itVal = LuaUtilGetNum<IntType>(lS, iIndex, cpName);
  if(itVal >= itMin && itVal <= itMax) return itVal;
  // Throw error
  XC("Number out of range!",
    "Parameter", iIndex, "Name",       cpName, "Supplied", itVal,
    "NotLesser", itMin,  "NotGreater", itMax);
}
/* -- Try to get and check a valid number not < or >= ---------------------- */
template<typename IntType>
  static IntType LuaUtilGetNumLGE(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Throw if value not a number and return if it is in the valid range
  const IntType itVal = LuaUtilGetNum<IntType>(lS, iIndex, cpName);
  if(itVal >= itMin && itVal < itMax) return itVal;
  // Throw error
  XC("Number out of range!",
    "Parameter", iIndex, "Name",            cpName, "Supplied", itVal,
    "NotLesser", itMin,  "NotGreaterEqual", itMax);
}
/* -- Try to get and force a value between -1.0 and 1.0 -------------------- */
template<typename IntType>
  static IntType LuaUtilGetNormal(lua_State*const lS, const int iIndex,
    const char*const cpName)
      { return static_cast<IntType>(fmod(LuaUtilGetNum<lua_Number>(lS,
          iIndex, cpName), 1.0)); }
/* -- Try to get and check a valid integer --------------------------------- */
template<typename IntType>
  static IntType LuaUtilGetInt(lua_State*const lS, const int iIndex,
    const char*const cpName)
{ // Test to make sure if supplied parameter is a valid integer (not float)
  LuaUtilAssert(lS, !!lua_isinteger(lS, iIndex), iIndex, cpName, "integer");
  // Return the number
  return static_cast<IntType>(lua_tointeger(lS, iIndex));
}
/* -- Try to get and check a valid integer range not < or > ---------------- */
template<typename IntType>
  static IntType LuaUtilGetIntLG(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Throw if value not a integer and return if it is in the valid range
  const IntType itVal = LuaUtilGetInt<IntType>(lS, iIndex, cpName);
  if(itVal >= itMin && itVal <= itMax) return itVal;
  // Throw error
  XC("Integer out of range!",
    "Parameter", iIndex, "Name",       cpName, "Supplied", itVal,
    "NotLesser", itMin,  "NotGreater", itMax);
}
/* -- Try to get and check a valid integer range not < or > and = ^2 ------- */
template<typename IntType>
  static IntType LuaUtilGetIntLGP2(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Get value in specified range it must be a power of two
  const IntType itVal = LuaUtilGetIntLG(lS, iIndex, itMin, itMax, cpName);
  if(StdIntIsPOW2(itVal)) return itVal;
  // Throw error
  XC("Integer is not a power of two!",
    "Parameter", iIndex, "Name", cpName, "Supplied", itVal);
}
/* -- Try to get and check a valid integer range not < or >= --------------- */
template<typename IntType>
  static IntType LuaUtilGetIntLGE(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Throw if value not a integer and return if it is in the valid range
  const IntType itVal = LuaUtilGetInt<IntType>(lS, iIndex, cpName);
  if(itVal >= itMin && itVal < itMax) return itVal;
  // Throw error
  XC("Integer out of range!",
    "Parameter", iIndex, "Name",            cpName, "Supplied", itVal,
    "NotLesser", itMin,  "NotGreaterEqual", itMax);
}
/* -- Try to get and check a valid integer range not <= or > --------------- */
template<typename IntType>
  static IntType LuaUtilGetIntLEG(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Throw if value not a integer and return if it is in the valid range
  const IntType itVal = LuaUtilGetInt<IntType>(lS, iIndex, cpName);
  if(itVal > itMin && itVal <= itMax) return itVal;
  // Throw error
  XC("Integer out of range!",
    "Parameter",      iIndex, "Name",       cpName, "Supplied", itVal,
    "NotLesserEqual", itMin,  "NotGreater", itMax);
}
/* -- Try to get and check a 'Flags' parameter ----------------------------- */
template<class FlagsType>
  static const FlagsType LuaUtilGetFlags(lua_State*const lS, const int iIndex,
    const FlagsType &ftMask, const char*const cpName)
{ // Throw if value not a integer and return if it is in the valid range
  const FlagsType ftFlags{
    LuaUtilGetInt<decltype(ftFlags.FlagGet())>(lS, iIndex, cpName) };
  if(ftFlags.FlagIsZero() || ftFlags.FlagIsInMask(ftMask)) return ftFlags;
  // Throw error
  XC("Flags out of range!",
    "Parameter", iIndex,          "Name", cpName,
    "Supplied",  ftFlags.FlagGet(), "Mask", ftMask.FlagGet());
}
/* -- Destroy an object ---------------------------------------------------- */
template<class ClassType>
  static void LuaUtilClassDestroy(lua_State*const lS, const int iParam,
    const char*const cpName)
{ // Get userdata pointer from Lua and if the address is valid?
  if(LuaUtilClass*const lucPtr =
    reinterpret_cast<LuaUtilClass*>(luaL_checkudata(lS, iParam, cpName)))
  { // Get address to the C++ class and if that is valid?
    if(ClassType*const ctPtr = reinterpret_cast<ClassType*>(lucPtr->vpPtr))
    { // Clear the pointer to the C++ class
      lucPtr->vpPtr = nullptr;
      // Destroy the C++ class if it is not internally locked
      if(ctPtr->LockIsNotSet()) delete ctPtr;
    }
  }
}
/* -- Creates a new item for object ---------------------------------------- */
static LuaUtilClass *LuaUtilClassPrepNew(lua_State*const lS,
  const char*const cpType)
{ // Create userdata
  LuaUtilClass*const lucPtr =
    reinterpret_cast<LuaUtilClass*>(lua_newuserdata(lS, sizeof(LuaUtilClass)));
  // Initialise meta data and if it hasn't been initialised yet?
  if(luaL_getmetatable(lS, cpType) == LUA_TNIL)
    XC("Metatable not available!", "Type", cpType);
  // Get and push garbage collector function. This is set from the
  // SetupEnvironment() function in lua.hpp.
  LuaUtilPushCStr(lS, "__dtor");
  lua_rawget(lS, -2);
  if(!lua_iscfunction(lS, -1))
    XC("Metatable destructor invalid!", "Type", cpType);
  lua_setfield(lS, -2, "__gc");
  // Done setting metamethods, set the table
  lua_setmetatable(lS, -2);
  // Return pointer to new class allocated by Lua
  return lucPtr;
}
/* -- Takes ownership of an object ----------------------------------------- */
template<class ClassType>
  static ClassType *LuaUtilClassReuse(lua_State*const lS,
    const char*const cpType, ClassType*const ctPtr)
{ // Prepare a new object
  LuaUtilClass*const lucPtr = LuaUtilClassPrepNew(lS, cpType);
  // Assign object to lua so lua will be incharge of deleting it
  lucPtr->vpPtr = ctPtr;
  // Return pointer to new class allocated elseware
  return ctPtr;
}
/* -- Creates and allocates a pointer to a new class ----------------------- */
template<typename ClassType>
  static ClassType *LuaUtilClassCreate(lua_State*const lS,
    const char*const cpType)
{ // Prepare a new object
  LuaUtilClass*const lucPtr = LuaUtilClassPrepNew(lS, cpType);
  // Allocate class and return it if succeeded return it
  if(void*const vpPtr = lucPtr->vpPtr = new (nothrow)ClassType)
    return reinterpret_cast<ClassType*>(vpPtr);
  // Error occured so just throw exception
  XC("Failed to allocate class!", "Type", cpType, "Size", sizeof(ClassType));
}
/* -- Creates a pointer to a class that LUA CAN'T deallocate --------------- */
template<typename ClassType>
  static ClassType *LuaUtilClassCreatePtr(lua_State*const lS,
    const char*const cpType, ClassType*const ctPtr)
{ // Create userdata
  LuaUtilClass*const lucPtr =
    reinterpret_cast<LuaUtilClass*>(lua_newuserdata(lS, sizeof(LuaUtilClass)));
  // Initialise meta data and if it hasn't been initialised yet?
  if(luaL_getmetatable(lS, cpType) == LUA_TNIL)
    XC("Metadata not available!", "Type", cpType);
  // Done setting methods, set the table
  lua_setmetatable(lS, -2);
  // Set pointer to class
  lucPtr->vpPtr = reinterpret_cast<void*>(ctPtr);
  // Return pointer to memory
  return ctPtr;
}
/* -- Gets a pointer to any class ------------------------------------------ */
template<typename ClassType>
  ClassType *LuaUtilGetPtr(lua_State*const lS, const int iParam,
    const char*const cpName)
{ // Get lua data class and if it is valid
  if(const LuaUtilClass*const lucPtr =
    reinterpret_cast<LuaUtilClass*>(luaL_checkudata(lS, iParam, cpName)))
  { // Get reference to class and return pointer if valid
    const LuaUtilClass &lcR = *lucPtr;
    if(lcR.vpPtr) return reinterpret_cast<ClassType*>(lcR.vpPtr);
    // Actual class pointer has already been freed so error occured
    XC("Unallocated class parameter!", "Parameter", iParam, "Type", cpName);
  } // lua data class not valid
  XC("Null class parameter!", "Parameter", iParam, "Type", cpName);
}
/* -- Check that a class isn't locked (i.e. a built-in class) -------------- */
template<class ClassType>
  ClassType *LuaUtilGetUnlockedPtr(lua_State*const lS, const int iParam,
    const char*const cpName)
{ // Get pointer to class and return if isn't locked (a built-in class)
  ClassType*const ctPtr = LuaUtilGetPtr<ClassType>(lS, iParam, cpName);
  if(ctPtr->LockIsNotSet()) return ctPtr;
  // Throw error
  XC("Call not allowed on this class!", "Identifier", ctPtr->IdentGet());
}
/* -- Get the light user data pointer -------------------------------------- */
template<typename PtrType>
  static PtrType *LuaUtilGetSimplePtr(lua_State*const lS, const int iParam)
{ // Check that it is user data
  if(!lua_isuserdata(lS, iParam)) XC("Not userdata!", "Param", iParam);
  // Test to make sure if supplied parameter is a valid string
  void*const vpPtr = lua_touserdata(lS, iParam);
  // Return the pointer as the requested type
  return reinterpret_cast<PtrType*>(vpPtr);
}
/* -- Garbage collection control (two params) ------------------------------ */
static int LuaUtilGCSet(lua_State*const lS, const int iCmd, const int iVal1,
  const int iVal2)
    { return lua_gc(lS, iCmd, iVal1, iVal2); }
/* -- Garbage collection control (one param) ------------------------------- */
static int LuaUtilGCSet[[maybe_unused]](lua_State*const lS, const int iCmd,
  const int iVal)
    { return lua_gc(lS, iCmd, iVal); }
/* -- Garbage collection control (no param) -------------------------------- */
static int LuaUtilGCSet(lua_State*const lS, const int iCmd)
  { return lua_gc(lS, iCmd); }
/* -- Stop garbage collection ---------------------------------------------- */
static int LuaUtilGCStop(lua_State*const lS)
  { return LuaUtilGCSet(lS, LUA_GCSTOP); }
/* -- Start garbage collection --------------------------------------------- */
static int LuaUtilGCStart(lua_State*const lS)
  { return LuaUtilGCSet(lS, LUA_GCRESTART); }
/* -- Execute garbage collection ------------------------------------------- */
static int LuaUtilGCRun(lua_State*const lS)
  { return LuaUtilGCSet(lS, LUA_GCCOLLECT); }
/* -- Returns if garbage collection is running ----------------------------- */
static bool LuaUtilGCRunning(lua_State*const lS)
  { return !!LuaUtilGCSet(lS, LUA_GCISRUNNING); }
/* -- Get memory usage ----------------------------------------------------- */
static size_t LuaUtilGetUsage(lua_State*const lS)
  { return static_cast<size_t>(LuaUtilGCSet(lS, LUA_GCCOUNT) +
      LuaUtilGCSet(lS, LUA_GCCOUNTB) / 1024) * 1024; }
/* -- Full garbage collection while logging memory usage ------------------- */
static size_t LuaUtilGCCollect(lua_State*const lS)
{ // Get current usage, do a full garbage collect and return delta
  const size_t stUsage = LuaUtilGetUsage(lS);
  LuaUtilGCRun(lS);
  return stUsage - LuaUtilGetUsage(lS);
}
/* -- Standard in-sandbox call function (unmanaged) ------------------------ */
static void LuaUtilCallFuncEx(lua_State*const lS, const int iParams=0,
  const int iReturns=0)
    { lua_call(lS, iParams, iReturns); }
/* -- Standard in-sandbox call function (unmanaged, no params) ------------- */
static void LuaUtilCallFunc(lua_State*const lS, const int iReturns=0)
  { LuaUtilCallFuncEx(lS, 0, iReturns); }
/* -- Sandboxed call function (doesn't remove error handler) --------------- */
static int LuaUtilPCallEx(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
    { return lua_pcall(lS, iParams, iReturns, iHandler); }
/* -- Sandboxed call function (removes error handler) ---------------------- */
static int LuaUtilPCallExSafe(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
{ // Do protected call and get result
  const int iResult = LuaUtilPCallEx(lS, iParams, iReturns, iHandler);
  // Remove error handler from stack if handler specified
  if(iHandler) LuaUtilRmStack(lS, iHandler);
  // Return result
  return iResult;
}
/* -- Handle LuaUtilPCall result ------------------------------------------- */
static void LuaUtilPCallResultHandle(lua_State*const lS, const int iResult)
{ // Compare error code
  switch(iResult)
  { // No error
    case 0: return;
    // Run-time error
    case LUA_ERRRUN:
      XC(StrAppend("Runtime error! > ", LuaUtilGetAndPopStr(lS)));
    // Memory allocation error
    case LUA_ERRMEM:
      XC("Memory allocation error!", "Usage", LuaUtilGetUsage(lS));
    // Error + error in error handler
    case LUA_ERRERR: XC("Error in error handler!");
    // Unknown error
    default: XC("Unknown error!");
  }
}
/* -- Sandboxed call function that pops the handler ------------------------ */
static void LuaUtilPCallSafe(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
    { LuaUtilPCallResultHandle(lS,
        LuaUtilPCallExSafe(lS, iParams, iReturns, iHandler));}
/* -- Sandboxed call function that doesn't pop the handler ----------------- */
static void LuaUtilPCall(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
    { LuaUtilPCallResultHandle(lS,
        LuaUtilPCallEx(lS, iParams, iReturns, iHandler)); }
/* -- If string is blank then return other string -------------------------- */
static void LuaUtilIfBlank(lua_State*const lS)
{ // Get replacement string first
  size_t stEmp;
  const char*const cpEmp = LuaUtilGetLStr<char>(lS, 1, stEmp, "StringIfEmpty");
  // If the second parameter doesn't exist then return the empty string
  if(lua_isnoneornil(lS, 2)) { LuaUtilPushLStr(lS, cpEmp, stEmp); return; }
  // Second parameter is valid, but return it if LUA says it is empty
  size_t stStr;
  const char*const cpStr =
    LuaUtilGetLStr<char>(lS, 2, stStr, "LuaUtilCheckStr");
  if(!stStr) { LuaUtilPushLStr(lS, cpEmp, stEmp); return; }
  // It isn't empty so return original string
  LuaUtilPushLStr(lS, cpStr, stStr);
}
/* -- Convert string string map to lua table and put it on stack ----------- */
static void LuaUtilToTable[[maybe_unused]](lua_State*const lS,
  const StrStrMap &ssmData)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushTable(lS, 0, ssmData.size());
  // For each table item
  for(const StrStrMapPair &ssmPair : ssmData)
  { // Push value and key name
    LuaUtilPushStr(lS, ssmPair.second);
    lua_setfield(lS, -2, ssmPair.first.c_str());
  }
}
/* -- Push the specified string at the specified index --------------------- */
static void LuaUtilSetTableIdxStr(lua_State*const lS,
  const int iTableId, const lua_Integer liIndex, const string &strValue)
{ // Push at the specified index, the specified string and set it to the table
  LuaUtilPushInt(lS, liIndex);
  LuaUtilPushStr(lS, strValue);
  lua_rawset(lS, iTableId);
}
/* -- Push the specified integer at the specified index -------------------- */
template<typename IntType>static void LuaUtilSetTableIdxInt(lua_State*const lS,
  const int iTableId, const lua_Integer liIndex, const IntType itValue)
{ // Push at the specified index, the specified value and set it to the table
  LuaUtilPushInt(lS, liIndex);
  LuaUtilPushInt(lS, static_cast<lua_Integer>(itValue));
  lua_rawset(lS, iTableId);
}
/* -- Convert a directory info object and put it on stack ------------------ */
static void LuaUtilToTable(lua_State*const lS, const DirEntMap &demList)
{ // Create the table, we're creating a indexed/value array
  LuaUtilPushTable(lS, demList.size());
  // Entry id
  lua_Integer liId = 0;
  // For each table item
  for(const DirEntMapPair &dempRef : demList)
  { // Push table index
    LuaUtilPushInt(lS, ++liId);
    // Create the sub for file info, we're creating a indexed/value array
    LuaUtilPushTable(lS, 6);
    // Push file parts
    LuaUtilSetTableIdxStr(lS, -3, 1, dempRef.first);               // File name
    LuaUtilSetTableIdxInt(lS, -3, 2, dempRef.second.Size());       // Size
    LuaUtilSetTableIdxInt(lS, -3, 3, dempRef.second.Created());    // Created
    LuaUtilSetTableIdxInt(lS, -3, 4, dempRef.second.Written());    // Updated
    LuaUtilSetTableIdxInt(lS, -3, 5, dempRef.second.Accessed());   // Accessed
    LuaUtilSetTableIdxInt(lS, -3, 6, dempRef.second.Attributes()); // Attrs
    // Push file data table
    lua_rawset(lS, -3);
  }
}
/* -- Convert string vector to lua table and put it on stack --------------- */
template<typename ListType>
  static void LuaUtilToTable(lua_State*const lS, const ListType &ltData)
{ // Create the table, we're creating a indexed/value array
  LuaUtilPushTable(lS, ltData.size());
  // Done if empty
  if(ltData.empty()) return;
  // Id number for array index
  lua_Integer iIndex = 0;
  // For each table item
  for(const string &vItem : ltData)
    LuaUtilSetTableIdxStr(lS, -3, ++iIndex, vItem);
}
/* -- Explode LUA string into table ---------------------------------------- */
static void LuaUtilExplode(lua_State*const lS)
{ // Check parameters
  size_t stStr, stSep;
  const char*const cpStr = LuaUtilGetLStr<char>(lS, 1, stStr, "String"),
            *const cpSep = LuaUtilGetLStr<char>(lS, 2, stSep, "Separator");
  // Create empty table if string invalid
  if(!stStr || !stSep) { LuaUtilPushTable(lS); return; }
  // Else convert whats in the string
  LuaUtilToTable(lS, Token({cpStr, stStr}, {cpSep, stSep}));
}
/* -- Explode LUA string into table ---------------------------------------- */
static void LuaUtilExplodeEx(lua_State*const lS)
{ // Check parameters
  size_t stStr, stSep;
  const char*const cpStr = LuaUtilGetLStr<char>(lS, 1, stStr, "String"),
            *const cpSep = LuaUtilGetLStr<char>(lS, 2, stSep, "Separator");
  const size_t stMax = LuaUtilGetInt<size_t>(lS, 3, "Maximum");
  // Create empty table if string invalid
  if(!stStr || !stSep || !stMax) { LuaUtilPushTable(lS); return; }
  // Else convert whats in the string
  LuaUtilToTable(lS, Token({cpStr, stStr}, {cpSep, stSep}, stMax));
}
/* -- Process initial implosion a table ------------------------------------ */
static lua_Integer LuaUtilImplodePrepare(lua_State*const lS,
  const int iMaxParams)
{ // Must have this many parameters
  LuaUtilCheckParams(lS, iMaxParams);
  // Check table and get its size
  LuaUtilCheckTable(lS, 1, "StringTable");
  // Get size of table clamped since lua_rawlen returns unsigned and the
  // lua_rawgeti parameter is signed. Compare the result...
  switch(const lua_Integer liLen =
    UtilIntOrMax<lua_Integer>(LuaUtilGetSize(lS, 1)))
  { // No entries? Just check the separator for consistency and push blank
    case 0: LuaUtilCheckStr(lS, 2, "Separator");
            LuaUtilPushStr(lS, cCommon->Blank());
            break;
    // One entry? Just check the separator and push the first item
    case 1: LuaUtilCheckStr(lS, 2, "Separator");
            LuaUtilGetRefEx(lS);
            break;
    // More than one entry? Caller must process this;
    default: return liLen;
  } // We handled it
  return 0;
}
/* -- Pushes an item from the specified table onto the stack --------------- */
static void LuaUtilImplodeItem(lua_State*const lS, const int iParam,
  const lua_Integer liIndex, string &strOutput, const char *cpStr,
  size_t stStr)
{ // Add separator to string
  strOutput.append(cpStr, stStr);
  // Get item from table
  LuaUtilGetRefEx(lS, 1, liIndex);
  // Get the string from Lua stack and save the length
  cpStr = lua_tolstring(lS, iParam, &stStr);
  // Append to supplied string
  strOutput.append(cpStr, stStr);
  // Remove item from stack
  LuaUtilRmStack(lS);
}
/* -- Implode LUA table into string ---------------------------------------- */
static void LuaUtilImplode(lua_State*const lS)
{ // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Integer liLen = LuaUtilImplodePrepare(lS, 2))
  { // Get separator
    size_t stSep;
    const char*const cpSep = LuaUtilGetLStr<char>(lS, 2, stSep, "Separator");
    // Write first item
    LuaUtilGetRefEx(lS);
    string strOutput{ LuaUtilToCppString(lS) };
    LuaUtilRmStack(lS);
    // Iterate through rest of table and implode the items
    for(lua_Integer liI = 2; liI <= liLen; ++liI)
      LuaUtilImplodeItem(lS, 3, liI, strOutput, cpSep, stSep);
    // Return string
    LuaUtilPushStr(lS, strOutput);
  }
}
/* -- Implode LUA table into human readable string ------------------------- */
static void LuaUtilImplodeEx(lua_State*const lS)
{ // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Integer liLen = LuaUtilImplodePrepare(lS, 3))
  { // Get and check separators
    size_t stSep, stSep2;
    const char
      *const cpSep = LuaUtilGetLStr<char>(lS, 2, stSep, "Separator"),
      *const cpSep2 = LuaUtilGetLStr<char>(lS, 3, stSep2, "LastSeparator");
    // Write first item
    LuaUtilGetRefEx(lS);
    string strOutput{ LuaUtilToCppString(lS) };
    LuaUtilRmStack(lS);
    // Iterator through rest of table except for last entry
    for(lua_Integer liI = 2; liI < liLen; ++liI)
      LuaUtilImplodeItem(lS, 4, liI, strOutput, cpSep, stSep);
    // If there was more than one item? StrImplode the last item
    if(liLen > 1) LuaUtilImplodeItem(lS, 4, liLen, strOutput, cpSep2, stSep2);
    // Return string
    LuaUtilPushStr(lS, strOutput);
  }
}
/* -- Enumerate number of items in a table (non-indexed) ------------------- */
static lua_Unsigned LuaUtilGetKeyValTableSize(lua_State*const lS)
{ // Check that we have a table of strings
  LuaUtilCheckTable(lS, 1, "KeyValueTable");
  // Number of indexed items in table
  const lua_Unsigned uiIndexedCount = LuaUtilGetSize(lS, 1);
  // Number of items in table
  lua_Unsigned uiCount = 0;
  // Until there are no more items
  for(LuaUtilPushNil(lS); lua_next(lS, -2); LuaUtilRmStack(lS)) ++uiCount;
  // Remove key
  LuaUtilRmStack(lS);
  // Return count of key/value pairs in table
  return uiCount - uiIndexedCount;
}
/* -- Replace text with values from specified LUA table -------------------- */
static void LuaUtilReplaceMulti(lua_State*const lS)
{ // Get string to replace
  string strDest{ LuaUtilGetCppStr(lS, 1, "String") };
  // Check that we have a table of strings
  LuaUtilCheckTable(lS, 2, "Data");
  // Source string is empty or there are indexed items in the table? Remove
  // table and return original blank string
  if(strDest.empty() || LuaUtilGetSize(lS, 2)) return LuaUtilRmStack(lS);
  // Build table
  StrPairList lList;
  // Until there are no more items, add value if key is a string
  for(LuaUtilPushNil(lS); lua_next(lS, -2); LuaUtilRmStack(lS))
    if(lua_isstring(lS, -1))
      lList.push_back({ LuaUtilToCppString(lS, -2), LuaUtilToCppString(lS) });
  // Return original string if nothing added
  if(lList.empty()) return LuaUtilRmStack(lS);
  // Remove table and string parameter
  lua_pop(lS, 2);
  // Execute replacements and return newly made string
  LuaUtilPushStr(lS, StrReplaceEx(strDest, lList));
}
/* -- Convert string/uint map to table ------------------------------------- */
static void LuaUtilToTable(lua_State*const lS, const StrUIntMap &suimRef)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushTable(lS, 0, suimRef.size());
  // For each table item
  for(const StrUIntMapPair &suimpRef : suimRef)
  { // Push value and key name
    LuaUtilPushInt(lS, suimpRef.second);
    lua_setfield(lS, -2, suimpRef.first.c_str());
  }
}
/* -- Convert varlist to lua table and put it on stack --------------------- */
static void LuaUtilToTable(lua_State*const lS, const StrNCStrMap &sncsmMap)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushTable(lS, 0, sncsmMap.size());
  // For each table item
  for(const StrNCStrMapPair &sncsmpPair : sncsmMap)
  { // Push value and key name
    LuaUtilPushStr(lS, sncsmpPair.second);
    lua_setfield(lS, -2, sncsmpPair.first.c_str());
  }
}
/* -- Initialise lua and clib random number generators --------------------- */
static void LuaUtilInitRNGSeed(lua_State*const lS, const lua_Integer liSeed)
{ // Push a new random seed into C-Lib too
  StdSRand(static_cast<unsigned int>(liSeed));
  // Get 'math' table
  lua_getglobal(lS, "math");
  // Get pointer to function
  lua_getfield(lS, -1, "randomseed");
  // Push a random seed
  LuaUtilPushInt(lS, liSeed);
  // Calls randomseed(uqSeed)
  LuaUtilCallFuncEx(lS, 1);
  // Removes the table 'math'
  LuaUtilRmStack(lS);
}
/* -- Return true if lua stack can take specified more items --------------- */
static bool LuaUtilIsStackAvail(lua_State*const lS, const int iCount)
  { return lua_checkstack(lS, iCount); }
/* -- Return true if lua stack can take specified more items (diff type) --- */
template<typename IntType>
  static bool LuaUtilIsStackAvail(lua_State*const lS, const IntType itCount)
    { return UtilIntWillOverflow<int>(itCount) ? false :
        LuaUtilIsStackAvail(lS, static_cast<int>(itCount)); }
/* -- Simple class to save and restore stack ------------------------------- */
class LuaStackSaver                    // Lua stack saver class
{ /* -- Private variables -------------------------------------------------- */
  const int        iTop;               // Current stack position
  lua_State*const  lState;             // State to use
  /* -- Return stack position -------------------------------------- */ public:
  int Value(void) const { return iTop; }
  /* -- Restore stack position --------------------------------------------- */
  void Restore(void) const { LuaUtilPruneStack(lState, Value()); }
  /* -- Constructor -------------------------------------------------------- */
  explicit LuaStackSaver(lua_State*const lS) :
    iTop(LuaUtilStackSize(lS)), lState(lS) { }
  /* -- Destructor --------------------------------------------------------- */
  ~LuaStackSaver(void) { Restore(); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
