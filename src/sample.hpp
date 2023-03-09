/* == SAMPLE.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines a class that can load and play sound files      ## */
/* ## using OpenAL's 3D positioning functions.                            ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfSample {                   // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfPcm;                 // Using pcm interface
using namespace IfSource;              // Using source interface
/* -- Sample collector and member class ==================================== */
BEGIN_COLLECTORDUO(Samples, Sample, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -- Variables -------------------------------------------------- */ public:
  ALUIntVector     vuiBuffer;          // Buffers
  Pcm              pcmData;            // Loaded pcm data
  /* -- Get AL buffer index from the specified physical buffer index ------- */
  ALint GetBufferInt(const ALenum eP, const size_t stId=0) const
  { // Hold state
    ALint iV;
    // Store state
    AL(cOal->GetBufferInt(vuiBuffer[stId], eP, &iV),
      "Get buffer integer failed!", "Param", eP, "Id", stId);
    // Return state
    return iV;
  }
  /* -- Get buffer frequency ----------------------------------------------- */
  ALsizei GetFrequency(void) const { return GetBufferInt(AL_FREQUENCY); }
  ALsizei GetBits(void) const { return GetBufferInt(AL_BITS); }
  ALsizei GetChannels(void) const { return GetBufferInt(AL_CHANNELS); }
  ALsizei GetSize(void) const { return GetBufferInt(AL_SIZE); }
  ALdouble GetDuration(void) const
    { return (static_cast<ALdouble>(GetSize()) * 8 /
        (GetChannels() * GetBits())) / GetFrequency(); }
  /* -- Unload buffers ----------------------------------------------------- */
  void UnloadBuffer(void)
  { // Bail if buffers not allocated
    if(vuiBuffer.empty()) return;
    // Stop this sample from playing in its entirety
    if(const unsigned int uiStopped = Stop())
      LW(LH_DEBUG, "Sample '$' cleared $ sources using it!",
        pcmData.IdentGet(), uiStopped);
    // Delete the buffers
    ALL(cOal->DeleteBuffers(vuiBuffer),
      "Sample '$' failed to delete $ buffers",
        pcmData.IdentGet(), vuiBuffer.size());
    // Reset buffer
    vuiBuffer.clear();
  }
  /* ----------------------------------------------------------------------- */
  GLuint PrepareSource(Source &sCref, const ALuint uiBufId,
    const ALfloat fGain, const ALfloat fPan, const ALfloat fPitch,
    const bool bLoop, const bool bLuaManaged)
  { // Set parameters
    sCref.SetBuffer(static_cast<ALint>(uiBufId));
    sCref.SetRelative(true);
    sCref.SetPosition(fPan, 0.0f, -sqrtf(1.0f-fPan*fPan));
    sCref.SetLooping(bLoop);
    sCref.SetPitch(fPitch);
    sCref.SetGain(fGain);
    sCref.SetExternal(bLuaManaged);
    // Return source id
    return sCref.GetSource();
  }
  /* ----------------------------------------------------------------------- */
  GLfloat GetAdjustedGain(const ALfloat fGain)
    { return fGain * cSources->fSVolume * cSources->fGVolume; }
  /* ----------------------------------------------------------------------- */
  void PlayStereoSource(ALfloat fGain, ALfloat fPan,  const ALfloat fPitch,
    const bool bLoop, Source*const srLeft, Source*const srRight,
    const bool bLuaManaged)
  { // Adjust gain from global volumes
    fGain *= GetAdjustedGain(fGain);
    // Half the pan
    fPan *= 0.5f;
    // Prepare sources
    const array<const ALuint,2> aSourceIds{
      PrepareSource(*srLeft,           // Left channel
        vuiBuffer[0], fGain, -0.5f+fPan, fPitch, bLoop, bLuaManaged),
      PrepareSource(*srRight,          // Right chanel
        vuiBuffer[1], fGain,  0.5f+fPan, fPitch, bLoop, bLuaManaged)
    }; // Play all the sources together
    ALL(cOal->PlaySources(aSourceIds),
      "Sample '$' failed to play stereo sources!", pcmData.IdentGet());
  }
  /* ----------------------------------------------------------------------- */
  void PlayMonoSource(const ALfloat fGain, ALfloat fPan,
    const ALfloat fPitch, const bool bLoop, Source &sCref,
    const bool bLuaManaged)
  { // Play the source
    ALL(cOal->PlaySource(PrepareSource(sCref, vuiBuffer[0],
      GetAdjustedGain(fGain), fPan, fPitch, bLoop, bLuaManaged)),
        "Sample '$' failed to play mono source!", pcmData.IdentGet());
  }
  /* -- Spawn new sources in Lua ------------------------------------------- */
  void Spawn(lua_State*const lS)
  { // How many sources do we need?
    switch(vuiBuffer.size())
    { // 1? (Mono source?) Create a new mono source and if we got it? Play it!
      case 1: if(SourceGetFromLua(lS)) return;
              // Log that we could not grab a mono channel source
              LW(LH_WARNING, "Sample cannot get a free source "
                "for spawning '$'!", pcmData.IdentGet());
              // Done
              break;
      // 2? (Stereo sources) Get the left channel and if we got it?
      case 2: if(SourceGetFromLua(lS))
              { // Get a right channel source and if we got it?
                if(SourceGetFromLua(lS)) return;
                // Log that we could not grab a right channel source
                LW(LH_WARNING, "Sample cannot get a free source "
                  "for spawning '$' right channel!", pcmData.IdentGet());
              } // Could not grab a left channel source?
              else
              { // Log that we could not grab a left channel source
                LW(LH_WARNING, "Sample cannot get a free source "
                  "for spawning '$' left channel!", pcmData.IdentGet());
              } // Done
              break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
                  "Identifier", pcmData.IdentGet(),
                  "Channels",   vuiBuffer.size());
    }
  }
  /* -- Play with a pre-allocated sources by Lua --------------------------- */
  void Play(lua_State*const lS)
  { // Grab parameters
    const ALfloat fGain = GetNumLG<ALfloat>(lS, 2,  0, 1, "Gain");
    const ALfloat fPan = GetNumLG<ALfloat>(lS, 3, -1, 1, "Pan");
    const ALfloat fPitch = GetNum<ALfloat>(lS, 4, "Pitch");
    const bool bLoop = GetBool(lS, 5, "Loop");
    // How many sources do we need?
    switch(vuiBuffer.size())
    { // 1? (Mono source?) Create a new mono source and if we got it?
      case 1: if(Source*const scMono = SourceGetFromLua(lS))
              { // Play it!
                PlayMonoSource(fGain, fPan, fPitch, bLoop, *scMono, true);
              } // Could not grab a mono channel source?
              else
              { // Log that we could not grab a mono channel source
                LW(LH_WARNING, "Sample cannot get a free source "
                  "for playing '$'!", pcmData.IdentGet());
              } // Done
              break;
      // 2? (Stereo sources) Get the left channel and if we got it?
      case 2: if(Source*const sLeft = SourceGetFromLua(lS))
              { // Get a right channel source and if we got it?
                if(Source*const sRight = SourceGetFromLua(lS))
                { // Play the sources
                  PlayStereoSource(fGain, fPan, fPitch, bLoop,
                    sLeft, sRight, true);
                } // Could not grab a right channel source?
                else
                { // Log that we could not grab a right channel source
                  LW(LH_WARNING, "Sample cannot get a free source "
                    "for playing '$' left channel!", pcmData.IdentGet());
                }
              } // Could not grab a left channel source?
              else
              { // Log that we could not grab a left channel source
                LW(LH_WARNING, "Sample cannot get a free source "
                  "for playing '$' right channel!", pcmData.IdentGet());
              } // Done
              break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
                  "Identifier", pcmData.IdentGet(),
                  "Channels",   vuiBuffer.size());
    }
  }
  /* ----------------------------------------------------------------------- */
  void Play(const ALfloat fGain, const ALfloat fPan, const ALfloat fPitch,
    const ALuint uiLoop)
  { // How many sources do we need?
    switch(vuiBuffer.size())
    { // 1? (Mono source?) Create a new mono source and if we got it? Play it!
      case 1: if(Source*const scMono = GetSource())
                PlayMonoSource(fGain, fPan, fPitch, uiLoop, *scMono, false);
              // Done
              break;
      // 2? (Stereo sources) Get the left channel and if we got it?
      case 2: if(Source*const sLeft = GetSource())
              { // Get a right channel source and if we got it?
                if(Source*const sRight = GetSource())
                { // Play the sources
                  PlayStereoSource(fGain, fPan, fPitch, uiLoop,
                    sLeft, sRight, false);
                } // Could not grab a right channel source?
                else
                { // Log that we could not grab a right channel source
                  LW(LH_WARNING, "Sample cannot get a free source "
                    "for playing '$' left channel!", pcmData.IdentGet());
                }
              } // Could not grab a left channel source?
              else
              { // Log that we could not grab a left channel source
                LW(LH_WARNING, "Sample cannot get a free source "
                  "for playing '$' right channel!", pcmData.IdentGet());
              } // Done
              break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
                  "Identifier", pcmData.IdentGet(),
                  "Channels",   vuiBuffer.size());
    }
  }
  /* == Stop the buffer ==================================================== */
  unsigned int Stop(void) const { return SourceStop(vuiBuffer); }
  /* -- Load a single buffer from memory ----------------------------------- */
  void LoadSample(Pcm &pcmSrc)
  { // Allocate and generate openal buffers
    vuiBuffer.resize(pcmSrc.GetChannels());
    AL(cOal->CreateBuffers(vuiBuffer), "Error creating sample buffers!",
      "Identifier", pcmSrc.IdentGet(), "Count", vuiBuffer.size());
    // Buffer the left or mono channel
    AL(cOal->BufferData(vuiBuffer[0], pcmSrc.GetSFormat(),
      pcmSrc.GetData(), static_cast<ALsizei>(pcmSrc.GetRate())),
        "Error buffering left channel/mono PCM audio data!",
        "Identifier", pcmSrc.IdentGet(),     "Buffer",  vuiBuffer[0],
        "Format",     pcmSrc.GetFormat(), "MFormat", pcmSrc.GetSFormat(),
        "Rate",       pcmSrc.GetRate(),   "Size",    pcmSrc.GetUsage());
    // Stereo sample?
    if(vuiBuffer.size() > 1)
    { // Buffer the right stereo channel
      AL(cOal->BufferData(vuiBuffer[1], pcmSrc.GetSFormat(),
        pcmSrc.GetData2(), static_cast<ALsizei>(pcmSrc.GetRate())),
          "Error buffering right/stereo channel PCM audio data!",
          "Identifier", pcmSrc.IdentGet(),     "Buffer",  vuiBuffer[1],
          "Format",     pcmSrc.GetFormat(), "MFormat", pcmSrc.GetSFormat(),
          "Rate",       pcmSrc.GetRate(),   "Size",    pcmSrc.GetUsage());
      // Log progress
      LW(LH_INFO, "Sample '$' uploaded as $(L) and $(R)! (S:$;R:$;F:0x$$).",
        pcmSrc.IdentGet(), vuiBuffer[0], vuiBuffer[1], pcmSrc.GetUsage(),
        pcmSrc.GetRate(), hex, pcmSrc.GetFormat());
    } // Log progress
    else LW(LH_INFO, "Sample '$' uploaded as $! (S:$;R:$;F:0x$$).",
      pcmSrc.IdentGet(), vuiBuffer[0], pcmSrc.GetUsage(), pcmSrc.GetRate(),
      hex, pcmSrc.GetFormat());
  }
  /* -- Load a single buffer ----------------------------------------------- */
  void ReloadSample(void)
  { // If pcm sample was not loaded from disk? Just (re)load the bitmap data
    // that already should be there and should NEVER be released.
    if(pcmData.IsDynamic()) LoadSample(pcmData);
    // Pcm sample is static?
    else
    { // It can just be reloaded from disk
      pcmData.ReloadData();
      // Load the bitmap into memory
      LoadSample(pcmData);
      // Clear memory because we can just reload from file
      pcmData.ClearData();
    } // Show what was loaded
    LW(LH_INFO, "Sample loaded '$'!", pcmData.IdentGet());
  }
  /* -- Init from a bitmap class ------------------------------------------- */
  void InitSample(Pcm &pcmSrc)
  { // Show filename progress
    LW(LH_DEBUG, "Sample loading from pcm '$'[$] ($;$;$)...",
      pcmSrc.IdentGet(), pcmSrc.GetUsage(), pcmSrc.GetRate(),
      pcmSrc.GetChannels(), pcmSrc.GetBits());
    // If source and destination pcm class are not the same?
    if(&pcmData != &pcmSrc)
    { // Move pcm data over. The old pcm will be unusable so guest should
      // discard it.
      pcmData.SwapPcm(pcmSrc);
      // The pcm passed in the arguments is usually still allocated by LUA and
      // will still be registered, so lets put a note in the pcm sample to show
      // that this sample class has nicked the pcm sample.
      pcmSrc.IdentSet(Format("!SAM!$!", pcmData.IdentGet()));
    } // Initialise
    LoadSample(pcmData);
    // Remove all sample data because we can just load it from file again
    // and theres no point taking up precious memory for it.
    if(!pcmData.IsDynamic()) pcmData.ClearData();
  }
  /* -- Constructor -------------------------------------------------------- */
  Sample(void) :
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperSample{ *cSamples, this }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~Sample(void) { UnloadBuffer(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Sample);             // Supress copy constructor for safety
};/* -- End ---------------------------------------------------------------- */
END_COLLECTOR(Samples);
/* ========================================================================= */
static void StopAllSamples(void)
{ // Stop all samples from playing
  if(cSamples->empty()) return;
  for(Sample*const sCptr : *cSamples) sCptr->Stop();
}
/* ========================================================================= */
static void DeInitAllSamples(void)
{ // Done if empty
  if(cSamples->empty()) return;
  // Re-create buffers for all the samples and log pre/post de-init
  LW(LH_DEBUG, "Samples de-initialising $ objects...", cSamples->size());
  for(Sample*const sCptr : *cSamples) sCptr->UnloadBuffer();
  LW(LH_INFO, "Samples de-initialised $ objects.", cSamples->size());
}
/* ========================================================================= */
static void ReInitAllSamples(void)
{ // Done if empty
  if(cSamples->empty()) return;
  // Re-create buffers for all the samples and log pre/post re-init
  LW(LH_DEBUG, "Samples re-initialising $ objects...", cSamples->size());
  for(Sample*const sCptr : *cSamples) sCptr->ReloadSample();
  LW(LH_INFO, "Samples re-initialising $ sample objects.", cSamples->size());
}
/* == Update all streams base volume======================================== */
static void UpdateSampleVolume(void)
{ // Lock source list so it cannot be modified
  const LockGuard mLock{ cSources->CollectorGetMutex() };
  // Walk through sources
  for(Source*const sCptr : *cSources)
  { // Get class
    const Source &oCref = *sCptr;
    // If it is locked then its a sample so ignore it
    if(oCref.GetExternal()) continue;
    // Set new sample volume
    oCref.SetGain(cSources->fGVolume * cSources->fSVolume);
  }
}
/* == Set all streams base volume ========================================== */
static CVarReturn SetSampleVolume(const ALfloat fVolume)
{ // Ignore if invalid value
  if(fVolume < 0 || fVolume > 1) return DENY;
  // Store volume (SOURCES class keeps it)
  cSources->fSVolume = fVolume;
  // Update volumes on all streams
  UpdateSampleVolume();
  // Success
  return ACCEPT;
}
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
