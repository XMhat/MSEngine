/* == CVARS.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles the logic behind CVars which are settings that  ## **
** ## the user can change to manipulate the engine's functionality. Add   ## **
** ## new cvars in cvardef.hpp and cvarlib.hpp.                           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICVar {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace ICodec::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IDir::P;               using namespace IError::P;
using namespace ILog::P;               using namespace ILua::P;
using namespace ILuaFunc::P;           using namespace ILuaUtil::P;
using namespace IParser::P;            using namespace ISql::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace Lib::Sqlite;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* -- Actual cvar list types ----------------------------------------------- */
typedef pair<const string, CVarItem>   CVarMapPair;
typedef map<CVarMapPair::first_type,
            CVarMapPair::second_type>  CVarMap;
typedef CVarMap::iterator              CVarMapIt;
typedef CVarMap::const_iterator        CVarMapItConst;
/* -- Lua cvar list types -------------------------------------------------- */
typedef pair<LuaFunc, CVarMapIt>         LuaCVarPair;
typedef pair<const string, LuaCVarPair>  LuaCVarMapPair;
typedef map<LuaCVarMapPair::first_type,
            LuaCVarMapPair::second_type> LuaCVarMap;
typedef LuaCVarMap::iterator             LuaCVarMapIt;
typedef LuaCVarMap::const_iterator       LuaCVarMapItConst;
/* ------------------------------------------------------------------------- */
enum CVarDefaults : unsigned int       // Flags when loaded from DB
{ /* -- (Note: Don't ever change these around) ----------------------------- */
  DC_NONE,                             // Accept current configuration
  DC_OVERWRITE,                        // Overwrite core variables only
  DC_REFRESH                           // Wipe database completely
}; /* ---------------------------------------------------------------------- */
/* == CVars class ========================================================== */
static class CVars final               // Start of vars class
{ /* -- Settings ----------------------------------------------------------- */
  constexpr static const size_t        // Some internal settings
    stCVarConfigSizeMinimum = 2,       // Minimum config file size
    stCVarConfigSizeMaximum = 1048576, // Maximum config file size
    stCVarConfigMaxLevel    = 10,      // Maximum recursive level
    stCVarMinLength         = 5,       // Minimum length of a cvar name
    stCVarMaxLength         = 255;     // Maximum length of a cvar name
  /* -- Private typedefs --------------------------------------------------- */
  typedef array<CVarMapIt, CVAR_MAX> ArrayVars;
  /* -- Private variables -------------------------------------------------- */
  size_t           stMaxInactiveCount; // Maximum Initial CVars allowed
  CVarMap          cvmPending;         // CVars inactive list
  ArrayVars        avInternal;         // Quick lookup to internal vars
  CVarMap          cvmActive;          // CVars active list
  string           strCBError;         // Callback error message
  /* -- Lua -------------------------------------------------------- */ public:
  LuaCVarMap       lcvmMap;            // Lua cvar list
  mutex            mMutex;             // Synchronisation support
  /* ----------------------------------------------------------------------- */
  struct CVarMapMulti                  // Join initial with cvars
  { /* --------------------------------------------------------------------- */
    CVarMap       &cmMap;              // Reference to variable list
    const char*const cpName;           // Name of list
  }; /* -------------------------------------------------------------------- */
  typedef array<const CVarMapMulti,2> CVarList;   // CVarList type
  typedef CVarList::iterator          CVarListIt; // CVarList type
  const   CVarList                    cvlList;    // All variables lists
  const   ItemStaticList             &cvEngList;  // Reference to default cvars
  /* ----------------------------------------------------------------------- */
  bool InitialVarExists(const string &strVar) const
    { return cvmPending.contains(strVar); }
  /* ----------------------------------------------------------------------- */
  bool VarExists(const string &strVar) const
    { return cvmActive.contains(strVar); }
  /* ----------------------------------------------------------------------- */
  const CVarMapIt GetVarIterator(const string &strVar)
  { // Find item and throw error if the variable is not found
    const CVarMapIt cvmiIt{ cvmActive.find(strVar) };
    if(cvmiIt == cvmActive.cend()) XC("CVar not found!", "Variable", strVar);
    // Found the iterator so return it.
    return cvmiIt;
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Set(const CVarMapIt cvmiIt, const string &strValue,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst cvcfcFlags)
      { return cvmiIt->second.SetValue(strValue,
          cvfcFlags, cvcfcFlags, mMutex, strCBError); }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Set(const LuaCVarMapIt cvmiIt, const string &strValue,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst cvcfcFlags)
      { return Set(cvmiIt->second.second, strValue, cvfcFlags, cvcfcFlags); }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Set(const string &strVar, const string &strValue,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst cvcfcFlags)
  { // Find item and if variable is found? Goto the next step
    const CVarMapIt cvmiIt{ cvmActive.find(strVar) };
    if(cvmiIt != cvmActive.end())
      return Set(cvmiIt, strValue, cvfcFlags, cvcfcFlags);
    // Just return if missing else throw an error
    if(cvcfcFlags.FlagIsSet(CCF_IGNOREIFMISSING)) return CVS_NOTFOUND;
    XC("CVar not found!", "Variable", strVar, "Value", strValue);
  }
  /* -- Return the cvar name's default value as a string ------------------- */
  const string &GetDefStr(const CVarMapIt cvmiIt) const
    { return cvmiIt->second.GetDefValue(); }
  /* ----------------------------------------------------------------------- */
  const string &GetDefStr(const LuaCVarMapIt cvmiIt) const
    { return GetDefStr(cvmiIt->second.second); }
  /* ----------------------------------------------------------------------- */
  const string &GetDefStr(const string &strVar)
    { return GetDefStr(GetVarIterator(strVar)); }
  /* -- Return the cvar name's value as a string --------------------------- */
  const string &GetStr(const CVarMapIt cvmiIt) const
    { return cvmiIt->second.GetValue(); }
  /* ----------------------------------------------------------------------- */
  const string &GetStr(const LuaCVarMapIt cvmiIt) const
    { return GetStr(cvmiIt->second.second); }
  /* ----------------------------------------------------------------------- */
  const string &GetStr(const string &strVar)
    { return GetStr(GetVarIterator(strVar)); }
  /* ----------------------------------------------------------------------- */
  bool IsVarStrEmpty(const LuaCVarMapIt cvmiIt) const
    { return GetStr(cvmiIt).empty(); }
  /* -- Return the cvar id's value as a string ----------------------------- */
  const string &GetStrInternal(const CVarEnums cveId)
  { // Get internal iterator and return value or empty string if invalid
    const CVarMapIt cvmiIt{ avInternal[cveId] };
    return cvmiIt != cvmActive.cend() ? cvmiIt->second.GetValue() :
      cCommon->Blank();
  }
  /* ----------------------------------------------------------------------- */
  void BuildCVarIDsList(lua_State*const lS)
  { // Get variables namespace
    lua_getglobal(lS, "Variable");
    // Create a table of the specified number of variables
    LuaUtilPushTable(lS, CVAR_MAX, 0);
    // Push each cvar id to the table
    lua_Integer liIndex = 0;
    for(const CVarMapIt &cvmiIt : avInternal)
    { // If stored iterator is valid?
      if(cvmiIt != cvmActive.cend())
      { // Push internal id value name
        LuaUtilPushInt(lS, liIndex);
        // Assign the id to the cvar name
        lua_setfield(lS, -2, cvmiIt->first.c_str());
      } // Next id
      ++liIndex;
    } // Push cvar id table into the core namespace
    lua_setfield(lS, -2, "Internal");
    // Remove the table
    LuaUtilRmStack(lS);
  }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>const IntType GetInternal(const CVarEnums cveId)
    { return StdMove(StrToNum<IntType>(GetStrInternal(cveId))); }
  /* -- Unregister variable by iterator ------------------------------------ */
  void UnregisterVar(const CVarMapIt &cvmiIt)
  { // Get cvar data
    const CVarItem &ciItem = cvmiIt->second;
    // Get flags and throw exception if variable has been locked
    if(ciItem.FlagIsSet(LOCKED))
      XC("CVar cannot unregister in callback!", "Variable", ciItem.GetVar());
    // If this cvar is marked as commit, force save or loaded from database?
    // Move back into the initial list so it can be saved.
    if(ciItem.FlagIsAnyOfSet(COMMIT|OSAVEFORCE|LOADED))
      cvmPending.emplace(StdMove(*cvmiIt));
    // Erase variable
    cvmActive.erase(cvmiIt);
  }
  /* -- Unregister variable without logging -------------------------------- */
  bool UnregisterVarNoLog(const string &strVar)
  { // Find cvar data matching item name if not found? Log error and return
    const CVarMapIt cvmiIt{ cvmActive.find(strVar) };
    if(cvmiIt == cvmActive.cend()) return false;
    // Get cvar data
    UnregisterVar(cvmiIt);
    // Success
    return true;
  }
  /* -- Unregister variable with logging ----------------------------------- */
  void UnregisterVar(const string &strVar)
  { // Try to unregister the variable and if it succeeded?
    if(UnregisterVarNoLog(strVar))
    { // Log that we unregistered it
      cLog->LogDebugExSafe("CVars unregistered '$'.", strVar);
    } // We could not unregister the variable so log the failure
    else cLog->LogWarningExSafe("CVars un-register '$' not found!", strVar);
  }
  /* ----------------------------------------------------------------------- */
  void SetInitialVar(CVarItem &ciItem, const string &strVal,
    const CVarFlagsConst cvfcFlags)
  { // Ignore if same value
    if(ciItem.GetValue() == strVal)
    { // Flags are the same too?
      if(ciItem.FlagIsSet(cvfcFlags))
      { // Log that we're not overriding
        cLog->LogWarningExSafe("CVars initial var '$' already set to '$'/$$!",
          ciItem.GetVar(), strVal, hex, cvfcFlags.FlagGet());
      } // Flags need updating
      else
      { // Update flags
        ciItem.FlagSet(cvfcFlags);
        // Log that we're overriding flags
        cLog->LogWarningExSafe(
          "CVars initial var '$' flags overridden to $$ from $!",
            ciItem.GetVar(), hex, cvfcFlags.FlagGet(), ciItem.FlagGet());
      } // Done
      return;
    } // Log that we're overriding
    cLog->LogWarningExSafe(
      "CVars initial var '$' overridden with '$'[$$] from '$'[$!]",
        ciItem.GetVar(), strVal, hex, cvfcFlags.FlagGet(), ciItem.GetValue(),
        ciItem.FlagGet());
    // Now override
    ciItem.SetValue(strVal);
    ciItem.FlagSet(cvfcFlags);
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums ResetInternal(const CVarEnums cveId,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst cvcfcFlags)
  { // Get internal iterator and return value or empty string if invalid
    const CVarMapIt cvmiIt{ avInternal[cveId] };
    return cvmiIt != cvmActive.cend() ?
      cvmiIt->second.ResetValue(cvfcFlags, cvcfcFlags, mMutex, strCBError) :
      CVS_NOTFOUND;
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Reset(const string &strVar, const CVarFlagsConst cvfcFlags,
    const CVarConditionFlagsConst cvcfcFlags)
  { // Find item and if variable is found? Goto the next step
    const CVarMapIt cvmiIt{ cvmActive.find(strVar) };
    if(cvmiIt != cvmActive.end())
      return cvmiIt->second.ResetValue(cvfcFlags,
        cvcfcFlags, mMutex, strCBError);
    // Just return if missing else throw an error
    if(cvcfcFlags.FlagIsSet(CCF_IGNOREIFMISSING)) return CVS_NOTFOUND;
    XC("CVar not found!", "Variable", strVar);
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums SetInternal(const CVarEnums cveId, const string &strValue,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst cvcfcFlags)
  { // Get iterator and set the value if valid except return invalid
    const CVarMapIt cvmiIt{ avInternal[cveId] };
    return cvmiIt != cvmActive.cend() ?
      cvmiIt->second.SetValue(strValue,
        cvfcFlags, cvcfcFlags, mMutex, strCBError) : CVS_NOTFOUND;
  }
  /* ----------------------------------------------------------------------- */
  bool ParseBuffer(const string &strBuffer, const CVarFlagsConst cvfcFlags,
    const unsigned int uiLevel)
  { // Bail if size not acceptable
    if(strBuffer.length() <= stCVarConfigSizeMinimum ||
       strBuffer.length() > stCVarConfigSizeMaximum)
         return false;
    // Split characters and if nothing found?
    const string strSplit{ StrGetReturnFormat(strBuffer) };
    if(strSplit.empty())
    { // Log the detection issue and return failure
      cLog->LogErrorSafe("CVars failed to detect config file type!");
      return false;
    } // Initialise it and log and return failed if there are no lines
    const ParserConst<> pConfig{ strBuffer, strSplit, '=' };
    if(pConfig.empty())
    { // Log the issue and return failure
      cLog->LogWarningSafe("CVars detected no readable variables!");
      return false;
    } // Total variables parsed, good vars and bad vars.
    size_t stGood = 0, stBad = 0;
    // For each item. Set variable or save for future reference.
    for(const auto &vI : pConfig)
    { // If first string is not empty then check first character
      if(!vI.first.empty()) switch(vI.first[0])
      { // Comment?
        case '#': break;
        // No key was parsed?
        case '\255':
        { // Test value
          if(!vI.second.empty()) switch(vI.second[0])
          { // Comment?
            case '#': break;
            // Include?
            case '+':
            { // Get new level and if the limit is exceeded then throw error
              const unsigned int uiNewLevel = uiLevel + 1;
              if(uiNewLevel >= stCVarConfigMaxLevel)
                XC("CVar include nest level too deep!",
                   "File", vI.second, "Limit", stCVarConfigMaxLevel);
              // Log the include and parse it
              if(ParseBuffer(AssetExtract(StrTrim(vI.second.substr(1), ' ')).
                MemToString(), cvfcFlags, uiNewLevel)) ++stGood; else ++stBad;
              // Done
              break;
            } // Something else?
            default: ++stBad; break;
          } // Done
          break;
        } // Something else?
        default:
        { // Set the variable and if succeeded increment good counter else bad
          if(SetVarOrInitial(vI.first, vI.second, cvfcFlags,
            CCF_IGNOREIFMISSING|CCF_SAFECALL|CCF_THROWONERROR|
              CCF_NOMARKCOMMIT))
                ++stGood;
          else ++stBad;
          // Done
          break;
        }
      }
    } // Set total cvars processed and log the result
    const size_t stParsed = stGood + stBad,
                 stIgnored = pConfig.size() - stParsed;
    cLog->LogInfoExSafe("CVars parsed $ lines with $ vars (G:$;B:$;I:$).",
      pConfig.size(), stParsed, stGood, stBad, stIgnored);
    // Done
    return true;
  }
  /* -- Do register a new variable without any checks ---------------------- */
  const CVarMapIt RegisterVar(const string &strVar, const string &strValue,
    CbFunc cbTrigger, const CVarFlagsConst cvfcFlags,
    const CVarConditionFlagsConst cvcfcFlags)
  { // Find initial cvar in initial pending list and if not found?
    const CVarMapIt cvmiPendIt{ cvmPending.find(strVar) };
    if(cvmiPendIt == cvmPending.cend())
    { // Register cvar into cvar list and get iterator to it. Although putting
      // the default value in here does not matter since we are about to set it
      // anyway, the decryptfailquiet flag will need it if the decrypt fails.
      const CVarMapIt cvmiIt{ cvmActive.insert({ strVar,
        CVarItem{ strVar, strValue, cbTrigger, cvfcFlags } }).first };
      // Capture exceptions as we need to remove the variable if the value
      // failed to set for a multitude of reasons.
      try
      { // Use the default value. Although we already set the default value
        // when we inserted it, we need to check if it is valid too.
        cvmiIt->second.SetValue(strValue, cvfcFlags|PANY,
          cvcfcFlags|CCF_THROWONERROR|CCF_NEWCVAR, mMutex, strCBError);
      } // Exception occured?
      catch(const exception &)
      { // Unregister the variable that was created to not cause problems when
        // for example, resetting LUA.
        cvmActive.erase(cvmiIt);
        // Rethrow the error
        throw;
      } // Return iterator
      return cvmiIt;
    } // Persistant var exists? Move into cvar list, remove persist & get data.
    const CVarMapIt cvmiIt{ cvmActive.emplace(StdMove(*cvmiPendIt)).first };
    cvmPending.erase(cvmiPendIt);
    // Capture exceptions as we need to remove the variable if the value failed
    // to set for a multitude of reasons.
    try
    { // Update default value, trigger and flags. We also want to add the
      // LOADED flag to tell UnregisterVar() to move the variable back into the
      // initial list when unregistered for Lua reloads. Also maintain the
      // commit flag at least so the data is saved on exit when the cvar is
      // unregistered and re-registered.
      CVarItem &cvarData = cvmiIt->second;
      cvarData.SetDefValue(strValue);
      cvarData.SetTrigger(cbTrigger);
      cvarData.FlagReset(cvfcFlags | LOADED | (cvarData & CVREGMASK));
      // Use the value in persistent storage instead
      cvarData.SetValue(cvarData.GetValue(), cvfcFlags|PANY,
        cvcfcFlags|CCF_THROWONERROR|CCF_NEWCVAR, mMutex, strCBError);
      // Return iterator
      return cvmiIt;
    } // exception occured
    catch(const exception &)
    { // Remove the item. We won't put it back in the initial list because we
      // might have corrupted the data.
      cvmActive.erase(cvmiIt);
      // Rethrow the error.
      throw;
    }
  }
  /* ----------------------------------------------------------------------- */
  void RefreshSettings(void)
  { // Completely clear SQL cvars table.
    cLog->LogDebugSafe("CVars erasing saved engine settings...");
    cSql->CVarDropTable();
    cSql->CVarCreateTable();
    cLog->LogWarningSafe("CVars finished erasing saved engine settings.");
  }
  /* ----------------------------------------------------------------------- */
  void OverwriteExistingSettings(void)
  { // Overwrite engine variables with defaults
    cLog->LogDebugSafe("CVars forcing default engine settings...");
    cSql->Begin();
    for(const ItemStatic &cvaR : cvEngList) cSql->CVarPurge(cvaR.strVar);
    cSql->End();
    cLog->LogWarningSafe("CVars finished setting defaults.");
  }
  /* == Cvar updated callback for Lua ============================== */ public:
  static CVarReturn LuaCallbackStatic(CVarItem&,const string &);
  CVarReturn LuaCallback(CVarItem &cvarData, const string &strVal)
  { // Find cvar and ignore if we don't have it yet! This can happen if the
    // variable is initialising for the first time. We haven't added the
    // variable to cvmActive yet and we don't want to until the CVARS system
    // has created the variable.
    const LuaCVarMapIt lcvmpIt{ lcvmMap.find(cvarData.GetVar()) };
    if(lcvmpIt == lcvmMap.cend()) return ACCEPT;
    // Call the Lua callback assigned. We're expecting one return value
    lcvmpIt->second.first.LuaFuncProtectedDispatch(1,
      strVal, cvarData.GetVar());
    // Get result of the callback which means a boolean HAS to be returned or
    // we will deny the cvar change by default.
    const CVarReturn cvResult = BoolToCVarReturn(
      lua_isboolean(cLuaFuncs->LuaRefGetState(), -1) &&
      lua_toboolean(cLuaFuncs->LuaRefGetState(), -1));
    // Remove whatever it was that was returned by the callback
    LuaUtilRmStack(cLuaFuncs->LuaRefGetState());
    // Return result to cvar set function
    return cvResult;
  }
  /* -- -------------------------------------------------------------------- */
  bool SetVarOrInitial(const string &strVar, const string &strVal,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst cvcfcFlags)
  { // Try to set the variable, catch result
    switch(Set(strVar, strVal, cvfcFlags, cvcfcFlags|CCF_IGNOREIFMISSING))
    { // Not found? Set the variable in the initial list and return result
      case CVS_NOTFOUND:
        return SetInitialVar(strVar,
          strVal, cvfcFlags, cvcfcFlags|CCF_NOIOVERRIDE);
      // No error or not changed? return success!
      case CVS_OKNOTCHANGED: case CVS_OK: return true;
      // Failed status code
      default: return false;
    }
  }
  /* -- Set var or initial safe with synchronisation ----------------------- */
  bool SetVarOrInitialSafe(const string &strVar, const string &strVal,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst cvcfcFlags)
  { // Synchronise use of cvars list and call function
    const LockGuard lgCVarsSync{ mMutex };
    return SetVarOrInitial(strVar, strVal, cvfcFlags, cvcfcFlags);
  }
  /* -- Returns the lua console command list ------------------------------- */
  const LuaCVarMap &GetLuaVarList(void) { return lcvmMap; }
  /* -- Returns the end of the lua console command list -------------------- */
  LuaCVarMapIt GetLuaVarListEnd(void) { return lcvmMap.end(); }
  /* -- Register console variable  ----------------------------------------- */
  LuaCVarMapIt RegisterLuaVar(const string &strVar, const string &strD,
    const CVarFlagsConst cvfcFlags)
  { // Wait for concurrent operations on cvars to finish
    const LockGuard lgCVarsSync{ mMutex };
    // Make sure cvar doesn't already exist
    if(VarExists(strVar))
      XC("CVar already registered!", "Variable", strVar);
    // Find command and throw exception if already exists
    const LuaCVarMapIt lcvmiIt{ lcvmMap.find(strVar) };
    if(lcvmiIt != lcvmMap.cend())
      XC("Virtual cvar already exists!", "Variable", lcvmiIt->first);
    // Iterators to the cvar item and the lua cvar item
    LuaCVarMapIt itLVar{ lcvmMap.end() };
    // Capture exceptions as we need to remove the cvar if theres a problem.
    try
    { // Save the function at the top of the stack used for the callback
      itLVar = lcvmMap.insert(lcvmMap.cend(), { strVar,
        make_pair(LuaFunc{ StrAppend("CV:", strVar), true }, cvmActive.end())
      }); // Register the variable and set the iterator to the new cvar.
      itLVar->second.second = RegisterVar(strVar,
        strD, LuaCallbackStatic, cvfcFlags|TLUA|PANY, CCF_SAFECALL);
    } // Exception occurred during registration?
    catch(const exception &)
    { // Erase the cvar from the cvar list if it is set
      if(itLVar->second.second != cvmActive.end())
        cvmActive.erase(itLVar->second.second);
      // Erase the lua cvar from the lua cvar list if it is set
      if(itLVar != lcvmMap.end())
        lcvmMap.erase(itLVar);
      // Rethrow the error
      throw;
    } // Return the item that was registered
    return itLVar;
  }
  /* -- Do unregister console cvar ----------------------------------------- */
  LuaCVarMapIt UnregisterLuaVar(LuaCVarMapIt imiItem)
  { // Lock the cvar list
    const LockGuard lgCVarsSync{ mMutex };
    // If the iterator is valid?
    if(imiItem != lcvmMap.cend())
    { // Unregister the cvar and dereference the function callback
      UnregisterVar(imiItem->first);
      lcvmMap.erase(imiItem);
    } // Return the last item in the last
    return lcvmMap.end();
  }
  /* ----------------------------------------------------------------------- */
  const CVarMap &GetVarList(void) { return cvmActive; }
  size_t GetVarCount(void)
    { const LockGuard lgCVarsSync{ mMutex };  return cvmActive.size(); }
  const CVarMap &GetInitialVarList(void) { return cvmPending; }
  /* -- Synchronise cvar list and set the variable ------------------------- */
  CVarSetEnums SetInternalSafe(const CVarEnums cveId, const string &strVal,
    const CVarFlagsConst cvfcFlags=PUSR,
    const CVarConditionFlagsConst cvcfcFlags=CCF_NOTHING)
      { const LockGuard lgCVarsSync{ mMutex };
        return SetInternal(cveId,
          strVal, cvfcFlags, cvcfcFlags|CCF_SAFECALL); }
  /* ----------------------------------------------------------------------- */
  template<typename AnyType>
    CVarSetEnums SetInternalSafe(const CVarEnums cveId, const AnyType atV)
      { return SetInternalSafe(cveId, StrFromNum(atV)); }
  /* -- Synchronise cvar list and set the variable ------------------------- */
  CVarSetEnums SetSafe(const string &strVar, const string &strVal,
    const CVarFlagsConst cvfcFlags=PUSR,
    const CVarConditionFlagsConst cvcfcFlags=CCF_NOTHING)
      { const LockGuard lgCVarsSync{ mMutex };
        return Set(strVar, strVal, cvfcFlags, cvcfcFlags|CCF_SAFECALL); }
  CVarSetEnums SetSafe(const LuaCVarMapIt cvmiIt, const string &strVal,
    const CVarFlagsConst cvfcFlags=PUSR,
    const CVarConditionFlagsConst cvcfcFlags=CCF_NOTHING)
      { const LockGuard lgCVarsSync{ mMutex };
        return Set(cvmiIt, strVal, cvfcFlags, cvcfcFlags|CCF_SAFECALL); }
  /* -- Synchronise cvar list and reset the variable ----------------------- */
  CVarSetEnums ResetSafe(const string &strVar,
    const CVarFlagsConst cvfcFlags=PUSR,
    const CVarConditionFlagsConst cvcfcFlags=CCF_NOTHING)
      { const LockGuard lgCVarsSync{ mMutex };
        return Reset(strVar, cvfcFlags, cvcfcFlags|CCF_SAFECALL); }
  /* -- Synchronise cvar list and reset the internal id variable ----------- */
  CVarSetEnums ResetInternalSafe(const CVarEnums cveId,
    const CVarFlagsConst cvfcFlags=PUSR,
    const CVarConditionFlagsConst cvcfcFlags=CCF_NOTHING)
      { const LockGuard lgCVarsSync{ mMutex };
        return ResetInternal(cveId, cvfcFlags, cvcfcFlags|CCF_SAFECALL); }
  /* ----------------------------------------------------------------------- */
  bool ParseBufferSafe(const string &strBuffer, const CVarFlagsConst cvfcFlags,
    const unsigned int uiLevel=0)
      { const LockGuard lgCVarsSync{ mMutex };
        return ParseBuffer(strBuffer, cvfcFlags, uiLevel); }
  /* ----------------------------------------------------------------------- */
  bool LoadFromFile(const string &strFile, const CVarFlagsConst cvfcFlags)
    { return ParseBufferSafe(AssetExtract(strFile).MemToString(), cvfcFlags); }
  /* ----------------------------------------------------------------------- */
  bool IsTypeValidInFlags(const CVarFlagsConst cvfcFlags) const
  { // Check that the var has at least one type
    return cvfcFlags.FlagIsAnyOfSet(TSTRING|TINTEGER|TFLOAT|TBOOLEAN) &&
      // Check that types are not mixed
      cvfcFlags.FlagIsAnyOfSetAndClear(
        TSTRING,  /* <- Set? & Clear? -> */ TINTEGER|TFLOAT|TBOOLEAN,  /* Or */
        TINTEGER, /* <- Set? & Clear? -> */ TSTRING|TFLOAT|TBOOLEAN,   /* Or */
        TFLOAT,   /* <- Set? & Clear? -> */ TSTRING|TINTEGER|TBOOLEAN, /* Or */
        TBOOLEAN, /* <- Set? & Clear? -> */ TSTRING|TINTEGER|TFLOAT);
  }
  /* ----------------------------------------------------------------------- */
  void CheckTypeValidInFlags(const string &strVar,
    const CVarFlagsConst cvfcFlags)
  { // Throw error if the flags do not contain a type or mixed types
    if(!IsTypeValidInFlags(cvfcFlags))
      XC("CVar flags have none or mixed types!",
         "Variable", strVar, "Flags", cvfcFlags.FlagGet());
  }
  /* -- Check that the variable name is valid ------------------------------ */
  bool IsValidVariableName(const string &strVar)
  { // Check minimum name length
    if(strVar.length() < stCVarMinLength || strVar.length() > stCVarMaxLength)
      return false;
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
         "Minimum",  stCVarMinLength,
         "Maximum",  stCVarMaxLength);
  }
  /* ----------------------------------------------------------------------- */
  bool SetInitialVar(const string &strVar, const string &strVal,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst cvcfcFlags)
  { // Check that the variable name is valid.
    if(!IsValidVariableName(strVar))
    { // Throw if theres an error
      if(cvcfcFlags.FlagIsSet(CCF_THROWONERROR))
        XC("CVar name is not valid! Only alphanumeric characters "
           "and underscores are acceptable!",
           "Name",  strVar,         "Value",     strVal,
           "Flags", cvfcFlags.FlagGet(), "Condition", cvcfcFlags.FlagGet());
      // Return falure instead
      return false;
    } // Look if the initial var already exists and if we found it? Set it
    const CVarMapIt cvmiIt{ cvmPending.find(strVar) };
    if(cvmiIt != cvmPending.end())
    { // Ignore overwrite if requested
      if(cvcfcFlags.FlagIsSet(CCF_NOIOVERRIDE))
      { // Log that this action was denied
        cLog->LogWarningExSafe("CVars ignored overriding '$' with '$'!",
          strVar, strVal);
        // Failed
        return false;
      } // Do the set
      SetInitialVar(cvmiIt->second, strVal, cvfcFlags);
    } // Insert into initial list
    else
    { // Check that we can create another variable
      if(cvmPending.size() >= stMaxInactiveCount)
      { // Can we throw error?
        if(cvcfcFlags.FlagIsSet(CCF_THROWONERROR))
          XC("Initial CVar count upper threshold reached!",
             "Variable", strVar, "Maximum", stMaxInactiveCount);
        // Log that this action was denied
        cLog->LogWarningExSafe(
          "CVars not adding '$' because upper threshold of $ reached!",
          strVar, stMaxInactiveCount);
        // Failed
        return false;
      } // Check ok so insert
      cvmPending.insert({ strVar,
        CVarItem{ strVar, strVal, NoOp, cvfcFlags } });
    } // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool SetExistingInitialVar(const string &strVar, const string &strVal,
    const CVarFlagsConst cvfcFlags=PUSR)
  { // Concurrency
    const LockGuard lgCVarsSync{ mMutex };
    // Find initial item and return failure if it doesn't exist
    const CVarMapIt cVarItem{ cvmPending.find(strVar) };
    if(cVarItem == cvmPending.end()) return false;
    // Set the value
    SetInitialVar(cVarItem->second, strVal, cvfcFlags);
    // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  template<typename AnyType>
    void SetSafe(const string &strVar, const AnyType atV)
      { SetSafe(strVar, StrFromNum(atV)); }
  /* ----------------------------------------------------------------------- */
  CVarReturn SetDefaults(const CVarDefaults dcVal)
  { // Compare defaults setting
    switch(dcVal)
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
  const string GetCBError(void) { return StdMove(strCBError); }
  /* ----------------------------------------------------------------------- */
  template<typename AnyType>AnyType GetInternalSafe(const CVarEnums cveId)
    { const LockGuard lgCVarsSync{ mMutex };
      return GetInternal<AnyType>(cveId); }
  /* ----------------------------------------------------------------------- */
  const string GetInitialVarSafe(const string &strVar)
    { const LockGuard lgCVarsSync{ mMutex };
      return GetInitialVar(strVar); }
  /* ----------------------------------------------------------------------- */
  const string &GetInternalStrSafe(const CVarEnums cveId)
    { const LockGuard lgCVarsSync{ mMutex };
      return GetStrInternal(cveId); }
  const char *GetInternalCStrSafe(const CVarEnums cveId)
    { return GetInternalStrSafe(cveId).c_str(); }
  /* -- Return the cvar name's default value as a string ------------------- */
  const string GetDefStrSafe(const LuaCVarMapIt cvmiIt)
    { const LockGuard lgCVarsSync{ mMutex };
      return GetDefStr(cvmiIt); }
  /* -- Return the cvar name's value as a string --------------------------- */
  const string GetStrSafe(const string &strVar)
    { const LockGuard lgCVarsSync{ mMutex };
      return GetStr(strVar); }
  const string GetStrSafe(const LuaCVarMapIt cvmiIt)
    { const LockGuard lgCVarsSync{ mMutex };
      return GetStr(cvmiIt); }
  /* ----------------------------------------------------------------------- */
  bool IsVarStrEmptySafe(const LuaCVarMapIt cvmiIt)
    { const LockGuard lgCVarsSync{ mMutex };
      return IsVarStrEmpty(cvmiIt); }
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
    const CVarMapItConst cvmiIt{ cvmActive.find(strVar) };
    return cvmiIt == cvmActive.cend() ?
      "<invalid>" : cvmiIt->second.Protect();
  }
  /* ----------------------------------------------------------------------- */
  size_t MarkAllEncodedVarsAsCommit(void)
  { // Thread safety
    const LockGuard lgCVarsSync{ mMutex };
    // Total number of commits
    SafeSizeT stCommitted{0};
    // Enumerate the initial list and cvar list asyncronously
    StdForEach(par_unseq, cvlList.cbegin(), cvlList.cend(),
      [&stCommitted](const CVarMapMulti &impStruct)
      { // Enumerate the cvars in each list asynchronously
        StdForEach(par_unseq, impStruct.cmMap.begin(), impStruct.cmMap.end(),
          [&stCommitted](CVarMapPair &cvmpPair)
            { if(cvmpPair.second.MarkEncodedVarAsCommit()) ++stCommitted; });
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
    StdForEach(par_unseq, cvlList.cbegin(), cvlList.cend(),
      [&stCommitTotal, &stPurgeTotal](const CVarMapMulti &cvmpIt)
    { // Total number of commits attempted which may need to be read and
      // written by multiple threads.
      SafeSizeT stCommit{0}, stPurge{0};
      // Iterate through the initial list and try to commit the cvar. We might
      // as well try to do this as quick as possible
      StdForEach(par_unseq, cvmpIt.cmMap.begin(), cvmpIt.cmMap.end(),
        [&stCommit, &stPurge](CVarMapPair &cvmpPair)
          { cvmpPair.second.Save(stCommit, stPurge); });
      // Log variables written
      if(stCommit || stPurge)
        cLog->LogInfoExSafe("CVars commited $ and purged $ from $ pool.",
          stCommit.load(), stPurge.load(), cvmpIt.cpName);
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
  { // Find var and return empty string or the var
    const CVarMapIt cvmiIt{ cvmPending.find(strKey) };
    return cvmiIt != cvmPending.cend() ? cvmiIt->second.GetValue() : strO;
  }
  /* ----------------------------------------------------------------------- */
  size_t Clean(void)
  { // Get all key names in cvars, no values
    if(!cSql->CVarReadKeys()) return 0;
    // Get vars list
    const Sql::Result &srVars = cSql->GetRecords();
    if(srVars.empty())
    { // Log message and return failed
      cLog->LogInfoSafe("CVars found no variables to clean up!");
      return 0;
    } // Thread safety to protect cvar modification
    const LockGuard lgCVarsSync{ mMutex };
    // Say how many records we're probing
    cLog->LogDebugExSafe("CVars probing $ records...", srVars.size());
    // Begin transaction
    cSql->Begin();
    // Transactions committed
    size_t stCommit = 0;
    // For each record returned.
    for(const Sql::Records &srPairs : srVars)
    { // Get key and goto next record if not found, else assign cvar name
      const Sql::RecordsIt smmI{ srPairs.find("K") };
      if(smmI == srPairs.cend()) continue;
      const Sql::DataListItem &mbO = smmI->second;
      // If is not a string?
      if(mbO.iT != SQLITE_TEXT)
      { // Delete it from database and add to counter if ok and goto next cvar
        if(cSql->CVarPurgeData(mbO.MemPtr<char>(), mbO.MemSize()) ==
             Sql::PR_OK)
          ++stCommit;
        continue;
      } // Get string and find cvar name in live cvar list, ignore if it exists
      const string strKey{ mbO.MemToString() };
      if(VarExists(strKey)) continue;
      // Find item in initial/standby cvar list, delete it if found
      const CVarMapIt cvmiIt{ cvmPending.find(strKey) };
      if(cvmiIt != cvmPending.end()) cvmPending.erase(cvmiIt);
      // Delete from database and proceed to the next record and if failed?
      if(cSql->CVarPurge(strKey) != Sql::PR_OK) continue;
      // Number of transactions send to sqlite
      ++stCommit;
    } // End bulk transaction commital
    cSql->End();
    // If we have items to delete, we can say how many we deleted
    cLog->LogInfoExSafe("CVars removed $ of $ orphan records.",
      stCommit, srVars.size());
    // Clean up downloaded records
    cSql->Reset();
    // Return number of rows affected
    return stCommit;
  }
  /* ----------------------------------------------------------------------- */
  size_t LoadFromDatabase(void)
  { // Return if table is not already created or not available
    if(cSql->CVarCreateTable() != Sql::CTR_OK_ALREADY || !cSql->CVarReadAll())
      return 0;
    // Number of variables loaded
    SafeSizeT stLoaded{ 0 };
    // Get vars list and if not empty
    const Sql::Result &srVars = cSql->GetRecords();
    if(!srVars.empty())
    { // Log number of vars being parsed
      cLog->LogDebugExSafe("CVars read $ persistent variables, parsing...",
        srVars.size());
      // For each record returned. Set each keypair returned, these are user
      // variables. We're using multithreading for this to accellerate
      // decryption and decompression routines.
      StdForEach(par_unseq, srVars.cbegin(), srVars.cend(),
        [this, &stLoaded](const Sql::Records &smmPairs)
      { // Get key and goto next record if not found, else set the key string
        const Sql::RecordsIt iRecKey{ smmPairs.find("K") };
        if(iRecKey == smmPairs.cend()) return;
        const Sql::DataListItem &mbKey = iRecKey->second;
        if(mbKey.iT != SQLITE_TEXT) return;
        // Convert the variable to string and ignore if invalid
        const string strVar{ mbKey.MemToString() };
        // Get flags and goto next record if not found, else set the key string
        const Sql::RecordsIt iRecFlags{ smmPairs.find("F") };
        if(iRecFlags == smmPairs.cend()) return;
        const Sql::DataListItem &mbFlags = iRecFlags->second;
        if(mbFlags.iT != SQLITE_INTEGER) return;
        const SqlCVarDataFlagsConst
          scfFlags{ mbFlags.MemReadInt<sqlite_int64>() };
        // Get value and goto next record if not found, else set the value
        const Sql::RecordsIt iRecVal{ smmPairs.find("V") };
        if(iRecVal == smmPairs.cend()) return;
        const Sql::DataListItem &mbVal = iRecVal->second;
        // If the data pointed at 'V' is not encrypted?
        if(scfFlags.FlagIsClear(LF_ENCRYPTED))
        { // Must be text
          if(mbVal.iT != SQLITE_TEXT)
          { // Show warning and ignore if not
            cLog->LogWarningExSafe("CVars expected SQLITE_TEXT<$> "
              "and not type $ for '$'!", SQLITE_TEXT, mbVal.iT, strVar);
            return;
          } // Store value directly with synchronisation and goto next
          if(SetVarOrInitialSafe(strVar, mbVal.MemToString(),
            PUSR|SUDB, CCF_NOIOVERRIDE|CCF_NOTDECRYPTED))
              ++stLoaded;
          return;
        } // New decrypted value to write into
        string strNewValue;
        // Must be binary
        if(mbVal.iT != SQLITE_BLOB) return;
        // Capture exceptions because MagicBlock does that on error
        try
        { // Decrypt the value and get the result, and if that call fails?
          strNewValue = Block<CoDecoder>{ mbVal }.MemToString();
        } // exception occured?
        catch(const exception &e)
        { // Log failure and try to reset the initial var so this does not
          // happen again and goto next record
          return cLog->LogErrorExSafe(
            "CVars variable '$' decrypt exception: $", strVar, e.what());
        }
        // Set the variable or place it in the initials list if cvar not not
        // registered. The access will be user mode only and assignments
        // are not to be marked as changed. Do not throw on error because it
        // is not easy to change a sql database manually if we change the
        // rules on a cvar.
        if(SetVarOrInitialSafe(strVar, strNewValue, PUSR,
          CCF_NOIOVERRIDE|CCF_DECRYPTED))
            ++stLoaded;
      });
      // If we loaded all the variables? Report that we loaded all the vars
      if(stLoaded == srVars.size())
        cLog->LogInfoExSafe("CVars parsed all $ persistent variables.",
          srVars.size());
      // We didn't load all the variables? Report that we could not load all
      // the variables
      else cLog->LogWarningExSafe(
        "CVars only parsed $ of $ persistent variables!",
        stLoaded.load(), srVars.size());
    } // No variables loaded
    else cLog->LogInfoSafe("CVars found no variables to parse.");
    // Clean up downloaded records
    cSql->Reset();
    // Return loaded variables
    return stLoaded;
  }
  /* ----------------------------------------------------------------------- */
  CVarReturn SetDisplayFlags(const CVarShowFlagsType cstFlags)
  { // Failed if flags are not valid
    if(cstFlags != CSF_NONE && (cstFlags & ~CSF_MASK)) return DENY;
    // Set the new flags
    csfShowFlags.FlagReset(cstFlags);
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
  /* -- Set maximum offline cvar count ------------------------------------- */
  CVarReturn MaxICountModified(const size_t stCount)
    { return CVarSimpleSetInt(stMaxInactiveCount, stCount); }
  /* -- Set and execute default app configuration file --------------------- */
  CVarReturn ExecuteAppConfig(const string &strC, string &strV)
  { // Build filename and deny change if failed
    const string strFN{ StrAppend(strC, "." CFG_EXTENSION) };
    if(!LoadFromFile(strFN, PSYSTEM|SAPPCFG)) return DENY;
    // We are manually updating the value with the correct filename
    strV = StdMove(strFN);
    return ACCEPT_HANDLED;
  }
  /* ----------------------------------------------------------------------- */
  CVarReturn SetCompatFlags(const bool bEnabled)
  { // Compatibility flags are enabled?
    if(bEnabled)
    { // If only using one cpu and log if we can disable window threading
      if(cSystem->CPUCount() == 1 &&
        SetInternalSafe<bool>(WIN_THREAD, false) == CVS_OK)
          cLog->LogWarningSafe("CVars force disabled window threading due "
            "to only having one thread!");
    } // CVar is accepted
    return ACCEPT;
  }
  /* ----------------------------------------------------------------------- */
  void DeInit(void)
  { // Save all variables
    Save();
    // Log result then dereg core variables, they don't need testing
    cLog->LogDebugExSafe("CVars unregistering $ core variables...",
      cvEngList.size());
    // Unregister each built-in engine cvar
    for(const ItemStatic &cvaR : cvEngList)
    { // If the variable falls into the current gui mode then unregister it
      if(cSystem->IsCoreFlagsHave(cvaR.cfcRequired))
        UnregisterVarNoLog(cvaR.strVar);
    } // Log if everything was unregistered
    if(cvmActive.empty())
      cLog->LogInfoSafe("CVars finished unregistering all the variables.");
    // Impossible but log just incase
    else cLog->LogWarningExSafe("CVars has $ lingering variables!",
      cvmActive.size());
  }
  /* ----------------------------------------------------------------------- */
  void Init(void)
  { // Must have default variables to register
    if(cvEngList.empty()) XC("No default variables to register!");
    // Register each cvar
    cLog->LogDebugExSafe("CVars registering $ built-in variables...",
      cvEngList.size());
    for(size_t stIndex = 0; stIndex < cvEngList.size(); ++stIndex)
    { // Get cvar
      const ItemStatic &cvaR = cvEngList[stIndex];
      // Register the variable if the guimode is valid. Note that we might not
      // get the gui mode which is default 0, but when it is set, it will set
      // &guimId automatically (since it was passed as ref by core).
      avInternal[stIndex] = cSystem->IsCoreFlagsHave(cvaR.cfcRequired) ?
        RegisterVar(cvaR.strVar, cvaR.strValue, cvaR.cbTrigger,
          cvaR.cFlags, CCF_NOTHING) : cvmActive.end();
    } // Finished
    cLog->LogInfoExSafe("CVars registered $ of $ built-in variables for "
      "$ mode $.", cvmActive.size(), cvEngList.size(),
      cSystem->GetCoreFlagsString(), cSystem->GetCoreFlags());
  }
  /* -- Default constructor ------------------------------------------------ */
  explicit CVars(const ItemStaticList &cvDefEngList) :
    /* -- Initialisers ----------------------------------------------------- */
    stMaxInactiveCount(CVAR_MAX),      // Initially set to max cvar count
    cvlList{{                          // Set combined lists
      { cvmPending, "unregistered" },  // Inactive cvars list
      { cvmActive,  "registered" } }}, // Active cvars list
    cvEngList{ cvDefEngList }          // Default engine cvars list
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~CVars, DeInit())         // Save and clean-up all variables
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CVars)               // Disable copy constructor and operator
  /* ----------------------------------------------------------------------- */
} *cCVars = nullptr;                   // Pointer to static class
/* == Cvar lua callback ==================================================== */
CVarReturn CVars::LuaCallbackStatic(CVarItem &cvarD, const string &strVal)
  { return cCVars->LuaCallback(cvarD, strVal); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
