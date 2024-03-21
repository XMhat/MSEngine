/* == VIDEO.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file handles streaming from .OGV files and playing to OpenAL.  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IVideo {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IASync::P;
using namespace IClock::P;             using namespace ICollector::P;
using namespace ICVarDef::P;           using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFbo::P;
using namespace IFileMap::P;           using namespace IFlags;
using namespace IIdent::P;             using namespace ILog::P;
using namespace ILuaEvt::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IOal::P;
using namespace IOgl::P;               using namespace IPcmLib::P;
using namespace IShader::P;            using namespace IShaders::P;
using namespace ISource::P;            using namespace IStd::P;
using namespace IStream::P;            using namespace IString::P;
using namespace ISysUtil::P;           using namespace IThread::P;
using namespace ITimer::P;             using namespace IUtil::P;
using namespace Lib::Ogg;              using namespace Lib::Ogg::Theora;
using namespace Lib::OpenAL;           using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Video collector class for collector data and custom variables -------- */
BEGIN_ASYNCCOLLECTOREX(Videos, Video, CLHelperSafe,
/* -- Public variables ----------------------------------------------------- */
typedef IdList<TH_CS_NSPACES> CSStrings;
const CSStrings    csStrings;          /* Colour space strings list         */\
typedef IdList<TH_PF_NFORMATS> PFStrings;
const PFStrings    pfStrings;          /* Pixel format strings list         */\
SafeLong           lBufferSize;        /* Default buffer size               */\
SafeDouble         fdMaxDrift;,,       /* Maximum drift before drop frames  */\
/* -- Derived classes ------------------------------------------------------ */
private LuaEvtMaster<Video, LuaEvtTypeParam<Video>>); // Lua event
/* ------------------------------------------------------------------------- */
BUILD_FLAGS(Video,
  /* ----------------------------------------------------------------------- */
  // No flags set?                     // Locked on to a Theora stream?
  FL_NONE                {0x00000000}, FL_STINIT              {0x00000001},
  // Locked onto a Vorbis stream?      Have a theora stream?
  FL_SVINIT              {0x00000002}, FL_THEORA              {0x00000004},
  // Have a vorbis stream?             vorbis_synthesis_init was called?
  FL_VORBIS              {0x00000008}, FL_VDINIT              {0x00000010},
  // vorbis_block_init was called?     ogg_sync_init called?
  FL_VBINIT              {0x00000020}, FL_OSINIT              {0x00000040},
  // th_comment_init called?           th_info_init called?
  FL_TCINIT              {0x00000080}, FL_TIINIT              {0x00000100},
  // vorbis_info_init called?          vorbis_comment_init called?
  FL_VCINIT              {0x00000200}, FL_VIINIT              {0x00000400},
  // Video output initialised?         Video is keyed?
  FL_GLINIT              {0x00000800}, FL_KEYED               {0x08000000},
  // Filtering is enabled?             Hard stopped (reopen on play)?
  FL_FILTER              {0x10000000}, FL_STOP                {0x20000000},
  // Video is playing?
  FL_PLAY                {0x40000000}
);/* ======================================================================= */
BEGIN_ASYNCMEMBERCLASSEX(Videos, Video, ICHelperSafe, /* No CLHelper */),
  /* -- Base classes ------------------------------------------------------- */
  public Fbo,                          // Video file name
  public AsyncLoaderVideo,             // Asynchronous laoding of videos
  public LuaEvtSlave<Video>,           // Lua asynchronous events
  public VideoFlags,                   // Video settings flags
  private ClockInterval<>              // Frame playback timing helper
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
  enum Unblock { UB_STANDBY, UB_BLOCK, UB_DATA, UB_REINIT, UB_PLAY, UB_STOP,
                 UB_PAUSE, UB_FINISH };
  /* --------------------------------------------------------------- */ public:
  enum Event { VE_PLAY, VE_LOOP, VE_STOP, VE_PAUSE, VE_FINISH };
  /* -- Concurrency -------------------------------------------------------- */
  Thread           tThread;            // Video Decoding Thread
  mutex            mUpload;            // mutex for uploading data
  atomic<Unblock>  ubReason;           // Unlock condition variable
  SafeSizeT        stLoop;             // Loops count
  double           fdDrift,            // Drift between audio and video
                   fdMaxDriftNeg,      // Maximum allowed drift (negative
                   fdMaxDrift;         // Maximum allowed drift
  SafeBool         bPause;             // Only pause the stream?
  /* -- Ogg ---------------------------------------------------------------- */
  ogg_sync_state   osysData;           // Ogg sync state
  ogg_page         opgData;            // Ogg page
  ogg_packet       opkData;            // Ogg packet
  const long       lIOBuf;             // IO buffer size
  const size_t     stIOBuf;            // IO buffer size
  FileMap          fmFile;             // File map for reading file
  /* -- Theora ------------------------------------------------------------- */
  ogg_stream_state ostsTheora;         // Ogg (Theora) stream states
  th_info          tiData;             // Theora info struct
  th_comment       tcData;             // Theora comment struct
  th_setup_info   *tsiPtr;             // Theora setup info
  th_dec_ctx      *tdcPtr;             // Theora decoder context
  th_ycbcr_buffer  tybData;            // Theora colour buffer
  ogg_int64_t      iVideoGranulePos;   // Video granule position
  SafeDouble       fdVideoTime;        // Video time index
  double           fdFPS;              // Video fps
  SafeUInt         uiVideoFrames,      // Frames rendered
                   uiVideoFramesLost;  // Frames skipped
  array<Frame,2>   faData;             // Frame data
  size_t           stFActive,          // Currently active frame
                   stFNext,            // Next frame to process
                   stFWaiting;         // Frames waiting to be processed
  SafeSizeT        stFFree;            // Frames free to be processed
  /* -- Vorbis ------------------------------------------------------------- */
  ogg_stream_state ostsVorbis;         // Ogg (Vorbis) stream states
  vorbis_info      viData;             // Vorbis decoder info
  vorbis_comment   vcData;             // Vorbis comment block
  vorbis_dsp_state vdsData;            // Vorbis DSP state
  vorbis_block     vbData;             // Vorbis decoder block
  SafeDouble       fdAudioTime;        // Audio time index
  ALdouble         fdAudioBuffer;      // Audio buffered
  ALfloat          fAudioVolume;       // Audio volume
  /* -- OpenGL ------------------------------------------------------------- */
  FboItem          fboYCbCr;           // Blit data for actual YCbCr components
  array<GLuint,3>  uiaYCbCr;           // Texture id's for YCbCr components
  Shader          *shProgram;          // Shader program to use
  /* -- OpenAL ------------------------------------------------------------- */
  Source          *sCptr;              // Source class
  ALenum           eFormat;            // Internal format
  /* == Buffer more data for OGG decoder ========================== */ private:
  bool DoIOBuffer(void)
  { // Get some memory from ogg which we have to do every time we need to read
    // data into it and if succeeded? Read data info buffer and if we read
    // some bytes? Tell ogg how much we wrote and return.
    if(char*const cpBuffer = ogg_sync_buffer(&osysData, lIOBuf))
      if(const size_t stCount = fmFile.FileMapReadToAddr(cpBuffer, stIOBuf))
        return ogg_sync_wrote(&osysData, static_cast<long>(stCount)) == -1;
    // EOF or buffer invalid so return error
    return true;
  }
  /* -- Rewind the theora stream ------------------------------------------- */
  void DoRewind(void)
  { // Rewind video to start
    fmFile.FileMapRewind();
    // Tell theora we reset the video position
    if(FlagIsSet(FL_THEORA))
      SetParameter<ogg_int64_t>(TH_DECCTL_SET_GRANPOS, 0);
    // Reset Vorbis dsp as required by documentation
    if(FlagIsSet(FL_VORBIS)) vorbis_synthesis_restart(&vdsData);
  }
  /* -- Rewind the theora stream and reset variables ----------------------- */
  void DoRewindAndReset(void)
  { // Rewind video to start
    DoRewind();
    // Reset granule position and frames rendered
    iVideoGranulePos = 0;
    uiVideoFrames = uiVideoFramesLost = 0;
    // Reset counters
    fdVideoTime = fdAudioTime = fdDrift = fdAudioBuffer = 0.0;
  }
  /* -- Update video position ---------------------------------------------- */
  void UpdateVideoPosition(void)
    { fdVideoTime = th_granule_time(tdcPtr, iVideoGranulePos); }
  /* -- Tell worker thread to exit ----------------------------------------- */
  void InformExit(const Unblock ubNewReason = UB_PAUSE)
  { // Set exit reason
    ubReason = ubNewReason;
    // DeInit the thread, unblock the worker thread and stop and unload buffers
    tThread.ThreadStop();
  }
  /* -- Get/Set theora decoder control ------------------------------------- */
  template<typename AnyType>int SetParameter(const int iW, AnyType atV)
    { return th_decode_ctl(tdcPtr, iW,
        reinterpret_cast<void*>(&atV), sizeof(atV)); }
  template<typename AnyType=int>AnyType GetParameter(const int iW) const
    { return static_cast<AnyType>(th_decode_ctl(tdcPtr, iW, nullptr, 0)); }
  /* -- Manage video decoding thread --------------------------------------- */
  int VideoHandle(void)
  { // Audio and/or video availability flags
    BUILD_FLAGS(Avail, AF_NONE{0x0},  // Video or audio not ready?
                      AF_VIDEO{0x1},  // Enough video buffered?
                      AF_AUDIO{0x2}); // Enough audio buffered?
    // Video and audio was rendered successfully?
    AvailFlags afStatus{ AF_NONE };
    // Have audio stream?
    if(FlagIsSet(FL_VORBIS))
    { // Have audio source?
      if(sCptr) do
      { // OpenAL buffer id
        ALuint uiBuffer;
        // Get number of buffers queued
        for(ALsizei stP = sCptr->GetBuffersProcessed(); stP; --stP)
        { // Unqueue a buffer and break if failed
          uiBuffer = sCptr->UnQueueBuffer();
          if(cOal->HaveError()) continue;
          // Remove buffer time
          fdAudioBuffer = UtilMaximum(fdAudioBuffer -
            (cOal->GetBufferInt<ALdouble>(uiBuffer, AL_SIZE) /
              viData.rate / viData.channels), 0.0);
          // Delete the buffer that was returned continue if successful
          ALL(cOal->DeleteBuffer(uiBuffer),
            "Video failed to delete unqueued buffer $ in '$'!",
               uiBuffer, IdentGet());
        } // Raise pitch if behind
        if(fdDrift > fdMaxDrift) sCptr->SetPitch(1.1f);
        // Lower pitch if ahead
        else if(fdDrift < fdMaxDriftNeg) sCptr->SetPitch(0.9f);
        // No pitch adjustment required
        else sCptr->SetPitch(1.0f);
        // Break if we have enough audio buffered or...
        if(fdAudioBuffer >= fdMaxDrift) break;
        // Get PCM data stored as float
        ALfloat **fpPCM;
        // If frames are available, but we're way behind the video?
        if(const size_t stFrames =
          static_cast<size_t>(vorbis_synthesis_pcmout(&vdsData, &fpPCM)))
        { // Tell vorbis how much we read
          vorbis_synthesis_read(&vdsData, static_cast<int>(stFrames));
          // Set audio time
          fdAudioTime = vorbis_granule_time(&vdsData, vdsData.granulepos);
          // Set that we got audio
          afStatus.FlagSet(AF_AUDIO);
          // Get channels as size_t
          const size_t stChannels = static_cast<size_t>(viData.channels);
          // Length of data
          size_t stFrameSize = sizeof(float) * stFrames * stChannels;
          // Just incase we need more memory.
          MemResizeUp(stFrameSize);
          // Send the PCM processing depending on if audio device can play
          // as floating point pcm or not. If we have 32-bit float support?
          // Convert data to native PCM float 32-bit audio
          if(cOal->Have32FPPB())
            Stream::VorbisFramesToF32PCM(fpPCM, stFrames, stChannels,
              MemPtr<ALfloat>());
          // Uploading as INTEGER 16-Bit PCM data
          else
          { // Convert data to native PCM integer 16-bit audio
            Stream::VorbisFramesToI16PCM(fpPCM, stFrames, stChannels,
              MemPtr<ALshort>());
            // Now using half the buffer size (F32 is 4 bytes, I16 is 2)
            stFrameSize >>= 1;
          } // Generate a buffer for the pcm data and if succeeded?
          cOal->CreateBuffer(uiBuffer);
          ALenum alErr = cOal->GetError();
          if(alErr == AL_NO_ERROR)
          { // Buffer the data. Note that the size of mbAudio can be bigger
            // than the actual data so we have to pass in the calculated data
            // size.
            cOal->BufferData(uiBuffer, eFormat, MemPtr(),
              static_cast<ALsizei>(stFrameSize),
              static_cast<ALsizei>(viData.rate));
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
                fdAudioBuffer += static_cast<ALdouble>(stFrameSize) /
                  viData.rate / viData.channels;
                // Success grabbing and uploading audio so break the loop
                break;
              } // Log the error
              else cLog->LogWarningExSafe("Video queue buffer failed on '$' "
                 "(B:$;F:$;A:$;S:$;R:$;AL:$<$$>)!",
                 IdentGet(), uiBuffer, eFormat, MemPtr(), stFrameSize,
                 viData.rate, cOal->GetALErr(alErr), hex, alErr);
            } // Log the error
            else cLog->LogWarningExSafe("Video buffering failed attempt on '$' "
              "(B:$;F:$;A:$;S:$;R:$;AL:$<$$>)!",
              IdentGet(), uiBuffer, eFormat, MemPtr(), stFrameSize,
              viData.rate, cOal->GetALErr(alErr), hex, alErr);
            // Delete the buffers because of error
            ALL(cOal->DeleteBuffer(uiBuffer),
              "Video failed to delete buffer $ in '$' "
              "after failed data upload attempt!", uiBuffer, IdentGet());
          } // Log the error
          else cLog->LogWarningExSafe("Video create buffer failed on '$' "
            "(F:$;A:$;S:$;R:$;AL:$<$$>)!",
            IdentGet(), eFormat, MemPtr(), stFrameSize, viData.rate,
            cOal->GetALErr(alErr), hex, alErr);
          // Success grabbing audio anyway so break
          break;
        } // No audio left so try to feed another packet and break if failed
        else if(ogg_stream_packetout(&ostsVorbis, &opkData) <= 0) break;
        // We still have data? synthesize a packet and reloop to try again
        else if(!vorbis_synthesis(&vbData, &opkData))
          vorbis_synthesis_blockin(&vdsData, &vbData);
      } // ...until we got audio or lost the source
      while(afStatus.FlagIsClear(AF_AUDIO) && sCptr);
      // Have no audio source? Repeat...
      else do
      { // Get PCM data stored as float
        ALfloat **fpPCM;
        // If frames are available, but we're way behind the video?
        if(const size_t stFrames =
          static_cast<size_t>(vorbis_synthesis_pcmout(&vdsData, &fpPCM)))
        { // Tell vorbis how much we read
          vorbis_synthesis_read(&vdsData, static_cast<int>(stFrames));
          // Set audio time
          fdAudioTime = vorbis_granule_time(&vdsData, vdsData.granulepos);
          // Set that we got audio
          afStatus.FlagSet(AF_AUDIO);
          // Done with this loop for now
          break;
        } // No audio left so try to feed another packet and break if failed
        else if(ogg_stream_packetout(&ostsVorbis, &opkData) <= 0) break;
        // We still have data? synthesize a packet and reloop to try again
        else if(!vorbis_synthesis(&vbData, &opkData))
          vorbis_synthesis_blockin(&vdsData, &vbData);
      } // ...until we got a source
      while(afStatus.FlagIsClear(AF_AUDIO));
    } // Have theora stream and we've got enough audio buffered?
    if(FlagIsSet(FL_THEORA) &&
      ((FlagIsSet(FL_VORBIS) && fdAudioBuffer >= fdMaxDrift) ||
      FlagIsClear(FL_VORBIS)))
    { // Force to suspend if we haven't reached the time we expect to draw yet
      if(CINoTrigger()) afStatus.FlagSet(AF_VIDEO);
      // Assemble Theora packets and if successful?
      else if(ogg_stream_packetout(&ostsTheora, &opkData) > 0)
      { // Decode the packet and if we get a positive result?
        switch(const int iR =
          th_decode_packetin(tdcPtr, &opkData, &iVideoGranulePos))
        { // Success?
          case 0:
            // Need a scope for destructing upcoming sychronisation
            { // Wait until uploading is done
              const LockGuard lgWaitForUpload{ mUpload };
              // No buffers free? Force this next buffer to be free
              if(!stFFree) { ++stFFree; --stFWaiting; }
              // Get next frame to draw
              Frame &frRef = faData[stFNext];
              // If decoding the frame failed?
              if(th_decode_ycbcr_out(tdcPtr, tybData))
              { // Skip the frame
                frRef.bDraw = false;
                // We lost this frame
                ++uiVideoFramesLost;
              } // Decoding succeeded?
              else
              { // Set pointers to planes and we will be drawing this frame
                for(auto &aPlane : frRef.aP) aPlane.tipP = tybData[aPlane.stI];
                frRef.bDraw = true;
                // We processed this frame
                ++uiVideoFrames;
              } // Buffer filled
              stFNext = (stFNext + 1) % faData.size();
              ++stFWaiting;
              --stFFree;
            } // We processed a video frame
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
            // Set next frame time and fall through to break
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
    } // If we processed a video frame?
    if(afStatus.FlagIsSet(AF_VIDEO))
    { // If stream has audio and not at the beginning?
      if(FlagIsSet(FL_VORBIS) && fdAudioTime > 0.0)
      { // Update drift and if drifting too much?
        fdDrift = fdVideoTime - fdAudioTime;
        if(fdDrift >= fdMaxDrift)
        { // Wait until uploading is done
          const LockGuard lgWaitForUpload{ mUpload };
          // Reset buffer statistics. Don't reset buffer id though
          stFFree = faData.size();
          stFWaiting = 0;
          // Create a counter to hold skipped frames. Since the counter in the
          // class is atomic, we'll just make sure we modify it once.
          unsigned int uiSkipped = 0;
          // For each frame...
          for(Frame &fSlot : faData)
          { // Ignore if not set to draw
            if(!fSlot.bDraw) continue;
            // Reset the frame data
            fSlot.Reset();
            // Increment skipped frames counter
            ++uiSkipped;
          } // Add to frames lost
          uiVideoFramesLost += uiSkipped;
          // Read and decode a new packet and return if succeeded
          if(ogg_stream_packetout(&ostsTheora, &opkData) > 0
            && th_decode_packetin(tdcPtr, &opkData, &iVideoGranulePos) >= 0)
              UpdateVideoPosition();
          // Next frame can show immediately
          CISync();
        }
      } // Wait a little bit
      else cTimer->TimerSuspend(1);
    } // Didn't process a video frame or audio frame?
    else if(afStatus.FlagIsClear(AF_AUDIO))
    { // Is end of file?
      if(fmFile.FileMapIsEOF())
      { // Rewind video
        DoRewind();
        // We should loop?
        if(stLoop > 0)
        { // Reduce loops if not infinity
          if(stLoop != StdMaxSizeT) --stLoop;
          // Send looping event
          LuaEvtDispatch(VE_LOOP);
        } // No more loops so we're done if everything is played
        else
        { // Set finished reason for exiting
          ubReason = UB_FINISH;
          // Exit the thread
          return 2;
        }
      } // Load more data
      else if(DoIOBuffer())
        cLog->LogWarningExSafe("Video '$' failed to read file data: $!",
          IdentGet(), StdGetError());
      // Break apart file data to useful packets
      while(ogg_sync_pageout(&osysData, &opgData) > 0)
      { // Find more theora and vorbis data
        if(FlagIsSet(FL_THEORA)) ogg_stream_pagein(&ostsTheora, &opgData);
        if(FlagIsSet(FL_VORBIS)) ogg_stream_pagein(&ostsVorbis, &opgData);
      }
    } // Keep thread loop alive
    return 0;
  }
  /* -- Thread main function ----------------------------------------------- */
  int VideoThreadMain(const Thread &tClass) try
  { // Send playing event if we're not temporarily de-initialising
    if(ubReason != UB_REINIT) LuaEvtDispatch(VE_PLAY);
    // Loop forever until thread should exit and manage video stream.
    // Capture return value and keep looping until non-zero exit.
    while(!tClass.ThreadShouldExit() && !VideoHandle());
    // Log the reason why the thread should be terminated
    cLog->LogDebugExSafe("Video '$' main loop exit with reason $!",
      IdentGet(), ubReason.load());
    // Why did the video manager terminate?
    switch(ubReason.load())
    { // The thread was in standby? Shouldn't happen! Restart the thread
      case UB_STANDBY: [[fallthrough]];
      // The thread was blocking? Shouldn't happen! Restart the thread
      case UB_BLOCK: [[fallthrough]];
      // The video was playing? Shouldn't happen! Restart the thread
      case UB_PLAY: [[fallthrough]];
      // The video was re-initialised? Shouldn't happen! Restart the thread
      case UB_REINIT: [[fallthrough]];
      // The thread was terminated? Just break
      case UB_DATA: break;
      // The video finished playing? Send finish event
      case UB_FINISH: LuaEvtDispatch(VE_FINISH); break;
      // The video was stopped? Send stop event to guest
      case UB_STOP: LuaEvtDispatch(VE_STOP); break;
      // The video was paused? Send pause event to guest
      case UB_PAUSE: LuaEvtDispatch(VE_PAUSE); break;
    } // Exit thread cleanly with specified reason
    return 1;
  } // exception occured?
  catch(const exception &E)
  { // Report it to log
    cLog->LogErrorExSafe("(VIDEO THREAD EXCEPTION) $", E.what());
    // Failure exit code
    return -1;
  }
  /* -- Convert colour space to name --------------------------------------- */
  const string &ColourSpaceToString(const th_colorspace csId) const
    { return cVideos->csStrings.Get(csId); }
  /* -- Convert pixel format to name --------------------------------------- */
  const string &PixelFormatToString(const th_pixel_fmt pfId) const
    { return cVideos->pfStrings.Get(pfId); }
  /* -- Video properties ------------------------------------------- */ public:
  double GetVideoTime(void) const { return fdVideoTime; }
  double GetAudioTime(void) const { return fdAudioTime; }
  double GetDrift(void) const { return fdDrift; }
  double GetFPS(void) const { return fdFPS; }
  unsigned int GetFrame(void) const
    { return static_cast<unsigned int>(GetVideoTime() * GetFPS()); }
  unsigned int GetFrames(void) const { return uiVideoFrames; }
  unsigned int GetFramesSkipped(void) const { return uiVideoFramesLost; }
  th_pixel_fmt GetPixelFormat(void) const { return tiData.pixel_fmt; }
  th_colorspace GetColourSpace(void) const { return tiData.colorspace; }
  int GetFrameHeight(void) const
    { return static_cast<int>(tiData.frame_height); }
  int GetFrameWidth(void) const
    { return static_cast<int>(tiData.frame_width); }
  int GetHeight(void) const { return static_cast<int>(tiData.pic_height); }
  int GetWidth(void) const { return static_cast<int>(tiData.pic_width); }
  uint64_t GetLength(void) const { return fmFile.MemSize(); }
  bool HaveAudio(void) const { return !!sCptr; }
  ALenum GetAudioFormat(void) const { return eFormat; }
  const string GetFormatAsIdentifier(void) const
    { return cOal->GetALFormat(eFormat); }
  /* -- When data has asynchronously loaded -------------------------------- */
  void AsyncReady(FileMap &fClass)
  { // Move filemap into ours
    fmFile.FileMapSwap(fClass);
    // Ok, Ogg parsing. The idea here is we have a bitstream that is made up of
    // Ogg pages. The libogg sync layer will find them for us. There may be
    // pages from several logical streams interleaved; we find the first theora
    // stream and ignore any others. Then we pass the pages for our stream to
    // the libogg stream layer which assembles our original set of packets out
    // of them. It's the packets that libtheora actually knows how to handle.
    // Start up Ogg stream synchronization layer
    ogg_sync_init(&osysData);     FlagSet(FL_OSINIT);
    // Init supporting Theora and vorbis structures needed in header parsing
    th_comment_init(&tcData);     FlagSet(FL_TCINIT);
    th_info_init(&tiData);        FlagSet(FL_TIINIT);
    vorbis_comment_init(&vcData); FlagSet(FL_VCINIT);
    vorbis_info_init(&viData);    FlagSet(FL_VIINIT);
    // Which headers did we get?
    int iGotTheoraPage = 0, iGotVorbisPage = 0;
    // Finished buffering?
    bool bDone = false;
    // Loop.
    do
    { // This function takes the data stored in the buffer of the
      // ogg_sync_state struct and inserts them into an ogg_page. In an actual
      // decoding loop, this function should be called first to ensure that the
      // buffer is cleared. Caution:This function should be called before
      // reading into the buffer to ensure that data does not remain in the
      // ogg_sync_state struct. Failing to do so may result in a memory leak.
      switch(const int iPageOutResult = ogg_sync_pageout(&osysData, &opgData))
      { // Returned if more data needed or an internal error occurred.
        case 0:
        { // If we're end of file then it wasn't a valid stream
          if(fmFile.FileMapIsEOF())
            XC("Not a valid ogg/theora stream!", "Identifier", IdentGet());
          // Try to rebuffer more data and throw error if error reading
          if(DoIOBuffer())
            XC("Read ogg/theora stream error!",
               "Identifier", IdentGet(), "Reason", StrFromErrNo());
          // Done
          break;
        } // Indicated a page was synced and returned.
        case 1:
        { // Is this a mandated initial header? If not, stop parsing
          if(!ogg_page_bos(&opgData))
          { // don't leak the page; get it into the appropriate stream
            if(iGotTheoraPage) ogg_stream_pagein(&ostsTheora, &opgData);
            if(iGotVorbisPage) ogg_stream_pagein(&ostsVorbis, &opgData);
            // We've initialised successfully (Break parent loop)
            bDone = true;
            // Break all loops
            break;
          } // Init stream
          ogg_stream_state ossTest;
          ogg_stream_init(&ossTest, ogg_page_serialno(&opgData));
          ogg_stream_pagein(&ossTest, &opgData);
          ogg_stream_packetout(&ossTest, &opkData);
          // Try Theora. If it is theora -- save this stream state
          if(!iGotTheoraPage &&
            th_decode_headerin(&tiData, &tcData, &tsiPtr, &opkData) >= 0)
              { ostsTheora = ossTest; FlagSet(FL_STINIT); iGotTheoraPage = 1; }
          // Not Theora, try Vorbis. If it is vorbis -- save this stream state
          else if(!iGotVorbisPage &&
            vorbis_synthesis_headerin(&viData, &vcData, &opkData) >= 0)
              { ostsVorbis = ossTest; FlagSet(FL_SVINIT); iGotVorbisPage = 1; }
          // Whatever it is, we don't care about it
          else ogg_stream_clear(&ossTest);
        } // Returned if stream has not yet captured sync (bytes were skipped).
        case -1: break;
        // Done
        default: XC("Unknown OGG pageout result!",
                    "Identifier", IdentGet(), "Result", iPageOutResult);
                 break;
      } // fall through ostsTheora non-bos page parsing
    } // ...until done
    while(!bDone);
    // We're expecting more header packets
    while((iGotTheoraPage && iGotTheoraPage < 3) ||
          (iGotVorbisPage && iGotVorbisPage < 3))
    { // look for further theora headers
      while(iGotTheoraPage && iGotTheoraPage < 3)
      { // Is not a valid theora packet? Ignore. Might be a vorbis packet
        if(ogg_stream_packetout(&ostsTheora, &opkData) != 1) break;
        // decode the headers
        if(!th_decode_headerin(&tiData, &tcData, &tsiPtr, &opkData))
          XC("Error parsing Theora stream headers!",
             "Identifier", IdentGet());
        // Got the page
        ++iGotTheoraPage;
      } // Look for further vorbis headers
      while(iGotVorbisPage && iGotVorbisPage < 3)
      { // Is not a valid theora packet? Ignore. Might be a vorbis packet
        if(ogg_stream_packetout(&ostsVorbis, &opkData) != 1) break;
        // decode the headers
        if(vorbis_synthesis_headerin(&viData, &vcData, &opkData))
          XC("Error parsing Vorbis stream headers!",
             "Identifier", IdentGet());
        // Got the page
        ++iGotVorbisPage;
      }
      // The header pages/packets will arrive before anything else we
      // care about, or the stream is not obeying spec.
      if(ogg_sync_pageout(&osysData, &opgData) > 0)
      { // demux into the appropriate stream
        if(iGotTheoraPage) ogg_stream_pagein(&ostsTheora, &opgData);
        if(iGotVorbisPage) ogg_stream_pagein(&ostsVorbis, &opgData);
      } // Theora needs more data
      else if(DoIOBuffer())
        XC("EOF while searching for codec headers!",
           "Identifier", IdentGet(), "Position", fmFile.FileMapTell());
    } // Set flags of what headers we got
    if(iGotTheoraPage) FlagSet(FL_THEORA);
    if(iGotVorbisPage) FlagSet(FL_VORBIS);
    // And now we should have it all if not, well die
    if(FlagIsClear(FL_THEORA) && FlagIsClear(FL_VORBIS))
      XC("Could not find a Theora and/or Vorbis stream!",
         "Identifier", IdentGet());
    // If there is a video stream?
    if(FlagIsSet(FL_THEORA))
    { // Allocate a new one and throw error if not allocated
      tdcPtr = th_decode_alloc(&tiData, tsiPtr);
      if(!tdcPtr)
        XC("Error creating theora decoder context!",
           "Identifier", IdentGet());
      // Calculate FPS of video
      fdFPS = static_cast<double>(tiData.fps_numerator) /
              static_cast<double>(tiData.fps_denominator);
      // Sanity check FPS
      if(fdFPS<1 || fdFPS>200)
        XC("Ambiguous frame rate in video!",
          "Identifier", IdentGet(), "FPS", fdFPS);
      // Make sure GPU can support texture size
      if(static_cast<GLuint>(tiData.frame_width) > cOgl->MaxTexSize() ||
         static_cast<GLuint>(tiData.frame_height) > cOgl->MaxTexSize())
        XC("The currently active graphics device does not support a "
           "texture size that would fit the video dimensions!",
           "Identifier", IdentGet(),          "Width", tiData.frame_width,
           "Height",     tiData.frame_height, "Limit", cOgl->MaxTexSize());
      // Verify colour space
      switch(GetColourSpace())
      { // Valid colour spaces
        case TH_CS_UNSPECIFIED: [[fallthrough]];  // No colour content?
        case TH_CS_ITU_REC_470M: [[fallthrough]]; // NTSC content?
        case TH_CS_ITU_REC_470BG: break;          // PAL/SECAM content?
        // Invalid colour space
        default: XC("The specified colour space is unsupported!",
                    "Identifier", IdentGet(), "ColourSpace", GetColourSpace());
      } // Verify chroma subsampling type
      switch(GetPixelFormat())
      { // Valid chroma types
        case TH_PF_420: [[fallthrough]]; // YCbCr 4:2:0
        case TH_PF_422: [[fallthrough]]; // YCbCr 4:2:2
        case TH_PF_444: break;           // YCbCr 4:4:4
        // Invalid chroma type
        default: XC("Only 420, 422 or 444 pixel format is supported!",
                    "Identifier", IdentGet(), "PixelFormat", GetPixelFormat());
      } // Set buffer id's
      stFActive = stFWaiting = stFNext = 0;
      stFFree = faData.size();
      fdVideoTime = fdDrift = 0;
      // Update frame immediately
      CISetLimit(1.0 / fdFPS);
    } // If there is an audio stream?
    if(FlagIsSet(FL_VORBIS))
    { // Make sure rate is sane
      if(viData.rate < 1 || viData.rate > 192000)
        XC("Video playback rate not valid at this time!",
           "Identifier", IdentGet(), "Rate", viData.rate);
      // Make sure channels are correct
      if(viData.channels < 1 || viData.channels > 2)
        XC("Video playback channel count of not supported!",
           "Identifier", IdentGet(), "Channels", viData.channels);
      // Initialise vorbis synthesis
      if(vorbis_synthesis_init(&vdsData, &viData))
        XC("Failed to initialise vorbis synthesis!", "Identifier", IdentGet());
      FlagSet(FL_VDINIT);
      // Initialise vorbis block
      if(vorbis_block_init(&vdsData, &vbData))
        XC("Failed to initialise vorbis block!", "Identifier", IdentGet());
      FlagSet(FL_VBINIT);
      // Calculate memory required for buffering audio and verify the size
      // since we don't know if someone's sending us dodgy bitrate values
      const double fdMem =
        UtilNearestPow2<double>(UtilMaximum(viData.bitrate_upper,
          viData.bitrate_nominal) / 8);
      if(fdMem < 0 || fdMem > 1048576)
        XC("Calculated erroneous memory size for audio buffer!",
          "Identifier", IdentGet(), "Amount", fdMem,
          "Upper", viData.bitrate_upper, "Nominal", viData.bitrate_nominal,
          "Lower", viData.bitrate_lower, "Window", viData.bitrate_window);
      // Allocate enough memory for audio buffer. We may not actually use all
      // the memory, but it is better than putting the alloc in the decoder
      // tick. We can reuse the 'Memory' class from the 'AsyncLoader' class.
      MemResize(static_cast<size_t>(fdMem));
      // Show what we allocated
      cLog->LogDebugExSafe("Video pre-allocated $ bytes for audio decoder.",
        MemSize());
      // Have video stream?
      if(FlagIsSet(FL_THEORA))
      { // Set maximum drift
        fdMaxDrift = cVideos->fdMaxDrift;
        // Save a negated version of the drift too
        fdMaxDriftNeg = -fdMaxDrift;
      } // Reset audio position and drift
      fdAudioTime = fdDrift = 0.0;
    } // Log success
    cLog->LogInfoExSafe("Video loaded '$' successfully.", IdentGet());
    // Log debug inforation
    cLog->LogDebugExSafe(
      "- Version: $$.$.$.\n"         "- Serial: $ <0x$$$>.\n"
      "- Viewable size: $x$ <$>.\n"  "- Actual size: $x$ <$>.\n"
      "- Aspect ratio: $:$.\n"       "- Origin: $x$.\n"
      "- Pixel format: $ <$>.\n"     "- Colour space: $ <$>.\n"
      "- Frame rate: $ fps.\n"       "- Version: $.\n"
      "- Audio channels: $.\n"       "- Frequency: $ ($Hz).\n"
      "- Target bit rate: $ ($).\n"  "- Upper bit rate: $ ($).\n"
      "- Nominal bit rate: $ ($).\n" "- Lower bit rate: $ ($).\n"
      "- Bit window: $ ($).",
      fixed, static_cast<int>(tiData.version_major),
        static_cast<int>(tiData.version_minor),
        static_cast<int>(tiData.version_subminor),
      ostsTheora.serialno, hex, ostsTheora.serialno, dec,
      tiData.pic_width, tiData.pic_height,
        StrFromRatio(tiData.pic_width, tiData.pic_height),
      tiData.frame_width, tiData.frame_height,
        StrFromRatio(tiData.frame_width, tiData.frame_height),
      tiData.aspect_numerator, tiData.aspect_denominator,
      tiData.pic_x, tiData.pic_y,
      PixelFormatToString(GetPixelFormat()), GetPixelFormat(),
      ColourSpaceToString(GetColourSpace()), GetColourSpace(), fdFPS,
      viData.version,
      viData.channels,
      viData.rate, StrToGrouped(viData.rate),
      tiData.target_bitrate, StrToBits(tiData.target_bitrate),
      viData.bitrate_upper, StrToBits(viData.bitrate_upper),
      viData.bitrate_nominal, StrToBits(viData.bitrate_nominal),
      viData.bitrate_lower, StrToBits(viData.bitrate_lower),
      viData.bitrate_window, StrToBits(viData.bitrate_window));
    // Set stopped
    FlagSet(FL_STOP);
  }
  /* -- Reinitialise frame buffer object and texture ----------------------- */
  void ReInitDisplayOutput(void) { FboReInit(); InitTexture(); }
  /* -- De-initialise texture and frame buffer object ---------------------- */
  void DeInitDisplayOutput(void) { DeInitTexture(); FboDeInit(); }
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
  /* -- Blit specific triangle --------------------------------------------- */
  void BlitTri(const size_t stTId) { FboActive()->FboBlitTri(*this, stTId); }
  /* -- Blit quad ---------------------------------------------------------- */
  void Blit(void) { FboActive()->FboBlit(*this); }
  /* -- Upload the texture -------read ------------------------------------- */
  void Render(void)
  { // Try to lock and return if failed or no frames waiting
    const UniqueLock ulWaitForProcessing{ mUpload, try_to_lock };
    if(!ulWaitForProcessing.owns_lock() || !stFWaiting) return;
    // Get frame
    Frame &frRef = faData[stFActive];
    // Skip ahead frames if we need to catch up with audio
    // If we should draw?
    if(frRef.bDraw)
    { // Upload texture data. This is quite safe because this data isnt
      // written to until the decoding routine thread has finished setting
      // these values.
      for(auto &aP : frRef.aP)
      { // Bind the texture for this colour component
        cOgl->BindTexture(uiaYCbCr[aP.stI]);
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
      FboResetCache(uiaYCbCr[0], 0, shProgram->GetProgram());
      // Commit the Y setup and configure the U and V setup
      FboFinishAndReset(uiaYCbCr[1], 1, shProgram->GetProgram());
      FboFinishAndReset(uiaYCbCr[2], 2, shProgram->GetProgram());
      // Blit the YCbCr multi-texture into the fbo
      FboBlit(fboYCbCr, uiaYCbCr[2], 2, shProgram);
      // Commit the V texture and send everything to fbo list for rendering
      FboFinishAndRender();
      // No need to update again until decoder thread rendered another frame
      frRef.bDraw = false;
    } // One less buffer to wait
    --stFWaiting;
    ++stFFree;
    stFActive = (stFActive + 1) % faData.size();
  }
  /* -- Video is playing? -------------------------------------------------- */
  bool IsPlaying(void) const { return tThread.ThreadIsRunning(); }
  /* -- DeInitialise audio ouput (because re-initialising) ----------------- */
  void DeInitAudio(void)
  { // Return if there is no audio in this video
    if(FlagIsClear(FL_VORBIS)) return;
    // Pause the video from re-initialisation
    Pause(UB_REINIT);
    // Audio buffers are empty
    fdAudioBuffer = 0.0;
    // De-initialise the OpenAL segment
    StopAudioAndUnloadBuffers();
  }
  /* -- ReInitialise audio (because audio is restarting) ------------------- */
  void ReInitAudio(void)
  { // Return if there is no audio in this video
    if(FlagIsClear(FL_VORBIS)) return;
    // If there is an audio stream and we're re-initialising then resume play
    Play(UB_REINIT);
  }
  /* -- Stop and unload audio buffers -------------------------------------- */
  void StopAudioAndUnloadBuffers(void)
  { // Ignore if no source or no vorbis stream
    if(!sCptr) return;
    // Stop from playing so all buffers are unqueued and wait for stop
    // then unqueue and delete the buffer
    sCptr->StopUnQueueAndDeleteAllBuffers();
    // Unlock the source so the source manager can recycle it
    sCptr->Unlock();
    sCptr = nullptr;
  }
  /* -- Do pause video ----------------------------------------------------- */
  void Pause(const Unblock ubNewReason = UB_PAUSE)
  { // Make sure playing flag is removed
    FlagClear(FL_PLAY);
    // Inform the thread to exit
    InformExit(ubNewReason);
  }
  /* -- Stop video and free everything ------------------------------------- */
  void Stop(const Unblock ubNewReason = UB_STOP)
  { // Ignore if already stopped
    if(FlagIsSet(FL_STOP)) return;
    FlagSet(FL_STOP);
    // Pause playback and synchronise
    Pause(ubNewReason);
    // Set the reason for stopping
    ubReason = ubNewReason;
    // De-initialise the OpenGL segment
    if(FlagIsSet(FL_GLINIT)) { DeInitTexture(); FboDeInit(); }
    // De-initialise the OpenAL segment
    StopAudioAndUnloadBuffers();
    // Audio buffers are empty
    fdAudioBuffer = 0.0;
    // Reset buffer status
    stFActive = stFNext = stFWaiting = stFFree = stLoop = 0;
    // Rewind data stream
    DoRewindAndReset();
    // Log that the video was stopped
    cLog->LogDebugExSafe("Video '$' stopped with reason $.",
      IdentGet(), ubNewReason);
  }
  /* -- Advance a frame ---------------------------------------------------- */
  void Advance(void)
  { // If not playing and no there is a video stream?
    if(tThread.ThreadIsExited() && FlagIsSet(FL_THEORA))
      // Run video manager until we render a frame or thread termination
      while(tThread.ThreadShouldNotExit() && !VideoHandle() && !stFWaiting);
  }
  /* -- Awaken the video --------------------------------------------------- */
  void Awaken(void)
  { // If theres a video segment and GL portion is initialised?
    if(FlagIsSetAndClear(FL_THEORA, FL_GLINIT))
    { // OpenGL output initialised
      FlagSet(FL_GLINIT);
      // Init fbo
      FboInit(IdentGet(), static_cast<GLsizei>(tiData.pic_width),
                          static_cast<GLsizei>(tiData.pic_height));
      FboSetOrtho(0, 0, 0, 0);
      FboSetTransparency(true);
      FboItemSetTexCoord(0, 0, 1, 1);
      // Clear the fbo, initially transparent
      FboSetClearColour(0, 0, 0, 0);
      FboSetClear(false);
      // Only 2 triangles and 3 commands are needed so reserve the memory
      if(!FboReserve(2, 3))
        cLog->LogWarningExSafe(
          "Video failed to reserve memory for fbo lists.");
      // We must discard the extra garbage from the ogg video. We can do that
      // with the GPU very easily by altering texture coords!
      fboYCbCr.FboItemSetTexCoord(
        static_cast<GLfloat>(tiData.pic_x) / tiData.frame_width,
        static_cast<GLfloat>(tiData.pic_y +
          tiData.pic_height) / tiData.frame_height,
        static_cast<GLfloat>(tiData.pic_x +
          tiData.pic_width) / tiData.frame_width,
        static_cast<GLfloat>(tiData.pic_y) / tiData.frame_height);
      // Init texture
      InitTexture();
    } // If theres a audio segment and AL portion is initialised?
    if(FlagIsSet(FL_VORBIS) && !sCptr)
    { // Compare number of channels in file to set appropriate format. This is
      // here and not at the files init stage as it handles re-inits too and
      // the FP supported audio format flag have changed.
      switch(viData.channels)
      { // 1 channel mono?
        case 1: eFormat = cOal->Have32FPPB() ?
                  AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_MONO16;
                break;
        // 2 channel stereo?
        case 2: eFormat = cOal->Have32FPPB() ?
                  AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_STEREO16;
                break;
        // Unknown channel count. Problem should already be handled at init.
        default: cLog->LogWarningExSafe("Video '$' audio playback failed. "
          "Invalid channel count of $!", IdentGet(), viData.channels); return;
      } // Get a new sound source and if we got it update volume and return
      sCptr = GetSource();
      if(sCptr) CommitVolume();
      // Tell log
      else cLog->LogWarningExSafe(
        "Video '$' audio playback failed. Out of sources!", IdentGet());
    }
  }
  /* -- Play video --------------------------------------------------------- */
  void Play(const Unblock ubNewReason = UB_PLAY)
  { // If playing flag is already set?
    if(FlagIsSet(FL_PLAY))
    { // Thread is not started?
      if(tThread.ThreadIsExited())
      { // Set reason for playing
        ubReason = ubNewReason;
        // Next frame can show immediately
        CISync();
        // Thread is stopped? Just start it again
        tThread.ThreadStart(this);
        // Log that the video was restarted
        cLog->LogDebugExSafe("Video '$' restarted with reason $.",
          IdentGet(), ubNewReason);
      } // Log that the command was ignored
      else cLog->LogWarningExSafe(
        "Video '$' play request with reason $ ignored!",
        IdentGet(), ubNewReason);
      // Done
      return;
    } // Check that OpenGL and OpenAL is initialised
    Awaken();
    // Set reason for playing
    ubReason = ubNewReason;
    // Next frame can show immediately
    CISync();
    // Set playing flag
    FlagSet(FL_PLAY);
    // Start decoding if we're still playing
    tThread.ThreadStart(this);
    // Log that the video was started
    cLog->LogDebugExSafe("Video '$' playing with reason $!",
      IdentGet(), ubNewReason);
  }
  /* -- Rewind video ------------------------------------------------------- */
  void Rewind(void)
  { // Ignore if already rewound
    if(iVideoGranulePos <= 0) return;
    // Rewind video to start
    DoRewindAndReset();
    // Log that the video was rewound
    cLog->LogDebugExSafe("Video '$' rewound!", IdentGet());
  }
  /* -- (De)Initialise video ouput ----------------------------------------- */
  void DeInitTexture(void)
  { // Delete the component textures
    GLL(cOgl->SetDeleteTextures(uiaYCbCr),
      "Failed to delete $ texture components", uiaYCbCr.size());
    // Clear texture names
    uiaYCbCr.fill(0);
  }
  /* -- Generate texture for specified video component --------------------- */
  void ConfigTexture(const GLuint uiTU, const int iW, const int iH)
  { // Get texture id
    GLuint &uiT = uiaYCbCr[uiTU];
    // Bind texture
    GL(cOgl->BindTexture(uiT), "Failed to bind video component texture!",
      "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    // Allocate VRAM for texture
    GL(cOgl->UploadTexture(0, iW, iH, GL_R8, GL_RED, nullptr),
      "Failed to reserve texture memory for image component!",
        "Identifier", IdentGet(), "Index",  uiTU, "Texture", uiT,
        "Width",      iW,         "Height", iH);
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
    FboSetFilterCommit(bState ? 3 : 0);
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
    { shProgram = GetKeyed() ?
        &cShaderCore->sh3DYCbCrK : &cShaderCore->sh3DYCbCr; }
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
    { // YCbCr420p : Y = Full size; Cb/Cr = Half width and height
      case TH_PF_420: iWDIV = iHDIV = 2; break;
      // YCbCr422p : Y = Full size; Cb/Cr = Half width and full height
      case TH_PF_422: iWDIV = 2; iHDIV = 1; break;
      // YCbCr444p : Y = Full size; Cb/Cr = Full width and height
      case TH_PF_444: iWDIV = iHDIV = 1; break;
      // Unknown so throw error
      default: XC("Unsupported or unknown theora pixel format!",
        "Identifier", IdentGet(), "PixelFormat", GetPixelFormat());
    } // Create textures for Y/Cb/Cr multitexture
    GL(cOgl->CreateTextures(uiaYCbCr),
      "Failed to create texture components for YCbCr texture!",
      "Identifier", IdentGet(), "Components", uiaYCbCr.size());
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
  /* -- Load video from memory asynchronously ------------------------------ */
  void InitAsyncArray(lua_State*const lS)
  { // Need 5 parameters (class pointer was already pushed onto the stack);
    LuaUtilCheckParams(lS, 6);
    // Get and check parameters
    const string strF{ LuaUtilGetCppStrNE(lS, 1, "Identifier") };
    Asset &aData = *LuaUtilGetPtr<Asset>(lS, 2, "Data");
    LuaUtilCheckFuncs(lS, 3, "ErrorFunc", 4, "ProgressFunc", 5, "SuccessFunc");
    // Set base parameters
    AsyncInitArray(lS, strF, "videoarray", StdMove(aData));
  }
  /* -- Load stream from file asynchronously ------------------------------- */
  void InitAsyncFile(lua_State*const lS)
  { // Need 4 parameters (class pointer was already pushed onto the stack);
    LuaUtilCheckParams(lS, 5);
    // Get and check parameters
    const string strF{ LuaUtilGetCppFile(lS, 1, "File") };
    LuaUtilCheckFuncs(lS, 2, "ErrorFunc", 3, "ProgressFunc", 4, "SuccessFunc");
    // Load sample from file asynchronously
    AsyncInitFile(lS, strF, "videofile");
  }
  /* -- Destructor --------------------------------------------------------- */
  ~Video(void)
  { // Stop any pending async operations
    AsyncCancel();
    // Remove the registration now so it is no longer polled
    ICHelperVideo::CollectorUnregister();
    // Prevent more events being generated
    LuaEvtDeInit();
    // Make sure the thread is stopped
    InformExit(UB_FINISH);
    // Stop and unload audio buffers
    StopAudioAndUnloadBuffers();
    // Deinit texture and reset parameters
    if(FlagIsSet(FL_GLINIT)) { DeInitTexture(); FboDeInit(); }
    // Clear theora and vorbis internal structures
    if(tdcPtr) th_decode_free(tdcPtr);
    if(tsiPtr) th_setup_free(tsiPtr);
    if(FlagIsSet(FL_STINIT)) ogg_stream_clear(&ostsTheora);
    if(FlagIsSet(FL_SVINIT)) ogg_stream_clear(&ostsVorbis);
    if(FlagIsSet(FL_VBINIT)) vorbis_block_clear(&vbData);
    if(FlagIsSet(FL_VDINIT)) vorbis_dsp_clear(&vdsData);
    if(FlagIsSet(FL_OSINIT)) ogg_sync_clear(&osysData);
    if(FlagIsSet(FL_VCINIT)) vorbis_comment_clear(&vcData);
    if(FlagIsSet(FL_VIINIT)) vorbis_info_clear(&viData);
    if(FlagIsSet(FL_TCINIT)) th_comment_clear(&tcData);
    if(FlagIsSet(FL_TIINIT)) th_info_clear(&tiData);
  }
  /* -- Constructor -------------------------------------------------------- */
  Video(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperVideo{ cVideos, this },    // Initialise collector class
    Fbo{ GL_RGB8, false },             // Create unregistered opaque surface
    AsyncLoaderVideo{ *this, this,     // Initialise async loader
      EMC_MP_VIDEO },                  // ...video event code
    LuaEvtSlave{ this,                 // Initialise event handler
      EMC_VID_EVENT },                 // ...event handler code
    VideoFlags{ FL_NONE },             // No video flags just yet
    tThread{ "video", STP_HIGH,        // Initialise thread with high priority
      bind(&Video::VideoThreadMain,    // ...preset callback
        this, _1) },                   // ...class as parameter
    ubReason(UB_STANDBY),              // Initially set to blocked
    stLoop(0),                         // Initialise no loops remaining
    fdDrift(0.0),                      // Initialise drift time
    fdMaxDriftNeg(0.0),                // Initialise maximum drift time
    fdMaxDrift(0.0),                   // Initialise maximum drift time
    osysData{},                        // Clear ogg sync state
    opgData{},                         // Clear ogg page data
    opkData{},                         // Clear ogg packet data
    lIOBuf(cVideos->lBufferSize),      // Initialise buffer length
    stIOBuf(static_cast                // Make a size_t version too so we...
      <size_t>(lIOBuf)),               // ...don't have to cast it
    ostsTheora{},                      // Clear ogg stream state
    tiData{},                          // Clear Theora info struct
    tcData{},                          // Clear Theora comment struct
    tsiPtr(nullptr),                   // Initialise Theora setup information
    tdcPtr(nullptr),                   // Initialise Theora decoder context
    tybData{},                         // Clear Theora pixel colour buffers
    iVideoGranulePos(0),               // Initialise granule position
    fdVideoTime(0),                    // Initialise position
    fdFPS(0),                          // Initialise fps
    uiVideoFrames(0),                  // Initialise frames processed
    uiVideoFramesLost(0),              // Initialise frames lost
    stFActive(0),                      // initialise active frame id
    stFNext(0),                        // Initialise next frame id
    stFWaiting(0),                     // Initialise frames waiting
    stFFree{0},                        // Initialise free frames
    ostsVorbis{},                      // Clear Vorbis stream status data
    viData{},                          // Clear Vorbis decoder data
    vcData{},                          // Clear Vorbis comment data
    vdsData{},                         // Clear Vorbis DSP state data
    vbData{},                          // Clear Vorbis decoder block data
    fdAudioTime(0.0),                  // Initialise audio position
    fdAudioBuffer(0.0),                // Initialise audio buffer length
    fAudioVolume(1.0f),                // Initialise audio volume
    shProgram(nullptr),                // Initialise pointer to Shader used
    sCptr(nullptr),                    // Initialise pointer to Source used
    eFormat(AL_NONE)                   // Initialise audio format type
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
  }, "TH_PF_UNSUPPORTED" },            // End of pixel format strings list
  lBufferSize{0},                      // Buffer size initialised by cvar
  fdMaxDrift{0.0}                      // Max drift initialised by cvar
)/* == Reinit textures (after engine thread shutdown) ====================== */
static void VideoReInitTextures(void)
{ // Ignore if no videos otherwise re-initialise ogl textures on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos re-initialising $ video surfaces...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vCptr : *cVideos) vCptr->ReInitDisplayOutput();
  cLog->LogDebugExSafe("Videos re-initialised $ video surfaces!",
    cVideos->CollectorCountUnsafe());
}
/* == De-init video textures (after thread shutdown) ======================= */
static void VideoDeInitTextures(void)
{ // Ignore if no videos otherwise de-initialise ogl textures on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos de-initialising $ video surfaces...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vCptr : *cVideos) vCptr->DeInitDisplayOutput();
  cLog->LogDebugExSafe("Videos de-initialised $ video surfaces!",
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
  for(Video*const vCptr : *cVideos) vCptr->ReInitAudio();
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
/* == Set drift length maximum ============================================= */
static CVarReturn VideoSetMaximumDrift(const double fdMax)
  { return CVarSimpleSetIntNLG(cVideos->fdMaxDrift, fdMax, 0.01, 1.00); }
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
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
