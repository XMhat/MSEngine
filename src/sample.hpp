/* == SAMPLE.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module defines a class that can load and play sound files      ## */
/* ## using OpenAL's 3D positioning functions.                            ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfSample {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfPcm;                 // Using pcm namespace
using namespace IfSource;              // Using source namespace
/* -- Sample collector and member class ------------------------------------ */
BEGIN_COLLECTOR(Samples, Sample, CLHelperUnsafe)
/* -- Sample member class -------------------------------------------------- */
BEGIN_MEMBERCLASSEX(Samples, Sample, ICHelperUnsafe, /* n/a */),
  /* -- Base classes ------------------------------------------------------- */
  public ALUIntVector,                 // OpenAL buffer handle ids
  public Pcm                           // Loaded pcm data
{ /* -- Get AL buffer index from the specified physical buffer index */ public:
  template<typename IntType=ALint>IntType GetBufferInt(const ALenum eP,
    const size_t stId=0) const
  { // Hold state
    ALint iV;
    // Store state
    AL(cOal->GetBufferInt((*this)[stId], eP, &iV),
      "Get buffer integer failed!",
      "Identifier", IdentGet(), "Param", eP, "Index", stId);
    // Return state
    return static_cast<IntType>(iV);
  }
  /* -- Get buffer frequency ----------------------------------------------- */
  ALsizei GetFrequency(void) const
    { return GetBufferInt<ALsizei>(AL_FREQUENCY); }
  ALsizei GetBits(void) const
    { return GetBufferInt<ALsizei>(AL_BITS); }
  ALsizei GetChannels(void) const
    { return GetBufferInt<ALsizei>(AL_CHANNELS); }
  ALsizei GetSize(void) const
    { return GetBufferInt<ALsizei>(AL_SIZE); }
  ALdouble GetDuration(void) const
    { return (static_cast<ALdouble>(GetSize()) * 8 /
        (GetChannels() * GetBits())) / GetFrequency(); }
  /* -- Unload buffers ----------------------------------------------------- */
  void UnloadBuffer(void)
  { // Bail if buffers not allocated
    if(empty()) return;
    // Stop this sample from playing in its entirety
    if(const unsigned int uiStopped = Stop())
      cLog->LogDebugExSafe("Sample '$' cleared $ sources using it!",
        IdentGet(), uiStopped);
    // Delete the buffers
    ALL(cOal->DeleteBuffers(*this), "Sample '$' failed to delete $ buffers",
      IdentGet(), size());
    // Reset buffer
    clear();
  }
  /* ----------------------------------------------------------------------- */
  ALuint PrepareSource(Source &sCref, const ALuint uiBufId,
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
  ALfloat GetAdjustedGain(const ALfloat fGain)
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
        front(), fGain, -0.5f+fPan, fPitch, bLoop, bLuaManaged),
      PrepareSource(*srRight,          // Right chanel
        (*this)[1], fGain,  0.5f+fPan, fPitch, bLoop, bLuaManaged)
    }; // Play all the sources together
    ALL(cOal->PlaySources(aSourceIds),
      "Sample '$' failed to play stereo sources!", IdentGet());
  }
  /* ----------------------------------------------------------------------- */
  void PlayMonoSource(const ALfloat fGain, ALfloat fPan,
    const ALfloat fPitch, const bool bLoop, Source &sCref,
    const bool bLuaManaged)
  { // Play the source
    ALL(cOal->PlaySource(PrepareSource(sCref, front(),
      GetAdjustedGain(fGain), fPan, fPitch, bLoop, bLuaManaged)),
        "Sample '$' failed to play mono source!", IdentGet());
  }
  /* -- Spawn new sources in Lua ------------------------------------------- */
  void Spawn(lua_State*const lS)
  { // How many sources do we need?
    switch(size())
    { // 1? (Mono source?) Create a new mono source and if we got it? Play it!
      case 1: if(SourceGetFromLua(lS)) return;
              // Log that we could not grab a mono channel source
              cLog->LogWarningExSafe("Sample cannot get a free source "
                "for spawning '$'!", IdentGet());
              // Done
              break;
      // 2? (Stereo sources) Get the left channel and if we got it?
      case 2: if(SourceGetFromLua(lS))
              { // Get a right channel source and if we got it?
                if(SourceGetFromLua(lS)) return;
                // Log that we could not grab a right channel source
                cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for spawning '$' right channel!", IdentGet());
              } // Could not grab a left channel source?
              else
              { // Log that we could not grab a left channel source
                cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for spawning '$' left channel!", IdentGet());
              } // Done
              break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
                  "Identifier", IdentGet(),
                  "Channels",   size());
    }
  }
  /* -- Play with a pre-allocated sources by Lua --------------------------- */
  void Play(lua_State*const lS)
  { // Grab parameters
    const ALfloat fGain = GetNumLG<ALfloat>(lS, 2,  0, 1, "Gain"),
                  fPan = GetNumLG<ALfloat>(lS, 3, -1, 1, "Pan"),
                  fPitch = GetNum<ALfloat>(lS, 4, "Pitch");
    const bool bLoop = GetBool(lS, 5, "Loop");
    // How many sources do we need?
    switch(size())
    { // 1? (Mono source?) Create a new mono source and if we got it?
      case 1: if(Source*const scMono = SourceGetFromLua(lS))
              { // Play it!
                PlayMonoSource(fGain, fPan, fPitch, bLoop, *scMono, true);
              } // Could not grab a mono channel source?
              else
              { // Log that we could not grab a mono channel source
                cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for playing '$'!", IdentGet());
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
                  cLog->LogWarningExSafe("Sample cannot get a free source "
                    "for playing '$' left channel!", IdentGet());
                }
              } // Could not grab a left channel source?
              else
              { // Log that we could not grab a left channel source
                cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for playing '$' right channel!", IdentGet());
              } // Done
              break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
                  "Identifier", IdentGet(),
                  "Channels",   size());
    }
  }
  /* ----------------------------------------------------------------------- */
  void Play(const ALfloat fGain, const ALfloat fPan, const ALfloat fPitch,
    const ALuint uiLoop)
  { // How many sources do we need?
    switch(size())
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
                  cLog->LogWarningExSafe("Sample cannot get a free source "
                    "for playing '$' left channel!", IdentGet());
                }
              } // Could not grab a left channel source?
              else
              { // Log that we could not grab a left channel source
                cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for playing '$' right channel!", IdentGet());
              } // Done
              break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
                  "Identifier", IdentGet(),
                  "Channels",   size());
    }
  }
  /* == Stop the buffer ==================================================== */
  unsigned int Stop(void) const { return SourceStop(*this); }
  /* -- Load a single buffer from memory ----------------------------------- */
  void LoadSample(Pcm &pcmSrc)
  { // Allocate and generate openal buffers
    resize(pcmSrc.GetChannels());
    AL(cOal->CreateBuffers(*this), "Error creating sample buffers!",
      "Identifier", pcmSrc.IdentGet(), "Count", size());
    // Buffer the left or mono channel
    AL(cOal->BufferData(front(), pcmSrc.GetSFormat(),
      pcmSrc.aPcmL, static_cast<ALsizei>(pcmSrc.GetRate())),
        "Error buffering left channel/mono PCM audio data!",
        "Identifier", pcmSrc.IdentGet(),  "Buffer",  front(),
        "Format",     pcmSrc.GetFormat(), "MFormat", pcmSrc.GetSFormat(),
        "Rate",       pcmSrc.GetRate(),   "Size",    pcmSrc.aPcmL.Size());
    // Stereo sample?
    if(size() > 1)
    { // Buffer the right stereo channel
      AL(cOal->BufferData((*this)[1], pcmSrc.GetSFormat(),
        pcmSrc.aPcmR, static_cast<ALsizei>(pcmSrc.GetRate())),
          "Error buffering right/stereo channel PCM audio data!",
          "Identifier", pcmSrc.IdentGet(),  "Buffer",  (*this)[1],
          "Format",     pcmSrc.GetFormat(), "MFormat", pcmSrc.GetSFormat(),
          "Rate",       pcmSrc.GetRate(),   "Size",    pcmSrc.aPcmR.Size());
      // Log progress
      cLog->LogDebugExSafe(
        "Sample '$' uploaded as L:$[$] and R:$[$] at $Hz as format 0x$$.",
        pcmSrc.IdentGet(), front(), pcmSrc.aPcmL.Size(), (*this)[1],
        pcmSrc.aPcmR.Size(), pcmSrc.GetRate(), hex, pcmSrc.GetFormat());
    } // Log progress
    else cLog->LogDebugExSafe(
      "Sample '$' uploaded as $[$] at $Hz as format 0x$$.",
      pcmSrc.IdentGet(), front(), pcmSrc.aPcmL.Size(), pcmSrc.GetRate(),
      hex, pcmSrc.GetFormat());
  }
  /* -- Load a single buffer ----------------------------------------------- */
  void ReloadSample(void)
  { // If pcm sample was not loaded from disk? Just (re)load the bitmap data
    // that already should be there and should NEVER be released.
    if(IsDynamic()) LoadSample(*this);
    // Pcm sample is static?
    else
    { // It can just be reloaded from disk
      ReloadData();
      // Load the bitmap into memory
      LoadSample(*this);
      // Clear memory because we can just reload from file
      ClearData();
    } // Show what was loaded
    cLog->LogInfoExSafe("Sample loaded '$'!", IdentGet());
  }
  /* -- Init from a bitmap class ------------------------------------------- */
  void InitSample(Pcm &pcmSrc)
  { // Show filename progress
    cLog->LogDebugExSafe("Sample loading from pcm '$'[$] ($;$;$)...",
      pcmSrc.IdentGet(), pcmSrc.GetAlloc(), pcmSrc.GetRate(),
      pcmSrc.GetChannels(), pcmSrc.GetBits());
    // If source and destination pcm class are not the same?
    if(&*this != &pcmSrc)
    { // Move pcm data over. The old pcm will be unusable so guest should
      // discard it.
      SwapPcm(pcmSrc);
      // The pcm passed in the arguments is usually still allocated by LUA and
      // will still be registered, so lets put a note in the pcm sample to show
      // that this sample class has nicked the pcm sample.
      pcmSrc.IdentSet("!SAM!$!", IdentGet());
    } // Initialise
    LoadSample(*this);
    // Remove all sample data because we can just load it from file again
    // and theres no point taking up precious memory for it.
    if(IsNotDynamic()) ClearData();
  }
  /* -- Constructor -------------------------------------------------------- */
  Sample(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperSample{ *cSamples, this }  // Initialise collector class
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~Sample(void) { UnloadBuffer(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Sample)              // Supress copy constructor for safety
};/* -- End ---------------------------------------------------------------- */
END_COLLECTOR(Samples)
/* ========================================================================= */
static void StopAllSamples(void)
{ // Stop all samples from playing
  if(cSamples->empty()) return;
  for(const Sample*const sCptr : *cSamples) sCptr->Stop();
}
/* ========================================================================= */
static void DeInitAllSamples(void)
{ // Done if empty
  if(cSamples->empty()) return;
  // Re-create buffers for all the samples and log pre/post de-init
  cLog->LogDebugExSafe("Samples de-initialising $ objects...",
    cSamples->size());
  for(Sample*const sCptr : *cSamples) sCptr->UnloadBuffer();
  cLog->LogInfoExSafe("Samples de-initialised $ objects.",
    cSamples->size());
}
/* ========================================================================= */
static void ReInitAllSamples(void)
{ // Done if empty
  if(cSamples->empty()) return;
  // Re-create buffers for all the samples and log pre/post re-init
  cLog->LogDebugExSafe("Samples re-initialising $ objects...",
    cSamples->size());
  for(Sample*const sCptr : *cSamples) sCptr->ReloadSample();
  cLog->LogInfoExSafe("Samples re-initialising $ sample objects.",
    cSamples->size());
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
  if(fVolume < 0.0f || fVolume > 1.0f) return DENY;
  // Store volume (SOURCES class keeps it)
  cSources->fSVolume = fVolume;
  // Update volumes on all streams
  UpdateSampleVolume();
  // Success
  return ACCEPT;
}
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
