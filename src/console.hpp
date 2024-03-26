/* == CONSOLE.HPP ========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles a console for the engine so information about   ## **
** ## the engine can be seen and manipulated by the developer or user.    ## **
** ## Exclusively for use in the engine's main thread only.               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IConsole {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArgs;                 using namespace IClock::P;
using namespace ICollector::P;         using namespace IConDef::P;
using namespace IConLib::P;            using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IFbo::P;               using namespace IFboCore::P;
using namespace IFlags;                using namespace IFont::P;
using namespace IFtf::P;               using namespace IGlFW::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace ILog::P;               using namespace ILua::P;
using namespace ILuaUtil::P;           using namespace ILuaFunc::P;
using namespace IOgl::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISocket::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace ITexture::P;           using namespace ITimer::P;
using namespace IToken::P;             using namespace IUtf;
using namespace IUtil::P;              using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* == Typedefs ============================================================= */
BUILD_FLAGS(Console,                   // Console flags classes
  /* --------------------------------------------------------------------- */
  // No settings?                      Can't disable console? (temporary)
  CF_NONE                {0x00000000}, CF_CANTDISABLE         {0x00000001},
  // Ignore first key on show console? Autoscroll on message?
  CF_IGNOREKEY           {0x00000002}, CF_AUTOSCROLL          {0x00000004},
  // Automatically copy cvar on check? Character insert mode?
  CF_AUTOCOPYCVAR        {0x00000008}, CF_INSERT              {0x00000010},
  // Console displayed?                Ignore escape key?
  CF_ENABLED             {0x00000020}, CF_IGNOREESC           {0x00000040},
  // Can't disable console? (guest setting)
  CF_CANTDISABLEGLOBAL   {0x00000080}
);/* ======================================================================= */
BUILD_FLAGS(AutoComplete,              // Autocomplete flags classes
  /* ----------------------------------------------------------------------- */
  // No autocompletion?                Autocomplete command names?
  AC_NONE                {0x00000000}, AC_COMMANDS            {0x00000001},
  // Autocomplete cvar names?
  AC_CVARS               {0x00000002},
  /* ----------------------------------------------------------------------- */
  AC_MASK{ AC_COMMANDS|AC_CVARS }      // All flags
);/* ======================================================================= */
BUILD_FLAGS(Redraw,
  /* ----------------------------------------------------------------------- */
  // Redraw nothing                    Redraw text console
  RD_NONE                {0x00000000}, RD_TEXT                {0x00000001},
  // Redraw graphical console
  RD_GRAPHICS            {0x00000002},
  /* ----------------------------------------------------------------------- */
  RD_BOTH{ RD_TEXT|RD_GRAPHICS }       // All flags
);/* ======================================================================= */
static class Console final :           // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private ConLines,                    // Console text lines list
  private ConLinesConstIt,             // Text lines forward iterator
  private ConLinesConstRevIt,          // Text lines reverse iterator
  private IHelper,                     // Initialisation helper
  public ConsoleFlags                  // Console flags
{ /* -- Private variables -------------------------------------------------- */
  constexpr static const size_t
    stConCmdMinLength = 1,             // Minimum length of a console command
    stConCmdMaxLength = 255;           // Maximum length of a console command
  /* -- Public typedefs ---------------------------------------------------- */
  AutoCompleteFlags acFlags;           // Flags for autocomplete
  /* ----------------------------------------------------------------------- */
  typedef map<const string,const ConLib> LibList; // Map of commands type
  typedef LibList::iterator              LibListIt;
  typedef LibList::const_iterator        LibListItConst;
  typedef queue<ConLine>                 ConLineQueue;
  /* -- Input -------------------------------------------------------------- */
  ConLineQueue     clqOutput;          // Console lines pending
  ConLinesConstRevIt clriPosition;     // Console output position
  StrList          slHistory;          // Console history
  StrListConstRevIt slriInputPosition; // History position
  size_t           stInputMaximum,     // Maximum no. of input lines
                   stOutputMaximum,    // Maximum no. of output lines
                   stMaxInputLine,     // Maximum no. of chars in input
                   stMaxOutputLine,    // Maximum no. of chars per output line
                   stMaxOutputLineE;   // Maximum no. of chars with ellipsis
  string           strConsoleBegin,    // Console input line (before cursor)
                   strConsoleEnd;      // Console input line (after cursor)
  ssize_t          sstPageLines,       // Lines to move when paging up or down
                   sstPageLinesNeg;    // As above but negative version
  RedrawFlags      rfDefault,          // Default redraw type
                   rfFlags;            // Redraw flags
  /* -- Text mode ---------------------------------------------------------- */
  ClockInterval<>  ciOutputRefresh,    // Next screen buffer update time
                   ciInputRefresh;     // Time to wait before next peek
  string           strStatusLeft,      // Text-mode console left status text
                   strStatusRight,     // Text-mode console right status text
                   strTimeFormat;      // Default time format
  /* -- Graphics ----------------------------------------------------------- */
  const uint32_t   ulFgColour;         // Console input text colour
  uint32_t         ulBgColour;         // Console background texture colour
  Colour           cTextColour;        // Console text colour
  GLfloat          fTextScale,         // Console font scale
                   fConsoleHeight,     // Console height
                   fTextLetterSpacing, // Console text letter spacing
                   fTextLineSpacing,   // Console text line spacing
                   fTextWidth,         // Console text width
                   fTextHeight;        // Console text height
  Texture          ctConsole;          // Console background texture
  Font             cfConsole;          // Console font
  char             cCursor;            // Cursor character to use
  /* -- Other -------------------------------------------------------------- */
  const ConCmdStaticList &conLibList;  // Default console cmds list
  LibList          llCmds;             // Console commands list
  /* -- Lua ---------------------------------------------------------------- */
  LuaFunc::Map     lfList;             // Lua console callbacks list
  /* -- Do clear console, clear history and reset position ----------------- */
  void DoFlush(void) { clear(); clriPosition = rbegin(); }
  /* -- Check that the console variable name is valid ---------------------- */
  bool IsValidConsoleCommandName(const string &strName)
  { // Check minimum name length
    if(strName.length() < stConCmdMinLength ||
       strName.length() > stConCmdMaxLength) return false;
    // Get address of string. The first character must be a letter
    const unsigned char *ucpPtr =
      reinterpret_cast<const unsigned char*>(strName.c_str());
    if(!isalpha(*ucpPtr)) return false;
    // For each character in cvar name until end of string...
    for(const unsigned char*const ucpPtrEnd = ucpPtr + strName.length();
                                   ++ucpPtr < ucpPtrEnd;)
      if(!isalnum(*ucpPtr) && *ucpPtr != '_') return false;
    // Success!
    return true;
  }
  /* -- Reserve history items ---------------------------------------------- */
  void ReserveHistoryLines(const size_t stLines)
  { // Calculate total lines when added
    const size_t stTotal = slHistory.size() + stLines;
    // Return if we don't need to remove lines
    if(stTotal <= GetInputMaximum()) return;
    // If too many lines would be written?
    if(stLines >= GetInputMaximum()) return ClearHistory();
    // Iterator to find
    const StrListConstIt sliIt{ slriInputPosition.base() };
    // Lines to prune to
    const size_t stRemove = slHistory.size() - stLines;
    // Repeat...
    do
    { // If this item is not selected in the history? Erase and next
      if(slHistory.cbegin() != sliIt) { slHistory.pop_front(); continue; }
      // Now we know that this iterator is selected we can erase the rest
      slriInputPosition = StrListConstRevIt{
        slHistory.erase(slHistory.cbegin(), next(slHistory.cbegin(),
          static_cast<ssize_t>((slHistory.size() - 1) - GetInputMaximum()))) };
      // Done
      break;
    } // ...Until we have cleared enough lines
    while(size() > stRemove);
  }
  /* -- Delete lines at back of console log to make way for new entries ---- */
  void ReserveLines(const size_t stLines)
  { // Calculate total liens
    const size_t stTotal = size() + stLines;
    // If writing this many lines would fit in the log, then the log does not
    // need pruning.
    if(stTotal <= GetOutputMaximum()) return;
    // If too many lines would be written?
    if(stLines >= GetOutputMaximum()) return DoFlush();
    // Iterator to find
    const ConLinesConstIt cliIt{ clriPosition.base() };
    // Lines to prune to
    const size_t stRemove = size() - stLines;
    // Repeat...
    do
    { // If this item is not selected in the output history? Erase and next
      if(cbegin() != cliIt) { pop_front(); continue; }
      // Get items remaining to remove
      clriPosition = ConLinesConstRevIt{ erase(cbegin(),
        next(cbegin(), static_cast<ssize_t>(size() - stRemove))) };
      // Done
      break;
    } // ...Until we have cleared enough lines
    while(size() > stRemove);
  }
  /* -- Calculate the number of triangles and commands for console fbo ----- */
  void RecalculateFboListReserves(void)
  { // Ignore if font not available (not in graphical mode).
    if(GetFontRef().IsNotInitialised()) return;
    // Get console fbo and font
    Fbo &fboC = cFboCore->fboConsole;
    // Set console text size so the scaled size is properly calculated
    CommitScale();
    // Estimate amount of triangles that would fit in the console and if
    // we have a non-zero value?
    if(const size_t stTriangles = static_cast<size_t>(
      (ceilf(fboC.DimGetWidth<GLfloat>() /
       ceilf(GetFontRef().dfScale.DimGetWidth())) *
       ceilf(fboC.DimGetHeight<GLfloat>() /
       ceilf(GetFontRef().dfScale.DimGetHeight()))) + 2))
      // Try to reserve the triangles and 2 commands and log if failed!
      if(!fboC.FboReserve(stTriangles, 2))
        cLog->LogWarningExSafe("Console fbo failed to reserve $ triangles!",
          stTriangles);
  }
  /* -- Clear console line ------------------------------------------------- */
  void DoClearInput(void) { strConsoleBegin.clear(); strConsoleEnd.clear(); }
  /* -- Events list -------------------------------------------------------- */
  const EvtMain::RegVec reEvents;       // Events list to register
  /* -- Return commands list --------------------------------------- */ public:
  const LibList &GetCmdsList(void) const { return llCmds; }
  const LuaFunc::Map &GetLuaCmdsList(void) const { return lfList; }
  /* -- Return maximum number of output history lines ---------------------- */
  size_t GetOutputMaximum(void) const { return stOutputMaximum; }
  /* -- Return maximum number of input history lines ----------------------- */
  size_t GetInputMaximum(void) const { return stInputMaximum; }
  /* -- Return lua commands list ------------------------------------------- */
  const LuaFunc::Map &GetLuaCmds(void) const { return lfList; }
  /* -- Return information about a console command ------------------------- */
  const ConLib &GetCommand(const ConCmdEnums cceId) const
    { return conLibList[cceId]; }
  /* -- Clear console and redraw ------------------------------------------- */
  void Flush(void) { DoFlush(); SetRedraw(); }
  /* -- Find a virtual command and throw error if not found ---------------- */
  auto FindVirtualCommand(const string &strCmd)
  { // Find command in console command list and throw if we don't have it
    auto lfItem{ lfList.find(strCmd) };
    if(lfItem != lfList.cend()) return lfItem;
    // Not found so throw error
    XC("Virtual console command not found!", "Command", strCmd);
  }
  /* -- Push and get error callback function id ---------------------------- */
  static void LuaCallbackStatic(const Args &);
  void LuaCallback(const Args &aList)
    { FindVirtualCommand(aList.front())->
        second.LuaFuncProtectedDispatch(0, aList); }
  /* -- Returns the end of the lua console command list -------------------- */
  LuaFunc::MapIt GetLuaConCmdListEnd(void) { return lfList.end(); }
  /* -- Unregister all console commands ------------------------------------ */
  LuaFunc::MapIt UnregisterLuaCommand(const LuaFunc::MapIt lfmiCommand)
  { // Iterator is registered?
    if(lfmiCommand != lfList.cend())
    { // Unregister the command and erase the reference
      UnregisterCommand(lfmiCommand->first);
      lfList.erase(lfmiCommand);
    } // Return the last iterator
    return lfList.end();
  }
  /* -- Register user console command from lua ----------------------------- */
  LuaFunc::MapIt RegisterLuaCommand(string &strCmd,
    const unsigned int uiMinimum, const unsigned int uiMaximum)
  { // Find command and throw exception if already exists
    const auto lfItem{ lfList.find(strCmd) };
    if(lfItem != lfList.cend())
      XC("Virtual command already exists!",
         "Command", strCmd, "Reference", lfItem->second.LuaFuncGet());
    // Register real command
    const LibListItConst clItem{
      RegisterCommand(strCmd, uiMinimum, uiMaximum, LuaCallbackStatic) };
    // Add command to local list. Do not move the position of this call.
    return lfList.insert(lfList.cend(),
      { StdMove(strCmd), LuaFunc{ StrAppend("CC:", clItem->first), true } });
  }
  /* -- Add specified command line to history ------------------------------ */
  void AddHistory(const string &strCmdLine)
  { // Service room for one more input line
    ReserveHistoryLines(1);
    // Add input line to history
    slHistory.emplace_back(strCmdLine);
    // Update history to first item
    slriInputPosition = slHistory.crend();
  }
  /* -- TestAutoComplete ------------------- Processes a successful match -- */
  template<class MapType>
    bool TestAutoComplete(const MapType &mtList, const size_t stBPos,
      const size_t stEPos, const string &strWhat)
  { // Setup iterator to find item and return if empty
    typedef typename MapType::const_iterator MapIterator;
    const MapIterator ciItem{ mtList.lower_bound(strWhat) };
    if (ciItem == mtList.cend()) return false;
    // Get reference name of item and return failed if comparison failed
    const string &strKey = ciItem->first;
    if(strKey.compare(0, strWhat.size(), strWhat)) return false;
    // We found the word so now we need to replace it with the actual command.
    if(stBPos == string::npos) strConsoleBegin = strKey;
    else strConsoleBegin =
      StrAppend(strConsoleBegin.substr(0, stBPos+1), strKey);
    if(stEPos == string::npos) strConsoleEnd.clear();
    else strConsoleEnd = strConsoleEnd.substr(stEPos);
    // Redraw the console because we changed the input field
    SetRedraw();
    // Succeeded
    return true;
  }
  /* -- AutoComplete ------------ Auto complete the word under the cursor -- */
  bool AutoComplete(void)
  { // Do not attempt to autocomplete if disabled
    if(acFlags.FlagIsSet(AC_NONE)) return false;
    // Find last whitespace or use the whole text on the beginning text
    // and also find first whitespace or use the whole text on the end text
    const size_t stBPos = strConsoleBegin.find_last_of(' '),
                 stEPos = strConsoleEnd.find(' ');
    // Grab word to autocomplete and return if it is empty
    const string strWhat{ (stBPos == string::npos
      ? strConsoleBegin : strConsoleBegin.substr(stBPos + 1)) +
                          (stEPos == string::npos
      ? strConsoleEnd : strConsoleEnd.substr(0, stEPos)) };
    // Return failure if word is empty... or
    if(strWhat.empty()) return false;
    // Walk through all the cvars and check if the partial command matches
    if(acFlags.FlagIsSet(AC_COMMANDS) &&
      TestAutoComplete(llCmds, stBPos, stEPos, strWhat))
        return true;
    // Return if cvars not being checked
    if(acFlags.FlagIsClear(AC_CVARS)) return false;
    // Walk through all the cvars and check if the partial cvar matches
    return TestAutoComplete(cCVars->GetVarList(), stBPos, stEPos, strWhat);
  }
  /* -- Return text input -------------------------------------------------- */
  bool InputEmpty(void)
    { return strConsoleBegin.empty() && strConsoleEnd.empty(); }
  /* -- Return text input -------------------------------------------------- */
  const string InputText(void)
    { return StrAppend(strConsoleBegin, strConsoleEnd); }
  /* -- OnExecute event ------------------------ Execute inputted command -- */
  void Execute(void)
  { // Do not proceed further if no text is inputted
    if(InputEmpty()) return;
    // Build command by appending text before and after the cursor.
    const string strCmd{ InputText() };
    // Clear current text input now we have it here
    ClearInput();
    // Split command-line into arguments and process them if not empty
    if(const Args aList{ strCmd }) ExecuteArguments(strCmd, aList);
  }
  /* -- Fired when lua needs to be paused (EMC_LUA_PAUSE) ------------------ */
  void OnLuaPause(const EvtMain::Cell &)
  { // Return if pause was not successful
    if(!cLua->PauseExecution())
      return AddLine("Execution already paused. Type 'lresume' to continue.");
    // The mainmanual* functions will consume 100% of the thread load
    // when it doesn't request a request to redraw so make sure to
    // throttle it.
    if(cSystem->IsNotGraphicalMode()) cTimer->TimerSetDelayIfZero();
    // Can't disable console while paused
    SetCantDisable(true);
    // Write to console
    AddLine("Execution paused. Type 'lresume' to continue.");
  }
  /* -- Fired when lua needs to be resumed (EMC_LUA_RESUME) ---------------- */
  void OnLuaResume(const EvtMain::Cell &)
  { // Return if pause was not successful
    if(!cLua->ResumeExecution())
      return AddLine("Execution already in progress.");
    // Refresh stored delay because of manual render mode
    if(cSystem->IsNotGraphicalMode())
      cTimer->TimerSetDelay(cCVars->GetInternalSafe<unsigned int>(APP_DELAY));
    // Console can now be disabled
    SetCantDisable(false);
    // Write to console
    AddLine("Execution resumed.");
  }
  /* -- OnCharPress --------------------------- Console character pressed -- */
  void OnCharPress(const unsigned int uiKey)
  { // Ignore first key? Ignore it
    if(FlagIsSet(CF_IGNOREKEY)) FlagClear(CF_IGNOREKEY);
    // Insert character if insert mode is on
    else if(FlagIsSet(CF_INSERT)) AddInputChar(uiKey);
    // Repalce character in input buffer
    else ReplaceInputChar(uiKey);
  }
  /* -- OnForceRedraw ---------------- Force a bot console display update -- */
  void OnForceRedraw(const EvtMain::Cell &)
    { SetRedraw(); ciOutputRefresh.CISync(); }
  /* -- Size updated event (unused on win32) ------------------------------- */
  void OnResize(const EvtMain::Cell &ecParams)
  { // Send the event to SysCon
    cSystem->OnResize();
    // Force a redraw
    OnForceRedraw(ecParams);
  }
  /* -- OnKeyPress ---------------------------------- Console key pressed -- */
  void OnKeyPress(const int iKey, const int iAction, const int iMods)
  { // Key released? Ignore
    if(iAction == GLFW_RELEASE) return;
    // Control key, which key?
    if(iMods & GLFW_MOD_CONTROL) switch(iKey)
    { // Test keys with control held
      case GLFW_KEY_PAGE_UP   : MoveLogPageUp(); break;
      case GLFW_KEY_PAGE_DOWN : MoveLogPageDown(); break;
      case GLFW_KEY_HOME      : MoveLogHome(); break;
      case GLFW_KEY_END       : MoveLogEnd(); break;
#if defined(MACOS) // Because MacOS keyboards don't have an 'insert' key.
      case GLFW_KEY_DELETE    : ToggleCursorMode(); break;
#endif
      case GLFW_KEY_C         : CopyText(); break;
      case GLFW_KEY_V         : PasteText(); break;
    } // Normal key, which key?
    else switch(iKey)
    { // Test keys with no modifiers held
      case GLFW_KEY_PAGE_UP   : MoveLogUp(); break;
      case GLFW_KEY_PAGE_DOWN : MoveLogDown(); break;
      case GLFW_KEY_HOME      : CursorHome(); break;
      case GLFW_KEY_END       : CursorEnd(); break;
      case GLFW_KEY_BACKSPACE : PopInputBeforeCursor(); break;
      case GLFW_KEY_DELETE    : PopInputAfterCursor(); break;
      case GLFW_KEY_UP        : HistoryMoveBack(); break;
      case GLFW_KEY_DOWN      : HistoryMoveForward(); break;
      case GLFW_KEY_ESCAPE    : ClearInputOrClose(); break;
      case GLFW_KEY_LEFT      : CursorLeft(); break;
      case GLFW_KEY_RIGHT     : CursorRight(); break;
      case GLFW_KEY_ENTER     : Execute(); break;
      case GLFW_KEY_TAB       : AutoComplete(); break;
      case GLFW_KEY_INSERT    : ToggleCursorMode(); break;
    }
  }
  /* -- Pop the back of the before cursor string --------------------------- */
  void PopInputBeforeCursor(void)
    { if(UtfPopBack(strConsoleBegin)) SetRedraw(); }
  /* -- Pop the front the after cursor string ------------------------------ */
  void PopInputAfterCursor(void)
    { if(UtfPopFront(strConsoleEnd)) SetRedraw(); }
  /* -- Move cursor left --------------------------------------------------- */
  void CursorLeft(void)
    { if(UtfMoveBackToFront(strConsoleBegin, strConsoleEnd)) SetRedraw(); }
  /* -- Move cursor right -------------------------------------------------- */
  void CursorRight(void)
    { if(UtfMoveFrontToBack(strConsoleEnd, strConsoleBegin)) SetRedraw(); }
  /* -- Move cursor to beginning of line ----------------------------------- */
  void CursorHome(void)
  { // Ignore if no end text
    if(strConsoleBegin.empty()) return;
    // Prepend before cursor text to after cursor text and clear before text
    strConsoleEnd.swap(strConsoleBegin);
    strConsoleEnd += strConsoleBegin;
    strConsoleBegin.clear();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Move cursor to end of line ----------------------------------------- */
  void CursorEnd(void)
  { // Ignore if no end text
    if(strConsoleEnd.empty()) return;
    // Set beginning of text to end of text and clear after cursor text
    strConsoleBegin += strConsoleEnd;
    strConsoleEnd.clear();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Copy text ---------------------------------------------------------- */
  void CopyText(void)
  { // Build command by appending text before and after the cursor
    const string strCmd{ InputText() };
    // Copy to clipboard if not empty
    if(!strCmd.empty()) cGlFW->WinSetClipboardString(strCmd);
  }
  /* -- Paste text --------------------------------------------------------- */
  void PasteText(void) { cEvtMain->Add(EMC_INP_PASTE); }
  /* -- Scroll to the specified text in console backlog -------------------- */
  bool FindText(const string &strWhat)
  { // Ignore if no lines to search or we're at the end already
    if(empty() || clriPosition == rend()) return false;
    // Start from the current line, and iterate until we get to the end. We
    // Should skip the current line so recursive executions of this command
    // work properly.
    for(ConLinesConstRevIt clriLine{ next(clriPosition, 1) };
                           clriLine != rend();
                         ++clriLine)
    { // Get reference to console line data structure
      const ConLine &clD = *clriLine;
      // Find text and goto next line if not found
      if(clD.strLine.find(strWhat) == string::npos) continue;
      // This will be the new position of the console history buffer
      clriPosition = clriLine;
      // Redraw required obviously
      SetRedraw();
      // No need to search anymore. The user should enter the same command
      // again to continue searching from this position.
      return true;
    } // Not found
    return false;
  }
  /* -- Move log by a certain amount --------------------------------------- */
  void MoveLogPage(const ConLinesConstRevIt &clriBegin,
    const ConLinesConstRevIt &clriEnd, const ConLinesConstRevIt &clriReset,
    const ssize_t sstAmount, const ssize_t sstMove)
  { // Ignore if already at reset position
    if(clriPosition == clriReset) return;
    // Redrawing
    SetRedraw();
    // If we can advance that far? distance should always return positive
    // value since we're moving forwards
    if(distance(clriBegin, clriEnd) < sstAmount) clriPosition = clriReset;
    // Can safely advance forwards
    else advance(clriPosition, sstMove);
  }
  /* -- Functions to move the active console line -------------------------- */
  void MoveLogHome(void)
    { if(clriPosition != crend()){ SetRedraw(); clriPosition = crend(); } }
  void MoveLogEnd(void)
    { if(clriPosition != crbegin()){ SetRedraw(); clriPosition = crbegin(); }}
  void MoveLogUp(void)
    { if(clriPosition != crend()){ SetRedraw(); ++clriPosition; }}
  void MoveLogDown(void)
    { if(clriPosition != crbegin()){ SetRedraw(); --clriPosition; }}
  void MoveLogPageUp(void)
    { MoveLogPage(clriPosition, crend(), crend(),
        sstPageLines, sstPageLines); }
  void MoveLogPageDown(void)
    { MoveLogPage(crbegin(), clriPosition, crbegin(),
        sstPageLines, sstPageLinesNeg); }
  /* -- OnLastItem event ---------------------- Selects last console item -- */
  void HistoryMoveBack(void)
  { // Ignore if no history lines or there is text after the cursor
    if(slHistory.empty() || !strConsoleEnd.empty()) return;
    // Set last typed item if not initialised
    if(slriInputPosition == slHistory.crend())
      slriInputPosition = slHistory.crbegin();
    // Move back towards beginning of list if we can
    else if(next(slriInputPosition, 1) != slHistory.crend())
      ++slriInputPosition;
    // Set line from history
    strConsoleBegin = *slriInputPosition;
    strConsoleEnd.clear();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- OnNextItem event --------------------- Selects ntext console item -- */
  void HistoryMoveForward(void)
  { // Ignore if no history lines, history not initialised or there is text
    // after the cursor
    if(slHistory.empty() || !strConsoleEnd.empty() ||
       slriInputPosition == slHistory.crend()) return;
    // Move forward towards end if we can
    if(slriInputPosition != slHistory.crbegin()) --slriInputPosition;
    // Set line from history
    strConsoleBegin = *slriInputPosition;
    strConsoleEnd.clear();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Clear console input history ---------------------------------------- */
  void ClearHistory(void)
  { // Clear history list
    slHistory.clear();
    // No history selected
    slriInputPosition = slHistory.crend();
  }
  /* -- Clear console line ------------------------------------------------- */
  void ClearInput(void)
  { // Clear text before and after cursor
    DoClearInput();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Either clear the input text or close the console ------------------- */
  void ClearInputOrClose(void)
  { // If there is no text in the input buffer?
    if(InputEmpty())
    { // Hide the console and set to ignore an escape keypress if succeeded
      if(SetVisible(false)) FlagSet(CF_IGNOREESC);
    } // Else clear input
    else ClearInput();
  }
  /* -- Input manipulation ------------------------------------------------- */
  void AddInputChar(const unsigned int uiChar)
  { // Make sure line is under max characters
    if(strConsoleBegin.size() + strConsoleEnd.size() >= stMaxInputLine)
      return;
    // Encode character to utf-8
    UtfAppend(uiChar, strConsoleBegin);
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Input manipulation ------------------------------------------------- */
  void ReplaceInputChar(const unsigned int uiChar)
  { // Add character if we are at the end of the line input
    if(strConsoleEnd.empty()) return AddInputChar(uiChar);
    // Encode character to utf-8
    UtfAppend(uiChar, strConsoleBegin);
    // Remove character from beginning of end of line input
    UtfPopFront(strConsoleEnd);
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Toggle insert and overwrite mode ----------------------------------- */
  void ToggleCursorMode(void)
  { // Toggle cursor mode
    FlagToggle(CF_INSERT);
    // Set new cursor depending of if enabled or not
    if(GetFontRef().IsInitialised())
      cCursor = FlagIsSet(CF_INSERT) ? '|' : '_';
    // Tell SysCon that the cursor changed if needed and return
    else return cSystem->SetCursorMode(FlagIsSet(CF_INSERT));
    // Need to redraw too
    SetRedraw();
  }
  /* -- Returns if the console should be visible --------------------------- */
  bool IsVisible(void) { return FlagIsSet(CF_ENABLED); }
  /* -- Returns if the console should NOT be visible ----------------------- */
  bool IsNotVisible(void) { return !IsVisible(); }
  /* -- Sets redraw flag so the console fbo or terminal buffer is redrawn -- */
  void SetRedraw(void)
    { rfFlags.FlagReset(rfDefault.FlagIsSet(RD_TEXT) || IsVisible() ?
        rfDefault : RD_NONE); }
  /* -- Reset defaults (for lreset) ---------------------------------------- */
  void RestoreDefaultProperties(void)
  { // Restore default settings from cvar registry
    CommitLetterSpacing();
    CommitScale();
    CommitLineSpacing();
  }
  /* -- Init console font -------------------------------------------------- */
  void InitConsoleFont(void)
  { // Load font. Cvars won't set the font size initially so we have to do
    // it manually. If we init cvars after, then destructor will crash because
    // the cvars havn't been initialised
    Ftf fFTFont;
    fFTFont.InitFile(cCVars->GetInternalStrSafe(CON_FONT), fTextWidth,
      fTextHeight, 96, 96, 0.0);
    GetFontRef().InitFTFont(fFTFont,
      cCVars->GetInternalSafe<GLuint>(CON_FONTTEXSIZE),
      cCVars->GetInternalSafe<GLuint>(CON_FONTPADDING), OF_L_L_MM_L,
        GetFontRef());
    // Get minimum precache range and if valid?
    if(const unsigned int uiCharMin =
      cCVars->GetInternalSafe<unsigned int>(CON_FONTPCMIN))
      // Get maximum precache range and return if valid?
      if(const unsigned int uiCharMax =
        cCVars->GetInternalSafe<unsigned int>(CON_FONTPCMAX))
          // If maximum is above the minimum? Pre-cache the characters range
          if(uiCharMax >= uiCharMin)
            GetFontRef().InitFTCharRange(uiCharMin, uiCharMax);
    // Set default font parameters
    RestoreDefaultProperties();
    // LUA not allowed to deallocate this font!
    GetFontRef().LockSet();
  }
  /* -- Init console texture ----------------------------------------------- */
  void InitConsoleTexture(void)
  { // Get console texture filename, use a solid if not specified
    const string strCT{ cCVars->GetInternalStrSafe(CON_BGTEXTURE) };
    if(strCT.empty())
    { // Create simple image for solid colour and load it into a texture
      Image imgData{ 0xFFFFFFFF };
      GetTextureRef().InitImage(imgData, 0, 0, 0, 0, OF_N_N);
    } // Else filename specified so load it!
    else
    { // Load image from disk and load it into a texture
      Image imgData{ strCT, IL_NONE };
      GetTextureRef().InitImage(imgData, 0, 0, 0, 0, OF_L_L_MM_L);
    } // LUA will not be allowed to garbage collect this texture class!
    GetTextureRef().LockSet();
  }
  /* -- Execute arguments list --------------------------------------------- */
  void ExecuteArguments(const string &strCmd, const Args &aList)
  { // Get var or command name
    const string &strVarOrCmd = aList.front();
    if(strVarOrCmd.empty()) return;
    // Dump whole input to log
    AddLineExC(COLOUR_YELLOW, '>', strCmd);
    // Reset scrolling position if flag set and not at the bottom.
    if(FlagIsSet(CF_AUTOSCROLL)) MoveLogEnd();
    // Push entire text input to recall history
    AddHistory(strCmd);
    // Find console callback function and function not found?
    const auto cblItem{ llCmds.find(strVarOrCmd) };
    if(cblItem == llCmds.cend())
    { // Output string
      ostringstream osS;
      // If the cvar exists?
      if(cCVars->VarExistsSafe(strVarOrCmd))
      { // Resulting message
        osS << "Variable '" << strVarOrCmd << "' ";
        // No value specified?
        if(aList.size() <= 1)
        { // Show it
          osS << "is currently " << cCVars->Protect(strVarOrCmd) << "!";
          // Copy it to clipboard if requested
          if(FlagIsSet(CF_AUTOCOPYCVAR))
            cGlFW->WinSetClipboardString(StrFormat("$ \"$\"",
              strVarOrCmd, cCVars->GetStrSafe(strVarOrCmd)));
        } // Else set item and get return value
        else switch(const CVarSetEnums cscResult =
          aList[1] == "~" ? cCVars->ResetSafe(strVarOrCmd) :
          cCVars->SetSafe(strVarOrCmd, aList[1]))
        { // Success. Show result
          case CVS_OK:
            osS << "is now " << cCVars->Protect(strVarOrCmd) << '!'; break;
          // Success. Show result
          case CVS_OKNOTCHANGED: osS << "not changed!"; break;
          // Cvar not found (THIS SHOULDNT HAPPEN!!!)
          case CVS_NOTFOUND: osS << "not found!"; break;
          // Not writable
          case CVS_NOTWRITABLE: osS << "is read-only!"; break;
          // Invalid filename
          case CVS_NOTFILENAME: osS << "is an invalid filename!"; break;
          // Invalid integer
          case CVS_NOTINTEGER: osS << "must be an integer!"; break;
          // Invalid double
          case CVS_NOTFLOAT: osS << "must be a float!"; break;
          // Invalid boolean
          case CVS_NOTBOOLEAN: osS << "must be a boolean!"; break;
          // Not greater or equal than zero
          case CVS_NOTUNSIGNED: osS << "must be equal or over zero!"; break;
          // Not power of two
          case CVS_NOTPOW2: osS << "must be power of two!"; break;
          // Must only contain letters
          case CVS_NOTALPHA: osS << "must contain only letters!"; break;
          // Must only contain digits
          case CVS_NOTNUMERIC: osS << "must contain only digits!"; break;
          // Must only contain letters or digits
          case CVS_NOTALPHANUMERIC:
            osS << "must contain only alphanumeric characters!"; break;
          // Trigger denied
          case CVS_TRIGGERDENIED: osS << "could not be updated!"; break;
          // exception in trigger
          case CVS_TRIGGEREXCEPTION:
            osS << "not set due to exception! " << cCVars->GetCBError(); break;
          // String empty
          case CVS_EMPTY: osS << "must not be empty!"; break;
          // Integer is zero
          case CVS_ZERO: osS << "must not be zero!"; break;
          // No type is set
          case CVS_NOTYPESET: osS << "not set due to unknown type!"; break;
        }
      } // Try to set initial var if it exists
      else if(cCVars->InitialVarExistsSafe(strVarOrCmd))
      { // Resulting message
        osS << "Persistent variable '" << strVarOrCmd << "' ";
        // No value specified?
        if(aList.size() <= 1)
        { // Write that it is persistant
          osS << "exists!";
          // Copy to clipboard
          if(FlagIsSet(CF_AUTOCOPYCVAR))
            cGlFW->WinSetClipboardString(StrFormat("$ \"$\"",
              strVarOrCmd, cCVars->GetInitialVarSafe(strVarOrCmd)));
        } // Set the value and say if it worked
        else if(cCVars->SetExistingInitialVar(strVarOrCmd, aList[1]))
          osS << "was set to '" << aList[1] << "'!";
        // Not changed
        else osS << "was not changed!";
      } // Cvar doesn't exist
      else osS << "Bad command or cvar name!";
      // Write result
      AddLine(osS.str());
    } // Command found?
    else
    { // Get data structure and check if enough parameters specified
      const ConLib &clData = cblItem->second;
      if(clData.uiMinimum && aList.size() < clData.uiMinimum)
        AddLine("Required parameter missing!");
      else if(clData.uiMaximum && aList.size() > clData.uiMaximum)
        AddLine("Too many parameters!");
      // Pre-checks passed
      else try
      { // Important that we handle exceptions because I'm not confident that
        // the engine will stay in a consistant state if an exception drops
        // execution to the catch() block in the main loop.
        clData.ccbFunc(aList);
      } // exception did occur
      catch(const exception &E)
      { // Print the output in the console
        AddLineExA("Console CB failed! > ", E.what());
        // Force the console to be shown because the callback might have
        // hidden the console
        DoSetVisible(true);
      } // Carry on executing as normal
    }
  }
  /* -- Commit default text scale ------------------------------------------ */
  void CommitScale(void)
    { GetFontRef().SetSize(fTextScale); }
  /* -- Commit default letter spacing -------------------------------------- */
  void CommitLetterSpacing(void)
    { GetFontRef().SetCharSpacing(fTextLetterSpacing); }
  /* -- Commit default line spacing ---------------------------------------- */
  void CommitLineSpacing(void)
    { GetFontRef().SetLineSpacing(fTextLineSpacing); }
  /* -- Process queued console lines --------------------------------------- */
  void MoveQueuedLines(void)
  { // If there are console lines in the queue?
    if(clqOutput.empty()) return;
    // Stagger the queue to the output buffer but enough so it completes fast
    // and compensates for a growing queue. At minimum the number of elements
    // or the most of half of the number of elements or five.
    size_t stLines = UtilMinimum(clqOutput.size(),
      UtilMaximum(5, clqOutput.size()/2));
    ReserveLines(stLines);
    // Get if we're at the bottom of the log
    const bool bAtBottom = clriPosition == rbegin();
    // Repeat...
    do
    { // Move item into main console render buffer
      emplace_back(StdMove(clqOutput.front()));
      // remove the old item
      clqOutput.pop();
    } // ...until we've moved enough lines
    while(--stLines != 0);
    // Auto scroll enabled or were already at the bottom of log? Set the log
    // to the bottom.
    if(FlagIsSet(CF_AUTOSCROLL) || bAtBottom) clriPosition = rbegin();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Get console textures --------------------------------------- */ public:
  Texture &GetTextureRef(void) { return ctConsole; }
  Font &GetFontRef(void) { return cfConsole; }
  Texture *GetTexture(void) { return &GetTextureRef(); }
  Font *GetFont(void) { return &GetFontRef(); }
  /* -- SetRedraw ---------------------------------------------------------- */
  void SetRedrawIfEnabled(void) { if(IsVisible()) SetRedraw(); }
  /* -- Get console lines -------------------------------------------------- */
  size_t GetOutputCount(void) { return size(); }
  size_t GetInputCount(void) { return slHistory.size(); }
  /* -- Set console input status bar (left and right) ---------------------- */
  void SetStatusLeft(const string &strValue) { strStatusLeft = strValue; }
  void SetStatusRight(const string &strValue) { strStatusRight = strValue; }
  /* -- Clear both statuses ------------------------------------------------ */
  void ClearStatus(void)
  { // Clear left if empty
    if(!strStatusLeft.empty())
    { // Clear left and right side text
      strStatusLeft.clear();
      strStatusRight.clear();
    } // Left text already empty so clear right status if empty
    else if(!strStatusRight.empty()) strStatusRight.clear();
    // Nothing changed
    else return;
    // Redrawing screen again
    SetRedraw();
  }
  /* -- Tick for bot render ------------------------------------------------ */
  void FlushToLog(void)
  { // Process queued console log lines
    MoveQueuedLines();
    // If thread delay enabled and input polling interval ready to poll?
    if(cTimer->TimerGetDelay() != 0.0 || ciInputRefresh.CITriggerStrict())
    {  // Loop forever until no more keys are pressed
      for(int iKey, iMods;;) switch(cSystem->GetKey(iKey, iMods))
      { // No key is pressed (ignore)
        case SysBase::SysCon::KT_NONE: goto Done;
        // A key was pressed
        case SysBase::SysCon::KT_KEY:
          OnKeyPress(iKey, GLFW_PRESS, iMods);
          continue;
        // A scan code was pressed
        case SysBase::SysCon::KT_CHAR:
          OnCharPress(static_cast<unsigned int>(iKey));
          continue;
      } // Only clean way to double-break without extra vars or functions
      Done:;
    } // Status update elapsed?
    if(ciOutputRefresh.CITriggerStrict())
    { // Update memory and cpu status
      cSystem->UpdateMemoryUsageData();
      cSystem->UpdateCPUUsage();
      // Redraw title
      cSystem->RedrawTitleBar(StrFormat("CPU:$$$%  FPS:$$  MEM:$  NET:$  UP:$",
        fixed, setprecision(1), cSystem->CPUUsage(), setprecision(0),
        cTimer->TimerGetFPS(), StrToBytes(cSystem->RAMProcUse(), 0),
        cSockets->stConnected.load(),
        StrShortFromDuration(cLog->CCDeltaToDouble(), 0)),
        cmSys.FormatTime(strTimeFormat.c_str()));
      // Not redrawing?
      if(rfFlags.FlagIsClear(RD_TEXT))
      { // Redraw status if imput empty. Input redraw handled elseware.
        if(InputEmpty())
          cSystem->RedrawStatusBar(strStatusLeft, strStatusRight);
        // Commit and return buffer
        return cSystem->CommitBuffer();
      }
    } // Done if not redrawing
    else if(rfFlags.FlagIsClear(RD_TEXT)) return;
    // Reset redraw flag as we're about to draw
    rfFlags.FlagClear(RD_TEXT);
    // Redraw the main text buffer
    cSystem->RedrawBuffer(*this, clriPosition);
    // If no text is inputted? Redraw customised status bar
    if(InputEmpty()) cSystem->RedrawStatusBar(strStatusLeft, strStatusRight);
    // Text inputted so redraw input bar
    else cSystem->RedrawInputBar(strConsoleBegin, strConsoleEnd);
    // Commit buffer if we need to commit
    cSystem->CommitBuffer();
  }
  /* -- Copy all console lines to log -------------------------------------- */
  size_t ToLog(void)
  { // Write all console lines to log and return lines in buffer
    for(const ConLine &clItem : *this) cLog->LogNLCDebugSafe(clItem.strLine);
    return size();
  }
  /* -- Redraw the console fbo if the console contents changed ------------- */
  void Render(void)
  { // Shift queued console lines
    MoveQueuedLines();
    // Return if we're not to redraw
    if(rfFlags.FlagIsClear(RD_GRAPHICS)) return;
    // Get reference to console fbo
    Fbo &fboC = cFboCore->fboConsole;
    // Set main fbo to draw to
    fboC.FboSetActive();
    // Get reference to main fbo
    Fbo &fboM = cFboCore->fboMain;
    // Update ortho same as the main fbo
    fboC.FboSetOrtho(0, 0, fboM.GetCoRight(), fboM.GetCoBottom());
    // Set drawing position
    const GLfloat fYAdj = fboM.fcStage.GetCoBottom() * (1 - fConsoleHeight);
    fboC.FboItemSetVertex(fboM.fcStage.GetCoLeft(),
      fboM.fcStage.GetCoTop() - fYAdj, fboM.fcStage.GetCoRight(),
      fboM.fcStage.GetCoBottom() - fYAdj);
    // Set console texture colour and blit the console background
    GetTextureRef().FboItemSetQuadRGBAInt(ulBgColour);
    GetTextureRef().BlitLTRB(0, 0, fboC.fcStage.GetCoLeft(),
      fboC.fcStage.GetCoTop(), fboC.fcStage.GetCoRight(),
      fboC.fcStage.GetCoBottom());
    // Set console input text colour
    GetFontRef().FboItemSetQuadRGBAInt(ulFgColour);
    // Restore spacing and scale as well
    CommitLetterSpacing();
    CommitLineSpacing();
    CommitScale();
    // Get below baseline height
    const GLfloat fBL = (fboC.fcStage.GetCoBottom() -
      GetFontRef().GetBaselineBelow('g')) + GetFontRef().fLineSpacing;
    // Draw input text and subtract the height drawn from Y position
    GLfloat fY = fBL - GetFontRef().PrintWU(fboC.fcStage.GetCoLeft(), fBL,
      fboC.fcStage.GetCoRight(), GetFontRef().dfScale.DimGetWidth(),
        reinterpret_cast<const GLubyte*>(StrFormat(">$\rc000000ff$\rr$",
        strConsoleBegin, cCursor, strConsoleEnd).c_str()));
    // For each line or until we clip the top of the screen, print the text
    for(ConLinesConstRevIt clI{ clriPosition }; clI!=crend() && fY>0; ++clI)
    { // Get reference to console line data structure
      const ConLine &clD = *clI;
      // Set text foreground colour with opaqueness already set above
      GetFontRef().FboItemSetQuadRGBInt(uiNDXtoRGB[clD.cColour]);
      // Draw the text and move upwards of the height that was used
      fY -= GetFontRef().PrintWU(fboC.fcStage.GetCoLeft(), fY,
        fboC.fcStage.GetCoRight(),
          GetFontRef().dfScale.DimGetWidth(), reinterpret_cast<const GLubyte*>
            (clD.strLine.c_str()));
    } // Finish and render
    fboC.FboFinishAndRender();
    // Redrawn as requested
    rfFlags.FlagClear(RD_GRAPHICS);
    // Make sure the main fbo is updated
    cFboCore->SetDraw();
  }
  /* -- Render the console to main fbo if visible -------------------------- */
  void RenderToMain(void) { if(IsVisible()) cFboCore->BlitConsoleToMain(); }
  /* -- Show the console and render it and render the fbo to main fbo ------ */
  void RenderNow(void)
  { // Show the console, render it to main frame buffer and blit it
    DoSetVisible(true);
    Render();
    cFboCore->BlitConsoleToMain();
  }
  /* -- Register console command ------------------------------------------- */
  const LibListIt RegisterCommand(const string &strName,
    const unsigned int uiMin, const unsigned int uiMax,
    const ConCbFunc ccbFunc)
  { // Check that the console command is valid
    if(!IsValidConsoleCommandName(strName))
      XC("Console command name is invalid!",
         "Command", strName, "Minimum", stConCmdMinLength,
         "Maximum", stConCmdMaxLength);
    // Check min/Max params and that they're valid
    if(uiMin && uiMax && uiMax < uiMin)
      XC("Minimum greater than maximum!",
         "Identifier", strName, "Minimum",  uiMin,
         "Maximum",    uiMax,   "Function", &ccbFunc);
    // Check function callback
    if(!ccbFunc)
      XC("Invalid function callback!",
         "Identifier", strName, "Minimum",  uiMin,
         "Maximum",    uiMax,   "Function", &ccbFunc);
    // Get existing callback and if command already exists?
    const auto ccbItem{ llCmds.find(strName) };
    if(ccbItem != llCmds.cend())
      XC("Command already registered!",
         "Identifier", strName, "Minimum",  uiMin,
         "Maximum",    uiMax,   "Function", &ccbFunc);
    // Checks passed. Now add it
    return llCmds.insert({ strName,
      { strName, uiMin, uiMax, CFL_NONE, ccbFunc } }).first;
  }
  /* -- Command exists? ---------------------------------------------------- */
  bool CommandIsRegistered(const string &strName) const
    { return llCmds.find(strName) != llCmds.cend(); }
  /* -- Unregister console command ----------------------------------------- */
  void UnregisterCommand(const string &strName)
  { // Get existing callback and if the command does not exist? Failure!
    const auto ccbItem{ llCmds.find(strName) };
    if(ccbItem == llCmds.cend())
      XC("Console command not registered!", "Command", strName);
    // Remove it
    llCmds.erase(ccbItem);
  }
  /* -- Add line as string with specified text colour ---------------------- */
  void AddLine(const string &strText, const Colour pColour)
  { // Tokenise lines into a list limited by the maximum number of lines.
    if(const TokenList tLines{ strText, cCommon->Lf(), GetOutputMaximum() })
    { // Add all the lines to the output queue
      const double dTime = cLog->CCDeltaToDouble();
      for(const string &sLine : tLines)
      { // Move the line across if it is long enough
        if(sLine.length() <= stMaxOutputLine)
          clqOutput.push({ dTime, pColour, StdMove(sLine) });
        // Push a truncated line
        else clqOutput.push({ dTime, pColour,
          sLine.substr(0, stMaxOutputLineE) + cCommon->Ellipsis() });
      }
    }
  }
  /* -- Add line as string with default text colour ------------------- */
  void AddLine(const string &strText) { AddLine(strText, cTextColour); }
  /* -- Formatted console output ------------------------------------------- */
  template<typename ...VarArgs>void AddLineEx(const char*const cpFormat,
    const VarArgs &...vaArgs)
      { AddLine(StrFormat(cpFormat, vaArgs...)); }
  /* -- Formatted console output with colour ------------------------------- */
  template<typename ...VarArgs>void AddLineEx(const Colour pColour,
    const char*const cpFormat, const VarArgs &...vaArgs)
      { AddLine(StrFormat(cpFormat, vaArgs...), pColour); }
  /* -- Formatted console output using StrAppend() ------------------------- */
  template<typename ...VarArgs>void AddLineExC(const Colour pColour,
    const VarArgs &...vaArgs) { AddLine(StrAppend(vaArgs...), pColour); }
  template<typename ...VarArgs>void AddLineExA(const VarArgs &...vaArgs)
    { AddLineExC(cTextColour, vaArgs...); }
  /* -- Set Console status ------------------------------------------------- */
  void SetCantDisable(const bool bState)
  { // Ignore if not in graphical mode because CON_HEIGHT isn't defined in
    // bot mode as it is unneeded or the flag is already set as such.
    if(GetFontRef().IsNotInitialised()) return;
    // Return if state not changed
    if(FlagIsEqualToBool(CF_CANTDISABLE, bState)) return;
    // Set the state and if can no longer be disabled?
    FlagSetOrClear(CF_CANTDISABLE, bState);
    if(FlagIsSet(CF_CANTDISABLE))
    { // Log that the console has beend disabled
      cLog->LogDebugSafe("Console visibility control has been disabled.");
      // Make sure console is showing and set full height and return
      DoSetVisible(true);
      SetHeight(1);
      // Done
      return;
    } // Disabling so log that the console can now be disabled
    cLog->LogDebugSafe("Console visibility control has been enabled.");
    // Restore user defined height
    SetHeight(cCVars->GetInternalSafe<GLfloat>(CON_HEIGHT));
  }
  /* -- Do Set Console status ---------------------------------------------- */
  bool DoSetVisible(const bool bState)
  { // Enabling and not enabled?
    if(bState && IsNotVisible())
    { // Set console enabled and redraw the buffer
      FlagSet(CF_ENABLED);
      // Redraw console
      SetRedraw();
      // Log that the console has been enabled
      cLog->LogDebugSafe("Console has been enabled.");
    } // Disabled and not disabled?
    else if(!bState && IsVisible())
    { // Set console disabled and clear redraw flag
      FlagClear(CF_ENABLED);
      // Redraw console
      SetRedraw();
      // We'll need to force a final draw in order to visibly hide the console
      cFboCore->SetDraw();
      // Log that the console has been disabled
      cLog->LogDebugSafe("Console has been disabled.");
    } // Say that nothing changed
    else
    { // Log the request
      cLog->LogDebugExSafe("Console has already been $.",
        bState ? "enabled" : "disabled");
      // Failed
      return false;
    } // Success
    return true;
  }
  /* -- Set Console visibility --------------------------------------------- */
  bool SetVisible(const bool bState)
  { // Enabling? We cant enable if disabled
    if(bState) { if(FlagIsSet(CF_CANTDISABLEGLOBAL)) return false; }
    // Disabling but can't disable? Return failure
    else if(FlagIsSet(CF_CANTDISABLE)) return false;
    // Do set enabled
    return DoSetVisible(bState);
  }
  /* -- Print version information ------------------------------------------ */
  void PrintVersion(void)
  { // Show engine version
    AddLineEx(
      "$ ($) version $.$.$ build $ for $.\n"
      "Compiled $ using $ version $.\n"
      "Copyright \xC2\xA9 $, 2006-$. All Rights Reserved.",
      cSystem->ENGName(), cSystem->ENGBuildType(), cSystem->ENGMajor(),
      cSystem->ENGMinor(), cSystem->ENGBuild(), cSystem->ENGRevision(),
      cSystem->ENGTarget(), cSystem->ENGCompiled(), cSystem->ENGCompiler(),
      cSystem->ENGCompVer(), cSystem->ENGAuthor(), cmSys.FormatTime("%Y"));
    // Add disclaimer that the author of the engine disclaims all liability for
    // guest software actions and user usage.
    AddLine("Disclaimer: This scripting ENGINE is designed ONLY for "
      "legitimate and lawful multimedia solutions and is provided AS IS with "
      "ZERO warranty. It also contains very strong cryptographic technologies "
      "which might not be allowed to be imported in your country. By using "
      "this software, whether you are the GUEST author or the END user, you "
      "accept that the ENGINE author and ALL the authors of ALL the "
      "components listed in the 'credits' command output disclaim ALL "
      "liability for how the GUEST author or the END user chooses to use this "
      "software.", COLOUR_RED);
    // Write guest info in a different colour
    AddLineEx(COLOUR_GREEN, "Guest is $ ($) version $ by $.",
      cCVars->GetInternalStrSafe(APP_LONGNAME),
      cCVars->GetInternalStrSafe(APP_SHORTNAME),
      cCVars->GetInternalStrSafe(APP_VERSION),
      cCVars->GetInternalStrSafe(APP_AUTHOR));
    // Get optional variables
    const string &strCopy = cCVars->GetInternalStrSafe(APP_COPYRIGHT);
    if(!strCopy.empty()) AddLineExC(COLOUR_GREEN, strCopy, '.');
    const string &strDesc = cCVars->GetInternalStrSafe(APP_DESCRIPTION);
    if(!strDesc.empty()) AddLineExC(COLOUR_GREEN, strDesc, '.');
    const string &strSite = cCVars->GetInternalStrSafe(APP_WEBSITE);
    if(!strSite.empty())
      AddLineEx(COLOUR_GREEN, "Visit $ for more info, help and updates.",
        strSite);
  }
  /* -- DeInit console texture and font ------------------------------------ */
  void DeInitTextureAndFont(void)
  { // Deinit console texture and font
    GetTextureRef().DeInit();
    GetFontRef().DeInit();
  }
  /* -- Reload console texture and font ------------------------------------ */
  void ReInitTextureAndFont(void)
  { // Reload console texture and font
    GetTextureRef().ReloadTexture();
    GetFontRef().ReloadTexture();
  }
  /* -- Init framebuffer object -------------------------------------------- */
  void InitFBO(void)
  { // Ignore if bot mode
    if(GetFontRef().IsNotInitialised()) return;
    // Get reference to main FBO and initialise it
    cFboCore->InitConsoleFBO();
    // Redraw FBO
    SetRedraw();
  }
  /* -- Init console font and texture -------------------------------------- */
  void InitConsoleFontAndTexture(void)
  { // Initialise the console font and texture
    InitConsoleFont();
    InitConsoleTexture();
  }
  /* -- Print a string using textures -------------------------------------- */
  void Init(void)
  { // Class intiialised
    IHInitialise();
    // Log progress
    cLog->LogDebugExSafe("Console initialising with $ built-in commands...",
      conLibList.size());
    // Reset cursor position
    clriPosition = rbegin();
    // Graphical mode?
    if(cSystem->IsGraphicalMode())
    { // If we have terminal console as well we refresh both console otherwise
      // we refresh only the graphical console
      rfDefault.FlagReset(cSystem->IsTextMode() ?
        RD_GRAPHICS|RD_TEXT : RD_GRAPHICS);
      // Load console texture
      InitConsoleFontAndTexture();
    } // Only text console is redrawn
    else rfDefault.FlagReset(RD_TEXT);
    // Redraw the console
    SetRedraw();
    // Initially shown and not closable
    FlagSet(CF_CANTDISABLE|CF_ENABLED|CF_INSERT);
    // Register lua events
    cEvtMain->RegisterEx(reEvents);
    // Show version information
    PrintVersion();
    // Iterate each item and register it
    for(const ConLib &clD : conLibList)
      // Register command if the required core flags are set
      if(cSystem->IsCoreFlagsHave(clD.cfRequired))
        RegisterCommand(clD.strName, clD.uiMinimum, clD.uiMaximum,
          clD.ccbFunc);
      // Write in log to say we skipped registration of this command
      else cLog->LogDebugExSafe(
        "Console ignoring registration of command '$'.", clD.strName);
    // Say how many commands we registered
    cLog->LogInfoExSafe("Console initialised with $ of $ built-in commands.",
      llCmds.size(), conLibList.size());
  }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if already deinitialised
    if(IHNotDeInitialise()) return;
    // Log progress
    cLog->LogDebugSafe("Console de-initialising...");
    // Remove lua events
    cEvtMain->UnregisterEx(reEvents);
    // Initially shown and not closable. All other flags removed.
    FlagReset(CF_CANTDISABLE|CF_ENABLED|CF_INSERT);
    // If commands registered?
    switch(const size_t stCount = llCmds.size())
    { // Impossible?
      case 0: break;
      // Anything else?
      default:
        // Remove all commands quickly and report it
        cLog->LogDebugExSafe("Console flushing $ commands...", stCount);
        llCmds.clear();
        cLog->LogDebugExSafe("Console flushed $ commands.", stCount);
        // Done
        break;
    } // Unload console texture and font.
    DeInitTextureAndFont();
    // Remove font ftf
    GetFontRef().ftfData.DeInit();
    // Clear console input, output and input history
    DoClearInput();
    ClearHistory();
    DoFlush();
    // Log progress
    cLog->LogInfoSafe("Console de-initialised successfully.");
  }
  /* -- Set page move count ------------------------------------------------ */
  CVarReturn SetPageMoveCount(const ssize_t sstAmount)
  { // Deny if invalid value
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(sstPageLines, sstAmount, 1, 100)))
      return DENY;
    // Set negative too
    sstPageLinesNeg = -sstPageLines;
    // Success
    return ACCEPT;
  }
  /* -- Set time format ---------------------------------------------------- */
  CVarReturn SetTimeFormat(const string &strFmt, string &)
  { // Verify the new time format and log it
    cLog->LogInfoExSafe("Console time from format '$' is '$'.",
      strFmt, cmSys.FormatTime(strFmt.c_str()));
    // Accept the new time format
    strTimeFormat = strFmt;
    // Done
    return ACCEPT;
  }
  /* -- Set maximum console line length ------------------------------------ */
  CVarReturn SetMaxInputChars(const size_t stChars)
  { // Deny if out of range
    if(stChars < 256 ||
       stChars > UtilMinimum(UtilMinimum(strConsoleBegin.max_size(),
                                         strConsoleEnd.max_size()), 16384))
      return DENY;
    // Reserve buffer sizes for console input
    strConsoleBegin.reserve(stChars);
    strConsoleEnd.reserve(stChars);
    // Set new maximum length
    stMaxInputLine = stChars;
    // Redraw the buffer, it changed
    SetRedraw();
    // Value allowed
    return ACCEPT;
  }
  /* -- Set maximum console line length ------------------------------------ */
  CVarReturn SetMaxOutputChars(const size_t stChars)
  { // Check, set the new value and return if not acceptable
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(stMaxOutputLine, stChars,
      1024UL, 65536UL)))
        return DENY;
    // Set subtracted value from ellipsis size
    stMaxOutputLineE = stMaxOutputLine - cCommon->Ellipsis().length();
    // Value accepted
    return ACCEPT;
  }
  /* -- Set console buffers ------------------------------------------------ */
  CVarReturn SetConsoleOutputLines(const size_t stLines)
  { // Ignore if same value or invalid value or vector can't accept value
    if(stLines < 1 || stLines > 1000000 || stLines > max_size()) return DENY;
    // New value under previous amount? Remove excess lines
    if(stLines < size())
      clriPosition = ConLinesConstRevIt{ erase(cbegin(), next(cbegin(),
        static_cast<ssize_t>(size() - stLines))) };
    // Set new position
    stOutputMaximum = stLines;
    // Completed successfully
    return ACCEPT;
  }
  /* -- Set console buffers ------------------------------------------------ */
  CVarReturn SetConsoleInputLines(const size_t stLines)
  { // OK if same value
    if(GetInputMaximum() == stLines) return ACCEPT;
    // Ignore if same value or invalid value or vector can't accept value
    if(stLines < 1 || stLines > 1000000 || stLines > slHistory.max_size())
      return DENY;
    // New value under previous amount? Erase the rest
    if(stLines < slHistory.size())
      slriInputPosition =
        StrListConstRevIt{ slHistory.erase(slHistory.cbegin(),
          next(slHistory.cbegin(),
            static_cast<ssize_t>(slHistory.size() - stLines))) };
    // Set the new amount
    stInputMaximum = stLines;
    // Completed
    return ACCEPT;
  }
  /* -- Set console auto complete flags ------------------------------------ */
  CVarReturn SetAutoComplete(const AutoCompleteFlagsType acftFlags)
  { // Deny if flags are too much
    if(acftFlags != AC_NONE && (acftFlags & ~AC_MASK)) return DENY;
    // Set new flags
    acFlags.FlagReset(acftFlags);
    // Success
    return ACCEPT;
  }
  /* -- Set console auto copy cvar state ----------------------------------- */
  CVarReturn SetAutoCopyCVar(const bool bState)
  { // Set autoscroll state
    FlagSetOrClear(CF_AUTOCOPYCVAR, bState);
    // Succeeded
    return ACCEPT;
  }
  /* -- Set console autoscroll state --------------------------------------- */
  CVarReturn SetAutoScroll(const bool bState)
  { // Set autoscroll state
    FlagSetOrClear(CF_AUTOSCROLL, bState);
    // Succeeded
    return ACCEPT;
  }
  /* -- Set console height ------------------------------------------------- */
  CVarReturn SetHeight(const GLfloat fHeight)
    { return CVarSimpleSetIntNLG(fConsoleHeight, fHeight, 0.1f, 1.0f); }
  /* -- Set Console status ------------------------------------------------- */
  CVarReturn CantDisableModified(const bool bState)
  { // Update flag and disabled status
    FlagSetOrClear(CF_CANTDISABLEGLOBAL, bState);
    SetCantDisable(FlagIsSet(CF_CANTDISABLEGLOBAL));
    // Done
    return ACCEPT;
  }
  /* -- Set console background colour -------------------------------------- */
  CVarReturn TextBackgroundColourModified(const uint32_t ulNewColour)
    { return CVarSimpleSetInt(ulBgColour, ulNewColour); }
  /* -- Set console text colour -------------------------------------------- */
  CVarReturn TextForegroundColourModified(const unsigned int uiNewColour)
    { return CVarSimpleSetIntNG(cTextColour,
        static_cast<Colour>(uiNewColour), COLOUR_MAX); }
  /* -- Set console text scale --------------------------------------------- */
  CVarReturn TextScaleModified(const GLfloat fNewScale)
  { // Failed if supplied scale is not in range
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(fTextScale,
      fNewScale, 0.01f, 1.00f)))
        return DENY;
    // Set new font scale
    CommitScale();
    // Reallocate memory if neccesary for fbo lists
    RecalculateFboListReserves();
    // Succeeded reglardless of font availability
    return ACCEPT;
  }
  /* -- Set text letter spacing -------------------------------------------- */
  CVarReturn TextLetterSpacingModified(const GLfloat fNewSpacing)
  { // Failed if supplied spacing is not in range
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(fTextLetterSpacing,
      fNewSpacing, -256.0f, 256.0f))) return DENY;
    // Set console text line spacing if texture available
    CommitLetterSpacing();
    // Succeeded reglardless of font availability
    return ACCEPT;
  }
  /* -- Set text line spacing ---------------------------------------------- */
  CVarReturn TextLineSpacingModified(const GLfloat fNewSpacing)
  { // Failed if supplied spacing is not in range
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(fTextLineSpacing,
      fNewSpacing, -256.0f, 256.0f))) return DENY;
    // Set console text line spacing if texture available
    CommitLineSpacing();
    // Succeeded reglardless of font availability
    return ACCEPT;
  }
  /* -- Set text width ----------------------------------------------------- */
  CVarReturn TextWidthModified(const GLfloat fNewWidth)
    { return CVarSimpleSetIntNG(fTextWidth, fNewWidth, 4096.0f); }
  /* -- Set text height ---------------------------------------------------- */
  CVarReturn TextHeightModified(const GLfloat fNewHeight)
    { return CVarSimpleSetIntNG(fTextHeight, fNewHeight, 4096.0f); }
  /* -- Set console input refresh rate ------------------------------------- */
  CVarReturn InputRefreshModified(const unsigned int uiMicroseconds)
    { if(uiMicroseconds < 1000 || uiMicroseconds > 1000000) return DENY;
      ciInputRefresh.CISetLimit(microseconds{ uiMicroseconds });
      return ACCEPT; }
  /* -- Set text-mode console refresh rate --------------------------------- */
  CVarReturn OutputRefreshModified(const unsigned int uiMilliseconds)
    { if(uiMilliseconds < 100 || uiMilliseconds > 1000) return DENY;
      ciOutputRefresh.CISetLimit(milliseconds{ uiMilliseconds });
      return ACCEPT; }
  /* -- Console font flags modfiied ---------------------------------------- */
  CVarReturn ConsoleFontFlagsModified(const ImageFlagsType iftFlags)
  { // Check that flags are valid
    if(iftFlags != IL_NONE && (iftFlags & ~FF_MASK)) return DENY;
    // Set console flags
    GetFontRef().FlagReset(iftFlags);
    // Success
    return ACCEPT;
  }
  /* -- Constructor -------------------------------------------------------- */
  explicit Console(const ConCmdStaticList &ccslDef) :
    /* -- Initialisers ----------------------------------------------------- */
    IHelper{ __FUNCTION__ },           // Init helper function name
    Flags{ CF_NONE },                  // No initial flags
    acFlags{ AC_NONE },                // No autocomplete flags
    clriPosition{ rbegin() },          // Input position at beginning
    slriInputPosition{                 // Init log position...
      slHistory.crend() },             // ...at beginning
    stInputMaximum(0),                 // No maximum input characters
    stOutputMaximum(0),                // No maximum output lines
    stMaxInputLine(0),                 // No maximum characters per line
    stMaxOutputLine(0),                // No maximum output line
    stMaxOutputLineE(0),               // No maximum output line with ellipsis
    sstPageLines(0),                   // No page up/down lines setting
    sstPageLinesNeg(0),                // No neg page up/down lines setting
    rfDefault{ RD_NONE },              // Default redraw initially set by Init
    rfFlags{ RD_NONE },                // Redraw type
    ulFgColour(                        // Set input text colour
      uiNDXtoRGB[COLOUR_YELLOW] |      // - Lookup RGB value for yellow
      0xFF000000),                     // - Force opaqueness
    ulBgColour(0),                     // No background colour
    cTextColour(COLOUR_WHITE),         // Default white text colour
    fTextScale(0.0f),                  // No font scale
    fConsoleHeight(0.0f),              // No console height
    fTextLetterSpacing(0.0f),          // No text letter spacing
    fTextLineSpacing(0.0f),            // No text line spacing
    fTextWidth(0.0f),                  // No text width
    fTextHeight(0.0f),                 // No text height
    ctConsole{ true },                 // Console texture on stand-by
    cfConsole{ true },                 // Console font on stand-by
    cCursor('|'),                      // Cursor shape
    conLibList{ ccslDef },             // Set default commands list
    /* --------------------------------------------------------------------- */
    reEvents{                          // Default events
      { EMC_LUA_PAUSE,  bind(&Console::OnLuaPause,    this, _1) },
      { EMC_LUA_RESUME, bind(&Console::OnLuaResume,   this, _1) },
      { EMC_CON_UPDATE, bind(&Console::OnForceRedraw, this, _1) },
      { EMC_CON_RESIZE, bind(&Console::OnResize,      this, _1) },
    }
    /* --------------------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Console, DeInit())
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Console)             // Disable copy constructor and operator
  /* ----------------------------------------------------------------------- */
} *cConsole = nullptr;                 // Pointer to static class
/* ------------------------------------------------------------------------- */
void Console::LuaCallbackStatic(const Args &aList)
  { cConsole->LuaCallback(aList); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
