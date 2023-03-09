-- SETUP.LUA =============================================================== --
-- ooooooo.--ooooooo--.ooooo.-----.ooooo.--oooooooooo-oooooooo.----.ooooo..o --
-- 888'-`Y8b--`888'--d8P'-`Y8b---d8P'-`Y8b-`888'---`8-`888--`Y88.-d8P'---`Y8 --
-- 888----888--888--888---------888---------888--------888--.d88'-Y88bo.---- --
-- 888----888--888--888---------888---------888oo8-----888oo88P'---`"Y888o.- --
-- 888----888--888--888----oOOo-888----oOOo-888--"-----888`8b.--------`"Y88b --
-- 888---d88'--888--`88.---.88'-`88.---.88'-888-----o--888-`88b.--oo----.d8P --
-- 888bd8P'--oo888oo-`Y8bod8P'---`Y8bod8P'-o888ooood8-o888o-o888o-8""8888P'- --
-- ========================================================================= --
-- Copyr. © MS-Design, 2023       Copyr. © Millennium Interactive Ltd., 1994 --
-- ========================================================================= --
-- Core function aliases --------------------------------------------------- --
local tonumber<const>, tostring<const>, pairs<const>, insert<const>,
  remove<const>, format<const>, floor<const>, cos<const>, sin<const>,
  min<const>, max<const>, rep<const>
  = -- --------------------------------------------------------------------- --
  tonumber, tostring, pairs, table.insert, table.remove, string.format,
  math.floor, math.cos, math.sin, math.min, math.max, string.rep;
-- M-Engine function aliases ----------------------------------------------- --
local InfoMonitor<const>, InfoMonitorData<const>, InfoMonitors<const>,
  InfoVidModeData<const>, InfoVidModes<const>, InfoTime<const>,
  InfoCPUUsage<const>, InfoRAM<const>, InfoGPUFPS<const>, InfoEngine<const>,
  UtilGetRatio<const>, UtilClampInt<const>, UtilClamp<const>,
  UtilExplode<const>, AudioGetNumPBDs<const>, AudioGetPBDName<const>,
  AudioReset<const>, CVarsGet<const>, CVarsSet<const>, DisplayVReset<const>,
  DisplayReset<const>, CreditItem<const>, CreditLicence<const>,
  CreditTotal<const>, CVarsReset<const>, UtilWordWrap<const>
  = -- --------------------------------------------------------------------- --
  Display.Monitor, Display.MonitorData, Display.Monitors, Display.VidModeData,
  Display.VidModes, Info.Time, Info.CPUUsage, Info.RAM, Display.GPUFPS,
  Info.Engine, Util.GetRatio, Util.ClampInt, Util.Clamp, Util.Explode,
  Audio.GetNumPBDevices, Audio.GetPBDeviceName, Audio.Reset, CVars.Get,
  CVars.Set, Display.VReset, Display.Reset, Credit.Item, Credit.Licence,
  Credit.Total, CVars.Reset, Util.WordWrap;
-- Constants --------------------------------------------------------------- --
local aKeys<const> = Input.KeyCodes;
local iKeyEscape<const>, iKeyPageUp<const>, iKeyPageDown<const>,
  iKeyHome<const>, iKeyEnd<const>, iKeyUp<const>, iKeyDown<const>
  = -- --------------------------------------------------------------------- --
  aKeys.ESCAPE, aKeys.PAGE_UP, aKeys.PAGE_DOWN, aKeys.HOME, aKeys.END,
  aKeys.UP, aKeys.DOWN;
-- Diggers function and data aliases --------------------------------------- --
local InitSetup, aButtonData, aCursorIdData, aSfxData, GetCallbacks, SetCallbacks,
  LoadResources, PlayMusic, StopMusic, GetMusic, GetCursor, SetCursor,
  IsMouseYGreaterEqualThan, IsMouseYLessThan, RenderFade, IsKeyPressed,
  IsKeyRepeating, IsScrollingUp, IsScrollingDown, IsButtonPressed,
  IsButtonHeld, IsMouseInBounds, GetMouseY, PlayStaticSound,
  texSpr, fontLarge, fontLittle, fontTiny, RegisterFBUCallback,
  RenderShadow, IsScrollingLeft, IsScrollingRight, IsFading;
-- Frame-limiter types ----------------------------------------------------- --
local aFrameLimiterLabels<const> = {
  "Disabled", "Hardware Only", "Software Only", "Software and Hardware"
};
-- Window types ------------------------------------------------------------ --
local aWindowLabels<const> = {
  "Windowed Mode", "Borderless Full-screen Mode", "Exclusive Full-screen Mode"
};
-- Window sizes ----------------------------------------------------------- --
local aWindowSizes<const> = {          -- Keep to arbitrary aspect ratios
  {   0,   0}, { 320, 240}, { 512, 384}, { 640, 360}, { 640, 480}, {1024, 576},
  { 800, 600}, { 900, 600}, { 960, 720}, {1280, 720}, {1280, 960}, {1600,1200},
  {1680,1050}, {1600,1200}, {1920,1080}, {1920,1200}, {2560,1440}
};
-- Other ----------------------------------------------------------------------
local iOColour, aColours<const> = 1, {-- Colour transition animations
  0xFFF0F0F0,0xFFE0E0E0,0xFFD0D0D0,0xFFC0C0C0,0xFFB0B0B0,0xFFA0A0A0,0xFF909090,
  0xFF808080,0xFF707070,0xFF606060,0xFF505050,0xFF404040,0xFF303030,0xFF202020,
  0xFF101010,0xFF202020,0xFF303030,0xFF404040,0xFF505050,0xFF606060,0xFF707070,
  0xFF808080,0xFF909090,0xFFA0A0A0,0xFFB0B0B0,0xFFC0C0C0,0xFFD0D0D0,0xFFE0E0E0
};
-- ------------------------------------------------------------------------- --
local sTitle, sStatusLine1, sStatusLine2;
local nButtonIntensity, nButtonIntensityIncrement;
local iWindowId, iWindowIdOriginal, iSelectedOption, iSelectedButton;
local iFullScreenState, iFrameLimiter, iAudioDeviceId, iMonitorId;
local iFullScreenMode, iFullScreenModeOriginal;
local iFullScreenStateOriginal, iFrameLimiterOriginal, iAudioDeviceIdOriginal;
local iMonitorIdOriginal;
local iCursorId, fcbTick, fcbRender, fcbInput;
local aReadmeData, aReadmeVisibleLines, aReadmeColourData = { }, { }, { };
local iReadmeIndexBegin, iReadmeIndexEnd, iReadmeLines = 1, 1, 29;
local nRAMUsePercentage, nGPUFramesPerSecond, aOptionData;
local iCatSize, iCatBottom, nTime = 15, nil, nil;
local sStatusLineSave, nStatusLineSize, nStatusLinePos, nTipId;
local aCreditLines, lastMusic, nCPUUsageSystem, nCPUUsageProcess;
local iStageW, iStageH, iStageL, iStageT, iStageR, iStageB;
-- ------------------------------------------------------------------------- --
local function GetMonitorIdOrPrimary()
  -- If the monitor id is set to the primary monitor then we need to return
  -- the actual id of the primary monitor instead
  if iMonitorId == -1 then return InfoMonitor() end;
  -- Return selected monitor id
  return iMonitorId;
end
-- ------------------------------------------------------------------------- --
local function UpdateLabels()
  -- This will update the text on all the labels based on updated values
  for iI = 1, #aOptionData do
    local aData<const> = aOptionData[iI];
    aData[2] = aData[3]();
  end
end
-- ------------------------------------------------------------------------- --
local function MonitorUpdate()
  if iMonitorId == -1 then return "Primary Monitor" end;
  local sMsg<const> = InfoMonitorData(iMonitorId);
  if #sMsg > 34 then return sMsg:sub(0,30).."..." end;
  return sMsg;
end
local function MonitorDown()
  if iMonitorId == -1 then return end;
  iMonitorId = iMonitorId - 1;
  UpdateLabels()
end
local function MonitorUp()
  if iMonitorId == InfoMonitors()-1 then return end;
  iMonitorId = iMonitorId + 1;
  UpdateLabels()
end
-- ------------------------------------------------------------------------- --
local function FSStateUpdate()
  return aWindowLabels[iFullScreenState + 1];
end
local function FSStateDown()
  if iFullScreenState <= 0 then return end;
  iFullScreenState = iFullScreenState - 1;
  if iFullScreenState < 2 then iFullScreenMode = -2 end;
  UpdateLabels()
end
local function FSStateUp()
  if iFullScreenState >= #aWindowLabels-1 then return end;
  iFullScreenState = iFullScreenState + 1;
  if iFullScreenState == 2 then iFullScreenMode = -1 end;
  UpdateLabels()
end
-- ------------------------------------------------------------------------- --
local function FSResUpdate()
  if iFullScreenMode == -2 then return "Disabled" end;
  if iFullScreenMode == -1 then return "Automatic" end;
  -- Which one
  local iWidth<const>, iHeight<const>, iBits<const>, nRefresh<const> =
    InfoVidModeData(GetMonitorIdOrPrimary(), iFullScreenMode);
  -- Return data
  return iWidth.."x"..iHeight.."x"..iBits.." "..nRefresh.."hz ("..
    UtilGetRatio(iWidth, iHeight)..")";
end
local function FSResDown()
  if iFullScreenMode <= -1 then return end;
  iFullScreenMode = iFullScreenMode - 1;
end
local function FSResUp()
  if iFullScreenMode <= -2 or
    iFullScreenMode >= InfoVidModes(GetMonitorIdOrPrimary())-1 then
      return end;
  iFullScreenMode = iFullScreenMode + 1;
end
-- ------------------------------------------------------------------------- --
local function WSizeUpdate()
  -- Custom resolution?
  if iWindowId == 0 then return "Custom" end;
  if iWindowId == 1 then return "Automatic" end;
  -- Which one
  local aData<const> = aWindowSizes[iWindowId];
  if not aData then return "Unknown ("..iWindowId..")" end;
  -- Return data
  return aData[1].."x"..aData[2].." ("..
    UtilGetRatio(aData[1], aData[2])..")";
end
local function WSizeDown()
  iWindowId = iWindowId - 1;
  if iWindowId < 1 then iWindowId = 1 end;
end
local function WSizeUp()
  iWindowId = iWindowId + 1;
  if iWindowId > #aWindowSizes then iWindowId = #aWindowSizes end;
end
-- ------------------------------------------------------------------------- --
local function LimiterUpdate()
  return aFrameLimiterLabels[iFrameLimiter+1] or aFrameLimiterLabels[2]
end
local function LimiterDown()
  iFrameLimiter = UtilClampInt(iFrameLimiter-1, 0, #aFrameLimiterLabels-1);
end
local function LimiterUp()
  iFrameLimiter = UtilClampInt(iFrameLimiter+1, 0, #aFrameLimiterLabels-1);
end
-- ------------------------------------------------------------------------- --
local function FilterUpdate()
  if CVarsGet("vid_texfilter") == "0" then
    return "Point" end; return "Bilinear";
end
local function FilterSwap()
  if CVarsGet("vid_texfilter") == "0" then
    return CVarsSet("vid_texfilter", 3) end;
  CVarsSet("vid_texfilter", 0);
end
-- ------------------------------------------------------------------------- --
local function AudioUpdate()
  if iAudioDeviceId == -1 then return "Default Playback Device";
  elseif iAudioDeviceId >= AudioGetNumPBDs() then
    return "Invalid Playback Device" end;
  local N = AudioGetPBDName(iAudioDeviceId);
  local L = "OpenAL Soft on ";
  if N:sub(1, #L) == L then N = N:sub(#L) end;
  if #N > 34 then return N:sub(0,30).."..." end;
  return N;
end
local function AudioDown()
  if iAudioDeviceId == -1 then return end;
  iAudioDeviceId = iAudioDeviceId - 1;
end
local function AudioUp()
  if iAudioDeviceId == AudioGetNumPBDs()-1 then return end;
  iAudioDeviceId = iAudioDeviceId + 1;
end
-- ------------------------------------------------------------------------- --
local function VMasterUpdate()
  return floor(CVarsGet("aud_vol")*100).."%";
end
local function VMasterDown()
  CVarsSet("aud_vol",UtilClamp(tonumber(CVarsGet("aud_vol"))-0.05,0,1));
end
local function VMasterUp()
  CVarsSet("aud_vol",UtilClamp(tonumber(CVarsGet("aud_vol"))+0.05,0,1));
end
-- ------------------------------------------------------------------------- --
local function VStreamUpdate()
  return floor(CVarsGet("aud_strvol") * 100).."%";
end
local function VStreamDown()
  CVarsSet("aud_strvol",UtilClamp(tonumber(CVarsGet("aud_strvol"))-0.05,0,1));
end
local function VStreamUp()
  CVarsSet("aud_strvol",UtilClamp(tonumber(CVarsGet("aud_strvol"))+0.05,0,1));
end
-- ------------------------------------------------------------------------- --
local function VSampleUpdate()
  return floor(CVarsGet("aud_samvol") * 100).."%";
end
local function VSampleDown()
  CVarsSet("aud_samvol",UtilClamp(tonumber(CVarsGet("aud_samvol"))-0.05,0,1));
end
local function VSampleUp()
  CVarsSet("aud_samvol",UtilClamp(tonumber(CVarsGet("aud_samvol"))+0.05,0,1));
end
-- ------------------------------------------------------------------------- --
local function VFMVUpdate()
  return floor(CVarsGet("aud_fmvvol") * 100).."%";
end
local function VFMVDown()
  CVarsSet("aud_fmvvol",UtilClamp(tonumber(CVarsGet("aud_fmvvol"))-0.05,0,1));
end
local function VFMVUp()
  CVarsSet("aud_fmvvol",UtilClamp(tonumber(CVarsGet("aud_fmvvol"))+0.05,0,1));
end
-- ------------------------------------------------------------------------- --
local function FlickerColour1()
  fontLarge:SetRGBA(1, 1, 1, 1);
  fontTiny:SetRGBA(0.75, 0.75, 0.5, 1);
end
-- ------------------------------------------------------------------------- --
local function FlickerColour2()
  fontLarge:SetRGBA(0.75, 0.75, 0.75, 1);
  fontTiny:SetRGBA(1, 1, 0.75, 1);
end
-- ------------------------------------------------------------------------- --
local function FlickerColours(cbCol1, cbCol2)
  if nTime % 0.43 < 0.215 then cbCol1() else cbCol2() end;
end
-- ------------------------------------------------------------------------- --
local function ColouriseText(sText)
  -- Set next colour to start on
  local iColour, sOutText = iOColour, "";
  -- For each letter in the title
  for iI = 1, #sText do
    -- Apply colour to the letter
    sOutText = sOutText..format("\rc%08x",
      aColours[1 + (iColour % #aColours)])..
      sText:sub(iI, iI);
    -- Next colour
    iColour = iColour - 1;
  end
  -- Set colour for next time
  iOColour = (iOColour + 1) % #aColours;
  -- Return result
  return sOutText;
end
-- ------------------------------------------------------------------------- --
local function RenderBackgroundStart(nId)
  -- Render game background
  fcbRender();
  -- Draw background animation
  for iY = iStageT+6, iStageB, 16 do
    for iX = iStageL+6, iStageR, 16 do
      local nVal = (nTime*2) - (iX+iY);
      nVal = (0.5+((cos(nVal)*sin(nVal)))) * 1;
      texSpr:SetAlpha(nVal*0.5);
      local nVal2<const> = nVal * 16;
      texSpr:BlitSLTRBA(1023, iX, iY, iX + nVal2, iY + nVal2, nVal);
    end
  end
  -- Draw background for text
  texSpr:SetRGB(0.2, 0.1, 0.1);
  RenderFade(0.75, 4, 28, 316, 212, 1022);
  -- Draw title and status fades
  texSpr:SetRGB(0.4, 0.2, 0.2);
  RenderFade(0.75, 4, 212, 316, 236, 1022);
  RenderFade(0.75, 4, 4, 316, 28, 1022);
  -- Draw shadow around window
  RenderShadow(4, 4, 316, 236);
  -- Print title
  texSpr:SetRGBA(1, 1, 1, 1);
  -- Set alternatig colour for title
  FlickerColours(FlickerColour1, FlickerColour2);
  fontLarge:PrintC(160, 8, ColouriseText(sTitle));
  -- Print tip
  if nStatusLineSize and nStatusLineSize > 0 then
    fontTiny:PrintM(8-nStatusLinePos, 226,
      nStatusLinePos, 304+nStatusLinePos, sStatusLine2);
    nStatusLinePos = nStatusLinePos + 1;
    if nStatusLinePos >= nStatusLineSize then
      nStatusLinePos = 0 end;
  else
    fontTiny:PrintC(160, 226, sStatusLine2);
  end
  -- Pritn system information
  FlickerColours(FlickerColour2, FlickerColour1);
  fontTiny:PrintC(160, 217, sStatusLine1);
  fontTiny:Print (  8,  9, format("RAM %.1f%%", nRAMUsePercentage));
  fontTiny:PrintR(312, 18, format("%.1f%% ENG", nCPUUsageProcess));
  FlickerColours(FlickerColour1, FlickerColour2);
  fontTiny:PrintR(312,  9, format("%.1f%% SYS", nCPUUsageSystem));
  fontTiny:Print (  8, 18, format("%.1f FPS",   nGPUFramesPerSecond));
end
-- ------------------------------------------------------------------------- --
local function Finish()
  -- Unload data
  aReadmeColourData, aReadmeData = { }, { };
  -- Return to sub-proc
  SetCallbacks(fcbTick, fcbRender, fcbInput);
  -- Set original cursor
  SetCursor(iCursorId);
  -- No music to set? Stop current music and return
  if not lastMusic then return StopMusic() end;
  -- Resume music
  PlayMusic(lastMusic, nil, 2);
  -- Do not keep reference to handle
  lastMusic = nil;
  -- Restore redraw callback
  RegisterFBUCallback("setup");
end
-- ------------------------------------------------------------------------- --
local function Refresh()
  -- Refresh monitor settings
  local function RefreshMonitorSettings()
    -- Initialise monitor video modes and use primary monitor if invalid
    iMonitorId = tonumber(CVarsGet("vid_monitor"));
    if iMonitorId < -1 or iMonitorId >= InfoMonitors() then
      iMonitorId = -1 end;
    iMonitorIdOriginal = iMonitorId;
    -- Initialise video resolution and use desktop resolution if invalid
    iFullScreenState = tonumber(CVarsGet("vid_fs"));
    if iFullScreenState < 0 or iFullScreenState > 1 then
      iFullScreenState = 0 end;
    iFullScreenMode = tonumber(CVarsGet("vid_fsmode"));
    -- If full-screen mode is enabled?
    if iFullScreenState == 1 then
      -- If full-screen mode is -1 (Exclusive full-screen)?
      if iFullScreenMode >= -1 then iFullScreenState = 2;
      -- If full-screen mode is -2 (Borderless window mode)?
      elseif iFullScreenMode == -2 then iFullScreenState = 1 end;
    -- Windowed mode
    else iFullScreenMode = -2 end;
    -- If the full-screen mode cvar is invalid then reset to 'automatic' mode
    -- where the engine will set the full-screen mode to the desktop mode.
    if iFullScreenMode < -2 or
      iFullScreenMode >= InfoVidModes(GetMonitorIdOrPrimary()) then
        iFullScreenMode = -1 end;
    -- Record the the initialised variables so we can check if they were
    -- modified when the user clicks the apply button.
    iFullScreenModeOriginal = iFullScreenMode;
    iFullScreenStateOriginal = iFullScreenState;
  end;
  -- Refresh window settings
  local function RefreshWindowSettings()
    -- Get window size
    local iWindowWidth = tonumber(CVarsGet("win_width"));
    local iWindowHeight = tonumber(CVarsGet("win_height"));
    -- Set to defaults if invalid
    if iWindowWidth < -1 then iWindowWidth = -1 end;
    if iWindowHeight < -1 then iWindowHeight = -1 end;
    -- Set 'automatic' window id
    iWindowId = 0;
    -- Find window size in list and override the above index if found
    for iIndex = 1, #aWindowSizes do
      local aData = aWindowSizes[iIndex];
      if aData[1] == iWindowWidth and aData[2] == iWindowHeight then
        iWindowId = iIndex;
      end
    end
    -- Record original id
    iWindowIdOriginal = iWindowId;
  end
  -- Refresh frame limiter setting
  local function RefreshFrameLimiterSettings()
    -- Get vsync value, thread delay and kernel tick rate
    iFrameLimiter = tonumber(CVarsGet("vid_vsync"));
    -- Check for delay and if set? Set software category too
    local iAddedDelay<const> = tonumber(CVarsGet("app_delay"));
    if iAddedDelay > 0 then iFrameLimiter = iFrameLimiter + 2 end;
    -- Set original value
    iFrameLimiterOriginal = iFrameLimiter;
  end;
  -- Refresh audio settings
  local function RefreshAudioSettings()
    iAudioDeviceId = tonumber(CVarsGet("aud_interface"));
    iAudioDeviceIdOriginal = iAudioDeviceId;
  end
  -- Perform refreshes
  RefreshMonitorSettings();
  RefreshWindowSettings();
  RefreshFrameLimiterSettings();
  RefreshAudioSettings();
  -- Update labels
  UpdateLabels();
end
-- ------------------------------------------------------------------------- --
local function ApplySettings()
  -- Set window size
  if iWindowId >= 1 and iWindowId <= #aWindowSizes then
    local aData<const> = aWindowSizes[iWindowId];
    CVarsSet("win_width", aData[1]);
    CVarsSet("win_height", aData[2]);
  end
  -- Set window variables if needed
  if iFullScreenState == 2 then CVarsSet("vid_fs", 1)
                           else CVarsSet("vid_fs", iFullScreenState) end;
  -- If full-screen mode is resetting?
  if iFullScreenMode == -3 then
    -- Now set to default mode
    iFullScreenMode = -2;
    CVarsReset("vid_fsmode");
  else CVarsSet("vid_fsmode", iFullScreenMode) end;
  CVarsSet("vid_monitor", iMonitorId);
  CVarsSet("aud_interface", iAudioDeviceId);
  -- Set frame limiter options
  local iVSync, iDelay;
  if iFrameLimiter > 1 then iVSync, iDelay = iFrameLimiter % 2, 1;
  else iVSync, iDelay = iFrameLimiter, 0 end;
  CVarsSet("vid_vsync", iVSync);
  CVarsSet("app_delay", iDelay);
  -- Reset audio subsystem if interface changed
  if iAudioDeviceIdOriginal ~= iAudioDeviceId then AudioReset() end;
  -- If GPU related parameters changed from original then reset video
  if iFullScreenMode ~= iFullScreenModeOriginal or
     iFullScreenStateOriginal ~= iFullScreenState or
     iFrameLimiterOriginal ~= iFrameLimiter or
     iMonitorIdOriginal ~= iMonitorId then DisplayVReset();
  -- If window parameters changed then just reset window
  elseif iFullScreenState == 0 and iWindowId ~= iWindowIdOriginal then
    DisplayReset() end;
  -- Refresh/update settings
  Refresh();
end
-- ------------------------------------------------------------------------- --
local function SetDefaults()
  -- Push defaults
  iFullScreenState = 0;
  iFullScreenMode = -3;
  iMonitorId = -1;
  iAudioDeviceId = -1;
  iFrameLimiter = 1;
  iWindowId = 1;
  -- Other options
  CVarsReset("vid_texfilter");
  -- Reset volumes
  CVarsReset("aud_vol");
  CVarsReset("aud_strvol");
  CVarsReset("aud_samvol");
  CVarsReset("aud_fmvvol");
  -- Set new settings
  ApplySettings();
end
-- ------------------------------------------------------------------------- --
local nAlpha<const> = 1/60;
local function ProcSysInfo()
  -- Get time
  nTime = InfoTime();
  -- Get cpu info, ram info and nGPUFramesPerSecond
  nCPUUsageProcess, nCPUUsageSystem = InfoCPUUsage();
  nRAMUsePercentage = InfoRAM();
  nGPUFramesPerSecond = (nAlpha * InfoGPUFPS()) + (1.0 - nAlpha) *
    (nGPUFramesPerSecond or 60);
end
-- ------------------------------------------------------------------------- --
local function ProcReadme()
  -- Set system info
  ProcSysInfo();
  -- Show up mouse cursor if on top half of screen
  if IsMouseYGreaterEqualThan(206) then SetCursor(aCursorIdData.BOTTOM);
  -- Show down mouse cursor if on bottom half of screen
  elseif IsMouseYLessThan(32) then SetCursor(aCursorIdData.TOP);
  -- Show default cursor otherwise
  else SetCursor(aCursorIdData.ARROW) end;
end
-- ------------------------------------------------------------------------- --
local function RenderReadme()
  -- Render default background
  RenderBackgroundStart(757);
  -- Draw readme
  for iIndex = 1, #aReadmeVisibleLines do
    -- Get data
    local aData<const> = aReadmeVisibleLines[iIndex];
    -- Calculate graident colour
    local iCol<const> = aReadmeColourData[iIndex];
    -- Calculate intensity
    local nIntensity<const> =
      0.75 + (((iIndex / #aReadmeVisibleLines) + nTime) % 0.25);
    -- Set colour
    fontTiny:SetRGB(nIntensity, nIntensity, nIntensity);
    -- Print line
    fontTiny:Print(aData[1], aData[2], aData[3]);
  end
  -- Set alternating title colour based on current time
  if nTime % 0.43 < 0.215 then fontTiny:SetRGBA(0.5, 0.5, 0.5, 1);
                          else fontTiny:SetRGBA(0.75, 0.75, 0.75, 1) end;
end
-- ------------------------------------------------------------------------- --
local function UpdateReadmeLines()
  -- Clear displayed lines
  aReadmeVisibleLines = { };
  -- For each line in readme file. Add this line to the displayed lines list
  for iIndex = iReadmeIndexBegin,
    min(iReadmeIndexBegin + iReadmeLines, #aReadmeData) do
    insert(aReadmeVisibleLines,
      { 6, 24 + ((#aReadmeVisibleLines + 1) * 6),
        aReadmeData[iIndex]:sub(1,77) });
  end
  -- Update statuses
  sStatusLine1 = "DISPLAYING LINE "..iReadmeIndexBegin.." TO "..
    iReadmeIndexEnd.." OF "..#aReadmeData.." IN THIS README TEXT DOCUMENT";
  sStatusLine2 =
    "CURSORS+PGUP+PGDN+HOME+END+LMB+JB1:SCROLL  F2+RMB+JB2:SETUP  ESC:CANCEL";
  -- Remove marquee settings
  sStatusLineSave, nStatusLineSize, nStatusLinePos, nTipId =
    nil, nil, nil, nil;
end
-- ------------------------------------------------------------------------- --
local function SetReadme(Line)
  -- Readme line colour data count minus one
  local iCountMinusOne<const> = #aReadmeColourData - 1;
  -- Get maximum lines
  local iMax<const> =
    UtilClampInt(#aReadmeData - iCountMinusOne, 1, 0x7FFFFFFF);
  -- Set to end line?
  if Line == 0x7FFFFFFF then iReadmeIndexBegin = iMax;
  -- Set line?
  else iReadmeIndexBegin = UtilClampInt(Line, 1, iMax) end;
  -- Set ending line
  iReadmeIndexEnd = UtilClampInt(iReadmeIndexBegin +
    iCountMinusOne, 1, #aReadmeData);
  -- Update displayed readme lines
  UpdateReadmeLines();
end
-- ------------------------------------------------------------------------- --
local function ScrollReadme(iAdj) SetReadme(iReadmeIndexBegin + iAdj) end;
-- ------------------------------------------------------------------------- --
local function ProcReadmeInput()
  -- Check and process key commands
  if IsKeyPressed(iKeyEscape) then Finish();
  elseif IsKeyRepeating(iKeyPageUp) or
         IsScrollingLeft() then ScrollReadme(-29);
  elseif IsKeyRepeating(iKeyPageDown) or
         IsScrollingRight() then ScrollReadme(29);
  elseif IsKeyPressed(iKeyHome) then SetReadme(1);
  elseif IsKeyPressed(iKeyEnd) then SetReadme(0x7FFFFFFF);
  elseif IsKeyRepeating(iKeyUp) or IsScrollingUp() then ScrollReadme(-1);
  elseif IsKeyRepeating(iKeyDown) or IsScrollingDown() then ScrollReadme(1);
  -- Cancel button pressed? Cancel readme
  elseif IsButtonPressed(1) then InitSetup(false);
  -- Select button pressed? Scroll!
  elseif IsButtonHeld(0) then
    -- On bottom section of screen? Scroll down
    if IsMouseYGreaterEqualThan(206) then ScrollReadme(1);
    -- Else scroll up if on top section of screen
    elseif IsMouseYLessThan(32) then ScrollReadme(-1) end;
  end
end
-- ------------------------------------------------------------------------- --
local function InitReadme()
  -- Waiting cursor
  SetCursor(aCursorIdData.WAIT);
  -- Change render procs but no input so load doesn't finish after the user
  -- has left the screen.
  SetCallbacks(ProcReadme, RenderReadme, nil);
  -- Init text colours
  aReadmeColourData = { };
  for I = 1, 30 do insert(aReadmeColourData, (I/30)*0.25) end;
  -- Set title
  sTitle = "HELP AND INFO";
  -- Blank readme for now
  aReadmeData = { "Loading..." };
  UpdateReadmeLines();
  -- At least one tick
  ProcReadme();
  -- Readme text file loaded?
  local function OnLoaded(aRes)
    -- Arrow cursor
    SetCursor(aCursorIdData.ARROW);
    -- Load readme file into LUA array and then unload it
    aReadmeData = UtilExplode(aRes[1].H:ToString():upper(), "\n");
    -- Add credits
    local iPosition = #aReadmeData-4;
    for iLine = 1, #aCreditLines do
      insert(aReadmeData, iPosition+iLine, aCreditLines[iLine]) end;
    -- Update displayed readme lines
    UpdateReadmeLines();
    -- Allow input
    SetCallbacks(ProcReadme, RenderReadme, ProcReadmeInput);
  end
  -- Load resources asynchronously (keep previous cache)
  LoadResources("ReadMe", {{T=5,F="readme.txt",P={}}}, OnLoaded, true);
end
-- ----------------------------------------------------------------------- --
local function SetTip(nNewTipId, sTip)
  -- Ignore if tip set
  if nNewTipId == nTipId then return end;
  -- Set new tip
  nTipId = nNewTipId;
  -- No tip? Reset
  if not sTip then
    -- Save status line
    sStatusLine2 = sStatusLineSave;
    -- Re-calculate size and reset position
    nStatusLineSize, nStatusLinePos = -1, -1;
    -- Done
    return;
  end
  -- Get size of tip
  local sTipPlusSep = sTip.." -+- "
  local nTipSizePixels = fontTiny:PrintS(sTipPlusSep);
  local nMaxLine = 304+nTipSizePixels;
  -- Fill the line until it is big enough to scroll seamlessly
  sStatusLine2, nStatusLineSize = "", nTipSizePixels;
  while nStatusLineSize < nMaxLine do
    -- Set the tip and append a separator for seamless repeating
    sStatusLine2 = sStatusLine2..sTipPlusSep;
    -- Calculate size
    nStatusLineSize = fontTiny:PrintS(sStatusLine2);
  end
  -- Reset size to tip length
  nStatusLineSize = nTipSizePixels;
  -- Add more text so we can have a seamless repeating
  sStatusLine2 = sStatusLine2..sTip;
  -- Reset position
  nStatusLinePos = -304;
end
-- ------------------------------------------------------------------------- --
local function RenderSetup()
  -- Render default background
  RenderBackgroundStart(771);
  -- Draw selected item
  if iSelectedOption > 0 then
    texSpr:SetRGB(0, 0, 0);
    RenderFade(nButtonIntensity, 4,
       28 + ((iSelectedOption-1) * iCatSize),
     316, 28 + iCatSize + ((iSelectedOption-1) * iCatSize), 1022);
    texSpr:SetRGB(1, 1, 1);
    -- Set tip
    SetTip(iSelectedOption, aOptionData[iSelectedOption][6]);
  -- No selected option so remove tip if a button isn't selected
  end
  -- For each category
  for I, D in pairs(aOptionData) do
    local nIntensity;
    if iSelectedOption == I then fontLittle:SetRGB(1, 1, 1);
    else
      nIntensity = 0.5 + (((I/#aOptionData) + nTime) % 0.5);
      fontLittle:SetRGB(0, 0, nIntensity);
    end
    fontLittle:Print(8, 17+(I*iCatSize), D[1]);
    if iSelectedOption == I then fontLittle:SetRGB(1, 1, 1);
    else
      nIntensity = 0.5 + (((I/#aOptionData) + -nTime) % 0.5);
      fontLittle:SetRGB(0, nIntensity, 0);
    end
    fontLittle:PrintR(312, 17+(I*iCatSize), D[2]);
  end
  -- No selected button
  iSelectedButton = 0;
  -- For each button
  texSpr:SetRGB(0, 0, 0);
  for N, D in pairs(aButtonData) do
    -- Mouse in bounds?
    if IsMouseInBounds(D[1], D[2], D[3], D[4]) then
      -- Set tip
      SetTip(D[7], D[8]);
      -- Set button
      iSelectedButton = D[7];
      -- Set glowing colour
      texSpr:SetRGB(nButtonIntensity, nButtonIntensity, nButtonIntensity);
      -- Draw background
      RenderFade(1-nButtonIntensity, D[1], D[2], D[3], D[4], 1022);
    -- Mouse not in bounds? Just draw black background
    else RenderFade(0.5, D[1], D[2], D[3], D[4], 1023) end;
    -- Set button text colour and print the text
    fontLittle:SetRGB(1, 1, 1);
    fontLittle:PrintC(D[1]+39, D[2]+6, N);
  end
  -- Remove tip if an option isn't selected
  if iSelectedButton == 0 and iSelectedOption == 0 then
    SetTip(0, sStatusLineSave) end;
  -- Print generic info
  if nTime % 0.43 < 0.215 then fontTiny:SetRGBA(0.5, 0.5, 0.5, 1);
                          else fontTiny:SetRGBA(0.75, 0.75, 0.75, 1) end;
  -- Reset sprites colour because we changed it
  texSpr:SetRGBA(1, 1, 1, 1);
end
-- ------------------------------------------------------------------------- --
local function LastOne(Index)
  -- Play sound
  PlayStaticSound(aSfxData.CLICK);
  -- Get data
  local D = aOptionData[Index];
  -- Call last function
  D[4]();
  -- Refresh
  D[2] = D[3]();
end
-- ------------------------------------------------------------------------- --
local function NextOne(Index)
  -- Play sound
  PlayStaticSound(aSfxData.CLICK);
  -- Get data
  local D = aOptionData[Index];
  -- Call next function
  D[5]();
  -- Refresh
  D[2] = D[3]();
end
-- ------------------------------------------------------------------------- --
local function ProcSetupInput()
  -- If escape was pressed, return to game
  if IsKeyPressed(iKeyEscape) or IsButtonPressed(1) then
    return Finish() end;
  -- For each button
  for _, aData in pairs(aButtonData) do
    -- Mouse in bounds?
    if IsMouseInBounds(aData[1], aData[2], aData[3], aData[4]) then
      -- Nothing selected
      iSelectedOption = 0;
      -- Set specified cursor
      SetCursor(aData[5]);
      -- Left button clicked?
      if IsButtonPressed(0) then
        -- Play sound
        PlayStaticSound(aSfxData.SELECT);
        -- Execute command
        aData[6]();
      end
      -- Done
      return;
    end
  end
  -- Test if mouse is in the configuraiton options area
  if IsMouseInBounds(4, 28, 316, iCatBottom) then
    -- Set selected option
    iSelectedOption = floor((GetMouseY()-28) / iCatSize) + 1;
    -- Show select cursor
    SetCursor(aCursorIdData.SELECT);
    -- Left clicked or mouse scrolled down?
    if IsButtonPressed(2) or IsButtonPressed(5) or IsButtonPressed(7) or
       IsScrollingUp() then NextOne(iSelectedOption);
    -- Right clicked or mouse scrolled up?
    elseif IsButtonPressed(0) or IsButtonPressed(4) or IsButtonPressed(6) or
       IsScrollingDown() then LastOne(iSelectedOption) end;
    -- Done
    return;
  end
  -- Nothing selected
  iSelectedOption = 0;
  -- Default cursor
  SetCursor(aCursorIdData.ARROW);
end
-- ------------------------------------------------------------------------- --
local function ProcSetup()
  -- Set system info
  ProcSysInfo();
  -- Set button intensity
  nButtonIntensity =
    UtilClamp(nButtonIntensity + nButtonIntensityIncrement, 0.25, 0.75);
  if nButtonIntensity == 0.25 or nButtonIntensity == 0.75 then
    nButtonIntensityIncrement = -nButtonIntensityIncrement end;
end
-- ------------------------------------------------------------------------- --
local function InitConfig()
  -- No option selected
  iSelectedOption = 0;
  -- Set title
  sTitle = "CONFIGURATION";
  -- Initialise status bars
  local AT, AV, AMAV, AMIV, ABV, ARV, _, _, ATA = InfoEngine();
  sStatusLine1 = format("%s %s on %s %u.%u.%u.%u (%s)",
    CVarsGet("app_longname"), CVarsGet("app_version"),
    AT, AMAV, AMIV, ARV, ABV, ATA):upper();
  sStatusLine2 = "MS-DESIGN PROUDLY PRESENTS DIGGERS! A REMAKE FOR MODERN "..
    "OPERATING SYSTEMS AND HARDWARE FROM THE CLASSIC CD32 AND DOS DAYS. "..
    "THIS IS THE CONFIGURATION SCREEN. PRESS ESCAPE OR THE 'DONE' BUTTON "..
    "TO RETURN TO THE GAME OR MOVE YOUR MOUSE OVER AN OPTION TO HAVE MORE "..
    "EXPLAINED ABOUT IT HERE. USE YOUR MOUSE OR JOYSTICK TO MOVE THE CURSOR "..
    "AND THE BUTTONS TO CHANGE OPTIONS. PRESS F2 AT ANY TIME TO SEE THIS "..
    "SCREEN OR F1 FOR DOCUMENTATION. PRESS ALT+ENTER AT ANY TIME TO TOGGLE "..
    "FULL-SCREEN AND WINDOW. PRESS F11 TO RESET THE WINDOW SIZE AND F12 TO "..
    "TAKE A SCREENSHOT";
  sStatusLineSave = sStatusLine2;
  SetTip(0, sStatusLineSave);
  -- Refresh all settings
  Refresh();
  -- Set configuration screen
  SetCallbacks(ProcSetup, RenderSetup, ProcSetupInput)
end
-- ------------------------------------------------------------------------- --
local function InitThirdPartyCredits()
  -- Constants
  local sLeft, sRight, sFmt = "## ", " ##", "%-71s";
  local sRep = rep('#', 77);
  -- Init credit lines
  aCreditLines = { };
  -- For each credit
  for iIndex = 0, CreditTotal()-1 do
    -- Get credit information
    local sName, sVersion, bCopyright, sAuthor = CreditItem(iIndex);
    -- Add empty line
    insert(aCreditLines, "");
    -- Add titlebar for credit
    insert(aCreditLines, sRep);
    insert(aCreditLines, sLeft..format(sFmt,
      "USES THIRD-PARTY API '"..sName:upper().."' VERSION "..
        sVersion:upper()..".")..sRight);
    insert(aCreditLines, sLeft..
      format(sFmt, "BY "..sAuthor:upper().."...")..sRight);
    insert(aCreditLines, sRep);
    -- Add credit licence
    local aLines<const> = UtilExplode(CreditLicence(iIndex):upper(), "\n");
    for iI = 1, #aLines do
      local sLine<const> = aLines[iI];
      if #sLine > 78 then
        local aWrappedLines<const> = UtilWordWrap(sLine, 78, 0);
        for iWI = 1, #aWrappedLines do
          insert(aCreditLines, aWrappedLines[iWI]);
        end
      else insert(aCreditLines, sLine) end;
    end
  end
end
InitThirdPartyCredits();
-- ------------------------------------------------------------------------- --
local function DoInitSetup(bDoReadMe)
  -- Ignore if fading
  if IsFading() then return end;
  -- Get current callbacks
  local CBProc, CBRender, CBInput = GetCallbacks();
  -- Do readme?
  if bDoReadMe == true then
    -- Deny if already in readme
    if CBRender == RenderReadme then return end;
    -- If not in readme
    if CBRender == RenderSetup then return InitReadme() end;
  -- Do setup
  else
    -- Deny if already in setup
    if CBRender == RenderSetup then return end;
    -- If not in readme
    if CBRender == RenderReadme then return InitConfig() end;
  end
  -- Required setup assets finished loading
  local function OnLoaded(aResource)
    -- Save current music
    lastMusic = GetMusic();
    -- Play setup music
    PlayMusic(aResource[1].H, nil, 1);
    -- Initialise button intensity
    nButtonIntensity, nButtonIntensityIncrement = 1, 0.01;
    -- Backup old callbacks (Return to them later)
    fcbTick, fcbRender, fcbInput = CBProc, CBRender, CBInput;
    -- Backup current cursor id
    iCursorId = GetCursor();
    -- Get time
    nTime = InfoTime();
    -- Calculate bottom of categories
    iCatBottom = 28 + (#aOptionData * iCatSize);
    -- Do readme or do setup?
    if bDoReadMe then InitReadme() else InitConfig() end;
  end
  -- Load bank texture
  LoadResources("Setup", {{T=7,F="setup",P={}}}, OnLoaded);
  -- On fbo refresh callback
  local function OnRedraw(...)
    -- Update stage bounds
    iStageW, iStageH, iStageL, iStageT, iStageR, iStageB = ...;
    -- Refresh settings
    Refresh();
  end
  -- Register framebuffer update
  RegisterFBUCallback("setup", OnRedraw);
end
InitSetup = DoInitSetup;
-- Return imports and exports ---------------------------------------------- --
return { A = { InitSetup = InitSetup }, F = function(GetAPI)
  -- Imports --------------------------------------------------------------- --
  aSfxData, GetCallbacks, SetCallbacks, LoadResources, PlayMusic,
  StopMusic, GetMusic, GetCursor, SetCursor, IsMouseYGreaterEqualThan,
  IsMouseYLessThan, RenderFade, IsKeyPressed, IsKeyRepeating, IsScrollingUp,
  IsScrollingDown, IsButtonPressed, IsButtonHeld, IsMouseInBounds, GetMouseY,
  PlayStaticSound, texSpr, fontLarge, fontLittle, fontTiny,
  RegisterFBUCallback, RenderShadow, aCursorIdData, IsScrollingLeft,
  IsScrollingRight, IsFading
  = -- --------------------------------------------------------------------- --
  GetAPI("aSfxData", "GetCallbacks", "SetCallbacks",
    "LoadResources", "PlayMusic", "StopMusic", "GetMusic", "GetCursor",
    "SetCursor", "IsMouseYGreaterEqualThan", "IsMouseYLessThan", "RenderFade",
    "IsKeyPressed", "IsKeyRepeating", "IsScrollingUp","IsScrollingDown",
    "IsButtonPressed", "IsButtonHeld", "IsMouseInBounds", "GetMouseY",
    "PlayStaticSound", "texSpr", "fontLarge", "fontLittle", "fontTiny",
    "RegisterFBUCallback", "RenderShadow", "aCursorIdData", "IsScrollingLeft",
    "IsScrollingRight","IsFading");
  -- ----------------------------------------------------------------------- --
  aButtonData = {
    -- --------------------------------------------------------------------- --
    APPLY = {   4, 193,  82, 212, aCursorIdData.OK,   ApplySettings, 101,
      "CLICK TO APPLY ANY SYSTEM AFFECTING SETTINGS YOU HAVE CHANGED" },
    -- --------------------------------------------------------------------- --
    DONE  = {  82, 193, 160, 212, aCursorIdData.EXIT, Finish,        102,
      "CLICK TO EXIT THIS SETUP WINDOW AND RETURN TO YOUR GAME. ANY "..
      "CHANGED SETTINGS THAT NEED TO BE APPLIED WILL BE CANCELLED" },
    -- --------------------------------------------------------------------- --
    RESET = { 160, 193, 238, 212, aCursorIdData.OK,   SetDefaults,   103,
      "CLICK TO RESET ALL VALUES TO DEFAULTS AND AUTOMATICALLY APPLY THE "..
      "SETTINGS" },
    -- --------------------------------------------------------------------- --
    HELP  = { 238, 193, 316, 212, aCursorIdData.OK,   InitReadme,    104,
      "CLICK TO VIEW THE DOCUMENTATION FOR THIS GAME" }
  };-- --------------------------------------------------------------------- --
  aOptionData =
  { -- Option name -- Value -- UpdateFunc --- DownFunc --- UpFunc ---------- --
    { "Monitor",         "", MonitorUpdate, MonitorDown, MonitorUp,
      "CHANGES THE MONITOR THE GAME WILL APPEAR ON BY DEFAULT. PRESS "..
      "APPLY WHEN YOU ARE HAPPY WITH THE SELECTION TO ACTIVATE IT", },
    { "Display State",   "", FSStateUpdate, FSStateDown, FSStateUp,
      "CHANGES THE DEFAULT WINDOW STYLE OF THE GAME. PRESS APPLY WHEN YOU "..
      "ARE HAPPY WITH THE SELECTION TO ACTIVATE IT" },
    { "Full-Resolution", "", FSResUpdate,   FSResDown,   FSResUp,
      "ALLOWS YOU TO SET A CUSTOM DESKTOP RESOLUTION FOR EXCLUSIVE "..
      "FULL-SCREEN MODE. PRESS APPLY WHEN YOU ARE HAPPY WITH THE SELECTION "..
      "TO ACTIVATE IT" },
    { "Window Size",     "", WSizeUpdate,   WSizeDown,   WSizeUp,
      "ALLOWS YOU TO SET A CUSTOM WINDOW SIZE FOR DECORATED WINDOW ONLY "..
      "MODE. PRESS APPLY WHEN YOU ARE HAPPY WITH THE SELECTION TO ACTIVATE "..
      "IT" },
    { "Frame Limiter",   "", LimiterUpdate, LimiterDown, LimiterUp,
      "ALLOWS YOU TO SET A FRAME-LIMITER USING THE HARDWARE (VSYNC) AND/OR "..
      "BY USING A CPU WAIT FUNCTION TO CONSERVE POWER. PRESS APPLY WHEN YOU "..
      "ARE HAPPY WITH THE SELECTION TO ACTIVATE IT" },
    { "Texture Filter",  "", FilterUpdate,  FilterSwap,  FilterSwap,
      "APPLY A BILINEAR UPSCALE FILTER TO THE MAIN FRAMEBUFFER. THE GAME IS "..
      "RENDERED IN 320x240. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
    { "Audio Device",    "", AudioUpdate,   AudioDown,   AudioUp,
      "ALLOWS YOU TO SET THE DEFAULT AUDIO DEVICE TO USE FOR THE GAME. "..
      "PRESS APPLY WHEN YOU ARE HAPPY WITH THE SELECTION TO ACTIVATE IT" },
    { "Master Volume",   "", VMasterUpdate, VMasterDown, VMasterUp,
      "CHANGES THE FINAL OUTPUT VOLUME OF ALL MUSIC, SOUND EFFECTS AND FMV "..
      "MIXED TOGETHER. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
    { "Music Volume",    "", VStreamUpdate, VStreamDown, VStreamUp,
      "CHANGES THE MUSIC VOLUME. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
    { "Effect Volume",   "", VSampleUpdate, VSampleDown, VSampleUp,
      "CHANGES THE SFX VOLUME. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
    { "Fmv Volume",      "", VFMVUpdate,    VFMVDown,    VFMVUp,
      "CHANGES THE FMV VOLUME. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
  };-- Option name -- Value -- UpdateFunc --- DownFunc --- UpFunc
end };
-- == End-of-File ========================================================== --
