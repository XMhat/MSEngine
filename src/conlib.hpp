/* == CONLIB.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module is included inside the main procedure in 'core.cpp' and ## **
** ## handles all the default console cvars. Make sure to update the      ## **
** ## 'ConCmdEnums' enum scope if you modify the order, remove or add     ## **
** ## new console commands.                                               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
const ConCmdStaticList conLibList{{    // Default console commands list
/* ========================================================================= */
// ! archives
// ? Lists archives that are loaded into the game engine.
/* ------------------------------------------------------------------------- */
{ "archives", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get reference to assets collector class and lock it so it's not changed
const LockGuard lgAssetsSync{ cArchives->CollectorGetMutex() };
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("F").Header("D").Header("T").Header("U")
     .Header("FILENAME", false).Reserve(cArchives->size());
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
  StrPluraliseNum(cArchives->size(), "archive", "archives"),
  StrPluraliseNum(stResources, "resource", "resources"));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'archives' function
/* ========================================================================= */
// ! areset
// ? Resets the audio interface.
/* ------------------------------------------------------------------------- */
{ "areset", 1, 1, CF_AUDIO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Reset the audio subsystem and print result of the reset call
cConsole->AddLineExA("Audio subsystem reset ",
  cAudio->ReInit() ? "requested." : "failed.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'areset' function
/* ========================================================================= */
// ! assets
// ? Lists assets that are loaded into the game engine.
/* ------------------------------------------------------------------------- */
{ "assets", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get reference to arrays collector class and lock it so it's not changed
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
     .Header("ASCII", false).Reserve(cAssets->size());
#endif
// Walk mask list and add size of it
for(const Asset *aCptr : *cAssets)
{ // Get reference to memory block
  const DataConst &dcCref = *aCptr;
#if !defined(RELEASE)
  // Set preview size
  const size_t stMax = UtilMinimum(dcCref.Size(), stCount);
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
    strHex += StrHexFromInt(uiChar, 2) + ' ';
    // Put a dot if character is invalid
    if(uiChar < ' ') { strAscii += '.'; continue; }
    // Encode the character
    UtfAppend(uiChar, strAscii);
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
  StrPluraliseNum(cAssets->size(), "block", "blocks"),
  StrPluraliseNum(stTotal, "byte", "bytes"));
#else
cConsole->AddLineEx("$$ totalling $.", tData.Finish(),
  StrPluraliseNum(cAssets->size(), "block", "blocks"),
  StrPluraliseNum(stTotal, "byte", "bytes"));
#endif
/* ------------------------------------------------------------------------- */
} },                                   // End of 'assets' function
/* ========================================================================= */
// ! audins
// ? Lists available audio input devices (unused by engine right now).
/* ------------------------------------------------------------------------- */
{ "audins", 1, 1, CF_AUDIO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Ignore if no audio subsystem initialised
if(!cOal->IsInitialised())
  return cConsole->AddLine("Audio not initialised.");
// Data for device output
Statistic tData;
tData.Header("ID").Header("INPUT DEVICE", false)
     .Reserve(cAudio->GetNumCaptureDevices());
// list audio devices
size_t stDeviceId = 0;
for(const string &strName : cAudio->GetCTDevices())
  tData.DataN(stDeviceId++).Data(strName);
// Output devices
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(stDeviceId, "device.", "devices."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'audins' function
/* ========================================================================= */
// ! audio
// ? Shows information about the currently loaded audio device.
/* ------------------------------------------------------------------------- */
{ "audio", 1, 1, CF_AUDIO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Write information about current audio device
cConsole->AddLineEx(
   "Library: OpenAL $; Flags: $ (0x$$$).\n"
   "Playback: $.\n"
   "Sources: $ allocated (Max $ Stereo; $ Mono).",
  cOal->GetVersion(),
  StrFromEvalTokens({
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
/* ------------------------------------------------------------------------- */
} },                                   // End of 'audio' function
/* ========================================================================= */
// ! audouts
// ? Lists available audio output devices.
/* ------------------------------------------------------------------------- */
{ "audouts", 1, 1, CF_AUDIO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Ignore if no audio subsystem initialised
if(!cOal->IsInitialised())
  return cConsole->AddLine("Audio not initialised.");
// Data for device output
Statistic tData;
tData.Header("ID").Header("OUTPUT DEVICE", false)
     .Reserve(cAudio->GetNumPlaybackDevices());
// list audio devices
size_t stDeviceId = 0;
for(const string &strName : cAudio->GetPBDevices())
  tData.DataN(stDeviceId++).Data(strName);
// Output devices
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(stDeviceId, "device.", "devices."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'audouts' function
/* ========================================================================= */
// ! bins
// ? Lists currently loaded bin objects.
/* ------------------------------------------------------------------------- */
{ "bins", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Text table class to help us write neat output
Statistic tData;
tData.Header("WIDTH").Header("HEIGHT").Header("OCCUPANCY").Header("TOTL")
     .Header("USED").Header("FREE").Reserve(cBins->size());
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
  StrPluraliseNum(cBins->size(), "bin.", "bins."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'bins' function
/* ========================================================================= */
// ! certs
// ? Lists currently loaded X509 certificates which are populated by the
// ? 'net_castore' cvar on startup.
/* ------------------------------------------------------------------------- */
{ "certs", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Make a table to automatically format our data neatly
Statistic tData;
tData.Header("X").Header("FILE", false).Header("C", false).
  Header("CN", false).Reserve(cSockets->size());
// Walk shader list
for(const auto &caPair : cSockets->GetCertList())
{ // Put if expired and the filename on disk
  tData.Data(StrFromBoolYN(CertIsExpired(caPair.second)))
       .Data(caPair.first);
  // Split subject key/value pairs. We couldn't split the data if empty
  const VarsConst ssD{ CertGetSubject(caPair), cCommon->FSlash(), '=' };
  if(ssD.empty()) { tData.Data("??").Data("<No sub>"); continue; }
  // Print country and certificate name
  const StrStrMapConstIt iC{ ssD.find("C") }, iCN{ ssD.find("CN") };
  tData.Data(iC == ssD.cend() ? "--" : CryptURLDecode(iC->second)).
        Data(iCN == ssD.cend() ? "<Unknown>" : CryptURLDecode(iCN->second));
} // Print output and number of shaders listed
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cSockets->GetCertListSize(),
    "root certificate.", "root certificates."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'certs' function
/* ========================================================================= */
// ! cla
// ? Lists all command-line arguments sent to the process.
/* ------------------------------------------------------------------------- */
{ "cla", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Get list of environment variables
const StrVector &svArgs = cCmdLine->GetArgList();
// If parameter was specified?
if(aList.size() > 1)
{ // Convert parmeter to number
  const size_t stArg = StrToNum<size_t>(aList[1]);
  if(stArg >= svArgs.size())
    return cConsole->AddLine("Command line argument index invalid!");
  // Print it and return
  cConsole->AddLine(svArgs[stArg]);
  return;
} // Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("ARGUMENT", false).Reserve(svArgs.size());
// Walk through environment variables add them to the statistic object
size_t stCount = 0;
for(const string &strArg : svArgs) tData.DataN(stCount++).Data(strArg);
// Log counts
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(stCount, "command line argument.",
                        "command line arguments."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cla' function
/* ========================================================================= */
// ! clh
// ? Flushes the historic use of commands entered into the console.
/* ------------------------------------------------------------------------- */
{ "clh", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
cConsole->ClearHistory();
/* ------------------------------------------------------------------------- */
} },                                   // End of 'clh' function
/* ========================================================================= */
// ! cls
// ? Flushes the console output buffer.
/* ------------------------------------------------------------------------- */
{ "cls", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
cConsole->Flush();
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cls' function
/* ========================================================================= */
// ! cmds [partial]
// ? Shows all the available console commands. You can optionally specify
// ? [partial] to partially match the beginning of each command.
/* ------------------------------------------------------------------------- */
{ "cmds", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Setup iterator to find items and return if no commands.
const auto &cbCmds = cConsole->GetCmdsList();
if(cbCmds.empty()) return cConsole->AddLine("No commands are found!");
// Set filter if specified and look for command and if we found one?
const string &strFilter = aList.size() > 1 ? aList[1] : cCommon->Blank();
auto ciItem{ cbCmds.lower_bound(strFilter) };
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
    return cConsole->AddLineEx("$:$.", StrPluraliseNum(stMatched,
      "matching command", "matching commands"), osS.str());
} // No matches
cConsole->AddLineEx("No match from $.",
  StrPluraliseNum(cbCmds.size(), "command", "commands"));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cmds' function
/* ========================================================================= */
{ "con", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Write console status
cConsole->AddLineEx("Console flags are currently 0x$$$ ($).\n"
                    "- Output lines: $ (Maximum: $).\n"
                    "- Input commands: $ (Maximum: $).\n"
                    "- Max input characters: $; Page lines: $.\n"
                    "- Command functions: $ (Lua: $).",
  hex, cConsole->FlagGet(), dec, StrFromEvalTokens({
    { cConsole->FlagIsSet(CF_CANTDISABLE), 'D' },
    { cConsole->FlagIsSet(CF_IGNOREKEY),   'K' },
    { cConsole->FlagIsSet(CF_AUTOSCROLL),  'A' },
    { cConsole->FlagIsSet(CF_AUTOCOPYCVAR),'C' },
    { cConsole->FlagIsSet(CF_INSERT),      'I' },
    { cConsole->FlagIsSet(CF_ENABLED),     'E' }
  }),
  cConsole->GetOutputCount(), cConsole->GetOutputMaximum(),
  cConsole->GetInputCount(), cConsole->GetInputMaximum(),
  cConsole->GetMaximumChars(), cConsole->GetPageLines(),
  cConsole->GetCmdsList().size(), cConsole->GetLuaCmds().size());
/* ------------------------------------------------------------------------- */
} },                                   // End of 'con' function
/* ========================================================================= */
{ "conlog", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Copy all the console lines to log and report how many lines we copied
cConsole->AddLineExA(StrPluraliseNum(cConsole->ToLog(), "line", "lines"),
  " copied to log.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'conlog' function
/* ========================================================================= */
{ "cpu", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Update cpu usage times
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
    cTimer->TimerGetStart(), cTimer->TimerGetDuration(),
      cTimer->TimerGetLimit(), cTimer->TimerGetAccumulator(),
    cSystem->GetCoreFlags(), cSystem->GetCoreFlagsString(),
      cTimer->TimerGetTimeOut(), cTimer->TimerGetTriggers(),
      cLua->GetOpsInterval(),
    cTimer->TimerGetSecond(), cTimer->TimerGetDelay(),
      cTimer->TimerGetTicks());
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cpu' function
/* ========================================================================= */
{ "crash", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
System::CriticalHandler("Requested operation");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'crash' function
/* ========================================================================= */
{ "credits", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Do we have a parameter?
if(aList.size() > 1)
{  // Get index and show decompressed text for it or invalid
  const size_t stIndex = StrToNum<size_t>(aList[1]);
  return cConsole->AddLine(stIndex >= cCredits->CreditGetItemCount() ?
    "The specified credit index is invalid!" :
    cCredits->CreditGetItemText(static_cast<CreditEnums>(stIndex)));
} // Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("NAME", false).Header("VERSION")
     .Header("AUTHOR", false).Reserve(cCredits->CreditGetItemCount());
// For each item, show library information
for(size_t stIndex = 0; stIndex < cCredits->CreditGetItemCount(); ++stIndex)
{ // Get credit for item and add the data for the credit
  const CreditLib &sD =
    cCredits->CreditGetItem(static_cast<CreditEnums>(stIndex));
  tData.DataN(stIndex).Data(sD.GetName()).Data(sD.GetVersion())
       .Data(StrAppend(sD.IsCopyright() ? "\xC2\xA9 " : cCommon->Blank(),
         sD.GetAuthor()));
} // Show number of libs
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cCredits->CreditGetItemCount(),
    "third-party library.", "third-party libraries."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'credits' function
/* ========================================================================= */
{ "cvars", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// If we actually came from the 'cvpend' command, select internal 'load
// from/save to' list else select the main public cvar list.
const CVars::ItemMap &cimList = aList[0][2] == 'p' ? // Optimal and safe
  cCVars->GetInitialVarList() : cCVars->GetVarList();
// Ignore if empty
if(cimList.empty())
  return cConsole->AddLine("No cvars exist in this category!");
// Set filter if specified
const string &strFilter = aList.size() > 1 ? aList[1] : cCommon->Blank();
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
    StrFormat("Status for CVar '$'...\n"
           "- Registered by: $, Flags: 0x$$$, Callback: $.\n"
           "- Current Value<$> = \"$\".",
      strFilter, cD.FlagIsSet(TLUA) ? "Lua" : "Engine",
      hex, cD.FlagGet(), dec, cD.IsTriggerSet() ? "Set" : "None",
      cD.GetValueLength(), cD.GetValue()) };
  // Add default value if set
  if(cD.IsValueChanged())
    lCats.emplace_back(StrFormat("- Default Value<$> = \"$\".",
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
    lCats.emplace_back(StrFormat("- $<$> = $.", cItem.first, lDest.size(),
      lDest.empty() ? string{ "None" } : StrImplode(lDest, ", ")));
  } // Print data about the cvar
  return cConsole->AddLine(StrImplode(lCats, cCommon->Lf()));
} // Try as a lower bound check?
ciItem = cimList.lower_bound(strFilter);
#else
// Try as a lower bound check?
auto ciItem{ cimList.lower_bound(strFilter) };
#endif
if(ciItem != cimList.cend())
{ // Formatted output. Can assume all variables will be printed
  Statistic tData;
  tData.Header("FLAGS").Header("NAME", false).Header("VALUE", false)
       .Reserve(cimList.size());
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
    tData.Data(StrFromEvalTokens({
      // Types
      { true, cD.FlagIsSet(CFILENAME)  ? 'F' :
             (cD.FlagIsSet(CTRUSTEDFN) ? 'T' :
             (cD.FlagIsSet(TSTRING)    ? 'S' :
             (cD.FlagIsSet(TINTEGER)   ? 'I' :
             (cD.FlagIsSet(TFLOAT)     ? 'N' :
             (cD.FlagIsSet(TBOOLEAN)   ? 'B' :
                                         '?'))))) },
      // Permissions
      { cD.FlagIsSet(PBOOT),        '1' }, { cD.FlagIsSet(PSYSTEM),      '2' },
      { cD.FlagIsSet(PUSR),         '3' },
      // Sources
      { cD.FlagIsSet(SENGINE),      '6' }, { cD.FlagIsSet(SCMDLINE),     '7' },
      { cD.FlagIsSet(SAPPCFG),      '8' }, { cD.FlagIsSet(SUDB) ,        '9' },
      // Conditions and operations
      { cD.FlagIsSet(CUNSIGNED),    'U' }, { cD.FlagIsSet(TLUA),         'L' },
      { cD.FlagIsSet(CONFIDENTIAL), 'C' }, { cD.FlagIsSet(CPROTECTED),   'P' },
      { cD.FlagIsSet(CDEFLATE),     'D' }, { cD.FlagIsSet(COMMIT),       'M' },
      { cD.FlagIsSet(LOADED),       'O' }, { cD.FlagIsSet(CSAVEABLE),    'V' },
      { cD.FlagIsSet(OSAVEFORCE),   'Z' }, { cD.FlagIsSet(CPOW2),        'W' },
      { cD.FlagIsSet(CNOTEMPTY),    'Y' }, { cD.FlagIsSet(MTRIM),        'R' },
      { cD.IsTriggerSet(),      'K' }
    // Name and value
    })).Data(strKey).Data(cD.Protect());
  } // Until no more commands
  while(++ciItem != cimList.cend());
  // Print output if we matched commands
  if(stMatched)
    return cConsole->AddLineEx("$$ of $ matched.", tData.Finish(),
      stMatched, StrPluraliseNum(cimList.size(), "cvar", "cvars"));
} // No matches
cConsole->AddLineEx("No match from $.",
  StrPluraliseNum(cimList.size(), "cvar", "cvars"));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cvars' function
/* ========================================================================= */
{ "cvclr", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
cConsole->AddLineExA(StrPluraliseNum(cCVars->Clean(),
  "cvar", "cvars"), " purged.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cvclr' function
/* ========================================================================= */
{ "cvload", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
cConsole->AddLineExA(StrPluraliseNum(cCVars->LoadFromDatabase(),
  "cvar", "cvars"), " reloaded.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cvload' function
/* ========================================================================= */
{ "cvnpk", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Create the private key and if succeeded? Show the result
if(cSql->CreatePrivateKey())
  cConsole->AddLineEx("New private key set and $ variables re-encoded!",
    cCVars->MarkAllEncodedVarsAsCommit());
// Else failed so show the error
else cConsole->AddLine("Failed to create new private key!");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cvnpk' function
/* ========================================================================= */
{ "cvpend", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// We can reuse the 'cvars' callback here but the only way to get to it from
// here is to access this list from the 'CVars' class since it keeps a
// reference to this list. Compiler will most probably just optimise this away
// anyway to a direct access.
cConsole->GetCommand(CC_CVARS).ccbFunc(aList);
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cvpend' function
/* ========================================================================= */
{ "cvsave", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
cConsole->AddLineExA(StrPluraliseNum(cCVars->Save(),
  "cvar", "cvars"), " commited.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'cvsave' function
/* ========================================================================= */
{ "dir", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Make and checkfilename
const string &strVal = aList.size() > 1 ? aList[1] : ".";
const ValidResult vResult = DirValidName(strVal);
if(vResult != VR_OK)
  return cConsole->AddLineEx("Cannot check directory '$': $!",
    strVal, cDirBase->VNRtoStr(vResult));
// Enumerate local directories on disk
const Dir dPath{ StdMove(strVal) };
// Set directory and get directories and files
const string &strDir = aList.size() > 1 ? aList[1] : cCommon->Blank();
// Directory data we are enumerating
struct Item { const uint64_t qwSize;
              const unsigned int uiId;
              const string &strArc; };
typedef map<const string, const Item> StrItemList;
StrItemList silDirs, silFiles;
// Get reference to assets collector class and lock it so it's not changed
const LockGuard lgArchivesSync{ cArchives->CollectorGetMutex() };
// Iterate through the list
for(const Archive*const aCptr : *cArchives)
{ // Get archive class
  const Archive &aCref = *aCptr;
  // Goto next archive if directory specified and is not found
  const StrUIntMap &suimDirs = aCref.GetDirList();
  if(!strDir.empty() && suimDirs.find(strDir) == suimDirs.cend()) continue;
  // Enumerate directories
  for(const auto &dI : suimDirs)
  { // Skip directory if start of directory does not match
    if(strDir != dI.first.substr(0, strDir.length())) continue;
    // Get filename, and continue again if it is a sub-directory/file
    string strN{ StrTrim(dI.first.substr(strDir.length()), '/') };
    if(strN.find('/') != string::npos) continue;
    // Add to directory list and increment directory count
    silDirs.insert({ StdMove(strN),
      { StdMaxUInt64, dI.second, aCref.IdentGet() } });
  } // Enumerate all files in archive
  const StrUIntMap &suimFiles = aCref.GetFileList();
  for(const auto &dI : suimFiles)
  { // Skip file if start of directory does not match
    if(strDir != dI.first.substr(0, strDir.length())) continue;
    // Get filename, and continue again if it is a sub-directory/file
    string strN{ StrTrim(dI.first.substr(strDir.length()), '/') };
    if(strN.find('/') != string::npos) continue;
    // Add to file list and increment total bytes and file count
    const uint64_t qwSize = aCref.GetSize(dI.second);
    silFiles.insert({ StdMove(strN),
      { qwSize, dI.second, aCref.IdentGet() } });
  }
} // Enumerate local directories and add each entry to file list
for(const auto &dPair : dPath.dDirs)
  silDirs.insert({ StdMove(dPair.first),
    { StdMaxUInt64, StdMaxUInt, {} } });
// Enumerate local files
for(const auto &dPair : dPath.dFiles)
{ // Add to file list and increment byte and file count
  const Dir::Item &fItem = dPair.second;
  silFiles.insert({ StdMove(dPair.first),
    { fItem.qSize, StdMaxUInt, {} } });
} // Directory name and disk name strings
const string strDirName{"<DIR>"}, strDiskName{"<DISK>"};
// Prepare data table for archive display
Statistic tData;
tData.Header("SIZE").Header().Header("ID").Header("ARCHIVE", false)
     .Header("FILENAME", false).Reserve(silDirs.size() + silFiles.size());
// For each directory we found
for(const auto &spPair : silDirs)
{ // Get item data
  const Item &itData = spPair.second;
  // Add directory tag and blank cell
  tData.Data(strDirName).Data();
  // If this is a file on disk?
  if(itData.uiId == StdMaxUInt)
    tData.Data().Data(strDiskName);
  // Is from archive?
  else tData.DataN(itData.uiId).Data(itData.strArc);
  // Add directory name
  tData.Data(StdMove(spPair.first));
} // Files and directories and total bytes matched
uint64_t qwBytes = 0;
// For each file we found
for(const auto &spPair : silFiles)
{ // Get item data
  const Item &itData = spPair.second;
  // Add size and humann readable size
  tData.DataN(itData.qwSize).Data(StrToBytes(itData.qwSize));
  // If this is a file on disk?
  if(itData.uiId == StdMaxUInt)
    tData.Data().Data(strDiskName);
  // Is from archive?
  else tData.DataN(itData.uiId).Data(itData.strArc);
  // Add file name
  tData.Data(StdMove(spPair.first));
  // Add to file size
  qwBytes += itData.qwSize;
} // Show summary
cConsole->AddLineEx("$$ and $ totalling $ ($) in $.",
  tData.Finish(),
  StrPluraliseNum(silFiles.size(), "files", "files"),
  StrPluraliseNum(silDirs.size(), "dir", "dirs"),
  StrPluraliseNum(qwBytes, "byte", "bytes"),
  StrToBytes(qwBytes),
  StrPluraliseNum(cArchives->size(), "archive", "archives"));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'dir' function
/* ========================================================================= */
{ "env", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Get list of environment variables
const StrStrMap &ssmEnv = cCmdLine->GetEnvList();
// If parameter was specified?
if(aList.size() > 1)
{ // Get parameter name and find it
  const StrStrMapConstIt &ssmciVar = ssmEnv.find(aList[1]);
  if(ssmciVar == ssmEnv.cend())
    return cConsole->AddLine("Environment variable not found!");
  // Print it and return
  cConsole->AddLineExA(ssmciVar->first, '=', ssmciVar->second);
  return;
} // Text table class to help us write neat output
Statistic tData;
tData.Header("VARIABLE").Header("VALUE", false).Reserve(ssmEnv.size());
// Walk through environment variables add them to the statistic object
for(const auto &eVar : ssmEnv) tData.Data(eVar.first).Data(eVar.second);
// Log counts
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(ssmEnv.size(), "environment variable.",
                              "environment variables."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'env' function
/* ========================================================================= */
{ "events", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Log event counts
cConsole->AddLineEx("$ and $.",
  StrPluraliseNum(cEvtMain->SizeSafe(), "engine event", "engine events"),
  StrPluraliseNum(cEvtWin->SizeSafe(), "window event", "window events"));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'events' function
/* ========================================================================= */
{ "fbos", 1, 1, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
const function ShowFboInfo{ [](const Fbo &fC, Statistic &tData,
  size_t &stTriangles, size_t &stCommands)
{ // Add to totals
  stTriangles += fC.GetTris();
  stCommands += fC.GetCmds();
  // Show FBO status
  tData.DataN(fC.CtrGet()).DataN(fC.uiFBO).DataN(fC.uiFBOtex)
       .Data(StrFromEvalTokens({
          { fC.IsTransparencyEnabled(), 'A' }, { fC.IsClearEnabled(), 'C' },
       }))
       .DataN(fC.GetFilter()).DataN(fC.DimGetWidth()).DataN(fC.DimGetHeight())
       .DataN(fC.fcStage.GetCoLeft()).DataN(fC.fcStage.GetCoTop())
       .DataN(fC.fcStage.GetCoRight()).DataN(fC.fcStage.GetCoBottom())
       .DataN(fC.GetTris()).DataN(fC.GetCmds()).DataN(fC.GetTrisReserved())
       .DataN(fC.GetCmdsReserved()).Data(fC.IdentGet());
} };
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("FI").Header("TI").Header("FL").Header("FT")
     .Header("SIZW").Header("SIZH").Header("STX1").Header("STY1")
     .Header("STX2").Header("STY2").Header("TRIS").Header("CMDS")
     .Header("TRES").Header("CRES").Header("IDENTIFIER", false)
     .Reserve(2 + cFbos->size());
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
  StrPluraliseNum(2 + cFbos->size(), "framebuffer", "framebuffers"),
  StrPluraliseNum(stTriangles, "triangle", "triangles"),
  StrPluraliseNum(stCommands, "command", "commands"));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'fbos' function
/* ========================================================================= */
{ "files", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("FLAG").Header("FD").Header("ERRNO")
     .Header("POSITION").Header("LENGTH").Header("FILENAME", false)
     .Reserve(cFiles->size());
// Walk through textures classes
for(File*const fCptr : *cFiles)
{ // Get pointer to class
  File &fCref = *fCptr;
  // Show data
  tData.DataN(fCref.CtrGet())
       .Data(StrFromEvalTokens({
         { fCref.FStreamOpened(),     'O' },
         { fCref.FStreamFErrorSafe(), 'E' },
         { fCref.FStreamIsEOFSafe(),  'X' }
       }))
       .DataN(fCref.FStreamGetIDSafe()).DataN(fCref.FStreamGetErrNo())
       .DataN(fCref.FStreamGetPositionSafe()).DataN(fCref.FStreamSizeSafe())
       .Data(fCref.IdentGet());
} // Log counts
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cFiles->size(), "file.", "files."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'files' function
/* ========================================================================= */
{ "find", 2, 0, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Find text in console backlog and if not found, show message
cConsole->FindText(StrImplode(aList, 1));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'find' function
/* ========================================================================= */
{ "fonts", 1, 1, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
const function ShowFontInfo{ [](Statistic &tData, const Font &fC)
  { tData.DataN(fC.CtrGet())
         .DataC(fC.FlagIsSet(FF_FLOORADVANCE) ? 'F' :
               (fC.FlagIsSet(FF_CEILADVANCE)  ? 'C' :
               (fC.FlagIsSet(FF_ROUNDADVANCE) ? 'R' : '-')))
         .Data(StrFromEvalTokens({
           { fC.FlagIsSet(FF_USEGLYPHSIZE), 'S' },
           { fC.FlagIsSet(FF_STROKETYPE2),  'A' }
         }))
         .DataN(fC.GetCharScale(), 6)
         .DataN(fC.GetTexOccupancy(), 6)
         .DataN(fC.GetCharCount())
         .Data(fC.IdentGet()); } };
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("R").Header("FLAG").Header("SCALE")
     .Header("TEXOCPCY").Header("CC").Header("NAME", false)
     .Reserve(1 + cFonts->size());
// Include console font
ShowFontInfo(tData, cConsole->GetFontRef());
// Walk through textures classes
for(const Font*const fCptr : *cFonts) ShowFontInfo(tData, *fCptr);
// Log counts including the static console font class
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cFonts->size() + 1, "font.", "fonts."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'fonts' function
/* ========================================================================= */
{ "ftfs", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get reference to fonts collector class and lock it so it's not changed
const LockGuard lgFtfsSync{ cFtfs->CollectorGetMutex() };
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("GLYPH").Header("FW").Header("FH").Header("DW")
     .Header("DH").Header("STYLE", false).Header("FAMILY", false)
     .Header("FILENAME", false).Reserve(cFtfs->size());
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
  StrPluraliseNum(cFtfs->size(), "ftf.", "ftfs."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'ftfs' function
/* ========================================================================= */
{ "gpu", 1, 1, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Write opengl rendering stats
cConsole->AddLineEx(
  "Renderer: $ on $.\n"
  "Version: $ by $.\n"
  "$" // Memory (optional)
  "Window: $x$ ($) @ $x$ (*$x$); DFlags: 0x$$.\n"
  "- State: $$ ($); Max: $^2; Viewport: $x$ ($).\n"
  "- Ortho: $x$ ($); Total: $x$ ($).\n"
  "- Stage: $,$,$,$; Fbo: $x$; OFlags: 0x$$.\n"
  "- Polygons: $$/$; Commands: $/$.\n"
  "FPS: $$/s ($/s); Eff: $%; Limit: $.",
  cOgl->GetRenderer(), cDisplay->GetMonitorName(),
  cOgl->GetVersion(), cOgl->GetVendor(),
  cOgl->FlagIsSet(GFL_HAVEMEM) ?
    StrFormat("Memory: $ mBytes ($ mBytes available).\n",
      cOgl->GetVRAMTotal() / 1048576, cOgl->GetVRAMFree() / 1048576) :
    cCommon->Blank(),
  cInput->GetWindowWidth(), cInput->GetWindowHeight(),
    StrFromRatio(cInput->GetWindowWidth(), cInput->GetWindowHeight()),
    cDisplay->GetWindowPosX(), cDisplay->GetWindowPosY(),
    cDisplay->GetWindowScaleWidth(), cDisplay->GetWindowScaleHeight(),
    hex, cDisplay->FlagGet(),
  dec, cDisplay->GetFSType(), cDisplay->GetFSTypeString(),
  cOgl->MaxTexSize(), cFboMain->DimGetWidth(), cFboMain->DimGetHeight(),
    StrFromRatio(cFboMain->DimGetWidth(), cFboMain->DimGetHeight()),
    cFboMain->GetOrthoWidth(), cFboMain->GetOrthoHeight(),
    StrFromRatio(cFboMain->GetOrthoWidth(), cFboMain->GetOrthoHeight()),
    cFboMain->fboMain.GetCoRight(), cFboMain->fboMain.GetCoBottom(),
    StrFromRatio(cFboMain->fboMain.GetCoRight(),
      cFboMain->fboMain.GetCoBottom()),
  cFboMain->fboMain.fcStage.GetCoLeft(), cFboMain->fboMain.fcStage.GetCoTop(),
    cFboMain->fboMain.fcStage.GetCoRight(),
    cFboMain->fboMain.fcStage.GetCoBottom(),
    cFboMain->fboMain.DimGetWidth(), cFboMain->fboMain.DimGetHeight(),
    hex, cOgl->FlagGet(),
  dec, cFboMain->fboMain.GetTris(),
    cFboMain->fboMain.GetTrisReserved(),
    cFboMain->fboMain.GetCmds(),
    cFboMain->fboMain.GetCmdsReserved(),
  fixed, cFboMain->dRTFPS, cDisplay->GetRefreshRate(),
  cFboMain->dRTFPS / cDisplay->GetRefreshRate() * 100,
  cOgl->GetLimit());
/* ------------------------------------------------------------------------- */
} },                                   // End of 'gpu' function
/* ========================================================================= */
{ "images", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Get reference to images collector class and lock it so it's not changed
const LockGuard lgImagesSync{ cImages->CollectorGetMutex() };
// If we have more than one parameter?
if(aList.size() == 2)
{ // Get slot data and return if it is a bad slot
  const size_t stId = StrToNum<size_t>(aList[1]);
  if(stId >= cImages->size()) return cConsole->AddLine("Invalid image id!");
  Image &iCref = **next(cImages->cbegin(), static_cast<ssize_t>(stId));
  // Text table class to help us write neat output
  Statistic tData;
  tData.Header("SIZX").Header("SIZY").Header("SIZE")
       .Reserve(iCref.GetSlotCount());
  // For each slot
  for(const ImageSlot &imdD : iCref.GetSlotsConst())
    tData.DataN(imdD.DimGetWidth()).DataN(imdD.DimGetHeight())
         .DataN(imdD.Size());
  // Log texture counts
  cConsole->AddLineEx("$$ in $.", tData.Finish(),
    StrPluraliseNum(iCref.GetSlotCount(), "slot", "slots"), iCref.IdentGet());
  // Done
  return;
} // Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("FLAGS", false).Header("SIZW").Header("SIZH")
     .Header("BI").Header("B").Header("PIXT").Header("S").Header("ALLOC")
     .Header("NAME", false).Reserve(cImages->size());
// Walk through textures classes
for(const Image*const iCptr : *cImages)
{ // Convert to reference
  const Image &iCref = *iCptr;
  // Add data to table
  tData.DataN(iCref.CtrGet()).Data(StrFromEvalTokens({
    { iCref.IsDynamic(),          'Y' }, { iCref.IsNotDynamic(),       'S' },
    { iCref.IsPurposeFont(),      'F' }, { iCref.IsPurposeImage(),     'I' },
    { iCref.IsPurposeTexture(),   'X' }, { iCref.IsLoadAsDDS(),        'D' },
    { iCref.IsLoadAsGIF(),        'G' }, { iCref.IsLoadAsJPG(),        'J' },
    { iCref.IsLoadAsPNG(),        'P' }, { iCref.IsConvertAtlas(),     'A' },
    { iCref.IsActiveAtlas(),      'a' }, { iCref.IsConvertReverse(),   'E' },
    { iCref.IsActiveReverse(),    'e' }, { iCref.IsConvertRGB(),       'H' },
    { iCref.IsActiveRGB(),        'h' }, { iCref.IsConvertRGBA(),      'D' },
    { iCref.IsActiveRGBA(),       'd' }, { iCref.IsConvertBGROrder(),  'W' },
    { iCref.IsActiveBGROrder(),   'w' }, { iCref.IsConvertBinary(),    'N' },
    { iCref.IsActiveBinary(),     'n' }, { iCref.IsConvertGPUCompat(), 'O' },
    { iCref.IsActiveGPUCompat(),  'o' }, { iCref.IsConvertRGBOrder(),  'B' },
    { iCref.IsActiveRGBOrder(),   'b' }, { iCref.IsCompressed(),       'C' },
    { iCref.IsPalette(),          'L' }, { iCref.IsMipmaps(),          'M' },
    { iCref.IsReversed(),         'R' }
  })).DataN(iCref.DimGetWidth()).DataN(iCref.DimGetHeight())
     .DataN(iCref.GetBitsPerPixel()).DataN(iCref.GetBytesPerPixel())
     .DataH(iCref.GetPixelType(),4).DataN(iCref.GetSlotCount())
     .DataN(iCref.GetAlloc()).Data(iCref.IdentGet());
} // Log texture counts
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cImages->size(), "image.", "images."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'images' function
/* ========================================================================= */
{ "imgfmts", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("FL").Header("EXT").Header("NAME", false)
     .Reserve(cImageFmts->size());
// For each plugin
for(const ImageFmt*const iCptr : *cImageFmts)
{ // Get refernece to plugin
  const ImageFmt &iCref = *iCptr;
  // Write data
  tData.DataN(iCref.CtrGet())
       .Data(StrFromEvalTokens({ { iCref.HaveLoader(), 'L' },
                              { iCref.HaveSaver(),  'S' } }))
       .Data(iCref.GetExt()).Data(iCref.GetName());
} // Log total plugins
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cImageFmts->size(), "format.", "formats."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'imgfmts' function
/* ========================================================================= */
{ "input", 1, 1, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get joysticks data
const JoyList &jlList = cInput->GetConstJoyList();
// Joysticks connected
size_t stCount = 0;
// Make a table to automatically format our data neatly
Statistic tData;
tData.Header("ID").Header("FL")
     .Header("AX").Header("BT").Header("NAME", false)
     .Reserve(jlList.size());
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
  StrPluraliseNum(stCount, "input", "inputs"), jlList.size());
/* ------------------------------------------------------------------------- */
} },                                   // End of 'input' function
/* ========================================================================= */
{ "jsons", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get reference to jsons collector class and lock it so it's not changed
const LockGuard lgJsonsSync{ cJsons->CollectorGetMutex() };
// Print totals
cConsole->AddLineExA(StrPluraliseNum(cJsons->size(), "json.", "jsons."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'jsons' function
/* ========================================================================= */
{ "lcalc", 2, 0, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
cConsole->AddLine(cLua->CompileStringAndReturnResult(
  StrFormat("return $", StrImplode(aList, 1))));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lcalc' function
/* ========================================================================= */
{ "lend", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Re-init lua and inform user of the result
cConsole->AddLine(cLua->TryEventOrForce(EMC_LUA_END) ?
  "Bypassing guest end routine and going stand-by!" :
  "Asking guest to end execution and going stand-by.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lend' function
/* ========================================================================= */
{ "lexec", 2, 0, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
cConsole->AddLine(cLua->CompileStringAndReturnResult(StrImplode(aList, 1)));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lexec' function
/* ========================================================================= */
{ "lfuncs", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get reference to luarefs collector class and lock it so it's not changed
const LockGuard lgLuaRefsSync(cLuaFuncs->CollectorGetMutex());
// Make a table to automatically format our data neatly
Statistic tData;
tData.Header("ID").Header("FLAGS").Header("DATA", false).Header("MID")
     .Header("SID").Header("NAME", false).Reserve(cLuaFuncs->size());
// Walk sample list
for(const LuaFunc*const lCptr : *cLuaFuncs)
{ // Get sample
  const LuaFunc &lCref = *lCptr;
  // Put on stack
  lCref.LuaFuncPushFunc();
  // Print totals info
  tData.DataN(lCref.CtrGet())
       .Data(LuaUtilGetStackTokens(cLuaFuncs->LuaRefGetState(), -1))
       .Data(LuaUtilGetStackType(cLuaFuncs->LuaRefGetState(), -1))
       .DataN(lCref.LuaFuncGet()).DataN(lCref.LuaFuncGetSaved())
       .Data(lCref.IdentGet());
  // Remove what we pushed
  LuaUtilRmStack(cLuaFuncs->LuaRefGetState());
} // Number of items in buffer
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cLuaFuncs->size(), "function.", "functions."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lfuncs' function
/* ========================================================================= */
{ "lgc", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Free data and get bytes freed
const size_t stT = cLua->GarbageCollect();
// Return if no bytes freed
if(!stT) return cConsole->AddLine("No unreferenced memory!");
// Report how much was freed
cConsole->AddLineEx("$ bytes ($) freed.", stT, StrToBytes(stT));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lgc' function
/* ========================================================================= */
{ "log", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Colours for log levels
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
cConsole->AddLineExA(StrPluraliseNum(cLog->size(), "line.", "lines."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'log' function
/* ========================================================================= */
{ "logclr", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Lock access to the log
const LockGuard lgLogSync{ cLog->GetMutex() };
// Ignore if not empty
if(cLog->empty()) return cConsole->AddLine("No log lines to clear!");
// Get log lines count
const size_t stCount = cLog->size();
// Clear the log
cLog->Clear();
// Say we cleared it
cConsole->AddLineExA(StrPluraliseNum(stCount,
  "log line cleared.", "log lines cleared."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'logclr' function
/* ========================================================================= */
{ "lpause", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
cEvtMain->Add(EMC_LUA_PAUSE);
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lpause' function
/* ========================================================================= */
{ "lreset", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Re-init lua and inform user of the result
cConsole->AddLine(cLua->TryEventOrForce(EMC_LUA_REINIT) ?
  "Bypassing guest end routine and restarting execution!" :
  "Asking guest to end execution and restarting execution.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lreset' function
/* ========================================================================= */
{ "lresume", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
cEvtMain->Add(EMC_LUA_RESUME);
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lresume' function
/* ========================================================================= */
{ "lstack", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get lua state
lua_State*const lS = cLua->GetState();
// Get number of items in stack
const int iCount = LuaUtilStackSize(lS);
// Setup output spreadsheet
Statistic tData;
tData.Header("ID").Header("FLAGS").Header("NAME", false).Header("VALUE")
     .Reserve(iCount);
// Enumerate each stack element (1 is the first item)
for(int iIndex = 1; iIndex <= iCount; ++iIndex)
  tData.DataN(iIndex)
       .Data(LuaUtilGetStackTokens(lS, iIndex))
       .DataN(-(iCount - (iIndex + 1)))
       .Data(LuaUtilGetStackType(lS, iIndex));
// Print number of element
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(iCount, "element.", "elements."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lstack' function
/* ========================================================================= */
{ "lvars", 1, 0, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Get lua state
lua_State*const lS = cLua->GetState();
// Save stack position and restore it when this function call completes. We
// do this because we could write any number of values to the stack
// depending on how many child variables the user specifies. This simple
// class creation simplifies the cleanup process.
const LuaStackSaver lssSaved{ lS };
// We need free items on the stack, leave empty if not
if(!LuaUtilIsStackAvail(lS, aList.size()))
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
  for(const int iIndex = LuaUtilStackSize(lS); ++clItem != aList.cend();)
  { // Get name of parameter and if it's empty? Return empty sub-table
    const string &strParam = *clItem;
    if(strParam.empty()) return cConsole->AddLine("Empty sub-table name!");
    // ...and if its a valid number?
    if(StrIsInt(strParam))
    { // Get value by index and keep searching for more tables
      LuaUtilGetRefEx(lS, -1, StrToNum<lua_Integer>(strParam));
      if(lua_istable(lS, -1)) continue;
      // Restore where we were in the stack
      LuaUtilPruneStack(lS, iIndex);
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
for(LuaUtilPushNil(lS); lua_next(lS, -2); LuaUtilRmStack(lS))
{ // Index is an integer? Create item info struct and add to list
  if(lua_isinteger(lS, -2))
    smmList.insert({ StrFromNum(lua_tointeger(lS, -2)),
      { LuaUtilGetStackType(lS, -1), LuaUtilGetStackTokens(lS, -1) } });
  // For everything else. Create item info struct and add to list
  else smmList.insert({ lua_tostring(lS, -2),
    { LuaUtilGetStackType(lS, -1), LuaUtilGetStackTokens(lS, -1) } });
} // Build string to output
Statistic tData;
tData.Header("FLAGS").Header("NAME", false).Header("VALUE", false)
     .Reserve(smmList.size());
for(const auto &sprIt : smmList)
  tData.Data(sprIt.second.second)
       .Data(sprIt.first)
       .Data(sprIt.second.first);
// Print number of items
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(smmList.size(), "item.", "items."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'lvars' function
/* ========================================================================= */
{ "masks", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("WIDTH").Header("HEIGHT").Header("TOTAL")
     .Header("ALLOC").Header("NAME", false).Reserve(cMasks->size());
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
  StrPluraliseNum(cMasks->size(), "mask.", "masks."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'masks' function
/* ========================================================================= */
{ "mem", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Update counters
cSystem->UpdateMemoryUsageData();
// Precalculate lua and sql usage.
const size_t stUsage = LuaUtilGetUsage(cLua->GetState());
const size_t stSqlUse = cSql->HeapUsed();
// Report lua and sql memory usage
Statistic tData;
tData.Header("BYTES", true).Header()
     .Header("COMPONENT", false).DupeHeader()
     .Reserve(6)
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
/* ------------------------------------------------------------------------- */
} },                                   // End of 'mem' function
/* ========================================================================= */
{ "mlist", 1, 1, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Header (to be printed twice)
Statistic tData;
tData.Header("ID").Header("A").Header("POSX").Header("POSY").Header("HORI")
     .Header("VERT").Header("RATIO").Header("RD").Header("GD").Header("BD")
     .Header("TD").Header("HTZ").Header("DWIN").Header("DHIN").Header("SIZE")
     .Header("NAME", false).Reserve(cDisplay->GetMonitorsCount());
// Walk the monitors list
for(const GlFWMonitor &mItem : cDisplay->GetMonitors())
{ // Get video mode information
  const GlFWRes &rItem = *mItem.Primary();
  // Write id number
  tData.DataN(mItem.Index())
       .Data(StrFromBoolYN(&mItem == cDisplay->GetSelectedMonitor()))
       .DataN(mItem.CoordGetX()).DataN(mItem.CoordGetY())
       .DataN(rItem.Width()).DataN(rItem.Height())
       .Data(StrFromRatio(rItem.Width(), rItem.Height()))
       .DataN(rItem.Red()).DataN(rItem.Green()).DataN(rItem.Blue())
       .DataN(rItem.Depth()).DataN(rItem.Refresh())
       .DataN(mItem.WidthInch(), 1)
       .DataN(mItem.HeightInch(), 1)
       .DataN(mItem.DiagonalInch(), 1)
       .Data(mItem.Name());
} // Write total monitors found
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cDisplay->GetMonitorsCount(), "monitor", "monitors"),
    " detected.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'mlist' function
/* ========================================================================= */
{ "mods", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Make a table to automatically format our data neatly
Statistic tData;
tData.Header("DESCRIPTION").Header("VERSION", false).Header("VENDOR", false)
     .Header("MODULE", false).Reserve(cSystem->size());
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
  StrPluraliseNum(cSystem->size(), "module.", "modules."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'mods' function
/* ========================================================================= */
{ "objs", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Typedefs for building memory usage data
struct MemoryUsageItem
  { const string &strName; size_t stCount, stBytes; }
    muiTotal{ cCommon->Blank(), 0, 0 };
const string strCmds{"Cmds"}, strCVars{"CVars"};
typedef list<MemoryUsageItem> MemoryUsageItems;
// Helper macros so there is not as much spam
#define MSSEX(x,f,i) { x, f, f * sizeof(i) }
#define MSS(x) \
  MSSEX(c ## x ## s->IdentGet(), c ## x ## s->CollectorCount(), x)
// Build memory usage items database
MemoryUsageItems muiData{ {
  MSS(Archive), MSS(Asset),   MSS(Bin),     MSS(Clip),
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
stData.Header("TYPE").Header("#").Header("STACK").Header().DupeHeader(2)
      .Reserve(10);
// Add entries
for(const auto &muiPair : muiData)
{ // Add sizes to total
  muiTotal.stCount += muiPair.stCount;
  muiTotal.stBytes += muiPair.stBytes;
  // Add data to table
  stData.Data(muiPair.strName)
        .DataN(muiPair.stCount)
        .DataN(muiPair.stBytes)
        .Data(StrAppend('(', StrToBytes(muiPair.stBytes, 0),')'));
} // Display output to user
cConsole->AddLineEx("$$ totalling $ ($).", stData.Finish(),
  StrPluraliseNum(muiTotal.stCount, "object", "objects"),
  StrPluraliseNum(muiTotal.stBytes, "byte", "bytes"),
  StrToBytes(muiTotal.stBytes, 0));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'objs' function
/* ========================================================================= */
{ "oglext", 2, 2, CF_VIDEO, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Get extension name and output if the extension is supported
const string &strX = aList[1];
cConsole->AddLineEx(
  "Extension '$' is$ supported by the selected graphics device.",
    strX, cOgl->HaveExtension(strX.c_str()) ? cCommon->Blank() : " NOT");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'oglext' function
/* ========================================================================= */
{ "oglfunc", 2, 2, CF_VIDEO, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Get function name and output if the extension is supported
const string &strF = aList[1];
cConsole->AddLineEx(
  "Function '$' is$ supported by the selected graphics device.",
    strF, GlFWProcExists(strF.c_str()) ? cCommon->Blank() : " NOT");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'oglfunc' function
/* ========================================================================= */
{ "palettes", 2, 2, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Print totals
cConsole->AddLineExA(StrPluraliseNum(cPalettes->size(),
  "palette.", "palettes."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'palettes' function
/* ========================================================================= */
{ "pcmfmts", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("FL").Header("EXT").Header("NAME", false)
     .Reserve(cPcmFmts->size());
// For each plugin
for(const PcmFmt*const pCptr : *cPcmFmts)
{ // Get refernece to plugin
  const PcmFmt &pCref = *pCptr;
  // Write data
  tData.DataN(pCref.CtrGet())
       .Data(StrFromEvalTokens({
         { pCref.HaveLoader(), 'L' },
         { pCref.HaveSaver(),  'S' } }))
       .Data(pCref.GetExt()).Data(pCref.GetName());
} // Log total plugins
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cPcmFmts->size(), "format.", "formats."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'pcmfmts' function
/* ========================================================================= */
{ "pcms", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get reference to pcms collector class and lock it so it's not changed
const LockGuard lgPcmsSync{ cPcms->CollectorGetMutex() };
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("FLAGS", false).Header("RATE")
     .Header("C").Header("BT").Header("PFMT").Header("ALLOC")
     .Header("NAME", false).Reserve(cPcms->size());
// Walk through textures classes
for(const Pcm*const pCptr : *cPcms)
{ // Get pointer to class
  const Pcm &pCref = *pCptr;
  // Add data to table
  tData.DataN(pCref.CtrGet())
       .Data(StrFromEvalTokens({
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
  StrPluraliseNum(cPcms->size(), "pcm.", "pcms."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'pcms' function
/* ========================================================================= */
{ "quit", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Quit the engine and inform user of the result
cConsole->AddLine(cLua->TryEventOrForce(EMC_QUIT) ?
  "Bypassing guest end routine and terminating engine!" :
  "Asking guest to end execution and terminating engine.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'quit' function
/* ========================================================================= */
{ "restart", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Restart the process and inform user of the result
cConsole->AddLine(aList.size() == 2 ?
  (cLua->TryEventOrForce(EMC_QUIT_RESTART_NP) ?
    "Bypassing guest end routine and restarting engine with no args!" :
    "Asking guest to end execution and restarting engine with no args.") :
  (cLua->TryEventOrForce(EMC_QUIT_RESTART) ?
    "Bypassing guest end routine and restarting engine!" :
    "Asking guest to end execution and restarting engine."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'restart' function
/* ========================================================================= */
{ "samples", 1, 1, CF_AUDIO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Make a table to automatically format our data neatly
Statistic tData;
tData.Header("ID").Header("BID").Header("NAME", false)
     .Reserve(cSamples->size());
// Walk sample object list
for(const Sample*const sCptr : *cSamples)
{ // Get sample object reference
  const Sample &sCref = *sCptr;
  // Print totals info
  tData.DataN(sCref.CtrGet()).DataN(sCref.front()).Data(sCref.IdentGet());
} // Number of items in buffer
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cSamples->size(), "sample.", "samples."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'samples' function
/* ========================================================================= */
{ "shaders", 1, 2, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Make a table to automatically format our data neatly
Statistic tData;
tData.Header("ID").Header("PID").Header("L").Header("C").Header("SID")
     .Header("TYPE").Header("CODE").Header("NAME", false)
     .Reserve(cShaders->size() * 3);
// Shader count
size_t stShaders = 0;
// Walk shader list
for(const Shader*const sCptr : *cShaders)
{ // Get shader and its list
  const Shader &sCref = *sCptr;
  // Store shader information
  tData.DataN(sCref.CtrGet()).DataN(sCref.GetProgramId())
       .Data(StrFromBoolYN(sCref.IsLinked())).DataN(sCref.size())
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
  StrPluraliseNum(cShaders->size(), "program", "programs"),
  StrPluraliseNum(stShaders, "shader", "shaders"));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'shaders' function
/* ========================================================================= */
{ "shot", 1, 1, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
cSShot->DumpMain();
/* ------------------------------------------------------------------------- */
} },                                   // End of 'shot' function
/* ========================================================================= */
{ "sockets", 1, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Id specified?
if(aList.size() == 2)
{ // Parse the specified id
  const unsigned int uiId = StrToNum<unsigned int>(aList[1]);
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
        StrFromBoolTF(sCref.IsSecure()), StrIsBlank(sCref.GetCipher()));
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
    StrIsBlank(sCref.GetRealHost(), "<Unresolved>"),
    sCref.GetRXQCount(), sCref.GetRXpkt(), sCref.GetRX(),
      StrToBytes(sCref.GetRX()),
      StrShortFromDuration(cmHiRes.TimePointToClampedDouble(sCref.GetTRead())),
    sCref.GetTXQCount(), sCref.GetTXpkt(), sCref.GetTX(),
      StrToBytes(sCref.GetTX()),
      StrShortFromDuration(
        cmHiRes.TimePointToClampedDouble(sCref.GetTWrite())),
    StrFromBoolTF(sCref.IsSecure()), StrIsBlank(sCref.GetCipher()),
    StrShortFromDuration(fdConnect + fdInitial),
      StrShortFromDuration(fdConnect), StrShortFromDuration(fdInitial));
} // Make neatly formatted table
Statistic tData;
tData.Header("ID").Header("FLAGS").Header("IP").Header("PORT")
     .Header("ADDRESS", false).Reserve(cSockets->size());
// Walk through sockets
for(const Socket*const oCptr : *cSockets)
{ // Get pointer to class
  const Socket &sCref = *oCptr;
  // Get status
  const SocketFlagsConst ssType{ sCref.FlagGet() };
  // Show status
  tData.DataN(sCref.CtrGet())
       .Data(StrFromEvalTokens({
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
  tData.Finish(), StrPluraliseNum(cSockets->size(), "socket", "sockets"),
  cSockets->stConnected.load(),
  cSockets->qRXp.load(), cSockets->qRX.load(),
    StrToBytes(cSockets->qRX.load()),
  cSockets->qTXp.load(), cSockets->qTX.load(),
    StrToBytes(cSockets->qTX.load()));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'sockets' function
/* ========================================================================= */
{ "sockreset", 2, 2, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Get parameter and if user requested to close all connections? Close all
// the sockets and report how many we closed and return
const string &strId = aList[1];
if(strId == "*")
  return cConsole->AddLineExA(StrPluraliseNum(SocketReset(),
    "connection", "connections"), " closed.");
// Parse the specified id. Try to find the FD specified and return the class
const unsigned int uiId = StrToNum<unsigned int>(strId);
const auto sItem{ SocketFind(uiId) };
// Report if socket was not matched, or the result of disconnection.
if(sItem == cSockets->cend())
  cConsole->AddLine("The specified socket id was not matched!");
else cConsole->AddLineEx("Connection $ $ closed.",
  uiId, ((*sItem)->SendDisconnect() ? "has been" : "is already"));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'sockreset' function
/* ========================================================================= */
{ "sources", 1, 1, CF_AUDIO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get reference to sources collector class and lock it so it's not changed
const LockGuard lgSourcesSync{ cSources->CollectorGetMutex() };
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("SID").Header("FLAG").Header("S")
     .Header("T").Header("BQ").Header("BP").Header("BI").Header("GAIN")
     .Header("GAINMIN").Header("GAINMAX").Header("PITCH").Header("REFDIST")
     .Header("MAXDIST", false).Reserve(cSources->size());
// Walk through sources
for(const Source*const sCptr : *cSources)
{ // Get pointer to class
  const Source &sCref = *sCptr;
  // Get source play state and type
  const ALenum alState = sCref.GetState();
  const ALuint uiType = sCref.GetType();
  // Add data to text table
  tData.DataN(sCref.CtrGet()).DataN(sCref.GetSource()).Data(StrFromEvalTokens({
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
  StrPluraliseNum(cSources->size(), "source.", "sources."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'sources' function
/* ========================================================================= */
{ "sqlcheck", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Don't continue if theres a transaction in progress
if(cSql->Active())
  return cConsole->AddLine("Sql transaction already active!");
// Compact the database and print the reason if failed
cConsole->AddLine(cSql->CheckIntegrity() ?
  "UDB check has passed!" : cSql->GetErrorStr());
// Reset sql transaction because it will leave arrays registered
cSql->Reset();
/* ------------------------------------------------------------------------- */
} },                                   // End of 'sqlcheck' function
/* ========================================================================= */
{ "sqldefrag", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Don't continue if theres a transaction in progress
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
const double dPC = -100.0 + UtilMakePercentage(qAfter, qBefore);
// Write summary
cConsole->AddLine(qChange ?
  // If udb did change?
  StrFormat("UDB changed $B ($; $$$%) to $B ($) from $B ($) in $.",
    qChange, StrToBytes(-qChange,0), setprecision(2), fixed, dPC, qAfter,
    StrToBytes(qAfter,0), qBefore, StrToBytes(qBefore,0),
    StrShortFromDuration(dT)) :
  // Udb did not change so show the result
  StrFormat("UDB unchanged at $B ($) in $.",
    qAfter, StrToBytes(qAfter,0), StrShortFromDuration(dT)));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'sqldefrag' function
/* ========================================================================= */
{ "sqlend", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Don't continue if theres a transaction in progress
if(!cSql->Active()) return cConsole->AddLine("Sql transaction not active!");
// End transaction
cConsole->AddLineEx("Sql transaction$ ended.",
  cSql->End() == SQLITE_OK ? cCommon->Blank() : " NOT");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'sqlend' function
/* ========================================================================= */
{ "sqlexec", 2, 0, CF_NOTHING, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Don't continue if theres a transaction in progress
if(cSql->Active())
  return cConsole->AddLine("Sql transaction already active!");
// Execute the string and catch exceptions
if(cSql->ExecuteAndSuccess(StrImplode(aList, 1)))
{ // Get records and if we did not have any?
  const Sql::Result &vData = cSql->GetRecords();
  if(vData.empty())
  { // If we should show the rows affected. This is sloppy but sqllite
    // doesn't support resetting sqlite3_changes result yet :(
    const string &strFirst = StrToLowCaseRef(UtilToNonConst(aList[1]));
    // Show rows affected if we have them
    if(strFirst == "insert" || strFirst == "update" || strFirst == "delete")
      cConsole->AddLineEx("$ affected in $.",
        StrPluraliseNum(cSql->Affected(), "record", "records"),
          cSql->TimeStr());
    // Show time
    else if(cSql->Time() > 0)
      cConsole->AddLineEx("Query time was $.", cSql->TimeStr());
  } // Results were returned?
  else
  { // Data for output
    Statistic tData;
    tData.Header("R#").Header("C#").Header("SIZE").Header("KEY", false)
         .Header("T").Header("VALUE", false)
         .Reserve(vData.size() * vData.front().size());
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
            // StrFormat and store in spreadsheet
            tData.Data("I")
                 .Data(StrFormat("$ (0x$$)", qwVal, hex, qwVal));
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
          case SQLITE_NULL: tData.Data("N").Data("<Null>"); break;
          // Unknown type (impossible)
          default: tData.Data("?").Data(StrFormat("<Type $[0x$$]>",
                     mbOut.iT, hex, mbOut.iT));
                   break;
        }
      } // Increase record number
      stRecordId++;
    } // Show number of records matched and rows affected
    cConsole->AddLineExA(tData.Finish(), StrFormat("$ matched in $.",
      StrPluraliseNum(stRecordId, "record", "records"), cSql->TimeStr()));
  }
} // Error occured?
else cConsole->AddLineEx("Query took $ with $<$>: $!", cSql->TimeStr(),
  cSql->GetErrorAsIdString(), cSql->GetError(), cSql->GetErrorStr());
// Reset sql transaction because it will leave arrays registered
cSql->Reset();
/* ------------------------------------------------------------------------- */
} },                                   // End of 'sqlexec' function
/* ========================================================================= */
{ "stopall", 1, 1, CF_AUDIO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Tell audio to stop all sounds from playing
cAudio->Stop();
// Log event count
cConsole->AddLine("Stopping all sounds from playing.");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'stopall' function
/* ========================================================================= */
{ "streams", 1, 1, CF_AUDIO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Synchronise the streams collector list
const LockGuard lgStreamsSync{ cStreams->CollectorGetMutex() };
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("L").Header("LENGTH").Header("TIME").Header()
     .Header("SAMPLE").Header().Header("LOOP").Header("VOL").Header("C")
     .Header("RATE").Header("TYPE").Header("BRU").Header("BRN").Header("BRL")
     .Header("BRW").Header("NAME", false).Reserve(cStreams->size());
// Walk through sources
for(const Stream*const sCptr : *cStreams)
{ // Get pointer to class
  Stream &sCref = *const_cast<Stream*>(sCptr);
  // Add stream data to table
  tData.DataN(sCref.CtrGet()).Data(StrFromBoolYN(sCref.LuaRefIsSet()))
       .DataN(sCref.GetOggBytes()).DataN(sCref.GetElapsedSafe(), 1)
       .DataN(sCref.GetDurationSafe(), 1).DataN(sCref.GetPositionSafe())
       .DataN(sCref.GetSamplesSafe())
       .Data(sCref.GetLoop() == -1 ? "INF" : StrFromNum(sCref.GetLoop()))
       .DataN(sCref.GetVolume(), 2).DataN(sCref.GetChannels())
       .DataN(sCref.GetRate()).Data(sCref.GetFormatName())
       .DataN(sCref.GetBitRateUpper()).DataN(sCref.GetBitRateNominal())
       .DataN(sCref.GetBitRateLower()).DataN(sCref.GetBitRateWindow())
       .Data(sCref.IdentGet());
} // Show stream classes count
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cStreams->size(), "stream.", "streams."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'streams' function
/* ========================================================================= */
{ "system", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Print OS stats
cConsole->AddLineEx("$-bit $ version $.$ build $ locale $.",
  cSystem->OSBits(), cSystem->OSName(), cSystem->OSMajor(), cSystem->OSMinor(),
  cSystem->OSBuild(), cSystem->OSLocale());
/* ------------------------------------------------------------------------- */
} },                                   // End of 'system' function
/* ========================================================================= */
{ "textures", 1, 1, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
const function ShowTextureInfo{ [](Statistic &tData, const Texture &tC)
{ // Add data to table
  tData.DataN(tC.CtrGet())
       .Data(StrFromBoolYN(tC.FlagIsSet(TF_DELETE)))
       .DataN(tC.GetSubCount()).DataN(tC.GetMipmaps()).DataN(tC.GetTexFilter())
       .DataN(tC.GetTileCount()).DataN(tC.GetTileWidth())
       .DataN(tC.GetTileHeight()).DataN(tC.GetPaddingWidth(), 0)
       .DataN(tC.GetPaddingHeight(), 0).Data(tC.IdentGet());
} };
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("D").Header("SC").Header("MM").Header("TF")
     .Header("TLT").Header("TSSX").Header("TSSY").Header("TSPX")
     .Header("TSPY").Header("IDENTIFIER", false)
     .Reserve(2 + cFonts->size() + cTextures->size());
// Include console texture and font
ShowTextureInfo(tData, cConsole->GetTextureRef());
ShowTextureInfo(tData, cConsole->GetFontRef());
// Walk through font textures and textures classes
for(const Font*const fCptr : *cFonts) ShowTextureInfo(tData, *fCptr);
for(const Texture*const tCptr : *cTextures) ShowTextureInfo(tData, *tCptr);
// Log texture counts (+ 2 for the console texture and font)
cConsole->AddLineExA(tData.Finish(), StrPluraliseNum(cTextures->size() +
  cFonts->size() + 2, "texture.", "textures."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'textures' function
/* ========================================================================= */
{ "threads", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Make a table to automatically format our data neatly
Statistic tData;
tData.Header("ID").Header("FLAG").Header("P").Header("EC").Header("STTIME")
     .Header("ENTIME").Header("UPTIME").Header("IDENTIFIER", false)
     .Reserve(cThreads->size());
// For each thread pointer
for(const Thread*const tCptr : *cThreads)
{ // Get reference to thread pointer
  const Thread &tCref = *tCptr;
  // Store thread information
  tData.DataN(tCref.CtrGet()).Data(StrFromEvalTokens({
    { tCref.ThreadHaveCallback(), 'C' },
    { tCref.ThreadIsParamSet(),   'P' }, { tCref.ThreadIsRunning(),    'R' },
    { tCref.ThreadShouldExit(),   'T' }, { tCref.ThreadIsException(),  'E' },
    { tCref.ThreadIsJoinable(),   'J' }, { tCref.ThreadIsExited(),     'X' },
  })).DataN(tCref.ThreadGetPerf()).DataN(tCref.ThreadGetExitCode())
     .Data(StrShortFromDuration(cLog->
       CCDeltaToClampedDouble(tCref.ThreadGetStartTime())))
     .Data(StrShortFromDuration(cLog->
       CCDeltaToClampedDouble(tCref.ThreadGetEndTime())))
     .Data(StrShortFromDuration(tCref.ThreadIsRunning()
        ? cmHiRes.TimePointToClampedDouble(tCref.ThreadGetStartTime())
        : ClockTimePointRangeToClampedDouble(tCref.ThreadGetEndTime(),
            tCref.ThreadGetStartTime())))
     .Data(tCref.IdentGet());
} // Show number of threads
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cThreads->size(), "thread (", "threads ("),
  ThreadGetRunning(), " running).");
/* ------------------------------------------------------------------------- */
} },                                   // End of 'threads' function
/* ========================================================================= */
{ "time", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
// Timestamp to use
const char*const cpFmt = "%a %b %d %H:%M:%S %Y %z";
// Get current time
const StdTimeT tTime = cmSys.GetTimeS();
// Check if timezone's are different, if they are the same? Show local time
if(StrFromTimeTT(tTime, "%z") == StrFromTimeTTUTC(tTime, "%z"))
  cConsole->AddLineEx("Local and universal time is $.",
    StrFromTimeTTUTC(tTime, cpFmt));
// Write current local and universal system time
else cConsole->AddLineEx("Local time is $.\nUniversal time is $.",
    StrFromTimeTT(tTime, cpFmt), StrFromTimeTTUTC(tTime, cpFmt));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'time' function
/* ========================================================================= */
{ "version", 1, 1, CF_NOTHING, [](const Args &){
/* ------------------------------------------------------------------------- */
cConsole->PrintVersion();
/* ------------------------------------------------------------------------- */
} },                                   // End of 'version' function
/* ========================================================================= */
{ "videos", 1, 1, CF_AUDIOVIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
// Get reference to videos collector class and lock it so it's not changed
const LockGuard lgVideosSync{ cVideos->CollectorGetMutex() };
// Text table class to help us write neat output
Statistic tData;
tData.Header("ID").Header("FLAGS").Header("PCMF").Header("P").Header("C")
     .Header("FRMW").Header("FRMH").Header("FPS").Header("TIME").Header("FD")
     .Header("FR").Header("FL").Header("AVD").Header("LEN")
     .Header("NAME", false).Reserve(cVideos->size());;
// Walk through textures classes
for(const Video*const vCptr : *cVideos)
{ // Get pointer to class
  const Video &vCref = *vCptr;
  // and write data
  tData.DataN(vCref.CtrGet())
    .Data(StrFromEvalTokens({
      { vCref.LuaRefIsSet(),       'L' }, { vCref.HaveAudio(),         'A' },
      { vCref.FlagIsSet(FL_FILTER),'F' }, { vCref.GetKeyed(),          'K' },
      { vCref.FlagIsSet(FL_PLAY),  'P' }, { vCref.FlagIsSet(FL_STOP),  'S' },
      { vCref.FlagIsSet(FL_THEORA),'T' }, { vCref.FlagIsSet(FL_VORBIS),'V' },
      { vCref.FlagIsSet(FL_VDINIT),'1' }, { vCref.FlagIsSet(FL_VBINIT),'2' },
      { vCref.FlagIsSet(FL_OSINIT),'3' }, { vCref.FlagIsSet(FL_TCINIT),'4' },
      { vCref.FlagIsSet(FL_TIINIT),'5' }, { vCref.FlagIsSet(FL_VCINIT),'6' },
      { vCref.FlagIsSet(FL_VIINIT),'7' },
    }))
   .Data(vCref.GetFormatAsIdentifier()).DataH(vCref.GetPixelFormat(),4)
   .DataN(vCref.GetColourSpace())      .DataN(vCref.GetFrameWidth())
   .DataN(vCref.GetFrameHeight())      .DataN(vCref.GetFPS(), 2)
   .DataN(vCref.GetVideoTime(), 3)     .DataN(vCref.GetFrame())
   .DataN(vCref.GetFrames())           .DataN(vCref.GetFramesSkipped())
   .DataN(vCref.HaveAudio() ? vCref.GetDrift() : 0, 2)
   .DataN(vCref.GetLength())           .Data(vCref.IdentGet());
} // Log counts
cConsole->AddLineExA(tData.Finish(),
  StrPluraliseNum(cVideos->size(), "video.", "videos."));
/* ------------------------------------------------------------------------- */
} },                                   // End of 'videos' function
/* ========================================================================= */
{ "vmlist", 1, 2, CF_VIDEO, [](const Args &aList){
/* ------------------------------------------------------------------------- */
// Monitor number and return if invalid
const size_t stMonitorSelected =
  aList.size() == 1 ? 0 : StrToNum<size_t>(aList[1]);
if(stMonitorSelected >= cDisplay->GetMonitorsCount())
  return cConsole->AddLineEx("Invalid monitor $ specified!",
    stMonitorSelected);
// Header (to be printed twice)
Statistic tData;
tData.Header("VM#").Header("A").Header("HORI").Header("VERT")
     .Header("CD").Header("HTZ").Header("RD").Header("GD").Header("BD")
     .Header("RATIO ", false);
// Get selected monitor and add extra headers if more than one mode
const GlFWMonitor &mItem = cDisplay->GetMonitors()[stMonitorSelected];
if(mItem.size() > 1)
  tData.DupeHeader().Reserve(cDisplay->GetMonitorsCount() / 2);
else tData.Reserve(1);
// Enumerate video modes
for(const GlFWRes &rItem : mItem)
{ // Print resolution
  tData.DataN(rItem.Index()).Data(StrFromBoolYN(&rItem == mItem.Primary()))
       .DataN(rItem.Width()).DataN(rItem.Height()).DataN(rItem.Depth())
       .DataN(rItem.Refresh()).DataN(rItem.Red()).DataN(rItem.Green())
       .DataN(rItem.Blue()).Data(StrFromRatio(rItem.Width(), rItem.Height()));
} // Print number of video modes
cConsole->AddLineEx("$$ supported on monitor #$ ($).", tData.Finish(),
  StrPluraliseNum(mItem.size(), "mode", "modes"), stMonitorSelected,
  mItem.Name());
/* ------------------------------------------------------------------------- */
} },                                   // End of 'vmlist' function
/* ========================================================================= */
{ "vreset", 1, 1, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
cEvtMain->Add(EMC_QUIT_THREAD);
/* ------------------------------------------------------------------------- */
} },                                   // End of 'vreset' function
/* ========================================================================= */
{ "wreset", 1, 1, CF_VIDEO, [](const Args &){
/* ------------------------------------------------------------------------- */
cDisplay->RequestReposition();
/* ------------------------------------------------------------------------- */
} },                                   // End of 'wreset' function
/* ========================================================================= */
} };                                   // End of console commands list
/* == EoF =========================================================== EoF == */
