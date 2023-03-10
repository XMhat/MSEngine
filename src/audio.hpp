/* == AUDIO.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This the file handles audio management of sources and streams       ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfAudio {                    // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfSample;              // Using sample interface
using namespace IfVideo;               // Using video interface
/* == Typedefs ============================================================= */
BUILD_FLAGS(Audio,                     // Audio flags classes
  /* ----------------------------------------------------------------------- */
  // No settings?                      Audio system is resetting?
  AF_NONE                {0x00000000}, AF_REINIT              {0x00000001}
);/* ======================================================================= */
static class Audio final :             // Audio manager class
  /* -- Base classes ------------------------------------------------------- */
  private IHelper,                     // Initialisation helper class
  public AudioFlags                    // Audio flags
{ /* -------------------------------------------------------------- */ private:
  Thread           tThread;            // Thread monitor
  ClkTimePoint     tpNextCheck;        // Next check for hardware changes
  SafeClkDuration  cdCheckRate;        // Check rate
  SafeClkDuration  cdThreadDelay;      // Thread sleep time
  /* -- Devices ------------------------------------------------------------ */
  StrVector        dlPBDevices;        // list of playback devices
  StrVector        dlCTDevices;        // list of capture devices
  /* -- ReInit requested --------------------------------------------------- */
  void OnReInit(const EvtMain::Cell &)
  { // Capture exceptions
    try
    { // Log status
      LW(LH_DEBUG, "Audio class re-initialising...");
      // De-Init thread
      ThreadDeInit();
      // Unload all buffers for streams and samples and destroy all sources
      StreamDeInit();
      DeInitAllSamples();
      VideoDeInit();
      cSources->CollectorDestroyUnsafe();
      // Deinit and reinit AL context
      DeInitContext();
      InitContext();
      // Initialise volumes
      SetGlobalVolume(cCVars->GetInternalSafe<ALfloat>(AUD_VOL));
      SetSampleVolume(cCVars->GetInternalSafe<ALfloat>(AUD_SAMVOL));
      StreamSetVolume(cCVars->GetInternalSafe<ALfloat>(AUD_STRVOL));
      VideoSetVolume(cCVars->GetInternalSafe<ALfloat>(AUD_FMVVOL));
      // Re-create all buffers for streams and samples
      ReInitAllSamples();
      StreamReInit();
      VideoReInit();
      // Init monitoring thread
      InitThread();
      // Log status
      LW(LH_INFO, "Audio class re-initialised successfully.");
    } // We don't want LUA to hard break really.
    catch(const exception &E)
    { // Log the exception first
      LW(LH_ERROR, "Audio re-init exception: $", E.what());
      // Reset next thread check time
      ResetCheckTime();
    } // Remove re-initialisation flag
    FlagClear(AF_REINIT);
  }
  /* -- Reset timer interval ----------------------------------------------- */
  void ResetCheckTime(void)
    { tpNextCheck = cmHiRes.GetTime() + cdCheckRate.load(); }
  /* -- Thread main function ----------------------------------------------- */
  int AudioThreadMain(Thread &) try
  { // Enumerate...
    for(ResetCheckTime();              // Reset device list check time
        tThread.ThreadShouldNotExit(); // Enumerate until thread exit signalled
        TimerSuspend(cdThreadDelay))   // Suspend thread for specified time
    { // Manage all streams audio.
      StreamManage();
      // Verify the hardware setup and reset if there are any descreprencies.
      // If there were no descreprencies detected then loop again.
      if(Verify()) continue;
      // Put in infinite loop and wait for the reinit function to request
      // termination of this thread
      while(tThread.ThreadShouldNotExit()) TimerSuspend(milliseconds(100));
      // Thread terminate request recieved, now break the loop.
      break;
    } // Terminate thread
    return 1;
  } // exception occured in this thread
  catch(const exception &E)
  { // Report error
    LW(LH_ERROR, "(AUDIO THREAD EXCEPTION) $", E.what());
    // Restart the thread
    return 0;
  }
  /* -- Verification of context -------------------------------------------- */
  bool Verify(void)
  { // Ignore if next check time not met
    if(cmHiRes.GetTime() < tpNextCheck) return true;
    // Number of discreprencies found
    size_t stDiscreprencies = 0;
    // Grab list of playback devices and if we found them?
    if(const char *cpList = cOal->GetNCString(cOal->eQuery))
    { // Number of new items detected
      size_t stIndex;
      // For each playback device. If there is no such device in the current
      // list, or the current device item does not equal to the newly detected
      // item, then that is a discreprency.
      for(stIndex = 0; *cpList; ++stIndex, cpList += strlen(cpList) + 1)
      { // Is index valid?
        if(stIndex >= dlPBDevices.size())
        { // Log warning and add to discreprency list
          ++stDiscreprencies;
          LW(LH_WARNING, "Audio thread discreprency $: ",
            "Device index $ over limit of $!",
              stDiscreprencies, stIndex, dlPBDevices.size());
        } // Is the device name the same
        else if(dlPBDevices[stIndex] != cpList)
        { // Log warning and add to discreprency list
          ++stDiscreprencies;
          LW(LH_WARNING, "Audio thread discreprency $: "
            "Expected device '$' at $, not '$'!",
              stDiscreprencies, dlPBDevices[stIndex], stIndex, cpList);
        }
      }
      // If the number of items in the newly detected list doesn't match
      // the number of items in the current list then that is another
      // discreprency.
      if(stIndex != dlPBDevices.size())
      { // Log warning and add to discreprency list
        ++stDiscreprencies;
        LW(LH_WARNING, "Audio thread discreprency $: "
          "Detected $ devices and had $ before!",
            stDiscreprencies, stIndex, stIndex != dlPBDevices.size());
      }
    } // No newly detected list and have items in current list? Fail!
    else if(dlPBDevices.size())
    { // Log warning and add to discreprency list
      ++stDiscreprencies;
      LW(LH_WARNING, "Audio thread discreprency $: "
        "$ new devices detected!", stDiscreprencies, dlPBDevices.size());
    } // If there are discreprencies, then we need to restart the subsystem.
    if(stDiscreprencies)
    { // Send message to log
      LW(LH_WARNING, "Audio thread detected $ hardware discreprencies!",
        stDiscreprencies);
      // Send event to reset audio
      cEvtMain->Add(EMC_AUD_REINIT);
      // Terminate this thread
      return false;
    } // Check again in a few seconds
    ResetCheckTime();
    // Audio context still valid, continue monitoring
    return true;
  }
  /* -- DeInit thread ------------------------------------------------------ */
  void ThreadDeInit(void)
  { // Stop and de-init the thread and log progress
    LW(LH_DEBUG, "Audio monitoring thread de-initialising...");
    tThread.ThreadDeInit();
    LW(LH_INFO, "Audio monitoring thread de-initialised.");
  }
  /* -- Init thread -------------------------------------------------------- */
  void InitThread(void)
  { // Initialise and start thread and log progress
    LW(LH_DEBUG, "Audio monitoring thread initialising...");
    tThread.ThreadStart(this);
    LW(LH_INFO, "Audio monitoring thread initialised (D:$;C:$)!",
      ToShortDuration(ClockDurationToDouble(cdThreadDelay)),
      ToShortDuration(ClockDurationToDouble(cdCheckRate)));
  }
  /* -- Init context ------------------------------------------------------- */
  void InitContext(void)
  { // Log that we are initialising
    LW(LH_DEBUG, "Audio subsystem initialising...");
    // Enumerate devices
    EnumeratePlaybackDevices();
    EnumerateCaptureDevices();
    // Set device and override if requested
    size_t stDevice = cCVars->GetInternalSafe<size_t>(AUD_INTERFACE);
    // Holding current device name
    string strDevice;
    // If -1 is not set (use specific device)
    if(stDevice != string::npos)
    { // Invalid device? Use default device!
      if(stDevice >= dlPBDevices.size())
      { // Log that the device id is invalid
        LW(LH_WARNING, "Audio device #$ invalid so using default!", stDevice);
        // Set to default device
        stDevice = 0;
      } // Set device
      strDevice = GetPlaybackDeviceById(stDevice).c_str();
    } // Device id setup complete
    { // Get pointer to device name
      const char*const cpDevice =
        strDevice.empty() ? nullptr : strDevice.c_str();
      // Say device being used
      LW(LH_DEBUG, "Audio trying to initialise device '$'...",
        cpDevice ? cpDevice : "<Default>");
      // Open the device
      if(!cOal->InitDevice(cpDevice))
        XC("Failed to open al device!",
           "Identifier", strDevice, "Index", stDevice);
    } // Reopen the device to disable HRTF
    if(!cCVars->GetInternalSafe<bool>(AUD_HRTF) && !cOal->DisableHRTF())
      XC("Audio failed to reconfigure al device!",
         "Identifier", strDevice, "Index", stDevice);
    // Create the context
    if(!cOal->InitContext())
      XC("Failed to create al context!",
         "Identifier", strDevice, "Index", stDevice);
    // Activate the context (Use alGetError() from now on)
    AL(cOal->SetContext(),
      "Failed to make al context current!",
      "Identifier", strDevice, "Index", stDevice);
    // Now fully initialised
    cOal->SetInitialised(true);
    // Allocate sources data
    SourceAlloc(cCVars->GetInternalSafe<ALuint>(AUD_NUMSOURCES));
    // Register engine events
    cEvtMain->Register(EMC_AUD_REINIT, bind(&Audio::OnReInit, this, _1));
    // Set parameters
    SetDistanceModel(AL_NONE);
    SetPosition(0, 0, 0);
    SetVelocity(0, 0, 0);
    SetOrientation(0, 0, 1, 0, -1, 0);
  }
  /* -- DeInitContext ------------------------------------------------------ */
  void DeInitContext(void)
  { // Clear openAL context
    cOal->DeInit();
    // Unregister engine events
    cEvtMain->Unregister(EMC_AUD_REINIT);
  }
  /* -- Enumerate capture devices ------------------------------------------ */
  void EnumerateCaptureDevices(void)
  { // Log enumerations
    LW(LH_DEBUG, "Audio enumerating capture devices...");
    // Storage for list of devices
    const char *cpList = nullptr;
    // Clear recording devices list
    dlCTDevices.clear();
    // Same rule applies, try 3 times...
    for(size_t stIndex = 0; stIndex < 3; ++stIndex, TimerSuspend())
    { // Grab list of capture devices and break if succeeded
      cpList = cOal->GetNCString(ALC_CAPTURE_DEVICE_SPECIFIER);
      if(cpList) break;
      // Report that the attempt failed
      LW(LH_WARNING,
        "Audio attempt $ failed on capture device enumeration (0x$$), "
        "retrying...", stIndex+1, hex, cOal->GetError());
    } // Capture devices found?
    if(cpList)
    { // For each capture device
      while(*cpList)
      { // Print device
        LW(LH_DEBUG, "- $: $.", GetNumCaptureDevices(), cpList);
        // Push to list
        dlCTDevices.emplace_back(cpList);
        // Jump to next item
        cpList += strlen(cpList) + 1;
      } // Until no more devices
    } // Problems, problems
    else LW(LH_ERROR, "Audio couldn't access capture devices list (0x$$).",
        hex, cOal->GetError());
    // Log enumerations
    LW(LH_INFO, "Audio found $ capture devices.", GetNumCaptureDevices());
  }
  /* -- Enumerate playback devices ----------------------------------------- */
  void EnumeratePlaybackDevices(void)
  { // Log enumerations
    LW(LH_DEBUG, "Audio enumerating playback devices...");
    // Clear playback devices list
    dlPBDevices.clear();
    // Detect if we have extended or standard query
    cOal->DetectEnumerationMethod();
    // For some reason, this call is sometimes failing, probably because
    // another process is enumerating the list, so we'll do a retry loop.
    // Try to grab list of playback devices three times before failing.
    for(size_t stIndex = 0; stIndex < 3; ++stIndex, TimerSuspend())
    { // Get list of devices and break if succeeded
      if(const char *cpList = cOal->GetNCString(cOal->eQuery))
      { // For each playback device
        while(*cpList)
        { // Print device
          LW(LH_DEBUG, "- $: $.", GetNumPlaybackDevices(), cpList);
          // Push to list
          dlPBDevices.emplace_back(cpList);
          // Jump to next item
          cpList += strlen(cpList) + 1;
          // Until no more devices
        } // Log enumerations and return
        LW(LH_INFO, "Audio found $ playback devices.",
          GetNumPlaybackDevices());
        return;
      } // Report that the attempt failed
      LW(LH_WARNING,
        "Audio attempt $ failed on playback device enumeration (0x$$), "
        "retrying...", stIndex+1, hex, cOal->GetError());
    } // Problems, problems
    LW(LH_ERROR, "Audio couldn't access playback devices list.");
  }
  /* --------------------------------------------------------------- */ public:
  CVarReturn SetAudCheckRate(const unsigned int uiTime)
    { return CVarSimpleSetIntNLG(cdCheckRate,
        milliseconds{ uiTime }, milliseconds{ 100 }, milliseconds{ 60000 }); }
  /* ----------------------------------------------------------------------- */
  CVarReturn SetAudThreadDelay(const ALuint uiTime)
    { return CVarSimpleSetIntNG(cdThreadDelay,
        milliseconds{ uiTime }, milliseconds{ 1000 }); }
  /* -- Set global volume -------------------------------------------------- */
  CVarReturn SetGlobalVolume(const ALfloat fVolume)
  { // Ignore if invalid value
    if(fVolume < 0 || fVolume > 1) return DENY;
    // Store volume
    cSources->fGVolume = fVolume;
    // Update volumes on streams and videos
    StreamCommitVolume();
    VideoCommitVolume();
    UpdateSampleVolume();
    // Done
    return ACCEPT;
  }
  /* -- Set distance model ------------------------------------------------- */
  void SetDistanceModel(const ALenum eModel) const
    { AL(cOal->SetDistanceModel(eModel),
        "Failed to set audio distance model!", "Model", eModel); }
  /* -- Set listener position ---------------------------------------------- */
  void SetPosition(const ALfloat fX, const ALfloat fY, const ALfloat fZ) const
  { // Set position
    AL(cOal->SetListenerPosition(fX, fY, fZ),
      "Failed to set audio listener position!", "X", fX, "Y", fY, "Z", fZ);
  }
  /* -- Set listener velocity ---------------------------------------------- */
  void SetVelocity(const ALfloat fX, const ALfloat fY, const ALfloat fZ) const
  { // Set position
    AL(cOal->SetListenerVelocity(fX, fY, fZ),
      "Failed to set audio velocity!", "X", fX, "Y", fY, "Z", fZ);
  }
  /* -- Set listener orientation ------------------------------------------- */
  void SetOrientation(const ALfloat fX1, const ALfloat fY1,
    const ALfloat fZ1, const ALfloat fX2, const ALfloat fY2,
    const ALfloat fZ2) const
  { // Build array
    const array<const ALfloat, 6> aOr{ fX1, fY1, fZ1, fX2, fY2, fZ2 };
    // Set orientation
    AL(cOal->SetListenerVectors(AL_ORIENTATION, aOr.data()),
      "Failed to set audio orientation!",
      "X1", fX1, "Y1", fY1, "Z1", fZ1, "X2", fX2, "Y2", fY2, "Z2", fZ2);
  }
  /* -- Get current playback device ---------------------------------------- */
  const string GetPlaybackDeviceById(const size_t stId) const
  { // bail if value out of range
    if(stId >= GetNumPlaybackDevices())
      XC("Specified audio playback device id out of range!",
        "Index", stId, "Count", GetNumPlaybackDevices());
    // Return device
    return dlPBDevices[stId];
  }
  /* -- Return devices ----------------------------------------------------- */
  const StrVector &GetPBDevices(void) { return dlPBDevices; }
  const StrVector &GetCTDevices(void) { return dlCTDevices; }
  /* -- Get device information --------------------------------------------- */
  size_t GetNumPlaybackDevices(void) const { return dlPBDevices.size(); }
  /* -- Get device information --------------------------------------------- */
  const string GetCaptureDevice(const size_t stId) const
  { // bail if value out of range
    if(stId >= GetNumCaptureDevices())
      XC("Specified audio capture device id out of range!",
        "Index", stId, "Count", GetNumCaptureDevices());
    // Return device
    return dlCTDevices[stId];
  }
  /* -- Get device information --------------------------------------------- */
  size_t GetNumCaptureDevices(void) const { return dlCTDevices.size(); }
  /* -- Send init signal --------------------------------------------------- */
  bool ReInit(void)
  { // Return if signal already set to re-initialise
    if(FlagIsSet(AF_REINIT)) return false;
    // Set the signal to re-init (it will get unset when re-initialised)
    FlagSet(AF_REINIT);
    // Send event to re-initialise audio
    cEvtMain->Add(EMC_AUD_REINIT);
    // Event sent
    return true;
  }
  /* -- Init --------------------------------------------------------------- */
  void Init(void)
  { // Class initialised
    IHInitialise();
    // Log subsystem
    LW(LH_DEBUG, "Audio class starting up...");
    // Init context
    InitContext();
    // Init thread
    InitThread();
    // Log status
    LW(LH_INFO, "Audio class started successfully.");
  }
  /* -- Stop all sounds ---------------------------------------------------- */
  void Stop(void)
  { // Stop all videos, streams and samples from playing
    VideoStop();
    StreamStop();
    StopAllSamples();
  }
  /* -- Init --------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if class already de-initialised
    if(IHNotDeInitialise()) return;
    // Log subsystem
    LW(LH_DEBUG, "Audio class shutting down...");
    // DeInit thread
    ThreadDeInit();
    // Unload all Stream, Sample and Source classes
    cStreams->CollectorDestroyUnsafe();
    cSamples->CollectorDestroyUnsafe();
    cSources->CollectorDestroyUnsafe();
    // Unload handles
    DeInitContext();
    // Re-Init members
    tpNextCheck = {};
    dlPBDevices.clear();
    dlCTDevices.clear();
    // Report error code
    LW(LH_INFO, "Audio class shutdown finished.");
  }
  /* -- Default constructor ------------------------------------------------ */
  Audio(void) :                        // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    IHelper{ __FUNCTION__ },           // Initialise class name
    AudioFlags{ AF_NONE },             // Initialise no audio flags
    tThread{ "audio",                  // Initialise thread name
      bind(&Audio::AudioThreadMain,    // " with reference to callback
        this, _1) },                   // " function
    cdCheckRate{ seconds{ 0 } }        // Initialise thread check time
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Audio, DeInit());        // Destructor helper
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Audio);              // Omit copy constructor for safety
  /* -- End ---------------------------------------------------------------- */
} *cAudio = nullptr;                   // Pointer to static class
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
