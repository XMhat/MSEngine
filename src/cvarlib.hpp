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
{ CF_NOTHING, "log_level",
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
{ CF_NOTHING, "app_cmdline", cCommon->Blank(),
  CBSTR(cCore->CoreParseCmdLine), CONFIDENTIAL|TSTRING|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! AST_LZMABUFFER
// ? Specifies the decompression buffer size (in bytes) for the lzma api. The
// ? default value is 256 kilobytes.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "ast_lzmabuffer", "262144",
  CB(ArchiveSetBufferSize, size_t), TUINTEGER|CPOW2|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! AST_PIPEBUFFER
// ? Specifies the size of the pipe buffer.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "ast_pipebuffer", "4096",
  CB(AssetSetPipeBufferSize, size_t), TUINTEGER|CPOW2|PANY },
/* ------------------------------------------------------------------------- */
// ! AST_FSOVERRIDE
// ? Specifies whether the engine is allowed to override internal files from
// ? archives with files. This is makes it easier to update and mod resource
// ? if true. It is by default disabled on release builds and enabled on
// ? all other builds.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "ast_fsoverride",
/* ------------------------------------------------------------------------- */
#if defined(RELEASE)                   // Default disabled in release builds
  cCommon->Zero(),
#else                                  // Default enabled in other builds
  cCommon->One(),
#endif                                 // Build type check
  /* ----------------------------------------------------------------------- */
  CB(AssetSetFSOverride, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! AST_EXEBUNDLE
// ? A boolean to specify if the executable file should be checked for a
// ? 7-zip archive and use that to load guest assets. This is only supported
// ? on Windows architectures right now and ignored on others.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "ast_exebundle", cCommon->One(),
  CB(ArchiveInitExe, bool), TBOOLEAN|PBOOT },
/* ------------------------------------------------------------------------- */
// ! APP_BASEDIR
// ? Specifies the base directory of where the executable was started from. It
// ? is only readable by the end-user and the host, but not the guest.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_basedir", cCommon->Blank(),
  CBSTR(cSystem->SetWorkDir), CONFIDENTIAL|TSTRING|CTRUSTEDFN|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! AST_BUNDLES
// ? Specifies the [.ext]ension to use as 7-zip archives. These filenames will
// ? be checked for at startup and automatically loaded by the engine.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "ast_bundles", "." ARCHIVE_EXTENSION,
  CBSTR(ArchiveInit), TSTRING|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! APP_CONFIG
// ? Loads the configuration file with the specified extension (usually 'app').
// ? The suffix extension of '.cfg' is always assumed for protection so the
// ? guest need never specify it. This variable can only be set at the
// ? command-line.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_config", DEFAULT_CONFIGURATION,
  CBSTR(cCVars->ExecuteAppConfig), TSTRING|CFILENAME|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! APP_AUTHOR
// ? Specifies the author of the guest application. This is used for display
// ? purposes only and can be requested by the guest and only set in the
// ? app.cfg file.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_author", "Anonymous", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_SHORTNAME
// ? Specifies a short title name for the guest application. It is purely for
// ? display purposes only to the end-user. It can be requested by the guest
// ? and only set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_shortname", "Untitled", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_HOMEDIR
// ? Species the users home directory where files are written to if they cannot
// ? be written to the working directory. It is only readable by the end-user
// ? and the host, but not the guest.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_homedir", cCommon->Blank(),
  CBSTR(cCore->CoreSetHomeDir), CONFIDENTIAL|TSTRING|CTRUSTEDFN|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! SQL_DB
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_db", cCommon->Blank(),
  CBSTR(cSql->UdbFileModified), CONFIDENTIAL|TSTRING|CTRUSTEDFN|MTRIM|PBOOT },
