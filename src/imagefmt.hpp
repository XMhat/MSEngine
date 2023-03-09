/* == IMAGEFMT.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## These classes are plugins for the ImgLib manager to allow loading   ## */
/* ## of certain formatted images.                                        ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfImageFormat {              // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace Library::Png;          // Using libpng library functions
using namespace Library::JpegTurbo;    // Using jpegturbo library functions
using namespace Library::NSGif;        // Using libnsgif library functions
using namespace IfImageLib;            // Using imagelib interface
using namespace IfCVar;                // Using cvar interface
/* ========================================================================= */
/* ######################################################################### */
/* ## DirectDraw Surface                                              DDS ## */
/* ######################################################################### */
/* -- DDS Codec Object ----------------------------------------------------- */
class CodecDDS :
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* --------------------------------------------------------------- */ public:
  static bool Load(FileMap &fC, ImageData &ifD)
  { // *** DDS_FILEHEADER (128 bytes) ***
    // 000:         uint32_t    ulMagic; // (= 0x20534444)
    // 004:     *** DDS_HEADER (124 bytes) ***
    // 004:000:     uint32_t    ulSize;
    // 008:004:     uint32_t    ulFlags;
    // 012:008:     uint32_t    ulHeight;
    // 016:012:     uint32_t    ulWidth;
    // 020:016:     uint32_t    ulPitchOrLinearSize;
    // 024:020:     uint32_t    ulDepth;
    // 028:024:     uint32_t    ulMipMapCount;
    // 032:028:     uint32_t    ulReserved1[11];
    // 076:072: *** DDS_PIXELFORMAT (32 bytes) *** [ddspf];
    // 076:072:000: uint32_t    ulSize;
    // 080:076:004: uint32_t    ulFlags;
    // 084:080:008: uint32_t    ulFourCC;
    // 088:084:012: uint32_t    ulRGBBitCount;
    // 092:088:016: uint32_t    ulRBitMask;
    // 096:092:020: uint32_t    ulGBitMask;
    // 100:096:024: uint32_t    ulBBitMask;
    // 104:100:028: uint32_t    ulABitMask;
    // 108:104:     uint32_t    ulCaps;
    // 112:108:     uint32_t    ulCaps2;
    // 116:112:     uint32_t    ulCaps3;
    // 120:116:     uint32_t    ulCaps4;
    // 124:120:     uint32_t    ulReserved2;
    // Quick check header which should be at least 128 bytes
    if(fC.Size() < 128 || fC.FileMapReadVar32LE() != 0x20534444) return false;
    // Read the image size and flags
    const unsigned int uiSize = fC.FileMapReadVar32LE(),
                       uiFlags = fC.FileMapReadVar32LE();
    // To stop compiler whining but we needed to move the file pointer on
    UNUSED_VARIABLE(uiSize);
    UNUSED_VARIABLE(uiFlags);
    // Get and check dimensions
    ifD.uiHeight = fC.FileMapReadVar32LE();
    ifD.uiWidth = fC.FileMapReadVar32LE();
    if(!ifD.uiWidth || !ifD.uiHeight)
      XC("DDS dimensions invalid!",
         "Width", ifD.uiWidth, "Height", ifD.uiHeight);
    // Get pitch or linear size
    const unsigned int uiPitchOrLinearSize = fC.FileMapReadVar32LEFrom(20);
    UNUSED_VARIABLE(uiPitchOrLinearSize);
    // Get bit-depth, and it must be a power of two
    ifD.uiBitsPerPixel = fC.FileMapReadVar32LE();
    if((ifD.uiBitsPerPixel && !IsPow2(ifD.uiBitsPerPixel)) ||
      ifD.uiBitsPerPixel > 32)
        XC("DDS bit-depth not valid!",
           "BitsPerPixel", ifD.uiBitsPerPixel);
    // Get and check mipmap count. Theres still 1 image if this is zero.
    const unsigned int uiMipMapCount =
      fC.FileMapReadVar32LE() + 1;
    if(uiMipMapCount > 1) ifD.bMipmaps = true;
    // Get other parts
    const unsigned int uiReserved1_00 = fC.FileMapReadVar32LE(),
                       uiReserved1_01 = fC.FileMapReadVar32LE(),
                       uiReserved1_02 = fC.FileMapReadVar32LE(),
                       uiReserved1_03 = fC.FileMapReadVar32LE(),
                       uiReserved1_04 = fC.FileMapReadVar32LE(),
                       uiReserved1_05 = fC.FileMapReadVar32LE(),
                       uiReserved1_06 = fC.FileMapReadVar32LE(),
                       uiReserved1_07 = fC.FileMapReadVar32LE(),
                       uiReserved1_08 = fC.FileMapReadVar32LE(),
                       uiReserved1_09 = fC.FileMapReadVar32LE(),
                       uiReserved1_10 = fC.FileMapReadVar32LE(),
                       uiPFSize = fC.FileMapReadVar32LE(),
                       uiPFFlags = fC.FileMapReadVar32LE(),
                       uiPFFourCC = fC.FileMapReadVar32LE(),
                       uiPFRGBBitCount = fC.FileMapReadVar32LE(),
                       uiPFRBitMask = fC.FileMapReadVar32LE(),
                       uiPFGBitMask = fC.FileMapReadVar32LE(),
                       uiPFBBitMask = fC.FileMapReadVar32LE(),
                       uiPFABitMask = fC.FileMapReadVar32LE();
    // To stop compiler whining but we needed to move the file pointer on
    UNUSED_VARIABLE(uiReserved1_00);
    UNUSED_VARIABLE(uiReserved1_01);
    UNUSED_VARIABLE(uiReserved1_02);
    UNUSED_VARIABLE(uiReserved1_03);
    UNUSED_VARIABLE(uiReserved1_04);
    UNUSED_VARIABLE(uiReserved1_05);
    UNUSED_VARIABLE(uiReserved1_06);
    UNUSED_VARIABLE(uiReserved1_07);
    UNUSED_VARIABLE(uiReserved1_08);
    UNUSED_VARIABLE(uiReserved1_09);
    UNUSED_VARIABLE(uiReserved1_10);
    UNUSED_VARIABLE(uiPFSize);
    // Check pixel format, only compressed texture format supported because
    // OpenGL does not support ARGB/XRGB and the guest should use something
    // more simple like BMP/TGA/PNG if using RGB(A) textures.
    if(uiPFFlags & 0x4) switch(uiPFFourCC) // DDSF_FOURCC
    { // FOURCC_DXT1 / GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
      case 0x31545844: ifD.glPixelType = 0x83F1; ifD.uiBitsPerPixel = 32;
        ifD.uiBytesPerPixel = 3; ifD.bCompressed = true; break;
      // FOURCC_DXT3 / GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
      case 0x33545844: ifD.glPixelType = 0x83F2; ifD.uiBitsPerPixel = 32;
        ifD.uiBytesPerPixel = 4; ifD.bCompressed = true; break;
      // FOURCC_DXT5 / GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
      case 0x35545844: ifD.glPixelType = 0x83F3; ifD.uiBitsPerPixel = 32;
        ifD.uiBytesPerPixel = 4; ifD.bCompressed = true; break;
      // Unknown compression
      default: XC("Only DXT1, DXT3 or DXT5 supported!",
        "Type", uiPFFourCC);
    } // Unknown type
    else XC("FourCC formats unsupported!",
      "BitCount",  uiPFRGBBitCount, "RedMask",  uiPFRBitMask,
      "GreenMask", uiPFGBitMask,    "BlueMask", uiPFBBitMask,
      "AlphaMask", uiPFABitMask);
    // Get other parts
    const unsigned int uiCaps1 = fC.FileMapReadVar32LE(),
                       uiCaps2 = fC.FileMapReadVar32LE();
    // To stop compiler whining but we needed to move the file pointer on
    UNUSED_VARIABLE(uiCaps1);
    // Cube maps not allowed? (DDSF_CUBEMAP)
    if(uiCaps2 & 0x200) XC("Cubemap not supported!");
    // Volume texture not supported? (DDSF_VOLUME)
    if((uiCaps2 & 0x200000) && ifD.uiBitsPerPixel > 0)
      XC("Volume texture not supported!");
    // Read rest of values
    const unsigned int ulCaps3 = fC.FileMapReadVar32LE(),
                       ulCaps4 = fC.FileMapReadVar32LE(),
                       ulReserved2 = fC.FileMapReadVar32LE();
    // To stop compiler whining but we needed to move the file pointer on
    UNUSED_VARIABLE(ulCaps3);
    UNUSED_VARIABLE(ulCaps4);
    UNUSED_VARIABLE(ulReserved2);
    // Alocate slots as mipmaps
    ifD.sData.reserve(uiMipMapCount);
    // DDS's are reversed
    ifD.bReverse = true;
    // DXT[n]? Compressed bitmaps need a special calculation
    if(ifD.bCompressed) for(unsigned int
      // Pre-initialised variables
      uiBitDiv    = (ifD.glPixelType == 0x83F1 ? 8 : 16),
      uiMipIndex  = 0,
      uiMipWidth  = ifD.uiWidth,
      uiMipHeight = ifD.uiHeight,
      uiMipBPP    = ifD.uiBytesPerPixel,
      uiMipSize   = ((uiMipWidth+3)/4)*((uiMipHeight+3)/4)*uiBitDiv;
      // Conditions
      uiMipIndex < uiMipMapCount &&
      (uiMipWidth || uiMipHeight);
      // Pocedures on each loop
    ++uiMipIndex,
      uiMipWidth  >>= 1,
      uiMipHeight >>= 1,
      uiMipBPP      = Maximum(uiMipBPP >> 1, 1),
      uiMipSize     = ((uiMipWidth+3)/4)*((uiMipHeight+3)/4)*uiBitDiv)
    { // Read compressed data from file and show error if not enough data
      Memory aData{ fC.FileMapReadBlock(uiMipSize) };
      if(aData.Size() != uiMipSize)
        XC("Read error!", "Expected", uiMipSize, "Actual", aData.Size());
      // Push back a new slot for every new mipmap
      ifD.stAlloc += aData.Size();
      ifD.sData.push_back({ move(aData), uiMipWidth, uiMipHeight });
    } // Uncompressed image
    else for(unsigned int
      // Pre-initialised variables
      uiMipIndex = 0,                  // Mipmap index
      uiMipWidth = ifD.uiWidth,        // Mipmap width
      uiMipHeight = ifD.uiHeight,      // Mipmap height
      uiMipBPP = ifD.uiBytesPerPixel,  // Mipmap bytes-per-pixel
      uiMipSize = uiMipWidth*uiMipHeight*uiMipBPP; // Mipmap size in memory
      // Conditions
      uiMipIndex < uiMipMapCount &&    // Mipmap count not reached and...
      (uiMipWidth || uiMipHeight);     // ...width and height is not zero
      // Pocedures on each loop
    ++uiMipIndex,                      // Increment mipmap index
      uiMipWidth >>= 1,                // Divide mipmap width by 2
      uiMipHeight >>= 1,               // Divide mipmap height by 2
      uiMipBPP = Maximum(uiMipBPP>>1, 1), // Divide depth by 2 (1 minimum)
      uiMipSize = uiMipWidth*uiMipHeight*uiMipBPP) // New mipmap size
    { // Read uncompressed data from file and show error if not enough data
      Memory mData{ fC.FileMapReadBlock(uiMipSize) };
      if(mData.Size() != uiMipSize)
        XC("Read error!", "Expected", uiMipSize, "Actual", mData.Size());
      // Push back a new slot for every new mipmap
      ifD.stAlloc += mData.Size();
      ifD.sData.push_back({ move(mData), uiMipWidth, uiMipHeight });
    } // Some mipmaps might have beee ignored so shrink memory use if needed
    ifD.sData.shrink_to_fit();
    // Succeeded
    return true;
  }
  /* -- Constructor -------------------------------------------------------- */
  CodecDDS(void) :
    /* -- Initialisation of members ---------------------------------------- */
    ImageFmt{ "DirectDraw Surface", "DDS", Load }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecDDS);           // Omit copy constructor for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= */
