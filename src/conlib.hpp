/* == CONLIB.HPP ======================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles all the console cvars and function callbacks.   ## */
/* ## Add new console functions into the conCbList global array at the    ## */
/* ## bottom of this file.                                                ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
namespace IfConLib {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCore;                // Using core namespace
/* == Command list (note the commands are already locked) ================== */
static void CBcmds(const Arguments &aList)
{ // Setup iterator to find items and return if no commands.
  const Console::LibList &cbCmds = cConsole->llCmds;
  if(cbCmds.empty()) return cConsole->AddLine("No commands are found!");
  // Set filter if specified and look for command and if we found one?
  const string &strFilter = aList.size() > 1 ? aList[1] : strBlank;
  Console::LibList::const_iterator ciItem{ cbCmds.lower_bound(strFilter) };
  if(ciItem != cbCmds.cend())
  { // Output string
    ostringstream osS;
    // Commands matched
    size_t stMatched = 0;
    // Build output string
    do
    { // If no match found? return original string
      const string &strKey = ciItem->first;
      if(strKey.compare(0, strFilter.size(), strFilter)) continue;
      // Increment matched counter
      ++stMatched;
      // Add command to command list
      osS << ' ' << strKey;
    } // Until no more commands
    while(++ciItem != cbCmds.cend());
    // Print output if we matched commands
    if(stMatched)
      return cConsole->AddLineEx("$:$.", PluraliseNum(stMatched,
        "matching command", "matching commands"), osS.str());
  } // No matches
  cConsole->AddLineEx("No match from $.",
    PluraliseNum(cbCmds.size(), "command", "commands"));
}
/* == Show cvars =========================================================== */
static void ShowCVarList(const Arguments &aList, const CVars::ItemMap &cimList)
{ // Ignore if empty
  if(cimList.empty())
    return cConsole->AddLine("No cvars exist in this category!");
  // Set filter if specified
  const string &strFilter = aList.size() > 1 ? aList[1] : strBlank;
  // Lock cvars list
  const LockGuard lgCVarsSync{ cCVars->mMutex };
  // Try to find the cvar outright first (only make work when not in release)
#if !defined(RELEASE)
  CVars::ItemMapItConst ciItem{ cimList.find(strFilter) };
  if(ciItem != cimList.cend())
  { // Get cvar data an flags
    const Item &cD = ciItem->second;
    // Categories to add
    StrList lCats{
      Format("Status for CVar '$'...\n"
             "- Registered by: $, Flags: 0x$$$, Callback: $.\n"
             "- Current Value<$> = \"$\".",
        strFilter, cD.FlagIsSet(TLUA) ? "Lua" : "Engine",
        hex, cD.FlagGet(), dec, cD.IsTriggerSet() ? "Set" : "None",
        cD.GetValueLength(), cD.GetValue()) };
    // Add default value if set
    if(cD.IsValueChanged())
      lCats.emplace_back(Format("- Default Value<$> = \"$\".",
        cD.GetDefLength(), cD.GetDefValue()));
    // Flags to test data
    const vector<pair<const char*const,
      const vector<pair<const CVarFlagsConst&, const char*const>>>> vcList{
    { "Types", {
      { TSTRING,      "String"       }, { TBOOLEAN,     "Boolean"      },
      { TFLOAT,       "Float"        }, { TINTEGER,     "Integer"      } } },
    { "Sources", {
      { SENGINE,      "Internal"     }, { SCMDLINE,     "CmdLine"      },
      { SAPPCFG,      "AppConfig"    }, { SUDB,         "Database"     } } },
    { "Permissions", {
      { PSYSTEM,      "AppManifest"  }, { PBOOT,        "CommandLine"  },
      { PUSR,         "User"         }                                 } },
    { "Conditions", {
      { CDEFLATE,     "Compressed"   }, { CPROTECTED,   "Encrypted"    },
      { CALPHA,       "Alpha",       }, { CNUMERIC,     "Numeric",     },
      { CNOTEMPTY,    "NotEmpty"     }, { CPOW2,        "PowerOfTwo"   },
      { CSAVEABLE,    "SaveToDB"     }, { CFILENAME,    "FileUntrusted"},
      { CTRUSTEDFN,   "FileTrusted", }, { CUNSIGNED,    "NotNegative", } } },
    { "Other Flags", {
      { CONFIDENTIAL, "Confidential" }, { OSAVEFORCE,   "ForceSave"    },
      { LOADED,       "LoadedFromDB" }, { COMMIT,       "Modified"     },
      { MTRIM,        "TrimSpaces"   }                                 } } };
    // Walk through test categories
    for(const auto &cItem : vcList)
    { // Joined list of flags
      StrList lDest;
      // Test each flag and add to list if set
      for(const auto &tItem : cItem.second)
        if(cD.FlagIsSet(tItem.first)) lDest.emplace_back(tItem.second);
      // Add joined list to ca`1tegories list
      lCats.emplace_back(Format("- $<$> = $.", cItem.first, lDest.size(),
        lDest.empty() ? string{ "None" } : Implode(lDest, ", ")));
    } // Print data about the cvar
    return cConsole->AddLine(Implode(lCats, "\n"));
  } // Try as a lower bound check?
  ciItem = cimList.lower_bound(strFilter);
#else
  // Try as a lower bound check?
  auto ciItem{ cimList.lower_bound(strFilter) };
#endif
  if(ciItem != cimList.cend())
  { // Formatted output
    Statistic tData;
    tData.Header("FLAGS").Header("NAME", false).Header("VALUE", false);
    // Number of variables matched and tokens mask
    size_t stMatched = 0;
    // Build output string
    do
    { // If no match found? return original string
      const string &strKey = ciItem->first;
      if(strKey.compare(0, strFilter.size(), strFilter)) continue;
      // Increment matched counter
      ++stMatched;
      // Get cvar data and flags
      const Item &cD = ciItem->second;
      // Add tokens
      tData.Data(EvaluateTokens({
        // Types
        { true, cD.FlagIsSet(CFILENAME)  ? 'F' :
               (cD.FlagIsSet(CTRUSTEDFN) ? 'T' :
               (cD.FlagIsSet(TSTRING)    ? 'S' :
               (cD.FlagIsSet(TINTEGER)   ? 'I' :
               (cD.FlagIsSet(TFLOAT)     ? 'N' :
               (cD.FlagIsSet(TBOOLEAN)   ? 'B' :
                                           '?'))))) },
        // Permissions
        { cD.FlagIsSet(PBOOT),        '1' },
        { cD.FlagIsSet(PSYSTEM),      '2' },
        { cD.FlagIsSet(PUSR),         '3' },
        // Sources
        { cD.FlagIsSet(SENGINE),      '6' },
        { cD.FlagIsSet(SCMDLINE),     '7' },
        { cD.FlagIsSet(SAPPCFG),      '8' },
        { cD.FlagIsSet(SUDB) ,        '9' },
        // Conditions and operations
        { cD.FlagIsSet(CUNSIGNED),    'U' },
        { cD.FlagIsSet(TLUA),         'L' },
        { cD.FlagIsSet(CONFIDENTIAL), 'C' },
        { cD.FlagIsSet(CPROTECTED),   'P' },
        { cD.FlagIsSet(CDEFLATE),     'D' },
        { cD.FlagIsSet(COMMIT),       'M' },
        { cD.FlagIsSet(LOADED),       'O' },
        { cD.FlagIsSet(CSAVEABLE),    'V' },
        { cD.FlagIsSet(OSAVEFORCE),   'Z' },
        { cD.FlagIsSet(CPOW2),        'W' },
        { cD.FlagIsSet(CNOTEMPTY),    'Y' },
        { cD.FlagIsSet(MTRIM),        'R' },
        { cD.IsTriggerSet(),      'K' }
      // Name and value
      })).Data(strKey).Data(cD.Protect());
    } // Until no more commands
    while(++ciItem != cimList.cend());
    // Print output if we matched commands
    if(stMatched)
      return cConsole->AddLineEx("$$ of $ matched.", tData.Finish(),
        stMatched, PluraliseNum(cimList.size(), "cvar", "cvars"));
  } // No matches
  cConsole->AddLineEx("No match from $.",
    PluraliseNum(cimList.size(), "cvar", "cvars"));
}
/* == Show cvar pending list =============================================== */
static void CBcvpend(const Arguments &aList)
  { ShowCVarList(aList, cCVars->GetInitialVarList()); }
/* == Show cvars =========================================================== */
static void CBcvars(const Arguments &aList)
  { ShowCVarList(aList, cCVars->GetVarList()); }
/* == Clean cvars database and write cvars to database ===================== */
static void CBcvclr(const Arguments &)
  { cConsole->AddLineExA(PluraliseNum(cCVars->Clean(),
      "cvar", "cvars"), " purged."); }
/* == Load cvars from database ============================================= */
static void CBcvload(const Arguments &)
  { cConsole->AddLineExA(PluraliseNum(cCVars->LoadFromDatabase(),
      "cvar", "cvars"), " reloaded."); }
/* == Write cvars to database ============================================== */
static void CBcvsave(const Arguments &)
  { cConsole->AddLineExA(PluraliseNum(cCVars->Save(),
      "cvar", "cvars"), " commited."); }
