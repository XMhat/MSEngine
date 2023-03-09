-- INTRO.LUA =============================================================== --
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
-- M-Engine function aliases ----------------------------------------------- --
local UtilBlank<const>, InfoTime<const> = Util.Blank, Info.Time;
-- Diggers function and data aliases --------------------------------------- --
local LoadResources, GetCallbacks, SetCallbacks, VideoPlay, VideoStop, Fade,
  IsButtonPressed, InitTitle, texSpr, InitSetup, RegisterFBUCallback;
-- Intro initialisation function ------------------------------------------- --
local function InitIntro(bAndSetup)
  -- When intro resources have loaded?
  local function OnLoaded(aResources)
    -- Register framebuffer update
    local iStageW, iStageH, iStageL, iStageT, iStageR, iStageB;
    local function OnFrameBufferUpdate(...)
      iStageW, iStageH, iStageL, iStageT, iStageR, iStageB = ...;
    end
    RegisterFBUCallback("intro", OnFrameBufferUpdate);
    -- Get the texture handle that was loaded
    local tTiles, tTitle = aResources[1].H, aResources[2].H;
    tTitle:SetTile(0, 0, 0, 512, 240);
    tTitle:CreateTile(227, 240, 285, 448);
    -- Load and play intro video
    local vVideo = aResources[3].H;
    vVideo:SetFilter(false);
    -- Playing procedure()
    local function PlayingProc()
      -- If we're not in widescreen?
      if iStageL == 0 then
        -- Draw video normally
        vVideo:SetTexCoord(0, 0, 1, 1);
        vVideo:SetVertex(0, 0, 320, 240);
        return vVideo:Blit();
      end
      -- Draw video effect for widescreen (left side)
      vVideo:SetTexCoord(0, 0, 0, 1);
      vVideo:SetVertex(0, 0, iStageL, 240);
      vVideo:Blit();
      -- Draw video effect for widescreen (right side)
      vVideo:SetTexCoord(1, 0, 1, 1);
      vVideo:SetVertex(iStageR, 0, 320, 240);
      vVideo:Blit();
      -- Draw the actual video in the centre (4:3)
      vVideo:SetTexCoord(0, 0, 1, 1);
      vVideo:SetVertex(0, 0, 320, 240);
      vVideo:Blit();
      -- Draw transparent tiles over the top of the widescreen border
      tTiles:SetRGBA(1, 1, 1, 0.5);
      for iY = 0, 240, 16 do
        for iX = -16, iStageL-16, -16 do tTiles:BlitSLT(3, iX, iY) end;
        for iX = 320, iStageR, 16 do tTiles:BlitSLT(3, iX, iY) end;
      end
      -- Draw shadow
      texSpr:BlitSLTRB(1017, -16, iStageT,   0, iStageB);
      texSpr:BlitSLTRB(1018, 320, iStageT, 336, iStageB);
    end
    -- Not playing procedure
    local function NotPlayingProc()
      -- Draw sidebar scrolling logo's
      tTitle:SetRGBA(1, 1, 1, 1);
      tTitle:BlitLT(-96, 0);
      if iStageL == 0 then return end;
      local Width<const> = -iStageL-4;
      local Aspect<const> = 208/58;
      local Height<const> = Width*Aspect;
      local LX = (InfoTime()*100)%240;
      local LY = -LX;
      local X<const> = iStageL+4;
      tTitle:SetAlpha(0.25);
      tTitle:BlitSLTRB(1,         X, -240+LX,    X+Width,-240+Height+LX);
      tTitle:BlitSLTRB(1,         X,      LX,    X+Width,     Height+LX);
      tTitle:BlitSLTRB(1,         X,  240+LX,    X+Width, 240+Height+LX);
      tTitle:BlitSLTRB(1, 320+Width,      Height+LY, 320,     LY);
      tTitle:BlitSLTRB(1, 320+Width,  240+Height+LY, 320, 240+LY);
      tTitle:BlitSLTRB(1, 320+Width,  480+Height+LY, 320, 480+LY);
      LX = -LX;
      LY = -LY - 240;
      tTitle:BlitSLTRB(1,         X, -240+LX,    X+Width,-240+Height+LX);
      tTitle:BlitSLTRB(1,         X,      LX,    X+Width,     Height+LX);
      tTitle:BlitSLTRB(1,         X,  240+LX,    X+Width, 240+Height+LX);
      tTitle:BlitSLTRB(1, 320+Width,      Height+LY, 320,     LY);
      tTitle:BlitSLTRB(1, 320+Width,  240+Height+LY, 320, 240+LY);
      tTitle:BlitSLTRB(1, 320+Width,  480+Height+LY, 320, 480+LY);
    end
    -- Render proc
    local fcbRender = NotPlayingProc;
    local function Render() fcbRender() end;
    -- Finish function
    local function Finish()
      -- Remove evnet function
      vVideo:OnEvent(UtilBlank);
      -- When video has faded out?
      local function OnFadeOut()
        -- Destroy video and texture handles
        VideoStop();
        -- Remove framebuffer update callback
        RegisterFBUCallback("intro");
        -- Unreference loaded assets for garbage collector
        tTiles, tTitle, vVideo = nil, nil, nil;
        -- Load title screen
        InitTitle();
      end
      -- Start fading out
      Fade(0, 1, 0.032, Render, OnFadeOut, true);
    end
    -- Play video and setup event
    local function PlayVideo()
      -- A video event occured?
      local function OnEvent(iEvent)
        -- Playing?
        if iEvent == 0 then
          -- If we were in our render loop? Transition to title screen
          if fcbRender == NotPlayingProc then fcbRender = PlayingProc end;
        -- Looping? (unused)
        elseif iEvent == 1 then
        -- Stopped?
        elseif iEvent == 2 then
          -- We someone took our render callback (i.e. setup) then we need to
          -- to switch render proc
          local _, CBRender = GetCallbacks();
          if CBRender == Render then return Finish() end;
          -- If we were not in our render loop (i.e. setup?)
          fcbRender = NotPlayingProc;
        end
      end
      -- Set video event callback
      vVideo:OnEvent(OnEvent);
      -- Play the video
      VideoPlay(vVideo);
    end
    -- If not setup requested, we're done
    if not bAndSetup then PlayVideo() end;
    -- When video has faded in?
    local function OnFadeIn()
      -- Intro input procedure to just check if a button is pressed
      local function InputProc()
        if IsButtonPressed(0) then Finish() end;
      end
      -- Set intro callbacks
      SetCallbacks(nil, Render, InputProc);
      -- Load setup now
      if bAndSetup then PlayVideo() InitSetup() end;
    end
    -- Fade in
    Fade(1, 0, 0.025, Render, OnFadeIn);
  end
  -- Load resources asynchronously
  LoadResources("Intro", {{T=1,F="grass",P={16,16,0,0,0}},
                          {T=2,F="title",P={0}},
                          {T=8,F="intro",P={ }}}, OnLoaded);
end
-- Exports and imports ----------------------------------------------------- --
return { A = { InitIntro = InitIntro }, F = function(GetAPI)
  -- Imports --------------------------------------------------------------- --
  LoadResources, GetCallbacks, SetCallbacks, VideoPlay, VideoStop, Fade,
  IsButtonPressed, InitTitle, texSpr, InitSetup, RegisterFBUCallback
  = -- --------------------------------------------------------------------- --
  GetAPI("LoadResources", "GetCallbacks", "SetCallbacks", "VideoPlay",
    "VideoStop", "Fade", "IsButtonPressed", "InitTitle", "texSpr", "InitSetup",
    "RegisterFBUCallback");
  -- ----------------------------------------------------------------------- --
end };
-- End-of-File ============================================================= --