/* ------------------------------------------------------------------------- */
// ! SQL_ERASEEMPTY
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_eraseempty", cCommon->One(),
  CB(cSql->DeleteEmptyDBModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_TEMPSTORE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_tempstore", "MEMORY",
  CBSTR(cSql->TempStoreModified), TSTRING|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_SYNCHRONOUS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_synchronous", cCommon->Zero(),
  CB(cSql->SynchronousModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_JOURNALMODE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_journalmode", cCommon->Zero(),
  CB(cSql->JournalModeModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_AUTOVACUUM
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_autovacuum", cCommon->One(),
  CB(cSql->AutoVacuumModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_FOREIGNKEYS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_foreignkeys", cCommon->One(),
  CB(cSql->ForeignKeysModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_INCVACUUM
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_incvacuum", cCommon->Zero(),
  CB(cSql->IncVacuumModified, uint64_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_DEFAULTS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_defaults", cCommon->Zero(),
  CB(cCVars->SetDefaults, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! SQL_LOADCONFIG
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "sql_loadconfig", cCommon->One(),
  CB(cCVars->LoadSettings, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_CFLAGS
// ? Specifies how the host wants to be run with the following flags...
// ? 0x1 (CF_TERMINAL) = Opens (Win32) or reuses (Unix) a console window.
// ? 0x2 (CF_AUDIO)    = Initialises an OpenAL audio context and exposes API.
// ? 0x4 (CF_VIDEO)    = Initialises an OpenGL context+window and exposes API.
{ CF_NOTHING, "app_cflags", cCommon->Zero(),
  CB(cSystem->SetCoreFlags, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LOG_LINES
// ? Specifies the maximum number of lines to keep in the backlog. The log
// ? lines are always empty when logging to a file.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "log_lines", "10000",
  CB(cLog->LogLinesModified, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! LOG_FILE
// ? Specifies a file to log internal engine messages to. It is used to help
// ? debugging. Leave as blank for no log file. It can be set from command-line
// ? parameters, the app.cfg file and the user.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "log_file", cCommon->Blank(),
  CBSTR(cLog->LogFileModified), TSTRING|CFILENAME|MTRIM|PBOOT|PUSR },
/* ------------------------------------------------------------------------- */
// ! APP_LONGNAME
// ? Specifies a long title name for the guest application. It is uses as the
// ? window title and for display purposes only to the end-user. It can be
// ? requested by the guest and only set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_longname", "Untitled", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_CLEARMUTEX
// ? Asks the system to delete the global mutex in Linux and MacOS so the
// ? engine can startup. You only need to do this if the engine crashes. On
// ? Windows the mutex is freed on process termination so there is never any
// ? need to use this on Windows.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_clearmutex",
  /* ----------------------------------------------------------------------- */
#if !defined(WINDOWS) && defined(ALPHA)
  cCommon->One(),
#else
  cCommon->Zero(),
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
{ CF_NOTHING, "err_instance", cCommon->One(),
  CB(cCore->CoreSetOneInstance, bool), TBOOLEAN|PSYSTEM },
/* == Object cvars ========================================================= */
// ! OBJ_CLIPMAX
// ? Specifies the maximum number of clipboard objects allowed to be registered
// ? by the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_clipmax", "100",
  CB(cClips->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_CMDMAX
// ? Specifies the maximum number of console commands allowed to be registered
// ? by the engine. This includes the internal default commands. An exception
// ? is generated if more cvars than this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_cmdmax", "1000",
  CB(cConsole->MaxCountModified, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_CVARMAX
// ? Specifies the maximum number of cvars allowed to be registered by the
// ? engine. This includes the internal default cvars. An exception is
// ? generated if more cvars than this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_cvarmax", "1000",
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
{ CF_NOTHING, "obj_cvarimax", "1000",
  CB(cCVars->MaxICountModified, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_ARCHIVEMAX
// ? Specifies the maximum number of archive objects allowed to be registered
// ? by the engine. This includes the archives that are loaded at startup
// ? An exception is generated if more archives than this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_archivemax", "1000",
  CB(cArchives->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_ASSETMAX
// ? Specifies the maximum number of asset objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_assetmax", "1000",
  CB(cAssets->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_BINMAX
// ? Specifies the maximum number of bin objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_binmax", "1000",
  CB(cBins->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_CURSORMAX
// ? Specifies the maximum number of cursor objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "obj_cursormax", "1000",
  CB(cCursors->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FBOMAX
// ? Specifies the maximum number of framebuffer objects allowed to be
// ? registered by the engine. An exception is generated if more cvars than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "obj_fbomax", "1000",
  CB(cFbos->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FONTMAX
// ? Specifies the maximum number of font objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "obj_fontmax", "1000",
  CB(cFonts->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FILEMAX
// ? Specifies the maximum number of file objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_filemax", "1000",
  CB(cFiles->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FTFMAX
// ? Specifies the maximum number of freetype objects allowed to be registered
// ? by the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_ftfmax", "1000",
  CB(cFtfs->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_FUNCMAX
// ? Specifies the maximum number of lua referenced function objects allowed to
// ? be registered by the engine. An exception is generated if more cvars than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_funcmax", "1000",
  CB(cLuaFuncs->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_IMGMAX
// ? Specifies the maximum number of image objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_imgmax", "1000",
  CB(cImages->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_IMGFMTMAX
// ? Specifies the maximum number of image format objects allowed to be
// ? registered by the engine. An exception is generated if more cvars than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_imgfmtmax", "1000",
  CB(cImageFmts->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_JSONMAX
// ? Specifies the maximum number of json objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_jsonmax", "1000",
  CB(cJsons->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_MASKMAX
// ? Specifies the maximum number of mask objects allowed to be registered by
// ? the engine. An exception is generated if more cvars than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_maskmax", "1000",
  CB(cMasks->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_PCMMAX
// ? Specifies the maximum number of pcm objects allowed to be registered by
// ? the engine. An exception is generated if more objects than this are
// ? allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_pcmmax", "1000",
  CB(cPcms->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_PCMFMTMAX
// ? Specifies the maximum number of pcm format objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_pcmfmtmax", "1000",
  CB(cPcmFmts->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_SAMPLEMAX
// ? Specifies the maximum number of sample objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "obj_samplemax", "1000",
  CB(cSamples->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_SHADERMAX
// ? Specifies the maximum number of shader objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "obj_shadermax", "1000",
  CB(cShaders->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_SOCKETMAX
// ? Specifies the maximum number of socket objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_socketmax", "1000",
  CB(cSockets->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_SOURCEMAX
// ? Specifies the maximum number of source objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "obj_sourcemax", "1000",
  CB(cSources->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_STATMAX
// ? Specifies the maximum number of stat objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_statmax", "1000",
  CB(cStats->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_STREAMMAX
// ? Specifies the maximum number of stream objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "obj_streammax", "1000",
  CB(cStreams->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_TEXTUREMAX
// ? Specifies the maximum number of texture objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "obj_texturemax", "1000",
  CB(cTextures->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_THREADMAX
// ? Specifies the maximum number of thread objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "obj_threadmax", "1000",
  CB(cThreads->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! OBJ_VIDEOMAX
// ? Specifies the maximum number of video objects allowed to be
// ? registered by the engine. An exception is generated if more objects than
// ? this are allocated.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "obj_videomax", "1000",
  CB(cVideos->CollectorSetLimit, size_t), TUINTEGER|PSYSTEM },
/* == Base cvars =========================================================== */
// ! APP_DESCRIPTION
// ? Specifies a description for the guest application. It is purely for
// ? display purposes only to the end-user. It can be requested by the guest
// ? and only set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_description", "Undescribed", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_VERSION
// ? Specifies a version for the guest application. It is purely for display
// ? purposes only to the end-user. It can be requested by the guest and only
// ? set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_version", "0.0", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_ICON
// ? Specifies a large icon for use with the application and window. It can be
// ? any format that is supported with the Image.* loading functions. It can
// ? also be changed dynamically by Lua. Keep to empty for default icon.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_icon", cCommon->Blank(),
  CBSTR(cDisplay->SetIcon), TSTRING|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_COPYRIGHT
// ? Specifies any copyright information for the guest application. It is
// ? purely for display purposes only to the end-user. It can be requested by
// ? the guest and only set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_copyright", "Public Domain", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_WEBSITE
// ? Specifies a website for the guest application. It is purely for display
// ? purposes only to the end-user. It can be requested by the guest and only
// ? set in the app.cfg file.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_website", "about:blank", NoOp,
  TSTRING|CNOTEMPTY|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_TICKRATE
// ? Specifies the base tick rate of the guest application in ticks when using
// ? the timer accumulator. The engine locks to this tick rate regardless of
// ? lag. It can also be changed dynamically with Lua.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "app_tickrate", "60",
  CB(cTimer->TimerTickRateModified, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! APP_DELAY
// ? Specifies an artificial delay to force for bot mode in milliseconds. This
// ? is ignored on interactive mode because a one millisecond delay is forced
// ? for every frame under the target rate.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "app_delay", cCommon->One(),
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
{ CF_NOTHING, "app_title", cCommon->Blank(),
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
{ CF_NOTHING, "err_admin", "2",
  CB(cSystem->CheckAdminModified, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! ERR_CHECKSUM
// ? Throws an error if there is an executable checksum mismatch. This only
// ? applies on Windows executables only as Linux executable does not have a
// ? checksum and MacOS uses code signing externally. The default value is 1
// ? on release executable else 0 on beta executable.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "err_checksum",
  /* ----------------------------------------------------------------------- */
#if defined(RELEASE)
  cCommon->One(),
#else
  cCommon->Zero(),
#endif
  /* ----------------------------------------------------------------------- */
  CB(cSystem->CheckChecksumModified, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! ERR_DEBUGGER
// ? Throws an error if a debugger is running at start-up. The default value
// ? is 1 on release executable else 0 on beta executable.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "err_debugger",
  /* ----------------------------------------------------------------------- */
#if defined(RELEASE)
  cCommon->One(),
#else
  cCommon->Zero(),
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
{ CF_NOTHING, "err_luamode",
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
{ CF_NOTHING, "err_lmresetlimit",
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
{ CF_VIDEO, "err_minvram", cCommon->Zero(),
  CB(cOgl->SetMinVRAM, uint64_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! ERR_MINRAM
// ? The engine fails to run if the system does not have this amount of free
// ? memory available. If this value is zero then the check does not take
// ? place else the amount specified is allocated at startup, cleared, the
// ? speed of the clear is reported in the log then the memory is freed. The
// ? default value is zero.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "err_minram", cCommon->Zero(),
  CB(cSystem->SetMinRAM, uint64_t), TUINTEGER|PSYSTEM },
/* == Lua cvars ============================================================ */
// ! LUA_TICKTIMEOUT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "lua_ticktimeout", "10",
  CB(cTimer->TimerSetTimeOut, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_TICKCHECK
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "lua_tickcheck", "1000000",
  CB(cLua->SetOpsInterval, int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_CACHE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "lua_cache", cCommon->One(),
  CB(LuaCodeSetCache, unsigned int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! LUA_SIZESTACK
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "lua_sizestack", "1000",
  CB(cLua->SetStack, int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_GCPAUSE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "lua_gcpause", "200",
  CB(cLua->SetGCPause, int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_GCSTEPMUL
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "lua_gcstepmul", "100",
  CB(cLua->SetGCStep, int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_RANDOMSEED
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "lua_randomseed", cCommon->Zero(),
  CB(cLua->SetSeed, lua_Integer), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_APIFLAGS
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "lua_apiflags", "3",
  CB(cLua->SetFlags, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! LUA_SCRIPT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "lua_script", "main.lua", NoOp,
  TSTRING|CFILENAME|CNOTEMPTY|MTRIM|PSYSTEM },
/* == Audio cvars ========================================================== */
// ! AUD_DELAY
// ? Specifies an delay (in number of milliseconds) to suspend the audio
// ? thread each tick. Lower values use less CPU usage but may cause audio
// ? pops. The audio thread is in charge of managing Source classes and
// ? checking for discreprencies.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_delay", "10",
  CB(cAudio->SetAudThreadDelay, ALuint), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_VOL
// ? Specifies the global volume of all class types. 0.0 (mute) to
// ? 1.0 (maximum volume). Setting a value too high may cause artifacts on
// ? older OS audio API's.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_vol", "0.75",
  CB(cAudio->SetGlobalVolume, ALfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_INTERFACE
// ? Specifies the audio device number to use. -1 makes sure to use the default
// ? device, otherwise it is the index number of a specific audio device to
// ? use.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_interface", "-1", NoOp, TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_CHECK
// ? Specifies an interval (in number of milliseconds) of checking for audio
// ? device changes as OpenAL does not have an event interface for this. This
// ? check is done in the audio manager thread. If a change in the audio
// ? device list is detected then the audio is reset automatically, as if the
// ? 'areset' console command was used.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_check", "5000",
  CB(cAudio->SetAudCheckRate, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_NUMSOURCES
// ? Specifies the maximum number of sources to preallocate. 0 means sources
// ? are (de)allocated dynamically. Setting this value to non-zero may improve
// ? CPU usage but increases the chances of errors if too many sources are
// ? requested.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_numsources", cCommon->Zero(),
  CB(SourceSetCount, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_SAMVOL
// ? Specifies the volume of Sample classes. 0.0 (mute) to
// ? 1.0 (maximum volume). Setting a value too high may cause artifacts on
// ? older OS audio API's.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_samvol", "0.75",
  CB(SetSampleVolume, ALfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_STRBUFCOUNT
// ? Specifies the number of buffers to use for Stream classes. We need
// ? multiple buffers to make streamed audio playback consistent. Four should
// ? always be enough.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_strbufcount", "4",
  CB(StreamSetBufferCount, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_STRBUFSIZ
// ? Specifies the size (in bytes) of each stream buffer for Stream classes.
// ? We need sufficiently large enough buffers to make streamed audio playback
// ? consistent.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_strbufsiz", "32768",
  CB(StreamSetBufferSize, size_t), TUINTEGERSAVE|CPOW2|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_STRVOL
// ? Specifies the volume of Stream classes. 0.0 (mute) to
// ? 1.0 (maximum volume). Setting a value too high may cause artifacts on
// ? older OS audio API's.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_strvol", "0.75",
  CB(StreamSetVolume, ALfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_FMVVOL
// ? Specifies the volume of Video classes. 0.0 (mute) to 1.0 (maximum volume).
// ? Setting a value too high may cause artifacts on older OS audio API's.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_fmvvol", "0.75",
  CB(VideoSetVolume, ALfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! AUD_HRTF
// ? Specifies whether to use HRTF dynamics on audio output. This could cause
// ? strange audio stereo quality issues so it is recommended to disable.
/* ------------------------------------------------------------------------- */
{ CF_AUDIO, "aud_hrtf", cCommon->Zero(),
  NoOp, TUINTEGERSAVE|PANY },
/* == Console cvars ======================================================== */
// ! CON_KEYPRIMARY
// ? The primary GLFW console key virtual key code to use to toggle console
// ? visibility. It is set to '`' as default on a UK keyboard.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_keyprimary", "96",
  CB(cInput->SetConsoleKey1, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_KEYSECONDARY
// ? The secondary GLFW console key virtual key code to use to toggle console
// ? visibility. It is set to '+-' as default on a US keyboard.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_keysecondary", "161",
  CB(cInput->SetConsoleKey2, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_AUTOCOMPLETE
// ? Flags that specify what to auto complete when TAB key is pressed in the
// ? console...
// ? AC_NONE     (0x0): Autocompletion is disabled
// ? AC_COMMANDS (0x1): Autocomplete command names
// ? AC_CVARS    (0x2): Autocomplete cvar names
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "con_autocomplete", "3",
  CB(cConsole->SetAutoComplete, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_AUTOSCROLL
// ? A boolean that specifies whether to autoscroll the console to the last
// ? newly printed message.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "con_autoscroll", cCommon->One(),
  CB(cConsole->SetAutoScroll, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_AUTOCOPYCVAR
// ? A boolean that specifies whether to automatically copy the variable
// ? name and value when a cvar name is typed in the console.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "con_autocopycvar", cCommon->One(),
  CB(cConsole->SetAutoCopyCVar, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_HEIGHT
// ? Specifies the height of the console in OpenGL mode. 1 means the console
// ? is covering the whole screen, and 0 means the console is not showing.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_height", "0.5",
  CB(cConsole->SetHeight, GLfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_BLOUTPUT
// ? Specifies the maximum number of lines to keep in the console. Excess
// ? messages are discarded automatically to not fill up memory.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "con_bloutput", "10000",
  CB(cConsole->SetConsoleOutputLines, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_BLINPUT
// ? Number of lines to store in the input backlog. A successful command input
// ? automatically adds the last command typed into this list.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "con_blinput", "1000",
  CB(cConsole->SetConsoleInputLines, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_DISABLED
// ? Specifies wether the console is permanantly disabled or not.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_disabled",
  /* ----------------------------------------------------------------------- */
#if defined(RELEASE)
  cCommon->One(),
#else
  cCommon->Zero(),
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
{ CF_NOTHING, "con_cvshowflags", cCommon->Zero(),
  CB(cCVars->SetDisplayFlags, unsigned int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_BGCOLOUR
// ? Specifies the background colour of the console texture in the syntax of
// ? 0xAARRGGBB or an integral number.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_bgcolour", "2130706432",
  CB(cConsole->TextBackgroundColourModified, uint32_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_BGTEXTURE
// ? The texture file to load that will be used as the background for the
// ? console.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_bgtexture", cCommon->Blank(), NoOp,
  TSTRING|CFILENAME|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONT
// ? Specifies the filename of the FreeType compatible font to load as the
// ? console font file.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_font", "console.ttf", NoOp,
  TSTRING|CFILENAME|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTFLAGS
// ? Specifies manipulation of the loaded font. See the 'Char.Flags' for
// ? possible values
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontflags", cCommon->Zero(),
  CB(cConsole->ConsoleFontFlagsModified, unsigned int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_FONTCOLOUR
// ? Specifies the default colour of the console text. See the Console.Colours
// ? lookup table for possible values. There are only 16 pre-defined colours
// ? to keep compatibility with the system text console.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontcolour", "15",
  CB(cConsole->TextForegroundColourModified, unsigned int),
  TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_FONTHEIGHT
// ? Specifies the height of the loaded console FreeType font.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontheight", "20",
  CB(cConsole->TextHeightModified, GLfloat), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTPADDING
// ? Specifies any extra padding to add to each FreeType font glyph to prevent
// ? pixels from other glyphs spilling into the render due to filtering.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontpadding", cCommon->Zero(),
  NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTPCMIN
// ? Specifies the minimum character code to start precaching from
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontpcmin", "32", NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTPCMAX
// ? Specifies the maximum character code to end precaching at
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontpcmax", "256", NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTSCALE
// ? Specifies the scale adjust of the font. A value not equal to one will
// ? cause interpolation to occur so filtering is advised.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontscale", cCommon->One(),
  CB(cConsole->TextScaleModified, GLfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_FONTSPACING
// ? Specifies the amount of padding to add after each cahracter rendered.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontspacing", cCommon->Zero(),
  CB(cConsole->TextLetterSpacingModified, GLfloat), TFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTLSPACING
// ? Specifies the amount of padding to add below each line of text rendered.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontlspacing", cCommon->Zero(),
  CB(cConsole->TextLineSpacingModified, GLfloat), TFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTWIDTH
// ? Specifies the width of the loaded console FreeType font.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fontwidth", "20",
  CB(cConsole->TextWidthModified, GLfloat), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_FONTTEXSIZE
// ? Specifies the initial size of the texture canvas in pixels. Do not set
// ? this more than 1024 pixels unless you know for a fact the GPU will support
// ? this value. An exception is thrown if not. Zero means start with enough
// ? size for one character.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "con_fonttexsize", cCommon->Zero(),
  NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_INPUTMAX
// ? Specifies the maximum number of characters that are allowed to be typed
// ? into the console input buffer.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "con_inputmax", "1000",
  CB(cConsole->SetMaxConLineChars, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_PAGELINES
// ? The number of lines that are scrolled when pressing CONTROL and PAGEUP or
// ? PAGEDOWN keys.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "con_pagelines", "10",
  CB(cConsole->SetPageMoveCount, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_TMCCOLS
// ? In bot mode, this is the maximum number of columns to draw. The larger
// ? this value is, the more text that can be displayed at once but will
// ? increase CPU usage from the operating system.
/* ------------------------------------------------------------------------- */
{ CF_TERMINAL, "con_tmccols", "80",
  CB(cSystem->ColsModified, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_TMCROWS
// ? In bot mode, this is the maximum number of rows to draw. The larger
// ? this value is, the more text that can be displayed at once but will
// ? increase CPU usage from the operating system.
/* ------------------------------------------------------------------------- */
{ CF_TERMINAL, "con_tmcrows", "32",
  CB(cSystem->RowsModified, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_TMCREFRESH
// ? The interval in milliseconds where the status bar and title bar are
// ? refreshed. The title bar contains basic operating statistics and the
// ? status bar is configurable by LUA and contains the input bar when text
// ? is being typed.
/* ------------------------------------------------------------------------- */
{ CF_TERMINAL, "con_tmcrefresh", "1000",
  CB(cConsole->RefreshModified, unsigned int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! CON_TMCNOCLOSE
// ? Disables the X button and close in the application context menu. This does
// ? not prevent CONTROL+C/BREAK keys.
/* ------------------------------------------------------------------------- */
{ CF_TERMINAL, "con_tmcnoclose", cCommon->One(), NoOp,
  TBOOLEAN|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! CON_TMCTFORMAT
// ? The format of the time in the titlebar (see strftime() for details).
/* ------------------------------------------------------------------------- */
{ CF_TERMINAL, "con_tmctformat", "%y-%m-%d %T",
  CBSTR(cConsole->SetTimeFormat), TSTRINGSAVE|MTRIM|CNOTEMPTY|PSYSTEM|PUSR },
/* == Fmv cvars ============================================================ */
// ! FMV_IOBUFFER
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_AUDIOVIDEO, "fmv_iobuffer", "65536",
  CB(VideoSetBufferSize, long), TUINTEGERSAVE|CPOW2|PANY },
/* ------------------------------------------------------------------------- */
// ! FMV_MAXDRIFT
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_AUDIOVIDEO, "fmv_maxdrift", "0.1",
  CB(VideoSetMaximumDrift, double), TUFLOAT|PANY },
/* == Input cvars ========================================================== */
// ! INP_JOYDEFFDZ
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "inp_joydeffdz", "0.25",
  CB(cInput->SetDefaultJoyFwdDZ, float), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_JOYDEFRDZ
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "inp_joydefrdz", "0.25",
  CB(cInput->SetDefaultJoyRevDZ, float), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_JOYSTICK
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "inp_joystick", "-1",
  CB(cInput->SetJoystickEnabled, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_FSTOGGLER
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "inp_fstoggler", cCommon->One(),
  CB(cInput->SetFSTogglerEnabled, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_RAWMOUSE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "inp_rawmouse", cCommon->One(),
  CB(cInput->SetRawMouseEnabled, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_STICKYKEY
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "inp_stickykey", cCommon->One(),
  CB(cInput->SetStickyKeyEnabled, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! INP_STICKYMOUSE
// ? Not explained yet.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "inp_stickymouse", cCommon->One(),
  CB(cInput->SetStickyMouseEnabled, bool), TBOOLEANSAVE|PANY },
/* == Network cvars ======================================================== */
// ! NET_CBPFLAG1
// ? Specifies the certificate error ignore flags.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_cbpflag1", cCommon->Zero(),
  CB(cSockets->CertsSetBypassFlags1, uint64_t), TUINTEGER|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_CBPFLAG2
// ? Specifies the extended certificate error ignore flags.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_cbpflag2", cCommon->Zero(),
  CB(cSockets->CertsSetBypassFlags2, uint64_t), TUINTEGER|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_BUFFER
// ? Specifies the amount of memory in bytes to reserve for each recv() or
// ? send() call. Default is 64k.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_buffer", "65536",
  CB(SocketSetBufferSize, size_t), TUINTEGER|CPOW2|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_RTIMEOUT
// ? Specifies a socket recv() command timeout in seconds.
// ? Default is 2 minutes.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_rtimeout", "120",
  CB(SocketSetRXTimeout, double), TUFLOAT|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_STIMEOUT
// ? Specifies a socket send() command timeout in seconds.
// ? Default is 30 seconds.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_stimeout", "30",
  CB(SocketSetTXTimeout, double), TUFLOAT|PBOOT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_CIPHERTLSv1
// ? Specifies the default settings for TLSv1.x connections. The default is
// ? use only maximum strength ciphers.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_ciphertlsv1", "HIGH:!DSS:!aNULL@STRENGTH",
  NoOp, MTRIM|TSTRING|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_CIPHERTLSv13
// ? Specifies the default settings for TLSv3 connections. The default is empty
// ? which lets OpenSSL decide.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_ciphertlsv13", cCommon->Blank(), NoOp,
  MTRIM|TSTRING|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_CASTORE
// ? Specifies the relative directory to the client certificate store. These
// ? are required CA certificates that are used for the basis of every SSL
// ? connection. These certificates are loaded and checked at startup and
// ? a log warning message is generated if the certificate is not usable.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_castore", cCommon->Blank(),
  CBSTR(cSockets->CertsFileModified), TSTRING|CFILENAME|MTRIM|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! NET_OCSP
// ? Informs OpenSSL to verify server certificates via OCSP.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_ocsp", cCommon->One(),
  CB(SocketOCSPModified, int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! NET_USERAGENT
// ? When using the built-in simple HTTP client, this string is sent as the
// ? default 'User-Agent' variable. It is automatically generated if left empty
// ? to "Mozilla/5.0 (<EngineName>; <EngineBits>-bit; v<EngineVersion>)
// ? <AppName>/<AppVersion>".
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "net_useragent", cCommon->Blank(),
  CBSTR(SocketAgentModified), TSTRING|MTRIM|PBOOT|PSYSTEM },
/* == Video cvars ========================================================== */
// ! VID_ALPHA
// ? Specifies that the main frame buffer has alpha (GL_RGBA) which is needed
// ? for transparent windows. Default is 0 to use GL_RGB.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_alpha", cCommon->Zero(),
  CB(cDisplay->SetWindowTransparency, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_CLEAR
// ? Specifies to clear the main frame buffer every frame.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_clear", cCommon->One(),
  CB(cFboMain->SetBackBufferClear, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_CLEARCOLOUR
// ? Specifies the 32-bit integer (0xAARRGGBB) as the default clear value.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_clearcolour", cCommon->Zero(),
  CB(cFboMain->SetBackBufferClearColour, unsigned int), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_DEBUG
// ? Sets 'GLFW_OPENGL_DEBUG_CONTEXT'. Default is 0 (disabled).
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_debug", cCommon->Zero(),
  CB(cDisplay->SetGLDebugMode, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_FS
// ? Specifies what full-screen type to use. Specify 0 to keep the engine in
// ? a normal window, 1 for exclusive full-screen mode (specified by
// ? 'vid_fsmode') or 2 for borderless full-screen mode.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_fs", cCommon->Zero(),
  CB(cDisplay->FullScreenStateChanged, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_FSMODE
// ? Specifies which full-screen mode to use. You can check the log or use
// ? 'vmlist' console command to see which resolutions are available on your
// ? system. Anything >= -1 means exclusive full-screen mode and -1 makes the
// ? engine use the current desktop resolution (default on Mac for now). Use -2
// ? to make the engine use borderless full-screen mode (default on Win/Linux).
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_fsmode",
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
// ? Locks the main frame buffer size to the app author values specified by
// ? 'vid_orwidth' and 'vid_orheight' instead of resizing it to the windows
// ? size.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_lock", cCommon->Zero(),
  CB(cFboMain->SetLockViewport, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_MONITOR
// ? Specifies the monitor id to use. Use the 'mlist' console command to see
// ? possible values or just use -1 to let the operating system decide.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_monitor", "-1",
  CB(cDisplay->MonitorChanged, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_GAMMA
// ? Overrides the gamma level. The default is 1 which is to keep the desktop
// ? gamma level.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_gamma", cCommon->One(),
  CB(cDisplay->GammaChanged, GLfloat), TUFLOATSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_FSAA
// ? Enables full-scene anti-aliasing. Only needed to smooth the edges of
// ? textures if you frequently blit textures at an different angles other than
// ? 0, 90, 180 or 270 degrees.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_fsaa", "-1",
  CB(cDisplay->FsaaChanged, int), TINTEGERSAVE|CPOW2Z|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_BPP
// ? Set default bits-per-pixel. On Windows, the driver can decide properly
// ? what depth to use, but on others the bit depth is forced to RGBA161616 to
// ? resolve gradient banding issues but you can override that if you like.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_bpp",
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)
  cCommon->Zero(),                     // Win32 doesn't need forcing depth
#else
  "16",                                // For better bit-depth quality
#endif
/* ------------------------------------------------------------------------- */
  CB(cDisplay->SetForcedBitDepth, int), TUINTEGER|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_HIDPI
// ? Enables or disables HiDPI support.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_hidpi", cCommon->One(),
  CB(cDisplay->HiDPIChanged, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_STEREO
// ? Enables 3D glasses support.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_stereo", cCommon->Zero() ,
  CB(cDisplay->SetStereoMode, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_NOERRORS
// ? Sets GLFW_CONTEXT_NO_ERROR. Default is 0 (disabled).
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_noerrors", cCommon->Zero(),
  CB(cDisplay->SetNoErrorsMode, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_SRGB
// ? Enables SRGB colour space.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_srgb", cCommon->One(),
  CB(cDisplay->SRGBColourSpaceChanged, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_AUXBUFFERS
// ? Specified the number of auxiliary swap chain buffers to use. Specify 0
// ? for double-buffering, 1 for triple-buffering or -1 to let the OpenGL
// ? driver decide.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_auxbuffers", "-1",
  CB(cDisplay->AuxBuffersChanged, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_SIMPLEMATRIX
// ? Set to 0 to not maintain frame buffer aspect ratio, or 1 to allow the
// ? main frame buffer to stick to the users preferred aspect ratio (see
// ? 'vid_oraspmin' and 'vid_oraspmax).
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_simplematrix", cCommon->Zero(),
  CB(cFboMain->SetSimpleMatrix, bool), TBOOLEAN|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_TEXFILTER
// ? Sets the texture filter id and equates to the values of the 'Fbo.Filters'
// ? table.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_texfilter", "3",
  CB(cFboMain->SetFilter, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_VSYNC
// ? Enables vertical synchronisation which helps smooth the frame rate. Set to
// ? 0 if you don't care about the longevity of your GPU and make it draw as
// ? fast as possible, or 1 to keep it synchronised to your monitors vertical
// ? refresh rate. You can also use -1 to use adaptive sync, and 2 to half the
// ? refresh rate if you like.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_vsync", cCommon->One(),
  CB(cOgl->SetVSyncMode, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_GASWITCH
// ? Set to 0 (default) to disable MacOS graphics switching, or 1 to allow
// ? MacOS to switch between integral and dedicated graphics.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_gaswitch", cCommon->Zero(),
  CB(cDisplay->GraphicsSwitchingChanged, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_WIREFRAME
// ? Do not fill triangles with textures. Useless mainly.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_wireframe", cCommon->Zero(),
  CB(cOgl->SetPolygonMode, bool), TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_ORWIDTH
// ? Specifies the width of the main frame buffer.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_orwidth", "640",
  CB(cDisplay->SetOrthoWidth, GLfloat), TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_ORHEIGHT
// ? Specifies the height of the main frame buffer.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_orheight", "480",
  CB(cDisplay->SetOrthoHeight, GLfloat), TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_ORASPMIN
// ? Specifies the minmum aspect ratio allowed in the main frame buffer. For
// ? example, 1.333333 is 4:3 and 1.777778 is 16:9. Only applies when the
// ? 'vid_simplematrix' cvar is set to 0.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_oraspmin", "1.25",
  CB(cFboMain->SetMinOrtho, GLfloat), TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_ORASPMAX
// ? Specifies the maximum aspect ratio allowed in the main frame buffer. For
// ? example, 1.333333 is 4:3 and 1.777778 is 16:9. Only applies when the
// ? 'vid_simplematrix' cvar is set to 0.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_oraspmax", "1.777778",
  CB(cFboMain->SetMaxOrtho, GLfloat), TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_RFBO
// ? Pre-allocates space for the specified number of frame buffer names. By
// ? default we allocate enough room for the main frame buffer and the console
// ? frame buffer. There could be FPS issues initially if this value is set too
// ? low and many frame buffers are used. Only the app author needs to be
// ? concerned with this value and can only be set in the application manifest.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_rfbo", "2",
  CB(FboSetOrderReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_RFLOATS
// ? Pre-allocates buffer space for the specified number of GLfloats. There
// ? could be FPS issues initially if this value is set too low. Only the
// ? app author needs to be concerned with this value and can only be set in
// ? the application manifest.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_rfloats", "10000",
  CB(cFboMain->SetFloatReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_RCMDS
// ? Pre-allocates space for the specified number of OpenGL command structures.
// ? There could be FPS issues initially if this value is set too low. Only the
// ? app author needs to be concerned with this value and can only be set in
// ? the application manifest.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_rcmds", "1000",
  CB(cFboMain->SetCommandReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_RDTEX
// ? Pre-allocates the specified number of texture names to reserve in the
// ? pending texture names deletion list. Only the app author needs to be
// ? concerned with this value and can only be set in the application manifest.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_rdtex", "10",
  CB(cOgl->SetTexDListReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_RDFBO
// ? Pre-allocates the specified number of frame buffer names to reserve in the
// ? pending frame buffer names deletion list. Only the app author needs to be
// ? concerned with this value and can only be set in the application manifest.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_rdfbo", "10",
  CB(cOgl->SetFboDListReserve, size_t), TINTEGER|PSYSTEM|CUNSIGNED|PBOOT },
/* ------------------------------------------------------------------------- */
// ! VID_SSTYPE
// ? Specifies the screenshot type, see the command output of 'imgfmts' to see
// ? the image types supported and the id's for them. Only entries marked with
// ? the 'S' (saveable) token can be used.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_sstype", cCommon->Zero(),
  CB(cSShot->SetScreenShotType, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_SUBPIXROUND
// ? Specifies the type of sub-pixel rounding function on the main frame-buffer
// ? GLSL shader. Specify 0 for no sub-pixel rounding, 1 to use the floor()
// ? function, 2 to use the ceil() function, 3 to use the round() function or
// ? 4 to use the roundEven() function.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_subpixround", cCommon->Zero(),
  CB(SetSPRoundingMethod, size_t), TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! VID_QSHADER
// ? Specifies shader quality. Default is maximum.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_qshader", "3",
  CB(cOgl->SetQShaderHint, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_QLINE
// ? Specifies line quality. Default is maximum.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_qline", cCommon->Zero(),
  CB(cOgl->SetQLineHint, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_QPOLYGON
// ? Specifies polygon quality. Default is maximum.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_qpolygon", cCommon->Zero(),
  CB(cOgl->SetQPolygonHint, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! VID_QCOMPRESS
// ? Specifies texture compression quality. Default is maximum.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "vid_qcompress", "3",
  CB(cOgl->SetQCompressHint, size_t), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_ASPECT
// ? Force the window to have an aspect ratio. Specify as a floating point
// ? number. i.e. 16.9 for 16:9 or 4.3 for 4:3, etc. or 0 for numeric and/or
// ? denominator for freedom for the end user to resize at will.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_aspect", cCommon->Zero(),
  NoOp, TUFLOAT|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! WIN_BORDER
// ? Specifies if the window should have a titlebar and a border.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_border", cCommon->One(),
  CB(cDisplay->BorderChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_CLOSEABLE
// ? Specifies if the window is closable by the user.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_closeable", cCommon->One(),
  CB(cDisplay->CloseableChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_FLOATING
// ? Specifies to lock the visibility of the window, otherwise known as 'always
// ? on top' or 'Topmost'.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_floating", cCommon->Zero(),
  CB(cDisplay->FloatingChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_FOCUSED
// ? Specifies to automatically focus the window on creation.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_focused", cCommon->One(),
  CB(cDisplay->AutoFocusChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_HEIGHT
// ? Sets the initial height of the window. This value is saved to the
// ? persistence database when changed.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_height", cCommon->Zero(),
  CB(cDisplay->HeightChanged, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_HEIGHTMAX
// ? Sets the maximum height of the window. This is only changable at the
// ? application configuration file and is not saved to persistence database.
// ? The upper value is clamped by the GPU's maximum texture size which is
// ? normally 16384 on all modern GPU's. Specify 0 to use the GPU maximum.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_heightmax", cCommon->Zero(),
  NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! WIN_HEIGHTMIN
// ? Sets the minimum height of the window. This is only changable at the
// ? application configuration file and is not saved to persistence database.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_heightmin", cCommon->Zero(),
  NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! WIN_MAXIMISED
// ? Specify 1 to have the window automatically maximised on creation or 0
// ? to not. The default value is 0.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_maximised", cCommon->Zero(),
  CB(cDisplay->SetMaximisedMode, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_MINIMISEAUTO
// ? Specify 1 to have the window automatically minimise when it is not active
// ? or 0 to not. The default value is 1.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_minimiseauto", cCommon->One(),
  CB(cDisplay->AutoIconifyChanged, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_POSX
// ? Specify the default X position of the window. -2 means the centre of the
// ? screen and -1 means the default position. Any other value is the actual
// ? physical position on the screen. The default value is -2.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_posx", "-2",
  CB(cDisplay->SetXPosition, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_POSY
// ? Specify the default Y position of the window. -2 means the centre of the
// ? screen and -1 means the default position. Any other value is the actual
// ? physical position on the screen. The default value is -2.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_posy", "-2",
  CB(cDisplay->SetYPosition, int), TINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_SIZABLE
// ? Specify 1 if the window is allowed to be resized or 0 if not. The default
// ? value is 1.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_sizable", cCommon->One(),
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
{ CF_VIDEO, "win_thread",
  /* ----------------------------------------------------------------------- */
#if defined(WINDOWS)
  cCommon->One(),
#else
  cCommon->Zero(),                                 // Fix temporary bug in GLFW
#endif
  /* ----------------------------------------------------------------------- */
  NoOp, TBOOLEANSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_WIDTH
// ? Sets the initial width of the window. This value is saved to the
// ? persistence database when changed.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_width", cCommon->Zero(),
  CB(cDisplay->WidthChanged, int), TUINTEGERSAVE|PANY },
/* ------------------------------------------------------------------------- */
// ! WIN_WIDTHMAX
// ? Sets the maximum width of the window. This is only changable at the
// ? application configuration file and is not saved to persistence database.
// ? The upper value is clamped by the GPU's maximum texture size which is
// ? normally 16384 on all modern GPU's. Specify 0 to use the GPU maximum.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_widthmax", cCommon->Zero(),
  NoOp, TUINTEGER|PSYSTEM },
/* ------------------------------------------------------------------------- */
// ! WIN_WIDTHMIN
// ? Sets the minimum width of the window. This is only changable at the
// ? application configuration file and is not saved to persistence database.
/* ------------------------------------------------------------------------- */
{ CF_VIDEO, "win_widthmin", cCommon->Zero(),
  NoOp, TUINTEGER|PSYSTEM },
/* == Logging cvars ======================================================== */
// ! LOG_CREDITS
// ? Specifies to include credits in the log at startup.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "log_credits", cCommon->Zero(),
  CB(cCredits->CreditDumpList, bool), TBOOLEAN|PANY },
/* ------------------------------------------------------------------------- */
// ! LOG_DYLIBS
// ? Specifies to include shared libraries in the log at startup.
/* ------------------------------------------------------------------------- */
{ CF_NOTHING, "log_dylibs", cCommon->Zero(),
  CB(cSystem->DumpModuleList, bool), TBOOLEAN|PANY },
/* -- Undefines ------------------------------------------------------------ */
#undef CBSTR                           // Done with string function callback
#undef CB                              // Done with int function callback
/* ------------------------------------------------------------------------- */
} };                                   // End of module namespace
/* == EoF =========================================================== EoF == */
