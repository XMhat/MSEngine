/* == MEMORY.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## These classes allow allocation and manipulation of memory. Always   ## */
/* ## use the 'Memory' class as statically allocated so the memory will   ## */
/* ## never be lost. The 'Data' class is used to help manipulate existing ## */
/* ## memory and may be used when allocation isn't needed.                ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfMemory {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfUtil;                // Using util namespace
using namespace IfUtf;                 // Using utf namespace
/* == Read only data class ================================================= */
class DataConst                        // Start of const Data Block Class
{ /* -- Private variables ----------------------------------------- */ private:
  char            *cpPtr;              // Pointer to data
  size_t           stSize;             // Size of data
  /* -- Read pointer -------------------------------------------- */ protected:
  template<typename Type=char>Type *DoRead(const size_t stPos) const
    { return reinterpret_cast<Type*>(cpPtr + stPos); }
  /* -- Clear parameters. Used by FileMap() -------------------------------- */
  void ClearParams(void) { SetNewPtr(); SetNewSize(); }
  /* -- Swap members with another block ------------------------------------ */
  void SwapDataConst(DataConst &&dcOther)
    { swap(stSize, dcOther.stSize); swap(cpPtr, dcOther.cpPtr); }
  /* -- Bit test error handler --------------------------------------------- */
  void HandleBitError[[noreturn]](const char*const cpWhat,
    const size_t stPos) const
  { // Get absolute position and maximum bit position
    const size_t stAbsPos = Bit::Slot(stPos), stMax = Bit::Bit(stSize);
    // Throw the error
    XC(cpWhat, "BitPosition",  stPos,    "BitMaximum",  stMax,
               "BytePosition", stAbsPos, "ByteMaximum", stSize,
               "AddrPosition", DoRead<void*>(stAbsPos),
               "AddrStart",    Ptr(),    "AddrMaximum", PtrEnd());
  }
  /* -- Read pointer error handler------------------------------------------ */
  void HandleReadError[[noreturn]](const char*const cpWhat, const size_t stPos,
    const size_t stBytes) const
      { XC(cpWhat, "Position",  stPos,  "Amount",  stBytes,
                   "Maximum",   stSize, "AddrPos", DoRead<void*>(stPos),
                   "AddrStart", Ptr(),  "AddrMax", PtrEnd()); }
  /* -- Set size ----------------------------------------------------------- */
  void SetNewPtr(char*const cpNPtr = nullptr) { cpPtr = cpNPtr; }
  void SetNewSize(const size_t stNSize = 0) { stSize = stNSize; }
  void SetNewParams(char*const cpNPtr, const size_t stNSize)
    { SetNewPtr(cpNPtr); SetNewSize(stNSize); }
  /* -- Free the pointer --------------------------------------------------- */
  void FreePtr(void) { MemFree(Ptr()); }
  void FreePtrIfSet(void) { if(IsPtrSet()) FreePtr(); }
  /* -- Character access by position ------------------------------- */ public:
  char &operator[](const size_t stPos) const
    { CheckValid(stPos, 1); return cpPtr[stPos]; }
  /* -- Return memory at the allocated address ----------------------------- */
  template<typename Type=void>
    Type *Ptr(void) const { return reinterpret_cast<Type*>(cpPtr); }
  /* -- Return size of allocated memory ------------------------------------ */
  template<typename Type=size_t>
    Type Size(void) const { return static_cast<Type>(stSize); }
  /* -- Return if memory is allocated -------------------------------------- */
  bool Empty(void) const { return Size() == 0; }
  bool NotEmpty(void) const { return !Empty(); }
  /* -- Returns if the pointer is valid ------------------------------------ */
  bool IsPtrSet(void) const { return !!Ptr(); }
  bool IsPtrNotSet(void) const { return !Ptr(); }
  /* -- Return ending address ---------------------------------------------- */
  template<typename Type=void>
    Type *PtrEnd(void) const { return DoRead<Type>(stSize); }
  /* -- Return if we can read this amount of data -------------------------- */
  bool CheckValid(const size_t stPos, const size_t stBytes) const
  { // Return if bounds are safe
    return cpPtr                   &&  // Address is valid
           stPos         <  Size() &&  // Position is <= array size
           stBytes       <= Size() &&  // Bytes to copy is <= array size
           stPos+stBytes <= Size();    // End position <= array size
  }
  /* -- Same as CheckValid() with ptr check --------- Data::CheckValidPtr -- */
  bool CheckValidPtr(const size_t stPos, const size_t stBytes,
    const void*const vpOther) const
      { return CheckValid(stPos, stBytes) && vpOther != nullptr; }
  /* -- Test a bit position ------------------------------------------------ */
  bool CheckValidBit(const size_t stPos) const
    { return Bit::Slot(stPos) >= Size(); }
  /* -- Find specified string ---------------------------------------------- */
  size_t Find(const string &strMatch, size_t stPos=0) const
  { // Bail if parameters are invalid
    if(strMatch.empty() || Empty() || stPos > Size()) return string::npos;
    // Position
    size_t stIndex;
    // Until end of string
    while(const char*const cpLoc = reinterpret_cast<char*>
      (memchr(DoRead(stPos), strMatch.front(), Size()-stPos)))
    { // Calculate index
      stPos = static_cast<size_t>(Ptr<char>()-cpPtr);
      // Walk data until one of three things happen
      // - End of match string
      // - Character mismatch
      // - End of memory block
      for(stIndex = 0;
          stIndex < strMatch.length() &&
          strMatch[stIndex] == cpPtr[stPos+stIndex] &&
          stPos+stIndex < stSize;
        ++stIndex);
      // If we read all of the string match then we succeeded
      if(stIndex >= strMatch.length()) return stPos;
      // Incrememnt position and try again
      stPos += stIndex;
    } // Failed
    return string::npos;
  }
  /* -- Read with byte bound check ----------------------------------------- */
  template<typename Type=char>
    Type *Read(const size_t stPos, const size_t stBytes=0) const
  { // Bail if size bad
    if(!CheckValid(stPos, stBytes))
      HandleReadError("Read error!", stPos, stBytes);
    // Return pointer
    return DoRead<Type>(stPos);
  }
  /* -- Read static variable ----------------------------------------------- */
  template<typename Type>Type ReadInt(const size_t stPos=0) const
    { return *Read<Type>(stPos, sizeof(Type)); }
  template<typename Type>Type ReadIntLE(const size_t stPos=0) const
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      return ToLittleEndian(ReadInt<Type>(stPos)); }
  template<typename Type>Type ReadIntBE(const size_t stPos=0) const
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      return ToBigEndian(ReadInt<Type>(stPos)); }
  /* -- Test a bit --------------------------------------------------------- */
  bool BitTest(const size_t stPos) const
  { // Throw error if invalid position
    if(!CheckValidBit(stPos)) HandleBitError("Test error!", stPos);
    // Return the tested bit
    return Bit::Test(Ptr<char>(), stPos);
  }
  /* -- Stringify the memory ----------------------------------------------- */
  const string ToString(void) const
  { // Return empty string if no size
    if(Empty()) return { };
    // Find a null character and if we found it? We know what the size is!
    if(const char*const cpLocPtr =
      reinterpret_cast<char*>(memchr(Ptr<char>(), '\0', Size())))
        return { Ptr<char>(), static_cast<size_t>(cpLocPtr-Ptr<char>()) };
    // There is no null character so we have to limit the size
    return { Ptr<char>(), Size() };
  }
  /* -- Return if current size would overflow specified type --------------- */
  template<typename Type=size_t>bool IsSizeOverflow(void) const
    { return IntWillOverflow<Type>(Size()); }
  /* -- Init from string (does not copy) ----------------------------------- */
  explicit DataConst(const string &strSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    DataConst{ strSrc.length(),        // Copy string size and pointer over
      strSrc.data() }                  // from specified string
    /* -- Copy pointer and size over from string --------------------------- */
    { }
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  explicit DataConst(DataConst &&dSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    cpPtr(dSrc.Ptr<char>()),           // Copy pointer
    stSize(dSrc.Size())                // Copy size
    /* -- Code to clear other DataConst ------------------------------------ */
    { dSrc.ClearParams(); }
  /* -- Uninitialised constructor -- pointer ------------------------------- */
  DataConst(void) :
    /* -- Initialisers ----------------------------------------------------- */
    cpPtr(nullptr),                       // Start with null pointer
    stSize(0)                          // No size allocated yet
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Inherit an already allocated pointer ------------------------------- */
  DataConst(const size_t stNSize, const void*const vpSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    cpPtr(ToNonConstCast               // Initialise memory
      <char*>(vpSrc)),                 // To specified pointer
    stSize(stNSize)                    // Set pointer size
    /* -- Check that size is set with pointer ------------------------------ */
    { if(IsPtrNotSet() && NotEmpty())
        XC("Null pointer with non-zero memory size requested!",
           "Size", Size()); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(DataConst)           // Do not need defaults
};/* ----------------------------------------------------------------------- */
/* == Read and write data class ============================================ */
class Data :
  /* -- Base classes ------------------------------------------------------- */
  public DataConst                     // Start of Data Block Class
{ /* -- Copy memory ---------------------------- Data::DoWrite -- */ protected:
  void DoWrite(const size_t stPos, const void*const vpSrc,
    const size_t stBytes) { memcpy(DoRead(stPos), vpSrc, stBytes); }
  /* -- Fill with specified character at specifed position ----------------- */
  template<typename Type=unsigned char>void DoFill(const size_t stPos,
    const Type tVal, const size_t stBytes)
  { // Get address to start from
    char*const cpStart = DoRead(stPos);
    // Do the fill. This is supposedly faster than memset
    StdFill(par_unseq, reinterpret_cast<Type*const>(cpStart),
      reinterpret_cast<Type*const>(cpStart + stBytes), tVal);
  }
  /* -- Fill with specified character at specifed position --------- */ public:
  template<typename Type=unsigned char>void FillEx(const size_t stPos,
    const Type tVal, const size_t stBytes)
  { // Get end position and make sure it wont overrun
    if(!CheckValid(stPos, stBytes))
      HandleReadError("Fill error!", stPos, stBytes);
    // Do the fill
    DoFill(stPos, tVal, stBytes);
  }
  /* -- Fill with specified value ------------------------------------------ */
  template<typename Type=unsigned char>void Fill(const Type tVal=0)
    { FillEx(0, tVal, Size()); }
  /* -- Write memory with checks ------------------------------------------- */
  void Write(const size_t stPos, const void*const vpSrc, const size_t stBytes)
  { // Check parameters are valid
    if(!CheckValidPtr(stPos, stBytes, vpSrc))
      XC("Write error!", "Destination", Ptr(),   "Source",   vpSrc,
                         "Bytes",       stBytes, "Position", stPos,
                         "Maximum",     Size());
    // Do copy
    DoWrite(stPos, vpSrc, stBytes);
  }
  /* -- Swap bits ---------------------------------------------------------- */
  void Swap8(const size_t stPos)
    { WriteInt<uint8_t>(stPos, Swap4Bit(ReadInt<uint8_t>(stPos))); }
  void Swap16(const size_t stPos)
    { WriteInt<uint16_t>(stPos, SWAP_U16(ReadInt<uint16_t>(stPos))); }
  void Swap32(const size_t stPos)
    { WriteInt<uint32_t>(stPos, SWAP_U32(ReadInt<uint32_t>(stPos))); }
  void Swap64(const size_t stPos)
    { WriteInt<uint64_t>(stPos, SWAP_U64(ReadInt<uint64_t>(stPos))); }
  /* -- Write specified variable as an integer ----------------------------- */
  template<typename Type>void WriteInt(const size_t stPos, const Type tVar)
    { Write(stPos, &tVar, sizeof(tVar)); }
  template<typename Type>void WriteInt(const Type tVar)
    { WriteInt<Type>(0, tVar); }
  template<typename Type>void WriteIntLE(const size_t stPos, const Type tVar)
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      WriteInt<Type>(stPos, ToLittleEndian(tVar)); }
  template<typename Type>void WriteIntLE(const Type tVar)
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      WriteInt<Type>(ToLittleEndian(tVar)); }
  template<typename Type>void WriteIntBE(const size_t stPos, const Type tVar)
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      WriteInt<Type>(stPos, ToBigEndian(tVar)); }
  template<typename Type>void WriteIntBE(const Type tVar)
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      WriteInt<Type>(ToBigEndian(tVar)); }
  /* -- Write specified variable as an floating point number --------------- */
  void WriteFloatLE(const float fVar) { WriteFloatLE(0, fVar); }
  void WriteFloatLE(const size_t stPos, const float fVar)
    { WriteInt<float>(stPos, ToF32LE(fVar)); }
  void WriteFloatBE(const float fVar) { WriteFloatBE(0, fVar); }
  void WriteFloatBE(const size_t stPos, const float fVar)
    { WriteInt<float>(stPos, ToF32BE(fVar)); }
  void WriteDoubleLE(const double dVar) { WriteDoubleLE(0, dVar); }
  void WriteDoubleLE(const size_t stPos, const double dVar)
    { WriteInt<double>(stPos, ToF64LE(dVar)); }
  void WriteDoubleBE(const double dVar) { WriteDoubleBE(0, dVar); }
  void WriteDoubleBE(const size_t stPos, const double dVar)
    { WriteInt<double>(stPos, ToF64BE(dVar)); }
  /* -- Write memory block at specified position --------------------------- */
  void WriteBlock(const size_t stPos, const DataConst &dData,
    const size_t stBytes) { Write(stPos, dData.Ptr<char>(), stBytes); }
  void WriteBlock(const size_t stPos, const DataConst &dData)
    { WriteBlock(stPos, dData, dData.Size()); }
  /* -- Set a bit ---------------------------------------------------------- */
  void BitSet(const size_t stPos)
  { // Throw error if invalid position
    if(!CheckValidBit(stPos)) HandleBitError("Set error!", stPos);
    // Set the bit
    Bit::Set(Ptr<char>(), stPos);
  }
  /* -- Clear a bit -------------------------------------------------------- */
  void BitClear(const size_t stPos)
  { // Throw error if invalid position
    if(!CheckValidBit(stPos)) HandleBitError("Clear error!", stPos);
    // Clear the bit
    Bit::Clear(Ptr<char>(), stPos);
  }
  /* -- Flip a bit --------------------------------------------------------- */
  void BitFlip(const size_t stPos)
  { // Throw error if invalid position
    if(!CheckValidBit(stPos)) HandleBitError("Flip error!", stPos);
    // Flip the bit
    Bit::Flip(Ptr<char>(), stPos);
  }
  /* -- Invert specific flags ---------------------------------------------- */
  template<typename Type=uint8_t>void Invert(const size_t stPos,
    const Type tFlags=numeric_limits<Type>::max())
  { // Bail if size bad
    if(!CheckValid(stPos, sizeof(Type)))
      HandleReadError("Invert error!", stPos, sizeof(Type));
    // Do the invert
    *reinterpret_cast<Type*const>(DoRead(stPos)) ^= tFlags;
  }
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  Data(Data &&dSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    DataConst{ StdMove(dSrc) }            // Move other data object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Take ownership of pointer (rvalue) --------------------------------- */
  explicit Data(DataConst &&dcSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    DataConst{ StdMove(dcSrc) }           // Move other data const object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Inherit an already allocated pointer ------------------------------- */
  Data(const size_t stNSize, void*const vpSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    DataConst{ stNSize, vpSrc }        // Assign pointer and pointer size
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Uninitialised constructor -- pointer ------------------------------- */
  Data(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Data)                // Do not need defaults
};/* ----------------------------------------------------------------------- */
/* == Read, write and allocation data class ================================ */
class Memory :
  /* -- Base classes ------------------------------------------------------- */
  public Data                          // Start of Memory Block Class
{ /* -- Resize and preserve allocated memory ---------------------- */ private:
  void DoResize(const size_t stNSize)
  { // Realloc new amount of memory and if succeeded? Set new block and size
    if(char*const cpNew = MemReAlloc(Ptr<char>(), Maximum(stNSize, 1)))
      SetNewParams(cpNew, stNSize);
    // Failed so throw error
    else XC("Re-alloc failed!", "OldSize", Size(), "NewSize", stNSize);
  }
  /* -- Swap memory with another memory block ---------------------- */ public:
  void SwapMemory(Memory &&mOther) { SwapDataConst(StdMove(mOther)); }
  /* -- Resize and preserve allocated memory ------------------------------- */
  void Resize(const size_t stNSize)
    { if(stNSize != Size()) DoResize(stNSize); }
  /* -- Add allocated memory ----------------------------------------------- */
  void ResizeAdd(const size_t stNSize) { DoResize(Size() + stNSize); }
  /* -- Resize memory upwards never downwards ------------------------------ */
  void ResizeUp(const size_t stNSize)
    { if(stNSize > Size()) DoResize(stNSize); }
  /* -- Append the specified amount of memory ------------------------------ */
  void Append(const void*const vpSrc, const size_t stBytes)
  { // Bail out if the pointer is invalid
    if(!vpSrc) XC("Source address to append invalid!");
    // Return if no bytes are to be copied
    if(!stBytes) return;
    // Position to write to
    const size_t stPos = Size();
    // Resize block to write memory
    DoResize(Size() + stBytes);
    // Write data
    DoWrite(stPos, vpSrc, stBytes);
  }
  /* -- Resize a block from both ends -------------------------------------- */
  void Crop(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!CheckValid(stPos, stBytes))
      HandleReadError("Crop error!", stPos, stBytes);
    // If position is from start? We just need to realloc
    if(!stPos) return DoResize(stBytes);
    // Init new class and transfer it to this one
    SwapMemory({ stBytes-stPos, DoRead(stPos) });
  }
  /* -- Byte swap (Hi4<->Lo4 from 8-bit integer) --------------------------- */
  void ByteSwap8(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!CheckValid(stPos, stBytes))
      HandleReadError("8-bit swap error!", stPos, stBytes);
    // Reverse each byte
    for(uint8_t *ubPtr = DoRead<uint8_t>(stPos),
                *ubEnd = ubPtr + stBytes;
                 ubPtr < ubEnd;
               ++ubPtr)
      *ubPtr = ReverseByte(*ubPtr);
  }
  void ByteSwap8(const size_t stBytes) { ByteSwap8(0, stBytes); }
  void ByteSwap8(void) { ByteSwap8(Size()); }
  /* -- Byte swap (Hi8<->Lo8 from 16-bit integer) -------------------------- */
  void ByteSwap16(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!CheckValid(stPos, stBytes) || stBytes % sizeof(uint16_t) > 0)
      HandleReadError("16-bit swap error!", stPos, stBytes);
    // Reverse each word
    for(uint16_t *usPtr = DoRead<uint16_t>(stPos),
                 *usEnd = DoRead<uint16_t>(stPos + stBytes);
                  usPtr < usEnd;
                ++usPtr)
      *usPtr = SWAP_U16(*usPtr);
  }
  void ByteSwap16(const size_t stBytes) { ByteSwap16(0, stBytes); }
  void ByteSwap16(void) { ByteSwap16(Size()); }
  /* -- Byte swap (Hi16<->Lo16 from 32-bit integer) ------------------------ */
  void ByteSwap32(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!CheckValid(stPos, stBytes) || stBytes % sizeof(uint32_t) > 0)
      HandleReadError("32-bit swap error!", stPos, stBytes);
    // Reverse each dword
    for(uint32_t *ulPtr = DoRead<uint32_t>(stPos),
                 *ulEnd = DoRead<uint32_t>(stPos + stBytes);
                  ulPtr < ulEnd;
                ++ulPtr)
      *ulPtr = SWAP_U32(*ulPtr);
  }
  void ByteSwap32(const size_t stBytes) { ByteSwap32(0, stBytes); }
  void ByteSwap32(void) { ByteSwap32(Size()); }
  /* -- Byte swap (Hi32<->Lo32 from 64-bit integer) ------------------------ */
  void ByteSwap64(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!CheckValid(stPos, stBytes) || stBytes % sizeof(uint64_t) > 0)
      HandleReadError("64-bit swap error!", stPos, stBytes);
    // Reverse each quad
    for(uint64_t *uqPtr = DoRead<uint64_t>(stPos),
                 *uqEnd = DoRead<uint64_t>(stPos + stBytes);
                  uqPtr < uqEnd;
                ++uqPtr)
      *uqPtr = SWAP_U64(*uqPtr);
  }
  void ByteSwap64(const size_t stBytes) { ByteSwap64(0, stBytes); }
  void ByteSwap64(void) { ByteSwap64(Size()); }
  /* -- Reverse at the position for number of bytes ------------------------ */
  void Reverse(const size_t stPos, const size_t stBytes)
  { // Ignore if length not set and treat as if succeeded
    if(!stBytes) return;
    // Bail if size bad
    if(!CheckValid(stPos, stBytes))
      HandleReadError("Reverse error!", stPos, stBytes);
    // Create empty memory block
    Memory mDst{ stBytes };
    // Copy each byte from the start of source, to the end of destination
    for(size_t stIndex = 0; stIndex < stBytes; ++stIndex)
      mDst.DoWrite(Size() - stIndex - 1, DoRead(stIndex), 1);
    // Assign new memory block
    SwapMemory(StdMove(mDst));
  }
  /* -- Reverse the specified number of bytes------------------------------- */
  void Reverse(const size_t stBytes) { Reverse(0, stBytes); }
  /* -- Reverse the entire memory block ------------------------------------ */
  void Reverse(void) { Reverse(Size()); }
  /* -- Free the allocated memory and reset members ------------------------ */
  void DeInit(void)
    { if(IsPtrSet()) { FreePtr(); SetNewPtr(); } SetNewSize(); }
  /* -- Allocate memory ---------------------------------------------------- */
  void InitBlank(const size_t stBytesRequested=0)
  { // If allocated memory already exists? Free it!
    FreePtrIfSet();
    // Allocate memory forcing zero bytes to 1 byte for compatibility.
    if(char*const cpNew = MemAlloc<char>(Maximum(stBytesRequested, 1)))
      return SetNewParams(cpNew, stBytesRequested);
    // The memory was freed so this memory is no longer available.
    SetNewSize();
    // Failed so throw an exception.
    XC("Alloc failed!", "Size", stBytesRequested);
  }
  /* -- Allocate and copy from existing memory ----------------------------- */
  void InitData(const size_t stS, const void*const vpB)
    { InitBlank(stS); if(stS && vpB) Write(0, vpB, stS); }
  /* -- Allocate and copy from existing string ----------------------------- */
  void InitString(const string &strS)
    { InitData(strS.length(), reinterpret_cast<const void*>(strS.data())); }
  /* -- Allocate and copy from existing memory block ----------------------- */
  void InitCopy(const DataConst &dcSrc)
    { InitData(dcSrc.Size(), dcSrc.Ptr()); }
  /* -- Allocate and zero memory ------------------------------------------- */
  void InitSafe(const size_t stS) { InitBlank(stS); Fill(); }
  /* -- Assignment operator (rvalue) ------------------------------------ -- */
  Memory &operator=(Memory &&mbSrc)
    { SwapMemory(StdMove(mbSrc)); return *this; }
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  Memory(Memory &&mbSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    Data(StdMove(mbSrc))             // Move other memory object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Take ownership of pointer (must originally be malloc'd) ------------ */
  explicit Memory(Data &&dSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    Data(StdMove(dSrc))              // Move other data object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Take ownership of pointer (must originally be malloc'd) ------------ */
  explicit Memory(DataConst &&dcSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    Data(StdMove(dcSrc))             // Move other read only memory object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Init from string --------------------------------------------------- */
  explicit Memory(const string &strSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ strSrc.length(),           // Allocate memory and copy the string
      strSrc.data() }                  // over to our allocated memory
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Inherit an already allocated pointer ------------------------------- */
  Memory(const size_t stNSize, void*const vpSrc, const bool) :
    /* -- Initialisers ----------------------------------------------------- */
    Data{ stNSize, vpSrc }             // Take ownership of pointer
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Alloc uninitialised ------------------------------------------------ */
  explicit Memory(const size_t stNSize) :
    /* -- Initialisers ----------------------------------------------------- */
    Data{ stNSize,                     // Initialise data base class
      MemAlloc<char>                   // Allocate memory (checked by CTOR)
        (Maximum(stNSize, 1)) }        // Allocate requested size
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Alloc with fill ---------------------------------------------------- */
  Memory(const size_t stNSize, const bool) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ stNSize }                  // Allocate memory
    /* -- Full memory with zeros ------------------------------------------- */
    { Fill(); }
  /* -- Alloc with copy ---------------------------------------------------- */
  Memory(const size_t stNSize, const void*const vpSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ stNSize }                  // Allocate size of pointer
    /* -- Code to initialise pointer --------------------------------------- */
    { if(vpSrc) DoWrite(0, vpSrc, stNSize); }
  /* -- Standby constructor ------------------------------------------------ */
  Memory(void) { }
  /* -- Destructor (just a free() needed) ---------------------------------- */
  ~Memory(void) { FreePtrIfSet(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Memory)              // Do not need defaults
};/* -- Useful types ------------------------------------------------------- */
typedef list<Memory> MemoryList;       // List of memory blocks
typedef vector<Memory> MemoryVector;   // A vector of memory classes
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
