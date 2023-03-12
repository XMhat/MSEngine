/* == IMAGEFMT.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## These classes are plugins for the ImgLib manager to allow loading   ## */
/* ## of certain formatted images.                                        ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfImageFormat {              // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace Library::Png;          // Using libpng library functions
using namespace Library::JpegTurbo;    // Using jpegturbo library functions
using namespace Library::NSGif;        // Using libnsgif library functions
using namespace IfImageLib;            // Using imagelib namespace
using namespace IfCVar;                // Using cvar namespace
/* ========================================================================= */
/* ######################################################################### */
/* ## DirectDraw Surface                                              DDS ## */
/* ######################################################################### */
/* -- DDS Codec Object ----------------------------------------------------- */
struct CodecDDS final :                // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* ----------------------------------------------------------------------- */
  // *** DDS_FILEHEADER (128 bytes) ***
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
  /* -- Header flags ------------------------------------------------------- */
  enum DDSHeaderFlags : unsigned int {
    DDSD_CAPS        = 0x00000001,   DDSD_HEIGHT      = 0x00000002,
    DDSD_WIDTH       = 0x00000004,   DDSD_PITCH       = 0x00000008,
    DDSD_PIXELFORMAT = 0x00001000,   DDSD_MIPMAPCOUNT = 0x00020000,
    DDSD_LINEARSIZE  = 0x00080000,   DDSD_DEPTH       = 0x00800000,
    DDSD_REQUIRED    = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT|
                       DDSD_MIPMAPCOUNT,
    DDSD_MASK        = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PITCH|
                       DDSD_PIXELFORMAT|DDSD_MIPMAPCOUNT|DDSD_LINEARSIZE|
                       DDSD_DEPTH
  };/* -- Pixel format flags ----------------------------------------------- */
  enum DDSPixelFormatFlags : unsigned int {
    DDPF_ALPHAPIXELS = 0x00000001,   DDPF_ALPHA       = 0x00000002,
    DDPF_FOURCC      = 0x00000004,   DDPF_RGB         = 0x00000040,
    DDPF_YUV         = 0x00000200,   DDPF_LUMINANCE   = 0x00020000,
    DDPF_MASK        = DDPF_ALPHAPIXELS|DDPF_ALPHA|DDPF_FOURCC|DDPF_RGB|
                       DDPF_YUV|DDPF_LUMINANCE
  };/* -- FourCC formats --------------------------------------------------- */
  enum DDSFourCCFormat : unsigned int {
    FOURCC_DXT1      = 0x31545844,   FOURCC_DXT3      = 0x33545844,
    FOURCC_DXT5      = 0x35545844
  };/* -- Capabilities primary flags --------------------------------------- */
  enum DDSCapsFlags : unsigned int {
    DDSCAPS_COMPLEX  = 0x00000008,   DDSCAPS_TEXTURE  = 0x00001000,
    DDSCAPS_MIPMAP   = 0x00400000,
    DDSCAPS_MASK     = DDSCAPS_COMPLEX|DDSCAPS_TEXTURE|DDSCAPS_MIPMAP
  };/* -- Capabilities secondary flags ------------------------------------- */
  enum DDSCaps2Flags : unsigned int {
    DDSCAPS2_CUBEMAP           = 0x00000200,
    DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
    DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
    DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
    DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
    DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
    DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
    DDSCAPS2_VOLUME            = 0x00200000,
    DDSCAPS2_MASK              = DDSCAPS2_CUBEMAP|DDSCAPS2_CUBEMAP_POSITIVEX|
                                 DDSCAPS2_CUBEMAP_NEGATIVEX|
                                 DDSCAPS2_CUBEMAP_POSITIVEY|
                                 DDSCAPS2_CUBEMAP_NEGATIVEY|
                                 DDSCAPS2_CUBEMAP_POSITIVEZ|
                                 DDSCAPS2_CUBEMAP_NEGATIVEZ|DDSCAPS2_VOLUME
  };
  /* ----------------------------------------------------------------------- */
  static bool Load(FileMap &fC, ImageData &ifD)
  { // Get file size
    const size_t stSize = fC.Size();
    // Quick check header which should be at least 128 bytes
    if(stSize < 128 || fC.FileMapReadVar32LE() != 0x20534444) return false;
    // Read the image size and flags and if we got them
    if(const unsigned int uiSize = fC.FileMapReadVar32LE())
    { // Must be 124 bytes long
      if(uiSize != 124)
        XC("Header size must be 124 bytes!", "Required", stSize);
    } // No header bytes so throw error
    else XC("Header bytes not specified!");
    // Read header flags and if specified?
    if(const unsigned int uiFlags = fC.FileMapReadVar32LE())
    { // Check that they're in the correct range
      if(uiFlags & ~DDSD_MASK)
        XC("Header flags invalid!", "Flags", uiFlags);
      // We should have caps, width and pixelformat
      if(uiFlags & ~DDSD_REQUIRED)
        XC("Basic flags required!", "Flags", uiFlags, "Mask", DDSD_REQUIRED);
    } // Get and check dimensions
    if(!ifD.SetDimSafe(fC.FileMapReadVar32LE(), fC.FileMapReadVar32LE()))
      XC("DDS dimensions invalid!", "Width",  ifD.DimGetWidth(),
                                    "Height", ifD.DimGetHeight());
    // Get pitch or linear size
    if(const unsigned int uiPitchOrLinearSize = fC.FileMapReadVar32LEFrom(20))
      XC("Pitch or linear size not supported!", "Data", uiPitchOrLinearSize);
    // Get bit-depth, and it must be a power of two
    ifD.SetBitsPerPixelCast(fC.FileMapReadVar32LE());
    if((ifD.GetBitsPerPixel() != BD_NONE && !IsPow2(ifD.GetBitsPerPixel())) ||
        ifD.GetBitsPerPixel() > BD_RGBA)
      XC("DDS bit-depth not valid!",
         "BitsPerPixel", ifD.GetBitsPerPixel());
    // Get and check mipmap count. Theres still 1 image if this is zero.
    const unsigned int uiMipMapCount = fC.FileMapReadVar32LE() + 1;
    if(uiMipMapCount > 1) ifD.SetMipmaps();
    // Ignore other flags
    if(const uint64_t qReserved = fC.FileMapReadVar64LE())
      XC("Reserved values 0 to 1 must not be set!", "Data", qReserved);
    if(const uint64_t qReserved = fC.FileMapReadVar64LE())
      XC("Reserved values 2 to 3 must not be set!", "Data", qReserved);
    if(const uint64_t qReserved = fC.FileMapReadVar64LE())
      XC("Reserved values 4 to 5 must not be set!", "Data", qReserved);
    if(const uint64_t qReserved = fC.FileMapReadVar64LE())
      XC("Reserved values 6 to 7 must not be set!", "Data", qReserved);
    if(const uint64_t qReserved = fC.FileMapReadVar64LE())
      XC("Reserved values 8 to 9 must not be set!", "Data", qReserved);
    if(const uint32_t ulReserved = fC.FileMapReadVar32LE())
      XC("Reserved value 10 must not be set!", "Data", ulReserved);
    // Get pixel format size
    const unsigned int uiPFSize = fC.FileMapReadVar32LE(),
      uiPFFlags = fC.FileMapReadVar32LE();
    // Check flags and show error if incorrect flags
    if(uiPFFlags & ~DDPF_MASK)
      XC("Unsupported pixel format flags!",
         "Flags", uiPFFlags, "Mask", DDPF_MASK);
    // Carry on reading flags
    const unsigned int
      uiPFFourCC = fC.FileMapReadVar32LE(),
      uiPFRGBBitCount = fC.FileMapReadVar32LE(),
      uiPFRBitMask = fC.FileMapReadVar32LE(),
      uiPFGBitMask = fC.FileMapReadVar32LE(),
      uiPFBBitMask = fC.FileMapReadVar32LE(),
      uiPFABitMask = fC.FileMapReadVar32LE();
    // To stop compiler whining but we needed to move the file pointer on
    UNUSED_VARIABLE(uiPFSize);       UNUSED_VARIABLE(uiPFRGBBitCount);
    UNUSED_VARIABLE(uiPFRBitMask);   UNUSED_VARIABLE(uiPFGBitMask);
    UNUSED_VARIABLE(uiPFBBitMask);   UNUSED_VARIABLE(uiPFABitMask);
    // If using compressed texture?
    if(uiPFFlags & DDPF_FOURCC)
    { // Check pixel format, only compressed texture format supported because
      // OpenGL does not support ARGB/XRGB and the guest should use something
      // more simple like BMP/TGA/PNG if using RGB(A) textures.
      switch(uiPFFourCC)
      { case FOURCC_DXT1: ifD.SetPixelType(0x83F1); break;
        case FOURCC_DXT3: ifD.SetPixelType(0x83F2); break;
        case FOURCC_DXT5: ifD.SetPixelType(0x83F3); break;
        // Unknown compression
        default: XC("Only DXT1, DXT3 or DXT5 FourCC format supported!",
          "Type", uiPFFourCC);
      } // All formats are 32-bpp and compressed
      ifD.SetBitsAndBytesPerPixel(BD_RGBA);
      ifD.SetCompressed();
    } // Get primary capabilities and if set?
    if(const unsigned int uiCaps1 = fC.FileMapReadVar32LE())
    { // Throw if not in range
      if(uiCaps1 & ~DDSCAPS_MASK)
        XC("Unsupported primary flags!",
           "Flags", uiCaps1, "Mask", DDSCAPS_MASK);
    } // We should have some flags
    else XC("Primary flags not specified!");
    // Were secondary capabilities set?
    if(const unsigned int uiCaps2 = fC.FileMapReadVar32LE())
    { // Show error if not in range
      if(uiCaps2 & ~DDSCAPS2_MASK)
        XC("Invalid secondary flags!",
           "Flags", uiCaps2, "Mask", DDSCAPS2_MASK);
      // None of them implemented regardless
      XC("Unimplemented secondary flags!", "Flags", uiCaps2);
    } // Read rest of values
    if(const unsigned int uiCaps3 = fC.FileMapReadVar32LE())
      XC("Unimplemented tertiary flags!", "Flags", uiCaps3);
    if(const unsigned int uiCaps4 = fC.FileMapReadVar32LE())
      XC("Unimplemented quaternary flags!", "Flags", uiCaps4);
    if(const unsigned int uiReserved2 = fC.FileMapReadVar32LE())
      XC("Reserved flags must not be set!", "Data", uiReserved2);
    // Alocate slots as mipmaps
    ifD.reserve(uiMipMapCount);
    // DDS's are reversed
    ifD.SetReversed();
    // DXT[n]? Compressed bitmaps need a special calculation
    if(ifD.IsCompressed()) for(unsigned int
      // Pre-initialised variables
      uiBitDiv    = (ifD.GetPixelType() == 0x83F1 ? 8 : 16),
      uiMipIndex  = 0,
      uiMipWidth  = ifD.DimGetWidth(),
      uiMipHeight = ifD.DimGetHeight(),
      uiMipBPP    = ifD.GetBytesPerPixel(),
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
      ifD.AddSlot(aData, uiMipWidth, uiMipHeight);
    } // Uncompressed image
    else for(unsigned int
      // Pre-initialised variables
      uiMipIndex = 0,                              // Mipmap index
      uiMipWidth = ifD.DimGetWidth(),              // Mipmap width
      uiMipHeight = ifD.DimGetHeight(),            // Mipmap height
      uiMipBPP = ifD.GetBytesPerPixel(),           // Mipmap bytes-per-pixel
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
      ifD.AddSlot(mData, uiMipWidth, uiMipHeight);
    } // Succeeded
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
class CodecGIF final :                 // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* -- Allocate memory ---------------------------------------------------- */
  static void *GIFCreate(int iW, int iH)
    { return MemAlloc<void>(iW * iH * BY_RGBA); }
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
      ifD.reserve(gaData.frame_count);
      // Set members
      ifD.SetBitsAndBytesPerPixel(BD_RGBA);
      ifD.SetPixelType(GL_RGBA);
      ifD.SetReversed();
      ifD.DimSet(gaData.width, gaData.height);
      // Decode the frames
      for(unsigned int uiIndex = 0; uiIndex < gaData.frame_count; ++uiIndex)
      { // Decode the frame and throw exception if failed
        const gif_result grCode = gif_decode_frame(&gaData, uiIndex);
        if(grCode != GIF_OK) XC("GIF decode failed!",
          "Frame", uiIndex, "Total", gaData.frame_count, "Code", grCode);
        // Image size. Not using cached width and height just incase it
        // might change inside the .gif
        const size_t stSize =
          gaData.width * gaData.height * ifD.GetBytesPerPixel();
        // Since libnsgif has conveniently expanded the gif into RGBA we can
        // just go ahead and copy it all into our mem buffer.
        Memory mData{ stSize, gaData.frame_image };
        ifD.AddSlot(mData, gaData.width, gaData.height);
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
struct CodecPNG final :                // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* -- PNG callbacks ------------------------------------------------------ */
  static void PngError[[noreturn]](png_structp, png_const_charp pccString)
    { throw runtime_error{ pccString }; }
  static void PngWarning(png_structp, png_const_charp pccString)
    { cLog->LogWarningExSafe("Image PNG warning: $.", pccString); }
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
      nullptr,                             // lang
      nullptr                              // lang_key
    };
    // Set the header
    png_set_text(psData, piData, &ptData, 1);
  }
  /* ----------------------------------------------------------------------- */
  static bool Save(const FStream &fC, const ImageData &ifD,
                   const ImageSlot &bData)
  { // Check parameters
    if(ifD.GetBitsPerPixel() != BD_RGB && ifD.GetBitsPerPixel() != BD_RGBA)
      XC("Image is not 24 or 32bpp!", "BitsPerPixel", ifD.GetBitsPerPixel());
    if(bData.DimIsNotSet())
      XC("Dimensions are invalid!",
         "Width",  bData.DimGetWidth(),
         "Height", bData.DimGetHeight());
    if(bData.Empty()) XC("No image data!", "Size", bData.Size());
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
      png_set_IHDR(psData, piData, bData.DimGetWidth(),
        bData.DimGetHeight(), 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
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
      vector<png_bytep> vData{ bData.DimGetHeight() };
      // Calculate size of a uiRow
      const unsigned int uiStride =
        bData.DimGetWidth() * (ifD.GetBitsPerPixel() / 8);
      // For each scanline
      // Set the pointer to the data pointer + i times the uiRow stride.
      // Notice that the uiRow order is reversed with q.
      // This is how at least OpenGL expects it,
      // and how many other image loaders present the data.
      for(unsigned int uiI = 0; uiI < bData.DimGetHeight(); ++uiI)
        vData[uiI] = bData.Read<png_byte>
          ((bData.DimGetHeight()-uiI-1)*uiStride);
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
      } // Get and check bits PER CHANNEL (may change)
      unsigned int uiBitsPC = png_get_bit_depth(psData, piData);
      switch(uiBitsPC)
      { // 1, 2 and 4bpc need expanding
        case 1: case 2: case 4:
          png_set_expand_gray_1_2_4_to_8(psData); uiBitsPC = 8; break;
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
      ifD.SetBitsAndBytesPerPixelCast(uiBitsPC * uiChannels);
      // Set colour mode
      switch(ifD.GetBitsPerPixel())
      { // 32-bpp
        case BD_RGBA: ifD.SetPixelType(GL_RGBA); break;
        // 24-bpp
        case BD_RGB: ifD.SetPixelType(GL_RGB); break;
        // 16-bpp (Gray + Alpha)
        case BD_GRAYALPHA: ifD.SetPixelType(GL_RG); break;
        // 8-bits-per-pixel (Gray)
        case BD_GRAY: ifD.SetPixelType(GL_RED); break;
        // Unsupported bit-depth
        default: XC("Unsupported bit-depth!",
                    "BitsPerPixel", ifD.GetBitsPerPixel());
      } // Get image dimensions
      ifD.DimSet(png_get_image_width(psData, piData),
                        png_get_image_height(psData, piData));
      // Initialise memory
      Memory aData(ifD.TotalPixels() * uiBitsPC * uiChannels / 8);
      // Create vector array to hold scanline pointers and size it
      vector<png_bytep> vData{ ifD.DimGetHeight<size_t>() };
      // For each scanline
      // Set the pointer to the data pointer + i times the uiRow stride.
      // Notice that the uiRow order is reversed with q.
      // This is how at least OpenGL expects it,
      // and how many other image loaders present the data.
      for(unsigned int uiWidth = ifD.DimGetWidth<unsigned int>(),
                       uiHeight = ifD.DimGetHeight<unsigned int>(),
                       uiStride = uiWidth * uiBitsPC * uiChannels / 8,
                       uiRow = 0;
                       uiRow < uiHeight;
                     ++uiRow)
        vData[uiRow] = aData.Read<png_byte>
          ((uiHeight - uiRow - 1) * uiStride, uiStride);
      // Read image and clean up
      png_read_image(psData, vData.data());
      png_read_end(psData, piData);
      png_destroy_read_struct(&psData, &piData, nullptr);
      // Success, add the image data to list
      ifD.AddSlot(aData);
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
class CodecJPG final :                 // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* -- JPEG callbacks ----------------------------------------------------- */
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
  /* --------------------------------------------------------------- */ public:
  static bool Save(const FStream &fC, const ImageData &ifD,
                   const ImageSlot &bData)
  { // Only support 24-bit per pixel images
    if(ifD.GetBitsPerPixel() != BD_RGB)
      XC("Only RGB supported!", "BitsPerPixel", ifD.GetBitsPerPixel());
    // Check for valid dimensions
    if(bData.DimIsNotSet())
      XC("Dimensions are invalid!",
         "Width",  bData.DimGetWidth(), "Height", bData.DimGetHeight());
    // Have image data?
    if(bData.Empty()) XC("No image data!", "Size", bData.Size());
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
      ciData.image_width = bData.DimGetWidth();
      ciData.image_height = bData.DimGetHeight();
      ciData.input_components = ifD.GetBitsPerPixel() / 8;
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
        JSAMPROW rPtr = reinterpret_cast<JSAMPROW>
          (bData.Read((ciData.image_height-1-ciData.next_scanline)*
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
        case 1: ifD.SetPixelType(GL_RED); break;
        // (3 bytes) 24-bpp of course is handled natively.
        case 3: ifD.SetPixelType(GL_RGB); break;
        // (4 bytes) 32-bpp of course is handled natively.
        case 4: ifD.SetPixelType(GL_RGBA); break;
        // Anything else, throw back to proper exception handler
        default: XC("Component count unsupported!",
                     "OutputComponents", ciData.output_components);
      } // Set dimensions and pixel bit depth
      ifD.DimSet(ciData.output_width, ciData.output_height);
      ifD.SetBytesAndBitsPerPixelCast(ciData.output_components);
      // Jpegs are reversed
      ifD.SetReversed();
      // Create space for decompressed image
      Memory aData{ ifD.TotalPixels() * ifD.GetBytesPerPixel() };
      // Get row stride (or number of bytes in a scanline of image data)
      const size_t stRowStride =
        ifD.DimGetWidth() * ifD.GetBytesPerPixel();
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
      ifD.AddSlot(aData);
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
struct CodecTGA final :                // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private ImageFmt                     // Image format helper class
{ /* -- TGA header is 18 bytes long ---------------------------------------- */
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
  { // Check valid dimensions
    if(sData.DimIsNotSet())
      XC("Invalid dimensions!", "Width",  sData.DimGetWidth(),
                                "Height", sData.DimGetHeight());
    // Check bits-per-pixel
    switch(ifD.GetBitsPerPixel())
    { // Only 24bpp and 32bpp images can be saved right now
      case BD_RGB: case BD_RGBA: break;
      // Error if anything else
      default: XC("Only RGB(A) supported!",
                  "BitsPerPixel", ifD.GetBitsPerPixel());
    } // Get input data and check size
    const DataConst &dcIn = sData;
    if(dcIn.Empty()) XC("No image data!", "Size", dcIn.Size());
    // Duplicate memory data because we need to convert it to BGR
    Memory mbOut{ dcIn.Size(), dcIn.Ptr<void>() };
    // Swap pixels
    if(const int iResult = ImageSwapPixels(mbOut.Ptr<char>(),
      mbOut.Size(), ifD.GetBytesPerPixel(), 0, 2))
        XC("Re-order to BGR(A) failed!",
           "Code",      iResult,      "Address", mbOut.Ptr<void>(),
           "Length",    mbOut.Size(), "BPP",     ifD.GetBytesPerPixel(),
           "PixelType", ifD.GetPixelType());
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
      sData.DimGetWidth<uint16_t>());
    mbHdr.WriteIntLE<uint16_t>(HL_U16LE_HEIGHT,
      sData.DimGetHeight<uint16_t>());
    mbHdr.WriteInt<uint8_t>(HL_U08LE_BITS_PER_PIXEL,
      ifD.GetBitsPerPixel<uint8_t>());
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
    const size_t stImageSize = (ifD.GetBitsPerPixel() < 8) ?
        (ifD.TotalPixels() / 8) : (ifD.TotalPixels() * ifD.GetBytesPerPixel());
    // We can just copy the whole block and make sure we read it all
    Memory aData{ fC.FileMapReadBlock(stImageSize) };
    if(aData.Size() != stImageSize)
      XC("Read error!", "Expected", stImageSize, "Actual", aData.Size());
    // Add image to list
    ifD.AddSlot(aData);
    // OK!
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodeRLE(FileMap &fC, ImageData &ifD)
  { // Calculate bytes per pixel
    const size_t stBPP = ifD.GetBitsPerPixel() / 8;
    // Initialise output memory block for uncompressed data
    Memory aData{ ifD.TotalPixels() * stBPP };
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
    ifD.AddSlot(aData);
    // Decode success!
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodePalette(const ColourMapType uiColourMapType,
    const BitsPerPixel uiColourMapDepth, const unsigned int uiColourMapLength,
    FileMap &fC, ImageData &ifD)
  { // Colour-map type must be supported
    if(uiColourMapType == CMT_NONE)
      XC("Paletted TGA must support colour map!",
         "ColourMapType", uiColourMapType);
    // Only 24-bit palette entries supported
    if(uiColourMapDepth != BPP_RGB888)
      XC("Paletted TGA must only have 24bit palette entries!",
         "ColourMapDepth", uiColourMapDepth);
    // Read the indexes
    const size_t stPalBytesPerPix = uiColourMapDepth / 8,
                 stPaletteSize = uiColourMapLength * stPalBytesPerPix;
    Memory mPalette{ fC.FileMapReadBlock(stPaletteSize) };
    if(mPalette.Size() != stPaletteSize)
      XC("Read palette error!",
         "Expected", stPaletteSize, "Actual", mPalette.Size());
    // Read the pixel data
    const size_t stImageSize = ifD.TotalPixels();
    Memory mPixels{ fC.FileMapReadBlock(stImageSize) };
    if(mPixels.Size() != stImageSize)
      XC("Read pixels error!",
         "Expected", stImageSize, "Actual", mPixels.Size());
    // Set that we are including a palette and set as 8-bits per pixel
    ifD.SetPalette();
    ifD.SetBitsAndBytesPerPixel(BD_GRAY);
    ifD.SetPixelType(GL_RED);
    // Add image and palette to list. Height is used as pixel type.
    ifD.AddSlot(mPixels);
    ifD.AddSlot(mPalette, uiColourMapLength,
      static_cast<unsigned int>(stPalBytesPerPix));
    // Done
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodeGray(const ColourMapType uiColourMapType,
    FileMap &fC, ImageData &ifD)
  { // Colour-map type must be supported
    if(uiColourMapType == CMT_PALETTE)
      XC("Gray TGA must not support colour map!",
         "ColourMapType", uiColourMapType);
    // Update output, we will be converting to indexed
    ifD.SetBitsAndBytesPerPixel(BD_GRAY);
    ifD.SetPixelType(GL_RED);
    // Pass to generic copy function
    return CopyPixels(fC, ifD);
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodeRLEPalette(const ColourMapType uiColourMapType,
    const BitsPerPixel uiColourMapDepth, const unsigned int uiColourMapLength,
    FileMap &fC, ImageData &ifD)
  { // Colour-map type must be supported
    if(uiColourMapType == CMT_NONE)
      XC("Paletted RLE TGA must support colour map!",
        "ColourMapType", uiColourMapType);
    // Only 24-bit palette entries supported
    if(uiColourMapDepth != BPP_RGB888)
      XC("Paletted RLE TGA must only have 24bit palette entries!",
        "ColourMapDepth", uiColourMapDepth);
    // Read the palette pixel values
    const size_t stPalBytesPerPix = uiColourMapDepth / 8,
                 stPaletteSize = uiColourMapLength * stPalBytesPerPix;
    Memory mPalette{ fC.FileMapReadBlock(stPaletteSize) };
    if(mPalette.Size() != stPaletteSize)
      XC("Read palette error!",
         "Expected", stPaletteSize, "Actual", mPalette.Size());
    // Decode the RLE
    if(!DecodeRLE(fC, ifD)) return false;
    // Set that we have a palette, the bit/byte depth and pixel type
    ifD.SetPalette();
    ifD.SetBitsAndBytesPerPixel(BD_GRAY);
    ifD.SetPixelType(GL_RED);
    // Add palette to list. Height is used as pixel type.
    ifD.AddSlot(mPalette, uiColourMapLength,
      static_cast<unsigned int>(stPalBytesPerPix));
    // OK!
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodeRLEGray(const ColourMapType uiColourMapType, FileMap &fC,
    ImageData &ifD)
  { // Colour-map type must be supported
    if(uiColourMapType == CMT_PALETTE)
      XC("Gray RLE TGA must not support colour map!",
         "ColourMapType", uiColourMapType);
    // Decode the RLE
    if(!DecodeRLE(fC, ifD)) return false;
    // Update bit depth and pixel type
    ifD.SetBitsAndBytesPerPixel(BD_GRAY);
    ifD.SetPixelType(GL_RED);
    // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodeByte(const DataTypeCode uiDataTypeCode,
    const ColourMapType uiColourMapType, const BitsPerPixel uiColourMapDepth,
    const unsigned int uiColourMapLength, FileMap &fC, ImageData &ifD)
  { // Only 'index' or 'gray' data type is supported
    switch(uiDataTypeCode)
    { // Paletted?
      case DTC_INDEX:
        return DecodePalette(uiColourMapType, uiColourMapDepth,
          uiColourMapLength, fC, ifD);
      // Grayscale?
      case DTC_GRAY:
        return DecodeGray(uiColourMapType, fC, ifD);
      // Paletted RLE?
      case DTC_RLE_INDEX:
        return DecodeRLEPalette(uiColourMapType, uiColourMapDepth,
          uiColourMapLength, fC, ifD);
      // Grayscale RLE?
      case DTC_RLE_GRAY:
        return DecodeRLEGray(uiColourMapType, fC, ifD);
      // Expandable targa is not supported!
      default: XC("Indexed TGA has unsupported data type!",
                  "DataTypeCode", uiDataTypeCode);
    } // Should not get here
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodeRGBA(const DataTypeCode uiDataTypeCode,
    FileMap &fC, ImageData &ifD)
  { // Update output type to BGRA with four bytes per pixel
    ifD.SetPixelType(GL_BGRA);
    ifD.SetBitsAndBytesPerPixel(BD_RGBA);
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
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodeRGB(const DataTypeCode uiDataTypeCode,
    FileMap &fC, ImageData &ifD)
  { // Update output type to BGR with three bytes per pixel
    ifD.SetPixelType(GL_BGR);
    ifD.SetBytesAndBitsPerPixel(BY_RGB);
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
  }
  /* ----------------------------------------------------------------------- */
  static bool DecodeBinary(const DataTypeCode uiDataTypeCode,
      const ColourMapType uiColourMapType, FileMap &fC, ImageData &ifD)
  { // Only the 'gray' data type code can be used here
    if(uiDataTypeCode != DTC_GRAY)
      XC("Monochrome TGA must be data type 3!", "Code", uiDataTypeCode);
    // Colour-map type is not supported
    if(uiColourMapType == CMT_PALETTE)
      XC("Monochrome TGA does not support colour map!",
         "Code", uiColourMapType);
    // Update the pixel type to zero. This format can ONLY be used with the
    // BitMask colission system and cannot be used in OpenGL as there is no
    // shader support that will translate it yet.
    ifD.SetPixelType(0);
    // Pass to generic copy function
    return CopyPixels(fC, ifD);
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
    if(const unsigned int uiColourMapOrigin = fC.FileMapReadVar16LE())
      XC("TGA colour map origin unsupported!",
         "ColourMapOrigin", uiColourMapOrigin);
    // Get colour map length and pixel depth
    const unsigned int uiColourMapLength = fC.FileMapReadVar16LE();
    const BitsPerPixel uiColourMapDepth =
      static_cast<BitsPerPixel>(fC.FileMapReadVar<uint8_t>());
    // Get origins, but origins not supported
    if(const Coordinates<> coOrigin{ fC.FileMapReadVar16LE(),
                                     fC.FileMapReadVar16LE() })
      XC("TGA origins unsupported!",
         "OriginX", coOrigin.CoordGetX(), "OriginY", coOrigin.CoordGetY());
    // Get widths and check that they're valid
    if(!ifD.SetDimSafe(fC.FileMapReadVar16LE(), fC.FileMapReadVar16LE()))
      XC("TGA dimensions invalid!",
         "Width",  ifD.DimGetWidth(),
         "Height", ifD.DimGetHeight());
    // Get bit depth
    ifD.SetBitsAndBytesPerPixelCast(fC.FileMapReadVar<uint8_t>());
    // Get image descriptor bits
    const ImageDescriptorFlags idFlags =
      static_cast<ImageDescriptorFlags>(fC.FileMapReadVar<uint8_t>());
    ifD.SetReversed(!!(idFlags & IDF_FLIPPED));
    // Interleaving or attribute bits are not supported
    if(idFlags & (IDF_NUM_ATTR_BITS|IDF_IL_TWO_WAY|IDF_IL_FOUR_WAY))
      XC("TGA has unsupported image descriptor flags!", "Flags", idFlags);
    // Check bit depth
    switch(ifD.GetBitsPerPixel())
    { // Monochrome : 1-bit per pixel?
      case BD_BINARY:
        return DecodeBinary(uiDataTypeCode, uiColourMapType, fC, ifD);
      // Paletted or gray : 8-bits per pixel?
      case BD_GRAY:
        return DecodeByte(uiDataTypeCode, uiColourMapType, uiColourMapDepth,
          uiColourMapLength, fC, ifD);
      // 16.7 million colours : 24-bits per pixel
      case BD_RGB:
        return DecodeRGB(uiDataTypeCode, fC, ifD);
      // 16.7 million colours with alpha channel : 32-bits per pixel
      case BD_RGBA:
        return DecodeRGBA(uiDataTypeCode, fC, ifD);
      // Unknown or invalid. Throw error as the bit-depth is not recognised
      default: XC("TGA bit-depth not supported!",
                  "BitsPerPixel", ifD.GetBitsPerPixel());
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
};/* ----------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