/* ######################################################################### */
/* ## Graphics Interchange Format                                     GIF ## */
/* ######################################################################### */
/* -- GIF Codec Object ----------------------------------------------------- */
class CodecGIF :
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* -- Allocate memory ------------------------------------------- */ private:
  static void *GIFCreate(int iW, int iH)
    { return MemAlloc<void>(iW * iH * 4); }
  /* -- Free memory -------------------------------------------------------- */
  static void GIFDestroy(void*const vpBuffer)
    { if(!vpBuffer) throw runtime_error{ "Free buffer null pointer" };
      MemFree(vpBuffer); }
  /* -- Return ptr to pixel data in image ---------------------------------- */
  static unsigned char *GIFRead(void*const vpBuffer)
    { if(!vpBuffer) throw runtime_error{ "Get buffer null pointer" };
      return reinterpret_cast<unsigned char*>(vpBuffer); }
  /* -- Sets whether a image should opaque --------------------------------- */
  static void GIFOpaque(void*const vpBuffer, bool)
    { if(!vpBuffer) throw runtime_error{ "Set opaque null pointer" }; }
  /* -- If image has an opaque alpha channel ------------------------------- */
  static bool GIFIsOpaque(void*const vpBuffer)
    { if(!vpBuffer) throw runtime_error{ "Test opaque null pointer" };
      return true; }
  /* -- Image changed, flush cache ----------------------------------------- */
  static void GIFFlush(void*const vpBuffer)
    { if(!vpBuffer) throw runtime_error{ "Modified data null pointer" }; }
  /* --------------------------------------------------------------- */ public:
  static bool Load(FileMap &fC, ImageData &ifD)
  { // Must have at least 10 bytes for header 'GIF9' and ending footer.
    if(fC.Size() < 10 || fC.FileMapReadVar32LE() != 0x38464947) return false;
    // Next word must be 7a or 9a. Else not a gif file.
    const unsigned int uiMagic2 = fC.FileMapReadVar16LE();
    if(uiMagic2 != 0x6137 && uiMagic2 != 0x6139) return false;
    // Read and check last 2 footer bytes of file
    if(fC.FileMapReadVar16LEFrom(fC.Size() - sizeof(uint16_t)) != 0x3b00)
      return false;
    // Set gif callbacks
    gif_bitmap_callback_vt sBMPCB{ GIFCreate, GIFDestroy, GIFRead, GIFOpaque,
      GIFIsOpaque, GIFFlush };
    // Animations data and return code
    gif_animation gaData;
    // create our gif animation
    gif_create(&gaData, &sBMPCB);
    // Trap exceptions so we can clean up
    try
    { { // Decode the gif and report error if failed
        const gif_result grCode = gif_initialise(&gaData, fC.Size(),
          fC.Ptr<unsigned char>());
        if(grCode != GIF_OK) XC("GIF context failed!",
          "Total", gaData.frame_count, "Code", grCode);
      } // Reserve memory for frames
      ifD.sData.reserve(gaData.frame_count);
      // Set members
      ifD.uiBitsPerPixel = 32;
      ifD.uiBytesPerPixel = 4;
      ifD.glPixelType = GL_RGBA;
      ifD.bReverse = true;
      ifD.uiWidth = gaData.width;
      ifD.uiHeight = gaData.height;
      // Decode the frames
      for(unsigned int uiIndex = 0; uiIndex < gaData.frame_count; ++uiIndex)
      { // Decode the frame and throw exception if failed
        const gif_result grCode = gif_decode_frame(&gaData, uiIndex);
        if(grCode != GIF_OK) XC("GIF decode failed!",
          "Frame", uiIndex, "Total", gaData.frame_count, "Code", grCode);
        // Image size. Not using cached width and height just incase it
        // might change inside the .gif
        const size_t stSize = gaData.width * gaData.height * 4;
        // Since libnsgif has conveniently expanded the gif into RGBA we can
        // just go ahead and copy it all into our mem buffer.
        ifD.sData.push_back({ { stSize, gaData.frame_image },
           gaData.width, gaData.height });
        // Add to memory usage
        ifD.stAlloc += stSize;
      } // Success! Clean up
      gif_finalise(&gaData);
      // Success parsing gif
      return true;
    } // Caught an exception (unlikely)
    catch(const exception &)
    { // Clean up
      gif_finalise(&gaData);
      // Throw back to proper exception handler if message sent
      throw;
    } // Never gets here
  }
  /* -- Constructor -------------------------------------------------------- */
  CodecGIF(void) :
    /* -- Initialisation of members ---------------------------------------- */
    ImageFmt{ "Graphics Interchange Format", "GIF", Load }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecGIF);           // Omit copy constructor for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= */
