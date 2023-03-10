/* == CMDLINE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This static class stores command line arguments and assists in      ## */
/* ## restarting the engine when needed.                                  ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfCmdLine {                  // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfDir;                 // Using util interface
using namespace IfSystem;              // Using system interface
/* -- Command line helper class (should be the first global to inti) ------- */
static class CmdLine final             // StrVector is arguments list
{ /* -- Public typedefs -------------------------------------------- */ public:
  enum ExitCommand { EC_QUIT, EC_RESTART_NO_PARAM, EC_RESTART };
  /* -- Command-line and environment variables ---------------------*/ private:
  ExitCommand      ecExit;             // Exit mode
  const string     strCWD;             // Current startup working directory
  int              iArgC;              // Arguments count
  ARGTYPE        **lArgV;              // Arguments list
  ARGTYPE        **lEnvP;              // Environment list
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
       "Reason",   DirValidNameResultToString(vRes),
       "Result",   vRes);
  }
  /* -- Get parameter total ------------------------------------------------ */
  size_t GetTotalCArgs(void) const { return static_cast<size_t>(iArgC); }
  ARGTYPE*const*GetCArgs(void) const { return lArgV; }
  ARGTYPE*const*GetCEnv(void) const { return lEnvP; }
  const StrStrMap &GetEnvList(void) const { return lEnv; }
  const StrVector &GetArgList(void) const { return svArg; }
  /* -- Set restart flag (0 = no restart, 1 = no params, 2 = params) ------- */
  void SetRestart(const ExitCommand ecCmd) { ecExit = ecCmd; }
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
    for(const ARGTYPE*const *atPtr = lArgV+1;
        const ARGTYPE*const  atStr = *atPtr;
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
    for(const ARGTYPE*const *atPtr = lEnvP;
        const ARGTYPE*const  atStr = *atPtr;
                           ++atPtr)
    { // Ignore if parameter empty
      if(!*atStr) continue;
      // Split argument into key/value pair. Ignore if no parameters
      Token tokParam{ S16toUTF(atStr), "=", 2 };
      if(tokParam.empty()) continue;
      // Find key and insert it if not found then erase the ExitCommand value
      string &strKey = ToUpperRef(tokParam[0]);
      const StrStrMapConstIt itArg{ ssmRet.find(strKey) };
      if(itArg != ssmRet.cend()) ssmRet.erase(itArg);
      // Insert new key/value into list
      ssmRet.insert({ move(strKey),
        tokParam.size() > 1 ? move(tokParam[1]) : strBlank });
    } // Return environment variables list
    return ssmRet;
  }
  /* -- Assign arguments ------------------------------------------- */ public:
  CmdLine(const int iArgs, ARGTYPE**const atArgs, ARGTYPE**const atEnv) :
    /* -- Initialisation of members ---------------------------------------- */
    ecExit(EC_QUIT),                   // Initialise exit code
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
  { // Just return if no restart required
    case EC_QUIT: return;
    // Remove first parameter and break
    case EC_RESTART_NO_PARAM: lArgV[1] = nullptr; iArgC = 1;
    // Restart while keeping parameters
    case EC_RESTART: break;
  } // Do the restart! Again, everything is cleaned up so this is convenient!
  const int iCode = STDEXECVE(lArgV, lEnvP);
  // The execution failed so report the error
  XCL("Failed to restart process!",
      "Process", *lArgV, "Code", iCode, "Parameters", iArgC);
  DTORHELPEREND(CmdLine)
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CmdLine);            // Remove default functions
  /* -- End ---------------------------------------------------------------- */
} *cCmdLine = nullptr;                 // Pointer to static class
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