/* == Create a new private key ============================================= */
static void CBcvnpk(const Arguments &)
{ // Create the private key and if succeeded? Show the result
  if(cSql->CreatePrivateKey())
    cConsole->AddLineEx("New private key set and $ variables re-encoded!",
      cCVars->MarkAllEncodedVarsAsCommit());
  // Else failed so show the error
  else cConsole->AddLine("Failed to create new private key!");
}
/* == list monitors ======================================================== */
static void CBmlist(const Arguments &)
{ // Header (to be printed twice)
  Statistic tData;
  tData.Header("ID").Header("A").Header("POSX").Header("POSY").Header("HORI")
       .Header("VERT").Header("RATIO").Header("RD").Header("GD").Header("BD")
       .Header("TD").Header("HTZ").Header("DWIN").Header("DHIN").Header("SIZE")
       .Header("NAME", false);
  // Get monitor count and data and return if no monitors
  int iCount; GLFWmonitor*const*const mData = GlFWGetMonitors(iCount);
  if(!mData || !iCount) return cConsole->AddLine("No monitors detected!");
  // Get monitors list
  for(int iMonitorIndex = 0; iMonitorIndex < iCount; ++iMonitorIndex)
  { // Get monitor data
    GLFWmonitor *mPtr = mData[iMonitorIndex];
    // Write id number
    tData.DataN(iMonitorIndex)
         .Data(YesOrNo(cDisplay->GetMonitorId() == iMonitorIndex));
    // Write desktop position, ratio and name
    int iW, iH; glfwGetMonitorPos(mPtr, &iW, &iH);
    tData.DataN(iW).DataN(iH);
    // Get monitor size
    glfwGetMonitorPhysicalSize(mPtr, &iW, &iH);
    // Get and write current video mode
    const GLFWvidmode &vmData = *glfwGetVideoMode(mPtr);
    tData.DataN(vmData.width).DataN(vmData.height)
         .Data(ToRatio(vmData.width, vmData.height))
         .DataN(vmData.redBits).DataN(vmData.greenBits).DataN(vmData.blueBits)
         .DataN(vmData.redBits+vmData.greenBits+vmData.blueBits)
         .DataN(vmData.refreshRate)
         .DataN(static_cast<double>(iW)*0.0393701, 1)
         .DataN(static_cast<double>(iH)*0.0393701, 1)
         .DataN(sqrt(pow(iW,2)+pow(iH, 2))*0.0393701, 1)
         .Data(glfwGetMonitorName(mPtr));
  } // Write total monitors found
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(iCount, "monitor", "monitors"), " detected.");
}
/* == list video modes ===================================================== */
static void CBvmlist(const Arguments &clParam)
{ // Monitor number and return if invalid
  const int iMonitorSelected =
    clParam.size() == 1 ? 0 : ToNumber<int>(clParam[1]);
  if(iMonitorSelected < 0 || iMonitorSelected >= GlFWGetMonitorCount())
    return cConsole->AddLineEx("Invalid monitor $ specified!",
      iMonitorSelected);
  // Get number of modes for this monitor and return with message if no modes
  const int iMonitorCount = cDisplay->GetVideoModes(iMonitorSelected);
  if(!iMonitorCount)
    return cConsole->AddLineEx("No modes for monitor $!", iMonitorCount);
  // Header (to be printed twice)
  Statistic tData;
  tData.Header("VM#").Header("A").Header("HORI").Header("VERT")
       .Header("CD").Header("HTZ").Header("RD").Header("GD").Header("BD")
       .Header("RATIO ", false);
  // Total divided by 2 so we can have two pretty columns of video mode info
  const int iMonitorCountD2 =
    static_cast<int>(ceilf(static_cast<float>(iMonitorCount) / 2));
  // Add extra header if more than one mode
  if(iMonitorCountD2 > 1) tData.DupeHeader();
  // Enumerate video modes
  for(int iMonitorIndex = 0; iMonitorIndex < iMonitorCountD2; ++iMonitorIndex)
  { // Get resolution info
    const GLFWvidmode &vmLeft =
      cDisplay->GetVideoMode(iMonitorSelected, iMonitorIndex);
    // Print resolution
    tData.DataN(iMonitorIndex)
         .Data(YesOrNo(cDisplay->GetVideoModeId() == iMonitorIndex))
         .DataN(vmLeft.width).DataN(vmLeft.height)
         .DataN(vmLeft.redBits + vmLeft.greenBits + vmLeft.blueBits)
         .DataN(vmLeft.refreshRate).DataN(vmLeft.redBits)
         .DataN(vmLeft.greenBits).DataN(vmLeft.blueBits)
         .Data(ToRatio(vmLeft.width, vmLeft.height));
    // Get opposite number
    const int iMonitorIndex2 = iMonitorIndex + iMonitorCountD2;
    // Have another resolution to print aside?
    if(iMonitorIndex2 < iMonitorCount)
    { // Get another resolution info
      const GLFWvidmode &vmRight =
        cDisplay->GetVideoMode(iMonitorSelected, iMonitorIndex2);
      // Print resolution
      tData.DataN(iMonitorIndex2)
           .Data(YesOrNo(cDisplay->GetVideoModeId() == iMonitorIndex2))
           .DataN(vmRight.width).DataN(vmRight.height)
           .DataN(vmRight.redBits + vmLeft.greenBits + vmRight.blueBits)
           .DataN(vmRight.refreshRate).DataN(vmRight.redBits)
           .DataN(vmRight.greenBits).DataN(vmRight.blueBits)
           .Data(ToRatio(vmRight.width, vmRight.height));
    }
  } // Print number of video modes
  cConsole->AddLineEx("$$ supported on monitor #$.", tData.Finish(),
    PluraliseNum(iMonitorCount, "mode", "modes"), iMonitorSelected);
}
/* == Reset audio device =================================================== */
static void CBlreset(const Arguments &)
{ // Re-init lua and inform user of the result
  cConsole->AddLine(cLua->TryEventOrForce(EMC_LUA_REINIT) ?
    "Bypassing guest end routine and restarting execution!" :
    "Asking guest to end execution and restarting execution.");
}
/* == Lua manipulation ===================================================== */
static void CBlend(const Arguments &)
{ // Re-init lua and inform user of the result
  cConsole->AddLine(cLua->TryEventOrForce(EMC_LUA_END) ?
    "Bypassing guest end routine and going stand-by!" :
    "Asking guest to end execution and going stand-by.");
}
/* == Restart application ================================================== */
static void CBrestart(const Arguments &aList)
{ // Restart the process and inform user of the result
  cConsole->AddLine(aList.size() == 2 ?
    (cLua->TryEventOrForce(EMC_QUIT_RESTART_NP) ?
      "Bypassing guest end routine and restarting engine with no args!" :
      "Asking guest to end execution and restarting engine with no args.") :
    (cLua->TryEventOrForce(EMC_QUIT_RESTART) ?
      "Bypassing guest end routine and restarting engine!" :
      "Asking guest to end execution and restarting engine."));
}
/* == Quit the application ================================================= */
static void CBquit(const Arguments &)
{ // Quit the engine and inform user of the result
  cConsole->AddLine(cLua->TryEventOrForce(EMC_QUIT) ?
    "Bypassing guest end routine and terminating engine!" :
    "Asking guest to end execution and terminating engine.");
}
/* == Clear the console output buffer ====================================== */
static void CBcls(const Arguments &) { cConsole->Flush(); }
/* == Clear the console input buffer ======================================= */
static void CBclh(const Arguments &) { cConsole->ClearHistory(); }
/* == ScreenShot =========================================================== */
static void CBshot(const Arguments &) { cSShot->DumpMain(); }
/* == Reset audio device =================================================== */
static void CBareset(const Arguments &)
{ // Reset the audio subsystem and print result of the reset call
  cConsole->AddLineExA("Audio subsystem reset ",
    cAudio->ReInit() ? "requested." : "failed.");
}
/* == Reset video device =================================================== */
static void CBvreset(const Arguments &) { cEvtMain->Add(EMC_QUIT_THREAD); }
/* == Reset window position and size ======================================= */
static void CBwreset(const Arguments &) { cDisplay->RequestReposition(); }
/* == Reset audio device =================================================== */
static void CBlstack(const Arguments &)
{ // Setup output spreadsheet
  Statistic tData;
  tData.Header("ID").Header("FLAGS").Header("NAME", false).Header("VALUE");
  // Get lua state
  lua_State*const lS = cLua->GetState();
  // For each element (1 is the first item)
  const int iCount = GetStackCount(lS);
  for(int iIndex = 1; iIndex <= iCount; ++iIndex)
    tData.DataN(iIndex)
         .Data(GetStackTokens(lS, iIndex))
         .DataN(-(iCount - (iIndex + 1)))
         .Data(GetStackType(lS, iIndex));
  // Print number of element
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(iCount, "element.", "elements."));
}
/* == Lua globals list ===================================================== */
static void CBlvars(const Arguments &aList)
{ // Get lua state
  lua_State*const lS = cLua->GetState();
  // Save stack position and restore it when this function call completes. We
  // do this because we could write any number of values to the stack
  // depending on how many child variables the user specifies. This simple
  // class creation simplifies the cleanup process.
  const LuaStackSaver lssSaved{ lS };
  // We need free items on the stack, leave empty if not
  if(!StackHasCapacity(lS, aList.size()))
    return cConsole->AddLine("Too many path components!");
  // Get iterator to second argument. First is actually the command name. The
  // second argument in this instance is the root table name in globals.
  // Store if we have arguments and if we have them?
  auto clItem{ aList.cbegin() + 1 };
  if(clItem != aList.cend())
  { // Get root table
    const string &strRoot = *clItem;
    if(strRoot.empty()) return cConsole->AddLine("Empty table name!");
    // Push variable specified on command line and if it's not a table?
    // Tell user the table is invalid and return
    lua_getglobal(lS, strRoot.c_str());
    if(!lua_istable(lS, -1))
      return cConsole->AddLineEx("Table '$' $!", strRoot,
        lua_isnil(lS, -1) ? "does not exist" : "is not valid");
    // Save index so we can keep recursing the same table and check if each
    // remaining argument is a table until we reach no more arguments.
    for(const int iIndex = GetStackCount(lS); ++clItem != aList.cend();)
    { // Get name of parameter and if it's empty? Return empty sub-table
      const string &strParam = *clItem;
      if(strParam.empty()) return cConsole->AddLine("Empty sub-table name!");
      // ...and if its a valid number?
      if(IsNumber(strParam))
      { // Get value by index and keep searching for more tables
        GetReferenceEx(lS, -1, ToNumber<lua_Integer>(strParam));
        if(lua_istable(lS, -1)) continue;
        // Restore where we were in the stack
        PruneStack(lS, iIndex);
      } // Find subtable. It must be a table
      lua_getfield(lS, -1, strParam.c_str());
      if(lua_istable(lS, -1)) continue;
      // Tell user the table is invalid
      cConsole->AddLineEx("Sub-table '$' $!", strParam,
        lua_isnil(lS, -1) ? "does not exist" : "is not valid");
      // Done
      return;
    }
  } // Push global namspace and throw error if it is invalid
  else lua_pushglobaltable(lS);
  // Items for sorting (Name, Value, Tokens)
  map<const string, const pair<const string,const string>> smmList;
  // Make sure theres two elements
  for(PushNil(lS); lua_next(lS, -2); RemoveStack(lS))
  { // Index is an integer? Create item info struct and add to list
    if(lua_isinteger(lS, -2))
      smmList.insert({ ToString(lua_tointeger(lS, -2)),
        { GetStackType(lS, -1), GetStackTokens(lS, -1) } });
    // For everything else. Create item info struct and add to list
    else smmList.insert({ lua_tostring(lS, -2),
      { GetStackType(lS, -1), GetStackTokens(lS, -1) } });
  } // Build string to output
  Statistic tData;
  tData.Header("FLAGS").Header("NAME", false).Header("VALUE", false);
  for(const auto &sprIt : smmList)
    tData.Data(sprIt.second.second)
         .Data(sprIt.first)
         .Data(sprIt.second.first);
  // Print number of items
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(smmList.size(), "item.", "items."));
}
/* == Lua manipulation ===================================================== */
static void CBlexec(const Arguments &aList)
  { cConsole->AddLine(cLua->CompileStringAndReturnResult(Implode(aList, 1))); }
