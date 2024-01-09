-- CREDITS.LUA ============================================================= --
-- ooooooo.--ooooooo--.ooooo.-----.ooooo.--oooooooooo-oooooooo.----.ooooo..o --
-- 888'-`Y8b--`888'--d8P'-`Y8b---d8P'-`Y8b-`888'---`8-`888--`Y88.-d8P'---`Y8 --
-- 888----888--888--888---------888---------888--------888--.d88'-Y88bo.---- --
-- 888----888--888--888---------888---------888oo8-----888oo88P'---`"Y888o.- --
-- 888----888--888--888----oOOo-888----oOOo-888--"-----888`8b.--------`"Y88b --
-- 888---d88'--888--`88.---.88'-`88.---.88'-888-----o--888-`88b.--oo----.d8P --
-- 888bd8P'--oo888oo-`Y8bod8P'---`Y8bod8P'-o888ooood8-o888o-o888o-8""8888P'- --
-- ========================================================================= --
-- Copyr. (c) MS-Design, 2024   Copyr. (c) Millennium Interactive Ltd., 1994 --
-- ========================================================================= --
-- Core function aliases --------------------------------------------------- --
local random<const>, remove<const>, insert<const>
      = -- ----------------------------------------------------------------- --
      math.random, table.remove, table.insert;
-- M-Engine function aliases ----------------------------------------------- --
local ConsoleWrite<const> = Console.Write;
-- Diggers function and data aliases --------------------------------------- --
local LoadResources, Fade, SetCallbacks, InitScore, PlayMusic, GetMusic,
  aCreditsData, aCreditsXData, aLevelData, aObjectTypes, LoadLevel,
  aCursorIdData, SetCursor, DeInitLevel, fontLittle, fontTiny, fontLarge,
  RenderTerrain, RenderObjects, GameProc, SelectObject, RegisterFBUCallback,
  aObjects, aPlayers, aLevelTypesData;
-- Init ending screen functions -------------------------------------------- --
local function InitCredits()
  -- When assets have loaded
  local function OnLoaded(aResources)
    -- Set vignette texture
    local texVig = aResources[1].H;
    -- Register framebuffer update
    local iStageW, iStageH, iStageL, iStageT, iStageR, iStageB, iStageM;
    local function OnFrameBufferUpdate(...)
      -- Get new stage values
      iStageW, iStageH, iStageL, iStageT, iStageR, iStageB = ...;
      -- Set middle of stage
      iStageM = iStageL + (iStageW / 2);
    end
    RegisterFBUCallback("credits", OnFrameBufferUpdate);
    -- Build levels to pick from
    local aLevels<const> = { };
    for iI = 1, #aLevelData do insert(aLevels, iI) end;
    -- Load demo levels
    local function LoadDemoLevel(iCreditId, strMusic)
      -- Action timer
      local iActionTimer = 0;
      -- Get a random level to load
      local iLevelIdPicked<const> = random(#aLevels);
      local iLevelId<const> = aLevels[iLevelIdPicked];
      remove(aLevels, iLevelIdPicked);
      -- Record if this is the last credit
      local bLast<const> = iCreditId >= #aCreditsData or #aLevels == 0;
      -- Get credit and try next credit if invalid
      local aCredit<const> = aCreditsData[iCreditId];
      if not aCredit then return LoadDemoLevel(iLevelId, iCreditId+1) end;
      -- Get texts
      local strCredits1<const>, strCredits2<const> = aCredit[1], aCredit[2];
      -- Now we need to measure the height of all three strings so we
      -- can place the credits in the exact vertical centre of the screen
      local iCredits1H<const> = fontLittle:PrintWS(iStageW, 0, strCredits1);
      local iCredits2H<const> = fontLarge:PrintWS(iStageW, 0, strCredits2);
      local iCredits1Y<const> = 228 - iCredits2H - 4 - iCredits1H;
      local iCredits2Y<const> = 228 - iCredits2H;
      -- Main render function
      local function RenderCredits()
        -- Render terrain and objects
        RenderTerrain();
        RenderObjects();
        -- Render vignette
        texVig:BlitLTRB(iStageL, iStageT, iStageR, iStageB);
      end
      -- Render function
      local function RenderCreditsBottomLeft()
        -- Render terrain, objects and vignette
        RenderCredits();
        -- Render credits
        fontLittle:Print(iStageL+16, iCredits1Y, strCredits1);
        fontLarge:Print(iStageL+16, iCredits2Y, strCredits2);
      end
      -- Render function
      local function RenderCreditsTopRight()
        -- Render terrain, objects and vignette
        RenderCredits();
        -- Render credits
        fontLittle:PrintR(iStageR-16, 16, strCredits1);
        fontLarge:PrintR(iStageR-16, 28, strCredits2);
      end
      -- Pick which way to render the credits
      local fcbCreditsRender;
      if #aLevels % 2 == 0 then fcbCreditsRender = RenderCreditsTopRight;
                           else fcbCreditsRender = RenderCreditsBottomLeft end;
      -- Credits procedure
      local function CreditsProc()
        -- Process game functions
        GameProc();
        -- Set a random object in the level
        if iActionTimer == 0 then
          -- Get number of Diggers on both sides
          local iDiggers<const> = #aPlayers[1].D + #aPlayers[2].D;
          -- Select a random object and not a digger! The last objects in
          -- the list are always the diggers so we can just crop them out.
          -- If we get an object then select and focus on the object!
          local aObject<const> = aObjects[random(#aObjects-iDiggers)];
          if aObject then SelectObject(aObject) end;
        end
        -- Should we change level?
        iActionTimer = iActionTimer + 1;
        if iActionTimer < 600 then return end;
        -- Keep rendering text during blackout loading
        SetCallbacks(nil, nil, nil);
        -- If this is the last credit?
        if bLast then
          -- Credit position
          local aXCredits<const>, iYEnd = { }, 0;
          for iIndex = 1, #aCreditsXData do
            local aCredit<const> = aCreditsXData[iIndex];
            local nHeight1<const> = fontLittle:PrintWS(iStageW, 0, aCredit[1]);
            local nHeight2<const> = fontLarge:PrintWS(iStageW, 0, aCredit[2]);
            insert(aXCredits, { iYEnd,
                                iYEnd+nHeight1+4,
                                iYEnd+nHeight1+nHeight2+16,
                                aCredit[1],
                                aCredit[2] });
            iYEnd = iYEnd + nHeight1 + nHeight2 + 20;
          end
          -- Render function
          local iY = iStageB;
          local function RenderExtra()
            -- Render terrain and objects
            RenderTerrain();
            RenderObjects();
            -- Render vignette
            texVig:BlitLTRB(iStageL, iStageT, iStageR, iStageB);

            local iI = 1;
            fontLarge:SetCRGBA(1, 1, 1, 1);
            fontLittle:SetCRGBA(0, 1, 0, 1);
            while iI <= #aXCredits do
              local aCredit<const> = aXCredits[iI];
              if iY+aCredit[3] < iStageT then
                remove(aXCredits, iI);
                if #aXCredits == 0 then
                  -- When faded out to title?
                  local function OnFadeOutTitle()
                    -- De-init the level data
                    DeInitLevel();
                    -- Unreference assets to garbage collector
                    texVig = nil;
                    -- Remove frame buffer update
                    RegisterFBUCallback("credits");
                    -- Initialise score screen
                    InitScore();
                  end
                  -- Fade to title
                  return Fade(0, 1, 0.0025, RenderExtra, OnFadeOutTitle, true);
                end
              elseif iY+aCredit[1] < iStageB then
                if iY+aCredit[2] > iStageT then
                  fontLittle:PrintC(iStageM, iY+aCredit[1], aCredit[4]);
                end
                if iY+aCredit[2] < iStageB then
                  fontLarge:PrintC(iStageM, iY+aCredit[2], aCredit[5]);
                end
                iI = iI + 1;
              elseif iY+aCredit[3] >= iStageB then break end;
            end
          end
          -- Credits procedure
          local function ExtraProc()
            -- Process game functions
            GameProc();
            -- Scroll up credits
            iY = iY - 0.25;
          end
          -- Credits input procedure
          local function ExtraInput() end
          -- When faded out to title? Load demo level
          local function OnFadeOutExtra()
            LoadLevel({ n="END", f="end", t=aLevelTypesData[4] }, nil, nil,
              true, aObjectTypes.DIGRANDOM, true, ExtraProc, RenderExtra,
              ExtraInput);
          end
          -- Fade to title
          return Fade(0, 1, 0.04, fcbCreditsRender, OnFadeOutExtra);
        end
        -- When faded out to next level?
        local function OnFadeOut() LoadDemoLevel(iCreditId+1) end;
        -- Fade out to next level
        Fade(0, 1, 0.04, fcbCreditsRender, OnFadeOut);
      end
      -- Set input procedure
      local function CreditsInput() SetCursor(aCursorIdData.ARROW) end;
      -- Load demo level
      LoadLevel(iLevelId, strMusic, nil, true,
        aObjectTypes.DIGRANDOM, true, CreditsProc, fcbCreditsRender,
        CreditsInput);
    end
    -- Load first level
    LoadDemoLevel(1, "credits");
  end
  -- Load bank texture
  LoadResources("Credits", {{T=2,F="vignette",P={0}}}, OnLoaded);
end
-- Exports and imports ----------------------------------------------------- --
return { A={ InitCredits = InitCredits }, F=function(GetAPI)
  -- Imports --------------------------------------------------------------- --
  LoadResources, SetCallbacks, Fade, InitScore, PlayMusic, GetMusic,
  aCreditsData, aCreditsXData, aLevelData, aObjectTypes, LoadLevel,
  aCursorIdData, SetCursor, DeInitLevel, fontLittle, fontTiny, fontLarge,
  RenderTerrain, RenderObjects, GameProc, SelectObject, RegisterFBUCallback,
  aObjects, aPlayers, aLevelTypesData
  = -- --------------------------------------------------------------------- --
  GetAPI("LoadResources", "SetCallbacks", "Fade", "InitScore", "PlayMusic",
    "GetMusic", "aCreditsData", "aCreditsXData", "aLevelData", "aObjectTypes",
    "LoadLevel", "aCursorIdData", "SetCursor", "DeInitLevel", "fontLittle",
    "fontTiny", "fontLarge", "RenderTerrain", "RenderObjects", "GameProc",
    "SelectObject", "RegisterFBUCallback", "aObjects", "aPlayers",
    "aLevelTypesData");
  -- ----------------------------------------------------------------------- --
end };
-- End-of-File ============================================================= --