/* ######################################################################### */
/* ## Portable Network Graphics                                       PNG ## */
/* ######################################################################### */
/* -- PNG Codec Object ----------------------------------------------------- */
class CodecPNG :
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* -- PNG callbacks ---------------------------------------------- */ public:
  static void PngError[[noreturn]](png_structp, png_const_charp pccString)
    { throw runtime_error{ pccString }; }
  static void PngWarning(png_structp, png_const_charp pccString)
    { LW(LH_WARNING, "Image PNG warning: $.", pccString); }
  static void PngRead(png_structp psD, png_bytep ucP, png_size_t stC)
  { // Read file and return if we read the correct number of bytes
    const size_t stRead = reinterpret_cast<FileMap*>
      (png_get_io_ptr(psD))->FileMapReadToAddr(ucP, stC);
    if(stRead == stC) return;
    // Error occured so longjmp()
    png_error(psD, Format("Read only $ of the $ requested bytes",
      stRead, stC).c_str());
  }
  /* -- Set png meta data -------------------------------------------------- */
  static void SetPngMeta(png_structp psData, png_infop piData,
    const string &strN, const string &strV)
  { // Create struct
    png_text ptData{
      PNG_TEXT_COMPRESSION_NONE,           // compression
      const_cast<png_charp>(strN.c_str()), // key
      const_cast<png_charp>(strV.c_str()), // text
      strV.length(),                       // text_length
      0,                                   // itxt_length (?)
      nullptr,                                // lang
      nullptr                                 // lang_key
    };
    // Set the header
    png_set_text(psData, piData, &ptData, 1);
  }
  /* ----------------------------------------------------------------------- */
  static bool Save(const FStream &fC, const ImageData &ifD,
                   const ImageSlot &bData)
  { // Check parameters
    if(ifD.uiBitsPerPixel != 24 && ifD.uiBitsPerPixel != 32)
      XC("Image is not 24 or 32bpp!", "BitsPerPixel", ifD.uiBitsPerPixel);
    if(!bData.uiWidth || !bData.uiHeight)
      XC("Dimensions are invalid!",
         "Width", bData.uiWidth, "Height", bData.uiHeight);
    if(bData.memData.Empty())
      XC("No image data!", "Size", bData.memData.Size());
    // Get main png structure
    png_structp psData = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      nullptr, PngError, PngWarning);
    if(!psData) XC("Create PNG write struct failed!");
    // Png info struct
    png_infop piData = nullptr;
    // Capture exceptions so we can clean up
    try
    { // Get png info structure
      piData = png_create_info_struct(psData);
      if(!piData) XC("Create PNG info struct failed!");
      // Assign handle
      png_init_io(psData, fC.FStreamGetHandle());
      // Write header (8 bit colour depth)
      png_set_IHDR(psData, piData, bData.uiWidth, bData.uiHeight,
        8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
      // Set metadata
      SetPngMeta(psData, piData, "Title",
        cCVars->GetInternalStrSafe(APP_LONGNAME));
      SetPngMeta(psData, piData, "Author",
        cCVars->GetInternalStrSafe(APP_AUTHOR));
      SetPngMeta(psData, piData, "Copyright",
        cCVars->GetInternalStrSafe(APP_COPYRIGHT));
      SetPngMeta(psData, piData, "Creation Time",
        cmSys.FormatTime().c_str());
      SetPngMeta(psData, piData, "Description",
        cSystem->ENGName()+" Exported Image");
      SetPngMeta(psData, piData, "Software",
        cSystem->ENGName()+" ("+cSystem->ENGBuildType()+") v"+
          ToString(cSystem->ENGMajor())+'.'+
          ToString(cSystem->ENGMinor())+'.'+
          ToString(cSystem->ENGBuild())+'.'+
          ToString(cSystem->ENGRevision())+
          " ("+ToString(cSystem->ENGBits())+"-bit) by "+
          cSystem->ENGAuthor()+'.');
      SetPngMeta(psData, piData, "Source", "OpenGL");
      SetPngMeta(psData, piData, "Comment",
        cCVars->GetInternalStrSafe(APP_WEBSITE));
      // Write info
      png_write_info(psData, piData);
      // Create vector array to hold uiRow pointers
      vector<png_bytep> vData{ bData.uiHeight };
      // Calculate size of a uiRow
      const unsigned int uiStride = bData.uiWidth * (ifD.uiBitsPerPixel / 8);
      // For each scanline
      // Set the pointer to the data pointer + i times the uiRow stride.
      // Notice that the uiRow order is reversed with q.
      // This is how at least OpenGL expects it,
      // and how many other image loaders present the data.
      for(unsigned int uiI = 0; uiI < bData.uiHeight; ++uiI)
        vData[uiI] = bData.memData.Read<png_byte>
          ((bData.uiHeight-uiI-1)*uiStride);
      // Write image
      png_write_image(psData, vData.data());
      // Write end
      png_write_end(psData, piData);
      // Done with png
      png_destroy_write_struct(&psData, &piData);
    } // Caught an error message from our libpng callback
    catch(...)
    { // Clean up
      png_destroy_write_struct(&psData, &piData);
      // Throw back to proper exception handler if message sent
      throw;
    } // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool Load(FileMap &fC, ImageData &ifD)
  { // Not a PNG file if not at least 8 bytes or header is incorrect
    if(fC.Size() < 8 || !png_check_sig(fC.FileMapReadPtr<png_byte>(8), 8))
      return false;
    // Get main png structure
    png_structp psData = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
      PngError, PngWarning);
    if(!psData) XC("Create PNG context failed!");
    // Info data strict
    png_infop piData = nullptr;
    // Capture exceptions so we can clean up
    try
    { // Get png info structure
      piData = png_create_info_struct(psData);
      if(!piData) XC("Create PNG info structure failed!");
      // Set read callback, header bytes and then read header
      png_set_read_fn(psData, reinterpret_cast<void*>(&fC), PngRead);
      png_set_sig_bytes(psData, 8);
      png_read_info(psData, piData);
      // Get colour info as we may need to perform conversions
      unsigned int uiChannels = png_get_channels(psData, piData);
      // Compare colour type
      switch(const unsigned int uiColourType =
        png_get_color_type(psData, piData))
      { // PNG has a colour palette?
        case PNG_COLOR_TYPE_PALETTE:
          // Expand the palette to RGB
          png_set_palette_to_rgb(psData);
          // Now has 3 bytes per channel
          uiChannels = 3;
        // PNG has standard colour type
        case PNG_COLOR_TYPE_GRAY:        // Luminance
        case PNG_COLOR_TYPE_GRAY_ALPHA:  // Luminance + Alpha
        case PNG_COLOR_TYPE_RGB:         // RGB
        case PNG_COLOR_TYPE_RGBA: break; // RGB + Alpha
        // Unsupported colour type
        default: XC("Unsupported colour type!", "Type", uiColourType);
      } // Get and check bits per channel (may change)
      unsigned int uiBitsPC = png_get_bit_depth(psData, piData);
      switch(uiBitsPC)
      { // 1, 2 and 4bpc need expanding
        case 1: case 2:
        case 4: png_set_expand_gray_1_2_4_to_8(psData); uiBitsPC = 8; break;
        // No conversion needed for 8bpc
        case 8: break;
        // 16 or 32bpc needs compression to 8bpc
        case 16: case 32: png_set_strip_16(psData); uiBitsPC = 8; break;
        // Unsupported bpc
        default: XC("Unsupported bits-per-channel!", "Bits", uiBitsPC);
      } // If the image has a transparency set
      if(png_get_valid(psData, piData, PNG_INFO_tRNS))
      { // Convert it to a full Alpha channel
        png_set_tRNS_to_alpha(psData);
        // Added the extra channel
        ++uiChannels;
      } // Set bits per pixel
      ifD.uiBitsPerPixel = uiBitsPC * uiChannels;
      ifD.uiBytesPerPixel = ifD.uiBitsPerPixel / 8;
      // Set colour mode
      switch(ifD.uiBitsPerPixel)
      { case 32: ifD.glPixelType = GL_RGBA; break; // 32-bpp
        case 24: ifD.glPixelType = GL_RGB;  break; // 24-bpp
        case 16: ifD.glPixelType = GL_RG;   break; // 16-bpp (Gray + Alpha)
        case  8: ifD.glPixelType = GL_RED;  break; // 8-bits-per-pixel (Gray)
        // Unsupported bit-depth
        default: XC("Unsupported bit-depth!",
                    "BitsPerPixel", ifD.uiBitsPerPixel);
      } // Get image dimensions
      ifD.uiWidth = png_get_image_width(psData, piData);
      ifD.uiHeight = png_get_image_height(psData, piData);
      // Initialise memory
      Memory aData(ifD.uiWidth * ifD.uiHeight * uiBitsPC * uiChannels / 8);
      // Create vector array to hold scanline pointers and size it
      vector<png_bytep> vData{ ifD.uiHeight };
      // For each scanline
      // Set the pointer to the data pointer + i times the uiRow stride.
      // Notice that the uiRow order is reversed with q.
      // This is how at least OpenGL expects it,
      // and how many other image loaders present the data.
      for(unsigned int uiStride = ifD.uiWidth * uiBitsPC * uiChannels / 8,
                       uiRow = 0;
                       uiRow < ifD.uiHeight;
                     ++uiRow)
        vData[uiRow] = aData.Read<png_byte>
          ((ifD.uiHeight - uiRow - 1) * uiStride, uiStride);
      // Read image and clean up
      png_read_image(psData, vData.data());
      png_read_end(psData, piData);
      png_destroy_read_struct(&psData, &piData, nullptr);
      // Success, add the image data to list
      ifD.stAlloc = aData.Size();
      ifD.sData.push_back({ move(aData), ifD.uiWidth, ifD.uiHeight });
      // OK!
      return true;
    } // Caught an error message from our libpng callback
    catch(const exception &)
    { // Clean up
      png_destroy_read_struct(&psData, &piData, nullptr);
      // Throw to main catch handler
      throw;
    }
  }
  /* -- Constructor -------------------------------------------------------- */
  CodecPNG(void) :
    /* -- Initialisation of members ---------------------------------------- */
    ImageFmt{ "Portable Network Graphics", "PNG", Load, Save }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecPNG);           // Omit copy constructor for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= */