/* == Lua manipulation ===================================================== */
static void CBlgc(const Arguments &)
{ // Free data and get bytes freed
  const size_t stT = cLua->GarbageCollect();
  // Return if no bytes freed
  if(!stT) return cConsole->AddLine("No unreferenced memory!");
  // Report how much was freed
  cConsole->AddLineEx("$ bytes ($) freed.", stT, ToBytesStr(stT));
}
/* == Pause lua execution ================================================== */
static void CBlpause(const Arguments &) { cEvtMain->Add(EMC_LUA_PAUSE); }
/* == Resume lua execution ================================================= */
static void CBlresume(const Arguments &) { cEvtMain->Add(EMC_LUA_RESUME); }
/* == Lua function callback list =========================================== */
static void CBlfuncs(const Arguments &)
{ // Get reference to luarefs collector class and lock it so it's not changed
  const LockGuard lgLuaRefsSync(cLuaFuncs->CollectorGetMutex());
  // Make a table to automatically format our data neatly
  Statistic tData;
  tData.Header("ID").Header("FLAGS").Header("DATA", false).Header("MID")
       .Header("SID").Header("NAME", false);
  // Walk sample list
  for(const LuaFunc*const lCptr : *cLuaFuncs)
  { // Get sample
    const LuaFunc &lCref = *lCptr;
    // Put on stack
    lCref.LuaFuncPushFunc();
    // Print totals info
    tData.DataN(lCref.CtrGet())
         .Data(GetStackTokens(cLuaFuncs->LuaRefGetState(), -1))
         .Data(GetStackType(cLuaFuncs->LuaRefGetState(), -1))
         .DataN(lCref.LuaFuncGet()).DataN(lCref.LuaFuncGetSaved())
         .Data(lCref.IdentGet());
    // Remove what we pushed
    RemoveStack(cLuaFuncs->LuaRefGetState());
  } // Number of items in buffer
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cLuaFuncs->size(), "function.", "functions."));
}
/* == Lua manipulation ===================================================== */
static void CBsockreset(const Arguments &aList)
{ // Get parameter and if user requested to close all connections? Close all
  // the sockets and report how many we closed and return
  const string &strId = aList[1];
  if(strId == "*")
    return cConsole->AddLineExA(PluraliseNum(SocketReset(),
      "connection", "connections"), " closed.");
  // Parse the specified id. Try to find the FD specified and return the class
  const unsigned int uiId = ToNumber<unsigned int>(strId);
  const auto sItem{ SocketFind(uiId) };
  // Report if socket was not matched, or the result of disconnection.
  if(sItem == cSockets->cend())
    cConsole->AddLine("The specified socket id was not matched!");
  else cConsole->AddLineEx("Connection $ $ closed.",
    uiId, ((*sItem)->SendDisconnect() ? "has been" : "is already"));
}
/* == Show Sockets Information ============================================= */
static void CBsockets(const Arguments &aList)
{ // Id specified?
  if(aList.size() == 2)
  { // Parse the specified id
    const unsigned int uiId = ToNumber<unsigned int>(aList[1]);
    // For each socket. Try to find the FD specified and return the class
    // if we can't find, tell the console we could not find it.
    const auto sItem{ SocketFind(uiId) };
    if(sItem == cSockets->cend())
      return cConsole->AddLine("The specified socket id was not matched.");
    // Get reference to socket
    const Socket &sCref = **sItem;
    // Get socket flags
    const SocketFlagsConst sfFlags{ sCref.FlagGet() };
    // Tokens for status
    const string strStatus
        ((sfFlags.FlagIsSet(SS_CLOSEDBYCLIENT)) ? "ClientClosed"
      : ((sfFlags.FlagIsSet(SS_CLOSEDBYSERVER)) ? "ServerClosed"
      : ((sfFlags.FlagIsSet(SS_STANDBY))        ? "Disconnected"
      : ((sfFlags.FlagIsSet(SS_DISCONNECTING))  ? "Disconnecting"
      : ((sfFlags.FlagIsSet(SS_DOWNLOADING))    ? "Downloading"
      : ((sfFlags.FlagIsSet(SS_REPLYWAIT))      ? "ReplyWait"
      : ((sfFlags.FlagIsSet(SS_SENDREQUEST))    ? "SendRequest"
      : ((sfFlags.FlagIsSet(SS_CONNECTED))      ? "Connected"
      : ((sfFlags.FlagIsSet(SS_CONNECTING))     ? "Connecting"
      : ((sfFlags.FlagIsSet(SS_INITIALISING))   ? "Initialising"
      :                                  "Unknown"))))))))));
    // If the socket is not connected?
    if(sfFlags.FlagIsClear(SS_CONNECTED))
      return cConsole->AddLineEx("Status for socket $...\n"
        "- Status: $; Flags: 0x$$; Error: $$; Descriptor: $ (0x$$).\n"
        "- Address: $; Port: $$; IP: $.\n"
        "- Encryption: $; Last: $.",
          uiId,
          strStatus, hex, sfFlags.FlagGet(), dec, sCref.GetError(),
            sCref.GetFD(), hex, sCref.GetFD(),
          sCref.GetAddress(), dec, sCref.GetPort(), sCref.GetIPAddress(),
          TrueOrFalse(sCref.IsSecure()), IfBlank(sCref.GetCipher()));
     // Use disconnect time or connect time
    const double fdConnect = sfFlags.FlagIsSet(SS_STANDBY) ?
      ClockTimePointRangeToClampedDouble(sCref.GetTDisconnected(),
        sCref.GetTConnected()) :
      cmHiRes.TimePointToClampedDouble(sCref.GetTConnected()),
    // Set connected to connection time
    fdInitial = ClockTimePointRangeToClampedDouble(sCref.GetTConnected(),
      sCref.GetTConnect());
    // Write connected information
    return cConsole->AddLineEx("Status for socket $...\n"
      "- Status: $; Flags: 0x$$; Error: $$; Descriptor: $ (0x$$).\n"
      "- Address: $; Port: $$; IP: $.\n"
      "- Real host: $.\n"
      "- RX Queue: $; Packets: $; Bytes: $ ($); Last: $ ago.\n"
      "- TX Queue: $; Packets: $; Bytes: $ ($); Last: $ ago.\n"
      "- Encryption: $; Last: $.\n"
      "- Total Time: $; Connected: $; Initial: $.",
      uiId,
      strStatus, hex, sfFlags.FlagGet(), dec, sCref.GetError(),
        sCref.GetFD(), hex, sCref.GetFD(),
      sCref.GetAddress(), dec, sCref.GetPort(), sCref.GetIPAddress(),
      IfBlank(sCref.GetRealHost(), "<Unresolved>"),
      sCref.GetRXQCount(), sCref.GetRXpkt(), sCref.GetRX(),
        ToBytesStr(sCref.GetRX()),
        ToShortDuration(cmHiRes.TimePointToClampedDouble(sCref.GetTRead())),
      sCref.GetTXQCount(), sCref.GetTXpkt(), sCref.GetTX(),
        ToBytesStr(sCref.GetTX()),
        ToShortDuration(cmHiRes.TimePointToClampedDouble(sCref.GetTWrite())),
      TrueOrFalse(sCref.IsSecure()), IfBlank(sCref.GetCipher()),
      ToShortDuration(fdConnect + fdInitial),
        ToShortDuration(fdConnect), ToShortDuration(fdInitial));
  } // Make neatly formatted table
  Statistic tData;
  tData.Header("ID").Header("FLAGS").Header("IP").Header("PORT")
       .Header("ADDRESS", false);
  // Walk through sockets
  for(const Socket*const oCptr : *cSockets)
  { // Get pointer to class
    const Socket &sCref = *oCptr;
    // Get status
    const SocketFlagsConst ssType{ sCref.FlagGet() };
    // Show status
    tData.DataN(sCref.CtrGet())
         .Data(EvaluateTokens({
            { ssType.FlagIsSet(SS_INITIALISING),  'I' },
            { ssType.FlagIsSet(SS_ENCRYPTION),    'X' },
            { ssType.FlagIsSet(SS_CONNECTING),    'T' },
            { ssType.FlagIsSet(SS_CONNECTED),     'O' },
            { ssType.FlagIsSet(SS_SENDREQUEST),   'H' },
            { ssType.FlagIsSet(SS_REPLYWAIT),     'R' },
            { ssType.FlagIsSet(SS_DOWNLOADING),   'D' },
            { ssType.FlagIsSet(SS_DISCONNECTING), 'N' },
            { ssType.FlagIsSet(SS_STANDBY),       'B' },
            { sCref.GetError() != 0,          'E' },
            { ssType.FlagIsSet(SS_CLOSEDBYSERVER), 'S' },
            { ssType.FlagIsSet(SS_CLOSEDBYCLIENT), 'C' } }))
         .Data(sCref.GetIPAddress())
         .DataN(sCref.GetPort())
         .Data(sCref.GetAddress());
  } // Show result
  cConsole->AddLineEx("$$ ($ connected).\n"
    "Total RX Packets: $; Bytes: $ ($).\n"
    "Total TX Packets: $; Bytes: $ ($).",
    tData.Finish(), PluraliseNum(cSockets->size(), "socket", "sockets"),
    cSockets->stConnected.load(),
    cSockets->qRXp.load(), cSockets->qRX.load(),
      ToBytesStr(cSockets->qRX.load()),
    cSockets->qTXp.load(), cSockets->qTX.load(),
      ToBytesStr(cSockets->qTX.load()));
}
/* == Sql manipulation ===================================================== */
static void CBsqlexec(const Arguments &aList)
{ // Don't continue if theres a transaction in progress
  if(cSql->Active())
    return cConsole->AddLine("Sql transaction already active!");
  // Execute the string and catch exceptions
  if(cSql->ExecuteAndSuccess(Implode(aList, 1)))
  { // Get records and if we did not have any?
    const Sql::Result &vData = cSql->GetRecords();
    if(vData.empty())
    { // If we should show the rows affected. This is sloppy but sqllite
      // doesn't support resetting sqlite3_changes result yet :(
      const string &strFirst = ToLowerRef(ToNonConst(aList[1]));
      // Show rows affected if we have them
      if(strFirst == "insert" || strFirst == "update" || strFirst == "delete")
        cConsole->AddLineEx("$ affected in $.",
          PluraliseNum(cSql->Affected(), "record", "records"),
            cSql->TimeStr());
      // Show time
      else if(cSql->Time() > 0)
        cConsole->AddLineEx("Query time was $.", cSql->TimeStr());
    } // Results were returned?
    else
    { // Data for output
      Statistic tData;
      tData.Header("R#").Header("C#").Header("SIZE").Header("KEY", false)
           .Header("T").Header("VALUE", false);
      // Record id
      size_t stRecordId = 0;
      // For each record, print the column and value of each record.
      for(const Sql::Records &vRecord : vData)
      { // Column id
        size_t stColumnId = 0;
        // For each column in the record
        for(const auto &cItem : vRecord)
        { // Get reference to the record data
          const Sql::DataListItem &mbOut = cItem.second;
          // Add row, column id, size and key name
          tData.DataN(stRecordId).DataN(stColumnId++)
               .DataN(mbOut.Size()).Data(cItem.first);
          // What type is it?
          switch(mbOut.iT)
          { // 64-bit integer?
            case SQLITE_INTEGER:
            { // Get integer
              const sqlite3_int64 qwVal = mbOut.ReadInt<sqlite3_int64>();
              // Format and store in spreadsheet
              tData.Data("I")
                   .Data(Format("$$ (0x$$$)", dec, qwVal, hex, qwVal, dec));
              // Done
              break;
            } // 64-bit IEEE float?
            case SQLITE_FLOAT:
              tData.Data("F").DataN(mbOut.ReadInt<double>()); break;
            // Raw data? Just write number of bytes
            case SQLITE_BLOB: tData.Data("B").Data("<Blob>"); break;
            // Text?
            case SQLITE_TEXT: tData.Data("T").Data(mbOut.ToString()); break;
            // No data
            case SQLITE_NULL: tData.Data("?").Data("<Null>"); break;
            // Unknown type
            default:
              tData.Data("?").Data(Append("Unknown type ", mbOut.iT)); break;
          }
        } // Increase record number
        stRecordId++;
      } // Show number of records matched and rows affected
      cConsole->AddLineExA(tData.Finish(), Format("$ matched in $.",
        PluraliseNum(stRecordId, "record", "records"), cSql->TimeStr()));
    }
  } // Error occured?
  else cConsole->AddLineEx("Query took $ with $<$>: $!", cSql->TimeStr(),
    cSql->GetErrorAsIdString(), cSql->GetError(), cSql->GetErrorStr());
  // Reset sql transaction because it will leave arrays registered
  cSql->Reset();
}
/* == Check database ======================================================= */
static void CBsqlcheck(const Arguments &)
{ // Don't continue if theres a transaction in progress
  if(cSql->Active())
    return cConsole->AddLine("Sql transaction already active!");
  // Compact the database and print the reason if failed
  cConsole->AddLine(cSql->CheckIntegrity() ?
    "UDB check has passed!" : cSql->GetErrorStr());
  // Reset sql transaction because it will leave arrays registered
  cSql->Reset();
}
/* == End a currently in progress transaction ============================== */
static void CBsqlend(const Arguments &)
{ // Don't continue if theres a transaction in progress
  if(!cSql->Active()) return cConsole->AddLine("Sql transaction not active!");
  // End transaction
  cConsole->AddLineEx("Sql transaction$ ended.",
    cSql->End() == SQLITE_OK ? strBlank : " NOT");
}
/* == Optimise databsae ==================================================== */
static void CBsqldefrag(const Arguments &)
{ // Don't continue if theres a transaction in progress
  if(cSql->Active())
    return cConsole->AddLine("Sql transaction already active!");
  // Get current size
  const uint64_t qBefore = cSql->Size();
  // Do the defrag and say why if failed
  if(cSql->Optimise()) return cConsole->AddLine(cSql->GetErrorStr());
  // Get elapsed time (don't move this, or ->Reset() clears the duration).
  const double dT = cSql->Time();
  // Clear up result as it allocates arrays
  cSql->Reset();
  // Get new size in bytes
  const uint64_t qAfter = cSql->Size();
  // Set changed bytes
  const int64_t qChange = static_cast<int64_t>(qAfter - qBefore);
  // Get change percentile
  const double dPC = -100.0 + MakePercentage(qAfter, qBefore);
  // Write summary
  cConsole->AddLine(qChange ?
    // If udb did change?
    Format("UDB changed $B ($; $$$%) to $B ($) from $B ($) in $.",
      qChange, ToBytesStr(-qChange,0), setprecision(2), fixed, dPC, qAfter,
      ToBytesStr(qAfter,0), qBefore, ToBytesStr(qBefore,0),
      ToShortDuration(dT)) :
    // Udb did not change so show the result
    Format("UDB unchanged at $B ($) in $.",
      qAfter, ToBytesStr(qAfter,0), ToShortDuration(dT)));
}
/* == Engine calculation =================================================== */
static void CBlcalc(const Arguments &aList)
  { cConsole->AddLine(cLua->CompileStringAndReturnResult(
      Format("return $", Implode(aList, 1)))); }
