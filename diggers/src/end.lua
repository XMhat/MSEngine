-- END.LUA ================================================================= --
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
local floor<const>, insert<const> = math.floor, table.insert;
-- M-Engine function aliases ----------------------------------------------- --
local UtilTableSize<const> = Util.TableSize;
-- Diggers function and data aliases --------------------------------------- --
local LoadResources, Fade, SetCallbacks, IsMouseInBounds, IsMouseNotInBounds,
  aCursorIdData, SetCursor, PlayStaticSound, aSfxData, InitCon,
  PlayMusic, IsButtonPressed, IsButtonReleased, IsScrollingDown, IsScrollingUp,
  RenderFade, aObjectFlags, IsMouseXGreaterEqualThan, IsMouseYGreaterEqualThan,
  IsMouseXLessThan, IsMouseYLessThan, IsButtonHeld, aLevelData, InitLobby,
  InitScore, InitEnding, InitFail, fontLarge, aGemsAvailable, GetGameTicks,
  fontSpeech, GetAbsMousePos, RenderTerrain, RenderObjects, GetCapitalValue,
  IsSpriteCollide, AdjustViewPortX, AdjustViewPortY, aObjects, IsKeyHeld,
  aGlobalData;
-- Need some key codes ----------------------------------------------------- --
local aKeys<const> = Input.KeyCodes;
local iKeyUp<const>, iKeyDown<const>, iKeyLeft<const>, iKeyRight<const> =
  aKeys.UP, aKeys.DOWN, aKeys.LEFT, aKeys.RIGHT;
