/* == CMDLINE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This static class stores command line arguments and assists in      ## */
/* ## restarting the engine when needed.                                  ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfCmdLine {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfDir;                 // Using util namespace
using namespace IfSysUtil;             // Using system utility namespace
/* -- Command line helper class (should be the first global to inti) ------- */
static struct CmdLine final            // Members initially public
{ /* -- Public typedefs ---------------------------------------------------- */
  enum class EcId { Quit,              // Quit normally
                    RestartNoParam,    // Restart without parameters
                    RestartNoParamUI,  // Same as above but in UI mode
                    Restart,           // Restart with parameters
                    RestartUI };       // Same as above but in UI mode
  /* -- Command-line and environment variables ---------------------*/ private:
  EcId             ecExit;             // Exit mode
  const string     strCWD;             // Current startup working directory
  int              iArgC;              // Arguments count
  ArgType        **lArgV;              // Arguments list
  ArgType        **lEnvP;              // Environment list
  const StrVector  svArg;              // Arguments list
  const StrStrMap  lEnv;               // Formatted environment variables
  string           strHD;              // Persistant directory
  /* -- Set persistant directory ----------------------------------- */ public:
  void SetHome(const string &strDir) { strHD = strDir; }
  /* -- Get persistant directory ------------------------------------------- */
  bool IsNoHome(void) const { return strHD.empty(); }
  bool IsHome(void) const { return !IsNoHome(); }
  /* -- Return and move string into output string -------------------------- */
  const string GetHome(const string &strSuf) const
    { return Append(strHD, strSuf); }
  /* -- Get environment variable ------------------------------------------- */
  const string &GetEnv(const string &strEnv, const string &strO={}) const
  { // Find item and return it else return the default item
    const StrStrMapConstIt eiEnv{ lEnv.find(strEnv) };
    return eiEnv == lEnv.cend() ? strO : eiEnv->second;
  }
  /* -- Get environment variable and check that it is a valid pathname ----- */
  const string MakeEnvPath(const string &strEnv, const string &strSuffix)
  { // Get home environment variable and throw error if not found
    const StrStrMapConstIt eiEnv{ lEnv.find(strEnv) };
    if(eiEnv == lEnv.cend())
      XC("The specified environment variable is required and missing!",
         "Variable", strEnv);
    // Check validity of the specified environmen variable
    const string &strEnvVal = eiEnv->second;
    const ValidResult vRes = DirValidName(strEnvVal, VT_TRUSTED);
    if(vRes == VR_OK) return Append(strEnvVal, strSuffix);
    // Show error otherwise
    XC("The specified environment variable is invalid!",
       "Variable", strEnv,
       "Reason",   cDirBase->VNRtoStr(vRes),
       "Result",   vRes);
  }
  /* -- Get parameter total ------------------------------------------------ */
  size_t GetTotalCArgs(void) const { return static_cast<size_t>(iArgC); }
  ArgType*const*GetCArgs(void) const { return lArgV; }
  ArgType*const*GetCEnv(void) const { return lEnvP; }
  const StrStrMap &GetEnvList(void) const { return lEnv; }
  const StrVector &GetArgList(void) const { return svArg; }
  /* -- Set restart flag (0 = no restart, 1 = no params, 2 = params) ------- */
  void SetRestart(const EcId ecCmd) { ecExit = ecCmd; }
  /* -- Get startup current directory -------------------------------------- */
  const string &GetStartupCWD(void) const { return strCWD; }
  /* -- Return to startup directory ---------------------------------------- */
  void SetStartupCWD(void)
  { // Try to set the startup working direcotry and throw if failed.
    if(!DirSetCWD(GetStartupCWD()))
      XCL("Failed to set startup working directory!",
          "Directory", GetStartupCWD());
  }
  /* -- Parse command line arguments --------------------------------------- */
  StrVector ParseArgumentsArray(void)
  { // Check that args are valid
    if(iArgC < 1) XC("Arguments array count corrupted!", "Count", iArgC);
     // Check that args are valid
    if(!lArgV) XC("Arguments array corrupted!");
    if(!*lArgV) XC("Arguments array executable string corrupted!");
    if(!**lArgV) XC("Arguments array executable string is empty!");
    // Arguments list to return
    const size_t stArgCM1 = static_cast<size_t>(iArgC - 1);
    StrVector svRet; svRet.reserve(stArgCM1);
    // For each argument format the argument and add it to list
    for(const ArgType*const *atPtr = lArgV+1;
        const ArgType*const  atStr = *atPtr;
                           ++atPtr)
      svRet.emplace_back(S16toUTF(atStr));
    // One final sanity check
    if(svRet.size() != stArgCM1)
      XC("Arguments array actual count mismatch!",
         "Expect", stArgCM1, "Actual", svRet.size());
    // Return arguments list
    return svRet;
  }
  /* -- Parse environment variables ---------------------------------------- */
  StrStrMap ParseEnvironmentArray(void)
  { // Check that environment are valid
    if(!lEnvP) XC("Evironment array corrupted!");
    if(!*lEnvP) XC("First environment variable corrupted!");
    if(!**lEnvP) XC("First environment varable is empty!");
    // Compile on MacOS? Unset variables on process children because these
    // variables can cause our terminal apps to spit garbage output and ruin
    // the capture for scripts. If the guest needs these then they can just
    // try the apps standalone in the terminal without the need for the engine.
#if defined(MACOS)
    // Variables to unset
    static array<const char*const,9> aUnset{
      "NSZombieEnabled",               // Enable dealloc in foundation
      "DYLD_INSERT_LIBRARIES",         // Disable dylib override
      "MallocHelp",                    // Help messages
      "NSDeallocateZombies",           // Deallocate zombies
      "MallocCheckHeapEach",           // Don't check heap every 'n' mallocs
      "MallocCheckHeapStart",          // Don't check heap at 'n' mallocs
      "MallocScribble",                // Don't scribble memory
      "MallocGuardEdges",              // Don't guard edges
      "MallocCheckHeapAbort"           // Don't throw abort() on heap check
    };
    for(const char*const cpEnv : aUnset) unsetenv(cpEnv);
#endif
    // Arguments list to return
    StrStrMap ssmRet;
    // Process environment variables
    for(const ArgType*const *atPtr = lEnvP;
        const ArgType*const  atStr = *atPtr;
                           ++atPtr)
    { // Ignore if parameter empty
      if(!*atStr) continue;
      // Split argument into key/value pair. Ignore if no parameters
      Token tokParam{ S16toUTF(atStr), "=", 2 };
      if(tokParam.empty()) continue;
      // Find key and insert it if not found then erase the EcId value
      string &strKey = ToUpperRef(tokParam[0]);
      const StrStrMapConstIt itArg{ ssmRet.find(strKey) };
      if(itArg != ssmRet.cend()) ssmRet.erase(itArg);
      // Insert new key/value into list
      ssmRet.insert({ StdMove(strKey),
        tokParam.size() > 1 ? StdMove(tokParam[1]) : cCommon->Blank() });
    } // Return environment variables list
    return ssmRet;
  }
  /* -- Assign arguments ------------------------------------------- */ public:
  CmdLine(const int iArgs, ArgType**const atArgs, ArgType**const atEnv) :
    /* -- Initialisers ----------------------------------------------------- */
    ecExit(EcId::Quit),                // Initialise exit code
    strCWD{ DirGetCWD() },             // Initialise current working directory
    iArgC(iArgs),                      // Initialise stdlib args count
    lArgV(atArgs),                     // Initialise stdlib args ptr
    lEnvP(atEnv),                      // Initialise stdlib environment ptr
    svArg{ ParseArgumentsArray() },    // Initialise command line arguments
    lEnv{ ParseEnvironmentArray() }    // Initialise environment variables
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPERBEGIN(~CmdLine)
  // Done if arguments were never initialised
  if(iArgC <= 0) return;
  // Restore startup working directory
  SetStartupCWD();
  // Do we have a restart mode set?
  switch(ecExit)
  { // Just return if no restart required?
    case EcId::Quit: return;
    // Remove first parameter and break?
    case EcId::RestartNoParam:
      lArgV[1] = nullptr; iArgC = 1; [[fallthrough]];
    // Restart while keeping parameters?
    case EcId::Restart: SetRestart(EcId::Quit);
      // Do the restart! Again, everything is cleaned up so this is convenient!
      switch(const int iCode = StdExecVE(lArgV, lEnvP))
      { // Success? Proceed to quit
        case 0: break;
        // Error occured? Don't attempt execution again and show error
        default: XCL("Failed to restart process!",
          "Process", *lArgV, "Code", iCode, "Parameters", iArgC);
      } // Done
      break;
    // Remove first parameter and break in ui mode?
    case EcId::RestartNoParamUI:
      lArgV[1] = nullptr; iArgC = 1; [[fallthrough]];
    // Restart while keeping parameters in ui mode?
    case EcId::RestartUI: SetRestart(EcId::Quit);
      // Do the restart! Again, everything is cleaned up so this is convenient!
      switch(const int iCode = StdSpawnVE(lArgV, lEnvP))
      { // Success? Proceed to quit
        case 0: break;
        // Error occurred? Don't attempt execution again and show error
        default: XCL("Failed to spawn new process!",
          "Process", *lArgV, "Code", iCode, "Parameters", iArgC);
      } // Done
      break;
  } // Parent process should be exiting cleanly after returning here
  DTORHELPEREND(~CmdLine)
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CmdLine)             // Remove default functions
  /* -- End ---------------------------------------------------------------- */
} *cCmdLine = nullptr;                 // Pointer to static class
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
