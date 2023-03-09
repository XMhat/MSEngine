/* == LUAUTIL.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Lua utility functions. They normally need a state to work.          ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfLuaUtil {                  // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfMemory;              // Using memory interface
using namespace IfDir;                 // Using dir interface
/* -- Variables ------------------------------------------------------------ */
static bool bLuaPaused = false;        // Is Lua paused or not?
/* -- Utility type defs ---------------------------------------------------- */
struct LuaClass { void *vpPtr; };      // Holds a pointer to a class
/* -- Prune stack ---------------------------------------------------------- */
void PruneStack(lua_State*const lS, const int iTot) { lua_settop(lS, iTot); }
/* -- Return items in stack ------------------------------------------------ */
int GetStackCount(lua_State*const lS) { return lua_gettop(lS); }
/* -- Return if stack is empty --------------------------------------------- */
bool IsStackEmpty(lua_State*const lS) { return GetStackCount(lS) == 0; }
/* -- Simple class to save and restore stack ------------------------------- */
class LuaStackSaver                    // Lua stack saver class
{ /* -- Private variables ----------------------------------------- */ private:
  const int        iTop;               // Current stack position
  lua_State*const  lsState;            // State to use
  /* -- Return stack position -------------------------------------- */ public:
  int Value(void) const { return iTop; }
  /* -- Restore stack position --------------------------------------------- */
  void Restore(void) const { PruneStack(lsState, Value()); }
  /* -- Constructor -------------------------------------------------------- */
  explicit LuaStackSaver(lua_State*const lsS) :
    iTop(GetStackCount(lsS)), lsState(lsS) { }
  /* -- Destructor --------------------------------------------------------- */
  ~LuaStackSaver(void) { Restore(); }
  /* ----------------------------------------------------------------------- */
};
/* -- Get length of a table ------------------------------------------------ */
static lua_Unsigned LuaUtilGetSize(lua_State*const lS, const int iParam)
  { return lua_rawlen(lS, iParam); }
