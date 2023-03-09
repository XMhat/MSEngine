/* == LOG.HPP ============================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles logging of engine events which the user and     ## */
/* ## developer can read to help resolve issues.                          ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfLog {                      // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfFStream;             // Using fstream interface
using namespace IfClock;               // Using clock interface
using namespace IfCVarDef;             // Using cvardef interface
/* -- Log levels ----------------------------------------------------------- */
enum LHLevel                           // Log helper level flags
{ /* ----------------------------------------------------------------------- */
  LH_DISABLED,                         // Log messages are disabled
  LH_ERROR,                            // Log message is an error (exception)
  LH_WARNING,                          // Log message is a warning
  LH_INFO,                             // Log message is informational only
  LH_DEBUG,                            // Log message is for debugging
  LH_MAX,                              // Maximum log message level
};/* -- Log line typedefs -------------------------------------------------- */
struct LogLine { double dTime; LHLevel lhLevel; string strLine; };
typedef list<LogLine> LogLines;
/* == Log class ============================================================ */
static class Log :
  /* -- Base classes ------------------------------------------------------- */
  public LogLines,                     // Holds info about every log line
  private FStream,                     // Output log file if needed
  public ClockChrono<CoreClock>,       // Holds the current log time
  private mutex                        // Because logger needs thread safe
{ /* -- Macros ------------------------------------------------------------- */
  #define LWN(l,...) cLog->WriteStringSafe(l, Format(__VA_ARGS__));
  #define LW(l,...) if(cLog->HasLevel(l)) LWN(l, ## __VA_ARGS__)
  #define ILW(l,...) if(HasLevel(l)) WriteString(l, Format(__VA_ARGS__));
  /* -- Private variables -------------------------------------------------- */
  const string     strStdOut;          // Label for 'stdout'
  const string     strStdErr;          // Label for 'stderr'
  atomic<LHLevel>  lhLevel;            // Log helper level for this instance
  size_t           stMaximum;          // Maximum log lines
  bool             bInitialised;       // Log cvar has been initialised?
  /* ----------------------------------------------------------------------- */
  void ReserveLines(const size_t stLines)
  { // Calculate total liens
    const size_t stTotal = size() + stLines;
    // If writing this many lines would fit in the log, then the log does not
    // need pruning.
    if(stTotal <= stMaximum) return;
    // If too many lines would be written? Just clear the log
    if(stLines >= stMaximum) return clear();
    // Erase enough lines for the new ones to fit
    erase(begin(), next(begin(), static_cast<ssize_t>(stTotal - stMaximum)));
  }
  /* ----------------------------------------------------------------------- */
  void FlushLog(void)
  { // Ignore if file not opened or there is nothing to write
    if(empty() || FStreamClosed()) return;
    // Get start of log
    LogLines::const_iterator llciItem{ cbegin() };
    // Repeat...
    do
    { // Get reference to line
      const LogLine &llLine = *llciItem;
      // Write stored line and if succeeded
      if(FStreamWriteString(Format("[$$$] $\n", fixed,
        setprecision(6), llLine.dTime, llLine.strLine)))
      { // Flush the line to file straight away and continue if succeeded
        if(FStreamFlush()) continue;
        // Flush failed so close file
        FStreamClose();
        // Write closure reason
        ILW(LH_ERROR, "Log file closed (flush error: $)!", LocalError());
      } // Write string failed?
      else
      { // Close file
        FStreamClose();
        // Write closure reason
        ILW(LH_ERROR, "Log file closed (write error: $)!", LocalError());
      } // Erase the lines we actually wrote. Never erase the first line.
      erase(begin(), llciItem);
      // Do not write anymore lines
      return;
    } // ... until all lines written
    while(++llciItem != end());
    // We wrote all the lines
    clear();
  }
  /* -- Write lines to log ------------------------------------------------- */
  void WriteLines(const LHLevel lhL, const TokenList &tLines) noexcept(true)
  { // Ignore if no lines
    if(tLines.empty()) return;
    // Prune log to fit this many lines
    ReserveLines(tLines.size());
    // For each line, print to log. Note that I tried using string
    // appending for this and it turned out to be almost twice as slow as
    // using formatstring due to the fact that less memory management
    // is required!
    for(const string &sL : tLines)
      push_back({ CCDeltaToDouble(), lhL, move(sL) });
    // Write lines to log
    FlushLog();
  }
  /* -- Write string to log. Line feed creates multiple lines -------------- */
  void WriteString(const LHLevel lhL, const string &strL) noexcept(true)
    { WriteLines(lhL, { strL, "\n", stMaximum }); }
  void WriteString(const LHLevel lhL, string &&strL) noexcept(true)
    { WriteLines(lhL, { move(strL), "\n", stMaximum }); }
  /* ----------------------------------------------------------------------- */
  void WriteString(const string &strL) { WriteString(LH_DISABLED, strL); }
  /* ----------------------------------------------------------------------- */
  void DeInit(void)
  { // Bail if initialised
    if(FStreamClosed()) return;
    // Log file closure
    WriteString("Log file closed.");
    // Flush all remaining strings
    FlushLog();
    // Done
    FStreamClose();
  }
  /* -- Convert log level to a string -------------------------------------- */
  const string &LogLevelToString(const LHLevel lhId)
  { // Lookup table of log levels
    static const IdList<LH_MAX> ilLevels{{
      "Disabled", "Error", "Warning", "Info", "Debug"
    }};
    // Return looked up string
    return ilLevels.Get(lhId);
  }
  /* -- Safe access to members ------------------------------------- */ public:
  CVarReturn SetLevel(const unsigned int uiLevel)
  { // Deny if invalid level
    if(uiLevel >= LH_MAX) return DENY;
    // Set new logging state
    const LHLevel lhOldLevel = lhLevel;
    lhLevel = static_cast<LHLevel>(uiLevel);
    // Report state, we could disable logging so we should force report it
    WriteStringSafe(LH_DEBUG,
      Format("Log change verbosity from $ ($) to $ ($).",
        LogLevelToString(lhOldLevel), lhOldLevel,
        LogLevelToString(lhLevel.load()), lhLevel.load()));
    // Success
    return ACCEPT;
  }
  /* ----------------------------------------------------------------------- */
  size_t Clear(void)
  { // Get num log lines for returning
    const size_t stSize = size();
    // Clear the log
    clear();
    // Return number of lines cleared
    return stSize;
  }
  /* ----------------------------------------------------------------------- */
  bool HasLevel(const LHLevel lhReq) const { return lhReq <= lhLevel; }
  /* ----------------------------------------------------------------------- */
  LHLevel GetLevel(void) const { return lhLevel; }
  /* ----------------------------------------------------------------------- */
  mutex &GetMutex(void) { return *this; }
  bool IsRedirectedToDevice(void)
    { const LockGuard lgLogSync{ GetMutex() }; return FStreamIsHandleStd(); }
  bool OpenedSafe(void)
    { const LockGuard lgLogSync{ GetMutex() }; return FStreamOpened(); }
  void DeInitSafe(void)
    { const LockGuard lgLogSync{ GetMutex() }; DeInit(); }
  const string GetNameSafe(void)
    { const LockGuard lgLogSync{ GetMutex() }; return IdentGet(); }
  /* ----------------------------------------------------------------------- */
  void WriteStringSafe(const LHLevel lhL, const string& strLine)
    { const LockGuard lgLogSync{ GetMutex() }; WriteString(lhL, strLine); }
  /* -- Return buffer lines for debugger ----------------------------------- */
  void GetBufferLines(ostringstream &osS)
  { // Gain exclusive access to log lines
    const LockGuard lgLogSync{ GetMutex() };
    // For each log entry, write the line to the buffer
    for(const LogLine &llLine : *this) osS
      << '[' << fixed << setprecision(6) << llLine.dTime << "] "
      << llLine.strLine << '\n';
  }
  /* -- Initialise log to built-in standard output ------------------------- */
  void Init(FILE*const fpDevice, const string &strLabel)
  { // Set device
    FStreamSetHandle(fpDevice);
    // Set name
    IdentSet(strLabel);
    // Write log ifle
    ILW(LH_INFO, "Logging to standard output '$'.", IdentGet());
  }
  /* ----------------------------------------------------------------------- */
  bool Init(const string &strFN)
  { // If already opened or open/create log failed? Ignore. We're using
    // trusted here because this is only changable with the cvar and the cvar
    // has already checked that this filename is valid.
    if(FStreamOpen(strFN, FStream::FM_W_T)) return false;
    // Write log file
    ILW(LH_INFO, "Log file is '$'.", IdentGet());
    // Success
    return true;
  }
  /* -- Conlib callback function for APP_LOG variable ---------------------- */
  CVarReturn LogFileModified(const string &strFN, string &strCV)
  { // Lock mutex
    const LockGuard lgLogSync{ GetMutex() };
    // Close log if opened
    if(FStreamOpened()) DeInit();
    // Has cvar change not been triggered yet?
    if(!bInitialised)
    { // Cvar has now been initialised so this condition cannot re-execute
      bInitialised = true;
      // Check for special character
      switch(strFN.length())
      { // Empty? Ignore
        case 0: return ACCEPT;
        // One character? Compare it...
        case 1: switch(strFN.front())
        { // Check for requested use of stderr or stdout
          case '!': Init(stderr, strStdErr); return ACCEPT;
          case '-': Init(stdout, strStdOut); return ACCEPT;
          // Anything else ignore and open the file normally
          default: break;
        } // Anything else just break;
        default: break;
      }
    } // Just check if empty and return acceptance if so
    else if(strFN.empty()) return ACCEPT;
    // Create new filename with log extension and try to create it
    if(!Init(Append(strFN, "." LOG_EXTENSION))) return DENY;
    // Success so update the actual filename
    strCV = IdentGet();
    // Success
    return ACCEPT_HANDLED;
  }
  /* -- Conlib callback function for APP_LOGLINES variable ----------------- */
  CVarReturn LogLinesModified(const size_t stL)
  { // Must have at least one line and lets also set a safe maximum
    if(stL < 1 || stL > 1000000 || stL > max_size()) return DENY;
    // Prevent use of variables off the main thread
    const LockGuard lgLogSync{ GetMutex() };
    // Current size is over the new maximum? Trim the oldest entries out
    if(size() > stMaximum)
      erase(begin(), next(begin(), static_cast<ssize_t>(size() - stMaximum)));
    // Set new maximum
    stMaximum = stL;
    // Value has been accepted
    return ACCEPT;
  }
  /* -- Constructor -------------------------------------------------------- */
  Log(void) :
    /* -- Initialisation of members ---------------------------------------- */
    strStdOut{ "/dev/stdout" },        // Initialise display label for stdout
    strStdErr{ "/dev/stderr" },        // Initialise display label for stderr
    lhLevel{ LH_DEBUG },               // Initialise default level
    stMaximum(1000),                   // Initialise maximum output lines
    bInitialised(false)                // CVar has not been initialised
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Log, DeInitSafe());
  /* -- Macros ------------------------------------------------------------- */
  DELETECOPYCTORS(Log);                // Do not need defaults
  /* -- Done with this, it's only for internal use ------------------------- */
  #undef ILW
  /* -- End ---------------------------------------------------------------- */
} *cLog = nullptr;                     // Pointer to static class
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
