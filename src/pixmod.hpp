/* == PIXMOD.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is a POSIX specific module that parses executable files to     ## */
/* ## reveal information about it. (ToDo: This is not implemented yet!).  ## */
/* ## Since we support MacOS and Linux, we can support both systems very  ## */
/* ## simply with POSIX compatible calls.                                 ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == Version information subclass ========================================= */
class SysModule :
  /* -- Base classes ------------------------------------------------------- */
  public SysModuleData                 // System module data
{ /* -- Manual data (i.e. for executable) -------------------------- */ public:
  explicit SysModule(const string &strModule) :
    /* -- Initialisation of members ---------------------------------------- */
    SysModuleData{ strModule }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Manual data (i.e. for executable) ---------------------------------- */
  explicit SysModule(string &&strModule) :
    /* -- Initialisation of members ---------------------------------------- */
    SysModuleData{ move(strModule) }
    /* -- No code ---------------------------------------------------------- */
    { }
   /* -- Return data (move filename) --------------------------------------- */
  explicit SysModule(string &&strModule, const unsigned int uiMa,
    const unsigned int uiMi, const unsigned int uiRe, const unsigned int uiBu,
    string &&strVen, string &&strDe, string &&strCo, string &&strVer) :
    /* -- Initialisation of members ---------------------------------------- */
    SysModuleData{ move(strModule), uiMa, uiMi, uiRe, uiBu, move(strVen),
      move(strDe), move(strCo), move(strVer) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Return data (copy filename) ---------------------------------------- */
  explicit SysModule(const string &strModule, const unsigned int uiMa,
    const unsigned int uiMi, const unsigned int uiRe, const unsigned int uiBu,
    string &&strVen, string &&strDe, string &&strCo, string &&strVer) :
    /* -- Initialisation of members ---------------------------------------- */
    SysModuleData{ strModule, uiMa, uiMi, uiRe, uiBu, move(strVen),
      move(strDe), move(strCo), move(strVer) }
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- End ---------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
