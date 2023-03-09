/* == CVARS.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles the logic behind CVars which are settings that  ## */
/* ## the user can change to manipulate the engine's functionality. Add   ## */
/* ## new cvars in cvardef.hpp and cvarlib.hpp.                           ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfCVar {                     // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfCVarLib;             // Using cvar lib interface
using namespace IfLua;                 // Using lua interface
/* == CVars class ========================================================== */
static class CVars                     // Start of vars class
{ /* ----------------------------------------------------------------------- */
  #define CVAR_CONFIG_SIZE_MINIMUM     2
  #define CVAR_CONFIG_SIZE_MAXIMUM     1048576
  #define CVAR_CONFIG_MAX_LEVEL        10
  /* --------------------------------------------------------------- */ public:
  enum DefaultsCommand                 // Flags when loaded from DB
  { /* -- (Note: Don't ever change these around) --------------------------- */
    DC_NONE,                           // Accept current configuration
    DC_OVERWRITE,                      // Overwrite core variables only
    DC_REFRESH                         // Wipe database completely
  };/* -- Lua -------------------------------------------------------------- */
  LuaFunc::Map     lfList;             // Lua cvar list
  mutex            mMutex;             // Synchronisation support
  /* ----------------------------------------------------------------------- */
  typedef map<const string, Item> ItemMap;        // Var list
  typedef ItemMap::iterator       ItemMapIt;      // " iterator
  typedef ItemMap::const_iterator ItemMapItConst; // " iterator
  typedef array<ItemMapIt, MAX_CVAR> ArrayVars;   // " to internal vars
  /* -- Private variables -------------------------------------------------- */
  size_t           stMaxInactiveCount; // Maximum Initial CVars allowed
  ItemMap          imInactive;         // CVars inactive list
  ArrayVars        avInternal;         // Quick lookup to internal vars
  size_t           stMaxActiveCount;   // Maximum CVars allowed
  ItemMap          imActive;           // CVars active list
  string           strCBError;         // Callback error message
  /* ----------------------------------------------------------------------- */
  struct ItemMapPair                   // Join initial with cvars
  { /* --------------------------------------------------------------------- */
    ItemMap         &imList;           // Reference to variable list
    const char*const cpName;           // Name of list
  }; /* -------------------------------------------------------------------- */
  typedef array<const ItemMapPair,2> CVarList;   // CVarList type
  typedef CVarList::iterator         CVarListIt; // CVarList type
  const   CVarList                   impCVars;   // All variables lists
  /* ----------------------------------------------------------------------- */
  void UnregisterAllVars(void)
  { // Log result then dereg core variables, they don't need testing
    LW(LH_DEBUG, "CVars unregistering core variables...");
    // Unregister each built-in engine cvar
    for(const ItemStatic &cvaR : cvEngList)
    { // If the variable falls into the current gui mode then unregister it
      if(cSystem->GetGuiMode() >= cvaR.guimMin &&
         cSystem->GetGuiMode() <= cvaR.guimMax)
        UnregisterVarNoLog(cvaR.tcsVar.strN);
    } // if everything is unregistered? (This should always be true!)
    if(imActive.empty())
    { // Log that we are done and return
      LW(LH_INFO, "CVars finished unregistering all variables.");
      return;
    }
    // Report amount of lingering variables. We should never get here so if
    // this code is ever executed then there is an internal issue.
    LW(LH_DEBUG, "CVars unregistering $ lingering variables...",
      imActive.size());
    // Report remaining variables
    while(!imActive.empty())
    { // Get iterator and data
      const ItemMapIt itItem{ imActive.begin() };
      const Item &cvarItem = itItem->second;
      // Report it
      LW(LH_WARNING, "CVars detected lingering cvar '$' "
                     "with value '$' and flags $$.",
        cvarItem.GetVar(), cvarItem.GetValue(), hex, cvarItem.FlagGet());
      // Delete it
      imActive.erase(itItem);
    } // Report proper cleanup
    LW(LH_INFO, "CVars finished unregistering lingering variables.");
  }
  /* ----------------------------------------------------------------------- */
  bool InitialVarExists(const string &strVar) const
    { return imInactive.find(strVar) != imInactive.cend(); }
  /* ----------------------------------------------------------------------- */
  bool VarExists(const string &strVar) const
    { return imActive.find(strVar) != imActive.cend(); }
  /* ----------------------------------------------------------------------- */
  const ItemMapIt GetVarIterator(const string &strVar)
  { // Find item and throw error if the variable is not found
    const ItemMapIt cvarItem{ imActive.find(strVar) };
    if(cvarItem == imActive.cend()) XC("CVar not found!", "Variable", strVar);
    // Found the iterator so return it.
    return cvarItem;
  }
  /* -- Return the cvar name's value as a string --------------------------- */
  const string &GetStr(const string &strVar)
    { return GetVarIterator(strVar)->second.GetValue(); }
  /* -- Return the cvar id's value as a string ----------------------------- */
  const string &GetStrInternal(const CVarEnums cvId)
    { return avInternal[cvId]->second.GetValue(); }
  /* ----------------------------------------------------------------------- */
  const string &LuaGetStr(const string &strVar)
  { // Get the cvar item structure
    const Item &ciItem = GetVarIterator(strVar)->second;
    if(ciItem.FlagIsSet(CONFIDENTIAL))
      XC("CVar is private!", "Variable", strVar);
    // Return the value
    return ciItem.GetValue();
  }
  /* ----------------------------------------------------------------------- */
  bool IsVarStrEmpty(const string &strVar) { return GetStr(strVar).empty(); }
  /* ----------------------------------------------------------------------- */
  template<typename T>const T GetInternal(const CVarEnums cvId)
    { return move(ToNumber<T>(GetStrInternal(cvId))); }
  /* -- Check that the variable name is valid ------------------------------ */
  bool IsValidVariableName(const string &strVar)
  { // Check minimum name length
    if(strVar.length() < CVAR_MIN_LENGTH ||
       strVar.length() > CVAR_MAX_LENGTH) return false;
    // Get address of string. The first character must be a letter
    const unsigned char *ucpPtr =
      reinterpret_cast<const unsigned char*>(strVar.c_str());
    if(!isalpha(*ucpPtr)) return false;
    // For each character in cvar name until end of string...
    for(const unsigned char*const ucpPtrEnd = ucpPtr + strVar.length();
                                   ++ucpPtr < ucpPtrEnd;)
    { // If it is an underscore?
      if(*ucpPtr == '_')
      { // Next character must be a letter. This could also catch a nullptr
        // character if at the end of string but thats okay too!
        if(!isalpha(*(++ucpPtr))) return false;
        // Skip underscore and keep comparing with new conditions. The
        // underscore is now allowed normally.
        while(++ucpPtr < ucpPtrEnd)
          if(!isalnum(*ucpPtr) && *ucpPtr != '_')
            return false;
        // Success!
        return true;
      } // Fail if not a letter
      else if(!isalpha(*ucpPtr)) break;
    } // An underscore was not specified or invalid character
    return false;
  }
  /* ----------------------------------------------------------------------- */
  void CheckValidVariableName(const string &strVar)
  { // Throw error if the variable name is not valid
    if(!IsValidVariableName(strVar))
      XC("CVar name is not valid!",
         "Variable", strVar,
         "Minimum",  CVAR_MIN_LENGTH,
         "Maximum",  CVAR_MAX_LENGTH);
  }
  /* ----------------------------------------------------------------------- */
  bool IsTypeValidInFlags(const CVarFlagsConst &cFlags)
  { // Check that the var has at least one type
    return (cFlags.FlagIsSet(TSTRING) || cFlags.FlagIsSet(TINTEGER)  ||
            cFlags.FlagIsSet(TFLOAT)  || cFlags.FlagIsSet(TBOOLEAN)) &&
    // Check that types are not mixed
    ((cFlags.FlagIsSet(TSTRING)    && cFlags.FlagIsClear(TINTEGER)  &&
      cFlags.FlagIsClear(TFLOAT)   && cFlags.FlagIsClear(TBOOLEAN)) ||
     (cFlags.FlagIsSet(TINTEGER)   && cFlags.FlagIsClear(TSTRING)   &&
      cFlags.FlagIsClear(TFLOAT)   && cFlags.FlagIsClear(TBOOLEAN)) ||
     (cFlags.FlagIsSet(TFLOAT)     && cFlags.FlagIsClear(TSTRING)   &&
      cFlags.FlagIsClear(TINTEGER) && cFlags.FlagIsClear(TBOOLEAN)) ||
     (cFlags.FlagIsSet(TBOOLEAN)   && cFlags.FlagIsClear(TSTRING)   &&
      cFlags.FlagIsClear(TINTEGER) && cFlags.FlagIsClear(TFLOAT)));
  }
  /* ----------------------------------------------------------------------- */
  void CheckTypeValidInFlags(const string &strVar,
    const CVarFlagsConst &cFlags)
  { // Throw error if the flags do not contain a type or mixed types
    if(!IsTypeValidInFlags(cFlags))
      XC("CVar flags have none or mixed types!",
         "Variable", strVar, "Flags", cFlags.FlagGet());
  }
  /* ----------------------------------------------------------------------- */
  void CheckVariableDoesntExist(const string &strVar)
  { // Throw error if the variable exists
    if(VarExists(strVar))
      XC("CVar already registered!", "Variable", strVar);
  }
  /* ----------------------------------------------------------------------- */
  void RegisterVar(const string &strVar, const string &strValue,
    CbFunc cbTrigger, const CVarFlagsConst &cFlags,
    const CVarConditionFlagsConst &scFlags)
  { // Check that the variable name is valid
    CheckValidVariableName(strVar);
    // Check that the var has at least one type
    CheckTypeValidInFlags(strVar, cFlags);
    // Check that the variable does not exist already
    CheckVariableDoesntExist(strVar);
    // Do register the var
    DoRegisterVar(strVar, strValue, cbTrigger, cFlags, scFlags);
  }
  /* -- Unregister variable by iterator ------------------------------------ */
  void UnregisterVar(const ItemMapIt &itItem)
  { // Get cvar data
    const Item &ciItem = itItem->second;
    // Get flags and throw exception if variable has been locked
    if(ciItem.FlagIsSet(LOCKED))
      XC("CVar cannot unregister in callback!", "Variable", ciItem.GetVar());
    // If this cvar is marked as commit or force save or loaded from database?
    // Move back into the initial list so it can be saved.
    if(ciItem.FlagIsSet(COMMIT) ||
       ciItem.FlagIsSet(OSAVEFORCE) ||
       ciItem.FlagIsSet(LOADED))
      imInactive.emplace(move(*itItem));
    // Erase variable
    imActive.erase(itItem);
  }
  /* -- Unregister variable without logging -------------------------------- */
  bool UnregisterVarNoLog(const string &strVar)
  { // Find cvar data matching item name if not found? Log error and return
    const ItemMapIt itItem{ imActive.find(strVar) };
    if(itItem == imActive.cend()) return false;
    // Get cvar data
    UnregisterVar(itItem);
    // Success
    return true;
  }
  /* -- Unregister variable with logging ----------------------------------- */
  void UnregisterVar(const string &strVar)
  { // Try to unregister the variable and if it succeeded?
    if(UnregisterVarNoLog(strVar))
    { // Log that we unregistered it
      LW(LH_DEBUG, "CVars unregistered '$'.", strVar);
    } // We could not unregister the variable so log the failure
    else LW(LH_WARNING, "CVars un-register '$' not found!", strVar);
  }
  /* ----------------------------------------------------------------------- */
  void SetInitialVar(Item &ciItem, const string &strVal,
    const CVarFlagsConst &cFlags)
  { // Ignore if same value
    if(ciItem.GetValue() == strVal)
    { // Flags are the same too?
      if(ciItem.FlagIsSet(cFlags))
      { // Log that we're not overriding
        LW(LH_WARNING,
          "CVars initial var '$' already set to '$'/$$!",
            ciItem.GetVar(), strVal, hex, cFlags.FlagGet());
      } // Flags need updating
      else
      { // Update flags
        ciItem.FlagSet(cFlags);
        // Log that we're overriding flags
        LW(LH_WARNING, "CVars initial var '$' flags overridden to $$ from $!",
          ciItem.GetVar(), hex, cFlags.FlagGet(), ciItem.FlagGet());
      } // Done
      return;
    } // Log that we're overriding
    LW(LH_WARNING,
      "CVars initial var '$' overridden with '$'[$$] from '$'[$!]",
        ciItem.GetVar(), strVal, hex, cFlags.FlagGet(), ciItem.GetValue(),
        ciItem.FlagGet());
    // Now override
    ciItem.SetValue(strVal);
    ciItem.FlagSet(cFlags);
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Reset(const string &strVar, const CVarFlagsConst &cAcc,
    const CVarConditionFlagsConst &scFlags)
  { // Find item and if variable is found? Goto the next step
    const ItemMapIt cvarItem{ imActive.find(strVar) };
    if(cvarItem != imActive.end())
      return cvarItem->second.ResetValue(cAcc, scFlags, mMutex, strCBError);
    // Just return if missing else throw an error
    if(scFlags.FlagIsSet(CSC_IGNOREIFMISSING)) return CVS_NOTFOUND;
    XC("CVar not found!", "Variable", strVar);
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums SetInternal(const CVarEnums cvId, const string &strValue,
    const CVarFlagsConst &cAcc, const CVarConditionFlagsConst &scFlags)
      { return avInternal[cvId]->second.SetValue(strValue, cAcc, scFlags,
          mMutex, strCBError); }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Set(const string &strVar, const string &strValue,
    const CVarFlagsConst &cAcc, const CVarConditionFlagsConst &scFlags)
  { // Find item and if variable is found? Goto the next step
    const ItemMapIt cvarItem{ imActive.find(strVar) };
    if(cvarItem != imActive.end())
      return cvarItem->second.SetValue(strValue, cAcc, scFlags,
        mMutex, strCBError);
    // Just return if missing else throw an error
    if(scFlags.FlagIsSet(CSC_IGNOREIFMISSING)) return CVS_NOTFOUND;
    XC("CVar not found!", "Variable", strVar, "Value", strValue);
  }
  /* ----------------------------------------------------------------------- */
  bool ParseBuffer(const string &strBuffer, const CVarFlagsConst &cF,
    const unsigned int uiLevel=0)
  { // Bail if size not acceptable
    if(strBuffer.length() <= CVAR_CONFIG_SIZE_MINIMUM ||
       strBuffer.length() > CVAR_CONFIG_SIZE_MAXIMUM)
         return false;
    // Split characters and if nothing found?
    const string strSplit{ GetTextFormat(strBuffer) };
    if(strSplit.empty())
    { // Log the detection issue and return failure
      LW(LH_ERROR, "CVars failed to detect config file type!");
      return false;
    } // Initialise it and log and return failed if there are no lines
    const VarsConst varLoaded{ strBuffer, strSplit, '=' };
    if(varLoaded.empty())
    { // Log the issue and return failure
      LW(LH_WARNING, "CVars detected no readable variables!");
      return false;
    } // Thread safety
    const LockGuard lgCVarsSync{ mMutex };
    // Total variables parsed, good vars and bad vars.
    size_t stGood = 0, stBad = 0;
    // For each item. Set variable or save for future reference.
    for(const auto &vI : varLoaded)
    { // If first string is not empty then check first character
      if(!vI.first.empty()) switch(vI.first[0])
      { // Comment? Ignore
        case '#': break;
        // Include?
        case '+':
        { // Get new level and if the limit is exceeded then throw error
          const unsigned int uiNewLevel = uiLevel + 1;
          if(uiNewLevel >= CVAR_CONFIG_MAX_LEVEL)
            XC("CVar include nest level too deep!",
               "File", vI.second, "Limit", CVAR_CONFIG_MAX_LEVEL);
          // Parse the buffer and strip spaces
          ParseBuffer(AssetExtract(Trim(vI.first.substr(1), ' ')).ToString(),
            cF, uiNewLevel);
          // Done
          break;
        } // Something else?
        default:
        { // Set the variable and if succeeded increment good counter else bad
          if(SetVarOrInitial(vI.first, vI.second, cF, CSC_IGNOREIFMISSING|
            CSC_SAFECALL|CSC_THROWONERROR|CSC_NOMARKCOMMIT))
              ++stGood;
          else ++stBad;
          // Done
          break;
        }
      }
    } // Set total cvars processed and log the result
    const size_t stParsed = stGood + stBad,
                 stIgnored = varLoaded.size() - stParsed;
    LW(LH_INFO, "CVars parsed $ lines with $ vars (G:$;B:$;I:$).",
      varLoaded.size(), stParsed, stGood, stBad, stIgnored);
    // Done
    return true;
  }
  /* -- Do register a new variable without any checks ---------------------- */
  const ItemMapIt DoRegisterVar(const string &strVar, const string &strValue,
    CbFunc cbTrigger, const CVarFlagsConst &cFlags,
    const CVarConditionFlagsConst &scFlags)
  { // Check that we can create another variable
    if(imActive.size() >= stMaxActiveCount)
      XC("CVar count upper threshold reached!",
         "Variable", strVar, "Maximum", stMaxActiveCount);
    // Find initial cvar in initial pending list and if not found?
    const ItemMapIt cvarPendItem{ imInactive.find(strVar) };
    if(cvarPendItem == imInactive.cend())
    { // Register cvar into cvar list and get iterator to it. Although putting
      // the default value in here does not matter since we are about to set it
      // anyway, the decryptfailquiet flag will need it if the decrypt fails.
      const ItemMapIt liIter{ imActive.insert({ strVar,
        Item{ strVar, strValue, cbTrigger, cFlags } }).first };
      // Capture exceptions as we need to remove the variable if the value
      // failed to set for a multitude of reasons.
      try
      { // Use the default value. Although we already set the default value
        // when we inserted it, we need to check if it is valid too.
        liIter->second.SetValue(strValue, cFlags|PANY,
          scFlags|CSC_THROWONERROR|CSC_NEWCVAR, mMutex, strCBError);
      } // Exception occured?
      catch(const exception &)
      { // Unregister the variable that was created to not cause problems when
        // for example, resetting LUA.
        imActive.erase(liIter);
        // Rethrow the error
        throw;
      } // Return iterator
      return liIter;
    } // Persistant var exists? Move into cvar list, remove persist & get data.
    const ItemMapIt liIter{ imActive.emplace(move(*cvarPendItem)).first };
    imInactive.erase(cvarPendItem);
    // Capture exceptions as we need to remove the variable if the value failed
    // to set for a multitude of reasons.
    try
    { // Update default value, trigger and flags. We also want to add the
      // LOADED flag to tell UnregisterVar() to move the variable back into the
      // initial list when unregistered for Lua reloads. Also maintain the
      // commit flag at least so the data is saved on exit when the cvar is
      // unregistered and re-registered.
      Item &cvarData = liIter->second;
      cvarData.SetDefaultValue(strValue);
      cvarData.SetTrigger(cbTrigger);
      cvarData.FlagReset(cFlags | LOADED | (cvarData & CVREGMASK));
      // Types must not have changed
      CheckTypeValidInFlags(strVar, cvarData);
      // Use the value in persistent storage instead
      cvarData.SetValue(cvarData.GetValue(), cFlags|PANY,
        scFlags|CSC_THROWONERROR|CSC_NEWCVAR, mMutex, strCBError);
      // Return iterator
      return liIter;
    } // exception occured
    catch(const exception &)
    { // Remove the item. We won't put it back in the initial list because we
      // might have corrupted the data.
      imActive.erase(liIter);
      // Rethrow the error.
      throw;
    }
  }
  /* ----------------------------------------------------------------------- */
  void RefreshSettings(void)
  { // Completely clear SQL cvars table.
    LW(LH_DEBUG, "CVars erasing saved engine settings...");
    cSql->CVarDropTable();
    cSql->CVarCreateTable();
    LW(LH_WARNING, "CVars finished erasing saved engine settings.");
  }
  /* ----------------------------------------------------------------------- */
  void OverwriteExistingSettings(void)
  { // Overwrite engine variables with defaults
    LW(LH_DEBUG, "CVars forcing default engine settings...");
    cSql->Begin();
    for(const ItemStatic &cvaR : cvEngList) cSql->CVarPurge(cvaR.tcsVar.strN);
    cSql->End();
    LW(LH_WARNING, "CVars finished setting defaults.");
  }
  /* == Cvar updated callback for Lua ============================== */ public:
  static CVarReturn LuaCallbackStatic(Item&,const string &);
  CVarReturn LuaCallback(Item &cvarData, const string &strVal)
  { // Find cvar and ignore if we don't have it yet! This can happen if the
    // variable is initialising for the first time. We haven't added the
    // variable to imActive yet and we don't want to until the CVARS system
    // has created the variable.
    const auto lrItem{ lfList.find(cvarData.GetVar()) };
    if(lrItem == lfList.cend()) return ACCEPT;
    // Call the Lua callback assigned. We're expecting one return value
    lrItem->second.LuaFuncProtectedDispatch(1, strVal, cvarData.GetVar());
    // Get result of the callback which means a boolean HAS to be returned or
    // we will deny the cvar change by default.
    const CVarReturn cvResult = BoolToCVarReturn(
      lua_isboolean(cLuaFuncs->LuaRefGetState(), -1) &&
      lua_toboolean(cLuaFuncs->LuaRefGetState(), -1));
    // Remove whatever it was that was returned by the callback
    RemoveStack(cLuaFuncs->LuaRefGetState());
    // Return result to cvar set function
    return cvResult;
  }
  /* -- -------------------------------------------------------------------- */
  bool SetVarOrInitial(const string &strVar, const string &strVal,
    const CVarFlagsConst &cAcc, const CVarConditionFlagsConst &scFlags)
  { // Try to set the variable, catch result
    switch(Set(strVar, strVal, cAcc, scFlags|CSC_IGNOREIFMISSING))
    { // Not found?
      case CVS_NOTFOUND:
        // Set the variable in the initial list and return result
        return SetInitialVar(strVar, strVal, cAcc, scFlags|CSC_NOIOVERRIDE);
      // No error or not changed? return success!
      case CVS_OKNOTCHANGED: case CVS_OK: return true;
      // Failed status code
      default: return false;
    }
  }
  /* -- Set var or initial safe with synchronisation ----------------------- */
  bool SetVarOrInitialSafe(const string &strVar, const string &strVal,
    const CVarFlagsConst &cAcc, const CVarConditionFlagsConst &scFlags)
  { // Synchronise use of cvars list and call function
    const LockGuard lgCVarsSync{ mMutex };
    return SetVarOrInitial(strVar, strVal, cAcc, scFlags);
  }
  /* ----------------------------------------------------------------------- */
  void RegisterDefaults(void)
  { // Must have default variables to register
    if(cvEngList.empty()) XC("No default variables to register!");
    // Register each cvar
    LW(LH_DEBUG, "CVars registering $ built-in variables...",
      cvEngList.size());
    for(size_t stIndex = 0; stIndex < cvEngList.size(); ++stIndex)
    { // Get cvar
      const ItemStatic &cvaR = cvEngList[stIndex];
      // Register the variable if the guimode is valid. Note that we might not
      // get the gui mode which is default 0, but when it is set, it will set
      // &guimId automatically (since it was passed as ref by core).
      avInternal[stIndex] =
        (cSystem->GetGuiMode() >= cvaR.guimMin &&
         cSystem->GetGuiMode() <= cvaR.guimMax) ?
           DoRegisterVar(cvaR.tcsVar.strN, cvaR.tcsVar.strV, cvaR.cbTrigger,
             cvaR.cFlags, CSC_NOTHING) :
           imActive.end();
    } // Finished
    LW(LH_INFO, "CVars registered $ of $ built-in variables for "
      "$ mode $.", imActive.size(), cvEngList.size(),
      cSystem->GetGuiModeString(), cSystem->GetGuiMode());
  }
  /* -- Register console variable  ----------------------------------------- */
  void RegisterLuaVar(lua_State*const lS)
  { // Must be running on the main thread
    cLua->StateAssert(lS);
    // Must have 4 parameters
    CheckParams(lS, 4);
    // Get and check the variable name
    const string strVar{ GetCppString(lS, 1, "Variable") };
    CheckValidVariableName(strVar);
    // Get the value name
    const string strD{ GetCppString(lS, 2, "Default") };
    // Get the flags and check that the flags are in range
    const CVarFlagsConst cF{ GetFlags(lS, 3, NONE, PRIVATE, "Flags") };
    // Check that the fourth parameter is a function
    CheckFunction(lS, 4, "Callback");
    // Don't allow a flags value above PRIVATE for lua cvars
    if(cF.FlagIsLesser(NONE) || cF.FlagIsGreaterEqual(PRIVATE))
      XC("CVar flags are invalid!",
        "Variable", strVar,       "Flags",   cF.FlagGet(),
        "Minimum",  NONE.FlagGet(), "Maximum", PRIVATE.FlagGet());
    // Check that the var has at least one type
    CheckTypeValidInFlags(strVar, cF);
    // Wait for concurrent operations on cvars to finish
    const LockGuard lgCVarsSync{ mMutex };
    // Find command and throw exception if already exists
    const auto cI{ lfList.find(strVar) };
    if(cI != lfList.cend())
      XC("Virtual cvar already exists!", "Variable", cI->first);
    // Check that the variable does not exist in global list
    CheckVariableDoesntExist(strVar);
    // Create a function and reference the function on the lua stack and insert
    // the reference into the list
    const auto lfItem{ lfList.insert({ strVar,
      LuaFunc(Append("CV:", strVar), true) }).first };
    // Capture exceptions as we need to remove the lua function class if
    // theres a problem.
    try
    { // Register the variable and get the iterator to the new cvar. Don't
      // forget the lua reference needs to be in place for when the callback
      // is called.
      DoRegisterVar(strVar, strD, LuaCallbackStatic,
        cF|TLUA|PANY, CSC_SAFECALL);
    } // Exception occured during registration so remove item and rethrow
    catch(const exception &) { lfList.erase(lfItem); throw; }
  }
  /* -- Do unregister console cvar ----------------------------------------- */
  void UnregisterLuaVar(const string &strCVar)
  { // Find command and throw error if not found
    const auto imiItem{ lfList.find(strCVar) };
    if(imiItem == lfList.cend())
      XC("Virtual cvar not found!", "Variable", strCVar);
    // Unregister the command
    UnregisterVar(imiItem->first);
    // Unreference the callback function and remove the variable
    lfList.erase(imiItem);
  }
  /* -- Unregister all cvars commands -------------------------------------- */
  void UnregisterAllLuaCVars(void)
  { // Wait for concurrent operations on cvar list to finish
    const LockGuard lgCVarsSync{ mMutex };
    // Bail if no commands
    if(lfList.empty()) return;
    // Say arrays unloadinghow many arrays loaded
    LW(LH_DEBUG, "CVars unloading $ virtual cvars...", lfList.size());
    // Unregister each lua cvar until...
    // FIXME: I originally intended to unload in the order of creation but
    // dumbass me forgot that std::map's are sorted so this code unloads
    // the vars in sorted descending order. I'd have to make another list
    // which stores the order of creation.
    do UnregisterLuaVar(next(lfList.rbegin()).base()->first);
    // Until the lua cvarlist is empty
    while(!lfList.empty());
    // Say how many arrays loaded
    LW(LH_INFO, "CVars unloaded virtual cvars.");
  }
  /* ----------------------------------------------------------------------- */
  const ItemMap &GetVarList(void) { return imActive; }
  size_t GetVarCount(void)
    { const LockGuard lgCVarsSync{ mMutex };  return imActive.size(); }
  const ItemMap &GetInitialVarList(void) { return imInactive; }
  /* -- Synchronise cvar list and set the variable ------------------------- */
  CVarSetEnums SetInternalSafe(const CVarEnums cvId, const string &strVal,
    const CVarFlagsConst &cAcc=PUSR,
    const CVarConditionFlagsConst &cF=CSC_NOTHING)
      { const LockGuard lgCVarsSync{ mMutex };
        return SetInternal(cvId, strVal, cAcc, cF|CSC_SAFECALL); }
  /* ----------------------------------------------------------------------- */
  template<typename T>void SetInternalSafe(const CVarEnums cvId, const T tV)
    { SetInternalSafe(cvId, ToString(tV)); }
  /* -- Synchronise cvar list and set the variable ------------------------- */
  CVarSetEnums SetSafe(const string &strVar, const string &strVal,
    const CVarFlagsConst &cAcc=PUSR,
    const CVarConditionFlagsConst &cF=CSC_NOTHING)
      { const LockGuard lgCVarsSync{ mMutex };
        return Set(strVar, strVal, cAcc, cF|CSC_SAFECALL); }
  /* -- Synchronise cvar list and reset the variable ----------------------- */
  CVarSetEnums ResetSafe(const string &strVar, const CVarFlagsConst &cAcc=PUSR,
    const CVarConditionFlagsConst &cF=CSC_NOTHING)
      { const LockGuard lgCVarsSync{ mMutex };
        return Reset(strVar, cAcc, cF|CSC_SAFECALL); }
  /* ----------------------------------------------------------------------- */
  bool SetInitialVar(const string &strVar, const string &strVal,
    const CVarFlagsConst &cFlags, const CVarConditionFlagsConst &scFlags)
  { // Check that the variable name is valid.
    if(!IsValidVariableName(strVar))
    { // Throw if theres an error
      if(scFlags.FlagIsSet(CSC_THROWONERROR))
        XC("CVar name is not valid! Only alphanumeric characters "
           "and underscores are acceptable!",
           "Name",  strVar,         "Value",     strVal,
           "Flags", cFlags.FlagGet(), "Condition", scFlags.FlagGet());
      // Return falure instead
      return false;
    } // Look if the initial var already exists and if we found it? Set it
    const ItemMapIt cvarItem{ imInactive.find(strVar) };
    if(cvarItem != imInactive.end())
    { // Ignore overwrite if requested
      if(scFlags.FlagIsSet(CSC_NOIOVERRIDE))
      { // Log that this action was denied
        LW(LH_WARNING, "CVars ignored overriding '$' with '$'!",
          strVar, strVal);
        // Failed
        return false;
      } // Do the set
      SetInitialVar(cvarItem->second, strVal, cFlags);
    } // Insert into initial list
    else
    { // Check that we can create another variable
      if(imInactive.size() >= stMaxInactiveCount)
      { // Can we throw error?
        if(scFlags.FlagIsSet(CSC_THROWONERROR))
          XC("Initial CVar count upper threshold reached!",
             "Variable", strVar, "Maximum", stMaxInactiveCount);
        // Log that this action was denied
        LW(LH_WARNING,
          "CVars not adding '$' because upper threshold of $ reached!",
            strVar, stMaxInactiveCount);
        // Failed
        return false;
      } // Check ok so insert
      imInactive.insert({ strVar, Item{ strVar, strVal, NoOp, cFlags } });
    } // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool SetExistingInitialVar(const string &strVar, const string &strVal,
    const CVarFlagsConst &cAcc=PUSR)
  { // Concurrency
    const LockGuard lgCVarsSync{ mMutex };
    // Find initial item and return failure if it doesn't exist
    const ItemMapIt cVarItem{ imInactive.find(strVar) };
    if(cVarItem == imInactive.end()) return false;
    // Set the value
    SetInitialVar(cVarItem->second, strVal, cAcc);
    // Success
    return true;
  }
  /* -- Unregister console cvar with lock ---------------------------------- */
  void UnregisterLuaVarSafe(const string &strCVar)
    { const LockGuard lgCVarsSync{ mMutex };
      UnregisterLuaVar(strCVar); }
  /* ----------------------------------------------------------------------- */
  template<typename T>void SetSafe(const string &strVar, const T tV)
    { SetSafe(strVar, ToString(tV)); }
  /* ----------------------------------------------------------------------- */
  CVarReturn SetDefaults(const unsigned int uiVal)
  { // Compare defaults setting
    switch(static_cast<DefaultsCommand>(uiVal))
    { // Use current configuration
      case DC_NONE: break;
      // Set defaults only? Overwrite engine variables with defaults
      case DC_OVERWRITE: OverwriteExistingSettings(); break;
      // Completely clear SQL cvars table.
      case DC_REFRESH: RefreshSettings(); break;
      // Invalid value
      default: return DENY;
    } // Ok
    return ACCEPT;
  }
  /* -- Return last error from callback (also moves it) -------------------- */
  const string GetCBError(void) { return move(strCBError); }
  /* ----------------------------------------------------------------------- */
  template<typename T>T GetInternalSafe(const CVarEnums cvId)
    { const LockGuard lgCVarsSync{ mMutex };
      return move(GetInternal<T>(cvId)); }
  /* ----------------------------------------------------------------------- */
  const string GetInitialVarSafe(const string &strVar)
    { const LockGuard lgCVarsSync{ mMutex };
      return GetInitialVar(strVar); }
  /* ----------------------------------------------------------------------- */
  const string &GetInternalStrSafe(const CVarEnums cvId)
    { const LockGuard lgCVarsSync{ mMutex };
      return GetStrInternal(cvId); }
  const char *GetInternalCStrSafe(const CVarEnums cvId)
    { return GetInternalStrSafe(cvId).c_str(); }
  /* -- Return the cvar name's value as a string --------------------------- */
  const string GetStrSafe(const string &strVar)
    { const LockGuard lgCVarsSync{ mMutex };
      return GetStr(strVar); }
  /* ----------------------------------------------------------------------- */
  const string &LuaGetStrSafe(const string &strVar)
    { const LockGuard lgCVarsSync{ mMutex };
      return LuaGetStr(strVar); }
  /* ----------------------------------------------------------------------- */
  bool IsVarStrEmptySafe(const string &strVar)
    { const LockGuard lgCVarsSync{ mMutex };
      return IsVarStrEmpty(strVar); }
  /* ----------------------------------------------------------------------- */
  bool VarExistsSafe(const string &strVar)
    { const LockGuard lgCVarsSync{ mMutex };
      return VarExists(strVar); }
  /* ----------------------------------------------------------------------- */
  bool InitialVarExistsSafe(const string &strVar)
    { const LockGuard lgCVarsSync{ mMutex };
      return InitialVarExists(strVar); }
  /* ----------------------------------------------------------------------- */
  const string Protect(const string &strVar) const
  { // Find item and return invalid if not found
    const ItemMapItConst cvarItem{ imActive.find(strVar) };
    return cvarItem == imActive.cend() ?
      "<invalid>" : cvarItem->second.Protect();
  }
  /* ----------------------------------------------------------------------- */
  size_t MarkAllEncodedVarsAsCommit(void)
  { // Thread safety
    const LockGuard lgCVarsSync{ mMutex };
    // Total number of commits
    SafeSizeT stCommitted{0};
    // Enumerate the initial list and cvar list asyncronously
    MYFOREACH(par_unseq, impCVars.cbegin(), impCVars.cend(),
      [&stCommitted](const ItemMapPair &impStruct)
      { // Enumerate the cvars in each list asynchronously
        MYFOREACH(par_unseq, impStruct.imList.begin(), impStruct.imList.end(),
            [&stCommitted](auto &imPair)
            { if(imPair.second.MarkEncodedVarAsCommit()) ++stCommitted; });
      });
    // Return commit count
    return stCommitted;
  }
  /* ----------------------------------------------------------------------- */
  size_t Save(void)
  { // Thread safety
    const LockGuard lgCVarsSync{ mMutex };
    // Done if sqlite database is not opened or vars table is not availabe
    if(!cSql->IsOpened() || cSql->CVarCreateTable() == Sql::CTR_FAIL)
      return string::npos;
    // Begin transaction
    cSql->Begin();
    // Total number of commits attempted which may need to be read and
    // written by multiple threads.
    SafeSizeT stCommitTotal{0}, stPurgeTotal{0};
    // Enumerate the lists asyncronously
    MYFOREACH(par_unseq, impCVars.cbegin(), impCVars.cend(),
      [&stCommitTotal, &stPurgeTotal](const ItemMapPair &impData)
    { // Total number of commits attempted which may need to be read and
      // written by multiple threads.
      SafeSizeT stCommit{0}, stPurge{0};
      // Iterate through the initial list and try to commit the cvar. We might
      // as well try to do this as quick as possible
      MYFOREACH(par_unseq,
        impData.imList.begin(), impData.imList.end(),
          [&stCommit, &stPurge](auto &imPair)
            { imPair.second.Save(stCommit, stPurge); });
      // Log variables written
      if(stCommit || stPurge)
        LW(LH_INFO, "CVars commited $ and purged $ from $ pool.",
          stCommit.load(), stPurge.load(), impData.cpName);
      // Add to totals
      stCommitTotal += stCommit;
      stPurgeTotal += stPurge;
    });
    // End transaction
    cSql->End();
    // Return number of records saved or updated
    return stCommitTotal + stPurgeTotal;
  }
  /* ----------------------------------------------------------------------- */
  const string GetInitialVar(const string &strKey, const string &strO={})
  { // Find var
    const ItemMapIt cvarItem{ imInactive.find(strKey) };
    // Return empty string or the var
    return cvarItem != imInactive.cend() ? cvarItem->second.GetValue() : strO;
  }
  /* ----------------------------------------------------------------------- */
  size_t Clean(void)
  { // Get all key names in cvars, no values
    if(!cSql->CVarReadKeys()) return 0;
    // Get vars list
    const Sql::Result &vVars = cSql->GetRecords();
    if(vVars.empty())
    { // Log message and return failed
      LW(LH_INFO, "CVars found no variables to clean up!");
      return 0;
    } // Thread safety to protect cvar modification
    const LockGuard lgCVarsSync{ mMutex };
    // Say how many records we're probing
    LW(LH_DEBUG, "CVars probing $ records...", vVars.size());
    // Begin transaction
    cSql->Begin();
    // Transactions committed
    size_t stCommit = 0;
    // For each record returned.
    for(const Sql::Records &sslPairs : vVars)
    { // Get key and goto next record if not found, else assign cvar name
      const Sql::RecordsIt smmI{ sslPairs.find("K") };
      if(smmI == sslPairs.cend()) continue;
      const Sql::DataListItem &mbO = smmI->second;
      // If is not a string?
      if(mbO.iT != SQLITE_TEXT)
      { // Delete it from database and add to counter if ok and goto next cvar
        if(cSql->CVarPurgeData(mbO.Ptr<char>(), mbO.Size()) == Sql::PR_OK)
          ++stCommit;
        continue;
      } // Get string and find cvar name in live cvar list, ignore if it exists
      const string strKey{ mbO.ToString() };
      if(VarExists(strKey)) continue;
      // Find item in initial/standby cvar list, delete it if found
      const ItemMapIt cvarItem{ imInactive.find(strKey) };
      if(cvarItem != imInactive.end()) imInactive.erase(cvarItem);
      // Delete from database and proceed to the next record and if failed?
      if(cSql->CVarPurge(strKey) != Sql::PR_OK) continue;
      // Number of transactions send to sqlite
      ++stCommit;
    } // End bulk transaction commital
    cSql->End();
    // If we have items to delete, we can say how many we deleted
    LW(LH_INFO, "CVars removed $ of $ orphan records.",
      stCommit, vVars.size());
    // Clean up downloaded records
    cSql->Reset();
    // Return number of rows affected
    return stCommit;
  }
  /* ----------------------------------------------------------------------- */
  bool LoadFromFile(const string &strFile, const CVarFlagsConst &cF)
    { return ParseBuffer(AssetExtract(strFile).ToString(), cF); }
  /* ----------------------------------------------------------------------- */
  size_t LoadFromDatabase(void)
  { // Return if table is not already created or not available
    if(cSql->CVarCreateTable() != Sql::CTR_OK_ALREADY || !cSql->CVarReadAll())
      return 0;
    // Number of variables loaded
    SafeSizeT stLoaded(0);
    // Get vars list and if not empty
    const Sql::Result &vVars = cSql->GetRecords();
    if(!vVars.empty())
    { // Log number of vars being parsed
      LW(LH_DEBUG, "CVars read $ persistent variables, parsing...",
        vVars.size());
      // For each record returned. Set each keypair returned, these are user
      // variables. We're using multithreading for this to accellerate
      // decryption and decompression routines.
      MYFOREACH(par_unseq, vVars.cbegin(), vVars.cend(),
        [this, &stLoaded](const Sql::Records &smmPairs)
      { // Get key and goto next record if not found, else set the key string
        const Sql::RecordsIt iRecKey{ smmPairs.find("K") };
        if(iRecKey == smmPairs.cend()) return;
        const Sql::DataListItem &mbKey = iRecKey->second;
        if(mbKey.iT != SQLITE_TEXT) return;
        // Convert the variable to string and ignore if invalid
        const string strVar{ mbKey.ToString() };
        // Get flags and goto next record if not found, else set the key string
        const Sql::RecordsIt iRecFlags{ smmPairs.find("F") };
        if(iRecFlags == smmPairs.cend()) return;
        const Sql::DataListItem &mbFlags = iRecFlags->second;
        if(mbFlags.iT != SQLITE_INTEGER) return;
        const SqlCVarDataFlagsConst lFlags{ mbFlags.ReadInt<sqlite_int64>() };
        // Get value and goto next record if not found, else set the value
        const Sql::RecordsIt iRecVal{ smmPairs.find("V") };
        if(iRecVal == smmPairs.cend()) return;
        const Sql::DataListItem &mbVal = iRecVal->second;
        // If the data pointed at 'V' is not encrypted?
        if(lFlags.FlagIsClear(LF_ENCRYPTED))
        { // Must be text
          if(mbVal.iT != SQLITE_TEXT)
          { // Show warning and ignore if not
            LW(LH_WARNING, "CVars expected SQLITE_TEXT<$> "
              "and not type $ for '$'!", SQLITE_TEXT, mbVal.iT, strVar);
            return;
          } // Store value directly with synchronisation and goto next
          if(SetVarOrInitialSafe(strVar, mbVal.ToString(),
            PUSR|SUDB, CSC_NOIOVERRIDE|CSC_NOTDECRYPTED))
              ++stLoaded;
          return;
        } // New decrypted value to write into
        string strNewValue;
        // Must be binary
        if(mbVal.iT != SQLITE_BLOB) return;
        // Capture exceptions because MagicBlock does that on error
        try
        { // Decrypt the value and get the result, and if that call fails?
          strNewValue = Block<CoDecoder>{ mbVal }.ToString();
        } // exception occured?
        catch(const exception &e)
        { // Log failure and try to reset the initial var so this does not
          // happen again.
          LW(LH_ERROR, "CVars variable '$' decrypt exception: $",
            strVar, e.what());
          // Next record
          return;
        }
        // Set the variable or place it in the initials list if cvar not not
        // registered. The access will be user mode only and assignments
        // are not to be marked as changed. Do not throw on error because it
        // is not easy to change a sql database manually if we change the
        // rules on a cvar.
        if(SetVarOrInitialSafe(strVar, strNewValue, PUSR,
          CSC_NOIOVERRIDE|CSC_DECRYPTED))
            ++stLoaded;
      }); // If we loaded all the variables?
      if(stLoaded == vVars.size())
      { // Report that we loaded all the variables
        LW(LH_INFO, "CVars parsed all $ persistent variables.",
          vVars.size());
      } // We didn't load all the variables?
      else
      { // Report that we could not load all the variables
        LW(LH_WARNING, "CVars only parsed $ of $ persistent variables!",
          stLoaded.load(), vVars.size());
      }
    } // No variables loaded
    else LW(LH_WARNING, "CVars found no variables to parse.");
    // Clean up downloaded records
    cSql->Reset();
    // Return loaded variables
    return stLoaded;
  }
  /* ----------------------------------------------------------------------- */
  CVarReturn SetDisplayFlags(const unsigned int uiFlags)
  { // Set the flags
    sfShowFlags.FlagReset(CVarShowFlagsConst{ uiFlags });
    // Done
    return ACCEPT;
  }
  /* ----------------------------------------------------------------------- */
  CVarReturn LoadSettings(const bool bDoLoad)
  { // Load settings if needed
    if(bDoLoad) LoadFromDatabase();
    // Same
    return ACCEPT;
  }
  /* -- Set maximum cvar count --------------------------------------------- */
  CVarReturn MaxCountModified(const size_t stCount)
    { return CVarSimpleSetInt(stMaxActiveCount, stCount); }
  /* -- Set maximum offline cvar count ------------------------------------- */
  CVarReturn MaxICountModified(const size_t stCount)
    { return CVarSimpleSetInt(stMaxInactiveCount, stCount); }
  /* -- Set and execute default app configuration file --------------------- */
  CVarReturn ExecuteAppConfig(const string &strC, string &strV)
  { // Build filename and deny change if failed
    const string strFN{ Append(strC, "." CFG_EXTENSION) };
    if(!LoadFromFile(strFN, PSYSTEM|SAPPCFG)) return DENY;
    // We are manually updating the value with the correct filename
    strV = move(strFN);
    return ACCEPT_HANDLED;
  }
  /* -- Default constructor ------------------------------------------------ */
  CVars(void) :
    /* -- Initialisation of members ---------------------------------------- */
    stMaxInactiveCount(MAX_CVAR),      // Initially set to max cvar count
    stMaxActiveCount(MAX_CVAR),        // Initially set to max cvar count
    impCVars{{                         // Set combined lists
      { imInactive, "unregistered" },  // Inactive cvars list
      { imActive,   "registered" } }}  // Active cvars list
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DTORHELPER(~CVars, Save(); UnregisterAllVars());
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CVars);              // Disable copy constructor and operator
  /* ----------------------------------------------------------------------- */
} *cCVars = nullptr;                   // Pointer to static class
/* == Cvar lua callback ==================================================== */
CVarReturn CVars::LuaCallbackStatic(Item &cvarD, const string &strVal)
  { return cCVars->LuaCallback(cvarD, strVal); }
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
