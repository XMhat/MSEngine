/* == CVARLIB.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module is parsed at the main procedure in 'core.cpp' and       ## */
/* ## defines the default cvars and their callbacks. Make sure to use the ## */
/* ## helper CB(STR) macros to help define your callbacks. If you add,    ## */
/* ## remove or change the order of these cvars, you must update the      ## */
/* ## 'CVarEnums' in 'cvardef.hpp' to match the order in this list. This  ## */
/* ## This file is also parsed by the MS-Engine project management        ## */
/* ## utility to help create html documentation. New cvar descriptions    ## */
/* ## start with '// !' with the cvar name and continues on each          ## */
/* ## subsequent line with '// ?' to describe the cvar.                   ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == Built-in CVar definition struct ====================================== */
const ItemStaticList cvEngList{ {      // Default cvars (from cvars.hpp)
/* -- Use this when cvar is an integer ------------------------------------- */
#define CB(f,t) [](Item &, const string &strV)->CVarReturn\
  { return f(ToNumber<t>(strV)); }
/* -- Use this when cvar is a string (NoOp for no callback needed) --------- */
#define CBSTR(f) [](Item &ciD, const string &strV)->CVarReturn\
  { return f(strV, ciD.GetModifyableValue()); }
/* == Core cvars (don't modify order) ====================================== */
// ! LOG_LEVEL
// ? Specifies the logging level...
// ? 0 (LH_DISABLED) = Nothing. Lua log function can still use this.
// ? 1 (LH_ERROR)    = For reporting only errors (which halt execution).
// ? 2 (LH_WARNING)  = For reporting only warnings (recoverable errors).
// ? 3 (LH_INFO)     = For reporting useful important messages.
// ? 4 (LH_DEBUG)    = For reporitng other information (debugging).
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "log_level",
/* ------------------------------------------------------------------------- */
#if defined(ALPHA)                     // Alpha (Debug) version?
  "4",                                 // Default of DEBUG for log level
#elif defined(BETA)                    // Beta (Developer) version?
  "3",                                 // Default of INFO for log level
#else                                  // Release (Public) version?
  "2",                                 // Default of WARNING for log level
#endif                                 // Release type check
  /* ----------------------------------------------------------------------- */
  CB(cLog->SetLevel, unsigned int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! APP_CMDLINE
// ? Shows the commandline sent to the application it cannot be changed at all.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_cmdline", cCommon->Blank(),
  CBSTR(cCore->CoreParseCmdLine), CONFIDENTIAL|TSTRING|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! AST_LZMABUFFER
// ? Specifies the decompression buffer size (in bytes) for the lzma api. The
// ? default value is 256 kilobytes.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "ast_lzmabuffer", "262144",
  CB(ArchiveSetBufferSize, size_t), TUINTEGER|CPOW2|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! AST_PIPEBUFFER
// ? Specifies the size of the pipe buffer.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "ast_pipebuffer", "4096",
  CB(AssetSetPipeBufferSize, size_t), TUINTEGER|CPOW2|PANY },
/* ------------------------------------------------------------------------- */
// ! AST_FSOVERRIDE
// ? Specifies whether the engine is allowed to override internal files from
// ? archives with files. This is makes it easier to update and mod resource
// ? if true. It is by default disabled on release builds and enabled on
// ? all other builds.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "ast_fsoverride",
/* ------------------------------------------------------------------------- */
#if defined(RELEASE)                   // Default disabled in release builds
  "0",
#else                                  // Default enabled in other builds
  "1",
#endif                                 // Build type check
  /* ----------------------------------------------------------------------- */
  CB(AssetSetFSOverride, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! AST_EXEBUNDLE
// ? A boolean to specify if the executable file should be checked for a
// ? 7-zip archive and use that to load guest assets. This is only supported
// ? on Windows architectures right now and ignored on others.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "ast_exebundle", "1",
  CB(ArchiveInitExe, bool), TBOOLEAN|PBOOT },
/* ------------------------------------------------------------------------- */
// ! APP_BASEDIR
// ? Specifies the base directory of where the executable was started from. It
// ? is only readable by the end-user and the host, but not the guest.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_basedir", cCommon->Blank(),
  CBSTR(cSystem->SetWorkDir), CONFIDENTIAL|TSTRING|CTRUSTEDFN|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! AST_BUNDLES
// ? Specifies the [.ext]ension to use as 7-zip archives. These filenames will
// ? be checked for at startup and automatically loaded by the engine.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "ast_bundles", "." ARCHIVE_EXTENSION,
  CBSTR(ArchiveInit), TSTRING|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! APP_CONFIG
// ? Loads the configuration file with the specified extension (usually 'app').
// ? The suffix extension of '.cfg' is always assumed for protection so the
// ? guest need never specify it. This variable can only be set at the
// ? command-line.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_config", DEFAULT_CONFIGURATION,
  CBSTR(cCVars->ExecuteAppConfig), TSTRING|CFILENAME|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! APP_AUTHOR
// ? Specifies the author of the guest application. This is used for display
// ? purposes only and can be requested by the guest and only set in the
// ? app.cfg file.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_author", "Anonymous", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_SHORTNAME
// ? Specifies a short title name for the guest application. It is purely for
// ? display purposes only to the end-user. It can be requested by the guest
// ? and only set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_shortname", "Untitled", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_HOMEDIR
// ? Species the users home directory where files are written to if they cannot
// ? be written to the working directory. It is only readable by the end-user
// ? and the host, but not the guest.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_homedir", cCommon->Blank(),
  CBSTR(cCore->CoreSetHomeDir), CONFIDENTIAL|TSTRING|CTRUSTEDFN|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! SQL_DB
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_db", cCommon->Blank(),
  CBSTR(cSql->UdbFileModified), CONFIDENTIAL|TSTRING|CTRUSTEDFN|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! SQL_ERASEEMPTY
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_eraseempty", "1",
  CB(cSql->DeleteEmptyDBModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_TEMPSTORE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_tempstore", "MEMORY",
  CBSTR(cSql->TempStoreModified), TSTRING|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_SYNCHRONOUS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_synchronous", "0",
  CB(cSql->SynchronousModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_JOURNALMODE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_journalmode", "0",
  CB(cSql->JournalModeModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_AUTOVACUUM
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_autovacuum", "1",
  CB(cSql->AutoVacuumModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_FOREIGNKEYS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_foreignkeys", "1",
  CB(cSql->ForeignKeysModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_INCVACUUM
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_incvacuum", "0",
  CB(cSql->IncVacuumModified, uint64_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_DEFAULTS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_defaults", "0",
  CB(cCVars->SetDefaults, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_LOADCONFIG
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "sql_loadconfig", "1",
  CB(cCVars->LoadSettings, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_GUIMODE
// ? Specifies how the host wants to be run...
// ? 0 (GM_TEXT_NOAUDIO) = Botmode with no audio.
// ? 1 (GM_TEXT_AUDIO)   = Botmode with audio.
// ? 2 (GM_GRAPHICS)     = Interactive mode.
// ? Bot modes will show a terminal console window instead of an OpenGL window
// ? when the gpu is not needed by the guest. In interactive mode, you must
// ? use Fbo.Draw() in Lua to allow the engine draw the frame at the end of the
// ? frame which is useful for preserving cpu/gpu cycles when the screen does.
// ? not need to be redrawn. Certain commands, cvars, lua api functions are
// ? hidden and shown depending on this value. It can only be set in the guest
// ? app.cfg file.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_guimode", "0",
  CB(cSystem->SetGUIMode, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LOG_LINES
// ? Specifies the maximum number of lines to keep in the backlog. The log
// ? lines are always empty when logging to a file.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "log_lines", "10000",
  CB(cLog->LogLinesModified, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! LOG_FILE
// ? Specifies a file to log internal engine messages to. It is used to help
// ? debugging. Leave as blank for no log file. It can be set from command-line
// ? parameters, the app.cfg file and the user.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "log_file", cCommon->Blank(),
  CBSTR(cLog->LogFileModified), TSTRING|CFILENAME|MTRIM|PBOOT|PUSR },
/* ------------------------------------------------------------------------- */
// ! APP_LONGNAME
// ? Specifies a long title name for the guest application. It is uses as the
// ? window title and for display purposes only to the end-user. It can be
// ? requested by the guest and only set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_longname", "Untitled", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_CLEARMUTEX
// ? Asks the system to delete the global mutex in Linux and MacOS so the
// ? engine can startup. You only need to do this if the engine crashes. On
// ? Windows the mutex is freed on process termination so there is never any
// ? need to use this on Windows.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_clearmutex",
  /* ----------------------------------------------------------------------- */
#if !defined(WINDOWS) && defined(ALPHA)
  "1",
#else
  "0",
#endif
  /* ----------------------------------------------------------------------- */
  CB(cCore->CoreClearMutex, bool), TBOOLEAN|PBOOT },
/* ------------------------------------------------------------------------- */
// ! ERR_INSTANCE
// ? Tries to activate an existing window of the same name if another instance
// ? is running. On Windows, mutexes are always cleaned up properly even in a
// ? crash but not on Linux and OSX. So just simply remove the app name from
// ? /dev/shm and the engine should run again.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "err_instance", "1",
  CB(cCore->CoreSetOneInstance, bool), TBOOLEAN|PSYSTEM },
/* == Object cvars ========================================================= */
// ! OBJ_CLIPMAX
// ? Specifies the maximum number of clipboard objects allowed to be registered
// ? by the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_clipmax", "100",
  CB(cClips->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_CMDMAX
// ? Specifies the maximum number of console commands allowed to be registered
// ? by the engine. This includes the internal default commands. An exception
// ? is generated if more cvars than this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_cmdmax", "1000",
  CB(cConsole->MaxCountModified, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_CVARMAX
// ? Specifies the maximum number of cvars allowed to be registered by the
// ? engine. This includes the internal default cvars. An exception is
// ? generated if more cvars than this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_cvarmax", "1000",
  CB(cCVars->MaxCountModified, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_CVARIMAX
// ? Specifies the maximum number of initial cvars allowed to be registered by
// ? the engine. Initial cvars are in a temporary state, they are used when
// ? the corresponding cvar is registered, and passed back to this temporary
// ? state when the cvar is unregistered. The temporary state of all initial
// ? cvars are then commited to database at exit and on-demand. An exception is
// ? generated if more initial cvars than this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_cvarimax", "1000",
  CB(cCVars->MaxICountModified, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_ARCHIVEMAX
// ? Specifies the maximum number of archive objects allowed to be registered
// ? by the engine. This includes the archives that are loaded at startup
// ? An exception is generated if more archives than this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_archivemax", "1000",
  CB(cArchives->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_ASSETMAX
// ? Specifies the maximum number of asset objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_assetmax", "1000",
  CB(cAssets->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_BINMAX
// ? Specifies the maximum number of bin objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_binmax", "1000",
  CB(cBins->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_CURSORMAX
// ? Specifies the maximum number of cursor objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "obj_cursormax", "1000",
  CB(cCursors->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FBOMAX
// ? Specifies the maximum number of framebuffer objects allowed to be
// ? registered by the engine. An exception is generated if more cvars than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "obj_fbomax", "1000",
  CB(cFbos->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FONTMAX
// ? Specifies the maximum number of font objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "obj_fontmax", "1000",
  CB(cFonts->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FILEMAX
// ? Specifies the maximum number of file objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_filemax", "1000",
  CB(cFiles->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FTFMAX
// ? Specifies the maximum number of freetype objects allowed to be registered
// ? by the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_ftfmax", "1000",
  CB(cFtfs->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FUNCMAX
// ? Specifies the maximum number of lua referenced function objects allowed to
// ? be registered by the engine. An exception is generated if more cvars than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_funcmax", "1000",
  CB(cLuaFuncs->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_IMGMAX
// ? Specifies the maximum number of image objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_imgmax", "1000",
  CB(cImages->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_IMGFMTMAX
// ? Specifies the maximum number of image format objects allowed to be
// ? registered by the engine. An exception is generated if more cvars than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_imgfmtmax", "1000",
  CB(cImageFmts->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_JSONMAX
// ? Specifies the maximum number of json objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_jsonmax", "1000",
  CB(cJsons->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_MASKMAX
// ? Specifies the maximum number of mask objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_maskmax", "1000",
  CB(cMasks->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_PCMMAX
// ? Specifies the maximum number of pcm objects allowed to be registered by
// ? the engine. An exception is generated if more objects than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_pcmmax", "1000",
  CB(cPcms->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_PCMFMTMAX
// ? Specifies the maximum number of pcm format objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_pcmfmtmax", "1000",
  CB(cPcmFmts->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_SAMPLEMAX
// ? Specifies the maximum number of sample objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "obj_samplemax", "1000",
  CB(cSamples->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_SHADERMAX
// ? Specifies the maximum number of shader objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "obj_shadermax", "1000",
  CB(cShaders->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_SOCKETMAX
// ? Specifies the maximum number of socket objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_socketmax", "1000",
  CB(cSockets->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_SOURCEMAX
// ? Specifies the maximum number of source objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "obj_sourcemax", "1000",
  CB(cSources->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_STATMAX
// ? Specifies the maximum number of stat objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_statmax", "1000",
  CB(cStats->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_STREAMMAX
// ? Specifies the maximum number of stream objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "obj_streammax", "1000",
  CB(cStreams->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_TEXTUREMAX
// ? Specifies the maximum number of texture objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "obj_texturemax", "1000",
  CB(cTextures->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_THREADMAX
// ? Specifies the maximum number of thread objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "obj_threadmax", "1000",
  CB(cThreads->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_VIDEOMAX
// ? Specifies the maximum number of video objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "obj_videomax", "1000",
  CB(cVideos->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* == Base cvars =========================================================== */
// ! APP_DESCRIPTION
// ? Specifies a description for the guest application. It is purely for
// ? display purposes only to the end-user. It can be requested by the guest
// ? and only set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_description", "Undescribed", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_VERSION
// ? Specifies a version for the guest application. It is purely for display
// ? purposes only to the end-user. It can be requested by the guest and only
// ? set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_version", "0.0", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_ICON
// ? Specifies a large icon for use with the application and window. It can be
// ? any format that is supported with the Image.* loading functions. It can
// ? also be changed dynamically by Lua. Keep to empty for default icon.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_icon", cCommon->Blank(),
  CBSTR(cDisplay->SetIcon), TSTRING|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_COPYRIGHT
// ? Specifies any copyright information for the guest application. It is
// ? purely for display purposes only to the end-user. It can be requested by
// ? the guest and only set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_copyright", "Public Domain", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_WEBSITE
// ? Specifies a website for the guest application. It is purely for display
// ? purposes only to the end-user. It can be requested by the guest and only
// ? set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_website", "about:blank", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_TICKRATE
// ? Specifies the base tick rate of the guest application in ticks when using
// ? the timer accumulator (GuImode=GM_INT_ACCU||GM_INT_MF_ACCU). The engine
// ? locks to this tick rate regardless of lag. It can also be changed
// ? dynamically with Lua.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "app_tickrate", "60",
  CB(cTimer->TimerTickRateModified, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_DELAY
// ? Specifies an artificial delay to force for bot mode in milliseconds. This
// ? is ignored on interactive mode because a one millisecond delay is forced
// ? for every frame under the target rate.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_delay", "1",
  CB(cTimer->TimerSetDelay, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! APP_TITLE
// ? Sets a custom title for the window. This can only be changed at the
// ? command-line or the application configuration file and not saved to the
// ? persistence database. It only applies to the Win32 terminal window (not
// ? MacOS nor Linux terminal windows) and to any OS desktop windows. If not
// ? specified, the default is used in the format of 'L V (T)' where 'L' is the
// ? value of 'app_longname', where 'V' is the value of 'app_version' and 'T'
// ? is the target executable architechture.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "app_title", cCommon->Blank(),
  CBSTR(cCore->CoreTitleModified), TSTRING|MTRIM|PSYSTEM },
/* == Error cvars ========================================================== */
// ! ERR_ADMIN
// ? Throws an error if the user is running the engine with elevated
// ? privileges. This can be one of three values...
// ? 0 = The engine is allowed to be run with elevated privileges.
// ? 1 = The engine is NOT allowed to be run with elevated privileges.
// ? 2 = Same as 1 but ALLOWS when OS's has admin as default (i.e. XP!).
// ? The default value is 2.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "err_admin", "2",
  CB(cSystem->CheckAdminModified, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! ERR_CHECKSUM
// ? Throws an error if there is an executable checksum mismatch. This only
// ? applies on Windows executables only as Linux executable does not have a
// ? checksum and MacOS uses code signing externally. The default value is 1
// ? on release executable else 0 on beta executable.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "err_checksum",
  /* ----------------------------------------------------------------------- */
#if defined(RELEASE)
  "1",
#else
  "0",
#endif
  /* ----------------------------------------------------------------------- */
  CB(cSystem->CheckChecksumModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! ERR_DEBUGGER
// ? Throws an error if a debugger is running at start-up. The default value
// ? is 1 on release executable else 0 on beta executable.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "err_debugger",
  /* ----------------------------------------------------------------------- */
#if defined(RELEASE)
  "1",
#else
  "0",
#endif
  /* ----------------------------------------------------------------------- */
  CB(cSystem->CheckDebuggerDetected, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! ERR_LUAMODE
// ? Sets how to handle a LUA script error to one of these values...
// ? 0 (LEM_IGNORE)   = Ignore errors and try to continue.
// ? 1 (LEM_RESET)    = Automatically reset on error.
// ? 2 (LEM_SHOW)     = Open console and show error.
// ? 3 (LEM_CRITICAL) = Terminate engine with error.
// ? The default value is 3 for release executable and 2 for beta executable.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "err_luamode",
  /* ----------------------------------------------------------------------- */
#if defined(RELEASE)
  "3",
#else
  "2",
#endif
  /* ----------------------------------------------------------------------- */
  CB(cCore->CoreErrorBehaviourModified, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! ERR_LMRESETLIMIT
// ? When ERR_LUAMODE is set to 1, this specifies the number of LUA script
// ? errors that are allowed before an error after this is treated as a
// ? critical error. The default value for release executable is 1000 and
// ? 10 for beta executable.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "err_lmresetlimit",
  /* ----------------------------------------------------------------------- */
#if defined(RELEASE)
  "1000",
#else
  "10",
#endif
  /* ----------------------------------------------------------------------- */
  CB(cCore->CoreSetResetLimit, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! ERR_MINVRAM
// ? The engine fails to run if the system does not have this amount of VRAM
// ? available.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "err_minvram", "0",
  CB(cOgl->SetMinVRAM, uint64_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! ERR_MINRAM
// ? The engine fails to run if the system does not have this amount of free
// ? memory available. If this value is zero then the check does not take
// ? place else the amount specified is allocated at startup, cleared, the
// ? speed of the clear is reported in the log then the memory is freed. The
// ? default value is zero.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "err_minram", "0",
  CB(cSystem->SetMinRAM, uint64_t), TUINTEGER|PSYSTEM },
/* == Lua cvars ============================================================ */
// ! LUA_TICKTIMEOUT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "lua_ticktimeout", "10",
  CB(cTimer->TimerSetTimeOut, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_TICKCHECK
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "lua_tickcheck", "1000000",
  CB(cLua->SetOpsInterval, int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_CACHE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "lua_cache", "1",
  CB(LuaCodeSetCache, unsigned int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! LUA_SIZESTACK
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "lua_sizestack", "1000",
  CB(cLua->SetStack, int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_GCPAUSE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "lua_gcpause", "200",
  CB(cLua->SetGCPause, int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_GCSTEPMUL
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "lua_gcstepmul", "100",
  CB(cLua->SetGCStep, int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_RANDOMSEED
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "lua_randomseed", "0",
  CB(cLua->SetSeed, lua_Integer), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_APIFLAGS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "lua_apiflags", "3",
  CB(cLua->SetFlags, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_SCRIPT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "lua_script", "main.lua", NoOp,
  TSTRING|CFILENAME|CNOTEMPTY|MTRIM|PSYSTEM },
/* == Audio cvars ========================================================== */
// ! AUD_DELAY
// ? Specifies an delay (in number of milliseconds) to suspend the audio
// ? thread each tick. Lower values use less CPU usage but may cause audio
// ? pops. The audio thread is in charge of managing Source classes and
// ? checking for discreprencies.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_delay", "10",
  CB(cAudio->SetAudThreadDelay, ALuint), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_VOL
// ? Specifies the global volume of all class types. 0.0 (mute) to
// ? 1.0 (maximum volume). Setting a value too high may cause artifacts on
// ? older OS audio API's.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_vol", "0.75",
  CB(cAudio->SetGlobalVolume, ALfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_INTERFACE
// ? Specifies the audio device number to use. -1 makes sure to use the default
// ? device, otherwise it is the index number of a specific audio device to
// ? use.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_interface", "-1", NoOp, TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_CHECK
// ? Specifies an interval (in number of milliseconds) of checking for audio
// ? device changes as OpenAL does not have an event interface for this. This
// ? check is done in the audio manager thread. If a change in the audio
// ? device list is detected then the audio is reset automatically, as if the
// ? 'areset' console command was used.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_check", "5000",
  CB(cAudio->SetAudCheckRate, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_NUMSOURCES
// ? Specifies the maximum number of sources to preallocate. 0 means sources
// ? are (de)allocated dynamically. Setting this value to non-zero may improve
// ? CPU usage but increases the chances of errors if too many sources are
// ? requested.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_numsources", "0",
  CB(SourceSetCount, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_SAMVOL
// ? Specifies the volume of Sample classes. 0.0 (mute) to
// ? 1.0 (maximum volume). Setting a value too high may cause artifacts on
// ? older OS audio API's.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_samvol", "0.75",
  CB(SetSampleVolume, ALfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_STRBUFCOUNT
// ? Specifies the number of buffers to use for Stream classes. We need
// ? multiple buffers to make streamed audio playback consistent. Four should
// ? always be enough.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_strbufcount", "4",
  CB(StreamSetBufferCount, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_STRBUFSIZ
// ? Specifies the size (in bytes) of each stream buffer for Stream classes.
// ? We need sufficiently large enough buffers to make streamed audio playback
// ? consistent.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_strbufsiz", "32768",
  CB(StreamSetBufferSize, size_t), TUINTEGERSAVE|CPOW2|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_STRVOL
// ? Specifies the volume of Stream classes. 0.0 (mute) to
// ? 1.0 (maximum volume). Setting a value too high may cause artifacts on
// ? older OS audio API's.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_strvol", "0.75",
  CB(StreamSetVolume, ALfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_FMVVOL
// ? Specifies the volume of Video classes. 0.0 (mute) to 1.0 (maximum volume).
// ? Setting a value too high may cause artifacts on older OS audio API's.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_fmvvol", "0.75",
  CB(VideoSetVolume, ALfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_HRTF
// ? Specifies whether to use HRTF dynamics on audio output. This could cause
// ? strange audio stereo quality issues so it is recommended to disable.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_AUDIO, GM_HIGHEST, "aud_hrtf", "0", NoOp, TUINTEGERSAVE|PANY },
/* == Console cvars ======================================================== */
// ! CON_KEYPRIMARY
// ? The primary GLFW console key virtual key code to use to toggle console
// ? visibility. It is set to '`' as default on a UK keyboard.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_keyprimary", "96",
  CB(cInput->SetConsoleKey1, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_KEYSECONDARY
// ? The secondary GLFW console key virtual key code to use to toggle console
// ? visibility. It is set to '+-' as default on a US keyboard.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_keysecondary", "161",
  CB(cInput->SetConsoleKey2, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_AUTOCOMPLETE
// ? Flags that specify what to auto complete when TAB key is pressed in the
// ? console...
// ? AC_NONE     (0x0): Autocompletion is disabled
// ? AC_COMMANDS (0x1): Autocomplete command names
// ? AC_CVARS    (0x2): Autocomplete cvar names
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "con_autocomplete", "3",
  CB(cConsole->SetAutoComplete, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_AUTOSCROLL
// ? A boolean that specifies whether to autoscroll the console to the last
// ? newly printed message.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "con_autoscroll", "1",
  CB(cConsole->SetAutoScroll, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_AUTOCOPYCVAR
// ? A boolean that specifies whether to automatically copy the variable
// ? name and value when a cvar name is typed in the console.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "con_autocopycvar", "1",
  CB(cConsole->SetAutoCopyCVar, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_HEIGHT
// ? Specifies the height of the console in OpenGL mode. 1 means the console
// ? is covering the whole screen, and 0 means the console is not showing.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_height", "0.5",
  CB(cConsole->SetHeight, GLfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_BLOUTPUT
// ? Specifies the maximum number of lines to keep in the console. Excess
// ? messages are discarded automatically to not fill up memory.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "con_bloutput", "10000",
  CB(cConsole->SetConsoleOutputLines, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_BLINPUT
// ? Number of lines to store in the input backlog. A successful command input
// ? automatically adds the last command typed into this list.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "con_blinput", "1000",
  CB(cConsole->SetConsoleInputLines, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_DISABLED
// ? Specifies wether the console is permanantly disabled or not.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_disabled",
  /* ----------------------------------------------------------------------- */
#if defined(RELEASE)
  "1",
#else
  "0",
#endif
  /* ----------------------------------------------------------------------- */
  CB(cConsole->CantDisableModified, bool), TBOOLEAN|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_CVSHOWFLAGS
// ? Flags specifying how to show CVar values in the console to protect
// ? exposure of sensetive information...
// ? SF_NONE         (0): Do not show cvars marked as private or protected.
// ? SF_CONFIDENTIAL (1): Show cvars marked as private.
// ? SF_PROTECTED    (2): Show cvars marked as protected.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "con_cvshowflags", "0",
  CB(cCVars->SetDisplayFlags, unsigned int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_BGCOLOUR
// ? Specifies the background colour of the console texture in the syntax of
// ? 0xAARRGGBB or an integral number.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_bgcolour", "2130706432",
  CB(cConsole->TextBackgroundColourModified, uint32_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_BGTEXTURE
// ? The texture file to load that will be used as the background for the
// ? console.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_bgtexture", cCommon->Blank(), NoOp,
  TSTRING|CFILENAME|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONT
// ? Specifies the filename of the FreeType compatible font to load as the
// ? console font file.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_font", "console.ttf", NoOp,
  TSTRING|CFILENAME|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTFLAGS
// ? Specifies manipulation of the loaded font. See the 'Char.Flags' for
// ? possible values
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontflags", "0",
  CB(cConsole->ConsoleFontFlagsModified, unsigned int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_FONTCOLOUR
// ? Specifies the default colour of the console text. See the Console.Colours
// ? lookup table for possible values. There are only 16 pre-defined colours
// ? to keep compatibility with the system text console.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontcolour", "15",
  CB(cConsole->TextForegroundColourModified, unsigned int),
  TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_FONTHEIGHT
// ? Specifies the height of the loaded console FreeType font.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontheight", "20",
  CB(cConsole->TextHeightModified, GLfloat), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTPADDING
// ? Specifies any extra padding to add to each FreeType font glyph to prevent
// ? pixels from other glyphs spilling into the render due to filtering.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontpadding", "0", NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTPCMIN
// ? Specifies the minimum character code to start precaching from
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontpcmin", "32", NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTPCMAX
// ? Specifies the maximum character code to end precaching at
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontpcmax", "256", NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTSCALE
// ? Specifies the scale adjust of the font. A value not equal to one will
// ? cause interpolation to occur so filtering is advised.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontscale", "1",
  CB(cConsole->TextScaleModified, GLfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_FONTSPACING
// ? Specifies the amount of padding to add after each cahracter rendered.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontspacing", "0",
  CB(cConsole->TextLetterSpacingModified, GLfloat), TFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTLSPACING
// ? Specifies the amount of padding to add below each line of text rendered.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontlspacing", "0",
  CB(cConsole->TextLineSpacingModified, GLfloat), TFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTWIDTH
// ? Specifies the width of the loaded console FreeType font.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fontwidth", "20",
  CB(cConsole->TextWidthModified, GLfloat), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTTEXSIZE
// ? Specifies the initial size of the texture canvas in pixels. Do not set
// ? this more than 1024 pixels unless you know for a fact the GPU will support
// ? this value. An exception is thrown if not. Zero means start with enough
// ? size for one character.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "con_fonttexsize", "0", NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_INPUTMAX
// ? Specifies the maximum number of characters that are allowed to be typed
// ? into the console input buffer.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "con_inputmax", "1000",
  CB(cConsole->SetMaxConLineChars, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_PAGELINES
// ? The number of lines that are scrolled when pressing CONTROL and PAGEUP or
// ? PAGEDOWN keys.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "con_pagelines", "10",
  CB(cConsole->SetPageMoveCount, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_TMCCOLS
// ? In bot mode, this is the maximum number of columns to draw. The larger
// ? this value is, the more text that can be displayed at once but will
// ? increase CPU usage from the operating system.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_TEXT_MAX, "con_tmccols", "80",
  CB(cSystem->ColsModified, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_TMCROWS
// ? In bot mode, this is the maximum number of rows to draw. The larger
// ? this value is, the more text that can be displayed at once but will
// ? increase CPU usage from the operating system.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_TEXT_MAX, "con_tmcrows", "32",
  CB(cSystem->RowsModified, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_TMCREFRESH
// ? The interval in milliseconds where the status bar and title bar are
// ? refreshed. The title bar contains basic operating statistics and the
// ? status bar is configurable by LUA and contains the input bar when text
// ? is being typed.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_TEXT_MAX, "con_tmcrefresh", "1000",
  CB(cConsole->RefreshModified, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_TMCNOCLOSE
// ? Disables the X button and close in the application context menu. This does
// ? not prevent CONTROL+C/BREAK keys.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_TEXT_MAX, "con_tmcnoclose", "1", NoOp,
  TBOOLEAN|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_TMCTFORMAT
// ? The format of the time in the titlebar (see strftime() for details).
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_TEXT_MAX, "con_tmctformat", "%y-%m-%d %T",
  CBSTR(cConsole->SetTimeFormat), TSTRINGSAVE|MTRIM|CNOTEMPTY|PSYSTEM|PUSR },
/* == Fmv cvars ============================================================ */
// ! FMV_IOBUFFER
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "fmv_iobuffer", "65536",
  CB(VideoSetBufferSize, long), TUINTEGERSAVE|CPOW2|PANY },
/* == Input cvars ========================================================== */
// ! INP_JOYDEFFDZ
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "inp_joydeffdz", "0.25",
  CB(cInput->SetDefaultJoyFwdDZ, float), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_JOYDEFRDZ
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "inp_joydefrdz", "0.25",
  CB(cInput->SetDefaultJoyRevDZ, float), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_JOYSTICK
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "inp_joystick", "-1",
  CB(cInput->SetJoystickEnabled, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_FSTOGGLER
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "inp_fstoggler", "1",
  CB(cInput->SetFSTogglerEnabled, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_RAWMOUSE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "inp_rawmouse", "1",
  CB(cInput->SetRawMouseEnabled, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_STICKYKEY
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "inp_stickykey", "1",
  CB(cInput->SetStickyKeyEnabled, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_STICKYMOUSE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "inp_stickymouse", "1",
  CB(cInput->SetStickyMouseEnabled, bool), TBOOLEANSAVE|PANY },
/* == Network cvars ======================================================== */
// ! NET_CBPFLAG1
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_cbpflag1", "0",
  CB(cSockets->CertsSetBypassFlags1, uint64_t), TUINTEGER|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_CBPFLAG2
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_cbpflag2", "0",
  CB(cSockets->CertsSetBypassFlags2, uint64_t), TUINTEGER|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_BUFFER
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_buffer", "65536",
  CB(SocketSetBufferSize, size_t), TUINTEGER|CPOW2|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_RTIMEOUT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_rtimeout", "120",
  CB(SocketSetRXTimeout, double), TUFLOAT|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_STIMEOUT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_stimeout", "30",
  CB(SocketSetTXTimeout, double), TUFLOAT|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_CIPHERTLSv1
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_ciphertlsv1", "HIGH:!DSS:!aNULL@STRENGTH",
  NoOp, MTRIM|TSTRING|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_CIPHERTLSv13
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_ciphertlsv13", cCommon->Blank(), NoOp,
  MTRIM|TSTRING|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_CASTORE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_castore", cCommon->Blank(),
  CBSTR(cSockets->CertsFileModified), TSTRING|CFILENAME|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_OCSP
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_ocsp", "1",
  CB(SocketOCSPModified, int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! NET_USERAGENT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "net_useragent", cCommon->Blank(),
  CBSTR(SocketAgentModified), TSTRING|MTRIM|PBOOT|PSYSTEM },
/* == Video cvars ========================================================== */
// ! VID_ALPHA
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_alpha", "0",
  CB(cDisplay->SetWindowTransparency, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_CLEAR
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_clear", "1",
  CB(cFboMain->SetBackBufferClear, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_CLEARCOLOUR
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_clearcolour", "0",
  CB(cFboMain->SetBackBufferClearColour, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_DEBUG
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_debug", "0",
  CB(cDisplay->SetGLDebugMode, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_FS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_fs", "0",
  CB(cDisplay->FullScreenStateChanged, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_FSMODE
// ? Specifies which full-screen mode to use. You can check the log or use
// ? 'vmlist' console command to see which resolutions are available on your
// ? system. Anything >= -1 means exclusive full-screen mode and -1 makes the
// ? engine use the current desktop resolution (default on Mac for now). Use -2
// ? to make the engine use borderless full-screen mode (default on Win/Linux).
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_fsmode",
  /* ----------------------------------------------------------------------- */
#if defined(MACOS)
  "-1",
#else
  "-2",
#endif
  /* ----------------------------------------------------------------------- */
  CB(cDisplay->FullScreenModeChanged, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_LOCK
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_lock", "0",
  CB(cFboMain->SetLockViewport, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_MONITOR
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_monitor", "-1",
  CB(cDisplay->MonitorChanged, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_GAMMA
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_gamma", "1",
  CB(cDisplay->GammaChanged, GLfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_FSAA
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_fsaa", "-1",
  CB(cDisplay->FsaaChanged, int), TINTEGERSAVE|CPOW2Z|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_BPP
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
  { GM_GRAPHICS, GM_HIGHEST, "vid_bpp",
  /* ----------------------------------------------------------------------- */
#if defined(WINDOWS)
  "0",                                 // Win32 doesn't need forcing depth
#else
  "16",                                // For better bit-depth quality
#endif
/* ------------------------------------------------------------------------- */
  CB(cDisplay->SetForcedBitDepth, int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_HIDPI
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_hidpi", "1",
  CB(cDisplay->HiDPIChanged, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_STEREO
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_stereo", "0" ,
  CB(cDisplay->SetStereoMode, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_NOERRORS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_noerrors", "0",
  CB(cDisplay->SetNoErrorsMode, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_SRGB
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_srgb", "1",
  CB(cDisplay->SRGBColourSpaceChanged, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_AUXBUFFERS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_auxbuffers", "-1",
  CB(cDisplay->AuxBuffersChanged, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_SIMPLEMATRIX
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_simplematrix", "0",
  CB(cFboMain->SetSimpleMatrix, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_TEXFILTER
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_texfilter", "3",
  CB(cFboMain->SetFilter, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_VSYNC
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_vsync", "1",
  CB(cOgl->SetVSyncMode, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_GASWITCH
// ? Set to 0 (default) to disable MacOS graphics switching, or 1 to allow
// ? MacOS to switch between integral and dedicated graphics.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_gaswitch", "0",
  CB(cDisplay->GraphicsSwitchingChanged, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_WIREFRAME
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_wireframe", "0",
  CB(cOgl->SetPolygonMode, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_ORWIDTH
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_orwidth", "640",
  CB(cDisplay->SetOrthoWidth, GLfloat), TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_ORHEIGHT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_orheight", "480",
  CB(cDisplay->SetOrthoHeight, GLfloat), TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_ORASPMIN
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_oraspmin", "1.25",
  CB(cFboMain->SetMinOrtho, GLfloat), TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_ORASPMAX
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_oraspmax", "1.777778",
  CB(cFboMain->SetMaxOrtho, GLfloat), TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_RFBO
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_rfbo", "2",
  CB(FboSetOrderReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_RFLOATS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_rfloats", "10000",
  CB(cFboMain->SetFloatReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_RCMDS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_rcmds", "1000",
  CB(cFboMain->SetCommandReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_RDTEX
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_rdtex", "10",
  CB(cOgl->SetTexDListReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_RDFBO
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_rdfbo", "10",
  CB(cOgl->SetFboDListReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_SSTYPE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_sstype", "0",
  CB(cSShot->SetScreenShotType, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_SUBPIXROUND
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_subpixround", "0",
  CB(SetSPRoundingMethod, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_QSHADER
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_qshader", "3",
  CB(cOgl->SetQShaderHint, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_QLINE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_qline", "0",
  CB(cOgl->SetQLineHint, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_QPOLYGON
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_qpolygon", "0",
  CB(cOgl->SetQPolygonHint, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_QCOMPRESS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "vid_qcompress", "3",
  CB(cOgl->SetQCompressHint, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_ASPECT
// ? Force the window to have an aspect ratio. Specify as a floating point
// ? number. i.e. 16.9 for 16:9 or 4.3 for 4:3, etc. or 0 for numeric and/or
// ? denominator for freedom for the end user to resize at will.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_aspect", "0", NoOp, TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! WIN_BORDER
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_border", "1",
  CB(cDisplay->BorderChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_CLOSEABLE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_closeable", "1",
  CB(cDisplay->CloseableChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_FLOATING
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_floating", "0",
  CB(cDisplay->FloatingChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_FOCUSED
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_focused", "1",
  CB(cDisplay->AutoFocusChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_HEIGHT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_height", "0",
  CB(cDisplay->HeightChanged, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_HEIGHTMIN
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_heightmin", "0", NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! WIN_MAXIMISED
// ? Specify 1 to have the window automatically maximised on creation or 0
// ? to not. The default value is 0.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_maximised", "0",
  CB(cDisplay->SetMaximisedMode, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_MINIMISEAUTO
// ? Specify 1 to have the window automatically minimise when it is not active
// ? or 0 to not. The default value is 1.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_minimiseauto", "1",
  CB(cDisplay->AutoIconifyChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_POSX
// ? Specify the default X position of the window. -2 means the centre of the
// ? screen and -1 means the default position. Any other value is the actual
// ? physical position on the screen. The default value is -2.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_posx", "-2",
  CB(cDisplay->SetXPosition, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_POSY
// ? Specify the default Y position of the window. -2 means the centre of the
// ? screen and -1 means the default position. Any other value is the actual
// ? physical position on the screen. The default value is -2.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_posy", "-2",
  CB(cDisplay->SetYPosition, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_SIZABLE
// ? Specify 1 if the window is allowed to be resized or 0 if not. The default
// ? value is 1.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_sizable", "1",
  CB(cDisplay->SizableChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_THREAD
// ? Set to 1 to allow the window to be managed in it's own thread for optimum
// ? performance. Set to 0 to have it share with the engine main thread. This
// ? variable is defaulted to 0 on anything but Windows due to what appears to
// ? be upstream issues on both Wayland and MacOS. This can only be changed at
// ? the in the application configuration file and not saved to the persistence
// ? database.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_thread",
  /* ----------------------------------------------------------------------- */
#if defined(WINDOWS)
  "1",
#else
  "0",                                 // Fix temporary bug in GLFW
#endif
  /* ----------------------------------------------------------------------- */
  NoOp, TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_WIDTH
// ? Sets the initial width of the window. This value is saved to the
// ? persistence database when changed.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_width", "0",
  CB(cDisplay->WidthChanged, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_WIDTHMIN
// ? Sets the minimum width of the window. This is only changable at the
// ? application configuration file and is not saved to persistence database.
/* ------------------------------------------------------------------------- */
{ GM_GRAPHICS, GM_HIGHEST, "win_widthmin", "0", NoOp, TUINTEGER|PSYSTEM },
/* == Logging cvars ======================================================== */
// ! LOG_CREDITS
// ? Specifies to include credits in the log at startup.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "log_credits", "0",
  CB(cCredits->CreditDumpList, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! LOG_DYLIBS
// ? Specifies to include shared libraries in the log at startup.
/* ------------------------------------------------------------------------- */
{ GM_TEXT_NOAUDIO, GM_HIGHEST, "log_dylibs", "0",
  CB(cSystem->DumpModuleList, bool), TBOOLEAN|PANY },
/* -- Undefines ------------------------------------------------------------ */
#undef CBSTR                           // Done with string function callback
#undef CB                              // Done with int function callback
/* ------------------------------------------------------------------------- */
} };                                   // End of module namespace
/* == EoF =========================================================== EoF == */