/* -- Get and return a C++ string without checking it ---------------------- */
static const string ToCppString(lua_State*const lS, const int iId=-1)
{ // Storage for string length. Do not optimise this because I am not sure
  // what the standard direction is for evaluating expression. Left-to-right
  // or right-to-left, so I will just store the string point first to be safe.
  size_t stLength;
  const char*const cpStr = lua_tolstring(lS, iId, &stLength);
  return { cpStr, stLength };
}
/* -- Return type of item in stack ----------------------------------------- */
static const string GetStackType(lua_State*const lS, const int iIndex)
{ // What type of variable?
  switch(const int iI = lua_type(lS, iIndex))
  { // Nil?
    case LUA_TNIL: return "nil"; break;
    // A number?
    case LUA_TNUMBER:
    { // If not actually an integer? Write as normal floating-point number
      if(!lua_isinteger(lS, iIndex)) return ToString(lua_tonumber(lS, iIndex));
      // Get actual integer value and return it and it's hex value
      const lua_Integer liValue = lua_tointeger(lS, iIndex);
      return Format("$ [0x$$]", liValue, hex, liValue);
    } // A boolean?
    case LUA_TBOOLEAN: return TrueOrFalse(lua_toboolean(lS, iIndex));
    // A string?
    case LUA_TSTRING:
    { // Get value of string and return value with size
      const string strVal{ ToCppString(lS, iIndex) };
      return Format("[$] \"$\"", strVal.length(), strVal);
    } // A table?
    case LUA_TTABLE: return Format("<table:$>[$]",
      lua_topointer(lS, iIndex), LuaUtilGetSize(lS, iIndex));
    // Who knows? Function? Userdata?
    default: return Format("<$:$>",
      lua_typename(lS, iI), lua_topointer(lS, iIndex));
  }
}
/* -- Return status of item in stack --------------------------------------- */
static const string GetStackTokens(lua_State*const lS, const int iIndex)
{ // Fill token buffer depending on status
  return EvaluateTokens({
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
static const string GetVarStack(lua_State*const lS)
{ // If there are variables in the stack?
  if(const int iCount = GetStackCount(lS))
  { // String to return
    ostringstream osS;
     // For each element (1 is the first item)
    for(int iIndex = 1; iIndex <= iCount; ++iIndex)
      osS << iIndex
          << "["
          << iIndex - iCount - 1
          << "] (" << GetStackTokens(lS, iIndex) << ") "
          << GetStackType(lS, iIndex)
          << "\n";
    // Return string
    return osS.str();
  } // No elements in variable stack
  return "<empty stack>";
}
/* -- Set hook ------------------------------------------------------------- */
static void SetHookCallback(lua_State*const lS, lua_Hook fcbCb, const int iC)
  { lua_sethook(lS, fcbCb, LUA_MASKCOUNT, iC); }
/* -- Push a table onto the stack ------------------------------------------ */
template<typename IdxIntType=int, typename KeyIntType=int>
  static void PushTable(lua_State*const lS, const IdxIntType iitIndexes=0,
    const KeyIntType kitKeys=0)
      { lua_createtable(lS, IntOrMax<int>(iitIndexes),
                            IntOrMax<int>(kitKeys)); }
/* -- Push a nil onto the stack -------------------------------------------- */
static void PushNil(lua_State*const lS) { lua_pushnil(lS); }
/* -- Push a boolean onto the stack ---------------------------------------- */
template<typename IntType>
  static void PushBoolean(lua_State*const lS, const IntType itValue)
    { lua_pushboolean(lS, static_cast<bool>(itValue)); }
/* -- Push a number onto the stack ----------------------------------------- */
template<typename IntType>
  static void PushNumber(lua_State*const lS, const IntType itValue)
    { lua_pushnumber(lS, static_cast<lua_Number>(itValue)); }
/* -- Push an integer onto the stack --------------------------------------- */
template<typename IntType>
  static void PushInteger(lua_State*const lS, const IntType itValue)
    { lua_pushinteger(lS, static_cast<lua_Integer>(itValue)); }
/* -- Push a string onto the stack ----------------------------------------- */
template<typename PtrType>
  static void PushString(lua_State*const lS, const PtrType ptValue)
    { lua_pushstring(lS, reinterpret_cast<const char*>(ptValue)); }
/* -- Push a literal string onto the stack --------------------------------- */
template<typename PtrType, typename IntType>
  static void PushLString(lua_State*const lS, const PtrType ptValue,
    const IntType itSize)
    { lua_pushlstring(lS, reinterpret_cast<const char*>(ptValue),
                          static_cast<size_t>(itSize)); }
/* -- Push a memory block onto the stack as a string ----------------------- */
static void PushDataBlock(lua_State*const lS, const DataConst &dcItem)
  { PushLString(lS, dcItem.Ptr<char>(), dcItem.Size()); }
/* -- Push a C++ string onto the stack ------------------------------------- */
static void PushCppString(lua_State*const lS, const string &strStr)
  { PushLString(lS, strStr.data(), strStr.length()); }
/* -- Throw error and add impossible abort to shut the compiler up --------- */
static void ThrowError(lua_State*const lS) { lua_error(lS); }
/* -- Push C-String on stack and throw ------------------------------------- */
static void ProcessError(lua_State*const lS,
  const char*const cpReason)
    { PushString(lS, cpReason); ThrowError(lS); }
/* == Get human readable name of specified type id ========================= */
static const char *GetType(lua_State*const lS, const int iIndex)
  { return lua_typename(lS, lua_type(lS, iIndex)); }
/* -- Remove item from stack ----------------------------------------------- */
static void RemoveStack(lua_State*const lS, const int iP=-1)
  { lua_remove(lS, iP); }
/* -- Get and pop string on top -------------------------------------------- */
static const string GetAndPopString(lua_State*const lS)
{ // If there is nothing on the stack then return a generic error
  if(lua_isnone(lS, -1)) return "Error signalled with no reason";
  // Not have a string on stack? Set embedded error!
  if(!lua_isstring(lS, -1))
    return Format("Error signalled with invalid '$' reason", GetType(lS, -1));
  // Get error string
  const string strError{ ToCppString(lS) };
  // Remove the error string
  RemoveStack(lS);
  // return the error
  return strError;
}
/* -- Return reference ----------------------------------------------------- */
static void GetReferenceEx(lua_State*const lS, const int iTable=1,
  const lua_Integer liIndex=1) { lua_rawgeti(lS, iTable, liIndex); }
/* -- Return reference ----------------------------------------------------- */
static void GetReference(lua_State*const lS, const int iParam)
  { GetReferenceEx(lS, LUA_REGISTRYINDEX, static_cast<lua_Integer>(iParam)); }
/* -- Return referenced function ------------------------------------------- */
static bool GetReferencedFunction(lua_State*const lS, const int iParam)
{ // If context and reference are valid?
  if(lS && iParam != LUA_REFNIL)
  { // Push the userdata onto the stack and return success if successful
    GetReference(lS, iParam);
    if(lua_isfunction(lS, -1)) return true;
    // Failed so remove whatever it was
    RemoveStack(lS);
  } // Failure
  return false;
}
/* -- Return referenced userdata ------------------------------------------- */
static bool GetReferencedUserdata(lua_State*const lS, const int iParam)
{ // If context and reference are valid?
  if(lS && iParam != LUA_REFNIL)
  { // Push the userdata onto the stack and return success if successful
    GetReference(lS, iParam);
    if(lua_isuserdata(lS, -1)) return true;
    // Failed so remove whatever it was
    RemoveStack(lS);
  } // Failure
  return false;
}
/* -- Remove reference to hidden variable without checking ----------------- */
static void DeleteReferenceUnsafe(lua_State*const lS, const int iRef)
  { luaL_unref(lS, LUA_REGISTRYINDEX, iRef); }
/* -- Remove reference to hidden variable ---------------------------------- */
static bool DeleteReference(lua_State*const lS, const int iRef)
{ // Return if no state or reference
  if(!lS || iRef == LUA_REFNIL) return false;
  // Unreference
  DeleteReferenceUnsafe(lS, iRef);
  // Success
  return true;
}
/* -- Safely delete a reference -------------------------------------------- */
static bool SafeDeleteReference(lua_State*const lS,
  int &iRef, const int iSRef=LUA_REFNIL)
{ // Remove state and return failed if failed
  const bool bReturn = DeleteReference(lS, iRef);
  // Reset the reference
  iRef = iSRef;
  // Success
  return bReturn;
}
/* -- Get a new reference without checking --------------------------------- */
static int InitReferenceUnsafe(lua_State*const lS)
  { return luaL_ref(lS, LUA_REGISTRYINDEX); }
/* -- Get a new reference -------------------------------------------------- */
static int InitReference(lua_State*const lS)
{ // Return if no state or nothing on the stack
  if(!lS || IsStackEmpty(lS)) return LUA_REFNIL;
  // Return result
  return InitReferenceUnsafe(lS);
}
/* -- Set a new reference -------------------------------------------------- */
static bool InitReference(lua_State*const lS, int &iRef)
{ // Do the reference
  iRef = InitReference(lS);
  // Done
  return iRef != LUA_REFNIL;
}
/* ------------------------------------------------------------------------- */
static const string GetStack(lua_State*const lST)
{ // We need the root state so we can iterate through all the threads and will
  // eventually arrive at *lS as the last stack. Most of the time GetState()
  // equals to *lS anyway, just depends if it triggered in a co-routine or not.
  GetReference(lST, LUA_RIDX_MAINTHREAD);
  lua_State *lS = lua_tothread(lST, -1);
  RemoveStack(lST);
  // Return if state is invalid. Impossible really but just incase.
  if(!lS) return "\n- Could not find main thread!";
  // Some typedefs that we'll use
  typedef list<lua_Debug> LuaStack;
  typedef LuaStack::reverse_iterator LuaStackRevIt;
  // list of stack traces for coroutines. They are ordered from most recent
  // call to the root call so we need to use this list to reverse them after.
  // Also we (or even Lua) does know how many total calls there has been, we
  // can only enumerate them.
  LuaStack lStack;
  // Co-routine id so user knows which coroutine sub-level they were at.
  int iCoId = 0;
  // Loop until we've enumerated all the upstates
  do
  { // list of stack traces for this coroutine
    LuaStack lThread;
    // For each stack
    for(int iId = 0; ; ++iId)
    { // Lua debug info container
      lua_Debug ldData;
      // Read stack data
      if(!lua_getstack(lS, iId, &ldData)) break;
      // Set co-routine id. We're not using this 'event' var and neither does
      // LUA in lua_getinfo() according to ldebug.c.
      ldData.event = iCoId;
      // Insert into list
      lThread.emplace_front(move(ldData));
    } // Move into lStack in reverse order
    lStack.splice(lStack.cend(), lThread);
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
  size_t stId = lStack.size();
  // For each stack trace
  for(LuaStackRevIt rIt{ lStack.rbegin() }; rIt != lStack.rend(); ++rIt)
  { // Get thread data
    lua_Debug &ldData = *rIt;
    // Get info and if succeeded and there is some basic information to show?
    if(lua_getinfo(lS, "Slnu", &ldData) &&
      (ldData.currentline != -1 || ldData.name))
    { // Prepare start of stack trace
      osS << "\n- " << --stId << ':' << ldData.event << " = "
          << ldData.short_src;
      // We have line data? Append data to string
      if(ldData.currentline != -1)
        osS << " @ " << ldData.currentline << '['
            << ldData.linedefined << '-' << ldData.lastlinedefined << ']';
      // Write rest of data
      osS << " : " << (ldData.name ? ldData.name : "?") << '('
          << (*ldData.namewhat ? ldData.namewhat : "?") << ';'
          << static_cast<unsigned int>(ldData.nparams) << ';'
          << static_cast<unsigned int>(ldData.nups) << ')';
    }
  } // Return formatted stack string
  return osS.str();
}
/* -- Generic panic handler ------------------------------------------------ */
static int GenericPanic(lua_State*const lS)
{ // Get error message and stack. Don't one line this because the order of
  // execution is important!
  const string strError{ GetAndPopString(lS) };
  // Throw exception
  XC(Append(strError, GetStack(lS)));
}
/* -- Generic error handler ------------------------------------------------ */
static int GenericError(lua_State*const lS)
{ // Get error message and stack. Don't one line this because the order of
  // execution is important!
  const string strError{ GetAndPopString(lS) };
  // Push error message onto stack
  PushCppString(lS, Append(strError, GetStack(lS)));
  // Returning error string
  return 1;
}
/* -- Push a function onto the stack --------------------------------------- */
static void PushCFunction(lua_State*const lS, lua_CFunction cFunc,
  const int iNVals=0)
    { lua_pushcclosure(lS, cFunc, iNVals); }
/* -- Push the above generic error function and return its id -------------- */
static int PushAndGetGenericErrorId(lua_State*const lS)
{ // Push error callback and store the index it is stored at.
  PushCFunction(lS, GenericError);
  return GetStackCount(lS);
}
/* == Generate an exception if the specified condition is false ============ */
static void Assert(lua_State*const lS, const bool bCond, const int iIndex,
  const char*const cpName, const char*const cpType)
{ // Ignore if condition is true
  if(bCond) return;
  // Throw error message
  XC("Invalid parameter!",
    "Name",     cpName, "Parameter", iIndex,
    "Required", cpType, "Supplied",  GetType(lS, iIndex));
}
/* -- Check that parameter is a table -------------------------------------- */
static void CheckTable(lua_State*const lS, const int iId,
  const char*const cpName)
    { Assert(lS, !!lua_istable(lS, iId), iId, cpName, "table"); }
/* -- Check that parameter is a string ------------------------------------- */
static void CheckString(lua_State*const lS, const int iId,
  const char*const cpName)
    { Assert(lS, !!lua_isstring(lS, iId), iId, cpName, "string"); }
/* -- Check that parameter is a string and is not empty -------------------- */
static void CheckStringNE(lua_State*const lS, const int iId,
  const char*const cpName)
{ // Check the string is valid
  CheckString(lS, iId, cpName);
  // Return if the string is not empty
  if(lua_rawlen(lS, iId) > 0) return;
  // Throw error message
  XC("Non-empty string required!", "Name", cpName, "Parameter", iId);
}
/* -- Get the specified string from the stack ------------------------------ */
template<typename StrType>static const StrType *GetString(lua_State*const lS,
  const int iId, const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  CheckString(lS, iId, cpName);
  // Return the number
  return reinterpret_cast<const StrType*>(lua_tostring(lS, iId));
}
/* -- Get the specified string from the stack ------------------------------ */
template<typename StrType>static const StrType *GetStringNE(lua_State*const lS,
  const int iId, const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  CheckStringNE(lS, iId, cpName);
  // Return the number
  return reinterpret_cast<const StrType*>(lua_tostring(lS, iId));
}
/* -- Get and return a string and throw exception if not a string ---------- */
template<typename StrType>static const StrType *GetLString(lua_State*const lS,
  const int iId, size_t &stLen, const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  CheckString(lS, iId, cpName);
  // Return the number
  return reinterpret_cast<const StrType*>(lua_tolstring(lS, iId, &stLen));
}
/* -- Helper for GetLString that makes a memory block --------------------- */
static Memory GetMBfromLString(lua_State*const lS, const int iId,
  const char*const cpName)
{ // Storage for size
  size_t stStrLen;
  // Get string and store size in string.
  const char*const cpStr = GetLString<char>(lS, iId, stStrLen, cpName);
  // Return as memory block
  return { stStrLen, cpStr };
}
/* -- Get and return a C++ string and throw exception if not a string ------ */
static const string GetCppString(lua_State*const lS, const int iId,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  CheckString(lS, iId, cpName);
  // Return the constructed string
  return ToCppString(lS, iId);
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
static const string GetCppStringNE(lua_State*const lS, const int iId,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  CheckStringNE(lS, iId, cpName);
  // Return the constructed string
  return ToCppString(lS, iId);
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
static const string GetCppFileName(lua_State*const lS, const int iId,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  CheckString(lS, iId, cpName);
  // Get the filename
  const string strFileName{ ToCppString(lS, iId) };
  // Verify that the filename is valid
  if(const ValidResult vrId = DirValidName(strFileName))
    XC("Invalid parameter!",
       "Param",    iId,
       "File",     strFileName,
       "Reason",   DirValidNameResultToString(vrId),
       "ReasonId", vrId);
  // Return the constructed string
  return strFileName;
}
/* -- Get and return a C++ string and throw exception if not a string ------ */
static const string GetCppStringUpper(lua_State*const lS, const int iId,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  string strStr{ GetCppString(lS, iId, cpName) };
  // Return the constructed string converting it to upper case
  return ToUpperRef(strStr);
}
/* -- Check the specified number of parameters are set --------------------- */
static void CheckParams(lua_State*const lS, const int iCount)
{ // Get number of parameters supplied and we're ok if the count is correct
  const int iTop = GetStackCount(lS);
  if(iCount == iTop) return;
  // Push error message
  XC((iCount < iTop) ? "Too many arguments!" : "Not enough arguments!",
    "Supplied", iTop, "Required", iCount);
}
/* -- Check the specified function is valid -------------------------------- */
static void CheckFunction(lua_State*const lS, const int iIndex,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid string
  Assert(lS, !!lua_isfunction(lS, iIndex), iIndex, cpName, "function");
}
/* -- Get and return a boolean and throw exception if not a boolean -------- */
static bool GetBool(lua_State*const lS, const int iIndex,
  const char*const cpName)
{ // Test to make sure if supplied parameter is a valid boolean
  Assert(lS, !!lua_isboolean(lS, iIndex), iIndex, cpName, "boolean");
  // Return the number
  return lua_toboolean(lS, iIndex) == 1;
}
/* -- Get and return a number (double) and throw exception if not a number - */
template<typename IntType>
  static IntType GetNum(lua_State*const lS, const int iIndex,
    const char*const cpName)
{ // Test to make sure if supplied parameter is a valid number (float)
  Assert(lS, !!lua_isnumber(lS, iIndex), iIndex, cpName, "number");
  // Return the number
  return static_cast<IntType>(lua_tonumber(lS, iIndex));
}
/* -- Get and return a number, throw if out of range or not a number ------- */
template<typename IntType>
  static IntType GetNumLG(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Throw if value not a number and return if it is in the valid range
  const IntType itVal = GetNum<IntType>(lS, iIndex, cpName);
  if(itVal >= itMin && itVal <= itMax) return itVal;
  // Throw error
  XC("Number out of range!",
    "Parameter", iIndex, "Name",       cpName, "Supplied", itVal,
    "NotLesser", itMin,  "NotGreater", itMax);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static IntType GetNumLGE(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Throw if value not a number and return if it is in the valid range
  const IntType itVal = GetNum<IntType>(lS, iIndex, cpName);
  if(itVal >= itMin && itVal < itMax) return itVal;
  // Throw error
  XC("Number out of range!",
    "Parameter", iIndex, "Name",            cpName, "Supplied", itVal,
    "NotLesser", itMin,  "NotGreaterEqual", itMax);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static IntType GetInt(lua_State*const lS, const int iIndex,
    const char*const cpName)
{ // Test to make sure if supplied parameter is a valid integer (not float)
  Assert(lS, !!lua_isinteger(lS, iIndex), iIndex, cpName, "integer");
  // Return the number
  return static_cast<IntType>(lua_tointeger(lS, iIndex));
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static IntType GetIntLG(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Throw if value not a integer and return if it is in the valid range
  const IntType itVal = GetInt<IntType>(lS, iIndex, cpName);
  if(itVal >= itMin && itVal <= itMax) return itVal;
  // Throw error
  XC("Integer out of range!",
    "Parameter", iIndex, "Name",            cpName, "Supplied", itVal,
    "NotLesser", itMin,  "NotGreaterEqual", itMax);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static IntType GetIntLGE(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax, const char*const cpName)
{ // Throw if value not a integer and return if it is in the valid range
  const IntType itVal = GetInt<IntType>(lS, iIndex, cpName);
  if(itVal >= itMin && itVal < itMax) return itVal;
  // Throw error
  XC("Integer out of range!",
    "Parameter", iIndex, "Name",       cpName, "Supplied", itVal,
    "NotLesser", itMin,  "NotGreater", itMax);
}
/* ------------------------------------------------------------------------- */
template<class FlagsType>
  static const FlagsType GetFlags(lua_State*const lS, const int iIndex,
    const FlagsType &fnMin, const FlagsType &fnMax, const char*const cpName)
{ // Throw if value not a integer and return if it is in the valid range
  const FlagsType fnVal{
    GetInt<decltype(fnMin.FlagGet())>(lS, iIndex, cpName) };
  if(fnVal.FlagIsGreaterEqual(fnMin) && fnVal.FlagIsLesser(fnMax))
    return fnVal;
  // Throw error
  XC("Flags out of range!",
    "Parameter",       iIndex,        "Name",      cpName,
    "Supplied",        fnVal.FlagGet(), "NotLesser", fnMin.FlagGet(),
    "NotGreaterEqual", fnMax.FlagGet());
}
/* ========================================================================= */
template<typename T>
  static void ClassDestroy(lua_State*const lS, const int iP,
    const char*const cpT)
{ // Get userdata pointer from Lua and if the address is valid?
  if(LuaClass*const lC =
    reinterpret_cast<LuaClass*>(luaL_checkudata(lS, iP, cpT)))
  { // Get address to the C++ class and if that is valid?
    if(T*const tcC = reinterpret_cast<T*>(lC->vpPtr))
    { // Clear the pointer to the C++ class
      lC->vpPtr = nullptr;
      // Destroy the C++ class if it is not internally locked
      if(tcC->LockIsNotSet()) delete tcC;
    }
  }
}
/* == Creates a new item for object ======================================== */
static LuaClass *PrepareNewClass(lua_State*const lS, const char*const cpType)
{ // Create userdata
  LuaClass*const lcD =
    reinterpret_cast<LuaClass*>(lua_newuserdata(lS, sizeof(LuaClass)));
  // Initialise meta data and if it hasn't been initialised yet?
  if(luaL_getmetatable(lS, cpType) == LUA_TNIL)
    XC("Metatable not available!", "Type", cpType);
  // Get and push garbage collector function. This is set from the
  // SetupEnvironment() function in lua.hpp.
  PushString(lS, "__dtor");
  lua_rawget(lS, -2);
  if(!lua_iscfunction(lS, -1))
    XC("Metatable destructor invalid!", "Type", cpType);
  lua_setfield(lS, -2, "__gc");
  // Done setting metamethods, set the table
  lua_setmetatable(lS, -2);
  // Return pointer to new class allocated by Lua
  return lcD;
}
/* == Takes ownership of an object ========================================= */
template<class ClassType>
  static ClassType *ClassReuse(lua_State*const lS, const char*const cpType,
    ClassType*tPtr)
{ // Prepare a new object
  LuaClass*const lcD = PrepareNewClass(lS, cpType);
  // Assign object to lua so lua will be incharge of deleting it
  lcD->vpPtr = tPtr;
  // Return pointer to new class allocated elseware
  return tPtr;
}
/* == Creates and allocates a pointer to a new class ======================= */
template<typename ClassType>
  static ClassType *ClassCreate(lua_State*const lS, const char*const cpType)
{ // Prepare a new object
  LuaClass*const lcD = PrepareNewClass(lS, cpType);
  // Allocate class and return it if succeeded
  if(void*const vpPtr = lcD->vpPtr = new (nothrow)ClassType)
    return reinterpret_cast<ClassType*>(vpPtr);
  // Error occured so just throw it
  XC("Failed to allocate class!", "Type", cpType, "Size", sizeof(ClassType));
}
/* == Creates a pointer to a class that LUA CAN'T deallocate =============== */
template<typename ClassType>
  static ClassType *ClassCreatePtr(lua_State*const lS, const char*const cpType,
    ClassType*const tPtr)
{ // Create userdata
  LuaClass*const lcD =
    reinterpret_cast<LuaClass*>(lua_newuserdata(lS, sizeof(LuaClass)));
  // Initialise meta data and if it hasn't been initialised yet?
  if(luaL_getmetatable(lS, cpType) == LUA_TNIL)
    XC("Metadata not available!", "Type", cpType);
  // Done setting methods, set the table
  lua_setmetatable(lS, -2);
  // Set pointer to class
  lcD->vpPtr = reinterpret_cast<void*>(tPtr);
  // Return pointer to memory
  return tPtr;
}
/* -- Gets a pointer to any class ------------------------------------------ */
template<typename ClassType>
  ClassType *GetPtr(lua_State*const lS, const int iP, const char*const cpT)
{ // Get lua data class and if it is valid
  if(const LuaClass*const lcD =
    reinterpret_cast<LuaClass*>(luaL_checkudata(lS, iP, cpT)))
  { // Get reference to class and return pointer if valid
    const LuaClass &lcR = *lcD;
    if(lcR.vpPtr) return reinterpret_cast<ClassType*>(lcR.vpPtr);
    // Actual class pointer has already been freed so error occured
    XC("Unallocated class parameter!", "Parameter", iP, "Type", cpT);
  } // lua data class not valid
  XC("Null class parameter!", "Parameter", iP, "Type", cpT);
}
/* -- Garbage collection control ------------------------------------------- */
static int GCSet(lua_State*const lS, const int iCmd, const int iVal1,
  const int iVal2)
    { return lua_gc(lS, iCmd, iVal1, iVal2); }
//static int GCSet(lua_State*const lS, const int iCmd, const int iVal)
//  { return lua_gc(lS, iCmd, iVal); }
static int GCSet(lua_State*const lS, const int iCmd)
  { return lua_gc(lS, iCmd); }
static int GCStop(lua_State*const lS)
  { return GCSet(lS, LUA_GCSTOP); }
static int GCStart(lua_State*const lS)
  { return GCSet(lS, LUA_GCRESTART); }
static int GCRun(lua_State*const lS)
  { return GCSet(lS, LUA_GCCOLLECT); }
static bool GCRunning(lua_State*const lS)
  { return !!GCSet(lS, LUA_GCISRUNNING); }
/* -- Get memory usage ----------------------------------------------------- */
static size_t GetLuaUsage(lua_State*const lS)
  { return static_cast<size_t>
      (GCSet(lS, LUA_GCCOUNT) + GCSet(lS, LUA_GCCOUNTB) / 1024) * 1024; }
/* -- Full garbage collection while logging memory usage ------------------- */
static size_t GCCollect(lua_State*const lS)
{ // Get current usage
  const size_t stUsage = GetLuaUsage(lS);
  // Do full garbage collect
  GCRun(lS);
  // Return old minus new memory usage
  return stUsage - GetLuaUsage(lS);
}
/* -- Standard in-sandbox call function (unmanaged) ------------------------ */
static void CallFuncEx(lua_State*const lS, const int iParams=0,
  const int iReturns=0)
    { lua_call(lS, iParams, iReturns); }
/* -- Standard in-sandbox call function (unmanaged, no params) ------------- */
static void CallFunc(lua_State*const lS, const int iReturns=0)
  { CallFuncEx(lS, 0, iReturns); }
/* -- Sandboxed call function (doesn't remove error handler) --------------- */
static int PCallEx(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
    { return lua_pcall(lS, iParams, iReturns, iHandler); }
/* -- Sandboxed call function (removes error handler) ---------------------- */
static int PCallExSafe(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
{ // Do protected call and get result
  const int iR = PCallEx(lS, iParams, iReturns, iHandler);
  // Remove error handler from stack if handler specified
  if(iHandler) RemoveStack(lS, iHandler);
  // Return result
  return iR;
}
/* -- Handle PCall result -------------------------------------------------- */
static void PCallResultHandle(lua_State*const lS, const int iResult)
{ // Compare error code
  switch(iResult)
  { // No error
    case 0: return;
    // Run-time error
    case LUA_ERRRUN: XC(Append("Runtime error! > ", GetAndPopString(lS)));
    // Memory allocation error
    case LUA_ERRMEM: XC("Memory allocation error!", "Usage", GetLuaUsage(lS));
    // Error + error in error handler
    case LUA_ERRERR: XC("Error in error handler!");
    // Unknown error
    default: XC("Unknown error!");
  }
}
/* -- Sandboxed call function that pops the handler ------------------------ */
static void PCallSafe(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
    { PCallResultHandle(lS, PCallExSafe(lS, iParams, iReturns, iHandler));}
/* -- Sandboxed call function that doesn't pop the handler ----------------- */
static void PCall(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
    { PCallResultHandle(lS, PCallEx(lS, iParams, iReturns, iHandler)); }
/* -- If string is blank then return other string -------------------------- */
static void IfBlank(lua_State*const lS)
{ // Get replacement string first
  size_t stEmp;
  const char*const cpEmp = GetLString<char>(lS, 1, stEmp, "StringIfEmpty");
  // If the second parameter doesn't exist then return the empty string
  if(lua_isnoneornil(lS, 2)) { PushLString(lS, cpEmp, stEmp); return; }
  // Second parameter is valid, but return it if LUA says it is empty
  size_t stStr;
  const char*const cpStr = GetLString<char>(lS, 2, stStr, "CheckString");
  if(!stStr) { PushLString(lS, cpEmp, stEmp); return; }
  // It isn't empty so return original string
  PushLString(lS, cpStr, stStr);
}
/* -- Convert string string map to lua table and put it on stack -----------
static void ToTable(lua_State*const lS, const StrStrMap &ssmData)
{ // Create the table, we're creating non-indexed key/value pairs
  PushTable(lS, 0, ssmData.size());
  // For each table item
  for(const auto &ssmPair : ssmData)
  { // Push value name
    PushCppString(lS, ssmPair.second);
    // Push key name
    lua_setfield(lS, -2, ssmPair.first.c_str());
  }
} */
/* -- Convert a directory info object and put it on stack ------------------ */
static void ToTable(lua_State*const lS, const Dir::EntMap &demList)
{ // Create the table, we're creating a indexed/value array
  PushTable(lS, demList.size());
  // Entry id
  lua_Integer iId = 0;
  // For each table item
  for(const auto &demPair : demList)
  { // Push table index
    PushInteger(lS, ++iId);
    // Create the sub for file info, we're creating a indexed/value array
    PushTable(lS, 6);
    // Push filename
    PushInteger(lS, 1);
    PushCppString(lS, demPair.first);
    lua_rawset(lS, -3);
    // Push file size
    PushInteger(lS, 2);
    PushInteger(lS, static_cast<lua_Integer>(demPair.second.qSize));
    lua_rawset(lS, -3);
    // Push file creation time
    PushInteger(lS, 3);
    PushInteger(lS, static_cast<lua_Integer>(demPair.second.tCreate));
    lua_rawset(lS, -3);
    // Push file write time
    PushInteger(lS, 4);
    PushInteger(lS, static_cast<lua_Integer>(demPair.second.tWrite));
    lua_rawset(lS, -3);
    // Push file access time
    PushInteger(lS, 5);
    PushInteger(lS, static_cast<lua_Integer>(demPair.second.tAccess));
    lua_rawset(lS, -3);
    // Push file flags
    PushInteger(lS, 6);
    PushInteger(lS, static_cast<lua_Integer>(demPair.second.qFlags));
    lua_rawset(lS, -3);
    // Push file data table
    lua_rawset(lS, -3);
  }
}
/* -- Convert string vector to lua table and put it on stack --------------- */
template<typename ListType>
  static void ToTable(lua_State*const lS, const ListType &ltData)
{ // Create the table, we're creating a indexed/value array
  PushTable(lS, ltData.size());
  // Done if empty
  if(ltData.empty()) return;
  // Id number for array index
  lua_Integer iIndex = 0;
  // For each table item
  for(const string &vItem : ltData)
  { // Table index
    PushInteger(lS, ++iIndex);
    // Push value as string
    PushCppString(lS, vItem);
    // Push key pair as integer table
    lua_rawset(lS, -3);
  }
}
/* -- Explode LUA string into table ---------------------------------------- */
static void Explode(lua_State*const lS)
{ // Check parameters
  size_t stStr, stSep;
  const char*const cpStr = GetLString<char>(lS, 1, stStr, "String");
  const char*const cpSep = GetLString<char>(lS, 2, stSep, "Separator");
  // Create empty table if string invalid
  if(!stStr || !stSep) { PushTable(lS); return; }
  // Else convert whats in the string
  ToTable(lS, Token({cpStr, stStr}, {cpSep, stSep}));
}
/* -- Explode LUA string into table ---------------------------------------- */
static void ExplodeEx(lua_State*const lS)
{ // Check parameters
  size_t stStr, stSep;
  const char*const cpStr = GetLString<char>(lS, 1, stStr, "String");
  const char*const cpSep = GetLString<char>(lS, 2, stSep, "Separator");
  const size_t stMax = GetInt<size_t>(lS, 3, "Maximum");
  // Create empty table if string invalid
  if(!stStr || !stSep || !stMax) { PushTable(lS); return; }
  // Else convert whats in the string
  ToTable(lS, Token({cpStr, stStr}, {cpSep, stSep}, stMax));
}
/* -- Process initial implosion a table ------------------------------------ */
static lua_Integer ImplodePrepare(lua_State*const lS, const int iMaxParams)
{ // Must have this many parameters
  CheckParams(lS, iMaxParams);
  // Check table and get its size
  CheckTable(lS, 1, "StringTable");
  // Get size of table clamped since lua_rawlen returns unsigned and the
  // lua_rawgeti parameter is signed. Compare the result...
  switch(const lua_Integer liLen =
    IntOrMax<lua_Integer>(LuaUtilGetSize(lS, 1)))
  { // No entries? Just check the separator for consistency and push blank
    case 0: CheckString(lS, 2, "Separator");
            PushCppString(lS, strBlank);
            break;
    // One entry? Just check the separator and push the first item
    case 1: CheckString(lS, 2, "Separator");
            GetReferenceEx(lS);
            break;
    // More than one entry? Caller must process this;
    default: return liLen;
  } // We handled it
  return 0;
}
/* -- Pushes an item from the specified table onto the stack --------------- */
static void ImplodeItem(lua_State*const lS, const int iParam,
  const lua_Integer liIndex, string &strOutput, const char *cpStr,
  size_t stStr)
{ // Add separator to string
  strOutput.append(cpStr, stStr);
  // Get item from table
  GetReferenceEx(lS, 1, liIndex);
  // Get the string from Lua stack and save the length
  cpStr = lua_tolstring(lS, iParam, &stStr);
  // Append to supplied string
  strOutput.append(cpStr, stStr);
  // Remove item from stack
  RemoveStack(lS);
}
/* -- Implode LUA table into string ---------------------------------------- */
static void Implode(lua_State*const lS)
{ // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Integer liLen = ImplodePrepare(lS, 2))
  { // Get separator
    size_t stSep;
    const char*const cpSep = GetLString<char>(lS, 2, stSep, "Separator");
    // Write first item
    GetReferenceEx(lS);
    string strOutput{ ToCppString(lS) };
    RemoveStack(lS);
    // Iterate through rest of table and implode the items
    for(lua_Integer liI = 2; liI <= liLen; ++liI)
      ImplodeItem(lS, 3, liI, strOutput, cpSep, stSep);
    // Return string
    PushCppString(lS, strOutput);
  }
}
/* -- Implode LUA table into human readable string ------------------------- */
static void ImplodeEx(lua_State*const lS)
{ // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Integer liLen = ImplodePrepare(lS, 3))
  { // Get and check separators
    size_t stSep;
    const char*const cpSep = GetLString<char>(lS, 2, stSep, "Separator");
    size_t stSep2;
    const char*const cpSep2 = GetLString<char>(lS, 3, stSep2, "LastSeparator");
    // Write first item
    GetReferenceEx(lS);
    string strOutput{ ToCppString(lS) };
    RemoveStack(lS);
    // Iterator through rest of table except for last entry
    for(lua_Integer liI = 2; liI < liLen; ++liI)
      ImplodeItem(lS, 4, liI, strOutput, cpSep, stSep);
    // If there was more than one item? Implode the last item
    if(liLen > 1) ImplodeItem(lS, 4, liLen, strOutput, cpSep2, stSep2);
    // Return string
    PushCppString(lS, strOutput);
  }
}
/* -- Enumerate number of items in a table (non-indexed) ------------------- */
static lua_Unsigned GetKeyValueTableSize(lua_State*const lS)
{ // Check that we have a table of strings
  CheckTable(lS, 1, "KeyValueTable");
  // Number of indexed items in table
  const lua_Unsigned uiIndexedCount = LuaUtilGetSize(lS, 1);
  // Number of items in table
  lua_Unsigned uiCount = 0;
  // Until there are no more items
  for(PushNil(lS); lua_next(lS, -2); RemoveStack(lS)) ++uiCount;
  // Remove key
  RemoveStack(lS);
  // Return count of key/value pairs in table
  return uiCount - uiIndexedCount;
}

/* -- Replace text with values from specified LUA table -------------------- */
static void ReplaceMulti(lua_State*const lS)
{ // Get string to replace
  string strDest{ GetCppString(lS, 1, "String") };
  // Check that we have a table of strings
  CheckTable(lS, 2, "Data");
  // Source string is empty?
  if(!strDest.empty())
  { // Build table
    StrPairList lList;
    // Until there are no more items, add value if key is a string
    for(PushNil(lS); lua_next(lS, -2); RemoveStack(lS))
      if(lua_isstring(lS, -1))
        lList.push_back({ ToCppString(lS, -2), ToCppString(lS) });
    // Remove table
    RemoveStack(lS);
    // List isn't empty?
    if(!lList.empty())
    { // Build the string and push it to lua
      PushCppString(lS, ReplaceEx(strDest, lList));
      // Done
      return;
    }
  } // Return original string by deleting the table parameter
  RemoveStack(lS);
}
/* -- Convert string/uint map to table ------------------------------------- */
static void ToTable(lua_State*const lS, const StrUIntMap &vData)
{ // Create the table, we're creating non-indexed key/value pairs
  PushTable(lS, 0, vData.size());
  // For each table item
  for(const auto &vI : vData)
  { // Push value name
    PushInteger(lS, vI.second);
    // Push key name
    lua_setfield(lS, -2, vI.first.c_str());
  }
}
/* -- Convert varlist to lua table and put it on stack --------------------- */
static void ToTable(lua_State*const lS, const StrNCStrMap &vData)
{ // Create the table, we're creating non-indexed key/value pairs
  PushTable(lS, 0, vData.size());
  // For each table item
  for(const auto &vI : vData)
  { // Push value name
    PushCppString(lS, vI.second);
    // Push key name
    lua_setfield(lS, -2, vI.first.c_str());
  }
}
/* -- Initialise lua and clib random number generators --------------------- */
static void InitRNGSeed(lua_State*const lS, const lua_Integer liSeed)
{ // Push a new random seed into C-Lib too
  STDSRAND(static_cast<unsigned int>(liSeed));
  // Get 'math' table
  lua_getglobal(lS, "math");
  // Get pointer to function
  lua_getfield(lS, -1, "randomseed");
  // Push a random seed
  PushInteger(lS, liSeed);
  // Calls randomseed(uqSeed)
  CallFuncEx(lS, 1);
  // Removes the table 'math'
  RemoveStack(lS);
}
/* -- Return true if lua stack can take specified more items --------------- */
static bool StackHasCapacity(lua_State*const lS, const int iCount)
  { return lua_checkstack(lS, iCount); }
/* -- Return true if lua stack can take specified more items --------------- */
/* -- FIXME: Multiple types can access this, checkstack also restricted to - */
/* --        'int' type so this is not good enough! ------------------------ */
template<typename IntType>
  static bool StackHasCapacity(lua_State*const lS, const IntType itCount)
    { return IntWillOverflow<int>(itCount) ? false :
        StackHasCapacity(lS, static_cast<int>(itCount)); }
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
