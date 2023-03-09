/* == IMAGE.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines a class that can load and save image files and  ## */
/* ## then can optionally be sent to opengl for viewing.                  ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfImage {                    // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfImageFormat;         // Using imageformat interface
/* -- Loading flags -------------------------------------------------------- */
BUILD_FLAGS(Image,
  /* -- Post processing ---------------------------------------------------- */
  // No flags                          // Image will be loadable in OpenGL
  IL_NONE                {0x00000000}, IL_TOGPU               {0x00000002},
  // Convert loaded image to 24bpp     // Convert loaded image to 32bpp
  IL_TO24BPP             {0x00000004}, IL_TO32BPP             {0x00000008},
  // Convert loaded image to BGR(A)    Convert loaded image to RGB(A)
  IL_TOBGR               {0x00000010}, IL_TORGB               {0x00000020},
  // Convert loaded image to BINARY    Force reverse the image
  IL_TOBINARY            {0x00000040}, IL_REVERSE             {0x00000080},
  /* -- Formats ------------------------------------------------------------ */
  // Force load as TARGA               Force load as JPEG
  IL_FCE_TGA             {0x02000000}, IL_FCE_JPG             {0x04000000},
  // Force load as PNG                 Force load as GIF
  IL_FCE_PNG             {0x08000000}, IL_FCE_GIF             {0x20000000},
  // Force load as DDS
  IL_FCE_DDS             {0x40000000},
  /* -- Mask bits ---------------------------------------------------------- */
  IL_MASK{ IL_TO32BPP|IL_TOBGR|IL_TORGB|IL_TOBINARY|IL_REVERSE|
    IL_FCE_TGA|IL_FCE_JPG|IL_FCE_PNG|IL_FCE_GIF|IL_FCE_DDS }
);/* == Image collector and member class =================================== */
BEGIN_ASYNCCOLLECTORDUO(Images, Image, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoader<Image>,           // For loading images off main thread
  public Lockable,                     // Lua garbage collector instruction
  public ImageFlags                    // Image load settings
{ /* -- Variables -------------------------------------------------- */ public:
  ImageData             imgData;       // Image data
  /* -- ImageData retrieval ------------------------------------------------ */
  SlotList &GetSlotsData(void) { return imgData.sData; }
  bool NoSlots(void) const { return imgData.sData.empty(); }
  ImageSlot &GetSlot(const size_t stId) { return imgData.sData[stId]; }
  ImageSlot &GetFirstSlot(void) { return GetSlot(0); }
  size_t GetSlots(void) const { return imgData.sData.size(); }
  size_t GetSlotsMax(void) const { return imgData.sData.capacity(); }
  size_t GetSize(void) const { return imgData.stFile; }
  size_t GetUsage(void) const { return imgData.stAlloc; }
  bool IsMipmap(void) const { return imgData.bMipmaps; }
  bool IsReverse(void) const { return imgData.bReverse; }
  bool IsCompressed(void) const { return imgData.bCompressed; }
  bool IsDynamic(void) const { return imgData.bDynamic; }
  GLenum GetPixelType(void) const { return imgData.glPixelType; }
  template<typename IntType=decltype(imgData.uiWidth)>
    IntType GetWidth(void) const
      { return static_cast<IntType>(imgData.uiWidth); }
  template<typename IntType=decltype(imgData.uiHeight)>
    IntType GetHeight(void) const
      { return static_cast<IntType>(imgData.uiHeight); }
  template<typename IntType=decltype(imgData.uiBitsPerPixel)>
    IntType GetBitsPP(void) const
      { return static_cast<IntType>(imgData.uiBitsPerPixel); }
  template<typename IntType=decltype(imgData.uiBytesPerPixel)>
    IntType GetBytesPP(void) const
      { return static_cast<IntType>(imgData.uiBytesPerPixel); }
  /* ----------------------------------------------------------------------- */
  void SwapImage(Image &oCref)
  { // Swap async, lua lock data and registration
    IdentSwap(oCref);
    LockSwap(oCref);
    CollectorSwapRegistration(oCref);
    FlagSwap(oCref);
    // Swap image lib data
    swap(imgData.uiWidth, oCref.imgData.uiWidth);
    swap(imgData.uiHeight, oCref.imgData.uiHeight);
    swap(imgData.uiBitsPerPixel, oCref.imgData.uiBitsPerPixel);
    swap(imgData.uiBytesPerPixel, oCref.imgData.uiBytesPerPixel);
    swap(imgData.glPixelType, oCref.imgData.glPixelType);
    swap(imgData.bMipmaps, oCref.imgData.bMipmaps);
    swap(imgData.bReverse, oCref.imgData.bReverse);
    swap(imgData.bCompressed, oCref.imgData.bCompressed);
    swap(imgData.bDynamic, oCref.imgData.bDynamic);
    swap(imgData.stAlloc, oCref.imgData.stAlloc);
    swap(imgData.stFile, oCref.imgData.stFile);
    imgData.sData.swap(oCref.imgData.sData);
  }
  /* -- Clear allocated data ----------------------------------------------- */
  void ClearData(void) { imgData.sData.clear(); imgData.stAlloc = 0; }
  /* -- Force the specified colour mode ------------------------------------ */
  void ForcePixelOrder(const unsigned int uiCM)
  { // Force a RGB colour mode?
    if((uiCM == GL_BGR &&
       (imgData.glPixelType == GL_RGBA || imgData.glPixelType == GL_RGB)) ||
       (uiCM == GL_RGB &&
       (imgData.glPixelType == GL_BGRA || imgData.glPixelType == GL_BGR)))
    { // For each slot
      for(ImageSlot &sItem : imgData.sData)
      { // Get memory
        Memory &mbIn = sItem.memData;
        // Swap pixels
        const int iResult = ImageSwapPixels(mbIn.Ptr<char>(), mbIn.Size(),
          imgData.uiBytesPerPixel, 0, 2);
        if(iResult < 0) XC("Pixel reorder failed!",
          "Identifier",    IdentGet(),
          "Code",          iResult,
          "FromFormat",    imgData.glPixelType,
          "ToFormat",      uiCM,
          "BytesPerPixel", imgData.uiBytesPerPixel,
          "Address",       mbIn.Ptr<void>(),
          "Length",        mbIn.Size());
      } // Set new pixel type
      switch(imgData.glPixelType)
      { case GL_RGBA : imgData.glPixelType = GL_BGRA; break;
        case GL_RGB  : imgData.glPixelType = GL_BGR;  break;
        case GL_BGRA : imgData.glPixelType = GL_RGBA; break;
        case GL_BGR  : imgData.glPixelType = GL_RGB;  break;
      } // Log operation
      LW(LH_DEBUG, "Image '$' pixel type converted from 0x$$ to 0x$.",
        IdentGet(), hex, imgData.glPixelType, uiCM);
    }
  }
  /* -- Force binary mode -------------------------------------------------- */
  void ForceBinary(void)
  { // Ignore if already one bit or there are no slots
    if(imgData.uiBitsPerPixel == 1 || imgData.sData.empty()) return;
    // Throw error if compressed
    if(imgData.bCompressed)
      XC("Cannot binary downsample a compressed image!",
         "File", IdentGet());
    // Must be 32bpp
    if(imgData.uiBitsPerPixel != 32)
      XC("Cannot binary downsample a non RGBA encoded image!",
         "File", IdentGet(), "BytesPerPixel", imgData.uiBytesPerPixel);
    // Calculate destination size
    const size_t stDst = imgData.uiWidth * imgData.uiHeight / 8;
    // Must be divisible by eight
    if(!IsDivisible(static_cast<double>(stDst)))
      XC("Image size not divisible by eight!",
         "File", IdentGet(), "Size", stDst);
    // Bits lookup table
    static const array<const unsigned char,8>
      uiaBits{1, 2, 4, 8, 16, 32, 64, 128};
    // For each image slot
    for(ImageSlot &imgSlot : imgData.sData)
    { // Destination image for binary data
      Memory &aSrc = imgSlot.memData, aDst{ stDst };
      // For each byte in the alpha channel array
      for(size_t stIndex = 0, stSubIndex = 0, stLimit = aSrc.Size();
                 stIndex < stLimit;
                 stIndex += 8, ++stSubIndex)
      { // Init bits
        unsigned char ucBits = 0;
        // Set bits depending on image bytes
        for(size_t stBitIndex = 0; stBitIndex < 8; ++stBitIndex)
          if(aSrc.ReadInt<uint8_t>(stBitIndex) > 0)
            ucBits |= uiaBits[stBitIndex];
        // Write new bit value
        aDst.WriteInt<uint8_t>(stSubIndex, ucBits);
      } // Update slot data
      aSrc.SwapMemory(move(aDst));
    } // Update image data
    imgData.uiBitsPerPixel = 1;
    imgData.uiBytesPerPixel = 1;
    imgData.glPixelType = GL_RED;
    imgData.stAlloc = imgData.stFile = stDst;
    // Log operation
    LW(LH_DEBUG, "Image '$' pixel components forced to binary.", IdentGet());
  }
  /* -- Reverse pixels ----------------------------------------------------- */
  void ReversePixels(void)
  { // Return if no slots
    if(imgData.sData.empty()) return;
    // Monochrome image?
    if(imgData.uiBitsPerPixel == 1)
      // Enumerate through each slot and swap the 4-bits in each 8-bit byte
      for(ImageSlot &sItem : imgData.sData) sItem.memData.ByteSwap8();
    // Bits >= 8?
    else if(imgData.uiBitsPerPixel >= 8)
    { // Enumerate through each slot
      for(ImageSlot &sItem : imgData.sData)
      { // Get reference to data and source memblock
        const Memory &mbIn = sItem.memData;
        // Create new mem block to write to
        Memory mbOut(mbIn.Size());
        // Pixel size
        const size_t stStep = imgData.uiBytesPerPixel * sItem.uiWidth;
        // Copy the pixels
        for(size_t stI = 0; stI < mbIn.Size(); stI += stStep)
          mbOut.Write(stI, mbIn.Read(mbIn.Size()-stStep-stI), stStep);
        // Set new mem block for this class automatically unloading the old
        // one
        sItem.memData.SwapMemory(move(mbOut));
      }
    } // Log operation
    LW(LH_DEBUG, "Image '$' flipped.", IdentGet());
  }
  /* -- Pixel conversion process ------------------------------------------- */
  template<class PixelConversionFunction, size_t stSrcStep, size_t stDstStep,
    unsigned int uiNewBPP, GLenum glNewPixelType>void ConvertPixels(void)
  { // Calculate bytes per pixel
    const unsigned int uiBytesPerPixel = uiNewBPP / 8;
    // For each slot
    for(ImageSlot &sItem : imgData.sData)
    { // Get source memory block
      Memory &aSrc = sItem.memData;
      // Make a new memblock for the destinaiton pixels
      Memory aDst{ sItem.uiWidth * sItem.uiHeight * uiBytesPerPixel };
      // Iterate through the array
      for(uint8_t *cpSrc = aSrc.Ptr<uint8_t>(),
         *const cpSrcEnd = aSrc.PtrEnd<uint8_t>(),
                  *cpDst = aDst.Ptr<uint8_t>();
                   cpSrc < cpSrcEnd;
                   cpSrc += stSrcStep,
                   cpDst += stDstStep)
        // Call pixel function conversion
        PixelConversionFunction(cpSrc, cpDst);
      // Move memblocks
      imgData.stAlloc -= aSrc.Size();
      aSrc.SwapMemory(move(aDst));
      imgData.stAlloc += aSrc.Size();
    } // Save old bits per pixel for log because we're updating it
    const unsigned int uiOldBPP = imgData.uiBitsPerPixel;
    // Save old data size
    const size_t stOldFile = imgData.stFile;
    // Update pixel information
    imgData.uiBitsPerPixel = uiNewBPP;
    imgData.uiBytesPerPixel = uiBytesPerPixel;
    imgData.glPixelType = glNewPixelType;
    // Update actual data size
    imgData.stFile = imgData.stAlloc;
    // Log operation
    LW(LH_DEBUG, "Image '$' pixels changed (B:$>$;C:$;S:$>$).", IdentGet(),
      uiOldBPP, uiNewBPP, imgData.sData.size(), stOldFile, imgData.stFile);
  }
  /* -- Force luminance alpha pixel to RGB pixel type ---------------------- */
  void ConvertLuminanceAlphaToRGB(void)
  { // Class to convert a luminance alpha pixel to a RGB pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Unpack one luminance alpha pixel into one RGB pixel
      *reinterpret_cast<uint16_t*>(cpDst) =
        (static_cast<uint16_t>(*cpSrc) * 0x0101);
      *reinterpret_cast<uint8_t*>(cpDst+1) = 0xFF;
    } };
    // Do the conversion of luminance alpha to RGB
    ConvertPixels<Filter, 2, 3, 24, GL_RGB>();
  }
  /* -- Force luminance pixel to RGB pixel type ---------------------------- */
  void ConvertLuminanceToRGB(void)
  { // Class to convert a luminance pixel to a RGB pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Unpack one luminance pixel into one RGB pixel
      *reinterpret_cast<uint16_t*>(cpDst) = *cpSrc * 0x101;
      *reinterpret_cast<uint8_t*>(cpDst+2) = *cpSrc;
    } };
    // Do the conversion of luminance to RGB
    ConvertPixels<Filter, 1, 3, 24, GL_RGB>();
  }
  /* -- Force luminance alpha pixel to RGBA pixel type --------------------- */
  void ConvertLuminanceAlphaToRGBA(void)
  { // Class to convert a luminance alpha pixel to a RGBA pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Unpack one luminance alpha pixel into one RGBA pixel
      *reinterpret_cast<uint32_t*>(cpDst) =
        (static_cast<uint32_t>(*cpSrc) * 0x00010101) |
        (static_cast<uint32_t>(*(cpSrc+1)) * 0x01000000);
    } };
    // Do the conversion of luminance alpha to RGBA
    ConvertPixels<Filter, 2, 4, 32, GL_RGBA>();
  }
  /* -- Force luminance pixel to RGBA pixel type --------------------------- */
  void ConvertLuminanceToRGBA(void)
  { // Class to convert a luminance pixel to a RGBA pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Unpack one luminance pixel into one RGBA pixel ignoring alpha
      *reinterpret_cast<uint32_t*>(cpDst) = (*cpSrc * 0x01010100) | 0xFF;
    } };
    // Do the conversion of luminance alpha to RGBA
    ConvertPixels<Filter, 1, 4, 32, GL_RGBA>();
  }
  /* -- Force binary pixel to luminance pixel type ------------------------- */
  void ConvertBinaryToLuminance(void)
  { // Class to convert a BINARY pixel to a LUMINANCE pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Get the packed eight pixels
      const unsigned int uiPixels = *cpSrc;
      // Unpack eight binary pixels into eight luminance pixels
      *reinterpret_cast<uint64_t*>(cpDst) =
        (uiPixels & 0x01 ? 0xff00000000000000 : 0x0000000000000000) |
        (uiPixels & 0x02 ? 0x00ff000000000000 : 0x0000000000000000) |
        (uiPixels & 0x04 ? 0x0000ff0000000000 : 0x0000000000000000) |
        (uiPixels & 0x08 ? 0x000000ff00000000 : 0x0000000000000000) |
        (uiPixels & 0x10 ? 0x00000000ff000000 : 0x0000000000000000) |
        (uiPixels & 0x20 ? 0x0000000000ff0000 : 0x0000000000000000) |
        (uiPixels & 0x40 ? 0x000000000000ff00 : 0x0000000000000000) |
        (uiPixels & 0x80 ? 0x00000000000000ff : 0x0000000000000000);
    } };
    // Do the conversion of binary to luminance
    ConvertPixels<Filter, 1, 8, 8, GL_RED>();
  }
  /* -- Force binary pixel to RGB pixel type ------------------------------- */
  void ConvertBinaryToRGB(void)
  { // Class to convert a BINARY pixel to a RGB pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Get the packed eight pixels
      const unsigned int uiPixels = *cpSrc;
      // Unpack eight BINARY pixels into right RGB pixels
      *reinterpret_cast<uint64_t*>(cpDst) =
        (uiPixels & 0x80 ? 0xffffff0000000000 : 0x0000000000000000) |
        (uiPixels & 0x40 ? 0x000000ffffff0000 : 0x0000000000000000) |
        (uiPixels & 0x20 ? 0x000000000000ffff : 0x0000000000000000);
      *(reinterpret_cast<uint64_t*>(cpDst)+1) =
        (uiPixels & 0x20 ? 0xff00000000000000 : 0x0000000000000000) |
        (uiPixels & 0x10 ? 0x00ffffff00000000 : 0x0000000000000000) |
        (uiPixels & 0x08 ? 0x00000000ffffff00 : 0x0000000000000000) |
        (uiPixels & 0x04 ? 0x00000000000000ff : 0x0000000000000000);
      *(reinterpret_cast<uint64_t*>(cpDst)+2) =
        (uiPixels & 0x04 ? 0xffff000000000000 : 0x0000000000000000) |
        (uiPixels & 0x02 ? 0x0000ffffff000000 : 0x0000000000000000) |
        (uiPixels & 0x01 ? 0x0000000000ffffff : 0x0000000000000000);
    } };
    // Do the conversion of binary to RGBA
    ConvertPixels<Filter, 1, 24, 24, GL_RGB>();
  }
  /* -- Force binary pixel to RGBA pixel type ------------------------------ */
  void ConvertBinaryToRGBA(void)
  { // Class to convert a BINARY pixel to a RGBA pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Get the packed eight pixels
      const unsigned int uiPixels = *cpSrc;
      // Unpack eight BINARY pixels into eight RGBA pixels
      *reinterpret_cast<uint64_t*>(cpDst) =
        (uiPixels & 0x80 ? 0xffffffff00000000 : 0x0000000000000000) |
        (uiPixels & 0x40 ? 0x00000000ffffffff : 0x0000000000000000);
      *(reinterpret_cast<uint64_t*>(cpDst)+1) =
        (uiPixels & 0x20 ? 0xffffffff00000000 : 0x0000000000000000) |
        (uiPixels & 0x10 ? 0x00000000ffffffff : 0x0000000000000000);
      *(reinterpret_cast<uint64_t*>(cpDst)+2) =
        (uiPixels & 0x08 ? 0xffffffff00000000 : 0x0000000000000000) |
        (uiPixels & 0x04 ? 0x00000000ffffffff : 0x0000000000000000);
      *(reinterpret_cast<uint64_t*>(cpDst)+3) =
        (uiPixels & 0x02 ? 0xffffffff00000000 : 0x0000000000000000) |
        (uiPixels & 0x01 ? 0x00000000ffffffff : 0x0000000000000000);
    } };
    // Do the conversion of binary to RGBA
    ConvertPixels<Filter, 1, 32, 32, GL_RGBA>();
  }
  /* -- Load specified image ----------------------------------------------- */
  void LoadData(FileMap &fC)
  { // Force load a certain type of image (for speed?) but in Async mode,
    // force detection doesn't really matter as much, but overall, still
    // needed if speed is absolutely neccesary.
    if     (FlagIsSet(IL_FCE_TGA)) ImageLoad(0, fC, imgData);
    else if(FlagIsSet(IL_FCE_JPG)) ImageLoad(1, fC, imgData);
    else if(FlagIsSet(IL_FCE_PNG)) ImageLoad(2, fC, imgData);
    else if(FlagIsSet(IL_FCE_GIF)) ImageLoad(3, fC, imgData);
    else if(FlagIsSet(IL_FCE_DDS)) ImageLoad(4, fC, imgData);
    // Auto detection of image
    else ImageLoad(fC, imgData);
    // Record total size of raw pixel data
    imgData.stFile = imgData.stAlloc;
    // Do not apply filters on compressed textures or if no load flags set
    if(imgData.bCompressed || FlagIsZero()) return;
    // We don't have functionality to load <8bpp images in GPU yet so if this
    // flag is specified, we will make sure <8bpp textures get converted
    // properly so they can be loaded by OpenGL. This is useful if the
    // bit-depth of the bitmap isn't known and the caller wants to make sure
    // it guarantees to be loaded into OpenGL.
    if(FlagIsSet(IL_TOGPU)) switch(imgData.uiBitsPerPixel)
    { // 1bpp (BINARY)
      case 1: ConvertBinaryToLuminance(); break;
      // Ignore anything else
      default: break;
    } // If a request to convert to RGB?
    else if(FlagIsSet(IL_TO24BPP)) switch(imgData.uiBitsPerPixel)
    { // 1bpp (BINARY)
      case 1: ConvertBinaryToRGB(); break;
      // 8bpp (LUMINANCE)
      case 8: ConvertLuminanceToRGB(); break;
      // 16bpp (LUMINANCE+ALPHA)
      case 16: ConvertLuminanceAlphaToRGB(); break;
      // Ignore anything else
      default: break;
    } // If a request to convert to RGBA?
    else if(FlagIsSet(IL_TO32BPP)) switch(imgData.uiBitsPerPixel)
    { // 1bpp (BINARY)
      case 1: ConvertBinaryToRGBA(); break;
      // 8bpp (LUMINANCE)
      case 8: ConvertLuminanceToRGBA(); break;
      // 16bpp (LUMINANCE+ALPHA)
      case 16: ConvertLuminanceAlphaToRGBA(); break;
      // Ignore anything else
      default: break;
    } // To BGR colour mode?
    if(FlagIsSet(IL_TOBGR)) ForcePixelOrder(GL_BGR);
    // To RGB colour mode
    else if(FlagIsSet(IL_TORGB)) ForcePixelOrder(GL_RGB);
    // Reverse the image pixels
    if(FlagIsSet(IL_REVERSE)) ReversePixels();
    // To binary colour mode
    if(FlagIsSet(IL_TOBINARY)) ForceBinary();
  }
  /* -- Reload specified image --------------------------------------------- */
  void ReloadData(void)
    { FileMap fmData{ AssetExtract(IdentGet()) }; LoadData(fmData); }
  /* -- Save image using a type id ----------------------------------------- */
  void SaveFile(const string &strFN, const size_t stSId, const size_t stPId)
    { ImageSave(stPId, strFN, imgData, GetSlot(stSId)); }
  /* -- Load image from memory asynchronously ------------------------------ */
  void InitAsyncArray(lua_State*const lS)
  { // Need 6 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 7);
    // Get and check parameters
    const string strF{ GetCppStringNE(lS, 1, "Identifier") };
    Asset &aData = *GetPtr<Asset>(lS, 2, "Asset");
    FlagReset(GetFlags(lS, 3, IL_NONE, IL_MASK, "Flags"));
    CheckFunction(lS, 4, "ErrorFunc");
    CheckFunction(lS, 5, "ProgressFunc");
    CheckFunction(lS, 6, "SuccessFunc");
    // The decoded image will be kept in memory
    imgData.bDynamic = true;
    // Load image from memory asynchronously
    AsyncInitArray(lS, strF, "bmparray", move(aData));
  }
  /* -- Load image from file asynchronously -------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // Need 5 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 6);
    // Get and check parameters
    const string strF{ GetCppFileName(lS, 1, "File") };
    FlagReset(GetFlags(lS, 2, IL_NONE, IL_MASK, "Flags"));
    CheckFunction(lS, 3, "ErrorFunc");
    CheckFunction(lS, 4, "ProgressFunc");
    CheckFunction(lS, 5, "SuccessFunc");
    // Load image from file asynchronously
    AsyncInitFile(lS, strF, "bmpfile");
  }
  /* -- Create a blank image for working on -------------------------------- */
  void InitBlank(const string &strN, const unsigned int uiBWidth,
    const unsigned int uiBHeight, const bool bAlpha, const bool bClear)
  { // If alpha requested?
    if(bAlpha)
    { // Set a 32bpp transparent surface
      imgData.uiBitsPerPixel = 32;
      imgData.glPixelType = GL_RGBA;
      imgData.uiBytesPerPixel = 4;
    } // If alpha not requested?
    else
    { // Set a 24bpp opaque surface
      imgData.uiBitsPerPixel = 24;
      imgData.glPixelType = GL_RGB;
      imgData.uiBytesPerPixel = 3;
    } // Set other members
    IdentSet(strN);
    imgData.stFile = uiBWidth * uiBHeight * imgData.uiBytesPerPixel;
    imgData.bDynamic = true;
    imgData.uiWidth = uiBWidth;
    imgData.uiHeight = uiBHeight;
    imgData.stAlloc = imgData.stFile;
    // Add the raw data into a slot
    imgData.sData.push_back({ { imgData.stFile, bClear },
      uiBWidth, uiBHeight });
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Load image from a raw image ---------------------------------------- */
  void InitRaw(const string &strN, Memory &&aSrc,
    const unsigned int uiBWidth, const unsigned int uiBHeight,
    const unsigned int uiBitsPP, const GLenum ePixType)
  { // Error if no data specified
    if(aSrc.Empty()) XC("Image data is empty!", "File", strN);
    // Maximum size is 16-bit
    if(uiBWidth > 0xFFFF || uiBHeight > 0xFFFF)
      XC("Image dimensions are out of 16-bit integer range!",
        "File", strN, "Width", uiBWidth, "Height", uiBHeight);
    // Check bitrate
    switch(uiBitsPP)
    { // Allowed bit-rates are 1, 8, 16, 24 and 32 bits per pixel
      case 1: case 8: case 16: case 24: case 32: break;
      // Error
      default: XC("Image bit-depth is not valid!",
        "File", strN, "Depth", uiBitsPP);
    } // Check the size
    imgData.uiBytesPerPixel = uiBitsPP / 8;
    imgData.stFile = aSrc.Size();
    const size_t stExpect = uiBWidth * uiBHeight * imgData.uiBytesPerPixel;
    if(stExpect != imgData.stFile)
      XC("Arguments are not valid for specified image data!",
        "File", strN, "Expect", stExpect, "Actual",   imgData.stFile);
    // Everything looks OK, set rest of the members
    IdentSet(strN);
    imgData.uiBitsPerPixel = uiBitsPP;
    imgData.glPixelType = ePixType;
    imgData.bDynamic = true;
    imgData.bMipmaps =
      imgData.bReverse =
      imgData.bCompressed = false;
    imgData.uiWidth = uiBWidth;
    imgData.uiHeight = uiBHeight;
    imgData.stAlloc = imgData.stFile;
    // Add the raw data into a slot
    imgData.sData.push_back({ move(aSrc), uiBWidth, uiBHeight });
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Load image from a single colour ------------------------------------ */
  void InitColour(const uint32_t ulColour)
  { // Set members
    IdentSet(Append("solid/0x", hex, ulColour));
    imgData.uiBitsPerPixel = 32;
    imgData.uiBytesPerPixel = 4;
    imgData.glPixelType = GL_RGBA;
    imgData.stFile = sizeof(ulColour);
    imgData.bDynamic = true;
    imgData.uiWidth = imgData.uiHeight = 1;
    imgData.stAlloc = imgData.stFile;
    // Make sure the specified colour is in the correct order that the GPU can
    // read. It will be interpreted as GL_RGBA. The guest will specify the
    // input as 0xAARRGGBB like FboItem::SetRGBAInt().
    const array<const uint8_t,4>ucaColour{  // Source ----- Bit - Dst Pixels
      static_cast<uint8_t>(ulColour >> 16), // 0x00[RR]0000 16-24 [0] [R]gba
      static_cast<uint8_t>(ulColour >>  8), // 0x0000[GG]00 08-16 [1] r[G]ba
      static_cast<uint8_t>(ulColour      ), // 0x000000[BB] 00-08 [2] rg[B]a
      static_cast<uint8_t>(ulColour >> 24)  // 0x[AA]000000 24-32 [3] rgb[A]
    };                                      // ------------ ----- --- ------
    // Add the image
    imgData.sData.push_back({
      { imgData.uiBytesPerPixel, ucaColour.data() }, 1, 1 });
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Init from file ----------------------------------------------------- */
  void InitFile(const string &strFilename, const ImageFlagsConst &lfS)
  { // Set the loading flags
    FlagReset(lfS);
    // Load the file normally
    SyncInitFileSafe(strFilename);
  }
  /* -- Init from array ---------------------------------------------------- */
  void InitArray(const string &strName, Memory &&mbD,
    const ImageFlagsConst &lfS)
  { // Is dynamic because it was not loaded from disk
    imgData.bDynamic = true;
    // Set the loading flags
    FlagReset(lfS);
    // Load the array normally
    SyncInitArray(strName, move(mbD));
  }
  /* -- Get slot image data ------------------------------------------------ */
  Memory &GetData(const size_t stId=0) { return GetSlot(stId).memData; }
  /* -- Constructor -------------------------------------------------------- */
  Image(void) :                        // Default constructor
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperImage(*cImages),           // Initialise collector helper
    AsyncLoader<Image>(this,           // Initialise async loader
      EMC_MP_IMAGE),                   // Initialise async event id
    ImageFlags(IL_NONE),               // No image loading flags
    imgData{}                          // Clear image data
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise a 1x1 pixel texture
    /* -- Parameters ------------------------------------------------------- */
    const uint32_t uiColour            // 32-bit RGBA colour pixel value
    ): /* -- Initialisation of members ------------------------------------- */
    Image()                            // Default initialisation
    /* -- Initialise a 1x1 with the specified colour ----------------------- */
    { InitColour(uiColour); }
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise from RAW pixel data
    /* -- Parameters ------------------------------------------------------- */
    const string &strName,             // Name of the object
    Memory &&aSource,                  // Source pixel data
    const unsigned int uiWidth,        // Number of pixels in each scanline
    const unsigned int uiHeight,       // Number of scan lines
    const unsigned int uiBits,         // Nit depth of the pixel data
    const GLenum eFormat               // OpenGL pixel format
    ): /* -- Initialisation of members ------------------------------------- */
    Image()                            // Default initialisation
    /* -- Initialise raw image --------------------------------------------- */
    { InitRaw(strName, move(aSource), uiWidth, uiHeight, uiBits, eFormat); }
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise from known file formats
    /* -- Parameters ------------------------------------------------------- */
    const string &strName,             // Name of object
    Memory &&aSource,                  // Source memory block to read from
    const ImageFlagsConst &lfFlags     // Loading flags
    ): /* -- Initialisation of members ------------------------------------- */
    Image()                            // Default initialisation
    /* -- Initialise from array -------------------------------------------- */
    { InitArray(strName, move(aSource), lfFlags); }
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise image from file
    /* -- Parameters ------------------------------------------------------- */
    const string &strName,             // Name of image from assets to load
    const ImageFlagsConst &lfFlags     // Loading flags
    ): /* -- Initialisation of members ------------------------------------- */
    Image()                            // Default initialisation
    /* -- Initialisation from file ----------------------------------------- */
    { InitFile(strName, lfFlags); }
  /* -- Constructor -------------------------------------------------------- */
  Image(                               // MOVE constructor to SWAP with another
    /* -- Parameters ------------------------------------------------------- */
    Image &&oCref                      // Other image to swap with
    ): /* -- Initialisation of members ------------------------------------- */
    Image()                            // Default initialisation
    /* -- Swap image over -------------------------------------------------- */
    { SwapImage(oCref); }
  /* -- Destructor --------------------------------------------------------- */
  ~Image(void) { AsyncCancel(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Image);              // Disable copy constructor and operator
};/* -- End ---------------------------------------------------------------- */
END_ASYNCCOLLECTOR(Images, Image, IMAGE);
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