/* == Show audio output list =============================================== */
static void CBaudouts(const Arguments &)
{ // Ignore if no audio subsystem initialised
  if(!cOal->IsInitialised())
    return cConsole->AddLine("Audio not initialised.");
  // Data for device output
  Statistic tData;
  tData.Header("ID").Header("OUTPUT DEVICE", false);
  // list audio devices
  size_t stDeviceId = 0;
  for(const string &strName : cAudio->GetPBDevices())
    tData.DataN(stDeviceId++).Data(strName);
  // Output devices
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(stDeviceId, "device.", "devices."));
}
/* == Show audio input list ================================================ */
static void CBaudins(const Arguments &)
{ // Ignore if no audio subsystem initialised
  if(!cOal->IsInitialised())
    return cConsole->AddLine("Audio not initialised.");
  // Data for device output
  Statistic tData;
  tData.Header("ID").Header("INPUT DEVICE", false);
  // list audio devices
  size_t stDeviceId = 0;
  for(const string &strName : cAudio->GetCTDevices())
    tData.DataN(stDeviceId++).Data(strName);
  // Output devices
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(stDeviceId, "device.", "devices."));
}
/* == Show Audio Information =============================================== */
static void CBaudio(const Arguments &)
{ // Write information about current audio device
  cConsole->AddLineEx(
     "Library: OpenAL $; Flags: $ (0x$$$).\n"
     "Playback: $.\n"
     "Sources: $ allocated (Max $ Stereo; $ Mono).",
    cOal->GetVersion(),
    EvaluateTokens({
      { cOal->FlagIsSet(AFL_INITCONTEXT),     'C' },
      { cOal->FlagIsSet(AFL_INITDEVICE),      'D' },
      { cOal->FlagIsSet(AFL_HAVE32FPPB),      'F' },
      { cOal->FlagIsSet(AFL_INITIALISED),     'I' },
      { cOal->FlagIsSet(AFL_INFINITESOURCES), 'S' },
      { cOal->FlagIsSet(AFL_HAVEENUMEXT),     'X' },
    }),
    hex, cOal->FlagGet(), dec,
    cOal->GetPlaybackDevice(),
    cSources->size(), cOal->GetMaxStereoSources(), cOal->GetMaxMonoSources());
}
/* == Show Loaded Memory Blocks Information ================================ */
static void CBassets(const Arguments &)
{ // Get reference to arrays collector class and lock it so it's not changed
  const LockGuard lgAssetsSync{ cAssets->CollectorGetMutex() };
  // Size of all blocks
  size_t stTotal = 0;
  // Onnly show previews in BETA or ALPHA version
#if !defined(RELEASE)
  // Number of bytes to show
  const size_t stCount = 16;
  // Preview of block and reserve memory for it
  string strHex; strHex.reserve(stCount * 3);
  string strAscii; strAscii.reserve(stCount);
  // Memory data
  Statistic tData;
  tData.Header("ID").Header("BYTES").Header("PREVIEW", false)
       .Header("ASCII", false);
#endif
  // Walk mask list and add size of it
  for(const Asset *aCptr : *cAssets)
  { // Get reference to memory block
    const DataConst &dcCref = *aCptr;
#if !defined(RELEASE)
    // Set preview size
    const size_t stMax = Minimum(dcCref.Size(), stCount);
    // Set id and size
    tData.DataN(aCptr->CtrGet()).DataN(dcCref.Size());
    // Clear preview strings
    strHex.clear();
    strAscii.clear();
    // Show first sixteen bytes
    for(size_t stIndex = 0; stIndex < stMax; ++stIndex)
    { // Get character
      const unsigned int uiChar =
        static_cast<unsigned int>(dcCref.ReadInt<uint8_t>(stIndex));
      // Add hex of block
      strHex += ToHex(uiChar, 2) + ' ';
      // Put a dot if character is invalid
      if(uiChar < ' ') { strAscii += '.'; continue; }
      // Encode the character
      AppendString(uiChar, strAscii);
    } // Add padding for characters that don't exist
    for(size_t stIndex = stMax; stIndex < stCount; ++stIndex)
      { strHex += ".. "; strAscii += ' '; }
    // Remove space on hex
    strHex.pop_back();
    // Write line to log
    tData.Data(strHex).Data(strAscii);
#endif
    // Add size of this array to the total size of all arrays
    stTotal += dcCref.Size();
  } // Number of items in buffer. We're not showing data in release mode.
#if defined(RELEASE)
  cConsole->AddLineEx("$ totalling $.",
    PluraliseNum(cAssets->size(), "block", "blocks"),
    PluraliseNum(stTotal, "byte", "bytes"));
#else
  cConsole->AddLineEx("$$ totalling $.", tData.Finish(),
    PluraliseNum(cAssets->size(), "block", "blocks"),
    PluraliseNum(stTotal, "byte", "bytes"));
#endif
}
/* == Show loaded objects information ====================================== */
static void CBobjs(const Arguments &)
{ // Typedefs for building memory usage data
  struct MemoryUsageItem
    { const string &strName; size_t stCount, stBytes; }
      muiTotal{ strBlank, 0, 0 };
  const string strCmds{"Cmds"}, strCVars{"CVars"};
  typedef list<MemoryUsageItem> MemoryUsageItems;
  // Helper macros so there is not as much spam
#define MSSEX(x,f,i) { x, f, f * sizeof(i) }
#define MSS(x) \
    MSSEX(c ## x ## s->IdentGet(), c ## x ## s->CollectorCount(), x)
  // Build memory usage items database
  MemoryUsageItems muiData{ {
    MSS(Archive), MSS(Asset),   MSS(Bin),
    MSSEX(strCmds, cConsole->GetCmdCount(), ConLib),        MSS(Cursor),
    MSSEX(strCVars,cCVars->GetVarCount(),   Item),          MSS(Fbo),
    MSS(File),    MSS(Font),    MSS(Ftf),     MSS(Image),   MSS(ImageFmt),
    MSS(Json),    MSS(LuaFunc), MSS(Mask),    MSS(Palette), MSS(Pcm),
    MSS(PcmFmt),  MSS(Sample),  MSS(Shader),  MSS(Socket),  MSS(Source),
    MSS(Stat),    MSS(Stream),  MSS(Texture), MSS(Thread),  MSS(Video)
  } };
  // Done with these macros
#undef MSS
#undef MSSEX
  // Prepare statistics data
  Statistic stData;
  stData.Header("TYPE").Header("#").Header("STACK").Header().DupeHeader(2);
  // Add entries
  for(const auto &muiPair : muiData)
  { // Add sizes to total
    muiTotal.stCount += muiPair.stCount;
    muiTotal.stBytes += muiPair.stBytes;
    // Add data to table
    stData.Data(muiPair.strName)
          .DataN(muiPair.stCount)
          .DataN(muiPair.stBytes)
          .Data(Append('(', ToBytesStr(muiPair.stBytes, 0),')'));
  } // Display output to user
  cConsole->AddLineEx("$$ totalling $ ($).", stData.Finish(),
    PluraliseNum(muiTotal.stCount, "object", "objects"),
    PluraliseNum(muiTotal.stBytes, "byte", "bytes"),
    ToBytesStr(muiTotal.stBytes, 0));
}
/* == Show Loaded Modules Information ====================================== */
static void CBmods(const Arguments &)
{ // Make a table to automatically format our data neatly
  Statistic tData;
  tData.Header("DESCRIPTION").Header("VERSION", false)
       .Header("VENDOR", false).Header("MODULE", false);
   // Re-enumerate modules
  cSystem->EnumModules();
  // list modules
  for(const auto &mD : *cSystem)
  { // Get mod data
    const SysModuleData &smdData = mD.second;
    tData.Data(smdData.GetDesc()).Data(smdData.GetVersion())
         .Data(smdData.GetVendor()).Data(smdData.GetFull());
  } // Finished enumeration of modules
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cSystem->size(), "module.", "modules."));
}
/* == Show Loaded Sample Information ======================================= */
static void CBsamples(const Arguments &)
{ // Make a table to automatically format our data neatly
  Statistic tData;
  tData.Header("ID").Header("BID").Header("NAME", false);
  // Walk sample object list
  for(const Sample*const sCptr : *cSamples)
  { // Get sample object reference
    const Sample &sCref = *sCptr;
    // Print totals info
    tData.DataN(sCref.CtrGet()).DataN(sCref.front()).Data(sCref.IdentGet());
  } // Number of items in buffer
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cSamples->size(), "sample.", "samples."));
}
/* == Show Loaded Shaders Information ====================================== */
static void CBshaders(const Arguments &)
{ // Make a table to automatically format our data neatly
  Statistic tData;
  tData.Header("ID").Header("PID").Header("L").Header("C").Header("SID")
       .Header("TYPE").Header("CODE").Header("NAME", false);
  // Shader count
  size_t stShaders = 0;
  // Walk shader list
  for(const Shader*const sCptr : *cShaders)
  { // Get shader and its list
    const Shader &sCref = *sCptr;
    // Store shader information
    tData.DataN(sCref.CtrGet()).DataN(sCref.GetProgramId())
         .Data(YesOrNo(sCref.IsLinked())).DataN(sCref.size())
         .Data().Data().Data().Data();
    // For each shader item, add data for each shader
    for(const ShaderCell &sItem : sCref)
      tData.Data().Data().Data().Data().DataN(sItem.GetHandle())
           .DataH(sItem.GetType()).DataN(sItem.GetCodeLength())
           .Data(sItem.IdentGet());
    // Increment number of shaders
    stShaders = stShaders + sCref.size();
  } // Print output and number of shaders listed
  cConsole->AddLineEx("$$ and $.", tData.Finish(),
    PluraliseNum(cShaders->size(), "program", "programs"),
    PluraliseNum(stShaders, "shader", "shaders"));
}
/* == Show root certificates =============================================== */
static void CBcerts(const Arguments &)
{ // Make a table to automatically format our data neatly
  Statistic tData;
  tData.Header("X").Header("FILE", false).Header("C", false).
    Header("CN", false);
  // Walk shader list
  for(const auto &caPair : cSockets->GetCertList())
  { // Put if expired and the filename on disk
    tData.Data(YesOrNo(CertIsExpired(caPair.second)))
         .Data(caPair.first);
    // Split subject key/value pairs. We couldn't split the data if empty
    const VarsConst ssD{ CertGetSubject(caPair), "/", '=' };
    if(ssD.empty()) { tData.Data("??").Data("<No sub>"); continue; }
    // Print country and certificate name
    const StrStrMapConstIt iC{ ssD.find("C") }, iCN{ ssD.find("CN") };
    tData.Data(iC == ssD.cend() ? "--" : CryptURLDecode(iC->second)).
          Data(iCN == ssD.cend() ? "<Unknown>" : CryptURLDecode(iCN->second));
  } // Print output and number of shaders listed
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cSockets->GetCertListSize(),
      "root certificate.", "root certificates."));
}
/* == Show Threads Information ============================================= */
static void CBthreads(const Arguments &)
{ // Make a table to automatically format our data neatly
  Statistic tData;
  tData.Header("ID").Header("FLAG").Header("EC").Header("STTIME")
       .Header("ENTIME").Header("UPTIME").Header("IDENTIFIER", false);
  // For each thread pointer
  for(const Thread*const tCptr : *cThreads)
  { // Get reference to thread pointer
    const Thread &tCref = *tCptr;
    // Store thread information // RXPC
    tData.DataN(tCref.CtrGet()).Data(EvaluateTokens({
      { tCref.ThreadHaveCallback(), 'C' }, { tCref.ThreadIsParamSet(), 'P' },
      { tCref.ThreadIsRunning(),    'R' }, { tCref.ThreadShouldExit(), 'T' },
      { tCref.ThreadIsException(),  'E' }, { tCref.ThreadIsJoinable(), 'J' },
      { tCref.ThreadIsExited(),     'X' },
    })).DataN(tCref.ThreadGetExitCode())
       .Data(ToShortDuration(cLog->
         CCDeltaToClampedDouble(tCref.ThreadGetStartTime())))
       .Data(ToShortDuration(cLog->
         CCDeltaToClampedDouble(tCref.ThreadGetEndTime())))
       .Data(ToShortDuration(tCref.ThreadIsRunning()
          ? cmHiRes.TimePointToClampedDouble(tCref.ThreadGetStartTime())
          : ClockTimePointRangeToClampedDouble(tCref.ThreadGetEndTime(),
              tCref.ThreadGetStartTime())))
       .Data(tCref.IdentGet());
  } // Show number of threads
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cThreads->size(), "thread (", "threads ("),
    ThreadGetRunning(), " running).");
}
/* == Show Input Information =============================================== */
static void CBinput(const Arguments &)
{ // Make a table to automatically format our data neatly
  Statistic tData;
  tData.Header("ID").Header("FL").
        Header("AX").Header("BT").Header("NAME", false);
  // Get joysticks data
  const JoyList &jlList = cInput->GetConstJoyList();
  // Joysticks connected
  size_t stCount = 0;
  // For each joystick
  for(const JoyInfo &jsD : jlList)
  { // Ignore if not connected
    if(jsD.IsDisconnected()) continue;
    // Store data
    tData.DataN(jsD.GetId()).DataH(jsD.FlagGet()).DataN(jsD.GetAxisCount())
         .DataN(jsD.GetButtonCount()).Data(jsD.IdentGet());
    // Connected
    ++stCount;
  } // Print totals
  cConsole->AddLineEx("$$ ($ supported).", tData.Finish(),
    PluraliseNum(stCount, "input", "inputs"), jlList.size());
}
/* == Show Json Information ================================================ */
static void CBjsons(const Arguments &)
{ // Get reference to jsons collector class and lock it so it's not changed
  const LockGuard lgJsonsSync{ cJsons->CollectorGetMutex() };
  // Print totals
  cConsole->AddLineExA(PluraliseNum(cJsons->size(), "json.", "jsons."));
}
/* == Show Masks Information =============================================== */
static void CBmasks(const Arguments &)
{ // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("WIDTH").Header("HEIGHT").Header("TOTAL")
       .Header("ALLOC").Header("NAME", false);
  // Walk mask list
  for(const Mask*const mCptr : *cMasks)
  { // Get mask
    const Mask &dcCref = *mCptr;
    // Add mask data to table
    tData.DataN(dcCref.CtrGet()).DataN(dcCref.DimGetWidth())
         .DataN(dcCref.DimGetHeight()).DataN(dcCref.size())
         .DataN(dcCref.GetAlloc()).Data(dcCref.IdentGet());
  } // Output data
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cMasks->size(), "mask.", "masks."));
}
/* == Show messages log ==================================================== */
static void CBlog(const Arguments &)
{ // Colours for log levels
  static const array<Colour, LH_MAX> cColours{
    COLOUR_LBLUE  /* LH_DISABLED */, COLOUR_LRED   /* LH_ERROR    */,
    COLOUR_YELLOW /* LH_WARNING  */, COLOUR_WHITE  /* LH_INFO     */,
    COLOUR_LGRAY  /* LH_DEBUG    */
  };
  // Get reference to log class and lock it so it's not changed
  const LockGuard lgLogSync{ cLog->GetMutex() };
  // For each log line. Add the line to console buffer
  for(const LogLine &llLine : *cLog)
    cConsole->AddLineEx(cColours[llLine.lhLevel], "[$$$] $",
      fixed, setprecision(6), llLine.dTime, llLine.strLine);
  // Number of items in buffer
  cConsole->AddLineExA(PluraliseNum(cLog->size(), "line.", "lines."));
}
/* == Clear messages log =================================================== */
static void CBlogclr(const Arguments &)
{ // Lock access to the log
  const LockGuard lgLogSync{ cLog->GetMutex() };
  // Ignore if not empty
  if(cLog->empty()) return cConsole->AddLine("No log lines to clear!");
  // Get log lines count
  const size_t stCount = cLog->size();
  // Clear the log
  cLog->Clear();
  // Say we cleared it
  cConsole->AddLineExA(PluraliseNum(stCount,
    "log line cleared.", "log lines cleared."));
}
/* == Show Memory Information ============================================== */
static void CBmem(const Arguments &)
{ // Update counters
  cSystem->UpdateMemoryUsageData();
  // Precalculate lua and sql usage.
  const size_t stUsage = GetLuaUsage(cLua->GetState());
  const size_t stSqlUse = cSql->HeapUsed();
  // Report lua and sql memory usage
  Statistic tData;
  tData.Header("BYTES", true).Header()
       .Header("COMPONENT", false).DupeHeader()
       .DataN(stUsage).DataB(stUsage).Data("Lua usage")
       .DataN(stSqlUse).DataB(stSqlUse).Data("Sqlite usage")
  // Print rest of information
       .DataN(cSystem->RAMProcUse()).DataB(cSystem->RAMProcUse())
       .Data("Process usage")
       .DataN(cSystem->RAMProcPeak()).DataB(cSystem->RAMProcPeak())
       .Data ("Process peak usage")
  // Using 32-bits?
#if defined(X86)
  // The engine can allocate up to 4GB in a 32-bit process
       .DataN(cSystem->RAMFree32()).DataB(cSystem->RAMFree32())
       .Data("Engine available").DataN(cSystem->RAMFree())
       .DataB(cSystem->RAMFree()).Data("System free")
#else
  // Using 64-bits? Show 64-bit specific data
       .DataN(cSystem->RAMFree()).DataB(cSystem->RAMFree()).Data("System free")
#endif
  // Report system memory info
       .DataN(cSystem->RAMUsed()).DataB(cSystem->RAMUsed())
       .Data("System usage").DataN(cSystem->RAMTotal())
       .DataB(cSystem->RAMTotal()).Data("System total");
  // If have video memory reporting?
  if(cOgl->FlagIsSet(GFL_HAVEMEM))
  { // Get VRAM info
    const uint64_t qwA = cOgl->GetVRAMFree(),
                   qwT = cOgl->GetVRAMTotal(),
                   qwU = qwT - qwA;
    // Report system memory info
    tData.DataN(qwA).DataB(qwA).Data("Renderer free")
         .DataN(qwU).DataB(qwU).Data("Renderer usage")
         .DataN(qwT).DataB(qwT).Data("Renderer total");
  } // Compiling in debug mode?
#if defined(ALPHA)
  // Add heap check result
  tData.Data().Data().Data(cSystem->HeapCheck());
#endif
  // Output all the data we built up
  cConsole->AddLine(tData.Finish(false));
}
/* == Show Palettes Information ============================================ */
static void CBpalettes(const Arguments &)
{ // Print totals
  cConsole->AddLineExA(PluraliseNum(cPalettes->size(),
    "palette.", "palettes."));
}
/* == Show Streams Information ============================================= */
static void CBstreams(const Arguments &)
{ // Synchronise the streams collector list
  const LockGuard lgStreamsSync{ cStreams->CollectorGetMutex() };
  // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("L").Header("ELAPSED").Header("PCMELAP")
       .Header("DURTION").Header("PCMDURA").Header("LOOP").Header("LENGTH")
       .Header("VOLUME").Header("TYPE").Header("NAME", false);
  // Walk through sources
  for(const Stream*const sCptr : *cStreams)
  { // Get pointer to class (Can't use const because of vorbis :( fixable?)
    Stream &sCref = *const_cast<Stream*>(sCptr);
    // Add stream data to table
    tData.DataN(sCref.CtrGet()).Data(YesOrNo(sCref.LuaRefIsSet()))
         .DataN(sCref.GetElapsedSafe(), 1).DataN(sCref.GetPositionSafe())
         .DataN(sCref.GetDurationSafe(), 1).DataN(sCref.GetTotalSafe())
         .Data(sCref.GetLoop() == string::npos ? "INFI" :
            ToString(sCref.GetLoop())).DataN(sCref.GetOggBytes())
         .DataN(sCref.GetVolume(), 6).Data(sCref.GetFormatName())
         .Data(sCref.IdentGet());
  } // Show stream classes count
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cStreams->size(), "stream.", "streams."));
}
/* == Show Archives Information ============================================ */
static void CBarchives(const Arguments &)
{ // Get reference to assets collector class and lock it so it's not changed
  const LockGuard lgAssetsSync{ cArchives->CollectorGetMutex() };
  // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("F").Header("D").Header("T").Header("U")
       .Header("FILENAME", false);
  // Total resources
  size_t stResources = 0;
  // Walk through archives
  for(const Archive*const oCptr : *cArchives)
  { // Get pointer to class
    const Archive &aCref = *oCptr;
    // Get number of resources in archive
    const size_t stFiles = aCref.GetFileList().size(),
                 stDirs = aCref.GetDirList().size();
    // Show status
    tData.DataN(aCref.CtrGet()).DataN(stFiles).DataN(stDirs)
         .DataN(stFiles+stDirs).DataN(aCref.GetInUse())
         .Data(aCref.IdentGet());
    // Add to resources total
    stResources += stFiles;
  } // Show count
  cConsole->AddLineEx("$$ and $.", tData.Finish(),
    PluraliseNum(cArchives->size(), "archive", "archives"),
    PluraliseNum(stResources, "resource", "resources"));
}
/* == Show command line arguments ========================================== */
static void CBcla(const Arguments &aList)
{ // Get list of environment variables
  const StrVector &lArgs = cCmdLine->GetArgList();
  // If parameter was specified?
  if(aList.size() > 1)
  { // Convert parmeter to number
    const size_t stArg = ToNumber<size_t>(aList[1]);
    if(stArg >= lArgs.size())
      return cConsole->AddLine("Command line argument index invalid!");
    // Print it and return
    cConsole->AddLine(lArgs[stArg]);
    return;
  } // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("ARGUMENT", false);
  // Walk through environment variables add them to the statistic object
  size_t stCount = 0;
  for(const string &strArg : lArgs) tData.DataN(stCount++).Data(strArg);
  // Log counts
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(stCount, "command line argument.",
                          "command line arguments."));
}
/* == Show Processor Information =========================================== */
static void CBcpu(const Arguments &)
{ // Update cpu usage times
  cSystem->UpdateCPUUsage();
  // Write detected processor information
  cConsole->AddLineEx(
    "CPU: $$MHz x $ x $.\n"
    "CPUID: $; Load: $% ($%).\n"
    "Start: $; Last: $; Limit: $; Accu: $.\n"
    "Mode: $ ($); TimeOut: $ ($x$).\n"
    "FPS: $/s; Delay: $/s; Ticks: $.",
      fixed, cSystem->CPUSpeed(), cSystem->CPUCount(), cSystem->CPUName(),
      cSystem->CPUIdentifier(), cSystem->CPUUsage(), cSystem->CPUUsageSystem(),
      TimerGetStart(), TimerGetDuration(), TimerGetLimit(),
        TimerGetAccumulator(),
      cSystem->GetGuiMode(), cSystem->GetGuiModeString(), TimerGetTimeOut(),
        TimerGetTriggers(), cLua->GetOpsInterval(),
      TimerGetSecond(), TimerGetDelay(), TimerGetTicks());
}
/* == Show current time ==================================================== */
static void CBtime(const Arguments &)
{ // Timestamp to use
  const char*const cpFmt = "%a %b %d %H:%M:%S %Y %z";
  // Get current time
  const StdTimeT tTime = cmSys.GetTimeS();
  // Check if timezone's are different, if they are the same? Show local time
  if(FormatTimeTT(tTime, "%z") == FormatTimeTTUTC(tTime, "%z"))
    cConsole->AddLineEx("Local and universal time is $.",
      FormatTimeTTUTC(tTime, cpFmt));
  // Write current local and universal system time
  else cConsole->AddLineEx("Local time is $.\nUniversal time is $.",
      FormatTimeTT(tTime, cpFmt), FormatTimeTTUTC(tTime, cpFmt));
}
/* == Show Loaded Sources Information ====================================== */
static void CBsources(const Arguments &)
{ // Get reference to sources collector class and lock it so it's not changed
  const LockGuard lgSourcesSync{ cSources->CollectorGetMutex() };
  // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("SID").Header("FLAG").Header("S")
       .Header("T").Header("BQ").Header("BP").Header("BI").Header("GAIN")
       .Header("GAINMIN").Header("GAINMAX").Header("PITCH").Header("REFDIST")
       .Header("MAXDIST", false);
  // Walk through sources
  for(const Source*const sCptr : *cSources)
  { // Get pointer to class
    const Source &sCref = *sCptr;
    // Get source play state and type
    const ALenum alState = sCref.GetState();
    const ALuint uiType = sCref.GetType();
    // Add data to text table
    tData.DataN(sCref.CtrGet()).DataN(sCref.GetSource()).Data(EvaluateTokens({
            { sCref.LockIsSet(),    'L' }, { sCref.GetExternal(),   'X' },
            { !!sCref.GetLooping(), 'O' }, { !!sCref.GetRelative(), 'R' },
          }))
         .DataC(alState == AL_INITIAL ? 'I' : (alState == AL_PLAYING ? 'P' :
               (alState == AL_PAUSED  ? 'H' : (alState == AL_STOPPED ? 'S' :
                                        '?'))))
         .DataC(uiType == AL_UNDETERMINED ? 'U' : (uiType == AL_STATIC ? 'T' :
               (uiType == AL_STREAMING    ? 'S' : '?')))
         .DataN(sCref.GetBuffersQueued()).DataN(sCref.GetBuffersProcessed())
         .DataN(sCref.GetBuffer())       .DataN(sCref.GetGain(), 6)
         .DataN(sCref.GetMinGain(), 6)   .DataN(sCref.GetMaxGain(), 6)
         .DataN(sCref.GetPitch(), 6)     .DataN(sCref.GetRefDist(), 6)
         .DataN(sCref.GetMaxDist(), 6);
  } // Show total
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cSources->size(), "source.", "sources."));
}
/* == Show Loaded Texture Information ====================================== */
static void ShowTextureInfo(Statistic &tData, const Texture &tC)
{ // Add data to table
  tData.DataN(tC.CtrGet())
       .Data(YesOrNo(tC.FlagIsSet(TF_DELETE)))
       .DataN(tC.GetSubCount()).DataN(tC.GetMipmaps()).DataN(tC.GetTexFilter())
       .DataN(tC.GetTileCount()).DataN(tC.GetTileWidth())
       .DataN(tC.GetTileHeight()).DataN(tC.GetPaddingWidth(), 0)
       .DataN(tC.GetPaddingHeight(), 0).Data(tC.IdentGet());
}
static void CBtextures(const Arguments &)
{ // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("D").Header("SC").Header("MM").Header("TF")
       .Header("TLT").Header("TSSX").Header("TSSY").Header("TSPX")
       .Header("TSPY").Header("IDENTIFIER", false);
  // Include console texture and font
  ShowTextureInfo(tData, cConsole->ctConsole);
  ShowTextureInfo(tData, cConsole->cfConsole);
  // Walk through font textures and textures classes
  for(const Font*const fCptr : *cFonts) ShowTextureInfo(tData, *fCptr);
  for(const Texture*const tCptr : *cTextures) ShowTextureInfo(tData, *tCptr);
  // Log texture counts (+ 2 for the console texture and font)
  cConsole->AddLineExA(tData.Finish(), PluraliseNum(cTextures->size() +
    cFonts->size() + 2, "texture.", "textures."));
}
/* == Show Loaded Audio Format Plugins ==================================== */
static void CBpcmfmts(const Arguments &)
{ // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("FL").Header("EXT").Header("NAME", false);
  // For each plugin
  for(const PcmFmt*const pCptr : *cPcmFmts)
  { // Get refernece to plugin
    const PcmFmt &pCref = *pCptr;
    // Write data
    tData.DataN(pCref.CtrGet())
         .Data(EvaluateTokens({
           { pCref.HaveLoader(), 'L' },
           { pCref.HaveSaver(),  'S' } }))
         .Data(pCref.GetExt()).Data(pCref.GetName());
  } // Log total plugins
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cPcmFmts->size(), "format.", "formats."));
}
/* == Show Loaded Image Format Plugins ==================================== */
static void CBimgfmts(const Arguments &)
{ // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("FL").Header("EXT").Header("NAME", false);
  // For each plugin
  for(const ImageFmt*const iCptr : *cImageFmts)
  { // Get refernece to plugin
    const ImageFmt &iCref = *iCptr;
    // Write data
    tData.DataN(iCref.CtrGet())
         .Data(EvaluateTokens({ { iCref.HaveLoader(), 'L' },
                                { iCref.HaveSaver(),  'S' } }))
         .Data(iCref.GetExt()).Data(iCref.GetName());
  } // Log total plugins
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cImageFmts->size(), "format.", "formats."));
}
/* == Show Loaded Images Information ======================================= */
static void CBimages(const Arguments &svParam)
{ // Get reference to images collector class and lock it so it's not changed
  const LockGuard lgImagesSync{ cImages->CollectorGetMutex() };
  // If we have more than one parameter?
  if(svParam.size() == 2)
  { // Get slot data and return if it is a bad slot
    const size_t stId = ToNumber<size_t>(svParam[1]);
    if(stId >= cImages->size()) return cConsole->AddLine("Invalid image id!");
    Image &iCref = **next(cImages->cbegin(), static_cast<ssize_t>(stId));
    // Text table class to help us write neat output
    Statistic tData;
    tData.Header("SIZX").Header("SIZY").Header("SIZE");
    // For each slot
    for(const ImageSlot &imdD : iCref)
      tData.DataN(imdD.DimGetWidth()).DataN(imdD.DimGetHeight())
           .DataN(imdD.Size());
    // Log texture counts
    cConsole->AddLineEx("$$ in $.", tData.Finish(),
      PluraliseNum(iCref.size(), "slot", "slots"), iCref.IdentGet());
    // Done
    return;
  } // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("FLAGS", false).Header("SA").Header("SC")
       .Header("SIZW").Header("SIZW").Header("BI").Header("B").Header("PIXT")
       .Header("ALLOC").Header("NAME", false);
  // Walk through textures classes
  for(const Image*const iCptr : *cImages)
  { // Convert to reference
    const Image &iCref = *iCptr;
    // Add data to table
    tData.DataN(iCref.CtrGet()).Data(EvaluateTokens({
      { iCref.IsDynamic(),          'Y' }, { iCref.IsNotDynamic(),       'S' },
      { iCref.IsPurposeFont(),      'F' }, { iCref.IsPurposeImage(),     'I' },
      { iCref.IsPurposeTexture(),   'X' }, { iCref.IsLoadAsDDS(),        'D' },
      { iCref.IsLoadAsGIF(),        'G' }, { iCref.IsLoadAsJPG(),        'J' },
      { iCref.IsLoadAsPNG(),        'P' }, { iCref.IsLoadAsTGA(),        'T' },
      { iCref.IsConvertAtlas(),     'A' }, { iCref.IsActiveAtlas(),      'a' },
      { iCref.IsConvertReverse(),   'E' }, { iCref.IsActiveReverse(),    'e' },
      { iCref.IsConvertRGB(),       'H' }, { iCref.IsActiveRGB(),        'h' },
      { iCref.IsConvertRGBA(),      'D' }, { iCref.IsActiveRGBA(),       'd' },
      { iCref.IsConvertBGROrder(),  'W' }, { iCref.IsActiveBGROrder(),   'w' },
      { iCref.IsConvertBinary(),    'N' }, { iCref.IsActiveBinary(),     'n' },
      { iCref.IsConvertGPUCompat(), 'O' }, { iCref.IsActiveGPUCompat(),  'o' },
      { iCref.IsConvertRGBOrder(),  'B' }, { iCref.IsActiveRGBOrder(),   'b' },
      { iCref.IsCompressed(),       'C' }, { iCref.IsPalette(),          'L' },
      { iCref.IsMipmaps(),          'M' }, { iCref.IsReversed(),         'R' }
    })).DataN(iCref.size()).DataN(iCref.capacity()).DataN(iCref.DimGetWidth())
       .DataN(iCref.DimGetHeight()).DataN(iCref.GetBitsPerPixel())
       .DataN(iCref.GetBytesPerPixel()).DataH(iCref.GetPixelType(),4)
       .DataN(iCref.GetAlloc()).Data(iCref.IdentGet());
  } // Log texture counts
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cImages->size(), "image.", "images."));
}
/* == Show Loaded Pcms Information ======================================== */
static void CBpcms(const Arguments &)
{ // Get reference to pcms collector class and lock it so it's not changed
  const LockGuard lgPcmsSync{ cPcms->CollectorGetMutex() };
  // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("FLAGS", false).Header("RATE")
       .Header("C").Header("BT").Header("PFMT").Header("ALLOC")
       .Header("NAME", false);
  // Walk through textures classes
  for(const Pcm*const pCptr : *cPcms)
  { // Get pointer to class
    const Pcm &pCref = *pCptr;
    // Add data to table
    tData.DataN(pCref.CtrGet())
         .Data(EvaluateTokens({
      { pCref.IsDynamic(),           'Y' },
      { pCref.IsNotDynamic(),        'S' },
      { pCref.FlagIsSet(PL_FCE_WAV), 'W' },
      { pCref.FlagIsSet(PL_FCE_CAF), 'C' },
      { pCref.FlagIsSet(PL_FCE_OGG), 'O' },
      { pCref.FlagIsSet(PL_FCE_MP3), 'M' } }))
         .DataN(pCref.GetRate()).DataN(pCref.GetChannels())
         .DataN(pCref.GetBits()).DataH(pCref.GetFormat(),4)
         .DataN(pCref.GetAlloc())
         .Data(pCref.IdentGet());
  } // Log texture counts
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cPcms->size(), "pcm.", "pcms."));
}
/* == Show Loaded Chars Information ======================================== */
static void ShowFontInfo(Statistic &tData, const Font &fC)
  { tData.DataN(fC.CtrGet())
         .DataC(fC.FlagIsSet(FF_FLOORADVANCE) ? 'F' :
               (fC.FlagIsSet(FF_CEILADVANCE)  ? 'C' :
               (fC.FlagIsSet(FF_ROUNDADVANCE) ? 'R' : '-')))
         .Data(EvaluateTokens({
           { fC.FlagIsSet(FF_USEGLYPHSIZE), 'S' },
           { fC.FlagIsSet(FF_STROKETYPE2),  'A' }
         }))
         .DataN(fC.GetCharScale(), 6)
         .DataN(fC.GetTexOccupancy(), 6)
         .DataN(fC.GetCharCount())
         .Data(fC.IdentGet()); }