/* ######################################################################### */
/* ## JPEG format                                                    JPEG ## */
/* ######################################################################### */
/* -- JPEG Codec Object ---------------------------------------------------- */
class CodecJPG :
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* -- JPEG callbacks --------------------------------------------- */ public:
  static void JPegInitSource(j_decompress_ptr) { }
  static void JPegTermSource(j_decompress_ptr) { }
  static boolean JPegFillInputBuffer(j_decompress_ptr)
    { return static_cast<boolean>(true); }
  static void JPegSkipInputData(j_decompress_ptr ciData, long lBytes)
  { // This function is called when jpeg needs to skip data, such as ICC
    // profiles. If we have no bytes then we're done
    if(lBytes <= 0) return;
    // Get jpeg data
    struct jpeg_source_mgr &jsmData = *ciData->src;
    // Since we're loading from memory, we can just move the pointers around.
    jsmData.next_input_byte += static_cast<size_t>(lBytes);
    jsmData.bytes_in_buffer -= static_cast<size_t>(lBytes);
  }
  static void JPegErrorExit[[noreturn]](j_common_ptr ciData)
  { // Buffer for error message
    string strMsg(JMSG_LENGTH_MAX, 0);
    // Popular error message
    (*(ciData->err->format_message))(ciData,const_cast<char*>(strMsg.c_str()));
    // Throw back to intermediate handler so we can cleanup libjpeg
    throw runtime_error{ strMsg.c_str() };
  }
  /* ----------------------------------------------------------------------- */
  static bool Save(const FStream &fC, const ImageData &ifD,
                   const ImageSlot &bData)
  {  // Check parameters
    if(ifD.uiBitsPerPixel != 24) XC("Only RGB supported!",
      "BitsPerPixel", ifD.uiBitsPerPixel);
    if(!bData.uiWidth || !bData.uiHeight)
      XC("Dimensions are invalid!",
        "Width", bData.uiWidth, "Height", bData.uiHeight);
    if(bData.memData.Empty())
      XC("No image data!", "Size", bData.memData.Size());
    // We need access to this from the exception block
    struct jpeg_compress_struct ciData;
    // Capture exceptions
    try
    { // Setup error manager
      struct jpeg_error_mgr jemData;
      ciData.err = jpeg_std_error(&jemData);
      jemData.error_exit = JPegErrorExit;
      // Setup decompressor
      jpeg_create_compress(&ciData);
      // first time for this JPEG object?
      jpeg_stdio_dest(&ciData, fC.FStreamGetHandle());
      // Set info
      ciData.image_width = bData.uiWidth;
      ciData.image_height = bData.uiHeight;
      ciData.input_components = ifD.uiBitsPerPixel / 8;
      ciData.in_color_space = JCS_RGB;
      // Call the setup defualts helper function to give us a starting point.
      // Note, don’t call any of the helper functions before you call this,
      // they will no effect if you do. Then call other helper functions,
      // here I call the set quality. Then start the compression.
      jpeg_set_defaults(&ciData);
      // Set the quality
      jpeg_set_quality(&ciData, 100, static_cast<boolean>(true));
      // We now start compressing
      jpeg_start_compress(&ciData, static_cast<boolean>(true));
      // Now we encode each can line
      while(ciData.next_scanline < ciData.image_height)
      { // Get scanline
        JSAMPROW rPtr = reinterpret_cast<JSAMPROW>(bData.memData.Read
          ((ciData.image_height-1-ciData.next_scanline)*
            static_cast<unsigned int>(ciData.input_components)*
            ciData.image_width));
        // Write scanline to disk
        jpeg_write_scanlines(&ciData, &rPtr, 1);
      } // Finished compressing
      jpeg_finish_compress(&ciData);
      // Cleanup
      jpeg_destroy_compress(&ciData);
    } // exception occured
    catch(const exception &E)
    { // Get code because we're about to destroy the data struct
      const int iCode = ciData.err ? ciData.err->msg_code : -1;
      // Cleanup
      jpeg_destroy_compress(&ciData);
      // Throw back to proper exception handler if message sent
      XC(E.what(), "Code", iCode);
    } // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool Load(FileMap &fC, ImageData &ifD)
  { // Make sure we have at least 12 bytes and the correct first 2 bytes
    if(fC.Size() < 12 || fC.FileMapReadVar16LE() != 0xD8FF) return false;
    // We need access to this from the exception block
    struct jpeg_decompress_struct ciData;
    // Capture exceptions
    try
    { // Setup error manager
      jpeg_error_mgr jemData;
      ciData.err = jpeg_std_error(&jemData);
      jemData.error_exit = JPegErrorExit;
      // Setup decompressor
      jpeg_create_decompress(&ciData);
      // Allocate memory manager object with a pointer to our file class
      ciData.src = reinterpret_cast<jpeg_source_mgr *>
        ((*ciData.mem->alloc_small) (reinterpret_cast<j_common_ptr>(&ciData),
          JPOOL_PERMANENT, sizeof(jpeg_source_mgr)));
      // Setup source manager
      jpeg_source_mgr &jsmData =
        *reinterpret_cast<struct jpeg_source_mgr *>(ciData.src);
      jsmData.init_source = JPegInitSource;
      jsmData.fill_input_buffer = JPegFillInputBuffer;
      jsmData.skip_input_data = JPegSkipInputData;
      jsmData.resync_to_restart = jpeg_resync_to_restart;
      jsmData.term_source = JPegTermSource;
      jsmData.next_input_byte = fC.Ptr<JOCTET>();
      jsmData.bytes_in_buffer = fC.Size();
      // Read the header and being decompression
      jpeg_read_header(&ciData, static_cast<boolean>(true));
      jpeg_start_decompress(&ciData);
      // Make sure component count is valid
      switch(ciData.output_components)
      { // (1 byte)   8-bpp grayscale is handled as luminosity.
        case 1: ifD.glPixelType = GL_RED; break;
        // (3 bytes) 24-bpp of course is handled natively.
        case 3: ifD.glPixelType = GL_RGB; break;
        // (4 bytes) 32-bpp of course is handled natively.
        case 4: ifD.glPixelType = GL_RGBA; break;
        // Anything else, throw back to proper exception handler
        default: XC("Component count unsupported!",
                     "OutputComponents", ciData.output_components);
      } // Set dimensions and pixel bit depth
      ifD.uiWidth = ciData.output_width;
      ifD.uiHeight = ciData.output_height;
      ifD.uiBytesPerPixel =
        static_cast<unsigned int>(ciData.output_components);
      ifD.uiBitsPerPixel = ifD.uiBytesPerPixel * 8;
      // Jpegs are reversed
      ifD.bReverse = true;
      // Create space for decompressed image
      Memory aData{ ifD.uiWidth * ifD.uiHeight * ifD.uiBytesPerPixel };
      // Get row stride (or number of bytes in a scanline of image data)
      const size_t stRowStride = ifD.uiWidth * ifD.uiBytesPerPixel;
      // Decompress scanlines
      while(ciData.output_scanline < ciData.output_height)
      { // For storing info
        array<unsigned char*,1> caPtr{
          aData.Read<unsigned char>(
            static_cast<size_t>(ciData.output_scanline) * stRowStride,
            stRowStride)
        }; // Decompress the data
        jpeg_read_scanlines(&ciData,
          reinterpret_cast<JSAMPARRAY>(caPtr.data()),
          static_cast<JDIMENSION>(caPtr.size()));
      } // Cleanup jpeg now we've done with it
      jpeg_destroy_decompress(&ciData);
      // Add data to image list
      ifD.stAlloc = aData.Size();
      ifD.sData.push_back({ move(aData), ifD.uiWidth, ifD.uiHeight });
      // Success
      return true;
    } // Exiting cleanly?
    catch(const exception &E)
    { // Get code because we're about to destroy the data struct
      const int iCode = ciData.err ? ciData.err->msg_code : -1;
      // Cleanup
      jpeg_destroy_decompress(&ciData);
      // Throw back to proper exception handler if message sent
      XC("Decode failed!", "Message", E.what(), "Code", iCode);
    }
  }
  /* -- Constructor -------------------------------------------------------- */
  CodecJPG(void) :
    /* -- Initialisation of members ---------------------------------------- */
    ImageFmt{ "Joint Photographic Experts Group", "JPG", Load, Save }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecJPG);           // Omit copy constructor for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= */
