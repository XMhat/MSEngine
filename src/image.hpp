/* == IMAGE.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines a class that can load and save image files and  ## */
/* ## then can optionally be sent to opengl for viewing.                  ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfImage {                    // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfImageFormat;         // Using imageformat namespace
/* == Image collector and member class ===================================== */
BEGIN_ASYNCCOLLECTORDUO(Images, Image, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoader<Image>,           // For loading images off main thread
  public Lockable,                     // Lua garbage collector instruction
  public ImageData                     // Raw image data
{ /* -- Swap image data  ------------------------------------------- */ public:
  void SwapImage(Image &imgRef)
  { // Swap members with other class
    IdentSwap(imgRef);                 // Image filename
    LockSwap(imgRef);                  // Lua lock status
    CollectorSwapRegistration(imgRef); // Collector registration
    ImageDataSwap(imgRef);             // Image data and flags swap
  }
  /* -- Force the specified colour mode ------------------------------------ */
  bool ForcePixelOrder(const unsigned int uiCM)
  { // Return failure if parameters are wrong
    if((uiCM == GL_BGR &&
       (GetPixelType() != GL_RGBA && GetPixelType() != GL_RGB)) ||
       (uiCM == GL_RGB &&
       (GetPixelType() != GL_BGRA && GetPixelType() != GL_BGR)))
      return false;
    // For each slot
    for(ImageSlot &sItem : *this)
    { // Swap pixels
      const int iResult = ImageSwapPixels(sItem.Ptr<char>(), sItem.Size(),
        GetBytesPerPixel(), 0, 2);
      if(iResult < 0)
        XC("Pixel reorder failed!",
           "Code",     iResult,           "FromFormat",    GetPixelType(),
           "ToFormat", uiCM,              "BytesPerPixel", GetBytesPerPixel(),
           "Address",  sItem.Ptr<void>(), "Length",        sItem.Size());
    } // Set new pixel type
    switch(GetPixelType())
    { case GL_RGBA : SetPixelType(GL_BGRA); break;
      case GL_RGB  : SetPixelType(GL_BGR);  break;
      case GL_BGRA : SetPixelType(GL_RGBA); break;
      case GL_BGR  : SetPixelType(GL_RGB);  break;
    } // Success
    return true;
  }
  /* -- Force binary mode -------------------------------------------------- */
  bool ForceBinary(void)
  { // Ignore if already one bit or there are no slots
    if(GetBitsPerPixel() == BD_BINARY || empty()) return false;
    // Throw error if compressed
    if(IsCompressed())
      XC("Cannot binary downsample a compressed image!");
    // Must be 32bpp
    if(GetBitsPerPixel() != BD_RGBA)
      XC("Cannot binary downsample a non RGBA encoded image!",
         "BitsPerPixel", GetBitsPerPixel());
    // Calculate destination size
    const size_t stDst = TotalPixels() / 8;
    // Must be divisible by eight
    if(!IsDivisible(static_cast<double>(stDst)))
      XC("Image size not divisible by eight!", "Size", stDst);
    // Bits lookup table
    static const array<const unsigned char,8>
      ucaBits{1, 2, 4, 8, 16, 32, 64, 128};
    // For each image slot
    for(ImageSlot &isRef : *this)
    { // Destination image for binary data
      Memory mDst{ stDst };
      // For each byte in the alpha channel array
      for(size_t stIndex = 0, stSubIndex = 0, stLimit = isRef.Size();
                 stIndex < stLimit;
                 stIndex += 8, ++stSubIndex)
      { // Init bits
        unsigned char ucBits = 0;
        // Set bits depending on image bytes
        for(size_t stBitIndex = 0; stBitIndex < 8; ++stBitIndex)
          if(isRef.ReadInt<uint8_t>(stBitIndex) > 0)
            ucBits |= ucaBits[stBitIndex];
        // Write new bit value
        mDst.WriteInt<uint8_t>(stSubIndex, ucBits);
      } // Update slot data
      isRef.SwapMemory(std::move(mDst));
    } // Update image data
    SetBitsPerPixel(BD_BINARY);
    SetBytesPerPixel(BY_GRAY);
    SetPixelType(GL_RED);
    SetAlloc(stDst);
    // Success
    return true;
  }
  /* -- Reverse pixels ----------------------------------------------------- */
  bool ReversePixels(void)
  { // Compare bits per pixel
    switch(GetBitsPerPixel())
    { // Monochrome image? Enumerate through each slot and swap the 4-bits in
      // each 8-bit byte.
      case BD_BINARY: for(ImageSlot &sItem : *this) sItem.ByteSwap8(); break;
      // 8-bits per pixel or greater?
      case BD_GRAY: case BD_GRAYALPHA: case BD_RGB: case BD_RGBA:
        // Enumerate through each slot
        for(ImageSlot &sItem : *this)
        { // Create new mem block to write to
          Memory mbOut{ sItem.Size() };
          // Pixel size
          const size_t stStep = GetBytesPerPixel() * sItem.DimGetWidth();
          // Copy the pixels
          for(size_t stI = 0; stI < sItem.Size(); stI += stStep)
            mbOut.Write(stI, sItem.Read(sItem.Size()-stStep-stI), stStep);
          // Set new mem block for this class automatically unloading the old
          // one
          sItem.SwapMemory(std::move(mbOut));
        } // Done
        break;
      // Nothing done so return and log a warning
      default: return false;
    } // Flip reversed state
    if(IsReversed()) ClearReversed();
    else SetReversed();
    // Success
    return true;
  }
  /* -- Pixel conversion process ------------------------------------------- */
  template<class PixelConversionFunction, size_t stSrcStep, size_t stDstStep,
    BitDepth bdNewBPP, GLenum glNewPixelType>void ConvertPixels(void)
  { // Set new bits and bytes
    SetBitsAndBytesPerPixel(bdNewBPP);
    SetPixelType(glNewPixelType);
    // New allocation size
    size_t stNewAlloc = 0;
    // For each slot
    for(ImageSlot &sItem : *this)
    { // Make a new memblock for the destinaiton pixels
      Memory mDst{ sItem.DimGetWidth() * sItem.DimGetHeight() *
        GetBytesPerPixel() };
      // Iterate through the array
      for(uint8_t *cpSrc = sItem.Ptr<uint8_t>(),
         *const cpSrcEnd = sItem.PtrEnd<uint8_t>(),
                  *cpDst = mDst.Ptr<uint8_t>();
                   cpSrc < cpSrcEnd;
                   cpSrc += stSrcStep,
                   cpDst += stDstStep)
        // Call pixel function conversion
        PixelConversionFunction(cpSrc, cpDst);
      // Move memory on top of old memory
      sItem.SwapMemory(std::move(mDst));
      // Adjust alloc size
      stNewAlloc += sItem.Size();
    } // Update new size
    SetAlloc(stNewAlloc);
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
    ConvertPixels<Filter, 2, 3, BD_RGB, GL_RGB>();
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
    ConvertPixels<Filter, 1, 3, BD_RGB, GL_RGB>();
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
    ConvertPixels<Filter, 2, 4, BD_RGBA, GL_RGBA>();
  }
  /* -- Force luminance pixel to RGBA pixel type --------------------------- */
  void ConvertLuminanceToRGBA(void)
  { // Class to convert a luminance pixel to a RGBA pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Unpack one luminance pixel into one RGBA pixel ignoring alpha
      *reinterpret_cast<uint32_t*>(cpDst) = (*cpSrc * 0x01010100) | 0xFF;
    } };
    // Do the conversion of luminance alpha to RGBA
    ConvertPixels<Filter, 1, 4, BD_RGBA, GL_RGBA>();
  }
  /* -- Force binary pixel to luminance pixel type ------------------------- */
  void ConvertBinaryToLuminance(void)
  { // Class to convert a BINARY pixel to a LUMINANCE pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Get the packed eight pixels
      const unsigned int uiPixels = *cpSrc;
      // Unpack eight binary pixels into eight luminance pixels
      *reinterpret_cast<uint64_t*>(cpDst) =
        (uiPixels & 0x01 ? 0xff00000000000000 : 0) |
        (uiPixels & 0x02 ? 0x00ff000000000000 : 0) |
        (uiPixels & 0x04 ? 0x0000ff0000000000 : 0) |
        (uiPixels & 0x08 ? 0x000000ff00000000 : 0) |
        (uiPixels & 0x10 ? 0x00000000ff000000 : 0) |
        (uiPixels & 0x20 ? 0x0000000000ff0000 : 0) |
        (uiPixels & 0x40 ? 0x000000000000ff00 : 0) |
        (uiPixels & 0x80 ? 0x00000000000000ff : 0);
    } };
    // Do the conversion of binary to luminance
    ConvertPixels<Filter, 1, 8, BD_GRAY, GL_RED>();
  }
  /* -- Force binary pixel to RGB pixel type ------------------------------- */
  void ConvertBinaryToRGB(void)
  { // Class to convert a BINARY pixel to a RGB pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Get the packed eight pixels
      const unsigned int uiPixels = *cpSrc;
      // Unpack eight BINARY pixels into right RGB pixels
      *reinterpret_cast<uint64_t*>(cpDst) =
        (uiPixels & 0x80 ? 0xffffff0000000000 : 0) |
        (uiPixels & 0x40 ? 0x000000ffffff0000 : 0) |
        (uiPixels & 0x20 ? 0x000000000000ffff : 0);
      *(reinterpret_cast<uint64_t*>(cpDst)+1) =
        (uiPixels & 0x20 ? 0xff00000000000000 : 0) |
        (uiPixels & 0x10 ? 0x00ffffff00000000 : 0) |
        (uiPixels & 0x08 ? 0x00000000ffffff00 : 0) |
        (uiPixels & 0x04 ? 0x00000000000000ff : 0);
      *(reinterpret_cast<uint64_t*>(cpDst)+2) =
        (uiPixels & 0x04 ? 0xffff000000000000 : 0) |
        (uiPixels & 0x02 ? 0x0000ffffff000000 : 0) |
        (uiPixels & 0x01 ? 0x0000000000ffffff : 0);
    } };
    // Do the conversion of binary to RGBA
    ConvertPixels<Filter, 1, 24, BD_RGB, GL_RGB>();
  }
  /* -- Force binary pixel to RGBA pixel type ------------------------------ */
  void ConvertBinaryToRGBA(void)
  { // Class to convert a BINARY pixel to a RGBA pixel
    struct Filter{inline Filter(const uint8_t*const cpSrc, uint8_t*const cpDst)
    { // Get the packed eight pixels
      const unsigned int uiPixels = *cpSrc;
      // Unpack eight BINARY pixels into eight RGBA pixels
      *reinterpret_cast<uint64_t*>(cpDst) =
        (uiPixels & 0x80 ? 0xffffffff00000000 : 0) |
        (uiPixels & 0x40 ? 0x00000000ffffffff : 0);
      *(reinterpret_cast<uint64_t*>(cpDst)+1) =
        (uiPixels & 0x20 ? 0xffffffff00000000 : 0) |
        (uiPixels & 0x10 ? 0x00000000ffffffff : 0);
      *(reinterpret_cast<uint64_t*>(cpDst)+2) =
        (uiPixels & 0x08 ? 0xffffffff00000000 : 0) |
        (uiPixels & 0x04 ? 0x00000000ffffffff : 0);
      *(reinterpret_cast<uint64_t*>(cpDst)+3) =
        (uiPixels & 0x02 ? 0xffffffff00000000 : 0) |
        (uiPixels & 0x01 ? 0x00000000ffffffff : 0);
    } };
    // Do the conversion of binary to RGBA
    ConvertPixels<Filter, 1, 32, BD_RGBA, GL_RGBA>();
  }
  /* -- Concatenate tiles into a single texture ---------------------------- */
  bool MakeAtlas(void)
  { // Return if 1 or less slides or 1 or less bit depth or has a palette
    if(size() <= 1 || GetBitsPerPixel() <= BD_BINARY || IsPalette())
      return false;
    // Save number of images compacted
    stTiles = size();
    // Remaining tiles
    size_t stRemain = stTiles;
    // Take ownership current slots list and make a blank new one.
    SlotList slSlots{ std::move(*this) };
    // Reset allocation
    SetAlloc(0);
    // Get first image slot
    ImageSlot &isFirst = slSlots.front();
    // Set override tile size and count
    duTileOR.DimSet(isFirst);
    // Get maximum texture size allowed
    const size_t stMaxSize = IfOgl::cOgl->MaxTexSize();
    // Setup compatible types needed to do the compaction
    size_t stTexWidth  = 0,            // Current destination texture width
           stTexHeight = 0,            // Current destination texture height
           stWidth     = isFirst.DimGetWidth(),  // Current tile width
           stHeight    = isFirst.DimGetHeight(), // Current tile height
           stOptSize   = stMaxSize,    // Safe texture size to use
           stCols      = stOptSize/stWidth,  // Safe columns count
           stRows      = stOptSize/stHeight, // Safe rows count
           stNOptSize  = stOptSize,    // New texture size to test with
           stNCols     = stCols,       // New columns count to test
           stNRows     = stRows;       // New rows count to test
    // Now keep dividing the texture size by two until we can no longer fit
    // the needed amount of tiles inside the texture.
    while(stNOptSize && stNCols * stNRows > stRemain)
    { // Update new size
      stOptSize = stNOptSize;
      // Record new valid values
      stCols = stNCols;
      stRows = stNRows;
      // Divide texture size by half to keep power of two textures
      stNOptSize = stOptSize / 2;
      // Calculate new columns and rows
      stNCols = stNOptSize / stWidth;
      stNRows = stNOptSize / stHeight;
    } // If we are going to need more than 1 sub-texture then reset to max
    if(!stOptSize)
    { // Use maximum texture size
      stOptSize = stMaxSize;
      // Set exact new texture size
      stTexWidth = (stOptSize / stWidth) * stWidth;
      stTexHeight = (stOptSize / stHeight) * stHeight;
    } // Now we have the new width and height
    else { stTexWidth = stCols * stWidth; stTexHeight = stRows * stHeight; }
    // Make memory for texture
    Memory mTexture{ stTexWidth * stTexHeight * GetBytesPerPixel() };
    // Texture position and tiles remaining
    size_t stTX = 0, stTY = 0;
    // Scanline size
    const size_t stScanLine = stWidth * GetBytesPerPixel();
    // Until we have no more slots. We'll keep deleting them as we process
    // them to keep the memory usage down
    for(const ImageSlot &isData : slSlots)
    { // Now we copy this slide into the texuree
      for(size_t stY = 0; stY < stHeight; ++stY)
        mTexture.Write((((stTY + stY) * stTexWidth) + stTX) *
          GetBytesPerPixel(), isData.Read(stY * stScanLine, stScanLine),
          stScanLine);
      // Add to number of tiles added
      --stRemain;
      // Move texture position and continue if we can hold another column
      stTX += stWidth;
      if(stTX + stWidth <= stTexWidth) continue;
      // Move to next row and continue if we can hold another row
      stTX = 0;
      stTY += stHeight;
      if(stTY + stHeight <= stTexHeight) continue;
      // Now off the bottom of the texture so reset Y position
      stTY = 0;
      // Add this texture as it is finished
      AddSlot(mTexture, static_cast<unsigned int>(stTexWidth),
                        static_cast<unsigned int>(stTexHeight));
      // Copy new tile sizes
      stWidth  = isData.DimGetWidth<size_t>();
      stHeight = isData.DimGetHeight<size_t>();
      // Set new safe values and setup next values to test
      stOptSize = stMaxSize;           stNOptSize = stOptSize;
      stRows    = stOptSize/stHeight;  stNRows    = stRows;
      stCols    = stOptSize/stWidth;   stNCols    = stCols;
      // Now keep dividing the texture size by two until we can no longer fit
      // the needed amount of tiles inside the texture.
      while(stNOptSize && stNCols * stNRows > stRemain)
      { // Update new size
        stOptSize = stNOptSize;
        // Record new valid values
        stCols = stNCols;
        stRows = stNRows;
        // Divide texture size by half to keep power of two textures
        stNOptSize = stOptSize / 2;
        // Calculate new columns and rows
        stNCols = stNOptSize / stWidth;
        stNRows = stNOptSize / stHeight;
      } // If we are going to need more than 1 sub-texture?
      if(!stOptSize)
      { // Reset to maximum
        stOptSize = stMaxSize;
        // Set exact new texture size
        stTexWidth = (stOptSize / stWidth) * stWidth;
        stTexHeight = (stOptSize / stHeight) * stHeight;
      } // Now we have the new width and height
      else { stTexWidth = stCols * stWidth; stTexHeight = stRows * stHeight; }
      // Make a new texture
      mTexture.InitBlank(stTexWidth * stTexHeight * GetBytesPerPixel());
    } // A new texture has been written? Add the final slot
    if(stTX || stTY) AddSlot(mTexture, static_cast<unsigned int>(stTexWidth),
                                       static_cast<unsigned int>(stTexHeight));
    // Set size of first texture
    DimSet(isFirst);
    // Success
    return true;
  }
  /* -- Convert palette to RGB(A) ------------------------------------------ */
  template<ByteDepth byDepth, GLenum eType>bool ExpandPalette(void)
  { // Ignore if not paletted
    if(IsNotPalette()) return false;
    // Must only have two slots
    if(size() != 2)
      XC("Cannot convert palette to RGB because we need two slots!",
         "Slots", size());
    // Take ownership current slots list and make a blank new one.
    SlotList slSlots{ std::move(*this) };
    // Reset allocation
    SetAlloc(0);
    // Get datas
    const ImageSlot &isImage = slSlots.front(),
                    &isPalette = slSlots.back();
    // Create output buffer and enumerate through the pixels
    Memory mOut{ DimGetWidth() * DimGetHeight() * byDepth };
    // If palette and output image are same depth?
    if(isPalette.DimGetHeight() == byDepth)
    { // We can copy directlry
      for(size_t stIn = 0, stOut = 0;
                 stIn < mOut.Size();
               ++stIn, stOut += byDepth)
        // Read palette index from pixel data, then write the RGB value
        // from the palette to the output image.
        mOut.Write(stOut, isPalette.Read(isImage.ReadInt<uint8_t>(stIn) *
          byDepth), byDepth);
    } // Not same so less trivial
    else if constexpr(byDepth == BY_RGBA) for(size_t stIn = 0, stOut = 0;
                                                     stIn < mOut.Size();
                                                   ++stIn, stOut += byDepth)
    { // Get palette location
      const size_t stPalIndex = isImage.ReadInt<uint8_t>(stIn) *
        isPalette.DimGetHeight();
      // Get palette value
      const uint32_t uiVal =
        (isPalette.ReadInt<uint16_t>(stPalIndex) << 8) |
         isPalette.ReadInt<uint8_t>(stPalIndex+sizeof(uint16_t));
      // Write new value
      mOut.WriteInt<uint32_t>(stOut, uiVal);
    } // Unknown
    else XC("Image expanding circumstances not implemented!");
    // Update output, we will be converting to rgb
    SetBytesAndBitsPerPixel(byDepth);
    SetPixelType(eType);
    // Add expanded image to list
    AddSlot(mOut);
    // Success
    return true;
  }
  /* -- Convert data to GPU compatible ------------------------------------- */
  bool ConvertGPUCompatible(void)
  { // We don't have functionality to load <8bpp images in GPU yet so if this
    // flag is specified, we will make sure <8bpp textures get converted
    // properly so they can be loaded by OpenGL. This is useful if the
    // bit-depth of the bitmap isn't known and the caller wants to make sure
    // it guarantees to be loaded into OpenGL.
    switch(GetBitsPerPixel())
    { // 1bpp (BINARY)
      case BD_BINARY: ConvertBinaryToLuminance(); break;
      // Ignore anything else
      default: return false;
    } // Success
    return true;
  }
  /* -- Convert image bit-depth to to 24-bits per pixel RGB format --------- */
  bool ConvertRGB(void)
  { // Compare current bit-depth
    switch(GetBitsPerPixel())
    { // 1bpp (BINARY)
      case BD_BINARY: ConvertBinaryToRGB(); break;
      // 8bpp (LUMINANCE)
      case BD_GRAY:
        if(IsPalette()) return ExpandPalette<BY_RGB,GL_RGB>();
        ConvertLuminanceToRGB();
        break;
      // 16bpp (LUMINANCE+ALPHA)
      case BD_GRAYALPHA: ConvertLuminanceAlphaToRGB(); break;
      // Ignore anything else
      default: return false;
    } // Success
    return true;
  }
  /* -- Convert image bit-depth to to 32-bits per pixel RGBA format -------- */
  bool ConvertRGBA(void)
  { // Compare current bit-depth
    switch(GetBitsPerPixel())
    { // 1bpp (BINARY)
      case BD_BINARY: ConvertBinaryToRGBA(); break;
      // 8bpp (LUMINANCE)
      case BD_GRAY:
        if(IsPalette()) return ExpandPalette<BY_RGBA,GL_RGBA>();
        ConvertLuminanceToRGBA();
        break;
      // 16bpp (LUMINANCE+ALPHA)
      case BD_GRAYALPHA: ConvertLuminanceAlphaToRGBA(); break;
      // Ignore anything else
      default: return false;
    } // Success
    return true;
  }
  /* -- Apply filters ------------------------------------------------------ */
  void ApplyFilters(void)
  { // Record current parameters
    const Dimensions<> dOld{ *this };
    const size_t stSlots = size();
    const BitDepth bdOld = GetBitsPerPixel();
    const ByteDepth byOld = GetBytesPerPixel();
    const GLenum glOld = GetPixelType();
    const size_t stOld = GetAlloc();
    // Convert to GPU copmatible texture?
    if(IsConvertGPUCompat())
    { // Log that we're running this function
      cLog->LogDebugExSafe("Image '$' safe pixel depth request...",
        IdentGet());
      // Run the function and log success if succeeded
      if(ConvertGPUCompatible())
      { // Log the successful result
        cLog->LogInfoExSafe("Image '$' pixel-depth now safe.",
          IdentGet());
        // Set activated flag
        SetActiveGPUCompat();
      } // Conversion did not happen so log that too
      else cLog->LogDebugExSafe("Image '$' skipped safe pixel depth!",
        IdentGet());
    } // If a request to convert to 24-bits per pixel?
    else if(IsConvertRGB())
    { // Log that we're running this function
      cLog->LogDebugExSafe("Image '$' force 24-bit request...",
        IdentGet());
      // Run the function and log success if succeeded
      if(ConvertRGB())
      { // Log the successful result
        cLog->LogInfoExSafe("Image '$' pixel depth now 24-bit.",
          IdentGet());
        // Set activated flag
        SetActiveRGB();
      } // Conversion did not happen so log that too
      else cLog->LogDebugExSafe("Image '$' skipped converting to 24-bit!",
        IdentGet());
    } // If a request to convert to RGBA?
    else if(IsConvertRGBA())
    { // Log that we're running this function
      cLog->LogDebugExSafe("Image '$' force 32-bit request...",
        IdentGet());
      // Run the function and log success if succeeded
      if(ConvertRGBA())
      { // Log the successful result
        cLog->LogInfoExSafe("Image '$' pixel depth now 32-bit.",
          IdentGet());
        // Set activated flag
        SetActiveRGBA();
      } // Conversion did not happen so log that too
      else cLog->LogDebugExSafe("Image '$' skipped converting to 32-bit!",
        IdentGet());
    } // To BGR colour mode?
    if(IsConvertBGROrder())
    { // Log that we're running this function
      cLog->LogDebugExSafe("Image '$' re-order to BGR request...",
        IdentGet());
      // Run the function and log success if succeeded
      if(ForcePixelOrder(GL_BGR))
      { // Log the successful result
        cLog->LogInfoExSafe("Image '$' re-ordered to BGR.",
          IdentGet());
        // Set activated flag
        SetActiveBGROrder();
      } // Conversion did not happen so log that too
      else cLog->LogDebugExSafe("Image '$' re-order to BGR skipped!",
        IdentGet());
    } // To RGB colour mode
    else if(IsConvertRGBOrder())
    { // Log that we're running this function
      cLog->LogDebugExSafe("Image '$' re-order to RGB request...",
        IdentGet());
      // Run the function and log success if succeeded
      if(ForcePixelOrder(GL_RGB))
      { // Log the successful result
        cLog->LogInfoExSafe("Image '$' re-ordered now RGB.",
          IdentGet());
        // Set activated flag
        SetActiveRGBOrder();
      } // Conversion did not happen so log that too
      else cLog->LogDebugExSafe("Image '$'[$] re-order to RGB skipped!",
        IdentGet());
    } // Reverse the image pixels
    if(IsConvertReverse())
    { // Log that we're running this function
      cLog->LogDebugExSafe("Image '$' pixel reversal request...",
        IdentGet());
      // Run the function and log success if succeeded
      if(ReversePixels())
      { // Log the successful result
        cLog->LogInfoExSafe("Image '$' pixels reversed!",
          IdentGet());
        // Set activated flag
        SetActiveReverse();
      } // Conversion did not happen so log that too
      else cLog->LogDebugExSafe("Image '$' skipped pixel reversal!",
        IdentGet());
    } // To binary colour mode
    if(IsConvertBinary())
    { // Log that we're running this function
      cLog->LogDebugExSafe("Image '$' downsample to binary request...",
        IdentGet());
      // Run the function and log success if succeeded
      if(ForceBinary())
      { // Log the successful result
        cLog->LogInfoExSafe("Image '$' downsample to binary completed.",
          IdentGet());
        // Set activated flag
        SetActiveBinary();
      } // Conversion did not happen so log that too
      else cLog->LogDebugExSafe("Image '$' downsample to binary skipped!",
        IdentGet());
    } // Compact all slides into a single texture if possible
    if(IsConvertAtlas())
    { // Log that we're running this function
      cLog->LogDebugExSafe("Image '$' compact request...",
        IdentGet());
      // Run the function and log success if succeeded
      if(MakeAtlas())
      { // Log the successful result
        cLog->LogInfoExSafe("Image '$' compacted.",
          IdentGet());
        // Set activated flag
        SetActiveAtlas();
      } // Conversion did not happen so log that too
      else cLog->LogDebugExSafe("Image '$' compact skipped!",
        IdentGet());
    } // Report status if we acticated anything
    if(IsActiveAtlas() || IsActiveReverse() || IsActiveRGB() ||
       IsActiveRGBA() || IsActiveBGROrder() || IsActiveBinary() ||
       IsActiveGPUCompat() || IsActiveRGBOrder())
      cLog->LogDebugExSafe("Image '$' filtering completed...\n"
                         "$$$$$$$$"
                         "- Bitmap dimensions: $x$ -> $x$.\n"
                         "- Bitmap slots: $ -> $.\n"
                         "- Pixel depth: $<$> -> $<$>.\n"
                         "- Pixel type: $<$$> -> $<$$>.\n"
                         "- Memory usage: $ -> $ bytes.\n"
                         "- Tile size override: $x$.\n"
                         "- Tile count override: $.",
        IdentGet(),
        IsActiveAtlas()     ? "- Slots compacted to atlas.\n"       : strBlank,
        IsActiveReverse()   ? "- Pixels reversed.\n"                : strBlank,
        IsActiveRGB()       ? "- Pixels converted to 24-bit.\n"     : strBlank,
        IsActiveRGBA()      ? "- Pixels converted to 32-bit.\n"     : strBlank,
        IsActiveBGROrder()  ? "- Pixels converted to BGR order.\n"  : strBlank,
        IsActiveRGBOrder()  ? "- Pixels converted to RGB order.\n"  : strBlank,
        IsActiveBinary()    ? "- Pixels converted to monochrome.\n" : strBlank,
        IsActiveGPUCompat() ? "- Pixels made OpenGL compatible.\n"  : strBlank,
        dOld.DimGetWidth(), dOld.DimGetHeight(), DimGetWidth(),
        DimGetHeight(), stSlots, size(), bdOld, byOld, GetBitsPerPixel(),
        GetBytesPerPixel(), IfOgl::cOgl->GetPixelFormat(glOld), hex, glOld,
        IfOgl::cOgl->GetPixelFormat(GetPixelType()), GetPixelType(), dec,
        stOld, GetAlloc(), duTileOR.DimGetWidth(), duTileOR.DimGetHeight(),
        stTiles);
  }
  /* -- Load specified image ----------------------------------------------- */
  void AsyncReady(FileMap &fmData)
  { // Force load a certain type of image (for speed?) but in Async mode,
    // force detection doesn't really matter as much, but overall, still
    // needed if speed is absolutely neccesary.
    if     (IsLoadAsTGA()) ImageLoad(0, fmData, *this);
    else if(IsLoadAsJPG()) ImageLoad(1, fmData, *this);
    else if(IsLoadAsPNG()) ImageLoad(2, fmData, *this);
    else if(IsLoadAsGIF()) ImageLoad(3, fmData, *this);
    else if(IsLoadAsDDS()) ImageLoad(4, fmData, *this);
    // Auto detection of image
    else ImageLoad(fmData, *this);
    // Apply filters if image has no special circumstances
    if(IsNotCompressed() && IsNotMipmaps()) ApplyFilters();
    // Recover slots memory if they were modified
    shrink_to_fit();
  }
  /* -- Reload specified image --------------------------------------------- */
  void ReloadData(void)
  { // Load the file from disk or archive
    FileMap fmData{ AssetExtract(IdentGet()) };
    // Reset memory usage to zero
    SetAlloc(0);
    // Run codecs and filters on it
    AsyncReady(fmData);
  }
  /* -- Save image using a type id ----------------------------------------- */
  void SaveFile(const string &strFN, const size_t stSId, const size_t stPId)
    { ImageSave(stPId, strFN, *this, (*this)[stSId]); }
  /* -- Load image from memory asynchronously ------------------------------ */
  void InitAsyncArray(lua_State*const lS)
  { // Need 6 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 7);
    // Get and check parameters
    const string strName{ GetCppStringNE(lS, 1, "Identifier") };
    Asset &aData = *GetPtr<Asset>(lS, 2, "Asset");
    FlagSet(GetFlags(lS, 3, IL_MASK, "Flags"));
    CheckFunction(lS, 4, "ErrorFunc");
    CheckFunction(lS, 5, "ProgressFunc");
    CheckFunction(lS, 6, "SuccessFunc");
    // The decoded image will be kept in memory
    SetDynamic();
    // Load image from memory asynchronously
    AsyncInitArray(lS, strName, "bmparray", std::move(aData));
  }
  /* -- Load image from file asynchronously -------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // Need 5 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 6);
    // Get and check parameters
    const string strFile{ GetCppFileName(lS, 1, "File") };
    FlagSet(GetFlags(lS, 2, IL_MASK, "Flags"));
    CheckFunction(lS, 3, "ErrorFunc");
    CheckFunction(lS, 4, "ProgressFunc");
    CheckFunction(lS, 5, "SuccessFunc");
    // Load image from file asynchronously
    AsyncInitFile(lS, strFile, "bmpfile");
  }
  /* -- Create a blank image for working on -------------------------------- */
  void InitBlank(const string &strName, const unsigned int uiBWidth,
    const unsigned int uiBHeight, const bool bAlpha, const bool bClear)
  { // Lookup table for alpha setting
    static const array<const std::pair<const BitDepth, const GLenum>,2>
      aLookup{ { { BD_RGB, GL_RGB }, { BD_RGBA, GL_RGBA } } };
    const auto &aLookupRef = aLookup[static_cast<size_t>(bAlpha)];
    // Set appropriate parameters
    SetBitsAndBytesPerPixel(aLookupRef.first);
    SetPixelType(aLookupRef.second);
    // Set other members
    IdentSet(strName);
    SetDynamic();
    DimSet(uiBWidth, uiBHeight);
    // Add the raw data into a slot
    Memory mData{ TotalPixels() * GetBytesPerPixel(), bClear };
    AddSlot(mData);
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Load image from a raw image ---------------------------------------- */
  void InitRaw(const string &strName, Memory &&mSrc,
    const unsigned int uiBWidth, const unsigned int uiBHeight,
    const BitDepth bdBitsPP, const GLenum ePixType)
  { // Error if no data specified
    if(mSrc.Empty()) XC("Image data is empty!", "File", strName);
    // Check that the range is valid
    if(!uiBWidth || !uiBHeight || uiBWidth > 0xFFFF || uiBHeight > 0xFFFF)
      XC("Image dimensions are not valid!",
        "File", strName, "Width", uiBWidth, "Height", uiBHeight);
    // Check bitrate
    switch(bdBitsPP)
    { // Allowed bit-rates are...
      case BD_BINARY:                  // 1bpp (binary)
      case BD_GRAY:                    // 8bpp (gray NOT palette)
      case BD_GRAYALPHA:               // 16bpp (gray+alpha NOT palette)
      case BD_RGB:                     // 24bpp (rgb or bgr)
      case BD_RGBA: break;             // 32bpp (rgba or bgra)
      // Error
      default: XC("Image bit-depth is not valid!",
        "File", strName, "Depth", bdBitsPP);
    } // Check the size
    SetBitsAndBytesPerPixel(bdBitsPP);
    DimSet(uiBWidth, uiBHeight);
    const size_t stExpect = TotalPixels() * GetBytesPerPixel();
    if(stExpect != mSrc.Size())
      XC("Arguments are not valid for specified image data!",
        "File", strName, "Expect", stExpect, "Actual", mSrc.Size());
    // Everything looks OK, set rest of the members
    IdentSet(strName);
    SetPixelType(ePixType);
    SetDynamic();
    // Add the raw data into a slot
    AddSlot(mSrc);
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Load image from a single colour ------------------------------------ */
  void InitColour(const uint32_t ulColour)
  { // Set members
    IdentSetA("solid/0x", hex, ulColour);
    SetBitsAndBytesPerPixel(BD_RGBA);
    SetPixelType(GL_RGBA);
    SetDynamic();
    DimSet(1);
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
    Memory mData{ GetBytesPerPixel(), ucaColour.data() };
    AddSlot(mData);
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Init from file ----------------------------------------------------- */
  void InitFile(const string &strFileName, const ImageFlagsConst &lfS)
  { // Set the loading flags
    FlagSet(lfS);
    // Load the file normally
    SyncInitFileSafe(strFileName);
  }
  /* -- Init from array ---------------------------------------------------- */
  void InitArray(const string &strName, Memory &&mbD,
    const ImageFlagsConst &lfS)
  { // Is dynamic because it was not loaded from disk
    SetDynamic();
    // Set the loading flags
    FlagSet(lfS);
    // Load the array normally
    SyncInitArray(strName, std::move(mbD));
  }
  /* -- Default constructor ------------------------------------------------ */
  Image(void) :                        // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperImage(*cImages),           // Initialise collector helper
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    AsyncLoader<Image>(this,           // Initialise async loader
      EMC_MP_IMAGE)                    // Initialise async event id
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise a 1x1 pixel texture
    /* -- Parameters ------------------------------------------------------- */
    const uint32_t uiColour            // 32-bit RGBA colour pixel value
    ): /* -- Initialisers -------------------------------------------------- */
    Image()                            // Default initialisation
    /* -- Code  ------------------------------------------------------------ */
    { InitColour(uiColour); }          // Init 1x1 tex with specified colour
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise from RAW pixel data
    /* -- Parameters ------------------------------------------------------- */
    const string &strName,             // Name of the object
    Memory &&mSrc,                     // Source pixel data
    const unsigned int uiWidth,        // Number of pixels in each scanline
    const unsigned int uiHeight,       // Number of scan lines
    const BitDepth bdBits,             // Bit depth of the pixel data
    const GLenum eFormat               // OpenGL pixel format
    ): /* -- Initialisation of members ------------------------------------- */
    Image()                            // Default initialisation
    /* -- Initialise raw image --------------------------------------------- */
    { InitRaw(strName, std::move(mSrc), uiWidth, uiHeight, bdBits, eFormat); }
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise from known file formats
    /* -- Parameters ------------------------------------------------------- */
    const string &strName,             // Name of object
    Memory &&mSrc,                     // Source memory block to read from
    const ImageFlagsConst &ifFlags     // Loading flags
    ): /* -- Initialisation of members ------------------------------------- */
    Image()                            // Default initialisation
    /* -- Initialise from array -------------------------------------------- */
    { InitArray(strName, std::move(mSrc), ifFlags); }
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise image from file
    /* -- Parameters ------------------------------------------------------- */
    const string &strName,             // Name of image from assets to load
    const ImageFlagsConst &ifFlags     // Loading flags
    ): /* -- Initialisation of members ------------------------------------- */
    Image()                            // Default initialisation
    /* -- Code ------------------------------------------------------------- */
    { InitFile(strName, ifFlags); }    // Initialisation from file
  /* -- Constructor -------------------------------------------------------- */
  Image(                               // MOVE constructor to SWAP with another
    /* -- Parameters ------------------------------------------------------- */
    Image &&imgRef                     // Other image to swap with
    ): /* -- Initialisation of members ------------------------------------- */
    Image()                            // Default initialisation
    /* -- Code ------------------------------------------------------------- */
    { SwapImage(imgRef); }             // Swap image over
  /* -- Destructor --------------------------------------------------------- */
  ~Image(void) { AsyncCancel(); }      // Wait for loading thread to cancel
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Image);              // Disable copy constructor and operator
};/* -- End ---------------------------------------------------------------- */
END_ASYNCCOLLECTOR(Images, Image, IMAGE);
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