static void CBfonts(const Arguments &)
{ // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("R").Header("FLAG").Header("SCALE")
       .Header("TEXOCPCY").Header("CC").Header("NAME", false);
  // Include console font
  ShowFontInfo(tData, cConsole->cfConsole);
  // Walk through textures classes
  for(const Font*const fCptr : *cFonts) ShowFontInfo(tData, *fCptr);
  // Log counts including the static console font class
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cFonts->size() + 1, "font.", "fonts."));
}
/* == Show Loaded Chars Information ======================================== */
static void CBftfs(const Arguments &)
{ // Get reference to fonts collector class and lock it so it's not changed
  const LockGuard lgFtfsSync{ cFtfs->CollectorGetMutex() };
  // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("GLYPH").Header("FW").Header("FH").Header("DW")
       .Header("DH").Header("STYLE", false).Header("FAMILY", false)
       .Header("FILENAME", false);
  // Walk through textures classes
  for(const Ftf*const fCptr : *cFtfs)
  { // Get pointer to class
    const Ftf &fCref = *fCptr;
    // Show data
    tData.DataN(fCref.CtrGet()).DataN(fCref.GetGlyphCount())
         .DataN(fCref.DimGetWidth(),0).DataN(fCref.DimGetHeight(),0)
         .DataN(fCref.dDPI.DimGetWidth()).DataN(fCref.dDPI.DimGetHeight())
         .Data(fCref.GetStyle()).Data(fCref.GetFamily())
         .Data(fCref.IdentGet());
  } // Log counts
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cFtfs->size(), "ftf.", "ftfs."));
}
/* == Find text in console ================================================= */
static void CBfind(const Arguments &aList)
{ // Find text in console backlog and if not found, show message
  cConsole->FindText(Implode(aList, 1));
}
/* == Show Loaded Bins Information ========================================= */
static void CBbins(const Arguments &)
{ // Text table class to help us write neat output
  Statistic tData;
  tData.Header("WIDTH").Header("HEIGHT").Header("OCCUPANCY").Header("TOTL")
       .Header("USED").Header("FREE");
  // Walk through textures classes
  for(const Bin*const bCptr : *cBins)
  { // Get pointer to class
    const Bin &bCref = *bCptr;
    // Show data
    tData.DataN(bCref.DimGetWidth()).DataN(bCref.DimGetHeight())
         .DataN(bCref.Occupancy(), 7).DataN(bCref.Total())
         .DataN(bCref.Used()).DataN(bCref.Free());
  } // Log counts
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cBins->size(), "bin.", "bins."));
}
/* == Show open file streams =============================================== */
static void CBfiles(const Arguments &)
{ // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("FD").Header("POSITION").Header("LENGTH")
       .Header("ERROR").Header("FILENAME", false);
  // Walk through textures classes
  for(const File*const fCptr : *cFiles)
  { // Get pointer to class
    const File &fCref = *fCptr;
    // Show data
    tData.DataN(fCref.CtrGet()).DataN(fCref.FStreamGetID())
         .DataN(fCref.FStreamGetPosition()).DataN(fCref.FStreamSize())
         .DataN(fCref.FStreamFError()).Data(fCref.IdentGet());
  } // Log counts
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cFiles->size(), "file.", "files."));
}
/* == Show environment variables =========================================== */
static void CBenv(const Arguments &aList)
{ // Get list of environment variables
  const StrStrMap &lEnv = cCmdLine->GetEnvList();
  // If parameter was specified?
  if(aList.size() > 1)
  { // Get parameter name and find it
    const StrStrMapConstIt &ssmciVar = lEnv.find(aList[1]);
    if(ssmciVar == lEnv.cend())
      return cConsole->AddLine("Environment variable not found!");
    // Print it and return
    cConsole->AddLineExA(ssmciVar->first, '=', ssmciVar->second);
    return;
  } // Text table class to help us write neat output
  Statistic tData;
  tData.Header("VARIABLE").Header("VALUE", false);
  // Walk through environment variables add them to the statistic object
  for(const auto &eVar : lEnv) tData.Data(eVar.first).Data(eVar.second);
  // Log counts
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(lEnv.size(), "environment variable.",
                              "environment variables."));
}
/* == Show Framebuffers Information ======================================== */
static void ShowFboInfo(const Fbo &fC, Statistic &tData,
  size_t &stTriangles, size_t &stCommands)
{ // Add to totals
  stTriangles += fC.GetTris();
  stCommands += fC.GetCmds();
  // Show FBO status
  tData.DataN(fC.CtrGet()).DataN(fC.uiFBO).DataN(fC.uiFBOtex)
       .Data(EvaluateTokens({
          { fC.IsTransparencyEnabled(), 'A' }, { fC.IsClearEnabled(), 'C' },
       }))
       .DataN(fC.GetFilter()).DataN(fC.DimGetWidth()).DataN(fC.DimGetHeight())
       .DataN(fC.fcStage.GetCoLeft()).DataN(fC.fcStage.GetCoTop())
       .DataN(fC.fcStage.GetCoRight()).DataN(fC.fcStage.GetCoBottom())
       .DataN(fC.GetTris()).DataN(fC.GetCmds()).DataN(fC.GetTrisReserved())
       .DataN(fC.GetCmdsReserved()).Data(fC.IdentGet());
}
static void CBfbos(const Arguments &)
{ // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("FI").Header("TI").Header("FL").Header("FT")
       .Header("SIZW").Header("SIZH").Header("STX1").Header("STY1")
       .Header("STX2").Header("STY2").Header("TRIS").Header("CMDS")
       .Header("TRES").Header("CRES").Header("IDENTIFIER", false);
  // Total triangles and commands
  size_t stTriangles = 0, stCommands = 0;
  // Show primary fbos info
  ShowFboInfo(cFboMain->fboMain, tData, stTriangles, stCommands);
  ShowFboInfo(cFboMain->fboConsole, tData, stTriangles, stCommands);
  // Walk through textures classes
  for(const Fbo*const fCptr : *cFbos)
    ShowFboInfo(*fCptr, tData, stTriangles, stCommands);
  // Log counts
  cConsole->AddLineEx("$$ totalling $ and $.", tData.Finish(),
    PluraliseNum(2 + cFbos->size(), "framebuffer", "framebuffers"),
    PluraliseNum(stTriangles, "triangle", "triangles"),
    PluraliseNum(stCommands, "command", "commands"));
}
/* == Show Videos Information ============================================== */
static void CBvideos(const Arguments &)
{ // Get reference to videos collector class and lock it so it's not changed
  const LockGuard lgVideosSync{ cVideos->CollectorGetMutex() };
  // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("FLAGS").Header("PCMF").Header("P").Header("C")
       .Header("FRMW").Header("FRMH").Header("FPS").Header("ELAPSED")
       .Header("FRAMES").Header("LEN").Header("NAME", false);
  // Walk through textures classes
  for(const Video*const vCptr : *cVideos)
  { // Get pointer to class
    const Video &vCref = *vCptr;
    // and write data
    tData.DataN(vCref.CtrGet())
      .Data(EvaluateTokens({
        { vCref.LuaRefIsSet(),       'L' }, { vCref.HaveAudio(),         'A' },
        { vCref.FlagIsSet(FL_FILTER),'F' }, { vCref.GetKeyed(),          'K' },
        { vCref.FlagIsSet(FL_PLAY),  'P' }, { vCref.FlagIsSet(FL_STOP),  'S' },
        { vCref.FlagIsSet(FL_THEORA),'T' }, { vCref.FlagIsSet(FL_VORBIS),'V' },
        { vCref.FlagIsSet(FL_VDINIT),'1' }, { vCref.FlagIsSet(FL_VBINIT),'2' },
        { vCref.FlagIsSet(FL_OSINIT),'3' }, { vCref.FlagIsSet(FL_TCINIT),'4' },
        { vCref.FlagIsSet(FL_TIINIT),'5' }, { vCref.FlagIsSet(FL_VCINIT),'6' },
        { vCref.FlagIsSet(FL_VIINIT),'7' },
      }))
     .Data(vCref.GetFormatAsIdentifier())
     .DataH(vCref.GetPixelFormat(),4).DataN(vCref.GetColourSpace())
     .DataN(vCref.GetFrameWidth()).DataN(vCref.GetFrameHeight())
     .DataN(vCref.GetFPS(), 2).DataN(vCref.GetVideoTime(), 3)
     .DataN(vCref.GetFrames()).DataN(vCref.GetLength())
     .Data(vCref.IdentGet());
  } // Log counts
  cConsole->AddLineExA(tData.Finish(),
    PluraliseNum(cVideos->size(), "video.", "videos."));
}
/* == Show Graphics System Information ===================================== */
static void CBgpu(const Arguments &)
{ // Write opengl rendering stats
  cConsole->AddLineEx(
    "Renderer: $ on $.\n"
    "Version: $ by $.\n"
    "$" // Memory (optional)
    "Window: $x$ ($) @ $x$ (*$x$); Flags: 0x$$.\n"
    "- Viewport size: $$x$ ($).\n"
    "- Ortho matrix: $x$ ($); total: $x$ ($).\n"
    "- Stage: $$; $; $; $.\n"
    "- Main Framebuffer: $x$; Maximum: $^2; Flags: 0x$$.\n"
    "- Polygons: $$/$; Commands: $/$.\n"
    "FPS: $$/s ($/s); Eff: $% ($%).",
    cOgl->GetRenderer(), cDisplay->GetMonitorName(),
    cOgl->GetVersion(), cOgl->GetVendor(),
    cOgl->FlagIsSet(GFL_HAVEMEM) ?
      Format("Memory: $ mBytes ($ mBytes available).\n",
        cOgl->GetVRAMTotal() / 1048576, cOgl->GetVRAMFree() / 1048576) :
      strBlank,
    cInput->GetWindowWidth(), cInput->GetWindowHeight(),
      ToRatio(cInput->GetWindowWidth(), cInput->GetWindowHeight()),
      cDisplay->GetWindowPosX(), cDisplay->GetWindowPosY(),
      cDisplay->GetWindowScaleWidth(), cDisplay->GetWindowScaleHeight(),
      hex, cDisplay->FlagGet(),
    dec, cFboMain->GetCoRight(), cFboMain->GetCoBottom(),
      ToRatio(cFboMain->GetCoRight(), cFboMain->GetCoBottom()),
      cFboMain->GetOrthoWidth(), cFboMain->GetOrthoHeight(),
      ToRatio(cFboMain->GetOrthoWidth(), cFboMain->GetOrthoHeight()),
      cFboMain->fboMain.GetCoRight(), cFboMain->fboMain.GetCoBottom(),
      ToRatio(cFboMain->fboMain.GetCoRight(),
        cFboMain->fboMain.GetCoBottom()), fixed,
      cFboMain->fboMain.fcStage.GetCoLeft(),
        cFboMain->fboMain.fcStage.GetCoTop(),
        cFboMain->fboMain.fcStage.GetCoRight(),
        cFboMain->fboMain.fcStage.GetCoBottom(),
    cFboMain->fboMain.DimGetWidth(), cFboMain->fboMain.DimGetHeight(),
      cOgl->MaxTexSize(), hex, cOgl->FlagGet(),
    dec, cFboMain->fboMain.GetTris(),
      cFboMain->fboMain.GetTrisReserved(),
      cFboMain->fboMain.GetCmds(),
      cFboMain->fboMain.GetCmdsReserved(),
    fixed, cFboMain->dRTFPS, cDisplay->GetRefreshRate(),
    cFboMain->dRTFPS / cDisplay->GetRefreshRate() * 100,
    TimerGetSecond() / cFboMain->dRTFPS * 100);
}
/* == Show Operating System Information ==================================== */
static void CBsystem(const Arguments &)
{ // Print OS stats
  cConsole->AddLineEx("$-bit $ version $.$ build $ locale $.",
    cSystem->OSBits(), cSystem->OSName(), cSystem->OSMajor(),
    cSystem->OSMinor(), cSystem->OSBuild(), cSystem->OSLocale());
}
/* == Show Credit Information ============================================= */
static void CBcredits(const Arguments &aList)
{ // Do we have a parameter?
  if(aList.size() > 1)
  {  // Get index and show decompressed text for it or invalid
    const size_t stIndex = ToNumber<size_t>(aList[1]);
    return cConsole->AddLine(stIndex >= CreditGetItemCount() ?
      "The specified credit index is invalid!" :
      CreditGetItemText(static_cast<CreditEnums>(stIndex)));
  } // Text table class to help us write neat output
  Statistic tData;
  tData.Header("ID").Header("NAME", false).Header("VERSION")
       .Header("AUTHOR", false);
  // For each item, show library information
  for(size_t stIndex = 0; stIndex < CreditGetItemCount(); ++stIndex)
  { // Get credit for item and add the data for the credit
    const CreditLib &sD = CreditGetItem(static_cast<CreditEnums>(stIndex));
    tData.DataN(stIndex).Data(sD.GetName()).Data(sD.GetVersion())
         .Data(Append(sD.IsCopyright() ? "\xC2\xA9 " : strBlank,
           sD.GetAuthor()));
  } // Show number of libs
  cConsole->AddLineExA(tData.Finish(), PluraliseNum(CreditGetItemCount(),
    "third-party library.", "third-party libraries."));
}
/* == Show engine version information ====================================== */
static void CBversion(const Arguments &) { cConsole->PrintVersion(); }
/* == Have specified opengl function ======================================= */
static void CBoglfunc(const Arguments &aList)
{ // Get function name and output if the extension is supported
  const string &strF = aList[1];
  cConsole->AddLineEx(
    "Function '$' is$ supported by the selected graphics device.",
      strF, GlFWProcExists(strF.c_str()) ? strBlank : " NOT");
}
/* == Have specified opengl extension ====================================== */
static void CBoglext(const Arguments &aList)
{ // Get extension name and output if the extension is supported
  const string &strX = aList[1];
  cConsole->AddLineEx(
    "Extension '$' is$ supported by the selected graphics device.",
      strX, cOgl->HaveExtension(strX.c_str()) ? strBlank : " NOT");
}
/* == Dump console information ============================================= */
static void CBcon(const Arguments &)
{ // Write console status
  cConsole->AddLineEx("Console flags are currently 0x$$$ ($).\n"
                      "- Output lines: $ (Maximum: $).\n"
                      "- Input commands: $ (Maximum: $).\n"
                      "- Max input characters: $; Page lines: $.\n"
                      "- Command functions: $ (Lua: $).",
    hex, cConsole->FlagGet(), dec, EvaluateTokens({
      { cConsole->FlagIsSet(CF_CANTDISABLE), 'D' },
      { cConsole->FlagIsSet(CF_IGNOREKEY),   'K' },
      { cConsole->FlagIsSet(CF_AUTOSCROLL),  'A' },
      { cConsole->FlagIsSet(CF_REDRAW),      'R' },
      { cConsole->FlagIsSet(CF_AUTOCOPYCVAR),'C' },
      { cConsole->FlagIsSet(CF_INSERT),      'I' },
      { cConsole->FlagIsSet(CF_ENABLED),     'E' }
    }),
    cConsole->GetOutputCount(), cConsole->stOutputMaximum,
    cConsole->GetInputCount(), cConsole->stInputMaximum,
    cConsole->stMaximumChars, cConsole->iPageLines,
    cConsole->llCmds.size(), cConsole->lfList.size());
}
/* == Dump console to log ================================================== */
static void CBconlog(const Arguments &)
{ // Copy all the console lines to log and report how many lines we copied
  cConsole->AddLineExA(PluraliseNum(cConsole->ToLog(), "line", "lines"),
    " copied to log.");
}
/* == Crash the client ===================================================== */
static void CBcrash[[noreturn]](const Arguments &)
  { System::CriticalHandler("Requested operation"); }
