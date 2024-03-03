/* == MEMORY.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## These classes allow allocation and manipulation of memory. Always   ## **
** ## use the 'Memory' class as statically allocated so the memory will   ## **
** ## never be lost. The 'MemBase' class is used to help manipulate       ## **
** ## existing memory and may be used when allocation isn't needed.       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IMemory {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IError::P;             using namespace IIdent::P;
using namespace IStd::P;               using namespace IUtf;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Read only data class ================================================= */
class MemConst                         // Start of const MemBase Block Class
{ /* -- Private variables ----------------------------------------- */ private:
  char            *cpPtr;              // Pointer to data
  size_t           stSize;             // MemSize of data
  /* -- Read pointer -------------------------------------------- */ protected:
  template<typename Type=char>Type *MemDoRead(const size_t stPos) const
    { return reinterpret_cast<Type*>(cpPtr + stPos); }
  /* -- Clear parameters. Used by FileMap() -------------------------------- */
  void MemReset(void) { MemSetPtr(); MemSetSize(); }
  /* -- Swap members with another block ------------------------------------ */
  void MemConstSwap(MemConst &&dcOther)
    { swap(stSize, dcOther.stSize); swap(cpPtr, dcOther.cpPtr); }
  /* -- Bit test error handler --------------------------------------------- */
  void MemCheckBit[[noreturn]](const char*const cpWhat,
    const size_t stPos) const
  { // Get absolute position and maximum bit position
    const size_t stAbsPos = UtilBitToByte(stPos),
                 stMax = UtilBitFromByte(stSize);
    // Throw the error
    XC(cpWhat, "BitPosition",  stPos,    "BitMaximum",  stMax,
               "BytePosition", stAbsPos, "ByteMaximum", stSize,
               "AddrPosition", MemDoRead<void*>(stAbsPos),
               "AddrStart",    MemPtr(),    "AddrMaximum", MemPtrEnd());
  }
  /* -- Read pointer error handler------------------------------------------ */
  void MemCheckRead[[noreturn]](const char*const cpWhat, const size_t stPos,
    const size_t stBytes) const
      { XC(cpWhat, "Position",  stPos,  "Amount",  stBytes,
                   "Maximum",   stSize, "AddrPos", MemDoRead<void*>(stPos),
                   "AddrStart", MemPtr(),  "AddrMax", MemPtrEnd()); }
  /* -- Set size ----------------------------------------------------------- */
  void MemSetPtr(char*const cpNPtr = nullptr) { cpPtr = cpNPtr; }
  void MemSetSize(const size_t stNSize = 0) { stSize = stNSize; }
  void MemSetPtrSize(char*const cpNPtr, const size_t stNSize)
    { MemSetPtr(cpNPtr); MemSetSize(stNSize); }
  /* -- Free the pointer --------------------------------------------------- */
  void MemFreePtr(void) { UtilMemFree(MemPtr()); }
  void MemFreePtrIfSet(void) { if(MemIsPtrSet()) MemFreePtr(); }
  /* -- Character access by position ------------------------------- */ public:
  char &operator[](const size_t stPos) const
    { MemCheckParam(stPos, 1); return cpPtr[stPos]; }
  /* -- Return memory at the allocated address ----------------------------- */
  template<typename Type=void>
    Type *MemPtr(void) const { return reinterpret_cast<Type*>(cpPtr); }
  /* -- Return size of allocated memory ------------------------------------ */
  template<typename Type=size_t>
    Type MemSize(void) const { return static_cast<Type>(stSize); }
  /* -- Return if memory is allocated -------------------------------------- */
  bool MemIsEmpty(void) const { return MemSize() == 0; }
  bool MemIsNotEmpty(void) const { return !MemIsEmpty(); }
  /* -- Returns if the pointer is valid ------------------------------------ */
  bool MemIsPtrSet(void) const { return !!MemPtr(); }
  bool MemIsPtrNotSet(void) const { return !MemPtr(); }
  /* -- Return ending address ---------------------------------------------- */
  template<typename Type=void>
    Type *MemPtrEnd(void) const { return MemDoRead<Type>(stSize); }
  /* -- Return if we can read this amount of data -------------------------- */
  bool MemCheckParam(const size_t stPos, const size_t stBytes) const
  { // Return if bounds are safe
    return MemIsPtrSet()              && // Address is valid?
           stPos         <  MemSize() && // Position is <= array size?
           stBytes       <= MemSize() && // Bytes to copy is <= array size?
           stPos+stBytes <= MemSize();   // End position <= array size?
  }
  /* -- Same as MemCheckParam() with ptr check ----------------------------- */
  bool MemCheckPtr(const size_t stPos, const size_t stBytes,
    const void*const vpOther) const
      { return MemCheckParam(stPos, stBytes) && vpOther != nullptr; }
  /* -- Test a bit position ------------------------------------------------ */
  bool MemCheckPos(const size_t stPos) const
    { return UtilBitToByte(stPos) >= MemSize(); }
  /* -- Find specified string ---------------------------------------------- */
  size_t MemFind(const string &strMatch, size_t stPos=0) const
  { // Bail if parameters are invalid
    if(strMatch.empty() || MemIsEmpty() || stPos > MemSize())
      return StdMaxSizeT;
    // Position
    size_t stIndex;
    // Until end of string
    while(const char*const cpLoc = reinterpret_cast<char*>
      (memchr(MemDoRead(stPos), strMatch.front(), MemSize()-stPos)))
    { // Calculate index
      stPos = static_cast<size_t>(MemPtr<char>()-cpPtr);
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
    return StdMaxSizeT;
  }
  /* -- Read with byte bound check ----------------------------------------- */
  template<typename Type=char>
    Type *MemRead(const size_t stPos, const size_t stBytes=0) const
  { // Bail if size bad
    if(!MemCheckParam(stPos, stBytes))
      MemCheckRead("Read error!", stPos, stBytes);
    // Return pointer
    return MemDoRead<Type>(stPos);
  }
  /* -- Read static variable ----------------------------------------------- */
  template<typename Type>Type MemReadInt(const size_t stPos=0) const
    { return *MemRead<Type>(stPos, sizeof(Type)); }
  template<typename Type>Type ReadIntLE(const size_t stPos=0) const
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      return UtilToLittleEndian(MemReadInt<Type>(stPos)); }
  template<typename Type>Type ReadIntBE(const size_t stPos=0) const
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      return UtilToBigEndian(MemReadInt<Type>(stPos)); }
  /* -- Test a bit --------------------------------------------------------- */
  bool MemBitTest(const size_t stPos) const
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Test error!", stPos);
    // Return the tested bit
    return UtilBitTest(MemPtr<char>(), stPos);
  }
  /* -- Stringify the memory ----------------------------------------------- */
  const string MemToString(void) const
  { // Return empty string if no size
    if(MemIsEmpty()) return { };
    // Find a null character and if we found it? We know what the size is!
    if(const char*const cpLocPtr =
      reinterpret_cast<char*>(memchr(MemPtr<char>(), '\0', MemSize())))
        return { MemPtr<char>(),
          static_cast<size_t>(cpLocPtr-MemPtr<char>()) };
    // There is no null character so we have to limit the size
    return { MemPtr<char>(), MemSize() };
  }
  /* -- Return if current size would overflow specified type --------------- */
  template<typename Type=size_t>bool MemIsSizeOverflow(void) const
    { return UtilIntWillOverflow<Type>(MemSize()); }
  /* -- Init from string (does not copy) ----------------------------------- */
  explicit MemConst(const string &strSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    MemConst{ strSrc.length(),         // Copy string size and pointer over
      strSrc.data() }                  // from specified string
    /* -- Copy pointer and size over from string --------------------------- */
    { }
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  explicit MemConst(MemConst &&dSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    cpPtr(dSrc.MemPtr<char>()),        // Copy pointer
    stSize(dSrc.MemSize())             // Copy size
    /* -- Code to clear other MemConst ------------------------------------ */
    { dSrc.MemReset(); }
  /* -- Uninitialised constructor -- pointer ------------------------------- */
  MemConst(void) :
    /* -- Initialisers ----------------------------------------------------- */
    cpPtr(nullptr),                    // Start with null pointer
    stSize(0)                          // No size allocated yet
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Inherit an already allocated pointer ------------------------------- */
  MemConst(const size_t stNSize, char*const cpSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    cpPtr(cpSrc),                      // Initialise memory
    stSize(stNSize)                    // Set pointer size
    /* -- No code ---------------------------------------------------------- */
    { if(MemIsPtrNotSet() && MemIsNotEmpty())
        XC("Null pointer with non-zero memory size requested!",
           "MemSize", MemSize()); }
  /* -- Cast void pointer to char pointer ---------------------------------- */
  MemConst(const size_t stNSize, void*const vpSrc) :
    MemConst(stNSize, reinterpret_cast<char*>(vpSrc)) { }
  /* -- Cast const char pointer to char pointer ---------------------------- */
  MemConst(const size_t stNSize, const char*const cpSrc) :
    MemConst(stNSize, const_cast<char*>(cpSrc)) { }
  /* -- Cast const void pointer to const char ------------------------------ */
  MemConst(const size_t stNSize, const void*const vpSrc) :
    MemConst(stNSize, const_cast<void*>(vpSrc)) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(MemConst)            // Do not need defaults
};/* ----------------------------------------------------------------------- */
/* == Read and write data class ============================================ */
class MemBase :
  /* -- Base classes ------------------------------------------------------- */
  public MemConst                     // Start of MemBase Block Class
{ /* -- Copy memory --------------------------------------------- */ protected:
  void MemDoWrite(const size_t stPos, const void*const vpSrc,
    const size_t stBytes) { memcpy(MemDoRead(stPos), vpSrc, stBytes); }
  /* -- Fill with specified character at specifed position ----------------- */
  template<typename Type=unsigned char>void MemFill(const size_t stPos,
    const Type tVal, const size_t stBytes)
  { // Get address to start from
    char*const cpStart = MemDoRead(stPos);
    // Do the fill. This is supposedly faster than memset
    StdFill(par_unseq, reinterpret_cast<Type*const>(cpStart),
      reinterpret_cast<Type*const>(cpStart + stBytes), tVal);
  }
  /* -- Fill with specified character at specifed position --------- */ public:
  template<typename Type=unsigned char>void MemFillEx(const size_t stPos,
    const Type tVal, const size_t stBytes)
  { // Get end position and make sure it wont overrun
    if(!MemCheckParam(stPos, stBytes))
      MemCheckRead("Fill error!", stPos, stBytes);
    // Do the fill
    MemFill(stPos, tVal, stBytes);
  }
  /* -- Fill with specified value ------------------------------------------ */
  template<typename Type=unsigned char>void MemFill(const Type tVal=0)
    { MemFillEx(0, tVal, MemSize()); }
  /* -- Write memory with checks ---------------------------------------- */
  void MemWrite(const size_t stPos, const void*const vpSrc,
    const size_t stBytes)
  { // Check parameters are valid
    if(!MemCheckPtr(stPos, stBytes, vpSrc))
      XC("MemWrite error!", "Destination", MemPtr(),   "Source",   vpSrc,
                         "Bytes",       stBytes, "Position", stPos,
                         "Maximum",     MemSize());
    // Do copy
    MemDoWrite(stPos, vpSrc, stBytes);
  }
  /* -- Swap bits ---------------------------------------------------------- */
  void MemSwap8(const size_t stPos)
    { MemWriteInt<uint8_t>(stPos, UtilBitSwap4(MemReadInt<uint8_t>(stPos))); }
  void MemSwap16(const size_t stPos)
    { MemWriteInt<uint16_t>(stPos, SWAP_U16(MemReadInt<uint16_t>(stPos))); }
  void MemSwap32(const size_t stPos)
    { MemWriteInt<uint32_t>(stPos, SWAP_U32(MemReadInt<uint32_t>(stPos))); }
  void MemSwap64(const size_t stPos)
    { MemWriteInt<uint64_t>(stPos, SWAP_U64(MemReadInt<uint64_t>(stPos))); }
  /* -- Write specified variable as an integer ----------------------------- */
  template<typename Type>void MemWriteInt(const size_t stPos,
    const Type tVar)
      { MemWrite(stPos, &tVar, sizeof(tVar)); }
  template<typename Type>void MemWriteInt(const Type tVar)
    { MemWriteInt<Type>(0, tVar); }
  template<typename Type>void MemWriteIntLE(const size_t stPos,
    const Type tVar)
      { static_assert(is_integral_v<Type>, "Wrong type!");
        static_assert(sizeof(Type) > 1, "Wrong size!");
        MemWriteInt<Type>(stPos, UtilToLittleEndian(tVar)); }
  template<typename Type>void MemWriteIntLE(const Type tVar)
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      MemWriteInt<Type>(UtilToLittleEndian(tVar)); }
  template<typename Type>
    void MemWriteIntBE(const size_t stPos, const Type tVar)
      { static_assert(is_integral_v<Type>, "Wrong type!");
        static_assert(sizeof(Type) > 1, "Wrong size!");
        MemWriteInt<Type>(stPos, UtilToBigEndian(tVar)); }
  template<typename Type>void MemWriteIntBE(const Type tVar)
    { static_assert(is_integral_v<Type>, "Wrong type!");
      static_assert(sizeof(Type) > 1, "Wrong size!");
      MemWriteInt<Type>(UtilToBigEndian(tVar)); }
  /* -- Write specified variable as an floating point number --------------- */
  void MemWriteFloatLE(const float fVar) { MemWriteFloatLE(0, fVar); }
  void MemWriteFloatLE(const size_t stPos, const float fVar)
    { MemWriteInt<float>(stPos, UtilToF32LE(fVar)); }
  void MemWriteFloatBE(const float fVar) { MemWriteFloatBE(0, fVar); }
  void MemWriteFloatBE(const size_t stPos, const float fVar)
    { MemWriteInt<float>(stPos, UtilToF32BE(fVar)); }
  void MemWriteDoubleLE(const double dVar) { MemWriteDoubleLE(0, dVar); }
  void MemWriteDoubleLE(const size_t stPos, const double dVar)
    { MemWriteInt<double>(stPos, UtilToF64LE(dVar)); }
  void MemWriteDoubleBE(const double dVar) { MemWriteDoubleBE(0, dVar); }
  void MemWriteDoubleBE(const size_t stPos, const double dVar)
    { MemWriteInt<double>(stPos, UtilToF64BE(dVar)); }
  /* -- Write memory block at specified position --------------------------- */
  void MemWriteBlock(const size_t stPos, const MemConst &dData,
    const size_t stBytes) { MemWrite(stPos, dData.MemPtr<char>(), stBytes); }
  void MemWriteBlock(const size_t stPos, const MemConst &dData)
    { MemWriteBlock(stPos, dData, dData.MemSize()); }
  /* -- Set a bit ---------------------------------------------------------- */
  void MemBitSet(const size_t stPos)
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Set error!", stPos);
    // Set the bit
    UtilBitSet(MemPtr<char>(), stPos);
  }
  /* -- Clear a bit -------------------------------------------------------- */
  void MemBitClear(const size_t stPos)
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Clear error!", stPos);
    // Clear the bit
    UtilBitClear(MemPtr<char>(), stPos);
  }
  /* -- Flip a bit --------------------------------------------------------- */
  void MemBitFlip(const size_t stPos)
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Flip error!", stPos);
    // Flip the bit
    UtilBitFlip(MemPtr<char>(), stPos);
  }
  /* -- Invert specific flags ---------------------------------------------- */
  template<typename Type=uint8_t>void MemInvert(const size_t stPos,
    const Type tFlags=numeric_limits<Type>::max())
  { // Bail if size bad
    if(!MemCheckParam(stPos, sizeof(Type)))
      MemCheckRead("Invert error!", stPos, sizeof(Type));
    // Do the invert
    *reinterpret_cast<Type*const>(MemDoRead(stPos)) ^= tFlags;
  }
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  MemBase(MemBase &&dSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    MemConst{ StdMove(dSrc) }          // Move other data object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Take ownership of pointer (rvalue) --------------------------------- */
  explicit MemBase(MemConst &&mcSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    MemConst{ StdMove(mcSrc) }         // Move other data const object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Inherit an already allocated pointer ------------------------------- */
  MemBase(const size_t stNSize, char*const cpSrc) :
    MemConst{ stNSize, cpSrc } { }
  MemBase(const size_t stNSize, void*const vpSrc) :
    MemConst{ stNSize, vpSrc } { }
  MemBase(const size_t stNSize, const char*const cpSrc) :
    MemConst{ stNSize, cpSrc } { }
  MemBase(const size_t stNSize, const void*const vpSrc) :
    MemConst{ stNSize, vpSrc } { }
  /* -- Uninitialised constructor -- pointer ------------------------------- */
  MemBase(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(MemBase)             // Do not need defaults
};/* ----------------------------------------------------------------------- */
/* == Read, write and allocation data class ================================ */
class Memory :
  /* -- Base classes ------------------------------------------------------- */
  public MemBase                       // Start of Memory Block Class
{ /* -- Resize and preserve allocated memory ---------------------- */ private:
  void MemDoResize(const size_t stNSize)
  { // Realloc new amount of memory and if succeeded? Set new block and size
    if(char*const cpNew =
      UtilMemReAlloc(MemPtr<char>(), UtilMaximum(stNSize, 1)))
        MemSetPtrSize(cpNew, stNSize);
    // Failed so throw error
    else XC("Re-alloc failed!", "OldSize", MemSize(), "NewSize", stNSize);
  }
  /* -- Swap memory with another memory block ---------------------- */ public:
  void MemSwap(Memory &&mOther) { MemConstSwap(StdMove(mOther)); }
  /* -- Resize and preserve allocated memory ------------------------------- */
  void MemResize(const size_t stNSize)
    { if(stNSize != MemSize()) MemDoResize(stNSize); }
  /* -- Add allocated memory ----------------------------------------------- */
  void MemResizeAdd(const size_t stNSize) { MemDoResize(MemSize() + stNSize); }
  /* -- Resize memory upwards never downwards ------------------------------ */
  void MemResizeUp(const size_t stNSize)
    { if(stNSize > MemSize()) MemDoResize(stNSize); }
  /* -- Append the specified amount of memory ------------------------------ */
  void MemAppend(const void*const vpSrc, const size_t stBytes)
  { // Bail out if the pointer is invalid
    if(!vpSrc) XC("Source address to append invalid!");
    // Return if no bytes are to be copied
    if(!stBytes) return;
    // Position to write to
    const size_t stPos = MemSize();
    // Resize block to write memory
    MemDoResize(MemSize() + stBytes);
    // MemWrite data
    MemDoWrite(stPos, vpSrc, stBytes);
  }
  /* -- Resize a block from both ends -------------------------------------- */
  void MemCrop(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes))
      MemCheckRead("Crop error!", stPos, stBytes);
    // If position is from start? We just need to realloc
    if(!stPos) return MemDoResize(stBytes);
    // Init new class and transfer it to this one
    MemSwap({ stBytes-stPos, MemDoRead(stPos) });
  }
  /* -- Byte swap (Hi4<->Lo4 from 8-bit integer) --------------------------- */
  void MemByteSwap8(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes))
      MemCheckRead("8-bit swap error!", stPos, stBytes);
    // Reverse each byte
    for(uint8_t *ubPtr = MemDoRead<uint8_t>(stPos),
                *ubEnd = ubPtr + stBytes;
                 ubPtr < ubEnd;
               ++ubPtr)
      *ubPtr = UtilReverseByte(*ubPtr);
  }
  void MemByteSwap8(const size_t stBytes) { MemByteSwap8(0, stBytes); }
  void MemByteSwap8(void) { MemByteSwap8(MemSize()); }
  /* -- Byte swap (Hi8<->Lo8 from 16-bit integer) -------------------------- */
  void MemByteSwap16(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes) || stBytes % sizeof(uint16_t) > 0)
      MemCheckRead("16-bit swap error!", stPos, stBytes);
    // Reverse each word
    for(uint16_t *usPtr = MemDoRead<uint16_t>(stPos),
                 *usEnd = MemDoRead<uint16_t>(stPos + stBytes);
                  usPtr < usEnd;
                ++usPtr)
      *usPtr = SWAP_U16(*usPtr);
  }
  void MemByteSwap16(const size_t stBytes) { MemByteSwap16(0, stBytes); }
  void MemByteSwap16(void) { MemByteSwap16(MemSize()); }
  /* -- Byte swap (Hi16<->Lo16 from 32-bit integer) ------------------------ */
  void MemByteSwap32(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes) || stBytes % sizeof(uint32_t) > 0)
      MemCheckRead("32-bit swap error!", stPos, stBytes);
    // Reverse each dword
    for(uint32_t *ulPtr = MemDoRead<uint32_t>(stPos),
                 *ulEnd = MemDoRead<uint32_t>(stPos + stBytes);
                  ulPtr < ulEnd;
                ++ulPtr)
      *ulPtr = SWAP_U32(*ulPtr);
  }
  void MemByteSwap32(const size_t stBytes) { MemByteSwap32(0, stBytes); }
  void MemByteSwap32(void) { MemByteSwap32(MemSize()); }
  /* -- Byte swap (Hi32<->Lo32 from 64-bit integer) ------------------------ */
  void MemByteSwap64(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes) || stBytes % sizeof(uint64_t) > 0)
      MemCheckRead("64-bit swap error!", stPos, stBytes);
    // Reverse each quad
    for(uint64_t *uqPtr = MemDoRead<uint64_t>(stPos),
                 *uqEnd = MemDoRead<uint64_t>(stPos + stBytes);
                  uqPtr < uqEnd;
                ++uqPtr)
      *uqPtr = SWAP_U64(*uqPtr);
  }
  void MemByteSwap64(const size_t stBytes) { MemByteSwap64(0, stBytes); }
  void MemByteSwap64(void) { MemByteSwap64(MemSize()); }
  /* -- Reverse at the position for number of bytes ------------------------ */
  void MemReverse(const size_t stPos, const size_t stBytes)
  { // Ignore if length not set and treat as if succeeded
    if(!stBytes) return;
    // Bail if size bad
    if(!MemCheckParam(stPos, stBytes))
      MemCheckRead("Reverse error!", stPos, stBytes);
    // Create empty memory block
    Memory mDst{ stBytes };
    // Copy each byte from the start of source, to the end of destination
    for(size_t stIndex = 0; stIndex < stBytes; ++stIndex)
      mDst.MemDoWrite(MemSize() - stIndex - 1, MemDoRead(stIndex), 1);
    // Assign new memory block
    MemSwap(StdMove(mDst));
  }
  /* -- Reverse the specified number of bytes------------------------------- */
  void MemReverse(const size_t stBytes) { MemReverse(0, stBytes); }
  /* -- Reverse the entire memory block ------------------------------------ */
  void MemReverse(void) { MemReverse(MemSize()); }
  /* -- Free the allocated memory and reset members ------------------------ */
  void MemDeInit(void)
    { if(MemIsPtrSet()) { MemFreePtr(); MemSetPtr(); } MemSetSize(); }
  /* -- Allocate memory ---------------------------------------------------- */
  void MemInitBlank(const size_t stBytesRequested=0)
  { // If allocated memory already exists? Free it!
    MemFreePtrIfSet();
    // Allocate memory forcing zero bytes to 1 byte for compatibility.
    if(char*const cpNew = UtilMemAlloc<char>(UtilMaximum(stBytesRequested, 1)))
      return MemSetPtrSize(cpNew, stBytesRequested);
    // The memory was freed so this memory is no longer available.
    MemSetSize();
    // Failed so throw an exception.
    XC("Alloc failed!", "MemSize", stBytesRequested);
  }
  /* -- Allocate and copy from existing memory ----------------------------- */
  void MemInitData(const size_t stS, const void*const vpB)
    { MemInitBlank(stS); if(stS && vpB) MemWrite(0, vpB, stS); }
  /* -- Allocate and copy from existing string ----------------------------- */
  void MemInitString(const string &strS)
    { MemInitData(strS.length(), reinterpret_cast<const void*>(strS.data())); }
  /* -- Allocate and copy from existing memory block ----------------------- */
  void MemInitCopy(const MemConst &mcSrc)
    { MemInitData(mcSrc.MemSize(), mcSrc.MemPtr()); }
  /* -- Allocate and zero memory ------------------------------------------- */
  void MemInitSafe(const size_t stS) { MemInitBlank(stS); MemFill(); }
  /* -- Assignment operator (rvalue) ------------------------------------ -- */
  Memory &operator=(Memory &&mbSrc)
    { MemSwap(StdMove(mbSrc)); return *this; }
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  Memory(Memory &&mbSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ StdMove(mbSrc) }          // Move other memory object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Take ownership of pointer (must originally be malloc'd) ------------ */
  explicit Memory(MemBase &&dSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ StdMove(dSrc) }           // Move other data object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Take ownership of pointer (must originally be malloc'd) ------------ */
  explicit Memory(MemConst &&mcSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ StdMove(mcSrc) }          // Move other read only memory object
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
  Memory(const size_t stNSize, const void*const vpSrc, const bool) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ stNSize, vpSrc }          // Take ownership of pointer
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Alloc uninitialised ------------------------------------------------ */
  explicit Memory(const size_t stNSize) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ stNSize,                  // Initialise data base class
      UtilMemAlloc<void>               // Allocate memory (checked by CTOR)
        (UtilMaximum(stNSize, 1)) }    // Allocate requested size
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Alloc with fill ---------------------------------------------------- */
  Memory(const size_t stNSize, const bool) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ stNSize }                  // Allocate memory
    /* -- Full memory with zeros ------------------------------------------- */
    { MemFill(); }
  /* -- Alloc with copy ---------------------------------------------------- */
  Memory(const size_t stNSize, const void*const vpSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ stNSize }                  // Allocate size of pointer
    /* -- Code to initialise pointer --------------------------------------- */
    { if(vpSrc) MemDoWrite(0, vpSrc, stNSize); }
  /* -- Standby constructor ------------------------------------------------ */
  Memory(void) { }
  /* -- Destructor (just a free() needed) ---------------------------------- */
  ~Memory(void) { MemFreePtrIfSet(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Memory)              // Do not need defaults
};/* -- Useful types ------------------------------------------------------- */
typedef list<Memory> MemoryList;       // List of memory blocks
typedef vector<Memory> MemoryVector;   // A vector of memory classes
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