-- Initialise the lose screen ---------------------------------------------- --
local function Init(iLId, aActivePlayer, aOpponentPlayer, sMusic, iEndTexId,
  sMsg, bIsComplete)
  -- Check parameters
  assert(type(iLId)=="number", "Invalid level id specified!");
  assert(iLId>0, "Positive level id mus be specified!");
  assert(type(aActivePlayer)=="table", "Active player must be specified!");
  assert(type(aOpponentPlayer)=="table", "Opponent player must be specified!");
  assert(type(sMusic)=="string", "Music track not specified!");
  assert(type(iEndTexId)=="number", "Texture id not specified!");
  assert(type(sMsg)=="string", "Message not specified!");
  assert(type(bIsComplete)=="boolean", "Completion flag not specified!");
  -- On loaded event function
  local function OnLoaded(aResources)
    -- Play End music
    PlayMusic(aResources[2].H);
    -- Load texture
    local texEnd = aResources[1].H;
    texEnd:TileSTC(4);
    local tileBar<const> = texEnd:TileA(0, 488, 304, 512);
    -- Get cost of capital
    aGlobalData.gCapitalCarried = GetCapitalValue();
    -- Get cost of digger deaths
    local iDeadCost, iSalary = 0, 0;
    local aActivePlayerDiggers<const> = aActivePlayer.D;
    for iI = 1, #aActivePlayerDiggers do
      local aDigger<const> = aActivePlayerDiggers[iI];
      if not aDigger then
        aGlobalData.gTotalDeaths = aGlobalData.gTotalDeaths + 1;
        iDeadCost = iDeadCost + 65;
      else
        iSalary = iSalary + 30;
      end
    end
    -- Get game ticks and time
    local iGameTicks<const> = GetGameTicks();
    local iGameTime<const> = floor(iGameTicks/3600);
    -- Add data
    aGlobalData.gTotalGemsFound =
      aGlobalData.gTotalGemsFound + aActivePlayer.GEM;
    aGlobalData.gTotalGemsSold =
      aGlobalData.gTotalGemsSold + aActivePlayer.GS;
    aGlobalData.gTotalCapital =
      aGlobalData.gTotalCapital + aGlobalData.gCapitalCarried;
    aGlobalData.gTotalDeathExp =
      aGlobalData.gTotalDeathExp + iDeadCost;
    aGlobalData.gTotalPurchExp =
      aGlobalData.gTotalPurchExp + aActivePlayer.BP;
    aGlobalData.gTotalTimeTaken =
      aGlobalData.gTotalTimeTaken + floor(iGameTicks/60);
    aGlobalData.gTotalIncome =
      aGlobalData.gTotalIncome + aActivePlayer.GI;
    aGlobalData.gTotalDug =
      aGlobalData.gTotalDug + aActivePlayer.DUG;
    aGlobalData.gTotalPurchases =
      aGlobalData.gTotalPurchases + aActivePlayer.PUR;
    aGlobalData.gTotalSalaryPaid =
      aGlobalData.gTotalSalaryPaid + iSalary;
    aGlobalData.gBankBalance =
      aGlobalData.gBankBalance + (aActivePlayer.M - iDeadCost - iSalary);
    aGlobalData.gPercentCompleted =
      floor(aGlobalData.gBankBalance / aGlobalData.gZogsToWinGame * 100);
    -- Clear lines
    local topLines, bottomLines, centreLines = { }, { }, { };
    -- Set stats data
    insert(topLines, sMsg);
    insert(topLines, "OPPONENT HAD "..aOpponentPlayer.M.." ZOGS");
    insert(topLines, "GAME TIME WAS "..iGameTime.." MINS");
    insert(bottomLines, aGlobalData.gCapitalCarried.." CAPITAL CARRIED");
    insert(bottomLines, iSalary.." SALARY PAID");
    insert(bottomLines, iDeadCost.." DEATH DUTIES");
    -- Set appropriate cursor
    if bIsComplete then
      -- Next screen
      SetCursor(aCursorIdData.OK);
      -- Set data
      insert(centreLines, aGlobalData.gBankBalance.." IN BANK");
      insert(centreLines, aGlobalData.gPercentCompleted.."% COMPLETED");
      insert(centreLines, "RAISE "..
        (aGlobalData.gZogsToWinGame-aGlobalData.gBankBalance).." MORE");
      insert(centreLines, "ZOGS TO WIN THE GAME");
      insert(centreLines, "(REQUIRED:"..aGlobalData.gZogsToWinGame..")");
      -- Game over
    else SetCursor(aCursorIdData.EXIT) end;
    -- Render end function
    local function RenderEnd()
      -- Render terrain and objects
      RenderTerrain();
      RenderObjects();
      -- Fade backdrop
      RenderFade(0.5);
      -- Draw ending graphic
      texEnd:BlitSLT(iEndTexId, 80, 73);
      -- Set font colour and draw lines
      fontLarge:SetCRGBA(1, 1, 1, 1);
      for iLine = 1, #topLines do
        fontLarge:PrintC(160, -4 + (iLine * 16), topLines[iLine]) end;
      for iLine = 1, #bottomLines do
        fontLarge:PrintC(160, 164 + (iLine * 16), bottomLines[iLine]) end;
    end
    -- Render end input function
    local function InputEnd()
      -- Mouse button not clicked? Return!
      if IsButtonReleased(0) then return end;
      -- Play sound
      PlayStaticSound(aSfxData.SELECT);
      -- If level is complete?
      if bIsComplete then
        -- Render win information screen
        local function RenderWinInfo()
          -- Render terrain and objects
          RenderTerrain();
          RenderObjects();
          -- Fade backdrop
          RenderFade(0.5);
          -- Draw centre tiles
          fontLarge:SetCRGBA(1, 1, 1, 1);
          for iLine = 1, #centreLines do
            fontLarge:PrintC(160, 64 + (iLine * 16), centreLines[iLine]) end;
        end
        -- Input win info screen
        local function InputWinInfo()
          -- Mouse button not clicked? Return!
          if IsButtonReleased(0) then return end;
          -- Play sound
          PlayStaticSound(aSfxData.SELECT);
          -- When post mortem assets are loaded?
          local function OnWinLoaded(aResources)
            -- Loop End music
            PlayMusic(aResources[1].H, nil, nil, nil, 371767);
            -- Tip
            local barMessage;
            -- Post mortem logic
            local function LogicPostMortem()
              -- Get absolute mouse position on level
              local AMX<const>, AMY<const> = GetAbsMousePos();
              -- Walk through objects
              for iIndex = 1, #aObjects do
                -- Get object data and if cursor overlapping it ?
                local aObject<const> = aObjects[iIndex];
                if IsSpriteCollide(479, AMX, AMY, aObject.S,
                  aObject.X, aObject.Y) then
                  -- Set tip with name and health of object
                  barMessage = aObject.OD.NAME.." ("..aObject.H.."%)";
                  -- Done
                  return;
                end
              end
              -- Set generic message
              barMessage = "MAP POST MORTEM";
            end
            -- Post mortem render
            local function RenderPostMortem()
              -- Render terrain and objects
              RenderTerrain();
              RenderObjects();
              -- Render post mortem banner and text
              texEnd:BlitSLT(tileBar, 8, 208);
              fontSpeech:SetCRGB(0, 0, 0.25);
              fontSpeech:PrintC(160, 215, barMessage);
            end
            -- Process horizontal move?
            local function ProcHorizontalPress(iXA)
              -- Move view in specified direction
              AdjustViewPortX(iXA);
              -- Check for other keys too
              if IsKeyHeld(iKeyUp) then AdjustViewPortY(-16);
              elseif IsKeyHeld(iKeyDown) then AdjustViewPortY(16) end;
            end
            -- Process vertical move?
            local function ProcVerticalPress(iYA)
              -- Move view in specified direction
              AdjustViewPortY(iYA);
              -- Check for other keys too
              if     IsKeyHeld(iKeyLeft)  then AdjustViewPortX(-16);
              elseif IsKeyHeld(iKeyRight) then AdjustViewPortX(16) end;
            end
            -- Post mortem input
            local function InputPostMortem()
              -- Is up key pressed?
              if IsKeyHeld(iKeyUp) then ProcVerticalPress(-16);
              -- Is down key pressed?
              elseif IsKeyHeld(iKeyDown) then ProcVerticalPress(16);
              -- Is left key pressed?
              elseif IsKeyHeld(iKeyLeft) then ProcHorizontalPress(-16);
              -- Is right key pressed?
              elseif IsKeyHeld(iKeyRight) then ProcHorizontalPress(16);
              -- Mouse at top edge of screen?
              elseif IsMouseXLessThan(16) then
                SetCursor(aCursorIdData.LEFT);
                if IsButtonHeld(0) then AdjustViewPortX(-16) end;
              -- Mouse at right edge of screen?
              elseif IsMouseXGreaterEqualThan(304) then
                SetCursor(aCursorIdData.RIGHT);
                if IsButtonHeld(0) then AdjustViewPortX(16) end;
              -- Mouse at left edge of screen?
              elseif IsMouseYLessThan(16) then
                SetCursor(aCursorIdData.TOP);
                if IsButtonHeld(0) then AdjustViewPortY(-16) end;
              -- Mouse over exit point?
              elseif IsMouseYGreaterEqualThan(224) then
                -- Set exit cursor
                SetCursor(aCursorIdData.EXIT);
                -- Left mouse button pressed?
                if IsButtonPressed(0) then
                  -- Play button select sound
                  PlayStaticSound(aSfxData.SELECT);
                  -- When faded out?
                  local function OnFadeOut()
                    -- Unreference assets for garbage collection
                    texEnd = nil;
                    -- Select new level and no longer new game
                    aGlobalData.gSelectedLevel, aGlobalData.gNewGame =
                      nil, nil;
                    -- Current level completed
                    aGlobalData.gLevelsCompleted[iLId] = true;
                    -- Bank balance reached? Show good ending if bank balance
                    -- reached
                    if aGlobalData.gBankBalance >=
                       aGlobalData.gZogsToWinGame then
                      return InitEnding(aGlobalData.gSelectedRace) end;
                    -- Count number of levels completed and if all levels
                    -- completed? Show bad ending :( else back to the lobby.
                    local iNumCompleted<const> =
                      UtilTableSize(aGlobalData.gLevelsCompleted);
                    if iNumCompleted >= #aLevelData then InitFail();
                    else
                      aGlobalData.gGameSaved = false;
                      InitLobby();
                    end
                  end
                  -- Fade out...
                  Fade(0, 1, 0.04, RenderPostMortem, OnFadeOut, true);
                end
              -- Mouse over edge of bottom?
              elseif IsMouseYGreaterEqualThan(192) then
                SetCursor(aCursorIdData.BOTTOM);
                if IsButtonHeld(0) then AdjustViewPortY(16) end;
              -- Mouse not anywhere interesting?
              else SetCursor(aCursorIdData.ARROW) end;
            end
            -- Set post mortem procedure
            SetCallbacks(LogicPostMortem, RenderPostMortem, InputPostMortem);
          end
          -- Load music and when finished execute the win assets handler
          LoadResources("PostMortem", {{T=7,F="win",P={}}}, OnWinLoaded);
        end
        -- Show win information screen
        SetCallbacks(nil, RenderWinInfo, InputWinInfo);
      -- Failed? Restart the level!
      else Fade(0, 1, 0.04, RenderEnd, InitScore, true) end;
    end
    -- Change render procs
    SetCallbacks(nil, RenderEnd, InputEnd);
  end
  -- Load level ending resources
  LoadResources("ZoneEnd", {{T=1,F="end", P={159,95,1,1,0}},
                            {T=7,F=sMusic,P={ }}}, OnLoaded);
end
-- ------------------------------------------------------------------------- --
local function InitLoseDead(iLId, aP, aOP)
  Init(iLId, aP, aOP, "lose",   0, "ALL YOUR DIGGERS DIED", false) end;
local function InitWinDead(iLId, aP, aOP)
  Init(iLId, aP, aOP, "select", 1, "YOUR OPPONENT IS DEAD",  true) end;
local function InitWin(iLId, aP, aOP)
  Init(iLId, aP, aOP, "select", 2, "YOU RAISED THE CASH",    true) end;
local function InitLose(iLId, aP, aOP)
  Init(iLId, aP, aOP, "lose",   3, "YOUR OPPONENT WON",     false) end;
-- Exports and imports ----------------------------------------------------- --
return { A = {                         -- Exports
  -- Exports ------------------------------------------------------------- --
  InitWin = InitWin,                   InitWinDead = InitWinDead,
  InitLose = InitLose,                 InitLoseDead = InitLoseDead
  -- --------------------------------------------------------------------- --
  }, F = function(GetAPI)              -- Imports
  -- Imports ------------------------------------------------------------- --
  LoadResources, SetCallbacks, SetCursor, IsMouseInBounds, PlayStaticSound,
  Fade, IsMouseNotInBounds, aCursorIdData, aSfxData, InitCon, PlayMusic,
  IsButtonPressed, IsButtonReleased, IsScrollingDown, IsScrollingUp,
  RenderFade, aGemsAvailable, aObjectFlags, IsMouseXGreaterEqualThan,
  IsMouseYGreaterEqualThan, IsMouseXLessThan, IsMouseYLessThan, IsButtonHeld,
  aLevelData, InitLobby, InitScore, InitEnding, InitFail, fontLarge,
  GetGameTicks, fontSpeech, GetAbsMousePos, RenderTerrain, RenderObjects,
  GetCapitalValue, IsSpriteCollide, AdjustViewPortX, AdjustViewPortY, aObjects,
  IsKeyHeld, aGlobalData
  = -- ------------------------------------------------------------------- --
  GetAPI("LoadResources", "SetCallbacks", "SetCursor", "IsMouseInBounds",
    "PlayStaticSound", "Fade", "IsMouseNotInBounds", "aCursorIdData",
    "aSfxData", "InitCon", "PlayMusic", "IsButtonPressed", "IsButtonReleased",
    "IsScrollingDown", "IsScrollingUp", "RenderFade", "GemsAvailable",
    "aObjectFlags", "IsMouseXGreaterEqualThan", "IsMouseYGreaterEqualThan",
    "IsMouseXLessThan", "IsMouseYLessThan", "IsButtonHeld", "aLevelData",
    "InitLobby", "InitScore", "InitEnding", "InitFail", "fontLarge",
    "GetGameTicks", "fontSpeech", "GetAbsMousePos", "RenderTerrain",
    "RenderObjects", "GetCapitalValue", "IsSpriteCollide", "AdjustViewPortX",
    "AdjustViewPortY", "aObjects", "IsKeyHeld", "aGlobalData");
  -- --------------------------------------------------------------------- --
end };
-- End-of-File ============================================================= --
