/* == FILEMAP.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module closely emulates a FILE* stream but instead maps a file ## */
/* ## on disk into memory. This interface is used only by the 'Assets'    ## */
/* ## interface when loading files from archives or disk. ONLY supply     ## */
/* ## already malloc()'d data to the class if re-using existing data.     ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfFileMap {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfSystem;              // Using system namespace
/* == FileMap Class Definition ============================================= */
class FileMap :
  /* -- Derivced classes --------------------------------------------------- */
  public SysMap,                       // File mapping
  public DataConst                     // Read only memory block
{ /* -- Private variables ----------------------------------------- */ private:
  size_t           stPosition;         // Current position
  /* -- Read from a certain position without checking ---------------------- */
  template<typename T=char>T *FileMapDoReadPtrFrom(const size_t stFrom,
    const size_t stBytes=0)
  { // Read address. Memory() will handle all the error checking for us
    T*const tAddr = DoRead<T>(stFrom);
    // Set new position clamping to size of file
    stPosition = Minimum(Size(), stFrom + stBytes);
    // Return address
    return tAddr;
  }
  /* -- Return current position ------------------------------------ */ public:
  size_t FileMapTell(void) const { return stPosition; }
  /* -- Return bytes remaining --------------------------------------------- */
  size_t FileMapRemain(void) const { return Size() - FileMapTell(); }
  /* -- Is end of file ----------------------------------------------------- */
  bool FileMapIsEOF(void) const { return FileMapTell() >= Size(); }
  bool FileMapIsNotEOF(void) const { return !FileMapIsEOF(); }
  /* -- Return file times -------------------------------------------------- */
  StdTimeT FileMapModifiedTime(void) { return SysMapGetModified(); }
  StdTimeT FileMapCreationTime(void) { return SysMapGetCreation(); }
  /* -- Return if file is opened ------------------------------------------- */
  bool FileMapOpened(void) const { return !!Ptr(); }
  bool FileMapClosed(void) const { return !FileMapOpened(); }
  /* -- Read with byte bound check ----------------------------------------- */
  template<typename T=char>T *FileMapReadPtrFrom(const size_t stFrom,
    const size_t stBytes=0)
  { // Check parameters.
    if(!CheckValid(stFrom, stBytes))
      XC("Read error!",
         "Identifier", IdentGet(), "Destination", Ptr<void>(),
         "Bytes",      stBytes, "Position",    stFrom,
         "Maximum",    SysMapGetSize());
    // Return address. This also sets the new position
    return FileMapDoReadPtrFrom<T>(stFrom, stBytes);
  }
  /* -- Read from current position ----------------------------------------- */
  template<typename T=char>T *FileMapReadPtr(const size_t stBytes=0)
    { return FileMapReadPtrFrom<T>(FileMapTell(), stBytes); }
  /* -- Read variable from specified position ------------------------------ */
  template<typename T=char>const T FileMapReadVarFrom(const size_t stFrom)
    { return *FileMapReadPtrFrom<const T>(stFrom, sizeof(T)); }
  /* -- Read specified variable from current pos --------------------------- */
  template<typename T=char>const T FileMapReadVar(void)
    { return FileMapReadVarFrom<T>(FileMapTell()); }
  /* -- Read specified variable from specified position -------------------- */
  uint16_t FileMapReadVar16LEFrom(const size_t stFrom)
    { return ToI16LE(FileMapReadVarFrom<uint16_t>(stFrom)); }
  uint16_t FileMapReadVar16BEFrom(const size_t stFrom)
    { return ToI16BE(FileMapReadVarFrom<uint16_t>(stFrom)); }
  uint32_t FileMapReadVar32LEFrom(const size_t stFrom)
    { return ToI32LE(FileMapReadVarFrom<uint32_t>(stFrom)); }
  uint32_t FileMapReadVar32BEFrom(const size_t stFrom)
    { return ToI32BE(FileMapReadVarFrom<uint32_t>(stFrom)); }
  uint64_t FileMapReadVar64LEFrom(const size_t stFrom)
    { return ToI64LE(FileMapReadVarFrom<uint64_t>(stFrom)); }
  uint64_t FileMapReadVar64BEFrom(const size_t stFrom)
    { return ToI64BE(FileMapReadVarFrom<uint64_t>(stFrom)); }
  /* -- Read specified variable from current pos --------------------------- */
  uint16_t FileMapReadVar16LE(void)
    { return FileMapReadVar16LEFrom(FileMapTell()); }
  uint16_t FileMapReadVar16BE(void)
    { return FileMapReadVar16BEFrom(FileMapTell()); }
  uint32_t FileMapReadVar32LE(void)
    { return FileMapReadVar32LEFrom(FileMapTell()); }
  uint32_t FileMapReadVar32BE(void)
    { return FileMapReadVar32BEFrom(FileMapTell()); }
  uint64_t FileMapReadVar64LE(void)
    { return FileMapReadVar64LEFrom(FileMapTell()); }
  uint64_t FileMapReadVar64BE(void)
    { return FileMapReadVar64BEFrom(FileMapTell()); }
  /* -- Read data into new array class ------------------------------------- */
  const Memory FileMapReadBlock(const size_t stBytes)
  { // Allocate requested size
    Memory mOut{ stBytes };
    // Read data and shrink block to fit actual bytes read then return it
    mOut.Resize(FileMapReadToAddr(mOut.Ptr(), stBytes));
    // Return bytes read
    return mOut;
  }
  /* -- Read data from specified position in file to array ----------------- */
  const Memory FileMapReadBlockFrom(const size_t stFrom, const size_t stBytes)
  { // Do the seek and return a blank array if failed
    if(!FileMapSeek(stFrom, SEEK_SET)) return { };
    // Do the read and return the array
    return FileMapReadBlock(stBytes);
  }
  /* -- FileMap::Read data from specified position in file into memory ----- */
  size_t FileMapReadFrom(const size_t stFrom, void *vpDst,
    const size_t stBytes)
  { // Do the seek and return a blank array if failed
    if(!FileMapSeek(stFrom, SEEK_SET)) return 0;
    // Do the read and return the array
    return FileMapReadToAddr(vpDst, stBytes);
  }
  /* -- Return full memory of file ----------------------------------------- */
  Memory FileMapDecouple(void)
  { // If memory is not mapped? Just move the current memory across so the
    // returned Memory block takes ownership and frees the memory
    if(SysMapIsNotAvailable())
      return Memory{ std::move(static_cast<DataConst&>(*this)) };
    // We need to read mapped memory into a new memory block. The map class
    // disallows files greater than size_t(-1) so this is safe
    Memory mOut{ IntOrMax<size_t>(SysMapGetSize()), SysMapGetMemory() };
    // De-initialise the map, no point keeping it anymore
    SysMapDeInit();
    // Clear memory block members
    ClearParams();
    // Return memory
    return mOut;
  }
  /* -- FileMap::Read data into existing memory area ----------------------- */
  template<typename T=void>
    size_t FileMapReadToAddr(T*const vpDst, const size_t stBytes)
  { // Bail if no more data
    if(FileMapIsEOF()) return 0;
    // Calculate actual bytes to read
    const size_t stToRead =
      (FileMapTell() + stBytes > Size()) ? Size() - FileMapTell() : stBytes;
    // Copy memory to destination
    memcpy(reinterpret_cast<void*>(vpDst),
      DoRead<void*>(FileMapTell()), stToRead);
    // Seek forward
    stPosition += stToRead;
    // Return bytes
    return stToRead;
  }
  /* -- FileMap::Seek position---------------------------------------------- */
  bool FileMapSeek(const size_t stPos, const int iMode=SEEK_SET)
  { // Which was set
    switch(iMode)
    { // Seek from start?
      case SEEK_SET:
      { // Return failed if invalid position
        if(stPos == FileMapTell()) return true;
        if(stPos > Size()) return false;
        stPosition = stPos;
        break;
      } // Seek from current position?
      case SEEK_CUR:
      { // Get new position and return failed if it goes past end of file
        const size_t stNewPos = FileMapTell() + stPos;
        if(stNewPos == stPos) return true;
        if(stNewPos > Size()) return false;
        stPosition = stNewPos;
        break;
      } // Seek from eof
      case SEEK_END:
      { // Get new position and return failed if it goes past end of file
        const size_t stNewPos = Size() + stPos;
        if(stNewPos == stPos) return true;
        if(stNewPos > Size()) return false;
        stPosition = stNewPos;
        break;
      } // Anything else is a failure
      default: return false;
    } // Return success
    return true;
  }
  /* -- Rewind to start ---------------------------------------------------- */
  void FileMapRewind(void) { stPosition = 0; }
  /* -- Assignment operator ------------------------------------------------ */
  void FileMapSwap(FileMap &fmOther)
  { // Swap memory block and map
    SwapDataConst(std::move(fmOther));
    SysMapSwap(fmOther);
    // Swap position
    swap(stPosition, fmOther.stPosition);
  }
  /* -- Direct access using class variable name which returns opened ------- */
  operator bool(void) const { return FileMapOpened(); }
  /* -- Open a file on disk and map it to memory --------------------------- */
  explicit FileMap(const string &strF) :
    /* -- Initialisation of members ---------------------------------------- */
    SysMap{ strF },
    DataConst{ IntOrMax<size_t>(SysMapGetSize()), SysMapGetMemory() },
    stPosition(0)
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Take ownership of another memory block ----------------------------- */
  FileMap(const string &strF, DataConst &&dcData, const StdTimeT tC,
    const StdTimeT tM) :
    /* -- Initialisation of members ---------------------------------------- */
    SysMap{ strF, tC, tM },            // Reuse system map variables
    DataConst{ std::move(dcData) },         // Init read-only memory block
    stPosition(0)                      // Initialise position
    /* --------------------------------------------------------------------- */
    { }                                // Don't do anything else
  /* -- Take ownership of another memory block ----------------------------- */
  FileMap(const string &strF, DataConst &&dcData, const StdTimeT tC) :
    /* -- Initialisation of members ---------------------------------------- */
    FileMap{ strF, std::move(dcData), tC, tC }
    /* --------------------------------------------------------------------- */
    { }                                // Don't do anything else
  /* -- Move filemap constructor ------------------------------------------- */
  FileMap(FileMap &&fmOther) :
    /* -- Initialisation of members ---------------------------------------- */
    SysMap{ std::move(fmOther) },           // Just moves SysMap members
    DataConst{ std::move(fmOther) },        // Just moves DataConst members
    stPosition(fmOther.FileMapTell())  // Copy other current position
    /* --------------------------------------------------------------------- */
    { fmOther.FileMapRewind(); }       // Reset other position
  /* -- No-init constructor ------------------------------------------------ */
  FileMap(void) :
    /* -- Initialisation of members ---------------------------------------- */
    stPosition(0)                      // No position
    /* --------------------------------------------------------------------- */
    { }                                // Don't do anything else
  /* -- Free memory if we allocated it and it's not a map ------------------ */
  ~FileMap(void) { if(IsPtrSet() && Ptr() != SysMapGetMemory()) FreePtr(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FileMap);            // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
