-- END.LUA ================================================================= --
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
local floor<const>, insert<const>, abs<const> =
  math.floor, table.insert, math.abs;
-- M-Engine function aliases ----------------------------------------------- --
local UtilTableSize<const> = Util.TableSize;
-- Diggers function and data aliases --------------------------------------- --
local LoadResources, Fade, SetCallbacks, IsMouseInBounds, IsMouseNotInBounds,
  aCursorIdData, SetCursor, PlayStaticSound, aSfxData, InitCon, PlayMusic,
  IsButtonPressed, IsButtonReleased, IsScrollingDown, IsScrollingUp,
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
    -- Keep waiting cursor for animation
    SetCursor(aCursorIdData.WAIT);
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
    -- Make lines data with initial Y position
    local aLinesTop<const>, aLinesBottom<const>, aLinesCentre<const> =
      { Y=12 }, { Y=180 }, { Y=80 };
    -- Array holding top and bottom datas which are drawn together
    local aTopaLinesBottom<const> = { aLinesTop, aLinesBottom };
    -- Mark positive colour or negative
    local function Green(sValue) return "\rcff7fff7f"..sValue.."\rr" end;
    local function Red(sValue) return "\rcffff7f7f"..sValue.."\rr" end;
    local function Colourise(iValue)
      if iValue >= 0 then return Green(iValue);
      else return Red(iValue) end;
    end
    -- Function to help make lines data
    local function MakeLine(aDest, sMsg)
      -- Chosen X pixel and callback to scroll in
      local iX, fCb;
      -- Id would be even?
      if #aDest % 2 == 0 then
        -- Function to gradually scroll the messsage in from the left
        local function Increase(iX)
          -- Clamp (don't include 160 or we'll get a FP error)
          if iX >= 159 then return 160;
          -- Move the message right
          else return iX + abs(-160 + iX) * 0.1 end;
        end
        -- Set the X pixel and callback
        iX, fCb = -160, Increase;
      -- Id would be odd?
      else
        -- Function to gradually scroll the messsage in from the right
        local function Decrease(iX)
          -- Clamp (don't include 160 or we'll get a FP error)
          if iX < 161 then return 160;
          -- Move the message left
          else return iX - (iX - 160) * 0.1 end;
        end
        -- Set the X pixel and callback
        iX, fCb = 480, Decrease;
      end
      -- Insert into chosen lines
      insert(aDest, { iX, aDest.Y + (#aDest * 16), fCb, sMsg });
    end
    -- Build data for top three lines
    MakeLine(aLinesTop, sMsg);
    MakeLine(aLinesTop, "OPPONENT HAD "..Green(aOpponentPlayer.M).." ZOGS");
    MakeLine(aLinesTop, "GAME TIME WAS "..Green(iGameTime).." MINS");
    -- Build data for bottom three lines
    MakeLine(aLinesBottom,
      Green(aGlobalData.gCapitalCarried).." CAPITAL CARRIED");
    MakeLine(aLinesBottom, Red(iSalary).." SALARY PAID");
    MakeLine(aLinesBottom, Red(iDeadCost).." DEATH DUTIES");
    -- Proc a collection of lines
    local function ProcCollection(aCollection)
      for iLineId = 1, #aCollection do
        local aItem<const> = aCollection[iLineId];
        aItem[1] = aItem[3](aItem[1]);
      end
    end
    -- Draw a collection of lines
    local function DrawCollection(aCollection)
      fontLarge:SetCRGBA(1, 1, 1, 1);
      for iLineId = 1, #aCollection do
        local aItem<const> = aCollection[iLineId];
        fontLarge:PrintC(aItem[1], aItem[2], aItem[4]);
      end
    end
    -- Fade amount
    local nFade, nScale = 0, 0;
    -- Render end function
    local function RenderEnd()
      -- Render terrain and objects
      RenderTerrain();
      RenderObjects();
      -- Render animated fade
      RenderFade(nFade);
      -- Draw ending graphic
      texEnd:SetCA(nFade*2);
      local nScale<const> = nFade * 2;
      texEnd:BlitSLTRBA(iEndTexId,
        160, 120, 160+(159*nScale), 120+(95*nScale), nFade*2);
      -- Set font colour and draw lines
      for iCollectionId = 1, #aTopaLinesBottom do
        DrawCollection(aTopaLinesBottom[iCollectionId]);
      end
    end
    -- Render end win input function
    local function InputEndWin()
      -- Mouse button not clicked? Return!
      if IsButtonReleased(0) then return end;
      -- Play sound
      PlayStaticSound(aSfxData.SELECT);
      -- Wait cursor for bank text animation animation
      SetCursor(aCursorIdData.WAIT);
      -- Build data for centre lines
      MakeLine(aLinesCentre,
        Colourise(aGlobalData.gBankBalance).." IN BANK");
      MakeLine(aLinesCentre,
        Colourise(aGlobalData.gPercentCompleted).."% COMPLETED");
      MakeLine(aLinesCentre, "RAISE "..
        Colourise(aGlobalData.gZogsToWinGame-aGlobalData.gBankBalance)..
        " MORE");
      MakeLine(aLinesCentre, "ZOGS TO WIN THE GAME");
      MakeLine(aLinesCentre,
        "(REQUIRED: "..Colourise(aGlobalData.gZogsToWinGame)..")");
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
          -- Object hovered over by mouse
          local sObjectDefault<const> = "MAP POST MORTEM";
          local sObject = sObjectDefault
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
                sObject = (aObject.OD.LONGNAME or aObject.OD.NAME)..
                  " ("..aObject.H.."%)";
                -- Done
                return;
              end
            end
            -- Set generic message
            sObject = sObjectDefault;
          end
          -- Post mortem render
          local function RenderPostMortem()
            -- Render terrain and objects
            RenderTerrain();
            RenderObjects();
            -- Render post mortem banner and text
            texEnd:BlitSLT(tileBar, 8, 208);
            fontSpeech:SetCRGB(0, 0, 0.25);
            fontSpeech:PrintC(160, 215, sObject);
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
            if IsKeyHeld(iKeyLeft)  then AdjustViewPortX(-16);
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
          -- Fade in counter
          nFade = 0.5;
          -- Proc fade in
          local function LogicAnimatedPostMortem()
            -- Fade in elements
            if nFade > 0 then nFade = nFade - 0.01;
            -- Fade complete?
            elseif nFade <= 0 then
              -- Clamp fade to fully transparent
              nFade = 0;
              -- Set OK (continue) cursor
              SetCursor(aCursorIdData.OK);
              -- Set post mortem procedure
              SetCallbacks(LogicPostMortem, RenderPostMortem, InputPostMortem);
            end
          end
          -- Render fade in
          local function RenderAnimatedPostMortem()
            -- Render terrain and objects
            RenderTerrain();
            RenderObjects();
            -- Render fade in
            RenderFade(nFade);
            -- Render post mortem banner and text
            local nAdj<const> = nFade * 128;
            texEnd:BlitSLT(tileBar, 8, 208+nAdj);
            fontSpeech:SetCRGB(0, 0, 0.25);
            fontSpeech:PrintC(160, 215+nAdj, sObject);
          end
          -- Set WAIT cursor for animation
          SetCursor(aCursorIdData.WAIT);
          -- Set post mortem procedure
          SetCallbacks(LogicAnimatedPostMortem, RenderAnimatedPostMortem);
        end
        -- Load music and when finished execute the win assets handler
        LoadResources("PostMortem", {{T=7,F="win",P={}}}, OnWinLoaded);
      end
      -- We're going to reuse this value just as an input blocking timer
      nFade = 0;
      -- Render win information screen
      local function RenderWinInfo()
        -- Render terrain and objects
        RenderTerrain();
        RenderObjects();
        -- Fade backdrop
        RenderFade(0.5);
        -- Draw centre lines
        DrawCollection(aLinesCentre);
      end
      -- Proc win information screen
      local function ProcWinInfo()
        -- Fade in elements
        if nFade < 0.5 then nFade = nFade + 0.01;
        -- Fade complete?
        elseif nFade >= 0.5 then
          -- Set OK (continue) cursor
          SetCursor(aCursorIdData.OK);
          -- Clear animation procedure and set wait to click
          SetCallbacks(nil, RenderWinInfo, InputWinInfo);
        end
        -- Draw centre lines
        ProcCollection(aLinesCentre);
      end
      -- Show win information screen
      SetCallbacks(ProcWinInfo, RenderWinInfo, nil);
    end
    -- Render end input function
    local function InputEndLose()
      -- Mouse button not clicked? Return!
      if IsButtonReleased(0) then return end;
      -- Play sound
      PlayStaticSound(aSfxData.SELECT);
      -- Failed? Restart the level!
      Fade(0, 1, 0.04, RenderEnd, InitScore, true);
    end
    -- Proc end function
    local function ProcBankAnimateEnd()
      -- Animate centre lines
      ProcCollection(aLinesCentre);
    end
    -- Proc end function
    local function ProcAnimateEnd()
      -- Fade in elements
      if nFade < 0.5 then nFade = nFade + 0.01;
      -- Fade complete?
      elseif nFade >= 0.5 then
        -- Clamp fade
        nFade = 0.5;
        -- The zone was won? Clear animation proc and set input win
        if bIsComplete then
          -- Set OK (continue) cursor
          SetCursor(aCursorIdData.OK);
          -- Clear animation procedure and set wait to click
          SetCallbacks(nil, RenderEnd, InputEndWin);
        -- The one was lost? Clear animation proc and set input lose
        else
          -- Set EXIT cursor
          SetCursor(aCursorIdData.EXIT)
          -- Clear animation procedure and set wait to click
          SetCallbacks(nil, RenderEnd, InputEndLose);
        end
      end
      -- Alter animation
      for iCollectionId = 1, #aTopaLinesBottom do
        ProcCollection(aTopaLinesBottom[iCollectionId]);
      end
    end
    -- Change render procs
    SetCallbacks(ProcAnimateEnd, RenderEnd, nil);
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