/* == Dump directory listing to console which includes archived files ====== */
static void CBdir(const Arguments &aList)
{ // Make and checkfilename
  const string &strVal = aList.size() > 1 ? Append("./", aList[1]) : ".";
  const ValidResult vResult = DirValidName(strVal);
  if(vResult != VR_OK)
    return cConsole->AddLineEx("Cannot check directory '$': $!",
      strVal, DirValidNameResultToString(vResult));
  // Enumerate local directories on disk
  const Dir dPath{ std::move(strVal) };
  // Get reference to assets collector class and lock it so it's not changed
  const LockGuard lgArchivesSync{ cArchives->CollectorGetMutex() };
  // Files and directories and total bytes matched
  size_t stFiles = 0, stDirs = 0;
  uint64_t qwBytes = 0;
  // If there are no archives?
  if(cArchives->empty())
  { // Prepare data table for no archive output
    Statistic tData;
    tData.Header("SIZE").Header("FILENAME", false);
    // Directory data we are enumerating
    map<const string, const uint64_t> slData;
    // For each file found on disk
    for(const auto &dPair : dPath.dDirs)
    { // Add to file list and increment byte and directory count
      slData.insert({ std::move(dPair.first), static_cast<uint64_t>(-1) });
      ++stDirs;
    } // Enumerate local files
    for(const auto &dPair : dPath.dFiles)
    { // Add to file list and increment byte and file count
      slData.insert({ std::move(dPair.first), dPair.second.qSize });
      qwBytes += dPair.second.qSize;
      ++stFiles;
    } // Add each file we gathered to data table
    for(const auto &spPair : slData)
    { // Is not from archive and directory?
      if(spPair.second == static_cast<uint64_t>(-1))
        tData.Data("<DIR>").Data(std::move(spPair.first));
      // Is not from archive and file?
      else tData.DataN(spPair.second).Data(std::move(spPair.first));
    } // Show summary
    cConsole->AddLineEx("$$ in $ totalling $ ($).",
      tData.Finish(),
      PluraliseNum(stFiles, "file", "files"),
      PluraliseNum(stDirs, "dir", "dirs"),
      PluraliseNum(qwBytes, "byte", "bytes"),
      ToBytesStr(qwBytes));
    // Finished
    return;
  } // Set directory and get directories and files
  const string &strDir = aList.size() > 1 ? aList[1] : strBlank;
  // Prepare data table for archive display
  Statistic tData;
  tData.Header("SIZE").Header("ID").Header("ARCHIVE", false)
       .Header("FILENAME", false);
  // Directory data we are enumerating
  struct Item { const uint64_t qwSize;
                const unsigned int uiId;
                const string &strArc; };
  map<const string, const Item> slData;
  // Iterate through the list
  for(const Archive*const aCptr : *cArchives)
  { // Get archive class
    const Archive &aCref = *aCptr;
    // Get directory list and if the specified directory exists?
    const StrUIntMap &suimDirs = aCref.GetDirList();
    if(!strDir.empty() && suimDirs.find(strDir) == suimDirs.cend()) continue;
    // Enumerate directorys
    for(const auto &dI : suimDirs)
    { // Skip directory if start of directory does not match
      if(strDir != dI.first.substr(0, strDir.length())) continue;
      // Get filename, and continue again if it is a sub-directory/file
      const string strN{ Trim(dI.first.substr(strDir.length()), '/') };
      if(strN.find('/') != string::npos) continue;
      // Add to directory list and increment directory count
      slData.insert({ std::move(strN),
        { static_cast<uint64_t>(-1), dI.second, aCref.IdentGet() } });
      ++stDirs;
    } // Enumerate files
    const StrUIntMap &suimFiles = aCref.GetFileList();
    for(const auto &dI : suimFiles)
    { // Skip file if start of directory does not match
      if(strDir != dI.first.substr(0, strDir.length())) continue;
      // Get filename, and continue again if it is a sub-directory/file
      const string strN{ Trim(dI.first.substr(strDir.length()), '/') };
      if(strN.find('/') != string::npos) continue;
      // Add to file list and increment total bytes and file count
      const uint64_t qwSize = aCref.GetSize(dI.second);
      slData.insert({ std::move(strN),
        { qwSize, dI.second, aCref.IdentGet() } });
      qwBytes += qwSize;
      ++stFiles;
    }
  } // Enumerate local directories
  for(const auto &dPair : dPath.dDirs)
  { // Add to file list and increment byte and directory count
    slData.insert({ std::move(dPair.first),
      { static_cast<uint64_t>(-1), static_cast<unsigned int>(-1), {} } });
    ++stDirs;
  } // Enumerate local files
  for(const auto &dPair : dPath.dFiles)
  { // Add to file list and increment byte and file count
    const Dir::Item &fItem = dPair.second;
    slData.insert({ std::move(dPair.first),
      { fItem.qSize, static_cast<unsigned int>(-1), {} } });
    qwBytes += fItem.qSize;
    ++stFiles;
  } // Add each file we gathered to data table
  for(const auto &spPair : slData)
  { // Get item data and if this is a file on disk?
    const Item &itData = spPair.second;
    if(itData.strArc.empty())
    { // Is not from archive and directory?
      if(itData.qwSize == static_cast<uint64_t>(-1))
        tData.Data("<DIR>").Data().Data().
              Data(std::move(spPair.first));
      // Is not from archive and file?
      else tData.DataN(itData.qwSize).Data().Data().
                 Data(std::move(spPair.first));
    } // Is not from archive and directory?
    else if(itData.qwSize == static_cast<uint64_t>(-1))
      tData.Data("<DIR>").DataN(itData.uiId)
           .Data(itData.strArc).Data(std::move(spPair.first));
    // Is not from archive and file?
    else tData.DataN(itData.qwSize).DataN(itData.uiId)
              .Data(itData.strArc).Data(std::move(spPair.first));
  } // Show summary
  cConsole->AddLineEx("$$ and $ totalling $ ($) in $.",
    tData.Finish(),
    PluraliseNum(stFiles, "files", "files"),
    PluraliseNum(stDirs, "dir", "dirs"),
    PluraliseNum(qwBytes, "byte", "bytes"),
    ToBytesStr(qwBytes),
    PluraliseNum(cArchives->size(), "archive", "archives"));
}
/* == Dump events list ===================================================== */
static void CBevents(const Arguments &)
{ // Log event count
  cConsole->AddLine(PluraliseNum(cEvtMain->SizeSafe(), "event.", "events."));
}
/* == Stop all sounds from playing ========================================= */
static void CBstopall(const Arguments &)
{ // Tell audio to stop all sounds from playing
  cAudio->Stop();
  // Log event count
  cConsole->AddLine("Stopping all sounds from playing.");
}
/* ========================================================================= */
/* ######################################################################### */
/* ## Console commands lookup table. Use the macro below to keep it neat. ## */
/* ## > n = Name of the function from above. (Func must begin with CB).   ## */
/* ## > r = Required number of arguments.    (1 = Just command name).     ## */
/* ## > m = Maximum number of arguments.     (0 = Infinite).              ## */
/* ## > g = Required gui mode setting.       (See CVars::GuiModeFlags).   ## */
/* ######################################################################### */
/* -- The Macro ------------------------------------------------------------ */
#define CC(n,r,m,g) { #n, r, m, CB ## n, GM_ ## g }
/* -- The array (prototyped in IfConsole::Console) ------------------------- */
const ConCmdStaticList conLibList{     // Default cvars (from cvars.hpp)
{ // -- Name -- R  M  GuiMode -------- Name ----- R  M  GuiMode ---------------
  CC(archives,  1, 1, TEXT_NOAUDIO),   CC(areset,    1, 1, TEXT_AUDIO),
  CC(assets,    1, 1, TEXT_NOAUDIO),   CC(audins,    1, 1, TEXT_AUDIO),
  CC(audio,     1, 1, TEXT_AUDIO),     CC(audouts,   1, 1, TEXT_AUDIO),
  CC(bins,      1, 1, TEXT_NOAUDIO),   CC(certs,     1, 1, TEXT_NOAUDIO),
  CC(cla,       1, 2, TEXT_NOAUDIO),   CC(clh,       1, 1, TEXT_NOAUDIO),
  CC(cls,       1, 1, TEXT_NOAUDIO),   CC(cmds,      1, 2, TEXT_NOAUDIO),
  CC(con,       1, 1, TEXT_NOAUDIO),   CC(conlog,    1, 1, TEXT_NOAUDIO),
  CC(cpu,       1, 1, TEXT_NOAUDIO),   CC(crash,     1, 1, TEXT_NOAUDIO),
  CC(credits,   1, 2, TEXT_NOAUDIO),   CC(cvars,     1, 2, TEXT_NOAUDIO),
  CC(cvclr,     1, 1, TEXT_NOAUDIO),   CC(cvload,    1, 1, TEXT_NOAUDIO),
  CC(cvnpk,     1, 1, TEXT_NOAUDIO),   CC(cvpend,    1, 2, TEXT_NOAUDIO),
  CC(cvsave,    1, 1, TEXT_NOAUDIO),   CC(dir,       1, 2, TEXT_NOAUDIO),
  CC(env,       1, 2, TEXT_NOAUDIO),   CC(events,    1, 1, TEXT_NOAUDIO),
  CC(fbos,      1, 1, GRAPHICS),       CC(files,     1, 1, TEXT_NOAUDIO),
  CC(find,      2, 0, TEXT_NOAUDIO),   CC(fonts,     1, 1, GRAPHICS),
  CC(ftfs,      1, 1, TEXT_NOAUDIO),   CC(gpu,       1, 1, GRAPHICS),
  CC(images,    1, 2, TEXT_NOAUDIO),   CC(imgfmts,   1, 1, TEXT_NOAUDIO),
  CC(input,     1, 1, GRAPHICS),       CC(jsons,     1, 1, TEXT_NOAUDIO),
  CC(lcalc,     2, 0, TEXT_NOAUDIO),   CC(lend,      1, 1, TEXT_NOAUDIO),
  CC(lexec,     2, 0, TEXT_NOAUDIO),   CC(lfuncs,    1, 1, TEXT_NOAUDIO),
  CC(lgc,       1, 1, TEXT_NOAUDIO),   CC(log,       1, 1, TEXT_NOAUDIO),
  CC(logclr,    1, 1, TEXT_NOAUDIO),   CC(lpause,    1, 1, TEXT_NOAUDIO),
  CC(lreset,    1, 1, TEXT_NOAUDIO),   CC(lresume,   1, 1, TEXT_NOAUDIO),
  CC(lstack,    1, 1, TEXT_NOAUDIO),   CC(lvars,     1, 0, TEXT_NOAUDIO),
  CC(masks,     1, 1, TEXT_NOAUDIO),   CC(mem,       1, 1, TEXT_NOAUDIO),
  CC(mlist,     1, 1, GRAPHICS),       CC(mods,      1, 1, TEXT_NOAUDIO),
  CC(objs,      1, 1, TEXT_NOAUDIO),   CC(oglext,    2, 2, GRAPHICS),
  CC(oglfunc,   2, 2, GRAPHICS),       CC(palettes,  2, 2, GRAPHICS),
  CC(pcmfmts,   1, 1, TEXT_NOAUDIO),   CC(pcms,      1, 1, TEXT_NOAUDIO),
  CC(quit,      1, 1, TEXT_NOAUDIO),   CC(restart,   1, 2, TEXT_NOAUDIO),
  CC(samples,   1, 1, TEXT_AUDIO),     CC(shaders,   1, 2, GRAPHICS),
  CC(shot,      1, 1, GRAPHICS),       CC(sockets,   1, 2, TEXT_NOAUDIO),
  CC(sockreset, 2, 2, TEXT_NOAUDIO),   CC(sources,   1, 1, TEXT_AUDIO),
  CC(sqlcheck,  1, 1, TEXT_NOAUDIO),   CC(sqldefrag, 1, 1, TEXT_NOAUDIO),
  CC(sqlend,    1, 1, TEXT_NOAUDIO),   CC(sqlexec,   2, 0, TEXT_NOAUDIO),
  CC(stopall,   1, 1, TEXT_AUDIO),     CC(streams,   1, 1, TEXT_AUDIO),
  CC(system,    1, 1, TEXT_NOAUDIO),   CC(textures,  1, 1, GRAPHICS),
  CC(threads,   1, 1, TEXT_NOAUDIO),   CC(time,      1, 1, TEXT_NOAUDIO),
  CC(version,   1, 1, TEXT_NOAUDIO),   CC(videos,    1, 1, GRAPHICS),
  CC(vmlist,    1, 2, GRAPHICS),       CC(vreset,    1, 1, GRAPHICS),
  CC(wreset,    1, 1, GRAPHICS),
} };/* --------------------------------------------------------------------- */
#undef CC                              // Done with this macro
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
