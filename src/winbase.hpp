/* == WINBASE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is a Windows specific module that handles unhandled exceptions ## */
/* ## and writes debugging information to disk to help resolve bugs.      ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Includes ------------------------------------------------------------- */
using namespace IfStat;                // Using stat interface
/* == We'll put all these calls in a namespace ============================= */
class SysBase :                        // Safe exception handler namespace
  /* -- Base classes ------------------------------------------------------- */
  public Ident                         // Mutex name
{ /* -- Custom exceptions -------------------------------------------------- */
  #define EXCEPTION_APT     0xF0000001 // Abornmal program termination
  #define EXCEPTION_ISA     0xF0000002 // Illegal storage access
  #define EXCEPTION_FPE     0xF0000003 // Floating point exception
  /* -- Private variables -------------------------------------------------- */
  HWND             hwndWindow;         // Main window handle being used
  HANDLE           hMutex;             // Global mutex handle
  ostringstream    osS;                // Output stringstream
  /* ----------------------------------------------------------------------- */
  const UINT       uiOldErrorMode;            // Old error mode
  const _crt_signal_t fcbAbortCallback,       // Saved abort callback
                   fcbIllegalStorageAccess,   // " illegal storage access cb
                   fcbFloatingPointException; // " float point exception cb
  /* ----------------------------------------------------------------------- */
  void DeleteGlobalMutex(void)
  { // Ignore if not initialised
    if(hMutex) return;
    // Close the handle and log if failed
    if(!CloseHandle(hMutex))
      LW(LH_WARNING, "SysMutex failed to close mutex handle '$'! $.",
        IdentGet(), SysError());
  }
  /* ----------------------------------------------------------------------- */
  static BOOL WINAPI EnumWindowsProc(HWND hH, LPARAM lP)
  { // Get title of window and cancel if empty
    wstring wstrT(GetWindowTextLength(hH), 0);
    if(!GetWindowText(hH, const_cast<LPWSTR>(wstrT.c_str()),
      static_cast<DWORD>(wstrT.capacity())))
        return TRUE;
    // If there is not enough characters in this windows title or the title
    // and requested window title's contents do not match? Ignore this window!
    const wstring &wstrN = *reinterpret_cast<wstring*>(lP);
    if(wstrT.length() < wstrN.length() ||
      memcmp(wstrN.data(), wstrT.data(), wstrN.length()*sizeof(wchar_t)))
        return TRUE;
    // We found the window
    LW(LH_DEBUG, "- Found window handle at $$.\n"
                 "- Window name is '$'.",
      hex, reinterpret_cast<void*>(hH), WS16toUTF(wstrT));
    // First try showing the window and if successful?
    if(ShowWindow(hH, SW_RESTORE|SW_SHOWNORMAL))
    { // Log the successful command
      LW(LH_DEBUG, "- Show window request was successful.");
    } // Showing the window failed?
    else
    { // Log the failure with reason
      LW(LH_WARNING, "- Show window request failed: $!", SysError());
    } // Then try setting it as a foreground window and if succeeded?
    if(SetForegroundWindow(hH))
    { // Log the successful command
      LW(LH_DEBUG, "- Set fg window request was successful.");
    } // Setting foreground window failed?
    else
    { // Log the failure with reason
      LW(LH_WARNING, "- Set fg window request failed: $!", SysError());
    }// Then try setting the focus of the window and if succeeded?
    if(SetFocus(hH) || SysIsErrorCode())
    { // Log the successful command
      LW(LH_DEBUG, "- Set focus request was successful.");
    } // Setting focus failed?
    else
    { // Log the failure with reason
      LW(LH_WARNING, "- Set focus request failed: $!", SysError());
    } // Reset error so it's not seen as the result
    SetLastError(0);
    // Don't look for any more windows
    return FALSE;
  }
  /* == Get environment ==================================================== */
  void SEHDumpEnvironment(void) try
  { // Formatted data
    Statistic tD;
    tD.Header("Variable", false).Header("Value", false);
    // Get environment strings and if we have them?
    if(wchar_t*const wcpE = GetEnvironmentStrings())
    { // Iterate through the environment lines
      for(wchar_t *wcpEP = wcpE, *wcpN = wcpE + wcslen(wcpEP) + 1;
                  *wcpEP;
                   wcpEP = wcpN,  wcpN = wcpEP + wcslen(wcpEP) + 1)
      { // Find value and if we found it?
        if(wchar_t *wcpV = wcschr(wcpEP, '='))
        { // Remove equals
          *wcpV = 0;
          // Add variable and value
          tD.DataW(wcpEP).DataW(++wcpV);
          // Done
          continue;
        } // Not found. Add line as value
        tD.Data().DataW(wcpEP);
      } // Free the environment strings
      FreeEnvironmentStrings(wcpE);
    } // Return formatted data
    tD.Finish(osS);
  } // Shouldn't happen but just incase
  catch(const exception &e) { osS << e.what(); }
  /* == Get memory information ============================================= */
  void SEHDumpMemoryStatus(void) try
  { // Get process memory info
    PROCESS_MEMORY_COUNTERS pD;
    pD.cb = sizeof(pD);
    GetProcessMemoryInfo(GetCurrentProcess(), &pD, sizeof(pD));
    // Get memory info
    MEMORYSTATUSEX mD;
    mD.dwLength = sizeof(mD);
    GlobalMemoryStatusEx(&mD);
    // Process, format and return data
    Statistic()
      .Header("Type", false)
      .Header("Total")
      .Header("Free")
      .Header("Used")
      // Process memory
      .Data("Process")
      .DataB(pD.PeakWorkingSetSize, 2)
      .Data()
      .DataB(pD.WorkingSetSize, 2)
      // Physical memory
      .Data("Physical")
      .DataB(mD.ullTotalPhys, 2)
      .DataB(mD.ullAvailPhys, 2)
      .DataB(mD.ullTotalPhys - mD.ullAvailPhys, 2)
      // Paged memory
      .Data("Page")
      .DataB(mD.ullTotalPageFile, 2)
      .DataB(mD.ullAvailPageFile, 2)
      .DataB(mD.ullTotalPageFile - mD.ullAvailPageFile, 2)
      // Virtual memory
      .Data("Virtual")
      .DataB(mD.ullTotalVirtual, 2)
      .DataB(mD.ullAvailVirtual, 2)
      .DataB(mD.ullTotalVirtual - mD.ullAvailVirtual, 2)
      // Extended virtual memory
      .Data("XVirtual")
      .Data()
      .DataB(mD.ullAvailExtendedVirtual, 2)
      .DataN(int(mD.dwMemoryLoad) + '%').Finish(osS);
  } // Shouldn't happen but just incase
  catch(const exception &e) { osS << e.what(); }
  /* == Get executable filename ============================================ */
  const wstring SEHGetExecutableFileNameWithoutExtension(void)
  { // Storage for executable and crash log file name
    wstring wstrExe; wstrExe.resize(MAX_PATH);
    // Get executable file name
    wstrExe.resize(GetModuleFileName(nullptr,
      const_cast<wchar_t*>(wstrExe.c_str()), MAX_PATH));
    // Remove extension if we can
    if(wstrExe.length() >= 4 && wstrExe[wstrExe.length() - 4] == '.')
      wstrExe.resize(wstrExe.length() - 4);
    // Return result
    return wstrExe;
  }
  /* == Dump exception memory addresses ==================================== */
  void SEHDumpExceptionMemoryAddresses(const EXCEPTION_RECORD &erData) try
  { // Bail if exception code does not support this
    switch(erData.ExceptionCode)
    { // The first element of the array contains a read-write flag that
      // indicates the type of operation that caused the access violation. If
      // this value is zero, the thread attempted to read the inaccessible
      // data. If this value is 1, the thread attempted to write to an
      // inaccessible address. If this value is 8, the thread causes a
      // user-mode data execution prevention (DEP) violation.
      // The second array element specifies the virtual address of the
      // inaccessible data.
      // The third array element specifies the underlying NTSTATUS code that
      // resulted in the exception.
      case EXCEPTION_IN_PAGE_ERROR:
      // The first element of the array contains a read-write flag that
      // indicates the type of operation that caused the access violation. If
      // this value is zero, the thread attempted to read the inaccessible
      // data. If this value is 1, the thread attempted to write to an
      // inaccessible address. If this value is 8, the thread causes a
      // user-mode data execution prevention (DEP) violation.
      // The second array element specifies the virtual address of the
      // inaccessible data.
      case EXCEPTION_ACCESS_VIOLATION: break;
      // No other codes have exception information to show
      default: return;
    } // For each parameter. Clamping just incase
    for(DWORD dwParam = 0,
      dwMax = Minimum(erData.NumberParameters, EXCEPTION_MAXIMUM_PARAMETERS);
              dwParam < dwMax;
              dwParam += 2)
    { // Write where the access occured
      osS << "\r\nThe memory at 0x"
          << reinterpret_cast<void*>(erData.ExceptionInformation[dwParam+1])
          << " could not be ";
      // Write the action attempted
      switch(const
  #ifdef _M_AMD64
        DWORD64
  #else
        DWORD
  #endif
        &dwAccess = erData.ExceptionInformation[dwParam])
      { // Read, written or executed?
        case 0: osS << "read."; break;
        case 1: osS << "written."; break;
        case 8: osS << "executed."; break;
        // Unknown? (Shouldn't get here)
        default: osS << "0x" << hex << dwAccess << '.'; break;
      } // Was an in-page error? Move the parameter count onwards by one and
      // plus two to get the third element. The next iteration will push by
      // two.
      if(erData.ExceptionCode == EXCEPTION_IN_PAGE_ERROR)
        osS << "\r\nAn in page error occured with code 0x"
            << erData.ExceptionInformation[(dwParam++)+2]
            << '!';
    }
  } // Shouldn't happen but just incase
  catch(const exception &e) { osS << e.what(); }
  /* == Dump registers to string =========================================== */
  void SEHDumpRegisters(const CONTEXT *pcData) try
  { // Invalid context?
    if(!pcData)
      throw runtime_error{ "No registers when context is null pointer!" };
    // Get context from exception record
    const CONTEXT &cData = *pcData;
    // Helper macros
    #define PUSHINT(id,c,x,e) id "=" << setw(c) << cData.x << e
    #define D64(id,x,e)       PUSHINT(id,16,x,e)
    #define D32(id,x,e)       PUSHINT(id,8,x,e)
    #define D16(id,x,e)       PUSHINT(id,4,x,e)
    #define D32X(id,x,e)      id "="\
      << setw(8) << *reinterpret_cast<const uint32_t*>(&cData.x) << e
    #define D128X(id,x,e)     id "="\
      << setw(16) << *reinterpret_cast<const uint64_t*>(&cData.x)\
      << setw(16) << *(reinterpret_cast<const uint64_t*>(&cData.x)+1) << e
    #define SPC               "  "
    #define CRLF              "\r\n"
    // Return registers
  #ifdef _M_AMD64
    // Write basic registers
    osS << hex << setfill('0') <<
      D64("Rax", Rax, SPC)  D64("Rbx", Rbx, SPC)  D64("Rcx", Rcx, CRLF)
      D64("Rdx", Rdx, SPC)  D64("Rsp", Rsp, SPC)  D64("Rbp", Rbp, CRLF)
      D64("Rip", Rip, SPC)  D64("Rsi", Rsi, SPC)  D64("Rdi", Rdi, CRLF)
      CRLF
      D16("SegCs", SegCs, SPC)  D16("SegDs", SegDs, SPC)
      D16("SegEs", SegEs, SPC)  D16("SegFs", SegFs, SPC)
      D16("SegGs", SegGs, SPC)  D16("SegSs", SegSs, CRLF)
      CRLF
      D32("CFlags", ContextFlags, SPC)  D32("MxCsr", MxCsr, SPC)
      D32("EFlags", EFlags, CRLF)
      CRLF
      D64("P1H", P1Home, SPC)  D64("P2H", P2Home, SPC)
      D64("P3H", P3Home, CRLF) D64("P4H", P4Home, SPC)
      D64("P5H", P5Home, SPC)  D64("P6H", P6Home, CRLF)
      CRLF
      D64("Dr0", Dr0, SPC)  D64("Dr1", Dr1, SPC)  D64("Dr2", Dr2, CRLF)
      D64("Dr3", Dr3, SPC)  D64("Dr6", Dr6, SPC)  D64("Dr7", Dr7, CRLF)
      CRLF
      D64("R08", R8,  SPC)  D64("R09", R9,  SPC)  D64("R10", R10, CRLF)
      D64("R11", R11, SPC)  D64("R12", R12, SPC)  D64("R13", R13, CRLF)
      D64("R14", R14, SPC)  D64("R15", R15, CRLF)
      CRLF
      D64("VCon", VectorControl, SPC)     D64("DCon", DebugControl, SPC)
      D64("LBtR", LastBranchToRip, CRLF)  D64("LBfR", LastBranchFromRip, SPC)
      D64("LEtR", LastExceptionToRip, SPC)
      D64("LEfR", LastExceptionFromRip, CRLF)
      CRLF;
    // Write floating point header state
    for(size_t stQuad = 0; stQuad < 2; stQuad += 2)
      osS << D128X("XmmH" << dec << stQuad << hex <<,
        Header[stQuad], SPC)
          << D128X("XmmH" << dec << (stQuad+1) << hex <<,
        Header[stQuad+1], CRLF);
    // Write floating point legacy state
    for(size_t stQuad = 0; stQuad < 8; stQuad += 2)
      osS << D128X("XmmL" << dec << stQuad << hex <<,
        Legacy[stQuad], SPC)
          << D128X("XmmL" << dec << (stQuad+1) << hex <<,
        Legacy[stQuad+1], CRLF);
    // Write floating point state
    #define XMM(x,e) << D128X("Xmm" << setw(2) << dec << x\
      << hex <<, Xmm ## x, e)
    osS XMM( 0,SPC)  XMM( 1,CRLF) XMM( 2,SPC)  XMM( 3,CRLF) XMM( 4,SPC)
        XMM( 5,CRLF) XMM( 6,SPC)  XMM( 7,CRLF) XMM( 8,SPC)  XMM( 9,CRLF)
        XMM(10,SPC)  XMM(11,CRLF) XMM(12,SPC)  XMM(13,CRLF) XMM(14,SPC)
        XMM(15,CRLF) CRLF;
    #undef XMM
    // Write vector state
    for(size_t stQuad = 0; stQuad < 26; stQuad += 2)
      osS << D128X("Vec" << setw(2) << dec << stQuad
          << hex <<, VectorRegister[stQuad], SPC)
          << D128X("Vec" << setw(2) << dec << (stQuad+1)
          << hex <<, VectorRegister[stQuad+1], CRLF);
  #else
    // Write basic registers
    osS << hex << setfill('0') <<
      D32("Eax", Eax, SPC)   D32("Ebx", Ebx, SPC)  D32("Ecx", Ecx, SPC)
      D32("Edx", Edx, CRLF)  D32("Esp", Esp, SPC)  D32("Ebp", Ebp, SPC)
      D32("Esi", Esi, SPC)   D32("Edi", Edi, SPC)  D32("Eip", Eip, CRLF)
      CRLF
      D32("SegCs", SegCs, SPC)   D32("SegDs", SegDs, SPC)
      D32("SegEs", SegEs, CRLF)  D32("SegFs", SegFs, SPC)
      D32("SegGs", SegGs, SPC)   D32("SegSs", SegSs, CRLF)
      CRLF
      D32("CFlags", ContextFlags, SPC)  D32("EFlags", EFlags, CRLF)
      CRLF
      D32("Dr0", Dr0, SPC)  D32("Dr1", Dr1, SPC)  D32("Dr2", Dr2, CRLF)
      D32("Dr3", Dr3, SPC)  D32("Dr6", Dr6, SPC)  D32("Dr7", Dr7, CRLF)
      CRLF
      D32("FCW", FloatSave.ControlWord, SPC)
      D32("FSW", FloatSave.StatusWord, SPC)
      D32("FTW", FloatSave.TagWord, SPC)
      D32("FES", FloatSave.ErrorSelector, CRLF)
      D32("FDO", FloatSave.DataOffset, SPC)
      D32("FDS", FloatSave.DataSelector, SPC)
      D32("FNS", FloatSave.Spare0, CRLF)
      CRLF;
    // Write floating point state
    for(size_t stI = 0, stY = 0, stZ = 5 % WOW64_SIZE_OF_80387_REGISTERS;
               stY < WOW64_SIZE_OF_80387_REGISTERS;
               stY += sizeof(DWORD)*5)
      for(size_t stX = 0; stX < stZ; ++stX, ++stI)
        osS << D32X("FRA" << setw(2) << dec << stI << hex <<,
          FloatSave.RegisterArea[stY+(stX*sizeof(DWORD))],
            (stX == 4 ? CRLF : SPC));
    osS << CRLF;
    // Write extended registers state
    for(size_t stI = 0, stY = 0, stZ = 5 % WOW64_MAXIMUM_SUPPORTED_EXTENSION;
               stY < WOW64_MAXIMUM_SUPPORTED_EXTENSION;
               stY += sizeof(DWORD)*5)
      for(size_t stX = 0; stX < stZ; ++stX, ++stI)
        osS << D32X("ER" << setw(3) << dec << stI << hex <<,
          ExtendedRegisters[stY+(stX*sizeof(DWORD))], (stX == 4 ? CRLF : SPC));
  #endif
    // Set fill back to space
    osS << setfill(' ');
    // Done with helper macros
    #undef CRLF
    #undef SPC
    #undef D128
    #undef D16
    #undef D32
    #undef D64
    #undef PUSHINT
  } // Shouldn't happen but just incase
  catch(const exception &e) { osS << e.what(); }
  /* == Perform process dump =============================================== */
  void SEHProcessDump(void) try
  { // Data storage
    Statistic tData;
    tData.Header("Name", false).Header("Pid").Header("PPid").Header("Thr")
         .Header("Pri").Header("Aff").Header("Version", false)
         .Header("Description", false).Header("Vendor", false)
         .Header("Path", false);
    // Storage for filename
    wstring wstrFN; wstrFN.resize(MAX_PATH);
    // Show modules
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // Capture exceptions so we can clean up the snapshot handle
    try
    { // Process info
      PROCESSENTRY32 pedData;
      pedData.dwSize = sizeof(pedData);
      // Get first item and iterate through each process
      if(Process32First(hSnapshot, &pedData) == TRUE) do
      { // Add pid, parent pid, thread count and name
        tData.DataW(pedData.szExeFile)
             .DataN(pedData.th32ProcessID)
             .DataN(pedData.th32ParentProcessID)
             .DataN(pedData.cntThreads)
             .DataN(pedData.pcPriClassBase);
        // Open Process. This will fail on some processes, but not all
        HMODULE hProcess = reinterpret_cast<HMODULE>(OpenProcess(
          PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE,
          pedData.th32ProcessID));
        // If failed?
        if(!hProcess)
        { // Push no affinity, blank version, error as description,
          // error number in vendor, reason for error as filename.
          tData.DataN(0)
               .Data("N/A")
               .Data("OpenProcess() failed!")
               .Data(Append("Error ", SysErrorCode()))
               .Data(SysError());
          // Next process
          continue;
        } // Capture exceptions so we can clean up the snapshot handle
        try
        { // Get affinity mask and if failed
          DWORD_PTR dwMask, dwSys;
          tData.DataH(GetProcessAffinityMask(hProcess,
            &dwMask, &dwSys) ? dwMask : 0);
          // Get module file name and if succeeded?
          if(GetModuleFileNameEx(hProcess, nullptr,
            const_cast<LPWSTR>(wstrFN.c_str()), MAX_PATH))
          { // Get version information
            const SysModuleData vD{ move(SysModule(WS16toUTF(wstrFN))) };
            // Push version, description vendor and filename (use .c_str())
            tData.Data(Format("$.$.$.$",
              vD.GetMajor(), vD.GetMinor(), vD.GetRevision(), vD.GetBuild()))
                 .Data(move(vD.GetDesc()))
                 .Data(move(vD.GetVendor()))
                 .DataW(wstrFN);
          } // Push blank field, error as description, number and reason
          else tData.Data()
                    .Data("GetModuleFileNameEx() failed!")
                    .Data(Append("Error ", SysErrorCode()))
                    .Data(SysError());
          // Done with process handle
          CloseHandle(hProcess);
        } // exception occured?
        catch(const exception &)
        { // Close the process handle
          CloseHandle(hProcess);
          // Rethrow the exception
          throw;
        }
      } // Continue if there is more
      while(Process32Next(hSnapshot, &pedData) == TRUE);
      // Done with snapshot
      CloseHandle(hSnapshot);
    } // exception occured?
    catch(const exception &)
    { // Close the snapshot handle
      CloseHandle(hSnapshot);
      // Rethrow the exception
      throw;
    } // Build output into string stream
    tData.Finish(osS);
  } // Shouldn't happen but just incase
  catch(const exception &e) { osS << e.what(); }
  /* == Perform module dump ================================================ */
  void SEHModuleDump(void) try
  { // Data storage
    Statistic tD;
    // Prepare formatted data
    tD.Header("Description", false).Header("Version", false)
      .Header("Vendor", false).Header("Module", false);
    // Show modules
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    // Capture exceptions so we can close the handle
    try
    { // Module info
      MODULEENTRY32 medData;
      medData.dwSize = sizeof(medData);
      // Get first module and iterate each one
      if(Module32First(hSnapshot, &medData) == TRUE) do
      { // Get module information
        const SysModuleData vD{ move(SysModule(S16toUTF(medData.szExePath))) };
        // Print data
        tD.Data(vD.GetDesc()).Data(vD.GetVersion())
          .Data(vD.GetVendor()).DataW(medData.szExePath);
      } // Continue if there is more
      while(Module32Next(hSnapshot, &medData));
      // Done with handle
      CloseHandle(hSnapshot);
    } // If exception occured?
    catch(const exception&)
    { // Close handle
      CloseHandle(hSnapshot);
      // Rethrow
      throw;
    } // Build output into string stream
    tD.Finish(osS);
  } // Shouldn't happen but just incase
  catch(const exception &e) { osS << e.what(); }
  /* == Perform stack dump ================================================= */
  void SEHStackDump(const HANDLE hProcess, const HANDLE hThread,
    const CONTEXT *pcData) try
  { // Ignore if no context
    if(!pcData)
      throw runtime_error{ "No stacktrace when context is a null pointer!" };
    // Initialise symbol reader and if succeeded?
    if(SymInitialize(hProcess, nullptr, TRUE))
    { // Auto clean-up symbol table
      const unique_ptr<void, function<decltype(SymCleanup)>>
        uProcess{ hProcess, SymCleanup };
      // Get context from exception record
      const CONTEXT &cData = *pcData;
      // Initialise stack frame structure
      STACKFRAME sfData{};
      sfData.AddrReturn = sfData.AddrStack = sfData.AddrBStore =
        { 0, 0, AddrModeFlat };
     #ifdef _M_AMD64                        // Using AMD64/INTEL64? (64-bit)
     # define IMAGE_FILE_MACHINE IMAGE_FILE_MACHINE_AMD64
     # define ADDR_PC    cData.Rip
     # define ADDR_FRAME cData.Rbp
     #else
     # define IMAGE_FILE_MACHINE IMAGE_FILE_MACHINE_I386
     # define ADDR_PC    cData.Eip
     # define ADDR_FRAME cData.Ebp
     #endif
      // Set register pointers
      sfData.AddrPC = { ADDR_PC, 0, AddrModeFlat };
      sfData.AddrFrame = { ADDR_FRAME, 0, AddrModeFlat };
      // Set sym options
      SymSetOptions((SymGetOptions() & ~SYMOPT_UNDNAME) | SYMOPT_LOAD_LINES);
      // Number of functions
      size_t stFunctions = 0;
      // Walk the stack
      while(StackWalk(IMAGE_FILE_MACHINE, hProcess, hThread, &sfData,
        ToNonConstCast<PVOID>(&cData), nullptr, SymFunctionTableAccess,
        SymGetModuleBase, nullptr))
      { // Add function number
        osS << stFunctions++ << ": ";
        // Is a null address? Ignore it
        if(!sfData.AddrPC.Offset)
        { // Ignore it so goto next function
          osS << "<Null Address Pointer>\r\n";
          continue;
        } // Get module name and if succeeded?
        IMAGEHLP_MODULE ihmMod;
        ihmMod.SizeOfStruct = sizeof(ihmMod);
        if(SymGetModuleInfo(hProcess, sfData.AddrPC.Offset, &ihmMod))
          osS << S16toUTF(ihmMod.ModuleName) << '!';
        // Failed so write reason
        else osS << "<SGMI:" << SysError() << ">!";
        // Buffer for symbol name
        string strName; strName.resize(MAX_PATH);
        DWORD_PTR dwOffsetFromSym;
        // Holds the memory for the symbol structure
        Memory aStruct{ 4096, true };
        PIMAGEHLP_SYMBOL ihsData = aStruct.Ptr<IMAGEHLP_SYMBOL>();
        ihsData->SizeOfStruct = aStruct.Size<DWORD>();
        ihsData->MaxNameLength =
          static_cast<DWORD>(aStruct.Size() - sizeof(IMAGEHLP_SYMBOL));
        // Retreive function and if succeeded?
        if(SymGetSymFromAddr(hProcess, sfData.AddrPC.Offset, &dwOffsetFromSym,
          ihsData))
        { // Get function name
          strName.resize(UnDecorateSymbolName(ihsData->Name,
            ToNonConstCast<PSTR>(strName.data()),
            static_cast<DWORD>(strName.length()), UNDNAME_COMPLETE));
          // Put in output string
          osS << strName << '@';
        } // Failed so record reason why that couldn't be obtained
        else switch(const DWORD dwCode = SysErrorCode<DWORD>())
        { // Invalid address
          case ERROR_INVALID_ADDRESS:
          // Success? (This happens on Wine for some reason)
          case ERROR_SUCCESS:
            osS << "0x" << hex << sfData.AddrPC.Offset << '@';
            break;
          // Any other code
          default:
            osS << "<SGSFA:" << SysError(dwCode) << ">@";
            break;
        } // Get source file and line info and if succeded?
        IMAGEHLP_LINE ihlLine;
        ihlLine.SizeOfStruct = sizeof(ihlLine);
        if(SymGetLineFromAddr(hProcess, sfData.AddrPC.Offset,
          reinterpret_cast<PDWORD>(&dwOffsetFromSym), &ihlLine))
            osS << S16toUTF(ihlLine.FileName) << ':'
                << ihlLine.LineNumber << "\r\n";
        // Failed so record reason why that couldn't be obtained
        else switch(const DWORD dwCode = SysErrorCode<DWORD>())
        { // Invalid address
          case ERROR_INVALID_ADDRESS:
            osS << "<No Source Information>\r\n"; break;
          // Success? (This happens on Wine for some reason)
          case ERROR_SUCCESS: osS << "<Unavailable>\r\n"; break;
          // Any other code
          default: osS << "<SGLFA:" << SysError(dwCode) << ">!\r\n"; break;
        }
      }
    } // Show error
    else osS << "SymInitialise failed: " << SysError() << ".\r\n";
    // Done with these defines
    #undef ADDR_PC
    #undef ADDR_FRAME
    #undef IMAGE_FILE_MACHINE
  } // Shouldn't happen but just incase
  catch(const exception &e) { osS << e.what(); }
  /* == Dump summary to file =============================================== */
  DWORD SEHWrite(const HANDLE hH, const string &strS)
  { // Bytes out
    DWORD dwW;
    // Write data. MS says dwOut is optional, but the app crashes if you set
    // it to nullptr. So, something odd there.
    WriteFile(hH, strS.data(),
      static_cast<DWORD>(strS.length()), &dwW, nullptr);
    // Return bytes written
    return dwW;
  }
  /* == Create a subtitle for the output =================================== */
  void SEHSubTitle(const string &strS)
  { // Build title and underline
    osS << strS << "\r\n" << string(strS.length(), '=') << "\r\n";
  }
  /* == Dump file information ============================================== */
  void SEHDumpFileInfo(const wstring &wstrFile) try
  { // Get system time
    SYSTEMTIME stData;
    GetLocalTime(&stData);
    // Write data
    osS << dec << "Date/Time: "
        << right << setfill('0') << setw(2) << stData.wDay
        << '/' << setw(2) << stData.wMonth
        << '/' << setw(4) << stData.wYear
        << ' ' << setw(2) << stData.wHour
        << ':' << setw(2) << stData.wMinute
        << ':' << setw(2) << stData.wSecond
        << '.' << setw(3) << stData.wMilliseconds << ".\r\n"
           "Filename: " << WS16toUTF(wstrFile) << ".\r\n"
           "Arguments: " << S16toUTF(GetCommandLine()) << ".\r\n"
           "\r\n";
  } // Shouldn't happen but just incase
  catch(const exception &e) { osS << e.what(); }
  /* == Dump string to file ================================================ */
  void SEHDumpLog(const CONTEXT *cData, const string &strDialog) try
  { // Get filename
    const wstring wstrFile{ SEHGetExecutableFileNameWithoutExtension() +
      L".dbg" };
    // Open dump file and return if failed
    const HANDLE hFile = CreateFile(wstrFile.c_str(), GENERIC_WRITE, 0, 0,
      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(hFile == INVALID_HANDLE_VALUE) return;
    // Capture exceptions so we can close the file
    try
    { // Write data
      SEHSubTitle("Basic");       SEHDumpFileInfo(wstrFile);
      SEHSubTitle("Summary");     osS << strDialog << "\r\n";
      SEHSubTitle("Registers");   SEHDumpRegisters(cData);    osS << "\r\n";
      SEHSubTitle("Memory");      SEHDumpMemoryStatus();      osS << "\r\n";
      SEHSubTitle("Environment"); SEHDumpEnvironment();       osS << "\r\n";
      SEHSubTitle("Modules");     SEHModuleDump();            osS << "\r\n";
      SEHSubTitle("Processes");   SEHProcessDump();           osS << "\r\n";
      SEHSubTitle("Log");
      cLog->GetBufferLines(osS);                               osS << "\r\n";
      SEHSubTitle("End-of-File");
      // Write the log and throw if failed
      SEHWrite(hFile, osS.str());
      // Close handle
      CloseHandle(hFile);
    } // If exception occured?
    catch(const exception &)
    { // Close handle
      CloseHandle(hFile);
      // Rethrow
      throw;
    }
  } // Shouldn't happen but just incase
  catch(const exception &) { }
  /* == Convert exception to string ======================================== */
  const char *SEHExceptionToString(const DWORD dwCode)
  { // Compare exception code
    switch(dwCode) {
      #define CASE(n,m) case EXCEPTION_ ## n: return m
      CASE(ACCESS_VIOLATION,         "Access violation");
      CASE(ARRAY_BOUNDS_EXCEEDED,    "array bounds exceeded");
      CASE(BREAKPOINT,               "Breakpoint");
      CASE(DATATYPE_MISALIGNMENT,    "Datatype misalignment");
      CASE(FLT_DENORMAL_OPERAND,     "Float denormal operand");
      CASE(FLT_DIVIDE_BY_ZERO,       "Float divide-by-zero");
      CASE(FLT_INEXACT_RESULT,       "Float inexact result");
      CASE(FLT_INVALID_OPERATION,    "Float invalid operation");
      CASE(FLT_OVERFLOW,             "Float overflow");
      CASE(FLT_STACK_CHECK,          "Float stack check");
      CASE(FLT_UNDERFLOW,            "Float underflow");
      CASE(ILLEGAL_INSTRUCTION,      "Illegal instruction");
      CASE(IN_PAGE_ERROR,            "In-page error");
      CASE(INT_DIVIDE_BY_ZERO,       "Integer divide-by-zero");
      CASE(INT_OVERFLOW,             "Integer overflow");
      CASE(INVALID_DISPOSITION,      "Invalid disposition");
      CASE(NONCONTINUABLE_EXCEPTION, "Non-continuable exception");
      CASE(PRIV_INSTRUCTION,         "Privileged instruction");
      CASE(SINGLE_STEP,              "Single step");
      CASE(STACK_OVERFLOW,           "Stack overflow");
      CASE(GUARD_PAGE,               "Guard page violation");
      CASE(INVALID_HANDLE,           "Invalid handle");
      CASE(APT,                      "Abormal program termination");
      CASE(ISA,                      "Illegal storage access");
      CASE(FPE,                      "Floating point exception");
      #undef CASE
      default: return "Unrecognised exception";
    }
  }
  /* == Build summary ====================================================== */
  const string SEHGetSummary(const EXCEPTION_POINTERS &epData) try
  { // Get exception record
    const EXCEPTION_RECORD &erData = *epData.ExceptionRecord;
    // Build message for dialog box
    osS << "The instruction at address 0x" << erData.ExceptionAddress
        << " of thread " << GetCurrentThreadId()
        << " in process " << GetCurrentProcessId()
        << " caused exception 0x" << hex << erData.ExceptionCode
        << " (" << SEHExceptionToString(erData.ExceptionCode)
        << ") with flags 0x" << erData.ExceptionFlags << dec << '.';
    SEHDumpExceptionMemoryAddresses(erData);
    osS << "\r\n\r\n";
    // Check for illegal instruction?
    if(erData.ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION)
    { // Add extra information
      osS << "This illegal instruction exception usually indicates that this "
             BUILD_TARGET " compiled binary is NOT compatible with your "
             "operating system and/or central processing unit. "
  #ifdef X64                             // Special message for 64-bit system?
             "Since you are running the 64-bit version of this binary, please "
             "consider trying the 32-bit version instead."
  #else                                  // Special message for 32-bit system?
             "Since you are running the 32-bit version of this binary, you "
             "may need to upgrade your operating system and/or central "
             "processing unit. If you originally tried the 64-bit version and "
             "you still got the same error then you may need to report this "
             "issue as a bug."
  #endif
             "\r\n\r\n";
    } // Add stack dump[
    SEHStackDump(GetCurrentProcess(), GetCurrentThread(),
      epData.ContextRecord);
    // Return string
    return osS.str();
  } // Shouldn't happen but just incase
  catch(const exception &e) { return e.what(); }
  /* == Method for exception handler ======================================= */
  static LONG WINAPI HandleExceptionStatic(LPEXCEPTION_POINTERS);
  LONG HandleException(const EXCEPTION_POINTERS &epData) try
  { // Check exception record
    if(!epData.ExceptionRecord)
      throw runtime_error{ "The engine crash handler was called "
                           "but with a null pointer exception record!" };
    // Prepare summary
    const string strDialog{ SEHGetSummary(epData) };
    // Clear string stream
    osS.str(cpBlank);
    // Write the log file
    SEHDumpLog(epData.ContextRecord, strDialog);
    // No need to show anything if we're in a debugger
    if(IsDebuggerPresent()) return EXCEPTION_CONTINUE_SEARCH;
    // Show message box
    MessageBox(hwndWindow, UTFtoS16(strDialog),
      L"Unhandled exception", MB_ICONSTOP);
    // We handled the exception
    return EXCEPTION_EXECUTE_HANDLER;
  } // This shouldn't happen but just incase
  catch(const exception &e)
  { // Show message box
    MessageBox(hwndWindow, UTFtoS16(e.what()),
      L"exception in unhandled exception", MB_ICONSTOP);
    // We handled the exception
    return EXCEPTION_EXECUTE_HANDLER;
  }
  /* == Signal event handler =============================================== */
  /* Make sure to follow the rules of using the signal callback:-            */
  /* + Do not issue low-level or STDIO.H I/O routines (for example, printf   */
  /*   or fread).                                                            */
  /* + Do not call heap routines or any routine that uses the heap routines  */
  /*   (for example, malloc, _strdup, or _putenv). See malloc for more       */
  /*   information.                                                          */
  /* + Do not use any function that generates a system call (for example,    */
  /*   _getcwd or time).                                                     */
  /* + Do not use longjmp unless the interrupt is caused by a floating-point */
  /*   exception (that is, sig is SIGFPE). In this case, first reinitialize  */
  /*   the floating-point package by using a call to _fpreset.               */
  /* + Do not use any overlay routines.                                      */
  /*   docs.microsoft.com/en-us/previous-versions/xdkz3x12(v%3Dvs.140)       */
  /* ----------------------------------------------------------------------- */
  static void SignalEvent(int iSignal)
  { // Id code of exception (make sure to describe them in exception handler)
    DWORD dwId;
    // Compare signal
    switch(iSignal)
    { // Abort?
      case SIGABRT: dwId = EXCEPTION_APT; break;
      // Segmentation fault?
      case SIGSEGV: dwId = EXCEPTION_ISA; break;
      // Floating point error?
      case SIGFPE: dwId = EXCEPTION_FPE; break;
      // Something else? Breakpoint
      default: return DebugBreak();
    } // Raise exception
    RaiseException(dwId, 0, 0, nullptr);
  }
  /* ------------------------------------------------------------ */ protected:
  void SetWindowHandle(HWND hwndNew) { hwndWindow = hwndNew; }
  /* --------------------------------------------------------------- */ public:
  HWND GetWindowHandle(void) const { return hwndWindow; }
  bool IsWindowHandleSet(void) const { return GetWindowHandle() != nullptr; }
  bool IsNotWindowHandleSet(void) const { return !IsWindowHandleSet(); }
  void SetWindowDestroyed(void) { SetWindowHandle(nullptr); }
  /* ----------------------------------------------------------------------- */
  bool InitGlobalMutex(const string &strTitle)
  { // Convert UTF title to wide string
    const wstring wstrTitle{ UTFtoS16(strTitle) };
    // Create the global mutex handle with the specified name and check error
    hMutex = CreateMutex(nullptr, FALSE, wstrTitle.c_str());
    switch(const DWORD dwResult = SysErrorCode<DWORD>())
    { // No error, continue
      case 0: return true;
      // Global mutex name already exists?
      case ERROR_ALREADY_EXISTS:
        // Log that another instance already exists
        LW(LH_DEBUG,
          "System found existing mutex, scanning for original window...");
        // Look for the specified window and if we activated it?
        if(!EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&wstrTitle)))
        { // If an error occured?
          if(SysIsNotErrorCode())
          { // Log the error and reason why it failed
            LW(LH_ERROR, "System failed to find the window to activate! $!",
              SysError());
          } // No window was found
          else LW(LH_WARNING, "System window enumeration successful!");
        } // Could not find it?
        else LW(LH_DEBUG, "System window enumeration unsuccessful!");
        // Caller must terminate program
        return false;
      // Other error
      default: XCS("Failed to create global mutex object!",
        "Title",  strTitle,
        "Result", static_cast<unsigned int>(dwResult),
        "mutex",  reinterpret_cast<void*>(hMutex));
    } // Getting here is impossible
  }
  /* -- Delete specified mutex (not needed on windoze) --------------------- */
  void DeleteGlobalMutex(const string&) { }
  /* == Destructor ========================================================= */
  ~SysBase(void)
  { // Restore original signal handlers
    if(fcbFloatingPointException) signal(SIGFPE, fcbFloatingPointException);
    if(fcbIllegalStorageAccess) signal(SIGSEGV, fcbIllegalStorageAccess);
    if(fcbAbortCallback) signal(SIGABRT, fcbAbortCallback);
    // exception filter no longer valid
    SetUnhandledExceptionFilter(nullptr);
    // Restore old error mode
    SetErrorMode(uiOldErrorMode);
    // Delete global mutex
    DeleteGlobalMutex();
  }
  /* -- Constructor (install exception filter) ----------------------------- */
  SysBase(void) :
    /* -- Initialisers ----------------------------------------------------- */
    hwndWindow(nullptr),
    hMutex(nullptr),
    // Set no dialogues for system errors and save code
    uiOldErrorMode(SetErrorMode(SEM_NOOPENFILEERRORBOX)),
    // Install signal handlers and save old ones
    fcbAbortCallback(signal(SIGABRT, SignalEvent)),
    fcbIllegalStorageAccess(signal(SIGSEGV, SignalEvent)),
    fcbFloatingPointException(signal(SIGFPE, SignalEvent))
  /* -- Install unhandled exception filter --------------------------------- */
  { SetUnhandledExceptionFilter(HandleExceptionStatic); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SysBase);            // Suppress copy constructor for safety
};/* ======================================================================= */
#define MSENGINE_SYSBASE_CALLBACKS() \
  LONG WINAPI SysBase::HandleExceptionStatic(LPEXCEPTION_POINTERS epData) \
    { return cSystem->HandleException(*epData); }
/* == EoF =========================================================== EoF == */
