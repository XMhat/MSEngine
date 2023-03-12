/* == CVAR.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles the logic behind a single cvar.                 ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfCVarDef {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfSql;                 // Using sql namespace
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(CVarShow,
  /* ----------------------------------------------------------------------- */
  // Show no confidential cvars        Show private cvars
  SF_NONE                {0x00000000}, SF_CONFIDENTIAL          {0x00000001},
  // Show protected cvars
  SF_PROTECTED           {0x00000002}
);/* ----------------------------------------------------------------------- */
static CVarShowFlags sfShowFlags{SF_NONE}; // Console cvar display flags
/* == Notes ================================================================ */
/* ######################################################################### */
/* ## We cannot define the CVar library now because we have not compiled  ## */
/* ## the rest of the engine yet so we define this namespace so we can    ## */
/* ## setup the ability for the CVars manager to access the CVars here    ## */
/* ## and give us the ability to define the CVar library later on         ## */
/* ######################################################################### */
/* -- Default no-op for cvar lib callback functions ------------------------ */
static CVarReturn NoOp(Item&, const string&) { return ACCEPT; }
/* ------------------------------------------------------------------------- */
enum CVarSetEnums                      // Cvar set return codes
{ /* ----------------------------------------------------------------------- */
  CVS_OK,                              // Parameter set successful
  CVS_OKNOTCHANGED,                    // Parameter set successful, no change
  CVS_NOTFOUND,                        // Parameter not found
  CVS_NOTWRITABLE,                     // Parameter set is not writable
  CVS_NOTINTEGER,                      // Parameter set was not an integer
  CVS_NOTFLOAT,                        // Parameter set was not a float
  CVS_NOTBOOLEAN,                      // Parameter set was not a boolean
  CVS_NOTUNSIGNED,                     // Parameter set was not >= 0
  CVS_NOTPOW2,                         // Parameter set was not power of two
  CVS_NOTPOW2Z,                        // Same as above, but not zero too
  CVS_NOTALPHA,                        // Must contain only letters
  CVS_NOTNUMERIC,                      // Must contain only digits
  CVS_NOTALPHANUMERIC,                 // Must contain only alphanumerics
  CVS_NOTFILENAME,                     // Must be a valid filename
  CVS_TRIGGERDENIED,                   // Trigger callback denied change
  CVS_TRIGGEREXCEPTION,                // exception occured in trigger
  CVS_EMPTY,                           // Parameter set was an empty string
  CVS_NOTYPESET,                       // No type is set
};/* ----------------------------------------------------------------------- */
BUILD_FLAGS(CVarCondition,
  /* ----------------------------------------------------------------------- */
  // No flags specified?               Unlock then lock on callback trigger?
  CSC_NOTHING            {0x00000000}, CSC_SAFECALL             {0x00000001},
  // Don't throw if var missing?       Throw if there is an error?
  CSC_IGNOREIFMISSING    {0x00000002}, CSC_THROWONERROR         {0x00000004},
  // Variable was just registered?   Do not override existing initial var?
  CSC_NEWCVAR            {0x00000008}, CSC_NOIOVERRIDE          {0x00000010},
  // The variable was decrypted?     The variable was not encrypted?
  CSC_DECRYPTED          {0x00000020}, CSC_NOTDECRYPTED         {0x00000040},
  // Do not mark as commit?
  CSC_NOMARKCOMMIT       {0x00000080}
);/* -- Cvar item class ---------------------------------------------------- */
class Item :                           // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public CVarFlags                     // Cvar configuration settings
{ /* -- Private typedefs --------------------------------------------------- */
  typedef int64_t ValueIntType;        // When handling integral values
  /* -- Private variables -------------------------------------------------- */
  const string   strVar;               // Variable name
  string         strValue;             // Value name
  string         strDefValue;          // Default value name
  CbFunc         cbTrigger;            // Callback trigger event
  /* --------------------------------------------------------------- */ public:
  const CbFunc &GetTrigger(void) const { return cbTrigger; }
  /* ----------------------------------------------------------------------- */
  void SetTrigger(const CbFunc &cbT) { cbTrigger = cbT; }
  /* ----------------------------------------------------------------------- */
  bool IsTriggerSet(void) const { return GetTrigger() == NoOp; }
  /* ----------------------------------------------------------------------- */
  const string &GetVar(void) const { return strVar; }
  /* ----------------------------------------------------------------------- */
  const string &GetDefValue(void) const { return strDefValue; }
  /* ----------------------------------------------------------------------- */
  size_t GetDefLength(void) const { return GetDefValue().length(); }
  /* ----------------------------------------------------------------------- */
  string &GetModifyableValue(void) { return strValue; }
  /* ----------------------------------------------------------------------- */
  const string &GetValue(void) const { return strValue; }
  /* ----------------------------------------------------------------------- */
  size_t GetValueLength(void) const { return GetValue().length(); }
  /* ----------------------------------------------------------------------- */
  void PruneValue(void) { GetModifyableValue().shrink_to_fit(); }
  /* ----------------------------------------------------------------------- */
  bool IsValueUnset(void) const { return GetValue().empty(); }
  /* ----------------------------------------------------------------------- */
  bool IsValueSet(void) const { return !IsValueUnset(); }
  /* ----------------------------------------------------------------------- */
  bool IsValueChanged(void) const { return GetValue() != GetDefValue(); }
  /* ----------------------------------------------------------------------- */
  bool IsValueUnchanged(void) const { return !IsValueChanged(); }
  /* ----------------------------------------------------------------------- */
  void SetDefault(void) { strValue = GetDefValue(); }
  /* ----------------------------------------------------------------------- */
  void SetDefValue(const string &strN) { strDefValue = strN; }
  /* ----------------------------------------------------------------------- */
  void SetDefValue(string &&strN) { strDefValue = std::move(strN); }
  /* ----------------------------------------------------------------------- */
  void SetValue(const string &strV) { strValue = strV; }
  /* ----------------------------------------------------------------------- */
  void SetValue(string &&strV) { strValue = std::move(strV); }
  /* ----------------------------------------------------------------------- */
  const string Protect(void) const
  { // If confidential, return confidential
    if(FlagIsSet(CONFIDENTIAL) && sfShowFlags.FlagIsClear(SF_CONFIDENTIAL))
      return "<Private>";
    // If protected, return protected
    if(FlagIsSet(CPROTECTED) && sfShowFlags.FlagIsClear(SF_PROTECTED))
      return "<Protected>";
    // If value is empty, return as empty
    if(IsValueUnset()) return "<Empty>";
    // If is a float, return value
    if(FlagIsSet(TFLOAT)) return ToString(ToNumber<double>(GetValue()), 0, 12);
    // Is a boolean
    if(FlagIsSet(TBOOLEAN))
      return ToString(TrueOrFalse(ToNumber<bool>(GetValue())));
    // If is a integer, return number + hex
    if(FlagIsSet(TINTEGER))
    { // Get value as 64-bit integer
      const ValueIntType iV = ToNumber<ValueIntType>(GetValue());
      return Format("$ [0x$$]", iV, hex, iV);
    } // Unknown value or string. Return as-is.
    return Format("\"$\"", GetValue());
  }
  /* ----------------------------------------------------------------------- */
  bool MarkEncodedVarAsCommit(void)
  { // Ignore if appropriate flag not set
    if(FlagIsClear(CPROTECTED|CDEFLATE)) return false;
    // Mark as commit
    FlagSet(COMMIT);
    // Commit the cvar and increment count if succeeded. 'this->'
    // needed to stop clang whining about 'this' not being used but it
    // is needed!
    return Commit() == CR_OK;
  }
  /* ----------------------------------------------------------------------- */
  bool Commit(const DataConst &dcVal)
    { return cSql->CVarCommitBlob(GetVar(), dcVal); }
  /* --------------------------------------------------------------------- */
  enum CommitResult                    // Result to a commit request
  { /* --------------------------------------------------------------------- */
    CR_OK,                             // Sql call commited the variable
    CR_FAIL,                           // Sql call update cvar failed
    CR_OK_PURGE,                       // Sql call succeeded with purge
    CR_FAIL_PURGE,                     // Sql call failed purge
    CR_FAIL_PURGE_NOT_CHANGED,         // Sql call succeded but no changes
    CR_FAIL_PURGE_UNKNOWN_ERROR,       // Unknown result from sql purge call
    CR_FAIL_NOT_SAVEABLE,              // CVar is not saveable
    CR_FAIL_LOADED_NOT_MODIFIED,       // Cvar loaded but not modified
    CR_FAIL_ENCRYPT,                   // Cvar could not be encrypted
    CR_FAIL_COMPRESS,                  // Cvar could not be compress
  };/* --------------------------------------------------------------------- */
  CommitResult Commit(void)
  { // Ignore if variable not modified, force saved or loaded
    if(FlagIsClear(COMMIT|OSAVEFORCE|LOADED)) return CR_FAIL_NOT_SAVEABLE;
    // If the value is the same as the default value?
    if(IsValueUnchanged())
    { // Purge it, no point in writing it. Compare result
      switch(cSql->CVarPurge(GetVar()))
      { case Sql::PR_OK    : return CR_OK_PURGE;
        case Sql::PR_FAIL  : return CR_FAIL_PURGE;
        case Sql::PR_OK_NC : return CR_FAIL_PURGE_NOT_CHANGED;
        default            : return CR_FAIL_PURGE_UNKNOWN_ERROR;
      }
    } // Nothing to write if variable was just loaded
    else if(FlagIsSet(LOADED) && FlagIsClear(COMMIT|OSAVEFORCE))
      return CR_FAIL_LOADED_NOT_MODIFIED;
    // Value different from default?
    else
    { // If we are to encrypt
      if(FlagIsSet(CPROTECTED)) try
      { // Try encryption and/or compression and store result in database
        if(FlagIsSet(CDEFLATE))
          Commit(Block<AESZLIBEncoder>(GetValue()));
        else
          Commit(Block<AESEncoder>(GetValue()));
      } // exception occured
      catch(const exception &e)
      { // Log exception
        cLog->LogErrorExSafe("CVars encrypt exception: $", e.what());
        // Capture exceptions again
        try
        { // Try raw encoder and return string
          Commit(Block<RAWEncoder>(GetValue()));
        } // exception occured again?
        catch(const exception &e2)
        { // Log exception and return failure
          cLog->LogErrorExSafe("CVars store exception: $", e2.what());
          return CR_FAIL_ENCRYPT;
        } // Success
      } // If we are to compress
      else if(FlagIsSet(CDEFLATE)) try
      { // Try compression and commit the result to the database
        Commit(Block<ZLIBEncoder>(GetValue()));
      } // exception occured
      catch(const exception &e)
      { // Log exception
        cLog->LogErrorExSafe("CVars compress exception: $", e.what());
        // Capture exceptions again
        try
        { // Try raw encoder and return string
          Commit(Block<RAWEncoder>(GetValue()));
        } // exception occured again?
        catch(const exception &e2)
        { // Log exception and return failure
          cLog->LogErrorExSafe("CVars store exception: $", e2.what());
          return CR_FAIL_COMPRESS;
        } // Success
      } // Commit the unencrypted cvar and return if failed
      else if(!cSql->CVarCommitString(GetVar(), GetValue())) return CR_FAIL;
    } // Successfully saved so remove commit flag
    FlagClear(COMMIT);
    // Success
    return CR_OK;
  }
  /* -- Save with counter increment ---------------------------------------- */
  template<typename T>void Save(T &stCommit, T &stPurge)
  { // Try to commit the cvar. 'this->' needed to stop clang whining about
    // 'this' not being used but it is needed!
    switch(Commit())
    { // enum CommitResult
      case CR_OK       : stCommit = stCommit + 1; break; // Saved
      case CR_OK_PURGE : stPurge = stPurge + 1; break;   // Purged
      default          : break;                          // Error
    }
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums HandleCallbackException(const char*const cpWhat,
    const CVarConditionFlagsConst &scFlags, const string &strNValue,
    string &strCBError)
  { // Throw exception if requested
    if(scFlags.FlagIsSet(CSC_THROWONERROR))
      XC(cpWhat, "Variable", GetVar(), "Value", strNValue);
    // Save the error string because we have no access to the console
    strCBError = Append("CVar CB failed! > ", cpWhat);
    // Return error
    return CVS_TRIGGEREXCEPTION;
  }
  /* --------------------------------------------------------------------- */
  CVarSetEnums SetValue(const string &strNValue,
    const CVarConditionFlagsConst &scFlags, mutex &mLock, string &strCBError)
  { // If value is equal as current value then ignore it. We'll allow the
    // change when setting new vars because the triggers should trigger
    if(strNValue == GetValue() && scFlags.FlagIsClear(CSC_NEWCVAR))
      return CVS_OKNOTCHANGED;
    // Result storage
    CVarReturn cbrResult;
    // Mutex unlock/lock helper class to temporarily unlock and then relock
    // a mutex class.
    class MutexRelockHelper                // Members initially private
    { mutex &mMutex;                       // Reference to mutex to use
      public: explicit MutexRelockHelper(mutex &mMutexRef) : mMutex(mMutexRef)
        { mMutex.unlock(); }
      ~MutexRelockHelper(void) { mMutex.lock(); }
    }; // If it is a lua cvar?
    if(FlagIsSet(TLUA))
    { // Lock the cvar from being unregistered
      FlagSet(LOCKED);
      // If this is a safe call? Capture exceptions so we can clean up
      if(scFlags.FlagIsSet(CSC_SAFECALL)) try
      { // Temporarily unlock mutex so cvars can continue to be manipulated
        const MutexRelockHelper mrhUnlockRelock{ mLock };
        // Call the trigger and capture the result of the callback
        cbrResult = GetTrigger()(*this, strNValue);
        // Relock the mutex so further changes can be made
      } // exception occured
      catch(const exception &E)
      { // Remove the lock on the cvar
        FlagClear(LOCKED);
        // Handle exception and return error if no exception
        return HandleCallbackException(E.what(), scFlags, strNValue,
          strCBError);
      } // This is not a safe call? Capture exceptions so we can clean up
      else try
      { // Lock the cvar from being unregistered
        FlagSet(LOCKED);
        // Call the trigger and capture the result of the callback
        cbrResult = GetTrigger()(*this, strNValue);
      } // exception occured
      catch(const exception &E)
      { // Remove the lock on the cvar
        FlagClear(LOCKED);
        // Handle exception and return error if no exception
        return HandleCallbackException(E.what(), scFlags, strNValue,
          strCBError);
      } // Remove the lock on the cvar
      FlagClear(LOCKED);
    } // This is not a LUA callback, so if this is a safecall?
    else if(scFlags.FlagIsSet(CSC_SAFECALL)) try
    { // Temporarily unlock mutex so cvars can continue to be manipulated
      const MutexRelockHelper mrhUnlockRelock{ mLock };
      // Call the trigger and capture the result of the callback
      cbrResult = GetTrigger()(*this, strNValue);
    } // exception occured
    catch(const exception &E)
    { // Handle exception and return error if no exception
      return HandleCallbackException(E.what(), scFlags, strNValue,
        strCBError);
    } // Lua callback and not a safe call. Capture exceptions so we can clean
    else try
    { // Call the trigger and capture the result of the callback
      cbrResult = GetTrigger()(*this, strNValue);
    } // exception occured
    catch(const exception &E)
    { // Handle exception and return error if no exception
      return HandleCallbackException(E.what(), scFlags, strNValue,
        strCBError);
    } // If triggered denied to set the var? DON'T REARRANGE THESE!
    switch(cbrResult)
    { // The change was not allowed?
      case DENY:
      { // Throw if requested
        if(scFlags.FlagIsSet(CSC_THROWONERROR))
          XC("CVar callback denied change!",
             "Variable", GetVar(), "Value", strNValue);
        // Otherwise return trigger denied code
        return CVS_TRIGGERDENIED;
      } // The new value is acceptable?
      case ACCEPT:
      { // Set the new value and fall through
        SetValue(strNValue);
      } // The new value is acceptable, but the caller set the value?
      case ACCEPT_HANDLED:
      { // Throw to commit
        break;
      } // Same as above but forcing commit?
      case ACCEPT_HANDLED_FORCECOMMIT:
      { // Set commit flag as requested by the caller
        FlagSet(COMMIT);
        // Done
        break;
      } // Unknown return value?
      default:
      { // Throw if requested
        if(scFlags.FlagIsSet(CSC_THROWONERROR))
          XC("CVar callback returned unknown value!",
             "Variable", GetVar(), "Value", strNValue, "Result", cbrResult);
        // Otherwise return trigger denied code
        return CVS_TRIGGEREXCEPTION;
      }
    } // Free unused memory from cvar
    PruneValue();;
    // If decryption didn't fail and the no mark commit flag is set?
    if(scFlags.FlagIsClear(CSC_NOTDECRYPTED|CSC_NOMARKCOMMIT))
    { // CVar is saveable?
      if(FlagIsSet(CSAVEABLE))
      { // Set commit flag if the cvar was not just registered or it was new
        // and initialised from the command line
        if(scFlags.FlagIsClear(CSC_NEWCVAR) || FlagIsSet(SCMDLINE))
          FlagSet(COMMIT);
      } // Remove commit if set
      else if(FlagIsSet(COMMIT)) FlagClear(COMMIT);
    } // Log progress and return success
    cLog->LogDebugExSafe("CVars $ '$' to $.",
      scFlags.FlagIsSet(CSC_NEWCVAR) ? "registered" : "set",
      GetVar(), Protect());
    // Return success
    return CVS_OK;
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums SetValue(const string &strNValue, const CVarFlagsConst &cAcc,
    const CVarConditionFlagsConst &scFlags, mutex &mLock, string &strCBError)
  { // Failed if not writable
    if(FlagIsClear(cAcc))
    { // If we should not abort? Just return error else throw exception
      if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTWRITABLE;
      XC("CVar is not writable from this scope!",
         "Variable", GetVar(),     "Value", strNValue,
         "Scope",    cAcc.FlagGet(), "Flags", FlagGet());
    } // If integer?
    if(FlagIsSet(TINTEGER))
    { // If number begins with '0x' to denote hexadecimal? Convert specified
      // value from hexadecimal to decimal and restart the call with the
      // converted value.
      if(strNValue.length() > 2 && strNValue[0] == '0' &&
                                   strNValue[1] == 'x')
        return SetValue(ToString(HexToNumber<ValueIntType>
          (strNValue.substr(2))), scFlags, mLock, strCBError);
      // If specified value is not a valid integer?
      if(!IsNumber(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTINTEGER;
        XC("CVar specified is not a valid integer!",
           "Variable", GetVar(), "Value", strNValue);
      } // Deny negative values if unsigned only needed
      else if(FlagIsSet(CUNSIGNED) && strNValue.front() == '-')
      { // If we should not abort? Just return error else throw exception
        if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTUNSIGNED;
        XC("CVar specified must be an unsigned integer!",
           "Variable", GetVar(), "Value", strNValue);
      } // Deny non-power of two numbers?
      else if(FlagIsSet(CPOW2) && !IsNumberPOW2(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTPOW2;
        XC("CVar specified must be power of two!",
           "Variable", GetVar(), "Value", strNValue);
      } // Deny non-power of two numbers but allow zero?
      else if(FlagIsSet(CPOW2Z) && !IsNumberPOW2Zero(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTPOW2Z;
        XC("CVar specified must be power of two or zero!",
           "Variable", GetVar(), "Value", strNValue);
      } // Next step
      return SetValue(strNValue, scFlags, mLock, strCBError);
    } // If float? Bail if not a floating point number
    if(FlagIsSet(TFLOAT))
    { // If specified value is not a valid floating point number?
      if(!IsFloat(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTFLOAT;
        XC("CVar specified is not a valid number!",
           "Variable", GetVar(), "Value", strNValue);
      } // Deny negative values if unsigned only needed
      else if(FlagIsSet(CUNSIGNED) && strNValue.front() == '-')
      { // If we should not abort? Just return error else throw exception
        if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTUNSIGNED;
        XC("CVar specified must be a non-negative float!",
           "Variable", GetVar(), "Value", strNValue);
      } // Next step
      return SetValue(strNValue, scFlags, mLock, strCBError);
    } // Is a boolean?
    if(FlagIsSet(TBOOLEAN))
    { // Must be one byte, then test first character
      if(strNValue.size() == 1) switch(strNValue[0])
      { // Is zero or one? OK
        case '0': case '1':
          return SetValue(strNValue, scFlags, mLock, strCBError);
        // Invalid value
        default: break;
      } // If we should not abort? Just return error else throw exception
      if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTBOOLEAN;
      XC("CVar specified is not a valid boolean!",
         "Variable", GetVar(), "Value", strNValue);
    } // Is a string?
    if(FlagIsSet(TSTRING))
    { // Trim string if setting requests it and get new result
      const string &strNewValue =
        FlagIsSet(MTRIM) ? Trim(strNValue, ' ') : strNValue;
      // String cannot be empty and string is empty?
      if(FlagIsSet(CNOTEMPTY) && strNewValue.empty())
      { // If we should not abort? Just return error else throw exception
        if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_EMPTY;
        XC("CVar specified cannot be empty!", "Variable", GetVar());
      } // Check if valid untrusted pathname required
      if(FlagIsSet(CFILENAME))
      { // Check filename and get result
        switch(const ValidResult vRes =
          DirValidName(strNewValue, VT_UNTRUSTED))
        { // Break if ok or empty
          case VR_OK: case VR_EMPTY: break;
          // Show error otherwise
          default : if(scFlags.FlagIsClear(CSC_THROWONERROR))
                      return CVS_NOTFILENAME;
                    XC("CVar untrusted path name is invalid!",
                       "Reason",   DirValidNameResultToString(vRes),
                       "Result",   vRes,
                       "Variable", GetVar());
        }
      } // Check if valid trusted pathname required
      if(FlagIsSet(CTRUSTEDFN))
      { // Check filename and get result
        switch(const ValidResult vRes = DirValidName(strNewValue, VT_TRUSTED))
        { // Break if ok or empty
          case VR_OK: case VR_EMPTY: break;
          // Show error otherwise
          default : if(scFlags.FlagIsClear(CSC_THROWONERROR))
                      return CVS_NOTFILENAME;
                    XC("CVar trusted path name is invalid!",
                       "Reason",   DirValidNameResultToString(vRes),
                       "Result",   vRes,
                       "Variable", GetVar());
        }
      } // Alpha characters only?
      if(FlagIsSet(CALPHA))
      { // And numeric characters?
        if(FlagIsSet(CNUMERIC) && !IsAlphaNumeric(strNewValue))
        { // If we should not abort? Just return error else throw exception
          if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTALPHANUMERIC;
          XC("CVar specified must only contain alphanumeric characters!",
             "Variable", GetVar());
        } // Only letters?
        if(!IsAlpha(strNewValue))
        { // If we should not abort? Just return error else throw exception
          if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTALPHA;
          XC("CVar specified must only contain letters!",
             "Variable", GetVar());
        }
      } // Must only contain numbers
      else if(FlagIsSet(CNUMERIC) && !IsNumber(strNewValue))
      { // If we should not abort? Just return error else throw exception
        if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTNUMERIC;
        XC("CVar specified must only contain numeric characters!",
           "Variable", GetVar());
      } // Next step
      return SetValue(strNewValue, scFlags, mLock, strCBError);
    } // If we should not throw error? Just return code else throw exception
    if(scFlags.FlagIsClear(CSC_THROWONERROR)) return CVS_NOTYPESET;
    XC("CVar type is not set!",
       "Variable", GetVar(),   "NewValue", strNValue,
       "OldValue", GetValue(), "Flags",    FlagGet());
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums ResetValue(const CVarFlagsConst &cAcc,
    const CVarConditionFlagsConst &scFlags, mutex &mLock, string &strCBError)
      { return SetValue(strDefValue, cAcc, scFlags, mLock, strCBError); }
  /* -- Move constructor --------------------------------------------------- */
  Item(Item &&ciOther) :               // Other item
    /* -- Initialisation of members ---------------------------------------- */
    CVarFlags{ ciOther },              // Copy flags over
    strVar{                            // Variable
      std::move(ciOther.GetVar()) },        // Move variable
    strValue{                          // Value
      std::move(ciOther.GetValue()) },      // Move value
    strDefValue{                       // Default value
      std::move(ciOther.GetDefValue()) },   // Move default value
    cbTrigger{                         // Trigger
      std::move(ciOther.GetTrigger()) }     // Move trigger
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor -------------------------------------------------------- */
  Item(const string &strKey,           // Variable name
       const string &strVal,           // Value name
       const CbFunc &cbT,              // Trigger function
       const CVarFlagsConst &cF) :     // CVar flags
    /* -- Initialisation of members ---------------------------------------- */
    CVarFlags{ cF },                   // Initialise cvars flags
    strVar{ strKey },                  // Initialise variable name
    strValue{ strVal },                // Initialise new value
    strDefValue{ strVal },             // Initialise default value
    cbTrigger{ cbT }                   // Initialise trigger function
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Other -------------------------------------------------------------- */
  DELETECOPYCTORS(Item);               // No copy constructor
};/* ----------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