/* ######################################################################### */
/* ## Targa Bitmap Format                                             TGA ## */
/* ######################################################################### */
/* -- TGA Codec Object ----------------------------------------------------- */
class CodecTGA :
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* -- TGA header is 18 bytes long -------------------------------- */ public:
  enum HeaderLayout : size_t {         // Header layout for TGA (=location)
    HL_U08LE_LEN_AFTER_HEADER  =    0, // Length after hdr (0 usually)
    HL_U08LE_COLOUR_MAP_TYPE   =    1, // 0=none, 1=has palette
    HL_U08LE_DATA_TYPE_CODE    =    2, // 0=non, 1=index, 2=rgb, 3=gray, +8=rle
    HL_U16LE_COLOUR_MAP_ORIGIN =    3, // First colour map entry in palete
    HL_U16LE_COLOUR_MAP_LENGTH =    5, // Number of colours in palette
    HL_U08LE_COLOUR_MAP_DEPTH  =    7, // Number of bits per pal entry
    HL_U16LE_X_ORIGIN          =    8, // Image X origin
    HL_U16LE_Y_ORIGIN          =   10, // Image Y origin
    HL_U16LE_WIDTH             =   12, // Image width in pixels
    HL_U16LE_HEIGHT            =   14, // Image height in pixels
    HL_U08LE_BITS_PER_PIXEL    =   16, // Image bits per pixel
    HL_U08LE_IMAGE_DESCRIPTOR  =   17, // Image descriptor
    HL_MAX                     =   18  // End of header. (Palette/pixels data)
  };
  /* ----------------------------------------------------------------------- */
  enum ImageDescriptorFlags {          // Valid entries for IMAGE_DESCRIPTOR
    IDF_NUM_ATTR_BITS          = 0x08, // Num attribute bits for each pixel
    IDF_RESERVED               = 0x10, // Reserved must be zero
    IDF_FLIPPED                = 0x20, // Origin (0:bottom-left, 1:upper-left)
    IDF_IL_TWO_WAY             = 0x40, // Two-way (even/odd) interleaving
    IDF_IL_FOUR_WAY            = 0x80, // Four way interleaving
    IDF_IL_RESERVED            = 0xC0  // Reserved interleaving
  };
  /* ----------------------------------------------------------------------- */
  enum ColourMapType {                 // Helper codes for colour map type
    CMT_NONE,                          // FileMap does not have palette
    CMT_PALETTE,                       // FileMap has palette
    CMT_MAX,                           // Maximum allowed value
  };
  /* ----------------------------------------------------------------------- */
  enum DataTypeCode {                  // Helper codes for pixel data type
    DTC_NONE,    /* No pixel data */ DTC_INDEX,     /* Indexed       (00-01) */
    DTC_RGB,     /* RGB pixels    */ DTC_GRAY,      /* 8b grayscale  (02-03) */
    DTC_NA_4,    /* Unused        */ DTC_NA_5,      /* Unused        (04-05) */
    DTC_NA_6,    /* Unused        */ DTC_NA_7,      /* Unused        (06-07) */
    DTC_RLE,     /* RLE packed    */ DTC_RLE_INDEX, /* RLE indexed   (08-09) */
    DTC_RLE_RGB, /* RLE pack RGB  */ DTC_RLE_GRAY,  /* RLE grayscale (10-11) */
    DTC_MAX,     /* Maximum number of entries supported                 (12) */
  };
  /* ----------------------------------------------------------------------- */
  enum BitsPerPixel {                  // Helper codes for pixels type
    BPP_BINARY                   =  1, // Pixels are monochrome
    BPP_INDEXED                  =  8, // Pixels are indexed (palette or grays)
    BPP_RGB555                   = 15, // Pixels in RGB order (R5,G5,B5)
    BPP_RGBA5551                 = 16, // Pixels in RGB order (R5,G5,B5,1A)
    BPP_RGB888                   = 24, // Pixels in RGB order (R8,G8,B8)
    BPP_RGBA8888                 = 32, // Pixels in RGBA order (R8,G8,B8,A8)
  };
  /* ----------------------------------------------------------------------- */
  static bool Save(const FStream &fC, const ImageData &ifD,
                   const ImageSlot &sData)
  { // Check parameters
    if(ifD.uiBitsPerPixel != BPP_RGB888 && ifD.uiBitsPerPixel != BPP_RGBA8888)
      XC("Only RGB(A) supported!", "BitsPerPixel", ifD.uiBitsPerPixel);
    if(!sData.uiWidth || !sData.uiHeight)
      XC("Invalid dimensions!", "Width", sData.uiWidth,
                                "Height", sData.uiHeight);
    // Get input data and check size
    const DataConst &dcIn = sData.memData;
    if(dcIn.Empty()) XC("No image data!", "Size", dcIn.Size());
    // Duplicate memory data because we need to convert it to BGR
    Memory mbOut{ dcIn.Size(), dcIn.Ptr<void>() };
    // Swap pixels
    if(const int iResult = ImageSwapPixels(mbOut.Ptr<char>(),
      mbOut.Size(), ifD.uiBytesPerPixel, 0, 2))
        XC("Re-order to BGR(A) failed!",
           "Code",      iResult,      "Address", mbOut.Ptr<void>(),
           "Length",    mbOut.Size(), "BPP",     ifD.uiBytesPerPixel,
           "PixelType", ifD.glPixelType);
    // Allocate and write targa header. We don't write using a structure
    // because in the future, we will need to modify this to support
    // big-endian arcitechtures so we have to convert to big-endian when
    // writing these values which will make writing the code for this easier.
    Memory mbHdr{ HL_MAX };
    mbHdr.WriteInt<uint8_t>(HL_U08LE_LEN_AFTER_HEADER, 0);
    mbHdr.WriteInt<uint8_t>(HL_U08LE_COLOUR_MAP_TYPE, CMT_NONE);
    mbHdr.WriteInt<uint8_t>(HL_U08LE_DATA_TYPE_CODE, DTC_RGB);
    mbHdr.WriteIntLE<uint16_t>(HL_U16LE_COLOUR_MAP_ORIGIN, 0);
    mbHdr.WriteIntLE<uint16_t>(HL_U16LE_COLOUR_MAP_LENGTH, 0);
    mbHdr.WriteInt<uint8_t>(HL_U08LE_COLOUR_MAP_DEPTH, CMT_NONE);
    mbHdr.WriteIntLE<uint16_t>(HL_U16LE_X_ORIGIN, 0);
    mbHdr.WriteIntLE<uint16_t>(HL_U16LE_Y_ORIGIN, 0);
    mbHdr.WriteIntLE<uint16_t>(HL_U16LE_WIDTH,
      static_cast<uint16_t>(sData.uiWidth));
    mbHdr.WriteIntLE<uint16_t>(HL_U16LE_HEIGHT,
      static_cast<uint16_t>(sData.uiHeight));
    mbHdr.WriteInt<uint8_t>(HL_U08LE_BITS_PER_PIXEL,
      static_cast<uint8_t>(ifD.uiBitsPerPixel));
    mbHdr.WriteInt<uint8_t>(HL_U08LE_IMAGE_DESCRIPTOR, 0);
    // Ok write the head/er and the data and some padding at the end
    if(fC.FStreamWrite(mbHdr.Ptr<char>(), mbHdr.Size()) != mbHdr.Size())
      XCL("Write header error!",
        "Size", mbHdr.Size(), "Address",  mbHdr.Ptr<void>());
    if(fC.FStreamWrite(mbOut.Ptr<char>(), mbOut.Size()) != mbOut.Size())
      XCL("Write data error!",
        "Size", mbOut.Size(), "Address",  mbOut.Ptr<void>());
    // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool CopyPixels(FileMap &fC, ImageData &ifD)
  { // Calculate size (Kinda dirty, but we already checked above)
    const size_t stImageSize = (ifD.uiBitsPerPixel < 8) ?
        (ifD.uiWidth * ifD.uiHeight / 8)
      : (ifD.uiWidth * ifD.uiHeight * ifD.uiBytesPerPixel);
    // We can just copy the whole block and make sure we read it all
    Memory aData{ fC.FileMapReadBlock(stImageSize) };
    if(aData.Size() != stImageSize)
      XC("Read error!", "Expected", stImageSize, "Actual", aData.Size());
    // Add image to list
    ifD.stAlloc = aData.Size();
    ifD.sData.push_back({ move(aData), ifD.uiWidth, ifD.uiHeight });
    // OK!
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodeRLE(FileMap &fC, ImageData &ifD)
  { // Calculate bytes per pixel
    const size_t stBPP = ifD.uiBitsPerPixel / 8;
    // Initialise output memory block for uncompressed data
    Memory aData{ ifD.uiWidth * ifD.uiHeight * stBPP };
    // Until end of file
    for(size_t stO = 0; fC.FileMapIsNotEOF(); )
    { // Read and process header and get run-length count
      const unsigned int uiHeader = fC.FileMapReadVar<uint8_t>();
      const size_t stCount = (uiHeader & 0x7F) + 1;
      // RLE encoded pixel?
      if(uiHeader & 0x80)
      { // Read address of pixel
        const char*const cpPixel = fC.FileMapReadPtr(stBPP);
        // Insert requested number of packets into output buffer
        for(const size_t stEnd = stO + (stBPP * stCount);
                                 stO < stEnd;
                                 stO += stBPP)
          aData.Write(stO, cpPixel, stBPP);
        // Next packet
        continue;
      } // Raw pixel. Get amount of data to copy
      const size_t stToCopy = stCount * stBPP;
      // Copy the pixel
      aData.Write(stO, fC.FileMapReadPtr(stToCopy), stToCopy);
      // Output pointer moved forward
      stO += stToCopy;
    } // Add image to list
    ifD.stAlloc = aData.Size();
    ifD.sData.push_back({ move(aData), ifD.uiWidth, ifD.uiHeight });
    // Decode success!
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool Load(FileMap &fC, ImageData &ifD)
  { // Read header from start and check size and magic
    if(fC.Size() < HL_MAX || fC.FileMapReadVar<uint8_t>()) return false;
    // Get colour map type. Return unknown format if value out of range
    const ColourMapType uiColourMapType = TestEnumRange(
      static_cast<ColourMapType>(fC.FileMapReadVar<uint8_t>()),
        CMT_MAX, CMT_NONE);
    if(uiColourMapType == CMT_MAX) return false;
    // Get pixel data type. Return unknown format if value out of range
    const DataTypeCode uiDataTypeCode = TestEnumRange(
      static_cast<DataTypeCode>(fC.FileMapReadVar<uint8_t>()),
        DTC_MAX, DTC_NONE);
    if(uiDataTypeCode == DTC_MAX) return false;
    // We're not supporting colour map origins
    const unsigned int uiColourMapOrigin = fC.FileMapReadVar16LE();
    if(uiColourMapOrigin)
      XC("TGA colour map origin unsupported!",
         "ColourMapOrigin", uiColourMapOrigin);
    // Get colour map length and pixel depth
    const unsigned int uiColourMapLength = fC.FileMapReadVar16LE();
    const BitsPerPixel uiColourMapDepth =
      static_cast<BitsPerPixel>(fC.FileMapReadVar<uint8_t>());
    // Get origins, but origins not supported
    const unsigned int uiXorigin = fC.FileMapReadVar16LE();
    const unsigned int uiYorigin = fC.FileMapReadVar16LE();
    if(uiXorigin || uiYorigin)
      XC("TGA origins unsupported!",
         "OriginX", uiXorigin, "OriginY", uiYorigin);
    // Get widths and check that they're valid
    ifD.uiWidth = fC.FileMapReadVar16LE();
    ifD.uiHeight = fC.FileMapReadVar16LE();
    if(!ifD.uiWidth||!ifD.uiHeight)
      XC("TGA dimensions invalid!",
         "Width", ifD.uiWidth, "Height", ifD.uiHeight);
    // Get bit depth
    ifD.uiBitsPerPixel = fC.FileMapReadVar<uint8_t>();
    const BitsPerPixel uiBitsPerPixel =
      static_cast<BitsPerPixel>(ifD.uiBitsPerPixel);
    ifD.uiBitsPerPixel = uiBitsPerPixel;
    // Get image descriptor bits
    const ImageDescriptorFlags idFlags =
      static_cast<ImageDescriptorFlags>(fC.FileMapReadVar<uint8_t>());
    ifD.bReverse = !!(idFlags & IDF_FLIPPED);
    // Interleaving or attribute bits are not supported
    if(idFlags & (IDF_NUM_ATTR_BITS|IDF_IL_TWO_WAY|IDF_IL_FOUR_WAY))
      XC("TGA has unsupported image descriptor flags!", "Flags", idFlags);
    // Check bit depth
    switch(ifD.uiBitsPerPixel)
    { // Monochrome : 1-bit per pixel?
      case BPP_BINARY:
      { // Only the 'gray' data type code can be used here
        if(uiDataTypeCode != DTC_GRAY)
          XC("Monochrome TGA must be data type 3!", "Code", uiDataTypeCode);
        // Colour-map type is not supported
        if(uiColourMapType == CMT_PALETTE)
          XC("Monochrome TGA does not support colour map!",
             "Code", uiColourMapType);
        // Update the pixel type to zero. This format can ONLY be used with
        // the BitMask colission system and cannot be used in OpenGL as there
        // is no shader support that will translate it yet.
        ifD.glPixelType = 0;
        // Pass to generic copy function
        return CopyPixels(fC, ifD);
      } // Paletted or gray : 8-bits per pixel?
      case BPP_INDEXED:
      { // Only 'index' or 'gray' data type is supported
        switch(uiDataTypeCode)
        { // Paletted?
          case DTC_INDEX:
          { // Colour-map type must be supported
            if(uiColourMapType == CMT_NONE)
              XC("Paletted TGA must support colour map!",
                "ColourMapType", uiColourMapType);
            // Only 24-bit palette entries supported
            if(uiColourMapDepth != BPP_RGB888)
              XC("Paletted TGA must only have 24bit palette entries!",
                "ColourMapDepth", uiColourMapDepth);
            // Read the indexes
            const size_t stPaletteSize =
              uiColourMapLength * (uiColourMapDepth / 8);
            const Memory aPalette{ fC.FileMapReadBlock(stPaletteSize) };
            if(aPalette.Size() != stPaletteSize)
              XC("Read palette error!",
                 "Expected", stPaletteSize, "Actual", aPalette.Size());
            // Read the pixel data
            const size_t stImageSize = ifD.uiWidth * ifD.uiHeight;
            const Memory aPixels{ fC.FileMapReadBlock(stImageSize) };
            if(aPixels.Size() != stImageSize)
              XC("Read pixels error!",
                 "Expected", stImageSize, "Actual", aPixels.Size());
            // Create output buffer and enumerate through the pixels
            const size_t stOutSize = stImageSize * 3;
            Memory aOut{ stOutSize };
            for(size_t stIn = 0, stOut = 0;
                       stIn < stImageSize;
                     ++stIn, stOut += 3)
            { // Read palette index from pixel data, then write the RGB value
              // from the palette to the output image.
              aOut.Write(stOut,
                aPalette.Read(aPixels.ReadInt<uint8_t>(stIn) * 3), 3);
            } // Update output, we will be converting to rgb
            ifD.uiBitsPerPixel = BPP_RGB888;
            ifD.glPixelType = GL_BGR;
            ifD.uiBytesPerPixel = 3;
            // Add image to list
            ifD.stAlloc = aOut.Size();
            ifD.sData.push_back({ move(aOut),
              ifD.uiWidth, ifD.uiHeight });
            // Done
            return true;
          } // Grayscale?
          case DTC_GRAY:
          { // Colour-map type must be supported
            if(uiColourMapType == CMT_PALETTE)
              XC("Gray TGA must not support colour map!",
                 "ColourMapType", uiColourMapType);
            // Update output, we will be converting to rgb
            ifD.uiBitsPerPixel = BPP_INDEXED;
            ifD.glPixelType = GL_RED;
            ifD.uiBytesPerPixel = 1;
            // Pass to generic copy function
            return CopyPixels(fC, ifD);
          } // Paletted RLE?
          case DTC_RLE_INDEX:
          { // Colour-map type must be supported
            if(uiColourMapType == CMT_NONE)
              XC("Paletted RLE TGA must support colour map!",
                "ColourMapType", uiColourMapType);
            // Only 24-bit palette entries supported
            if(uiColourMapDepth != BPP_RGB888)
              XC("Paletted RLE TGA must only have 24bit palette entries!",
                "ColourMapDepth", uiColourMapDepth);
            // Read the indexes
            const size_t stPalBytesPerPix = uiColourMapDepth / 8;
            const size_t stPaletteSize = uiColourMapLength * stPalBytesPerPix;
            const Memory aPalette{ fC.FileMapReadBlock(stPaletteSize) };
            if(aPalette.Size() != stPaletteSize)
              XC("Read palette error!",
                 "Expected", stPaletteSize, "Actual", aPalette.Size());
            // Create output buffer and enumerate through the pixels
            const size_t stImageSize = ifD.uiWidth * ifD.uiHeight;
            const size_t stOutSize = stImageSize * stPalBytesPerPix;
            Memory aData{ stOutSize };
            // Until end of file
            for(size_t stO = 0; fC.FileMapIsNotEOF(); )
            { // Read and process header and get run-length count
              const unsigned int uiHeader = fC.FileMapReadVar<uint8_t>();
              const size_t stCount = (uiHeader & 0x7F) + 1;
              // RLE encoded pixel?
              if(uiHeader & 0x80)
              { // Read index of palette entry to lookup
                const char*const cpPixel =
                  aPalette.Read(fC.FileMapReadVar<uint8_t>() *
                  stPalBytesPerPix, stPalBytesPerPix);
                // Insert requested number of pixels into output buffer
                for(const size_t stEnd = stO+(stCount*stPalBytesPerPix);
                                         stO < stEnd;
                                         stO += stPalBytesPerPix)
                  aData.Write(stO, cpPixel, stPalBytesPerPix);
                // Next packet
                continue;
              } // Read the number of raw indexes to lookup in palette and
              // lookup and write each pixel to the new image.
              for(uint8_t *ucIndex = fC.FileMapReadPtr<uint8_t>(stCount),
                    *const ucEnd = ucIndex + stCount;
                           ucIndex < ucEnd;
                         ++ucIndex, stO += stPalBytesPerPix)
                aData.Write(stO, aPalette.Read(static_cast<size_t>(*ucIndex) *
                  stPalBytesPerPix, stPalBytesPerPix), stPalBytesPerPix);
            } // Update output, we will be converting to rgb
            ifD.uiBitsPerPixel = BPP_RGB888;
            ifD.glPixelType = GL_BGR;
            ifD.uiBytesPerPixel = static_cast<unsigned int>(stPalBytesPerPix);
            // Add image to list
            ifD.stAlloc = aData.Size();
            ifD.sData.push_back({ move(aData),
              ifD.uiWidth, ifD.uiHeight });
            // OK!
            return true;
          } // Grayscale RLE?
          case DTC_RLE_GRAY:
          { // Colour-map type must be supported
            if(uiColourMapType == CMT_PALETTE)
              XC("Gray RLE TGA must not support colour map!",
                "ColourMapType", uiColourMapType);
            // Decode the RLE
            if(!DecodeRLE(fC, ifD)) return false;
            // Decode success. Image is now indexed
            ifD.uiBitsPerPixel = BPP_INDEXED;
            // Pixel type changed to grayscale
            ifD.glPixelType = GL_RED;
            // Now 8-bits per pixel
            ifD.uiBytesPerPixel = 1;
            // Success
            return true;
          } // Expandable targa is not supported!
          default: XC("Indexed TGA has unsupported data type!",
                      "DataTypeCode", uiDataTypeCode);
        } // Should not get here
      } // 16.7 million colours : 24-bits per pixel
      case BPP_RGB888:
      { // Update output type to BGR with three bytes per pixel
        ifD.glPixelType = GL_BGR;
        ifD.uiBytesPerPixel = 3;
        // Only 'rgb' or 'rgbrle' data type is supported
        switch(uiDataTypeCode)
        { // Uncompressed RGB raw pixel encoding? Can just be copied
          case DTC_RGB: return CopyPixels(fC, ifD);
          // RLE compressed RGB encoding? ImageData needs decoding
          case DTC_RLE_RGB: return DecodeRLE(fC, ifD);
          // Anything else is not allowed
          default: XC("RGB TGA must be data type 2 or 10!",
                      "DataTypeCode", uiDataTypeCode);
        } // Should not get here
      } // 16.7 million colours with alpha channel : 32-bits per pixel
      case BPP_RGBA8888:
      { // Update output type to BGRA with four bytes per pixel
        ifD.glPixelType = GL_BGRA;
        ifD.uiBytesPerPixel = 4;
        // Only 'rgb' or 'rgbrle' data type is supported
        switch(uiDataTypeCode)
        { // Uncompressed RGB raw pixel encoding. Can just be copied
          case DTC_RGB: return CopyPixels(fC, ifD);
          // RLE compressed RGB encoding. Needs decoding
          case DTC_RLE_RGB: return DecodeRLE(fC, ifD);
          // Anything else is not allowed
          default: XC("RGBA TGA must be data type 2 or 10!",
                      "DataTypeCode", uiDataTypeCode);
        } // Should not get here
      } // Unknown or invalid. Throw error as the bit-depth is not recognised
      default: XC("TGA bit-depth not supported!",
        "BitsPerPixel", ifD.uiBitsPerPixel);
    }
  }
  /* -- Constructor -------------------------------------------------------- */
  CodecTGA(void) :
    /* -- Initialisation of members ---------------------------------------- */
    ImageFmt{ "Truevision Targa", "TGA", Load, Save }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecTGA);           // Omit copy constructor for safety
};/* -- End ---------------------------------------------------------------- */
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
