/* == CLIP.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles interfacing Lua and the engine with GLFW's      ## */
/* ## clipboard functions. Since GLFW requires that the clipboard funcs   ## */
/* ## run only in the window thread, we need to send an async event to    ## */
/* ## the window thread, and then back to the engine thread an Lua.       ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfClipboard {                // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCollector;           // Using collector namespace
using namespace IfEvtWin;              // Using event namespace
/* -- Clipboard mini class which can be used internally -------------------- */
class Clipboard
{ /* -- Protected variables ------------------------------------- */ protected:
  string           strClipboard;       // Clipboard string
  /* -- Default constructor that does nothing ------------------------------ */
  Clipboard(void) { }
  /* -- Window set clipboard request --------------------------------------- */
  void ClipOnSetNRCbT(void) { cGlFW->WinSetClipboardString(strClipboard); }
  /* -- Window set clipboard request ------------------------------- */ public:
  static void ClipOnSetNRCb(const EvtWin::Cell &ewcArgs)
    { reinterpret_cast<Clipboard*>
        (ewcArgs.vParams.front().vp)->ClipOnSetNRCbT(); }
  /* -- Get string sent or retrieved --------------------------------------- */
  const string &ClipGet(void) const { return strClipboard; }
  /* -- Get string sent or retrieved --------------------------------------- */
  void ClipSet(const string &strText)
  { // Set clipboard string to set
    strClipboard = strText;
    // Dispatch the event
    cEvtWin->Add(EWC_CB_SETNR, this);
  }
};/* -- Clipboard collector and lua interface class ------------------------ */
BEGIN_COLLECTOREX(Clips, Clip, CLHelperUnsafe,
  const EvtWin::RegVec rvEvents;,,     // Events list to register
  private LuaEvtMaster<Clip, LuaEvtTypeAsync<Clip>>);
BEGIN_MEMBERCLASS(Clips, Clip, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public LuaEvtSlave<Clip, 2>,         // Need to store callback and class
  public Lockable,                     // Lua garbage collector instruction
  public Clipboard,                    // Base clipboard class
  public Ident                         // Name of this clipboard event
{ /* -- Async thread event callback (called by LuaEvtMaster) ------- */ public:
  void LuaEvtCallbackAsync(const EvtMain::Cell &epData) try
  { // Get reference to string vector and we need three parameters
    // [0]=Pointer to socket class
    const EvtMain::Params &eParams = epData.vParams;
    // Must have 2 parameters
    if(!LuaEvtsCheckParams<2>(eParams))
      XC("Clipboard did not receive two parameters!",
         "Identifier", IdentGet(), "Count", eParams.size());
    // If lua is not paused?
    if(!bLuaPaused)
    { // Get and push function
      if(!LuaRefGetFunc(1))
        XC("Clipboard first argument not a function!",
           "Identifier", IdentGet());
      // Get and push class
      if(!LuaRefGetUData())
        XC("Clipboard second argument not a class!",
           "Identifier", IdentGet());
      // Call callback with class
      CallFuncEx(lsState, 1);
    } // Done with references. We won't be using them anymore.
    return LuaEvtDeInit();
  } // Exception occured? Cleanup and rethrow exception
  catch(const exception&) { LuaEvtDeInit(); throw; }
  /* -- Window get clipboard request in window thread -------------- */ public:
  void ClipOnGetCbT(void)
  { // Grab the string to clipboard
    strClipboard = cGlFW->WinGetClipboardString();
    // Pass the string back to the engine thread and Lua
    LuaEvtDispatch();
  }
  /* -- Window get clipboard request in window thread ---------------------- */
  static void ClipOnGetCb(const EvtWin::Cell &ewcArgs)
    { reinterpret_cast<Clip*>(ewcArgs.vParams.front().vp)->ClipOnGetCbT(); }
  /* -- Window set clipboard request --------------------------------------- */
  void ClipOnSetCbT(void) { ClipOnSetNRCbT(); LuaEvtDispatch(); }
  /* -- Window set clipboard request --------------------------------------- */
  static void ClipOnSetCb(const EvtWin::Cell &ewcArgs)
    { reinterpret_cast<Clip*>(ewcArgs.vParams.front().vp)->ClipOnSetCbT(); }
  /* -- Initialise and set string ------------------------------------------ */
  void ClipSetAsync(lua_State*const lS)
  { // Need 4 parameters (Name[1], Value[2], function[3] and class[4])
    CheckParams(lS, 4);
    // Get and check parameters
    IdentSet(GetCppStringNE(lS, 1, "Identifier"));
    strClipboard = GetCppString(lS, 2, "Value");
    CheckFunction(lS, 3, "EventFunc");
    // Init LUA references
    LuaEvtInitEx(lS);
    // We're ready, so dispatch to the window thread with this class
    cEvtWin->Add(EWC_CB_SET, this);
  }
  /* -- Initialise and get string ------------------------------------------ */
  void ClipGetAsync(lua_State*const lS)
  { // Need 3 parameters (Name[1], function[2] and class[3])
    CheckParams(lS, 3);
    // Get and check parameters
    IdentSet(GetCppStringNE(lS, 1, "Identifier"));
    CheckFunction(lS, 2, "EventFunc");
    // Init LUA references
    LuaEvtInitEx(lS);
    // We're ready, so dispatch to the window thread with this class
    cEvtWin->Add(EWC_CB_GET, this);
  }
  /* -- Default constructor ------------------------------------------------ */
  Clip(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperClip{ *cClips }, // Initially unregistered
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    LuaEvtSlave{ this, EMC_CB_EVENT }  // Register Clip async event
    /* --------------------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
END_COLLECTOREX(Clips,
  cEvtWin->RegisterEx(rvEvents),
  cEvtWin->UnregisterEx(rvEvents),,
  LuaEvtMaster{ EMC_CB_EVENT },
  rvEvents{
    { EWC_CB_SET,   &Clip::ClipOnSetCb },
    { EWC_CB_GET,   &Clip::ClipOnGetCb },
    { EWC_CB_SETNR, &Clipboard::ClipOnSetNRCb },
  }
);
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
