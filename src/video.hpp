/* == VIDEO.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This file handles streaming from .OGV files and playing to OpenAL.  ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfVideo {                    // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace Lib::Theora;           // Using Theora library functions
using namespace IfStream;              // Using source namespace
using namespace IfFbo;                 // Using fbo namespace
/* -- Video collector class for collector data and custom variables -------- */
BEGIN_ASYNCCOLLECTOREX(Videos, Video, CLHelperSafe,
/* -- Public variables ----------------------------------------------------- */
typedef IfIdent::IdList<TH_CS_NSPACES> CSStrings;
const CSStrings    csStrings;          /* Colour space strings list         */\
typedef IfIdent::IdList<TH_PF_NFORMATS> PFStrings;
const PFStrings    pfStrings;          /* Pixel format strings list         */\
SafeLong           lBufferSize;,,      /* Default buffer size               */\
/* -- Derived classes ------------------------------------------------------ */
private LuaEvtMaster<Video, LuaEvtTypeParam<Video>>); // Lua event
/* ------------------------------------------------------------------------- */
BUILD_FLAGS(Video,
  /* ----------------------------------------------------------------------- */
  // No flags set?                     Have a theora stream?
  FL_NONE                {0x00000000}, FL_THEORA              {0x00000001},
  // Have a vorbis stream?             vorbis_synthesis_init was called?
  FL_VORBIS              {0x00000002}, FL_VDINIT              {0x00000004},
  // vorbis_block_init was called?     ogg_sync_init called?
  FL_VBINIT              {0x00000008}, FL_OSINIT              {0x00000010},
  // th_comment_init called?           th_info_init called?
  FL_TCINIT              {0x00000020}, FL_TIINIT              {0x00000040},
  // vorbis_info_init called?          vorbis_comment_init called?
  FL_VCINIT              {0x00000080}, FL_VIINIT              {0x00000100},
  // Video output initialised?         Video is keyed?
  FL_GLINIT              {0x00000200}, FL_KEYED               {0x08000000},
  // Filtering is enabled?             Hard stopped (reopen on play)?
  FL_FILTER              {0x10000000}, FL_STOP                {0x20000000},
  // Video is playing?
  FL_PLAY                {0x40000000}
);/* ======================================================================= */
BEGIN_MEMBERCLASS(Videos, Video, ICHelperSafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoader<Video>,           // Asynchronous laoding of videos
  public LuaEvtSlave<Video>,           // Lua asynchronous events
  public VideoFlags,                   // Video settings flags
  private ClockInterval<CoreClock>,    // Frame playback timing helper
  public Lockable                      // Lua garbage collector instruction
{ /* -- Typedefs ----------------------------------------------------------- */
  struct Frame                         // Frame data
  { /* --------------------------------------------------------------------- */
    bool             bDraw;            // Draw this frame?
    /* --------------------------------------------------------------------- */
    struct YCbCr                       // Y/Cb/Cr plane data
    { /* ------------------------------------------------------------------- */
      const size_t   stI;              // Unique index
      th_img_plane   tipP;             // Plane data
      /* ------------------------------------------------------------------- */
      void Reset(void) {
        tipP.width = tipP.height = tipP.stride = 0;
        tipP.data = nullptr;
      }
      /* -- Constructor that initialises id -------------------------------- */
      explicit YCbCr(const size_t stNIndex) :
        /* -- Initialisers ------------------------------------------------- */
        stI(stNIndex),                 // Set unique id
        tipP{0, 0, 0, nullptr}         // Initialise frame data
        /* -- No code ------------------------------------------------------ */
        { }
    };/* ------------------------------------------------------------------- */
    array<YCbCr,3> aP;                 // The planes (Y, Cb and Cr);
    /* --------------------------------------------------------------------- */
    void Reset(void) { bDraw = false; for(auto &aI : aP) aI.Reset(); }
    /* -- Constructor that initialises frame data -------------------------- */
    Frame(void) :
      /* -- Initialisers --------------------------------------------------- */
      bDraw(false),                    // Set frame not ready for drawing
      aP{ YCbCr{0},YCbCr{1},YCbCr{2} } // Initialise Y/Cb/Cr frame data
      /* -- No code -------------------------------------------------------- */
      { }
  };/* --------------------------------------------------------------------- */
  enum Unblock { UB_BLOCK, UB_DATA, UB_REINIT, UB_PLAY, UB_STOP, UB_PAUSE };
  /* --------------------------------------------------------------- */ public:
  enum Event { VE_PLAY, VE_LOOP, VE_STOP, VE_PAUSE, VE_FINISH };
  /* -- Concurrency -------------------------------------------------------- */
  Thread           tThread;            // Video Decoding Thread
  condition_variable cvBuffer;         // Re-buffer unblocker
  mutex            mBuffer,            // mutex for rebuffering CV
                   mUpload;            // mutex for uploading data
  Unblock          ubReason;           // Unlock condition variable
  SafeSizeT        stLoop;             // Loops count
  SafeBool         bPause;             // Only pause the stream?
  /* -- Ogg ---------------------------------------------------------------- */
  ogg_sync_state   oSS;                // Ogg sync state
  ogg_page         oPG;                // Ogg page
  ogg_packet       oPK;                // Ogg packet
  long             lIOBuf;             // IO buffer size
  FileMap          fmFile;             // File map for reading file
  /* -- Theora ------------------------------------------------------------- */
  ogg_stream_state tSS;                // Ogg (Theora) stream states
  th_info          tIN;                // Theora info struct
  th_comment       tCO;                // Theora comment struct
  th_setup_info   *tSI;                // Theora setup info
  th_dec_ctx      *tDC;                // Theora decoder context
  th_ycbcr_buffer  tYB;                // Theora colour buffer
  ogg_int64_t      iVideoGranulePos;   // Video granule position
  SafeDouble       fdVideoTime;        // Video time index
  double           fdFPS;              // Video fps
  SafeUInt         uiVideoFrames,      // Frames rendered
                   uiVideoFramesLost;  // Frames skipped
  array<Frame,2>   fData;              // Frame data
  size_t           stFActive,          // Currently active frame
                   stFNext,            // Next frame to process
                   stFWaiting;         // Frames waiting to be processed
  SafeSizeT        stFFree;            // Frames free to be processed
  /* -- Vorbis ------------------------------------------------------------- */
  ogg_stream_state vSS;                // Ogg (Vorbis) stream states
  vorbis_info      vIN;                // Vorbis decoder info
  vorbis_comment   vCO;                // Vorbis comment block
  vorbis_dsp_state vDS;                // Vorbis DSP state
  vorbis_block     vBL;                // Vorbis decoder block
  Memory           mbAudio;            // Audio decoded data
  SafeDouble       fdAudioTime;        // Audio time index
  ALdouble         fdAudioBuffer;      // Audio buffered
  ALfloat          fAudioVolume;       // Audio volume
  /* -- OpenGL ------------------------------------------------------------- */
  Fbo              fboC;               // Fbo for video
  FboItem          fiYUV;              // Blit data for actual YUV components
  array<GLuint,3>  uiYUV;              // Texture id's for YUV components
  Shader          *shProgram;          // Shader program to use
  /* -- OpenAL ------------------------------------------------------------- */
  Source          *sCptr;              // Source class
  ALenum           eFormat;            // Internal format
  /* == Buffer more data for OGG decoder ========================== */ private:
  bool IOBuffer(void)
  { // If we're not EOF? Get some memory from ogg which can fail and if it
    // didn't? Read data info buffer and if we read data? Tell ogg how much we
    // wrote.
    if(fmFile.FileMapIsNotEOF())
      if(char*const cpBuffer = ogg_sync_buffer(&oSS, lIOBuf))
        if(const size_t stCount =
          fmFile.FileMapReadToAddr(cpBuffer, static_cast<size_t>(lIOBuf)))
            return ogg_sync_wrote(&oSS, static_cast<long>(stCount)) == -1;
    // EOF or buffer invalid so return error
    return true;
  }
  /* -- Clear textures array ----------------------------------------------- */
  void ClearTextures(void) { uiYUV.fill(0); }
  /* -- TimerCatchup a frame but don't render it --------------------------- */
  bool TimerCatchup(void)
  { // Wait until uploading is done
    const LockGuard lgWaitForUpload{ mUpload };
    // Reset buffer statistics. Don't reset buffer id though
    stFFree = fData.size();
    stFWaiting = 0;
    // Create a counter to hold skipped frames. Since the counter in the class
    // is atomic, we'll just make sure we modify it once.
    unsigned int uiSkipped = 0;
    // For each frame...
    for(Frame &fSlot : fData)
    { // Ignore if not set to draw
      if(!fSlot.bDraw) continue;
      // Reset the frame data
      fSlot.Reset();
      // Increment skipped frames counter
      ++uiSkipped;
    } // Add to frames lost
    uiVideoFramesLost += uiSkipped;
    // Read and decode a new packet and return if succeeded
    return ogg_stream_packetout(&tSS, &oPK) > 0
        && th_decode_packetin(tDC, &oPK, &iVideoGranulePos) >= 0;
  }
  /* -- Decode data and assign it ------------------------------------------ */
  void AssignDecodedData(void)
  { // Wait until uploading is done
    const LockGuard lgWaitForUpload{ mUpload };
    // No buffers free? Force this next buffer to be free
    if(!stFFree) { ++stFFree; --stFWaiting; }
    // Get next frame to draw
    Frame &fI = fData[stFNext];
    // If decoding the frame failed?
    if(th_decode_ycbcr_out(tDC, tYB))
    { // Skip the frame
      fI.bDraw = false;
      // We lost this frame
      ++uiVideoFramesLost;
    } // Decoding succeeded?
    else
    { // Set pointers to planes and we will be drawing this frame
      for(auto &aP : fI.aP) aP.tipP = tYB[aP.stI];
      fI.bDraw = true;
      // We processed this frame
      ++uiVideoFrames;
    } // Buffer filled
    stFNext = (stFNext + 1) % fData.size();
    ++stFWaiting;
    --stFFree;
  }
  /* -- Update video position ---------------------------------------------- */
  void UpdateVideoPosition(void)
    { fdVideoTime = th_granule_time(tDC, iVideoGranulePos); }
  /* -- Manage video decoding thread --------------------------------------- */
  int ManageVideo(void)
  { // Audio and/or video availability flags
    BUILD_FLAGS(Avail, AF_NONE{0x0},  // Video or audio not ready?
                      AF_VIDEO{0x1},  // Enough video buffered?
                      AF_AUDIO{0x2}); // Enough audio buffered?
    // Video and audio was rendered successfully?
    AvailFlags afStatus{ AF_NONE };
    // Try to read as much audio as possible
    while(FlagIsSet(FL_VORBIS) && sCptr)
    { // OpenAL buffer
      ALuint uiBuffer;
      // Get number of buffers queued
      for(ALsizei stP = sCptr->GetBuffersProcessed(); stP; --stP)
      { // Unqueue a buffer and break if failed
        uiBuffer = sCptr->UnQueueBuffer();
        if(cOal->HaveError()) continue;
        // Remove buffer time
        fdAudioBuffer -=
          Maximum(cOal->GetBufferInt<ALdouble>(uiBuffer, AL_SIZE) /
            vIN.rate, 0);
        // Delete the buffer that was returned continue if successful
        ALL(cOal->DeleteBuffer(uiBuffer),
          "Video failed to delete unqueued buffer $ in '$'!",
             uiBuffer, IdentGet());
      } // No need to process more if we have more than 1 sec of audio.
      if(fdAudioBuffer >= 1.0) break;
      // If audio time is ahead of video by two seconds then we don't need any
      // more data.
      if(FlagIsSet(FL_THEORA) && fdAudioTime > fdVideoTime + 1) break;
      // Get PCM data stored as float
      ALfloat **fpPCM;
      const size_t stFrames =
        static_cast<size_t>(vorbis_synthesis_pcmout(&vDS, &fpPCM));
      // If frames are available, but we're way behind the video?
      if(stFrames > 0)
      { // Get channels as size_t
        const size_t stChannels = static_cast<size_t>(vIN.channels);
        // Length of data
        size_t stFrameSize = sizeof(float) * stFrames * stChannels;
        // Just incase we need more memory
        mbAudio.ResizeUp(stFrameSize);
        // Send the PCM processing depending on if audio device can play
        // as floating point pcm or not. If we have 32-bit float support?
        // Convert data to native PCM float 32-bit audio
        if(cOal->Have32FPPB())
          Stream::VorbisFramesToF32PCM(fpPCM, stFrames, stChannels,
            mbAudio.Ptr<ALfloat>());
        // Uploading as INTEGER 16-Bit PCM data
        else
        { // Convert data to native PCM integer 16-bit audio
          Stream::VorbisFramesToI16PCM(fpPCM, stFrames, stChannels,
            mbAudio.Ptr<ALshort>());
          // Now using half the buffer size (F32 is 4 bytes, I16 is 2)
          stFrameSize >>= 1;
        } // Generate a buffer for the pcm data and if succeeded?
        cOal->CreateBuffer(uiBuffer);
        ALenum alErr = cOal->GetError();
        if(alErr == AL_NO_ERROR)
        { // Buffer the data. Note that the size of mbAudio can be bigger than
          // the actual data so we have to pass in the calculated data size.
          cOal->BufferData(uiBuffer, eFormat, mbAudio.Ptr(),
            static_cast<ALsizei>(stFrameSize), static_cast<ALsizei>(vIN.rate));
          // If buffering succeeded?
          alErr = cOal->GetError();
          if(alErr == AL_NO_ERROR)
          { // Requeue the buffers and if succeeded?
            sCptr->QueueBuffer(uiBuffer);
            alErr = cOal->GetError();
            if(alErr == AL_NO_ERROR)
            { // Play the source
              sCptr->Play();
              // We ate everything so set audio time and add to buffer
              vorbis_synthesis_read(&vDS, static_cast<int>(stFrames));
              fdAudioTime = vorbis_granule_time(&vDS, vDS.granulepos);
              fdAudioBuffer += static_cast<ALdouble>(stFrameSize) / vIN.rate;
              // We got audio
              afStatus.FlagSet(AF_AUDIO);
            } // Queue failed? Jump to delete buffer failed
            else goto DeleteBuffer;
          } // Buffer data had failed?
          else
          { // Label to delete the buffer
            DeleteBuffer:
            // Log the error
            cLog->LogWarningExSafe("Video failed buffering attempt on '$' "
              "(B:$;F:$;A:$;S:$;R:$;AL:$<$$>)!",
                IdentGet(), uiBuffer, eFormat, mbAudio.Ptr(), stFrameSize,
                vIN.rate, cOal->GetALErr(alErr), hex, alErr);
            // Delete the buffers because of error
            ALL(cOal->DeleteBuffer(uiBuffer),
              "Video failed to delete buffer $ in '$' "
              "after failed data upload attempt!", uiBuffer, IdentGet());
          }
        }
      }
      // No audio available left in current packet? Try to feed another packet
      // to the Vorbis stream.
      else if(ogg_stream_packetout(&vSS, &oPK) <= 0) break;
      // We still have audio, synthesize a packet and reloop to try again
      else if(!vorbis_synthesis(&vBL, &oPK))
        vorbis_synthesis_blockin(&vDS, &vBL);
    } // Have theora stream and we've got audio buffered?
    if(FlagIsSet(FL_THEORA) &&
      ((FlagIsSet(FL_VORBIS) && fdAudioBuffer >= 1.0) ||
      FlagIsClear(FL_VORBIS)))
    { // If we haven't reached the time we expect to draw the next frame?
      if(CINoTrigger()) afStatus.FlagSet(AF_VIDEO);
      // theora is one in, one out...
      else if(ogg_stream_packetout(&tSS, &oPK) > 0)
      { // Decode the packet and if we get a positive result?
        switch(const int iR = th_decode_packetin(tDC, &oPK, &iVideoGranulePos))
        { // Success?
          case 0:
            // Try to decode the data packet
            AssignDecodedData();
            // We processed a video frame
            afStatus.FlagSet(AF_VIDEO);
            // Update position
            UpdateVideoPosition();
            // Set next frome time and fall through to break
            CIAccumulate();
            // Done
            break;
          // Duplicate frame? We don't need to decode anything
          case TH_DUPFRAME:
            // We processed a video frame
            afStatus.FlagSet(AF_VIDEO);
            // Update video time and increment frames counter
            ++uiVideoFrames;
            // Update position
            UpdateVideoPosition();
            // Set next frome time and fall through to break
            CIAccumulate();
            // Done
            break;
          // Bad packet? (ignore)
          case TH_EBADPACKET: break;
          // Anything else?
          default:
            // Just put warning in log
            cLog->LogWarningExSafe("Video '$' decode failure code $!",
              IdentGet(), iR);
            // Done
            break;
        }
      }
    }
    // If was saw a video frame wait a little bit. We put this here and not
    // in the parent main thread loop because we want to get audio as fast as
    // possible, but with the video stream, we can.
    if(afStatus.FlagIsSet(AF_VIDEO))
    { // If stream has audio, audio is buffered and video is behind audio by
      // one-tenth of a second?
      if(FlagIsSet(FL_VORBIS) && fdAudioTime >= 0.0 &&
        fdVideoTime < fdAudioTime - 0.1)
      { // Catch up and if succeeded? Update video position
        if(TimerCatchup()) UpdateVideoPosition();
        // Next frame can show immediately
        CISync();
      } // No audio or not drifting so we can wait for engine thread
      else
      { // Setup lock for condition variable
        UniqueLock uLock{ mBuffer };
        // Wait for main thread to say we should rebuffer or when this thread
        // is requested to terminate.
        cvBuffer.wait(uLock, [this]{
          return ubReason != UB_BLOCK || tThread.ThreadShouldExit(); });
        // Check reason for unblocking
        switch(ubReason)
        { // Re-initialising, stopping or pausing?
          case UB_REINIT: [[fallthrough]];
          case UB_STOP: [[fallthrough]];
          case UB_PAUSE: return 1;
          // Just for data reasons or for playing?
          case UB_DATA: [[fallthrough]];
          case UB_PLAY: [[fallthrough]];
          case UB_BLOCK: break;
        } // Reset unlock flag
        ubReason = UB_BLOCK;
      }
    } // Video frame and audio frame not ready?
    else if(afStatus.FlagIsClear(AF_AUDIO))
    { // We didn't read anything or we're at end-of-file!
      if(IOBuffer())
      { // Rewind video
        Rewind();
        // We should loop?
        if(stLoop > 0)
        { // Reduce loops if not infinity
          if(stLoop != string::npos) --stLoop;
          // Send looping event
          LuaEvtDispatch(VE_LOOP);
        } // We should pause? Pause playback and stop the thread
        else return 2;
      } // Read data until we get something useful
      while(ogg_sync_pageout(&oSS, &oPG) > 0)
      { // Find theora data
        if(FlagIsSet(FL_THEORA)) ogg_stream_pagein(&tSS, &oPG);
        // Find vorbis data
        if(FlagIsSet(FL_VORBIS)) ogg_stream_pagein(&vSS, &oPG);
      }
    } // Keep thread loop alive
    return 0;
  }
  /* -- Get/Set theora decoder control ------------------------------------- */
  template<typename AnyType>int SetParameter(const int iW, AnyType atV)
    { return th_decode_ctl(tDC, iW,
        reinterpret_cast<void*>(&atV), sizeof(atV)); }
  template<typename AnyType=int>AnyType GetParameter(const int iW) const
    { return static_cast<AnyType>(th_decode_ctl(tDC, iW, nullptr, 0)); }
  /* -- Initialise audio ouput support ------------------------------------- */
  void InitAudioStream(void)
  { // Don't need to initialise anything if there is no audio stream
    if(FlagIsClear(FL_VORBIS)) return;
    // Make sure rate is sane
    if(vIN.rate < 1 || vIN.rate > 192000)
    { // We'll treat this as a warning as we can just disable audio playback
      cLog->LogWarningExSafe(
        "Video '$' playback rate of $ not valid at this time.",
        IdentGet(), vIN.channels);
      // Disable playback
      FlagClear(FL_VORBIS);
      // Done
      return;
    } // Make sure channels are correct
    if(vIN.channels < 1 || vIN.channels > 2)
    { // We'll treat this as a warning as we can just disable audio playback
      cLog->LogWarningExSafe(
        "Video '$' playback channel count of $ not supported.",
        IdentGet(), vIN.channels);
      // Disable playback
      FlagClear(FL_VORBIS);
      // Done
      return;
    } // Initialise vorbis synthesis
    if(vorbis_synthesis_init(&vDS, &vIN))
      XC("Failed to initialise vorbis synthesis!", "Identifier", IdentGet());
    FlagSet(FL_VDINIT);
    // Initialise vorbis block
    if(vorbis_block_init(&vDS, &vBL))
      XC("Failed to initialise vorbis block!", "Identifier", IdentGet());
    FlagSet(FL_VBINIT);
    // Calculate memory required for buffering audio and verify the size since
    // we don't know if someone's sending us dodgy bitrate values
    const double fdMem =
      NearestPow2<double>(Maximum(vIN.bitrate_upper, vIN.bitrate_nominal) / 8);
    if(fdMem < 0 || fdMem > 1048576)
      XC("Calculated erroneous memory size for audio buffer!",
        "Identifier", IdentGet(),        "Amount",  fdMem,
        "Upper",      vIN.bitrate_upper, "Nominal", vIN.bitrate_nominal,
        "Lower",      vIN.bitrate_lower, "Window",  vIN.bitrate_window);
    // Allocate enough memory for audio buffer. We may not actually use all the
    // memory, but it is better than putting the alloc in the decoder tick
    mbAudio.Resize(static_cast<size_t>(fdMem));
    // Show what we allocated
    cLog->LogDebugExSafe("Video pre-allocated $ bytes for audio decoder.",
      mbAudio.Size());
  }
  /* -- Initialise video stream portion of file ---------------------------- */
  void InitVideoStream(void)
  { // Don't need to initialise anything if there is no video stream
    if(FlagIsClear(FL_THEORA)) return;
    // Decoder context not initialised?
    if(!tDC)
    { // Allocate a new one and throw error if not allocated
      tDC = th_decode_alloc(&tIN, tSI);
      if(!tDC)
        XC("Error creating theora decoder context!", "Identifier", IdentGet());
    } // We're done with the codec setup data if we have it.
    if(tSI) { th_setup_free(tSI); tSI = nullptr; }
    // Calculate FPS of video
    fdFPS = static_cast<double>(tIN.fps_numerator) /
            static_cast<double>(tIN.fps_denominator);
    // Sanity check FPS
    if(fdFPS<1 || fdFPS>200)
      XC("Ambiguous frame rate in video!",
        "Identifier", IdentGet(), "FPS", fdFPS);
    // Make sure GPU can support texture size
    if(static_cast<GLuint>(tIN.frame_width) > cOgl->MaxTexSize() ||
       static_cast<GLuint>(tIN.frame_height) > cOgl->MaxTexSize())
      XC("The currently active graphics device does not support a "
         "texture size that would fit the video dimensions!",
         "Identifier", IdentGet(),       "Width", tIN.frame_width,
         "Height",     tIN.frame_height, "Limit", cOgl->MaxTexSize());
    // Verify colour space
    switch(GetColourSpace())
    { // Valid colour spaces
      case TH_CS_UNSPECIFIED:          // No colour content?
      case TH_CS_ITU_REC_470M:         // NTSC content?
      case TH_CS_ITU_REC_470BG:        // PAL/SECAM content?
        break;
      // Invalid colour space
      default: XC("The specified colour space is unsupported!",
                  "Identifier", IdentGet(), "ColourSpace", GetColourSpace());
    } // Verify chroma subsampling type
    switch(GetPixelFormat())
    { // Valid chroma types
      case TH_PF_420: [[fallthrough]]; // YUV 4:2:0
      case TH_PF_422: [[fallthrough]]; // YUV 4:2:2
      case TH_PF_444: break;           // YUV 4:4:4
      // Invalid chroma type
      default: XC("Only 420, 422 or 444 pixel format is supported!",
                  "Identifier", IdentGet(), "PixelFormat", GetPixelFormat());
    } // Set buffer id's
    stFActive = stFWaiting = stFNext = 0;
    stFFree = fData.size();
    fdVideoTime = 0;
    // Update frame immediately
    CISetLimit(1.0 / fdFPS);
  }
  /* -- Initialise video display output support ---------------------------- */
  void InitDisplayOutput(void)
  { // Don't need to initialise anything if there is no video stream or the
    // OpenGL portion is already initialised.
    if(FlagIsSet(FL_GLINIT) || FlagIsClear(FL_THEORA)) return;
    // OpenGL output initialised
    FlagSet(FL_GLINIT);
    // Init fbo
    fboC.Init(IdentGet(), static_cast<GLsizei>(tIN.pic_width),
                          static_cast<GLsizei>(tIN.pic_height));
    fboC.CollectorRegister();
    fboC.SetOrtho(0, 0, 0, 0);
    fboC.LockSet();
    fboC.SetTransparency(true);
    fboC.SetTexCoord(0, 0, 1, 1);
    // Clear the fbo, initially transparent
    fboC.SetClearColour(0, 0, 0, 0);
    fboC.SetClear(false);
    // Only 2 triangles and 3 commands are needed so reserve the memory
    if(!fboC.Reserve(2, 3))
      cLog->LogWarningExSafe("Video failed to reserve memory for fbo lists.");
    // We must discard the extra garbage from the ogg video. We can do that
    // with the GPU very easily by altering texture coords!
    fiYUV.SetTexCoord(
      static_cast<GLfloat>(tIN.pic_x) / tIN.frame_width,
      static_cast<GLfloat>(tIN.pic_y + tIN.pic_height) / tIN.frame_height,
      static_cast<GLfloat>(tIN.pic_x + tIN.pic_width) / tIN.frame_width,
      static_cast<GLfloat>(tIN.pic_y) / tIN.frame_height);
    // Init texture
    InitTexture();
  }
  /* -- Initialise ogg headers and look for audio/video streams ------------ */
  void InitOggHeaders(void)
  { // Ok, Ogg parsing. The idea here is we have a bitstream that is made up of
    // Ogg pages. The libogg sync layer will find them for us. There may be
    // pages from several logical streams interleaved; we find the first theora
    // stream and ignore any others. Then we pass the pages for our stream to
    // the libogg stream layer which assembles our original set of packets out
    // of them. It's the packets that libtheora actually knows how to handle.
    // Start up Ogg stream synchronization layer
    ogg_sync_init(&oSS);       FlagSet(FL_OSINIT);
    // Init supporting Theora and vorbis structures needed in header parsing
    th_comment_init(&tCO);     FlagSet(FL_TCINIT);
    th_info_init(&tIN);        FlagSet(FL_TIINIT);
    vorbis_comment_init(&vCO); FlagSet(FL_VCINIT);
    vorbis_info_init(&vIN);    FlagSet(FL_VIINIT);
    // Which headers did we get?
    int iGotTheoraPage = 0, iGotVorbisPage = 0;
    // Ogg file open; parse the headers. Theora (like Vorbis) depends on some
    // initial header packets for decoder setup and initialization. We retrieve
    // these first before entering the main decode loop.
    for(bool bDone = IOBuffer(); !bDone; )
    { // This function takes the data stored in the buffer of the
      // ogg_sync_state struct and inserts them into an ogg_page. In an actual
      // decoding loop, this function should be called first to ensure that the
      // buffer is cleared. Caution:This function should be called before
      // reading into the buffer to ensure that data does not remain in the
      // ogg_sync_state struct. Failing to do so may result in a memory leak.
      switch(const int iPageOutResult = ogg_sync_pageout(&oSS, &oPG))
      { // Returned if more data needed or an internal error occurred.
        case 0:
        { // Try to rebuffer more data and throw error if no more data
          bDone = IOBuffer();
          if(bDone)
            XC("Not a valid ogg/theora stream!", "Identifier", IdentGet());
          // Done
          break;
        } // Indicated a page was synced and returned.
        case 1:
        { // Is this a mandated initial header? If not, stop parsing
          if(!ogg_page_bos(&oPG))
          { // don't leak the page; get it into the appropriate stream
            if(iGotTheoraPage) ogg_stream_pagein(&tSS, &oPG);
            if(iGotVorbisPage) ogg_stream_pagein(&vSS, &oPG);
            // We've initialised successfully (Break parent loop)
            bDone = true;
            // Break all loops
            break;
          } // Init stream
          ogg_stream_state sTest;
          ogg_stream_init(&sTest, ogg_page_serialno(&oPG));
          ogg_stream_pagein(&sTest, &oPG);
          ogg_stream_packetout(&sTest, &oPK);
          // Try Theora. If it is theora -- save this stream state
          if(!iGotTheoraPage &&
            th_decode_headerin(&tIN, &tCO, &tSI, &oPK) >= 0)
              { tSS = sTest; iGotTheoraPage = 1; }
          // Not Theora, try Vorbis. If it is vorbis -- save this stream state
          else if(!iGotVorbisPage &&
            vorbis_synthesis_headerin(&vIN, &vCO, &oPK) >= 0)
              { vSS = sTest; iGotVorbisPage = 1; }
          // Whatever it is, we don't care about it
          else ogg_stream_clear(&sTest);
        } // Returned if stream has not yet captured sync (bytes were skipped).
        case -1: break;
        // Done
        default: XC("Unknown OGG pageout result!",
                   "Identifier", IdentGet(), "Result", iPageOutResult); break;
      } // fall through tSS non-bos page parsing
    } // We're expecting more header packets
    while((iGotTheoraPage && iGotTheoraPage < 3) ||
          (iGotVorbisPage && iGotVorbisPage < 3))
    { // look for further theora headers
      while(iGotTheoraPage && iGotTheoraPage < 3)
      { // Is not a valid theora packet? Ignore. Might be a vorbis packet
        if(ogg_stream_packetout(&tSS, &oPK) != 1) break;
        // decode the headers
        if(!th_decode_headerin(&tIN, &tCO, &tSI, &oPK))
          XC("Error parsing Theora stream headers!",
            "Identifier", IdentGet());
        // Got the page
        ++iGotTheoraPage;
      } // Look for further vorbis headers
      while(iGotVorbisPage && iGotVorbisPage < 3)
      { // Is not a valid theora packet? Ignore. Might be a vorbis packet
        if(ogg_stream_packetout(&vSS, &oPK) != 1) break;
        // decode the headers
        if(vorbis_synthesis_headerin(&vIN, &vCO, &oPK))
          XC("Error parsing Vorbis stream headers!",
            "Identifier", IdentGet());
        // Got the page
        ++iGotVorbisPage;
      }
      // The header pages/packets will arrive before anything else we
      // care about, or the stream is not obeying spec.
      if(ogg_sync_pageout(&oSS, &oPG) > 0)
      { // demux into the appropriate stream
        if(iGotTheoraPage) ogg_stream_pagein(&tSS, &oPG);
        if(iGotVorbisPage) ogg_stream_pagein(&vSS, &oPG);
      } // Theora needs more data
      else if(IOBuffer())
        XC("EOF while searching for codec headers!",
          "Identifier", IdentGet(), "Position", fmFile.FileMapTell());
    } // Set flags of what headers we got
    if(iGotTheoraPage) FlagSet(FL_THEORA);
    if(iGotVorbisPage) FlagSet(FL_VORBIS);
  }
  /* -- Convert colour space to name -------------------------------------- */
  const string &ColourSpaceToString(const th_colorspace csId) const
    { return cParent.csStrings.Get(csId); }
  /* -- Convert pixel format to name --------------------------------------- */
  const string &PixelFormatToString(const th_pixel_fmt pfId) const
    { return cParent.pfStrings.Get(pfId); }
  /* -- Video properties ------------------------------------------- */ public:
  double GetVideoTime(void) const { return fdVideoTime; }
  double GetAudioTime(void) const { return fdAudioTime; }
  double GetDrift(void) const { return fdVideoTime-fdAudioTime; }
  double GetFPS(void) const { return fdFPS; }
  unsigned int GetFrame(void) const
    { return static_cast<unsigned int>(GetVideoTime() * GetFPS()); }
  unsigned int GetFrames(void) const { return uiVideoFrames; }
  unsigned int GetFramesSkipped(void) const { return uiVideoFramesLost; }
  th_pixel_fmt GetPixelFormat(void) const { return tIN.pixel_fmt; }
  th_colorspace GetColourSpace(void) const { return tIN.colorspace; }
  int GetFrameHeight(void) const { return static_cast<int>(tIN.frame_height); }
  int GetFrameWidth(void) const { return static_cast<int>(tIN.frame_width); }
  int GetHeight(void) const { return static_cast<int>(tIN.pic_height); }
  int GetWidth(void) const { return static_cast<int>(tIN.pic_width); }
  uint64_t GetLength(void) const { return fmFile.Size(); }
  bool HaveAudio(void) const { return !!sCptr; }
  ALenum GetAudioFormat(void) const { return eFormat; }
  const string GetFormatAsIdentifier(void) const
    { return cOal->GetALFormat(eFormat); }
  /* -- Update volume ------------------------------------------------------ */
  void CommitVolume(void)
  { // Ignore if no source
    if(!sCptr) return;
    // Set volume
    sCptr->SetGain(fAudioVolume * cSources->fVVolume * cSources->fGVolume);
  }
  /* -- Set volume --------------------------------------------------------- */
  void SetVolume(const ALfloat fVolume)
  { // Set volume
    fAudioVolume = fVolume;
    // Update volume
    CommitVolume();
  }
  /* -- Set colour of vertexes --------------------------------------------- */
  void SetFBOColour(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA) { fboC.SetQuadRGBA(fR, fG, fB, fA); }
  /* -- Set vertexes for FBO (Full and simple)------------------------------ */
  void SetFBOVertex(const GLfloat fX1, const GLfloat fY1, const GLfloat fX2,
    const GLfloat fY2) { fboC.SetVertex(fX1, fY1, fX2, fY2); }
  /* -- Set vertexes for FBO (Full and simple) with Angle ------------------ */
  void SetFBOVertex(const GLfloat fX1, const GLfloat fY1, const GLfloat fX2,
    const GLfloat fY2, const GLfloat fA)
      { fboC.SetVertex(fX1, fY1, fX2, fY2, fA); }
  /* -- Set tex coords for FBO (Full and simple) --------------------------- */
  void SetFBOTexCoord(const GLfloat fX1, const GLfloat fY1, const GLfloat fX2,
    const GLfloat fY2) { fboC.SetTexCoord(fX1, fY1, fX2, fY2); }
  /* -- Blit specific triangle --------------------------------------------- */
  void BlitTri(const size_t stTId) { FboActive()->BlitTri(fboC, stTId); }
  /* -- Blit quad ---------------------------------------------------------- */
  void Blit(void) { FboActive()->Blit(fboC); }
  /* -- Upload the texture -------read ------------------------------------- */
  void UploadTexture(void)
  { // Try to lock and return if failed or no frames waiting
    const UniqueLock ulWaitForProcessing{ mUpload, try_to_lock };
    if(!ulWaitForProcessing.owns_lock() || !stFWaiting) return;
    // Get frame
    Frame &fI = fData[stFActive];
    // Skip ahead frames if we need to catch up with audio
    // If we should draw?
    if(fI.bDraw)
    { // Upload texture data. This is quite safe because this data isnt
      // written to until the decoding routine thread has finished setting
      // these values.
      for(auto &aP : fI.aP)
      { // Bind the texture for this colour component
        cOgl->BindTexture(uiYUV[aP.stI]);
        // Get data
        th_img_plane &tipD = aP.tipP;
        // Set unpack row length because of how the image data is formatted by
        // the vorbis api
        cOgl->SetUnpackRowLength(tipD.stride);
        // Now upload the image data to opengl, the memory is already
        // pre-allocated
        cOgl->UploadTextureSub(tipD.width, tipD.height, GL_RED, tipD.data);
      } // Reset unpack row length to default
      cOgl->SetUnpackRowLength(0);
      // Initialise Y texture id, active texture and shader program
      fboC.ResetCache(uiYUV[0], 0, shProgram->GetProgram());
      // Commit the Y setup and configure the U setup
      fboC.FinishAndReset(uiYUV[1], 1, shProgram->GetProgram());
      // Commit the U setup
      fboC.FinishAndReset(uiYUV[2], 2, shProgram->GetProgram());
      // Blit the YUV multi-texture into the fbo
      fboC.Blit(fiYUV, uiYUV[2], 2, shProgram);
      // Commit the V texture and send everything to fbo list for rendering
      fboC.FinishAndRender();
      // No need to update again until decoder thread is done with another
      // frame
      fI.bDraw = false;
    } // One less buffer to wait
    --stFWaiting;
    ++stFFree;
    stFActive = (stFActive + 1) % fData.size();
  }
  /* -- Manage videos from main thread ------------------------------------- */
  void Render(void)
  { // If decoder thread is updating, then ignore
    UploadTexture();
    // Tell the thread to continue rebuffering
    Unsuspend(UB_DATA);
  }
  /* -- Video is playing? -------------------------------------------------- */
  bool IsPlaying(void) const { return tThread.ThreadIsRunning(); }
  /* -- DeInitialise audio ouput (because re-initialising) ----------------- */
  void DeInitAudio(void)
  { // Return if there is no audio in this video or video is not playing
    if(FlagIsClear(FL_VORBIS) || !IsPlaying()) return;
    // Pause the video from re-initialisation
    Pause(UB_REINIT);
  }
  /* -- Initialise audio (because audio is restarting) --------------------- */
  void InitAudio(void)
  { // Return if there is no audio in this video or video was not playing
    if(FlagIsClear(FL_VORBIS) || ubReason != UB_REINIT) return;
    // Remove playback after re-init flag
    ubReason = UB_BLOCK;
    // Continue playback from re-initialisation
    Play(UB_REINIT);
  }
  /* -- Stop and unload audio buffers -------------------------------------- */
  void StopAudioAndUnloadBuffers(const bool bReset)
  { // Ignore if no source or no vorbis stream
    if(!sCptr || FlagIsClear(FL_VORBIS)) return;
    // Stop from playing so all buffers are unqueued and wait for stop
    // then unqueue and delete the buffer
    sCptr->StopUnQueueAndDeleteAllBuffers();
    // Audio buffers are empty
    fdAudioBuffer = 0.0;
    // We need to free the source?
    if(!bReset) return;
    // Unlock the source so the source manager can recycle it
    sCptr->Unlock();
    sCptr = nullptr;
  }
  /* -- Unblock rebuffer --------------------------------------------------- */
  void Unsuspend(const Unblock ubNewReason)
  { // Acquire unique lock
    const UniqueLock ulGuard{ mBuffer };
    // Modify the unlock boolean
    ubReason = ubNewReason;
    // Send notification
    cvBuffer.notify_one();
  }
  /* -- Do pause video ----------------------------------------------------- */
  void Pause(const Unblock ubNewReason = UB_PAUSE)
  { // Return if not playing
    if(FlagIsClear(FL_PLAY)) return;
    // Remove playing flag
    FlagClear(FL_PLAY);
    // DeInit the thread
    tThread.ThreadSetExit();
    // Unblock the rebuffering thread
    Unsuspend(ubNewReason);
    // Wait for the thread to stop
    tThread.ThreadStop();
    // Stop and unload buffers
    StopAudioAndUnloadBuffers(true);
    // Flush video data
    FlushVideoData();
  }
  /* -- Stop video and free everything ------------------------------------- */
  void Stop(const Unblock ubNewReason = UB_STOP)
  { // Ignore if already stopped
    if(FlagIsSet(FL_STOP)) return;
    // Hard stop
    FlagSet(FL_STOP);
    // If thread already exited? Send stop event
    if(tThread.ThreadIsExited()) LuaEvtDispatch(VE_STOP);
    // Pause playback and synchronise
    Pause(ubNewReason);
    // Set the reason for stopping
    ubReason = ubNewReason;
    // Deinit texture and reset parameters
    if(FlagIsSet(FL_GLINIT))
    { // Remove opengl related initialisations
      FlagClear(FL_GLINIT);
      DeInitTexture();
      shProgram = nullptr;
      fboC.DeInit();
      fboC.CollectorUnregister();
    } // Vorbis audio stream was initialised?
    if(FlagIsSet(FL_VORBIS))
    { // Clear and reset it
      FlagClear(FL_VORBIS);
      ogg_stream_clear(&vSS);
      ClearStatic(vSS);
    } // Vorbis block data initialised?
    if(FlagIsSet(FL_VBINIT))
    { // Clear and reset it
      FlagClear(FL_VBINIT);
      vorbis_block_clear(&vBL);
      ClearStatic(vBL);
    } // Vorbis is dsp data initialised?
    if(FlagIsSet(FL_VDINIT))
    { // Clear and reset it
      FlagClear(FL_VDINIT);
      vorbis_dsp_clear(&vDS);
      ClearStatic(vDS);
    } // Vorbis info data initialised?
    if(FlagIsSet(FL_VIINIT))
    { // Clear and reset it
      FlagClear(FL_VIINIT);
      vorbis_info_clear(&vIN);
      ClearStatic(vIN);
    } // Vorbis comment data initialised?
    if(FlagIsSet(FL_VCINIT))
    { // Clear and reset it
      FlagClear(FL_VCINIT);
      vorbis_comment_clear(&vCO);
      ClearStatic(vCO);
    } // Theora video stream initialised?
    if(FlagIsSet(FL_THEORA))
    { // Clear and reset it
      FlagClear(FL_THEORA);
      ogg_stream_clear(&tSS);
      ClearStatic(tSS);
    } // Free theora decoding data if initialised
    if(tDC) th_decode_free(tDC);
    // Free theora setup data if initialised
    if(tSI) th_setup_free(tSI);
    // Theora info data initialised?
    if(FlagIsSet(FL_TIINIT))
    { // Clear and reset it
      FlagClear(FL_TIINIT);
      th_info_clear(&tIN);
      ClearStatic(tIN);
    } // Theora comment data initialised?
    if(FlagIsSet(FL_TCINIT))
    { // Clear and reset it
      FlagClear(FL_TCINIT);
      th_comment_clear(&tCO);
      ClearStatic(tCO);
    } // Ogg synchronisation data initialised?
    if(FlagIsSet(FL_OSINIT))
    { // Clear and reset it
      FlagClear(FL_OSINIT);
      ogg_sync_clear(&oSS);
      ClearStatic(oSS);
    } // De-init audio buffer
    mbAudio.DeInit();
    // Clear frame data
    for(Frame &fFrame : fData) fFrame.Reset();
    // Reset other structs
    ClearStatic(oPG); ClearStatic(oPK); ClearStatic(oPG); ClearStatic(tYB);
    // Reset Theora stuff
    tSI = nullptr;
    tDC = nullptr;
    // Reset counters
    uiVideoFrames = uiVideoFramesLost = 0;
    iVideoGranulePos = -1;
    fdVideoTime = fdAudioTime = fdAudioBuffer = fdFPS = 0.0;
    // Reset buffer status
    stFActive = stFNext = stFWaiting = stFFree = stLoop = 0;
    // Reset OpenAL stuff
    sCptr = nullptr;
    eFormat = AL_NONE;
    // Rewind data stream
    fmFile.FileMapRewind();
  }
  /* -- Awaken stream if hard stopped -------------------------------------- */
  void Awaken(void)
  { // Ignore if not hardstopped
    if(FlagIsClear(FL_STOP)) return;
    // Re-initialise as all memory was free'd
    FindVideoAndAudioStreams();
    // Initialise opengl if needed
    InitDisplayOutput();
    // Remove hardstop flag
    FlagClear(FL_STOP);
  }
  /* -- Play video --------------------------------------------------------- */
  void Play(const Unblock ubNewReason = UB_PLAY)
  { // Return if there is no audio in this video or video was not playing
    if(FlagIsSet(FL_PLAY)) return;
    // Re-open if hard stopped
    Awaken();
    // Get a source from OpenAL
    if(FlagIsSet(FL_VORBIS) && !sCptr)
    { // Compare number of channels in file to set appropriate format. This is
      // here and not at the files init stage as it handles re-inits too and
      // the FP supported audio format flag have changed.
      switch(vIN.channels)
      { // 1 channel mono?
        case 1: eFormat = cOal->Have32FPPB() ? AL_FORMAT_MONO_FLOAT32 :
                                               AL_FORMAT_MONO16;
                break;
        // 2 channel stereo?
        case 2: eFormat = cOal->Have32FPPB() ? AL_FORMAT_STEREO_FLOAT32 :
                                               AL_FORMAT_STEREO16;
                break;
        // Unknown channel count. Problem should already be handled at init.
        default: cLog->LogWarningExSafe("Video '$' audio playback failed. "
          "Invalid channel count of $!", IdentGet(), vIN.channels); return;
      } // Get a new sound source and if we got it update volume and return
      sCptr = GetSource();
      if(sCptr) CommitVolume();
      // Tell log
      else cLog->LogWarningExSafe(
        "Video '$' audio playback failed. Out of sources!", IdentGet());
    } // Set playing flag
    FlagSet(FL_PLAY);
    // Reset update time so the frame draws immediately
    CISync();
    // Set reason for playing
    ubReason = ubNewReason;
    // Start decoding
    tThread.ThreadStart(this);
  }
  /* -- Flush video frame data --------------------------------------------- */
  void FlushVideoData(void)
  { // Wait until async frame process
    const LockGuard lgWaitForUpload{ mUpload };
    // Disable draw and drop  all frames
    for(Frame &fSlot : fData) if(fSlot.bDraw)
    { // Do not show anymore
      fSlot.bDraw = false;
      // Now lost
      ++uiVideoFrames;
    } // Reset buffer index counters
    stFWaiting = stFActive = stFNext = 0;
    stFFree = fData.size();
    // Next frame will be drawn immediately
    CISync();
  }
  /* -- Rewind video ------------------------------------------------------- */
  void Rewind(void)
  { // Rewind video to start
    fmFile.FileMapRewind();
    // Reset granule position and frames rendered
    iVideoGranulePos = 0;
    uiVideoFrames = uiVideoFramesLost = 0;
    // Tell theora we reset the video position
    SetParameter<ogg_int64_t>(TH_DECCTL_SET_GRANPOS, 0);
    // Tell vorbis we reset the audio position. We have to call this whenever
    // we are seeking in the file.
    if(FlagIsSet(FL_VORBIS)) vorbis_synthesis_restart(&vDS);
    // Unload any playing buffers
    StopAudioAndUnloadBuffers(false);
    // Reset counters
    fdVideoTime = fdAudioTime = fdAudioBuffer = 0.0;
    // Flush current video data
    FlushVideoData();
    // Tell the thread to continue rebuffering
    Unsuspend(UB_DATA);
  }
  /* -- (De)Initialise video ouput ----------------------------------------- */
  void DeInitTexture(void)
  { // Delete the component textures
    GLL(cOgl->SetDeleteTextures(uiYUV),
      "Failed to delete $ texture components", uiYUV.size());
    ClearTextures();
  }
  /* -- Generate texture for specified video component --------------------- */
  void ConfigTexture(const GLuint uiTU, const int iW, const int iH)
  { // Get texture id
    GLuint &uiT = uiYUV[uiTU];
    // Bind texture
    GL(cOgl->BindTexture(uiT), "Failed to bind video component texture!",
      "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    // Allocate VRAM for texture
    GL(cOgl->UploadTexture(0, iW, iH, GL_R8, GL_RED, nullptr),
      "Failed to reserve texture memory for image component!",
        "Identifier", IdentGet(), "Index",  uiTU, "Texture", uiT,
        "Width",      iW,        "Height", iH);
    // Set filtering. Only need to use GL_NEAREST because the FBO is the same
    // size as the video textures.
    GL(cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR),
      "Failed to set minification filter for video component!",
      "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR),
      "Failed to set magnification filter for video component!",
        "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    // Set to repeat and disable filter
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT),
      "Failed to set texture wrapping S mode for video component!",
        "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT),
      "Failed to set texture wrapping T mode for video component!",
        "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    // Say what we did
    cLog->LogDebugExSafe("Video texture $x$ created for channel $.",
      iW, iH, uiTU);
  }
  /* -- Set filtering on video textures ------------------------------------ */
  void SetFilter(const bool bState)
  { // Set filter on fbo and commit
    fboC.SetFilter(bState ? 3 : 0);
    fboC.CommitFilter();
    // Update filter
    FlagSetOrClear(FL_FILTER, bState);
  }
  /* -- Looping functions -------------------------------------------------- */
  size_t GetLoop(void) const { return stLoop; }
  void SetLoop(const size_t stL) { stLoop = stL; }
  /* -- Colour key functions ----------------------------------------------- */
  bool GetKeyed(void) const
    { return FlagIsSet(FL_KEYED); }
  void UpdateShader(void)
    { shProgram = GetKeyed() ? &cFboBase->sh3DYUVK : &cFboBase->sh3DYUV; }
  void SetKeyed(const bool bState)
    { FlagSetOrClear(FL_KEYED, bState); UpdateShader(); }
  /* -- Generate component textures ---------------------------------------- */
  void InitTexture(void)
  { // Ignore if we don't have a video stream
    if(FlagIsClear(FL_THEORA)) return;
    // Chosen divisors
    int iWDIV, iHDIV;
    // Set divisor based on pixel format
    switch(GetPixelFormat())
    { // YUV420p : Y = Full size; UV = Half width and height
      case TH_PF_420: iWDIV = iHDIV = 2; break;
      // YUV422p : Y = Full size; UV = Half width and full height
      case TH_PF_422: iWDIV = 2; iHDIV = 1; break;
      // YUV444p : Y = Full size; UV = Full width and height
      case TH_PF_444: iWDIV = iHDIV = 1; break;
      // Unknown so throw error
      default: XC("Unsupported or unknown theora pixel format!",
        "Identifier", IdentGet(), "PixelFormat", GetPixelFormat());
    } // Create textures for Y/Cb/Cr multitexture
    GL(cOgl->CreateTextures(uiYUV),
      "Failed to create texture components for YUV texture!",
      "Identifier", IdentGet(), "Components", uiYUV.size());
    // Configure the Y component (always full size)
    ConfigTexture(0, GetFrameWidth(), GetFrameHeight());
    // Configure the Cb/Cr components
    for(GLuint uiIndex = 1; uiIndex <= 2; ++uiIndex)
      ConfigTexture(uiIndex, GetFrameWidth()/iWDIV, GetFrameHeight()/iHDIV);
    // Update fbo filter on fbo
    SetFilter(FlagIsSet(FL_FILTER));
    // Update choice of shader to use
    UpdateShader();
  }
  /* -- ReInitialise video from hard stop ---------------------------------- */
  void FindVideoAndAudioStreams(void)
  { // Ignore if headers already processed
    if(FlagIsSet(FL_THEORA) || FlagIsSet(FL_VORBIS)) return;
    // Look for audio/video streams
    InitOggHeaders();
    // And now we should have it all if not, well die
    if(FlagIsClear(FL_THEORA) && FlagIsClear(FL_VORBIS))
      XC("Could not find a Theora and/or Vorbis stream!",
         "Identifier", IdentGet());
    // Initialise video and audio stream parameters
    InitVideoStream();
    InitAudioStream();
    // Log success
    cLog->LogInfoExSafe("Video loaded '$' successfully.", IdentGet());
    // Log debug inforation
    cLog->LogDebugExSafe(
      "- Version: $$.$.$.\n"         "- Serial: $ <0x$$$>.\n"
      "- Viewable size: $x$ <$>.\n"  "- Actual size: $x$ <$>.\n"
      "- Aspect ratio: $:$.\n"       "- Origin: $x$.\n"
      "- Pixel format: $ <$>.\n"     "- Colour space: $ <$>.\n"
      "- Frame rate: $ fps.\n"       "- Version: $.\n"
      "- Audio channels: $.\n"       "- Bit rate: $ ($).\n"
      "- Target bit rate: $ ($).\n"  "- Upper bit rate: $ ($).\n"
      "- Nominal bit rate: $ ($).\n" "- Lower bit rate: $ ($).\n"
      "- Bit window: $ ($).",
      fixed, static_cast<int>(tIN.version_major),
        static_cast<int>(tIN.version_minor),
        static_cast<int>(tIN.version_subminor),
      tSS.serialno, hex, tSS.serialno, dec,
      tIN.pic_width, tIN.pic_height,
        ToRatio(tIN.pic_width, tIN.pic_height),
      tIN.frame_width, tIN.frame_height,
        ToRatio(tIN.frame_width, tIN.frame_height),
      tIN.aspect_numerator, tIN.aspect_denominator,
      tIN.pic_x, tIN.pic_y,
      PixelFormatToString(GetPixelFormat()), GetPixelFormat(),
      ColourSpaceToString(GetColourSpace()), GetColourSpace(), fdFPS,
      vIN.version,
      vIN.channels,
      vIN.rate, ToBitsStr(vIN.rate),
      tIN.target_bitrate, ToBitsStr(tIN.target_bitrate),
      vIN.bitrate_upper, ToBitsStr(vIN.bitrate_upper),
      vIN.bitrate_nominal, ToBitsStr(vIN.bitrate_nominal),
      vIN.bitrate_lower, ToBitsStr(vIN.bitrate_lower),
      vIN.bitrate_window, ToBitsStr(vIN.bitrate_window));
  }
  /* -- When data has asynchronously loaded -------------------------------- */
  void AsyncReady(FileMap &fClass)
  { // Move filemap into ours
    fmFile.FileMapSwap(fClass);
    // Initialise ogg video and audio
    FindVideoAndAudioStreams();
    // Set stopped
    FlagSet(FL_STOP);
  }
  /* -- Load video from memory asynchronously ------------------------------ */
  void InitAsyncArray(lua_State*const lS)
  { // Need 5 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 6);
    // Get and check parameters
    const string strF{ GetCppStringNE(lS, 1, "Identifier") };
    Asset &aData = *GetPtr<Asset>(lS, 2, "Data");
    CheckFunction(lS, 3, "ErrorFunc");
    CheckFunction(lS, 4, "ProgressFunc");
    CheckFunction(lS, 5, "SuccessFunc");
    // Set base parameters
    AsyncInitArray(lS, strF, "videoarray", StdMove(aData));
  }
  /* -- Load stream from file asynchronously ------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // Need 4 parameters (class pointer was already pushed onto the stack);
    CheckParams(lS, 5);
    // Get and check parameters
    const string strF{ GetCppFileName(lS, 1, "File") };
    CheckFunction(lS, 2, "ErrorFunc");
    CheckFunction(lS, 3, "ProgressFunc");
    CheckFunction(lS, 4, "SuccessFunc");
    // Load sample from file asynchronously
    AsyncInitFile(lS, strF, "videofile");
  }
  /* -- Thread main function ----------------------------------------------- */
  int VideoThreadMain(const Thread &tClass) try
  { // Send playing event if we're not temporarily de-initialising
    if(ubReason != UB_REINIT) LuaEvtDispatch(VE_PLAY);
    // Loop forever until thread should exit and manage video stream.
    // Capture return value and keep looping until non-zero exit.
    for(int iReason = 0;
        !tClass.ThreadShouldExit() && !iReason;
        iReason = ManageVideo());
    // Check reason for exiting
    switch(ubReason)
    { // Blocking mode set? Normal exit
      case UB_BLOCK: return 1;
      // Stopping or finishing?
      case UB_STOP: LuaEvtDispatch(VE_STOP); return 2;
      // Pausing?
      case UB_PAUSE: LuaEvtDispatch(VE_PAUSE); return 3;
      // Finishing?
      case UB_DATA: LuaEvtDispatch(VE_FINISH); return 4;
      // Re-initialising?
      case UB_REINIT: return 2;
      // Playing? (not possible)
      case UB_PLAY: return -3;
    } // Shouldn't get here
    return -2;
  } // exception occured?
  catch(const exception &E)
  { // Report it to log
    cLog->LogErrorExSafe("(VIDEO THREAD EXCEPTION) $", E.what());
    // Failure exit code
    return -1;
  }
  /* -- Destructor --------------------------------------------------------- */
  ~Video(void)
  { // Stop any pending async operations
    AsyncCancel();
    // Remove the registration now! The destructor of ICHelper will do this but
    // thats too late and the Manage() thread will access members when they are
    // being deinitialised and destroyed so the de-init needs to be serialised
    // properly.
    CollectorUnregister();
    // Stop the video. It will wait for the thread to terminate before
    // de-initialising everything so this should be safe.
    Stop();
  }
  /* -- Constructor -------------------------------------------------------- */
  Video(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperVideo{ *cVideos, this },
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    AsyncLoader<Video>{ this,
      EMC_MP_VIDEO },
    LuaEvtSlave{ this,
      EMC_VID_EVENT },
    VideoFlags(FL_NONE),
    tThread{ "video",
      SysThread::High,
      bind(&Video::VideoThreadMain,
        this, _1) },
    ubReason(UB_BLOCK),
    stLoop(0),
    oSS{},
    oPG{},
    oPK{},
    lIOBuf(cParent.lBufferSize),
    tSS{},
    tIN{},
    tCO{},
    tSI(nullptr),
    tDC(nullptr),
    tYB{},
    iVideoGranulePos(-1),
    fdVideoTime(0),
    fdFPS(0),
    uiVideoFrames(0),
    uiVideoFramesLost(0),
    stFActive(0),
    stFNext(0),
    stFWaiting(0),
    stFFree(0),
    vSS{},
    vIN{},
    vCO{},
    vDS{},
    vBL{},
    fdAudioTime(0.0),
    fdAudioBuffer(0.0),
    fAudioVolume(1.0f),
    fboC(GL_RGB8),
    shProgram(nullptr),
    sCptr(nullptr),
    eFormat(AL_NONE)
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- End ---------------------------------------------------------------- */
END_ASYNCCOLLECTOR(Videos, Video, VIDEO, // Finish Videos collector class
  /* -- Initialisers ------------------------------------------------------- */
  LuaEvtMaster{ EMC_VID_EVENT },       // Init lua event async helper
  csStrings{{                          // Init colour space strings list
    "TH_CS_UNSPECIFIED",         "TH_CS_ITU_REC_470M",
    "TH_CS_ITU_REC_470BG"
  }, "TH_CS_UNSUPPORTED" },            // End of colour space strings list
  pfStrings{{                          // Init pixel format strings list
    "TH_PF_420", "TH_PF_RSVD",         // 0-1
    "TH_PF_422", "TH_PF_444",          // 2-3
  }, "TH_PF_UNSUPPORTED"}              // End of pixel format strings list
)/* == Reinit textures (after engine thread shutdown) ====================== */
static void VideoReInitTextures(void)
{ // Ignore if no videos otherwise re-initialise ogl textures on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos re-initialising $ video textures...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vCptr : *cVideos) vCptr->InitTexture();
  cLog->LogDebugExSafe("Videos re-initialised $ video textures!",
    cVideos->CollectorCountUnsafe());
}
/* == De-init video textures (after thread shutdown) ======================= */
static void VideoDeInitTextures(void)
{ // Ignore if no videos otherwise de-initialise ogl textures on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos de-initialising $ video textures...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vCptr : *cVideos) vCptr->DeInitTexture();
  cLog->LogDebugExSafe("Videos de-initialised $ video textures!",
    cVideos->CollectorCountUnsafe());
}
/* == Clear event callbacks on all videos (must be synchronised) =========== */
static void VideoClearEvents(void)
{ // Lock access to video collector list and clear all video events
  const LockGuard lgVideosSync{ cVideos->CollectorGetMutex() };
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos clearing events from $ video objects...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vCptr : *cVideos) vCptr->LuaEvtDeInit();
  cLog->LogDebugExSafe("Videos cleared events from $ video objects!",
    cVideos->CollectorCountUnsafe());
}
/* == Stop all videos (must be sychronised) ================================ */
static void VideoStop(void)
{ // Lock access to video collector list and stop all videos
  const LockGuard lgVideosSync{ cVideos->CollectorGetMutex() };
  for(Video*const vCptr : *cVideos) vCptr->Stop();
}
/* == DeInit all videos (after engine thread shutdown) ===================== */
static void VideoDeInit(void)
{ // Ignore if no videos otherwise de-initialise oal buffers on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos de-initialising $ videos...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vCptr : *cVideos) vCptr->DeInitAudio();
  cLog->LogDebugExSafe("Videos de-initialised $ videos!",
    cVideos->CollectorCountUnsafe());
}
/* == ReInit all videos (after engine thread shutdown) ===================== */
static void VideoReInit(void)
{ // Ignore if no videos otherwise re-initialise oal buffers on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos re-initialising $ videos...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vCptr : *cVideos) vCptr->InitAudio();
  cLog->LogDebugExSafe("Videos re-initialised $ videos!",
    cVideos->CollectorCountUnsafe());
}
/* == Render all videos ==================================================== */
static void VideoRender(void)
  { for(Video*const vCptr : *cVideos) vCptr->Render(); }
/* == Update all streams base volume ======================================= */
static void VideoCommitVolume(void)
  { for(Video*const vCptr : *cVideos) vCptr->CommitVolume(); }
/* == Set buffer size ====================================================== */
static CVarReturn VideoSetBufferSize(const long lSize)
  { return CVarSimpleSetIntNLG(cVideos->lBufferSize, lSize, 4096, 16777216); }
/* == Set all streams base volume ========================================== */
static CVarReturn VideoSetVolume(const ALfloat fVolume)
{ // Ignore if invalid value
  if(fVolume < 0.0f || fVolume > 1.0f) return DENY;
  // Store volume (SOURCES class keeps it)
  cSources->fVVolume = fVolume;
  // Update volumes
  VideoCommitVolume();
  // Success
  return ACCEPT;
}
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
