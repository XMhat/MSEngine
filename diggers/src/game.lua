-- GAME.LUA ================================================================ --
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
local abs<const>, ceil<const>, error<const>, floor<const>, format<const>,
  max<const>, maxinteger<const>, min<const>, random<const>, remove<const>,
  tostring<const> =
    math.abs, math.ceil, error, math.floor, string.format, math.max,
    math.maxinteger, math.min, math.random, table.remove, tostring;
-- M-Engine function aliases ----------------------------------------------- --
local CoreLog<const>, CoreQuit<const>, CoreWrite<const>, InfoTicks<const>,
  InfoTime<const>, MaskCreateZero<const>, UtilBlank<const>, UtilClamp<const>,
  UtilClampInt<const>, UtilFormatNTime<const>, UtilIsBoolean<const>,
  UtilIsFunction<const>, UtilIsInteger<const>, UtilIsString<const>,
  UtilIsTable<const>, UtilRound<const> =
    Core.Log, Core.Quit, Core.WriteEx, Info.Ticks, Info.Time, Mask.CreateZero,
    Util.Blank, Util.Clamp, Util.ClampInt, Util.FormatNTime, Util.IsBoolean,
    Util.IsFunction, Util.IsInteger, Util.IsString, Util.IsTable, Util.Round;
-- Assets required --------------------------------------------------------- --
local aLvlTerrainAsset<const> = { T = 5, F = false, P = { } };
local aLvlObjectAsset<const>  = { T = 5, F = false, P = { } };
local aLvlTextureAsset<const> = { T = 1, F = false, P = { 16, 16, 0, 0, 0 } };
local aAssetsMusic<const>     = { aLvlTerrainAsset, aLvlObjectAsset,
    aLvlTextureAsset,           { T = 7, F = false, P = { } } };
local aAssetsNoMusic<const>   = { aLvlTerrainAsset, aLvlObjectAsset,
    aLvlTextureAsset };
local aContAssets<const>      = { { T = 7, F = "game", P = { } } };
-- Medium priority variables (because of MAXVARS limit) -------------------- --
local function MediumPriorityVars()
-- Diggers shared functions and data --------------------------------------- --
local ACT, AI, BCBlit, DF, DIR, Fade, GetCallbacks, GetMouseX, GetMouseY,
  GetMusic, GetTestMode, InitBook, InitLobby, InitLose, InitLoseDead,
  InitTNTMap, InitWin, InitWinDead, IsButtonHeld, IsButtonPressed,
  IsButtonPressedNoRelease, IsJoyPressed, IsKeyHeld, IsKeyPressed,
  IsMouseInBounds, IsMouseXLessThan, IsScrollingDown, IsScrollingUp, JOB,
  LoadResources, MFL, MNU, OFL, PlayMusic, PlaySound, PlayStaticSound,
  RegisterFBUCallback, RenderFade, RenderShadow, SetBottomRightTip,
  SetCallbacks, SetCursor, StopMusic, TYP, aAIChoicesData, aCursorIdData,
  aDigBlockData, aDigData, aDigTileData, aDugRandShaftData, aExplodeAboveData,
  aExplodeDirData, aFloodGateData, aGlobalData, aJumpFallData, aJumpRiseData,
  aLevelsData, aMenuData, aObjToUIData, aObjectData, aSfxData, aShopData,
  aShroudCircle, aShroudTileLookup, aTileData, aTileFlags, aTimerData,
  aTrainTrackData, fontLarge, fontLittle, fontTiny, iPosX, iPosY, texSpr;
-- High priority variables (because of MAXVARS limit) ---------------------- --
local function HighPriorityVars()
-- Prototype functions (assigned later) ------------------------------------ --
local CreateObject, MoveOtherObjects, PlaySoundAtObject, SetAction;
-- Locals ------------------------------------------------------------------ --
local aActiveMenu, aActiveObject, aActivePlayer, aFloodData, aGemsAvailable,
  aLevelData, aObjects, aOpponentPlayer, aPlayers, aRacesData, aRacesAvailable,
  aShroudColour, aShroudData, bAIvsAI, fcbPause, iAbsCenPosX, iAbsCenPosY,
  iAnimMoney, iGameTicks, iInfoScreen, iLevelId, iLLAbsHmVP, iLLAbsWmVP,
  iLLPixHmVP, iLLPixWmVP, iMenuBottom, iMenuLeft, iMenuRight,
  iMenuTop, iPixCenPosX, iPixCenPosY, iPixPosTargetX, iPixPosTargetY,
  iPixPosX, iPixPosY, iScrTilesH, iScrTilesHd2, iScrTilesHd2p1, iScrTilesHm1,
  iScrTilesHmVPS, iScrTilesW, iScrTilesWd2, iScrTilesWd2p1, iScrTilesWm1,
  iScrTilesWmVPS, iScrollRate, iStageB, iStageH, iStageL, iStageR,
  iStageT, iStageW, iTilesHeight, iTilesWidth, iUniqueId, iViewportH,
  iViewportW, iWinLimit, maskLev, maskSpr, maskZone, sLevelName, sLevelType,
  sMoney, sTip, texBg, texLev =
    nil, nil, nil, { }, { }, { }, { }, nil, { }, { }, { }, nil, { }, nil, nil,
    nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil,
    nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil,
    nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil,
    nil, nil, nil, nil, nil, nil, nil;
-- Level limits ------------------------------------------------------------ --
local iLLAbsW<const>   = 128;                -- Total # of horizontal tiles
local iLLAbsH<const>   = 128;                -- Total # of vertical tiles
local iLLAbsWm1<const> = iLLAbsW - 1;        -- Horizontal tiles minus one
local iLLAbsHm1<const> = iLLAbsH - 1;        -- Vertical tiles minus one
local iLLAbs<const>    = iLLAbsW * iLLAbsH;  -- Total # of tiles in level
local iLLAbsM1<const>  = iLLAbs - 1;         -- Total tiles minus one
local iLLAbsMLW<const> = iLLAbs - iLLAbsW;   -- Total tiles minus one row
local iLLPixW<const>   = iLLAbsW * 16;       -- Total # of horizontal pixels
local iLLPixH<const>   = iLLAbsH * 16;       -- Total # of vertical pixels
local iLLPixWm1<const> = iLLPixW - 1;        -- Total H pixels minus one
local iLLPixHm1<const> = iLLPixH - 1;        -- Total V pixels minus one
-- Other consts ------------------------------------------------------------ --
local iVPScrollThreshold<const> = 4;
-- Function to play a sound ------------------------------------------------ --
local function DoPlaySoundAtObject(aObject, iSfxId, nPitch)
  -- Check that object is in the players view
  local nX<const> = (aObject.X / 16) - iPosX;
  if nX < -1 or nX > iScrTilesW then return end;
  local nY<const> = (aObject.Y / 16) - iPosY;
  if nY < -1 or nY > iScrTilesH then return end;
  -- Play the sound and clamp the pan value as engine requires
  PlaySound(iSfxId,
    UtilClamp(-1 + ((nX / iScrTilesW) * 2), -1, 1), nPitch);
end
-- Enable or disable playing sounds ---------------------------------------- --
local function SetPlaySounds(bState)
  if bState then PlaySoundAtObject = DoPlaySoundAtObject;
            else PlaySoundAtObject = UtilBlank end;
end
-- Dump mask to disk ------------------------------------------------------- --
function DumpLevelMask(strFile) maskZone:Save(0, strFile or "mask") end;
-- Update viewport data ---------------------------------------------------- --
local function UpdateViewPort(nPos, iTLMVPS, iTTD2, iTT, iTL)
  -- Obey limits of level
  if nPos > iTLMVPS then nPos = iTLMVPS elseif nPos < 0 then nPos = 0 end;
  -- Reuse limit and calculate centre tile
  iTLMVPS = nPos // 16;
  -- Return calculated results
  return nPos,                    -- iPixPos[X|Y]    (Abs position)
         -(nPos % 16),            -- iPixCenPos[X|Y] (Pixel centre)
         iTLMVPS,                 -- iPos[X|Y]       (Tile position)
         iTLMVPS + iTTD2,         -- iAbsCenPos[X|Y] (Centre tile position)
         min(iTLMVPS + iTT, iTL); -- iViewport[W|H]  (Scroll pos)
end
-- Update horizontal viewport data ----------------------------------------- --
local function SetViewPortX(nX)
  iPixPosX, iPixCenPosX, iPosX, iAbsCenPosX, iViewportW =
    UpdateViewPort(nX, iLLPixWmVP, iScrTilesWd2,
      iScrTilesW, iLLAbsW);
  if iPixCenPosX < 0 then iTilesWidth = iScrTilesW;
                     else iTilesWidth = iScrTilesWm1 end;
end
-- Update vertical viewport data ------------------------------------------- --
local function SetViewPortY(nY)
  iPixPosY, iPixCenPosY, iPosY, iAbsCenPosY, iViewportH =
    UpdateViewPort(nY, iLLPixHmVP, iScrTilesHd2,
      iScrTilesH, iLLAbsWm1);
  if iPixCenPosY < 0 then iTilesHeight = iScrTilesH;
                     else iTilesHeight = iScrTilesHm1 end;
end
-- Adjust viewport --------------------------------------------------------- --
local function AdjustViewPortX(iX) SetViewPortX(iPixPosX + iX) end;
local function AdjustViewPortY(iY) SetViewPortY(iPixPosY + iY) end;
-- Scroll viewport --------------------------------------------------------- --
local function ProcessViewPort()
  -- Move horizontally if not over requested viewport
  if iPixPosTargetX < iPixPosX then
    AdjustViewPortX(-ceil((iPixPosX - iPixPosTargetX) / iScrollRate));
  elseif iPixPosTargetX > iPixPosX then
    AdjustViewPortX(ceil((iPixPosTargetX - iPixPosX) / iScrollRate)) end;
  if iPixPosTargetY < iPixPosY then
    AdjustViewPortY(-ceil((iPixPosY - iPixPosTargetY) / iScrollRate));
  elseif iPixPosTargetY > iPixPosY then
    AdjustViewPortY(ceil((iPixPosTargetY - iPixPosY) / iScrollRate)) end;
end
-- The frame buffer was updated -------------------------------------------- --
local function FrameBufferUpdated(...)
  -- Set new stage bounds
  iStageW, iStageH, iStageL, iStageT, iStageR, iStageB = ...;
  -- Set new limits based on frame buffer size
  iScrTilesW, iScrTilesH = ceil(iStageW / 16), ceil(iStageH / 16);
  -- Used in game so calculate them now to prevent unnecessary math
  iScrTilesWd2, iScrTilesHd2 = iScrTilesW // 2, iScrTilesH // 2;
  iScrTilesWd2p1, iScrTilesHd2p1 = iScrTilesWd2 + 1, iScrTilesHd2 + 1;
  iScrTilesWm1, iScrTilesHm1 = iScrTilesW - 1, iScrTilesH - 1;
  -- Level width minus viewport size
  iLLAbsWmVP, iLLAbsHmVP = iLLAbsW - iScrTilesW, iLLAbsH - iScrTilesH;
  iLLPixWmVP, iLLPixHmVP = iLLAbsWmVP * 16, iLLAbsHmVP * 16;
  -- Update viewport limits
  AdjustViewPortX(0);
  AdjustViewPortY(0);
end
-- Update new viewport ----------------------------------------------------- --
local function SetViewPort(iX, iY) SetViewPortX(iX) SetViewPortY(iY) end
-- Set instant focus on object horizontally -------------------------------- --
local function ScrollViewPortTo(iX, iY)
  iPixPosTargetX, iPixPosTargetY =
    UtilClampInt(iX, 0, iLLAbsWmVP) * 16,
    UtilClampInt(iY, 0, iLLAbsHmVP) * 16;
end
-- Force viewport position without scrolling ------------------------------- --
local function ForceViewport() SetViewPort(iPixPosTargetX, iPixPosTargetY) end;
-- Lock viewport to top left ----------------------------------------------- --
local function LockViewPort()
  ScrollViewPortTo(0, 0);
  ForceViewport();
end
-- Focus on object --------------------------------------------------------- --
local function ObjectFocus(aObj)
  -- This object is not selected? Ignore it
  if aActiveObject ~= aObj then return end;
  -- Get object absolute position
  local iX<const>, iY<const> = aObj.AX, aObj.AY;
  -- Object is almost out of the viewport?
  if (iPosX > 0 and                             -- VP not left edge? *and*
      iX - iVPScrollThreshold < iPosX) or       -- ...Near off left VP? *or*
     (iPosX < iLLAbsWmVP and                    -- ...VP not right edge? *and*
      iX + iVPScrollThreshold >= iViewportW) or -- ...Near right VP? *or*
     (iPosY > 0 and                             -- ...VP not top edge? *and*
      iY - iVPScrollThreshold < iPosY) or       -- ...Near top VP? *or*
     (iPosY < iLLAbsHmVP and                    -- ...VP not bottom edge? *and*
      iY + iVPScrollThreshold >= iViewportH) then -- ...Near bottom VP?
    -- Gradually scroll to this position centred on the object
    ScrollViewPortTo(iX - iScrTilesWd2, iY - iScrTilesHd2);
    -- If we're already off the end of the screen?
    if (abs(iX - iAbsCenPosX) > iScrTilesWd2p1 or
        abs(iY - iAbsCenPosY) > iScrTilesHd2p1) and not bAIvsAI then
      -- Set instant focus on object horizontally
      ForceViewport();
    end
  end
end
-- SelectObject ------------------------------------------------------------ --
local function SelectObject(aObj, bNow)
  -- Save active object
  local aObjActive<const> = aActiveObject;
  -- Set active object
  aActiveObject = aObj;
  -- Remove menu if different object
  if aActiveObject ~= aObjActive then aActiveMenu = nil end;
  -- Focus on object
  if aObj then ObjectFocus(aObj) if bNow then ForceViewport() end end;
end
-- Return game ticks ------------------------------------------------------- --
local function GetGameTicks() return iGameTicks end;
-- De-init the level ------------------------------------------------------- --
local function DeInitLevel()
  -- Volatile arrays to clear
  local aVolatileData<const> = { aObjects, aPlayers, aFloodData,
    aRacesAvailable, aGemsAvailable, aLevelData, aShroudData };
  -- Real function
  local function DeInitLevelInitialised()
    -- Unset FBU callback
    RegisterFBUCallback("game");
    -- Dereference loaded assets for garbage collector
    texBg, texLev, maskZone = nil, nil, nil;
    -- Clear current objects, players, flood, races and gems data
    for iVolId = 1, #aVolatileData do
      local aTable<const> = aVolatileData[iVolId];
      while #aTable > 0 do remove(aTable, #aTable) end;
    end
    -- Reset positions and other variables
    iPixPosTargetX, iPixPosTargetY, iPixPosX, iPixPosY, iGameTicks, iAnimMoney,
      iLevelId, iWinLimit, sMoney, iUniqueId =
        0, 0, 0, 0, 0, 0, nil, nil, nil, 0;
    -- Reset active objects, menus and players
    aActivePlayer, aOpponentPlayer, aActiveObject, aActiveMenu =
      nil, nil, nil, nil;
    -- We don't want to hear sounds
    SetPlaySounds(false);
  end
  -- Set real function
  DeInitLevel = DeInitLevelInitialised;
  -- Call it
  DeInitLevelInitialised();
end
-- Get level tile location from absolute ca-ordinates ---------------------- --
local function GetTileOffsetFromAbsCoordinates(iAbsX, iAbsY)
  -- Check parameters
  if not UtilIsInteger(iAbsX) then
    error("Invalid X co-ordinate! "..tostring(iAbsX)) end;
  if not UtilIsInteger(iAbsY) then
    error("Invalid Y co-ordinate! "..tostring(iAbsY)) end;
  -- Return tile offset if valid
  if iAbsX >= 0 and iAbsX < iLLAbsW and iAbsY >= 0 and iAbsY < iLLAbsH then
    return iAbsY * iLLAbsW + iAbsX end;
end
-- Get level tile location from absolute ca-ordinates ---------------------- --
local function GetLevelOffsetFromAbsCoordinates(iAbsX, iAbsY)
  -- Check parameters
  if not UtilIsInteger(iAbsX) then
    error("Invalid X co-ordinate! "..tostring(iAbsX)) end;
  if not UtilIsInteger(iAbsY) then
    error("Invalid Y co-ordinate! "..tostring(iAbsX)) end;
  -- Return location if valid
  local iLoc<const> = GetTileOffsetFromAbsCoordinates(iAbsX, iAbsY);
  if iLoc then return iLoc end;
end
-- Get zero based tile id at specified absolute location ------------------- --
local function GetLevelDataFromAbsCoordinates(iAbsX, iAbsY)
  -- Check parameters
  if not UtilIsInteger(iAbsX) then
    error("Invalid X co-ordinate! "..tostring(iAbsX)) end;
  if not UtilIsInteger(iAbsY) then
    error("Invalid Y co-ordinate! "..tostring(iAbsX)) end;
  -- Get tile at specified location and return tile id if valid
  local iLoc<const> = GetLevelOffsetFromAbsCoordinates(iAbsX, iAbsY);
  if iLoc then return aLevelData[1 + iLoc], iLoc end;
end
-- Get zero based tile id at specified absolute location ------------------- --
local function GetLevelDataFromLevelOffset(iLoc)
  -- Check parameters
  if not UtilIsInteger(iLoc) then
    error("Invalid location! "..tostring(iLoc)) end;
  -- Get tile at specified location
  if iLoc >= 0 and iLoc < iLLAbs then return aLevelData[1 + iLoc] end;
end
-- Get zero based tile id at specified absolute location ------------------- --
local function GetLevelDataFromTileOffset(iLoc)
  -- Check parameters
  if not UtilIsInteger(iLoc) then
    error("Invalid location! "..tostring(iLoc)) end;
  -- Get tile at specified location
  return GetLevelDataFromLevelOffset(iLoc);
end
-- Get tile location from pixel co-ordinates ------------------------------- --
local function GetTileOffsetFromCoordinates(iPixX, iPixY)
  -- Check parameters
  if not UtilIsInteger(iPixX) then
    error("Invalid X co-ordinate! "..tostring(iPixX)) end;
  if not UtilIsInteger(iPixY) then
    error("Invalid Y co-ordinate! "..tostring(iPixY)) end;
  -- Return location if valid
  if iPixX >= 0 and iPixX < iLLPixW and iPixY >= 0 and iPixY < iLLPixH then
    return (iPixY // 16 * iLLAbsW) + (iPixX // 16) end;
end
-- Get level tile location from co-ordinates ------------------------------- --
local function GetLevelOffsetFromCoordinates(iPixX, iPixY)
  -- Check parameters
  if not UtilIsInteger(iPixX) then
    error("Invalid X co-ordinate! "..tostring(iPixX)) end;
  if not UtilIsInteger(iPixY) then
    error("Invalid Y co-ordinate! "..tostring(iPixY)) end;
  -- Return level offset if valid
  local iLoc<const> = GetTileOffsetFromCoordinates(iPixX, iPixY);
  if iLoc then return iLoc end;
end
-- Get zero based tile id at specified location ---------------------------- --
local function GetLevelDataFromCoordinates(iPixX, iPixY)
  -- Check parameters
  if not UtilIsInteger(iPixX) then
    error("Invalid X co-ordinate! "..tostring(iPixX)) end;
  if not UtilIsInteger(iPixY) then
    error("Invalid Y co-ordinate! "..tostring(iPixY)) end;
  -- Return tile at specified location
  local iLoc<const> = GetLevelOffsetFromCoordinates(iPixX, iPixY);
  if iLoc then return aLevelData[1 + iLoc], iLoc end;
end
-- Get zero based tile id at specified object ------------------------------ --
local function GetLevelDataFromObject(aObject, iPixX, iPixY)
  -- Check parameters
  if not UtilIsTable(aObject) then
    error("Invalid object specified! "..tostring(aObject)) end;
  if not UtilIsInteger(iPixX) then
    error("Invalid X co-ordinate! "..tostring(iPixX)) end;
  if not UtilIsInteger(iPixY) then
    error("Invalid Y co-ordinate! "..tostring(iPixY)) end;
  -- Return level data from object co-ordinates
  return GetLevelDataFromCoordinates(aObject.X + aObject.OFX + iPixX,
                                     aObject.Y + aObject.OFY + iPixY);
end
-- Get tile at specified object offset pixels ------------------------------ --
local function GetTileOffsetFromObject(aObject, iPixX, iPixY)
  -- Check parameters
  if not UtilIsTable(aObject) then
    error("Invalid object specified! "..tostring(aObject)) end;
  if not UtilIsInteger(iPixX) then
    error("Invalid X co-ordinate! "..tostring(iPixX)) end;
  if not UtilIsInteger(iPixY) then
    error("Invalid Y co-ordinate! "..tostring(iPixY)) end;
  -- Return tile offset from object co-ordinates
  return GetTileOffsetFromCoordinates(aObject.X + aObject.OFX + iPixX,
                                      aObject.Y + aObject.OFY + iPixY);
end
-- Get tile at specified object offset pixels ------------------------------ --
local function GetLevelOffsetFromObject(aObject, iPixX, iPixY)
  -- Check parameters
  if not UtilIsTable(aObject) then
    error("Invalid object specified! "..tostring(aObject)) end;
  if not UtilIsInteger(iPixX) then
    error("Invalid X co-ordinate! "..tostring(iPixX)) end;
  if not UtilIsInteger(iPixY) then
    error("Invalid Y co-ordinate! "..tostring(iPixY)) end;
  -- Get tile offset from location and return level offset if valid
  local iLoc<const> = GetTileOffsetFromObject(aObject, iPixX, iPixY);
  if iLoc then return iLoc end;
end
-- Update level and level mask --------------------------------------------- --
local function UpdateLevel(iPos, iId)
  -- Check parameters
  if not UtilIsInteger(iPos) then
    error("Invalid level tile position! "..tostring(iPos)) end;
  if not UtilIsInteger(iId) then
     error("Invalid level tile index!"..tostring(iId)) end;
  -- Update level data with specified tile
  aLevelData[1 + iPos] = iId;
  -- Calculate absolute X and Y position from location
  local iX<const>, iY<const> = iPos % iLLAbsW * 16, iPos // iLLAbsW * 16;
  -- Update zone collision bit-mask
  maskZone:Copy(maskLev, iId, iX, iY);
  -- This part will keep the 1 pixel barrier around the level
  if iPos < iLLAbsW then
    -- Keep top-left corner barrier to stop objects going off screen
    if iPos == 0 then maskZone:Fill(0, 0, 16, 1);
                      maskZone:Fill(0, 0, 1, 16);
    -- Keep top-right corner barrier to stop objects going off screen
    elseif iPos == iLLAbsWm1 then
      maskZone:Fill(iX - 16, 0, 16, 1);
      maskZone:Fill(iX - 1, 0, 1, 16);
    -- Top row
    else maskZone:Fill(iX, 0, iX + 16, 1) end;
  -- Bottom row?
  elseif iPos >= iLLAbsMLW then
    -- Keep bottom-left corner barrier to stop objects going off screen
    if iPos == iLLAbsMLW then
      maskZone:Fill(0, iY, 1, 16);
      maskZone:Fill(0, iY + 15, 16, 1);
    -- Keep bottom-right corner barrier to stop objects going off screen
    elseif iPos == iLLAbsM1 then
      maskZone:Fill(iX + 15, iY, 1, 16);
      maskZone:Fill(iX, iY + 15, 16, 1);
    -- Bottom row?
    else maskZone:Fill(iX, iY + 15, 16, 1) end;
    -- Keep left side barrier to stop objects going off screen
  elseif iPos % iLLAbsW == 0 then maskZone:Fill(0, iY, 1, iY + 16);
  -- Keep right side barrier to stop objects going off screen
  elseif iPos % iLLAbsW == iLLAbsWm1 then
    maskZone:Fill(iX + 15, iY, 1, iY + 16);
  end
end
-- Can sell gem? ----------------------------------------------------------- --
local function CanSellGem(iObjId)
  -- Check parameters
  if not UtilIsInteger(iObjId) then
    error("Object not specified! "..tostring(iObjId)) end;
  -- Jennites can always be sold
  if iObjId == TYP.JENNITE then return true end;
  -- Not a Jennite so we need to check if it's a gem we can sell. If we can
  -- sell it? Then return success.
  for iGem = 1,3 do if aGemsAvailable[iGem] == iObjId then return true end end;
  -- Can't sell this gem so return failed
  return false;
end
-- Update shroud (Original Diggers only lets so much) ---------------------- --
local function UpdateShroud(iOX, iOY)
  -- For each entry in the circle
  for iI = 1, #aShroudCircle do
    -- Get the information about the circle
    local aShroudCircleItem<const> = aShroudCircle[iI];
    -- Calculate adjusted position based on object
    local iX<const>, iY<const> =
      iOX + aShroudCircleItem[1], iOY + aShroudCircleItem[2];
    -- If the co-ordinates are valid?
    if iX >= 0 and iY >= 0 and iX < iLLAbsW and iY < iLLAbsH then
      -- Get shroud flags and calc new flags data and if different?
      local aShroudItem<const> = aShroudData[iY * iLLAbsW + iX + 1];
      local iOldFlags<const> = aShroudItem[2];
      local iNewFlags<const> = iOldFlags | aShroudCircleItem[3];
      if iOldFlags ~= iNewFlags then
        -- Lookup new tile id and if theres only one? Set first tile
        local aTiles<const> = aShroudTileLookup[1 + iNewFlags];
        if #aTiles == 1 then aShroudItem[1], aShroudItem[2] =
                               aTiles[1], iNewFlags;
        -- More than one tile? Randomly select one
        else aShroudItem[1], aShroudItem[2] =
               aTiles[random(#aTiles)], iNewFlags end;
      end
    end
  end
end
-- Set object X position --------------------------------------------------- --
local function SetPositionX(aObject, iX)
  -- Calculate new absolute X position
  local iAX<const> = iX // 16;
  -- Update absolute and pixel position
  aObject.AX, aObject.X = iAX, iX;
  -- Update shroud if requested
  if aObject.US then UpdateShroud(iAX, aObject.AY) end;
  -- Refocus on object if needed
  ObjectFocus(aObject);
end
-- Set object Y position --------------------------------------------------- --
local function SetPositionY(aObject, iY)
  -- Calculate new absolute Y position
  local iAY<const> = iY // 16;
  -- Update absolute and pixel position
  aObject.AY, aObject.Y = iAY, iY;
  -- Update shroud if requested
  if aObject.US then UpdateShroud(aObject.AX, iAY) end;
  -- Refocus on object if needed
  ObjectFocus(aObject);
end
-- Set object X and Y position --------------------------------------------- --
local function SetPosition(aObject, iX, iY)
  -- Calculate new absolute X position
  local iAX<const>, iAY<const> = iX // 16, iY // 16;
  -- Update absolute and pixel position
  aObject.AX, aObject.X, aObject.AY, aObject.Y = iAX, iX, iAY, iY;
  -- Update shroud if requested
  if aObject.US then UpdateShroud(iAX, iAY) end;
  -- Refocus on object if needed
  ObjectFocus(aObject);
end
-- Trigger end condition --------------------------------------------------- --
local function TriggerEnd(fcbFunc)
  -- Parameter must be a function
  if not UtilIsFunction(fcbFunc) then
    error("End function not specified! "..tostring(fcbFunc)) end;
  -- Call the function
  fcbFunc(iLevelId, aActivePlayer, aOpponentPlayer);
end
-- Get win limit ----------------------------------------------------------- --
local function HaveZogsToWin(aPlayer) return aPlayer.M >= iWinLimit end;
-- Level end conditions check ---------------------------------------------- --
local function EndConditionsCheck()
  -- No active player? (Playing a demo?) Just ignore
  if bAIvsAI or PlaySoundAtObject == UtilBlank then return end;
  -- Player has enough Zogs?
  if HaveZogsToWin(aActivePlayer) then return TriggerEnd(InitWin) end;
  -- All the opponents Diggers have died?
  if aOpponentPlayer.DC <= 0 then return TriggerEnd(InitWinDead) end;
  -- The opponent has enough Zogs?
  if HaveZogsToWin(aOpponentPlayer) then return TriggerEnd(InitLose) end;
  -- All the players Diggers have died?
  if aActivePlayer.DC <= 0 then return TriggerEnd(InitLoseDead) end;
end
-- Destroy object ---------------------------------------------------------- --
local function DestroyObject(iObj, aObj)
  -- Check parameters
  if not UtilIsInteger(iObj) then
    error("Specified id is not an integer! "..tostring(iObj)) end;
  if not UtilIsTable(aObj) then
    error("Invalid object specified! "..tostring(aObj)) end;
  -- Object respawns?
  if aObj.F & OFL.RESPAWN ~= 0 then
    -- Restore object health
    aObj.H = 100;
    -- Move back to start position
    SetPosition(aObj, aObj.SX, aObj.SY);
    -- Get object data
    local aObjData<const> = aObj.OD;
    -- Restore object animation speed
    aObj.ANT = aObjData.ANIMTIMER;
    -- Phase back in with originally specified criteria
    SetAction(aObj, aObjData.ACTION, aObjData.JOB, aObjData.DIRECTION);
    -- Not actually destroyed
    return false;
  end
  -- Function to remove specified object from specified list
  local function RemoveObjectFromList(aList, aObj)
    -- Check parameters
    if not UtilIsTable(aList) then
      error("Specified list is not a table! "..tostring(aList)) end;
    if not UtilIsTable(aObj) then
      error("Invalid object specified! "..tostring(aObj)) end;
    -- Return if list empty
    if #aList == 0 then return end;
    -- For each object end to start because we are deleting items
    for iObj = #aList, 1, -1 do
      -- If target object is our object then delete it from the
      -- specified list else increment to the next object id.
      if aList[iObj] == aObj then remove(aList, iObj) end;
    end
  end
  -- Remove object from the global objects list
  remove(aObjects, iObj);
  -- Get objects Telepole destinations and remove them all
  local aObjTPD<const> = aObj.TD;
  for iTPIndex = #aObjTPD, 1, -1 do remove(aObjTPD, iTPIndex) end;
  -- Get objects inventory and if there are items?
  local aObjInv<const> = aObj.I;
  if #aObjInv > 0 then
    -- Remove all objects
    for iIIndex = #aObjInv, 1, -1 do remove(aObjInv, iIIndex) end;
    -- Reset weight
    aObj.IW = 0;
  end
  -- If pursuer had a target? Remove pursuer from targets pursuer list
  local aTarget<const> = aObj.T;
  if aTarget then aTarget.TL[aObj.U] = nil end;
  -- Get objects owner and if no player owner?
  local aPlayer<const> = aObj.P;
  if not aPlayer then
    -- If selected object is this object then unselect the object
    if aActiveObject == aObj then aActiveObject = nil end;
    -- Success
    return true;
  end
  -- Get digger id and if it was a Digger? Mark it as dead and reduce players'
  -- digger count.
  local iDiggerId<const> = aObj.DI;
  if iDiggerId then
    aPlayer.D[iDiggerId], aPlayer.DC = false, aPlayer.DC - 1;
  end
  -- Remove pursuers and reset pursuer targets
  local aPursuers<const> = aObj.TL;
  for iUId, aPursuer in pairs(aPursuers) do
    aPursuer.T, aPursuers[iUId] = nil, nil;
  end
  -- If selected object is this digger then disable the menu
  if aActiveObject == aObj then aActiveMenu, aActiveObject = nil, nil end;
  -- Recheck ending conditions
  EndConditionsCheck();
  -- Object removed successfully
  return true;
end
-- Destroy object without knowing the object id ---------------------------- --
local function DestroyObjectUnknown(aObject)
  -- Check id specified
  if not UtilIsTable(aObject) then
    error("Invalid object specified! "..tostring(aObject)) end;
  -- Enumerate through each global object (while because we modify the list)
  for iTargetIndex = 1, #aObjects do
    -- Get object from global list and destroy object if matches and return
    local aTargetObject<const> = aObjects[iTargetIndex];
    if aTargetObject == aObject then
      return DestroyObject(iTargetIndex, aObject) end;
    -- Try next object
    iTargetIndex = iTargetIndex + 1;
  end
end
-- Add to inventory -------------------------------------------------------- --
local function AddToInventory(aObj, aInvObj, bOnlyTreasure)
  -- Check parameters
  if not UtilIsTable(aObj) then
    error("Invalid object specified! "..tostring(aObj)) end;
  if not UtilIsTable(aInvObj) then
    error("Invalid inventory object specified! "..tostring(aInvObj)) end;
  -- Find object in objects array
  for iObj = 1, #aObjects do
    -- Get object and if it matches the specified object to pickup?
    if aObjects[iObj] == aInvObj and
      (not bOnlyTreasure or
           aInvObj.F & OFL.TREASURE ~= 0) then
      -- Remove object and add object to player inventory
      remove(aObjects, iObj);
      local aObjInv<const> = aObj.I;
      aObjInv[1 + #aObjInv] = aInvObj
      -- If object is phasing then reset the object
      if aInvObj.A == ACT.PHASE then
        SetAction(aInvObj, ACT.STOP, JOB.NONE, DIR.NONE) end;
      -- Add weight
      aObj.IW = aObj.IW + aInvObj.W;
      -- Set active inventory object to this object
      aObj.IS = aInvObj;
      -- If item picked up was active object close it and menu
      if aActiveObject == aInvObj then
        aActiveObject, aActiveMenu = nil, nil end;
      -- Success
      return true;
    end
  end
  -- Failed
  return false;
end
-- Buy an item ------------------------------------------------------------- --
local function BuyItem(aObj, iItemId)
  -- Check parameters
  if not UtilIsTable(aObj) then
    error("Invalid object specified! "..tostring(aObj)) end;
  if not UtilIsInteger(iItemId) then
    error("Invalid item id specified! "..tostring(iItemId)) end;
  -- Get object data
  local aObjData<const> = aObjectData[iItemId];
  if not UtilIsTable(aObjData) then
    error("No such item id exists! "..tostring(aObjData)) end;
  -- If objects owner doesn't have enough money or strength then failed
  local aParent<const> = aObj.P;
  local iValue<const> = aObjData.VALUE;
  if iValue > aParent.M or aObj.IW + aObjData.WEIGHT > aObj.STR or
    not AddToInventory(aObj,
      CreateObject(iItemId, aObj.X, aObj.Y, aParent)) then
        return false end;
  -- Reduce money
  aParent.M = aParent.M - iValue;
  -- Total money spend plus
  aParent.BP = aParent.BP + iValue;
  -- Total purchases plus one
  aParent.PUR = aParent.PUR + 1
  -- Success!
  return true;
end
-- Drop Object ------------------------------------------------------------- --
local function DropObject(aObj, aInvObj)
  -- Get object inventory and walk inventory
  local aInvObjs<const> = aObj.I;
  for iInvObj = 1, #aInvObjs do
    -- Object matches?
    if aInvObjs[iInvObj] == aInvObj then
      -- Remove object from inventory
      remove(aObj.I, iInvObj);
      -- Set new position of object
      SetPosition(aInvObj, aObj.X, aObj.Y);
      -- Add back to playfield
      aObjects[1 + #aObjects] = aInvObj;
      -- Reduce carrying weight
      aObj.IW = aObj.IW - aInvObj.W;
      -- Select next object
      aObj.IS = aObj.I[iInvObj];
      -- If invalid select first object
      if not aObj.IS then aObj.IS = aObj.I[1] end;
      -- Success!
      return true;
    end
  end
  -- Failed to drop object
  return false;
end
-- Sell an item ------------------------------------------------------------ --
local function SellItem(aObj, aInvObj)
  -- Check parameters
  if not UtilIsTable(aObj) then
    error("Invalid object specified! "..tostring(aObj)) end;
  if not UtilIsTable(aInvObj) then
    error("Invalid inventory object specified! "..tostring(aInvObj)) end;
  -- Remove object from inventory and return if failed
  if not DropObject(aObj, aInvObj) then return false end;
  -- Increment funds but deduct value according to damage
  local aParent<const> = aObj.P;
  aParent.M = aParent.M + floor((aInvObj.OD.VALUE / 2) * (aInvObj.H / 100));
  -- Plus time added value if treasure
  if aInvObj.F & OFL.TREASURE ~= 0 then
    aParent.GS = aParent.GS + 1;
    local iAmount<const> = iGameTicks // 18000;
    aParent.GI = aParent.GI + iAmount;
    aParent.M = aParent.M + iAmount;
  end
  -- Destroy the object
  DestroyObjectUnknown(aInvObj);
  -- Sold
  return true;
end
-- Sell all available items of specified type ------------------------------ --
local function SellSpecifiedItems(aObj, iItemId)
  -- Check parameters
  if not UtilIsTable(aObj) then
    error("Invalid object specified! "..tostring(aObj)) end;
  if not UtilIsInteger(iItemId) then
    error("Item type not specified! "..tostring(iItemId)) end;
  -- Get digger inventory and return if no items
  local aObjInv<const> = aObj.I;
  if #aObjInv == 0 then return -1 end;
  -- Something sold?
  local iItemsSold = 0;
  local iInvId = 1 while iInvId <= #aObjInv do
    -- Get object and if is a matching type and we can sell and we sold it?
    local aInvObj<const> = aObjInv[iInvId];
    local iTypeId<const> = aInvObj.ID;
    if iTypeId == iItemId and
       CanSellGem(iTypeId) and
       SellItem(aObj, aInvObj) then
      -- Sold something
      iItemsSold = iItemsSold + 1;
    -- Not sellable solid so increment inventory index
    else iInvId = iInvId + 1 end;
  end
  -- Return if we sold something
  return iItemsSold;
end
-- Sprite collides with another sprite ------------------------------------- --
local function IsSpriteCollide(S1, X1, Y1, S2, X2, Y2)
  return maskSpr:IsCollideEx(S1, X1, Y1, maskSpr, S2, X2, Y2);
end
-- Pickup Object ----------------------------------------------------------- --
local function PickupObject(aObj, aTObj, bOnlyTreasure)
  -- Return failure if target...
  if aObj == aTObj or                -- ...is me?
     aTObj.F & OFL.PICKUP == 0 or    -- *or* cant be grabbed?
     aObj.IW + aTObj.W > aObj.STR or -- *or* too heavy?
     not IsSpriteCollide(            -- *or* not touching?
       aObj.S, aObj.X+aObj.OFX, aObj.Y+aObj.OFY,
       aTObj.S, aTObj.X+aTObj.OFX, aTObj.Y+aTObj.OFY) then
    return false end;
  -- Add object to objects inventory
  return AddToInventory(aObj, aTObj, bOnlyTreasure);
end
-- Pickup Objects ---------------------------------------------------------- --
local function PickupObjects(aObj, bOnlyTreasure)
  -- Look for objects that can be picked up
  local iObj = 1 while iObj <= #aObjects do
    -- Try to pickup specified object and return success if succeeded
    if PickupObject(aObj, aObjects[iObj], bOnlyTreasure) then return true end;
    -- Try next object
    iObj = iObj + 1;
  end
  -- Failed!
  return false;
end
-- Set a random action, job and direction ---------------------------------- --
local function SetRandomJob(aObject, bUser)
  -- Select a random choice
  local aChoice = aAIChoicesData[random(#aAIChoicesData)];
  -- Failed direction matches then try something else
  if aChoice[1] == aObject.FDD then aChoice = aChoice[2];
  -- We're not blocked from digging so try moving in that direction
  else aChoice = aChoice[3] end;
  -- Set new AI choice and return
  SetAction(aObject, aChoice[1], aChoice[2], aChoice[3], bUser);
end
-- AI player override patience logic --------------------------------------- --
local function AIPatienceLogic(aObject)
  -- Return if...
  if (aObject.F & OFL.BUSY ~= 0 and -- Digger is busy? -AND-
     aObject.A ~= ACT.REST) or      -- Digger is NOT resting? -OR-
     aObject.JT < aObject.PL then   -- Digger is not at impatience limit?
    return end;
  -- If have rest ability? (25% chance to execute). Use it and return
  if aObject.OD[ACT.REST] and random() < 0.25 then
    return SetAction(aObject, ACT.REST, JOB.NONE, DIR.NONE);
  end
  -- Do something casual
  SetRandomJob(aObject, true);
end
-- Add player -------------------------------------------------------------- --
local function CreatePlayer(iX, iY, iPlayerId, iRaceId, bIsAI)
  -- Check parameters
  if not UtilIsInteger(iX) then
    error("X coord not an integer! "..tostring(iX)) end;
  if iX < 0 then
    error("X coord "..iX.." must be positive!") end;
  if iX >= iLLAbsW then
    error("X coord "..iX.." limit is "..iLLAbsW.."!") end;
  if not UtilIsInteger(iY) then
    error("X coord not an integer! "..tostring(iY)) end;
  if iY < 0 then
    error("Y coord "..iY.." must be positive!") end;
  if iY >= iLLAbsW then
    error("Y coord "..iY.." limit is "..iLLAbsH.."!") end;
  if not UtilIsInteger(iPlayerId) then
    error("Player id is invalid! "..tostring(iPlayerId)) end;
  if aPlayers[iPlayerId] then
    error("Player "..iPlayerId.." already initialised!") end;
  if not UtilIsInteger(iRaceId) then
    error("Race id is invalid! "..tostring(iRaceId)) end;
  if not UtilIsBoolean(bIsAI) then
    error("AI boolean is invalid! "..tostring(bIsAI)) end;
  if #aRacesAvailable == 0 then
    error("No races are available!") end;
  -- Digger in races list picked
  local iRacesId;
  -- If random digger requested?
  if iRaceId == TYP.DIGRANDOM then
    -- Pick a random race from the races array
    iRacesId = random(#aRacesAvailable);
    -- Override the random race type with an actual race type
    iRaceId = aRacesAvailable[iRacesId];
  -- Actually specified specific race
  else
    -- For each available race
    iRacesId = 1;
    while iRacesId <= #aRacesAvailable do
      -- Get race type id and if we matched requested race then break
      if aRacesAvailable[iRacesId] == iRaceId then break end;
      -- Try next races id
      iRacesId = iRacesId + 1;
    end
    -- Wasn't able to remove anything? Show error
    if iRacesId > #aRacesAvailable then
      error("Race "..iRaceId.." not available!") end;
  end
  -- Remove the race from the table so it can't be duplicated
  remove(aRacesAvailable, iRacesId);
  -- Players diggers and number of diggers to create
  local aDiggers<const>, iNumDiggers<const> = { }, 5;
  -- Calculate home point
  local iHomeX, iHomeY<const> = iX * 16 - 2, iY * 16 + 32;
  -- Get object data for race
  local aRaceData<const> = aObjectData[iRaceId];
  if not UtilIsTable(aRaceData) then
    error("Invalid race data for "..iRaceId.."! "..tostring(aRaceData)) end;
  -- Build player data object
  local aPlayer<const> = {
    AI  = bIsAI,                       -- Set AI status
    D   = aDiggers,                    -- List of diggers
    DT  = iNumDiggers,                 -- Diggers total
    DC  = iNumDiggers,                 -- Diggers count
    DUG = 0,                           -- Dirt dug
    GEM = 0,                           -- Gems found
    GS  = 0,                           -- Gems sold
    PUR = 0,                           -- Purchases made
    BP  = 0,                           -- Purchases made in Zogs
    GI  = 0,                           -- Total income
    M   = 100,                         -- Money
    R   = iRaceId,                     -- Race type (TYP.*)
    I   = iPlayerId,                   -- Player index
    HX  = iHomeX,                      -- Home point X position
    HY  = iHomeY,                      -- Home point Y position
    SX  = (iX - 1) * 16,               -- Adjust home point X
    SY  = (iY + 2) * 16,               -- Adjust home point Y
    RD  = aRaceData                    -- Race data
  };
  -- If this is player one
  if iPlayerId == 1 then
    -- Set as my player
    aActivePlayer = aPlayer;
    -- Is not AI?
    if not bIsAI then
      -- Set to un-shroud the players' objects
      aPlayer.US = true;
      -- Set viewpoint on this player and synchronise
      ScrollViewPortTo(iX - iScrTilesWd2, iY - iScrTilesHd2);
      ForceViewport();
      -- Add capital carried and reset its value
      aActivePlayer.M = aActivePlayer.M + aGlobalData.gCapitalCarried;
      aGlobalData.gCapitalCarried = 0;
      -- Not demo mode
      bAIvsAI = false;
      -- Fast scrolling
      iScrollRate = 16;
    -- Not AI vs AI
    else
      -- Slow scrolling
      iScrollRate = 32;
      -- Demo mode
      bAIvsAI = true;
    end
  -- Opponent player? Set Opponent
  elseif iPlayerId == 2 then aOpponentPlayer = aPlayer end;
  -- Adjust starting X co-ordinate for first Digger at the trade centre
  iHomeX = iHomeX - 16;
  -- Get weight of treasure
  local iMaxInventory<const> = aRaceData.STRENGTH;
  -- For each digger of the player
  for iDiggerId = 1, iNumDiggers do
    -- Create a new digger
    local aDigger<const> = CreateObject(iRaceId, iHomeX, iHomeY, aPlayer);
    if aDigger then
      -- Digger is not AI?
      if not bIsAI then
        -- Set patience AI for player controlled digger
        aDigger.AI, aDigger.AIF = AI.PATIENCE, AIPatienceLogic;
        -- Set and verify patience warning value
        local iPatience = aDigger.OD.PATIENCE;
        if not UtilIsInteger(iPatience) then
          error("Digger "..iDiggerId.." of player "..aPlayer.I..
            "has no patience warning value!") end;
        -- Randomise patience by +/- 25%
        local iOffset = random(floor(iPatience * 0.25));
        if random() < 0.5 then iOffset = -iOffset end;
        iPatience = iPatience + iOffset;
        aDigger.PW = iPatience;
        -- Digger will stray between 30-60 seconds of indicated impatience
        aDigger.PL = iPatience + 1800 + random(1800);
      -- Is AI?
      else
        -- Set maximum treasure items to carry (for AI)
        aDigger.MI = iMaxInventory;
        -- Initialise Digger AI anti-wriggle system
        aDigger.AW, aDigger.AWR = 0, 0;
      end;
      -- Set Digger id
      aDigger.DI = iDiggerId;
      -- Insert into Digger list
      aDiggers[1 + #aDiggers] = aDigger;
    -- Failed so show map maker in console that the object id is invalid
    else CoreWrite("Warning! Digger "..iDiggerId..
      " not created for player "..iPlayerId.."! Id="..iRaceId..", X="..iX..
      ", Y="..iY..".", 9) end;
    -- Increment home point X position
    iHomeX = iHomeX + 5;
  end
  -- Add player data to players array
  aPlayers[1 + #aPlayers] = aPlayer;
  -- Log creation of item
  CoreLog("Created player "..iPlayerId.." as '"..aObjectData[iRaceId].NAME..
    "'["..iRaceId.."] at AX:"..iX..",AY:"..iY.." in position #"..
    #aPlayers.."!");
end
-- Object is at home ------------------------------------------------------- --
local function ObjectIsAtHome(aObject)
  -- Check parameter is valid
  if not UtilIsTable(aObject) then
    error("Invalid object specified! "..tostring(aObject)) end;
  -- Make sure object has an owner
  local aPlayer<const> = aObject.P;
  if not UtilIsTable(aPlayer) then
    error("Object has invalid parent! "..tostring(aPlayer)) end;
  -- Return if object is at the home point
  return aObject.X == aPlayer.HX and aObject.Y == aPlayer.HY;
end
-- Cycle object inventory -------------------------------------------------- --
local function CycleObjInventory(aObject, iDirection)
  -- Get object inventory and enumerate through them
  local aInv<const> = aObject.I;
  for iInvIndex = 1, #aInv do
    -- Get inventory object and if we got it
    local aInvObj<const> = aInv[iInvIndex];
    if aInvObj == aObject.IS then
      -- Cycle object wrapping on low or high
      aObject.IS = aInv[1 + (((iInvIndex - 1) + iDirection) % #aInv)];
      -- Success
      return true;
    end
  end
  -- Failure
  return false;
end
-- Set object action ------------------------------------------------------- --
local function InitSetAction()
  -- Deployment of train track
  local function DEPLOYTrack(O)
    -- Deploy success
    local bDeploySuccess = false;
    -- Check 5 tiles at object position and lay track
    for I = 0, 4 do
      -- Calculate absolute location of object and get tile id. Break if bad
      local iId, iLoc<const> = GetLevelDataFromObject(O, (I * 16) + 8, 15);
      if not iId then break end;
      -- Check if it's a tile we can convert and break if we can't
      iId = aTrainTrackData[iId];
      if not iId then break end;
      -- Get terrain tile id blow this tile and if we can deploy on this?
      if aTileData[1 + aLevelData[1 + iLoc + iLLAbsW]] &
        aTileFlags.F ~= 0 then
        -- Update level data
        UpdateLevel(iLoc, iId);
        -- Deployed successfully and continue
        bDeploySuccess = true;
      end
    end
    -- Deploy if succeeded
    return bDeploySuccess;
  end
  -- Deployment of flood gate
  local function DEPLOYGate(O)
    -- Calculate absolute location of object below and if valid and the tile
    -- below it is firm ground? Also creation of an invisible flood gate
    -- object was successful?
    local iId, iLoc<const> = GetLevelDataFromObject(O, 8, 16);
    if iId and aTileData[1 + iId] & aTileFlags.F ~= 0 and
      CreateObject(TYP.GATEB,
        iLoc % iLLAbsW * 16,
        (iLoc - iLLAbsW) // iLLAbsW * 16, O.P) then
      -- Update tile to a flood gate
      UpdateLevel(iLoc - iLLAbsW, 438);
      -- Success!
      return true;
    end
    -- Failed
    return false;
  end
  -- Deployment functions
  local function DEPLOYLift(O)
    -- Calculate absolute location of object
    local iLoc<const> = GetLevelOffsetFromObject(O, 8, 0);
    -- Search for a buildable ground surface
    for iBottom = iLoc, iLLAbsMLW, iLLAbsW do
      -- Get tile
      local iId<const> = aLevelData[1 + iBottom];
      local iTileId<const> = aTileData[1 + iId];
      -- Tile has not been dug
      if iTileId & aTileFlags.AD == 0 then
        -- If we're on firm ground?
        if iTileId & aTileFlags.F ~= 0 then
          -- Search for a buildable above ground surface
          for iTop = iLoc, iLLAbsW, -iLLAbsW do
            -- Get tile
            local iId<const> = aLevelData[1 + iTop];
            local iTileId<const> = aTileData[1 + iId];
            -- Tile has not been dug
            if iTileId & aTileFlags.AD == 0 then
              -- Tile is firm buildable ground?
              if iTileId & aTileFlags.F ~= 0 then
                -- Height check and if ok and creating an object went ok?
                -- Create lift object
                if iTop >= iLLAbsW and iBottom-iTop >= 384 and
                  CreateObject(TYP.LIFTB,
                    (O.X + 8) // 16 * 16,
                    (O.Y + 15) // 16 * 16, O.P) then
                  -- Update level data for top and bottom part of lift
                  UpdateLevel(iTop, 62);
                  UpdateLevel(iBottom, 190);
                  -- Draw cable
                  for iTop = iTop + iLLAbsW,
                    iBottom - iLLAbsW, iLLAbsW do
                    -- Update level data for bottom part of lift
                    UpdateLevel(iTop, 189);
                  end
                  -- We are deploying now so set success
                  return true;
                end
              end
              -- Done
              break;
            end
          end
        end
        -- Done
        break;
      end
    end
    -- Failed to deploy
    return false;
  end
  -- Deployment lookup table
  local aDeployments<const> = {
    [TYP.TRACK] = DEPLOYTrack,
    [TYP.GATE]  = DEPLOYGate,
    [TYP.LIFT]  = DEPLOYLift,
  }
  -- Deploy specified device
  local function ACTDeployObject(O)
    -- Get deploy function and if deployable?
    local fcbDeployFunc<const> = aDeployments[O.ID];
    if fcbDeployFunc and fcbDeployFunc(O) then
      -- Destroy the object
      DestroyObjectUnknown(O)
      -- Success
      return true, true;
    end
    -- Failed
    return true, false;
  end
  -- Jump requested?
  local function ACTJump(O)
    -- Object is...
    if O.A ~= ACT.FIGHT and            -- ...not fighting *and*
       O.F & OFL.BUSY == 0 and         -- ...not busy *and*
       O.F & OFL.JUMPRISE == 0 and     -- ...not jumping *and*
       O.F & OFL.JUMPFALL == 0 and     -- ...not jump falling *and*
       O.FS == 1 and                   -- ...not actually falling *and*
       O.FD == 0 then                  -- ...not accumulating fall damage
      -- Remove fall flag and add busy and jumping flags
      O.F = (O.F | OFL.BUSY | OFL.JUMPRISE) & ~OFL.FALL;
      -- Play jump sound
      PlaySoundAtObject(O, aSfxData.JUMP);
      -- Reset action timer
      O.AT = 0;
      -- Jump succeeded
      return true, true;
    end
    -- Jump failed
    return true, false;
  end
  -- Dying or eaten requested?
  local function ACTDeathOrEaten(O)
    -- Remove jump and fall flags or if the digger is jumping then busy will
    -- be unset and they will be able to instantly come out of phasing.
    O.F = O.F & ~(OFL.JUMPRISE | OFL.JUMPFALL);
    -- Force normal timer speed for animation
    O.ANT = aTimerData.ANIMNORMAL;
    -- Continue execution of function
    return false;
  end
  -- Display map requested?
  local function ACTDisplayMap()
    -- Remove play sound function
    SetPlaySounds(false);
    -- Display map
    InitTNTMap();
    -- Halt further execution of function
    return true, true;
  end
  -- Open or close a gate
  local function ACTOpenCloseGate(O, A)
    -- Get location at specified tile and if id is valid?
    local iId<const>, iLoc<const> = GetLevelDataFromAbsCoordinates(O.AX, O.AY);
    if iId then
      -- Location updater and sound player
      local function UpdateFloodGate(iTileId, iSfxId)
        -- Update level with specified id and play requested sound effect
        UpdateLevel(iLoc, iTileId);
        PlaySoundAtObject(O, iSfxId);
        -- Halt further execution of function
        return true, true;
      end
      -- Open gate?
      if A == ACT.OPEN then
        -- Gate closed (no water any side)?
        if iId == 434 then
          -- Set open non-flooded gate tile and halt further exec of function
          return UpdateFloodGate(438, aSfxData.GOPEN);
        -- Gate closed (water on left, right or both sides)?
        elseif iId >= 435 and iId <= 437 then
          -- Check if opening caused a flood
          aFloodData[1 + #aFloodData] = { iLoc, aTileData[1 + iId] };
          -- Set flooded open gate and halt further execution of function
          return UpdateFloodGate(439, aSfxData.GOPEN);
        end
      -- Closed gate and gate open? (water on neither side)?
      elseif iId == 438 then
        -- Set non-flooded gate tile and halt further execution of function
        return UpdateFloodGate(434, aSfxData.GCLOSE);
      -- Gate open (water on both sides)?
      elseif iId == 439 then
        -- Set flooded gate tile and halt further execution of function
        return UpdateFloodGate(437, aSfxData.GCLOSE);
      end
    end
    -- Failed so halt further execution of function
    return true, false;
  end
  -- Grab requested?
  local function ACTGrabItem(O)
    return true, PickupObjects(O, false);
  end
  -- Drop requested?
  local function ACTDropItem(O)
    return true, DropObject(O, O.IS);
  end
  -- Next inventory item requested?
  local function ACTNextItem(O)
    return true, CycleObjInventory(O, 1);
  end
  -- Previous inventory item requested?
  local function ACTPreviousItem(O)
    return true, CycleObjInventory(O, -1);
  end
  -- Phase requested?
  local function ACTPhase(O, _, J, D)
    -- Phasing home? Refuse action if not enough health
    if J == JOB.PHASE and D == DIR.U and O.H <= 5 and
      O.F & OFL.TPMASTER == 0 then return true, false end;
    -- Remove jump and fall flags or if the digger is jumping then busy will
    -- be unset and they will be able to instantly come out of phasing.
    O.F = O.F & ~(OFL.JUMPRISE | OFL.JUMPFALL);
    -- Continue function execution
    return false;
  end
  -- Actions to perform depending on action. They return a boolean and if
  -- false then execution of the action continues, else the action is blocked
  -- from further processing and an additional boolean is returned of the
  -- success of that action (used by the the player interface).
  local aActions<const> = {
    [ACT.DEATH] = ACTDeathOrEaten,     [ACT.EATEN]  = ACTDeathOrEaten,
    [ACT.MAP]   = ACTDisplayMap,       [ACT.OPEN]   = ACTOpenCloseGate,
    [ACT.CLOSE] = ACTOpenCloseGate,    [ACT.DEPLOY] = ACTDeployObject,
    [ACT.JUMP]  = ACTJump,             [ACT.GRAB]   = ACTGrabItem,
    [ACT.DROP]  = ACTDropItem,         [ACT.NEXT]   = ACTNextItem,
    [ACT.PREV]  = ACTPreviousItem,     [ACT.PHASE]  = ACTPhase,
  };
  -- Going left or right?
  local function DIRLeftRight(_, A, J, D)
    -- 50% chance to go left?
    if random() < 0.5 then return A, J, DIR.L end;
    -- Else go right
    return A, J, DIR.R;
  end
  -- Going up or down?
  local function DIRUpDown(_, A, J, D)
    -- 50% chance to go left?
    if random() < 0.5 then return A, J, DIR.U end;
    -- Else go right
    return A, J, DIR.D;
  end
  -- Going to centre of tile (to dig down)?
  local function DIRMoveToCentre(O, A, J, D)
    -- Set direction so it heads to the centre of the tile
    if O.X % 16 - 8 < 0 then D = DIR.L else D = DIR.R end;
    -- Return original parameters
    return A, J, D;
  end
  -- Move towards trade centre?
  local function DIRMoveHomeward(O, A, J, D)
    -- Preserve action but action stopped? Set object walking
    if A == ACT.KEEP and O.A == ACT.STOP then A = ACT.WALK end;
    -- Go left if homeward is to the left
    if O.X < O.P.HX then return A, J, DIR.R end;
    -- Go right if homeward is to the right
    if O.X > O.P.HX then return A, J, DIR.L end;
    -- Return original parameters but no direction
    return A, J, DIR.NONE;
  end
  -- Keep original direction
  local function DIRKeep(O, A, J, D) return A, J, O.D end;
  -- Opposite directions
  local aOpposites<const> = {
    [DIR.UL] = DIR.UR, [DIR.L] = DIR.R, [DIR.DL] = DIR.DR,
    [DIR.UR] = DIR.UL, [DIR.R] = DIR.L, [DIR.DR] = DIR.DL;
  };
  -- Keep original direction if moving
  local function DIRKeepIfMoving(O, A, J, D)
    -- If going in a recognised moving direction? Don't change direction
    if aOpposites[O.D] then return A, J, O.D end;
    -- Set a random direction
    return DIRLeftRight(O, A, J, D);
  end
  -- Go opposite direction
  local function DIROpposite(O, A, J, D)
    -- Set opposite direction or just go right
    D = aOpposites[O.D] or DIR.R;
    -- Return original parameters
    return A, J, D;
  end
  -- Actions to perform depending on direction
  local aDirections<const> = {
    [DIR.LR]       = DIRLeftRight,     [DIR.TCTR]     = DIRMoveToCentre,
    [DIR.HOME]     = DIRMoveHomeward,  [DIR.KEEP]     = DIRKeep,
    [DIR.KEEPMOVE] = DIRKeepIfMoving,  [DIR.OPPOSITE] = DIROpposite,
    [DIR.UD]       = DIRUpDown,
  };
  -- Actions to ignore for job in danger function
  local aActionsToIgnore<const> = { [ACT.DEATH] = true, [ACT.PHASE] = true };
  -- Performed when object is in danger
  local function JOBInDanger(aObject, iJob)
    -- Keep busy unset if not dead or phasing!
    if not aActionsToIgnore[aObject.A] then
      aObject.F = aObject.F & ~OFL.BUSY end;
    -- Return originally set job
    return iJob;
  end
  -- Keep existing job but don't dig down?
  local function JOBKeepNoDigDown(aObject, iJob)
    -- Get current job and is digging down? Remove job
    iJob = aObject.J;
    if iJob == JOB.DIGDOWN then return JOB.NONE end;
    -- Return object's current job
    return iJob;
  end
  -- Keep existing job
  local function JOBKeep(aObject, iJob) return aObject.J end;
  -- Actions to perform depending on job
  local aJobs<const> = {
    [JOB.INDANGER] = JOBInDanger,
    [JOB.KNDD]     = JOBKeepNoDigDown,
    [JOB.KEEP]     = JOBKeep
  };
  -- Do set action function
  local function SetAction(aObject, iAction, iJob, iDirection,
    bResetJobTimer)
    -- Check parameters
    if not UtilIsTable(aObject) then
      error("Invalid object table specified! "..tostring(aObject)) end;
    if not UtilIsInteger(iAction) then
      error("Invalid action integer specified! "..tostring(iAction)) end;
    if not UtilIsInteger(iJob) then
      error("Invalid job integer specified! "..tostring(iJob)) end;
    if not UtilIsInteger(iDirection) then
      error("Invalid direction integer specified! "..tostring(iDirection)) end;
    -- Get action function and if we have one?
    local fcbActionCallback<const> = aActions[iAction];
    if fcbActionCallback then
      -- Call the function and return results
      local bBlock, bResult =
        fcbActionCallback(aObject, iAction, iJob, iDirection);
      -- If callback said to block further execution return execution result
      if bBlock then return bResult end;
    end
    -- Reset action timer
    aObject.AT = 0;
    -- Get direction function and if we have one? Call it and set new args
    local fcbDirCallback<const> = aDirections[iDirection];
    if fcbDirCallback then iAction, iJob, iDirection =
      fcbDirCallback(aObject, iAction, iJob, iDirection) end;
    -- Get job function and if we have one? Call it and set new args
    local fcbJobCallback<const> = aJobs[iJob];
    if fcbJobCallback then iJob = fcbJobCallback(aObject, iJob) end;
    -- Get object data
    local aObjInitData<const> = aObject.OD;
    -- Compare action. Stop requested?
    if iAction == ACT.STOP then
      -- If object can stop? Keep busy unset!
      if aObject.CS then aObject.F = aObject.F & ~OFL.BUSY;
      -- Can't stop? Set default action and move in opposite direction
      else return DoSetAction(aObject, aObjInitData.ACTION,
                    JOB.KEEP, DIR.OPPOSITE) end;
    -- Keep existing job? Keep existing action!
    elseif iAction == ACT.KEEP then iAction = aObject.A end;
    -- If...
    if iAction == aObject.A and     -- ...action is same as requested?
       iJob == aObject.J and        -- *and* job is same as requested?
       iDirection == aObject.D then -- *and* direction is same as requested?
      -- Reset job timer if user initiated
      if bResetJobTimer then aObject.JT = 0 end;
      -- Success regardless
      return true;
    end
    -- Set new action, direction and job
    aObject.A, aObject.J, aObject.D = iAction, iJob, iDirection;
    -- Remove all flags that are related to object actions
    local iDirFlags<const> = aObject.AD.FLAGS or OFL.NONE;
    aObject.F = aObject.F & ~iDirFlags;
    -- Set action data according to lookup table
    local aAction<const> = aObjInitData[iAction];
    if not UtilIsTable(aAction) then
      error(aObjInitData.NAME.." actdata for "..iAction..
        " not found!"..tostring(aAction)) end;
    aObject.AD = aAction;
    -- If object has patience?
    local iPatience<const> = aObject.PW;
    if iPatience then
      -- Object starts as impatient? Set impatient
      if iDirFlags & OFL.IMPATIENT ~= 0 then aObject.JT = iPatience;
      -- Reset value if the user made this action
      elseif bResetJobTimer then aObject.JT = 0 end;
    -- Patience disabled
    else aObject.JT = 0 end;
    -- Set directional data according to lookup table
    local aDirection<const> = aAction[iDirection];
    if not UtilIsTable(aDirection) then
      error(aObjInitData.NAME.." dirdata for "..iAction.."->"..iDirection..
        " not found! "..tostring(aDirection)) end;
    aObject.DD = aDirection;
    -- Get and check starting sprite id
    local iSprIdBegin<const> = aDirection[1];
    if not UtilIsInteger(iSprIdBegin) then
      error(aObjInitData.NAME.." dirdata for "..iAction.."->"..iDirection..
        " missing starting sprite! "..tostring(iSprIdBegin)) end;
    aObject.S1 = iSprIdBegin;
    -- Get and check ending sprite id
    local iSprIdEnd<const> = aDirection[2];
    if not UtilIsInteger(iSprIdEnd) then
      error(aObjInitData.NAME.." dirdata for "..iAction.."->"..iDirection..
        " missing starting sprite! "..tostring(iSprIdEnd)) end;
    aObject.S2 = iSprIdEnd;
    -- Set optional sprite draw offset
    aObject.OFX, aObject.OFY = aDirection[3] or 0, aDirection[4] or 0;
    -- Random tile requested?
    if aObject.F & OFL.RNGSPRITE ~= 0 then
      -- Get random sprite id
      local iSprite<const> = random(0) % (iSprIdEnd - iSprIdBegin);
      -- Does a new animation id need to be set?
      -- Set first animation frame and reset animation timer
      aObject.S, aObject.ST = iSprIdBegin + iSprite, iSprite;
    -- No random tile requested
    else
      -- Get current sprite id and does a new animation id need to be set?
      local iSprite<const> = aObject.S;
      if iSprite < iSprIdBegin or iSprite > iSprIdEnd then
        -- Set first animation frame and reset animation timer
        aObject.S, aObject.ST = iSprIdBegin, 0;
      end
    end
    -- Get and check attachment id
    local iAttTypeId<const> = aObject.STA;
    if iAttTypeId then
      -- Check that its valid
      if not UtilIsInteger(iAttTypeId) then
        error(aObjInitData.NAME.."'s specified attachment id is invalid! "..
          tostring(iAttTypeId)) end;
      -- Get and check object data for attachment
      local aAttObjectData<const> = aObjectData[iAttTypeId];
      if not UtilIsTable(aAttObjectData) then
        error(aObjInitData.NAME.."'s specified attachment id #"..iAttTypeId..
          " is invalid! "..tostring(aAttObjectData)) end;
      -- Get and check action data for attachment
      local aAttAction<const> = aAttObjectData[iAction];
      if not UtilIsTable(aAttAction) then
        error(aObjInitData.NAME.."'s specified attachment id #"..iAttTypeId..
          " actdata for "..iAction.." invalid! "..tostring(aAttAction)) end;
      aObject.AA = aAttAction;
      -- Set object data
      local aAttDirection<const> = aAttAction[iDirection];
      if not UtilIsTable(aAttDirection) then
        error(aObjInitData.NAME.."'s specified attachment id #"..iAttTypeId..
          " dirdata for "..iAction.." > "..iDirection.." invalid! "..
          tostring(aAttDirection)) end;
      aObject.DA = aAttDirection;
      -- Get and check attachment starting sprite id
      local iAttSprIdBegin<const> = aAttDirection[1];
      if not UtilIsInteger(iAttSprIdBegin) then
        error(aObjInitData.NAME.."'s specified attachment id #"..iAttTypeId..
          " dirdata for "..iAction.."->"..iDirection..
          " missing starting sprite! "..tostring(iAttSprIdBegin)) end;
      aObject.S1A = iAttSprIdBegin;
      -- Get and check attachment ending sprite id
      local iAttSprIdEnd<const> = aAttDirection[2];
      if not UtilIsInteger(iAttSprIdEnd) then
        error(aObjInitData.NAME.."'s specified attachment id #"..iAttTypeId..
          " dirdata for "..iAction.."->"..iDirection..
          " missing starting sprite! "..tostring(iAttSprIdEnd)) end;
      aObject.S2A = iAttSprIdEnd;
      -- Get and set new attachment sprite
      local iAttSprite<const> = aObject.SA;
      if iAttSprite < iAttSprIdBegin or iAttSprite > iAttSprIdEnd then
        aObject.SA = iAttSprIdBegin end;
      -- Set optional offset according to attachment
      aObject.OFXA, aObject.OFYA =
        aAttDirection[3] or 0, aAttDirection[4] or 0;
    -- Set no attachment
    end
    -- Re-add flags according to lookup table
    aObject.F = aObject.F | (aAction.FLAGS or 0);
    -- Get optional sound id and optional pitch and if specified?
    local iSoundId = aAction.SOUND;
    if iSoundId then
      -- Check it
      if not UtilIsInteger(iSoundId) then
        error(aObjInitData.NAME.." dirdata for "..iAction.."->"..iDirection..
          " has invalid sound id! "..tostring(iSoundId)) end;
      -- Play the sound now with no panning
      PlaySoundAtObject(aObject, iSoundId);
    -- No sound id?
    else
      -- Check for random pitch sound and if specified?
      iSoundId = aAction.SOUNDRP;
      if iSoundId then
        -- Check it
        if not UtilIsInteger(iSoundId) then
          error(aObjInitData.NAME.." dirdata for "..iAction.."->"..iDirection..
            " has invalid random pitch sound id! "..tostring(iSoundId)) end;
        -- Play sound with random pitch
        PlaySoundAtObject(aObject, iSoundId, 0.975 + (random() % 0.05));
      end
    end
    -- Success
    return true;
  end
  -- Return real functions (initialised at the bottom)
  return SetAction;
end
-- Roll the dice to spawn treasure at the specified location --------------- --
local function RollTheDice(nX, nY)
  -- Get chance to reveal a gem
  local nChance = aTimerData.GEMCHANCE;
  -- Add up to double chance depending on depth
  if nY >= aTimerData.GEMDEPTHEXTRA then
    nChance = nChance + (((nY - aTimerData.GEMDEPTHEXTRA) /
      aTimerData.GEMDEPTHEXTRA) * nChance) end;
  -- 5% chance to spawn a treasure
  if random() > nChance then return end
  -- Spawn a random object from the treasure data array and return success
  return not not CreateObject(aDigTileData[random(#aDigTileData)], nX, nY);
end
-- Roll the dice to spawn treasure at the specified location --------------- --
local function SetObjectAndParentCounter(aObject, sWhat)
  -- Increase objects gem find count
  aObject[sWhat] = aObject[sWhat] + 1;
  -- And of the objects owner if it has one
  local aPlayer<const> = aObject.P;
  if aPlayer then aPlayer[sWhat] = aPlayer[sWhat] + 1 end;
end
-- Dig tile at specified position ------------------------------------------ --
local function DigTile(aObject)
  -- Initialise centre location of tile
  local DP, CP, CId, TId, BId, AId, LId, RId;
  -- Get dig tile data
  local function GetDigTileData()
    -- Get the tile id that the object is on now
    CId = aLevelData[1 + CP];
    -- Get the tile above and adjacent the object
    if DP >= iLLAbsW then
      TId = aLevelData[1 + (DP - iLLAbsW)] else TId = 0 end
    -- Get the tile below and adjacent to the object
    if DP < iLLAbs - iLLAbsW then
      BId = aLevelData[1 + (DP + iLLAbsW)] else BId = 0 end;
    -- Get the tile adjacent to the object
    AId = aLevelData[1 + DP];
    -- Get the tile left of the object
    if DP - 2 >= 0 then LId = aLevelData[1 + (DP - 2)] else LId = 0 end;
    -- Get the tile right of the object
    if DP + 2 < iLLAbs then RId = aLevelData[1 + DP + 2]
                             else RId = 0 end;
  end
  -- Cache digging direction of object and if going left or up-left?
  local iDirection<const> = aObject.D;
  if iDirection == DIR.L or iDirection == DIR.UL then
    if iDirection == DIR.UL then
      CP = GetLevelOffsetFromObject(aObject, 8, 15) or 0;
    else CP = 0 end;
    DP = GetLevelOffsetFromObject(aObject, 5, 15) or 0;
    GetDigTileData();
    local TDLId<const>, TDTId<const> = aTileData[1 + LId], aTileData[1 + TId];
    if TDTId & aTileFlags.P ~= 0 or
      (TDLId & aTileFlags.W ~= 0 and TDLId & aTileFlags.ER ~= 0) or
      (TDTId & aTileFlags.W ~= 0 and TDTId & aTileFlags.EB ~= 0) then
      return false end;
  -- Going downright?
  elseif iDirection == DIR.DL then
    CP, DP = GetLevelOffsetFromObject(aObject, 8, 1) or 0,
             GetLevelOffsetFromObject(aObject, 5, 1) or 0;
    GetDigTileData();
    local TDBId<const>, TDLId<const>, TDTId<const> =
      aTileData[1 + BId], aTileData[1 + LId], aTileData[1 + TId];
    if TDTId & aTileFlags.P ~= 0 or
      (TDBId & aTileFlags.W ~= 0 and TDBId & aTileFlags.ET ~= 0) or
      (TDLId & aTileFlags.W ~= 0 and TDLId & aTileFlags.ER ~= 0) or
      (TDTId & aTileFlags.W ~= 0 and TDTId & aTileFlags.EB ~= 0) then
      return false end;
  -- Going right or upright?
  elseif iDirection == DIR.R or iDirection == DIR.UR then
    if iDirection == DIR.UR then
      CP = GetLevelOffsetFromObject(aObject, 8, 15) or 0;
    else CP = 0 end;
    DP = GetLevelOffsetFromObject(aObject, 10, 15) or 0;
    GetDigTileData();
    local TDTId<const>, TDRId<const> = aTileData[1 + TId], aTileData[1 + RId];
    if TDTId & aTileFlags.P ~= 0 or
      (TDRId & aTileFlags.W ~= 0 and TDRId & aTileFlags.EL ~= 0) or
      (TDTId & aTileFlags.W ~= 0 and TDTId & aTileFlags.EB ~= 0) then
      return false end;
  -- Going downright?
  elseif iDirection == DIR.DR then
    CP, DP = GetLevelOffsetFromObject(aObject, 8, 1) or 0,
             GetLevelOffsetFromObject(aObject, 10, 1) or 0;
    GetDigTileData();
    local TDBId<const>, TDRId<const>, TDTId<const> =
      aTileData[1 + BId], aTileData[1 + RId], aTileData[1 + TId];
    if TDTId & aTileFlags.P ~= 0 or
      (TDBId & aTileFlags.W ~= 0 and TDBId & aTileFlags.ET ~= 0) or
      (TDRId & aTileFlags.W ~= 0 and TDRId & aTileFlags.EL ~= 0) or
      (TDTId & aTileFlags.W ~= 0 and TDTId & aTileFlags.EB ~= 0) then
      return false end;
  -- Going down?
  elseif iDirection == DIR.D then
    CP, DP = 0, GetLevelOffsetFromObject(aObject, 8, 16) or 0;
    GetDigTileData();
    local TDLId<const>, TDTId<const>, TDRId<const>, TDBId<const> =
      aTileData[1 + LId], aTileData[1 + TId], aTileData[1 + RId],
      aTileData[1 + BId];
    if TDTId & aTileFlags.P ~= 0 or
      (TDBId & aTileFlags.W ~= 0 and TDBId & aTileFlags.ET ~= 0) or
      (TDLId & aTileFlags.W ~= 0 and TDLId & aTileFlags.ER ~= 0) or
      (TDRId & aTileFlags.W ~= 0 and TDRId & aTileFlags.EL ~= 0) then
      return false end;
  else return false end;
  -- Get digging data table
  local aDigDataItem<const> = aDigData[iDirection];
  if not aDigDataItem then return false end;
  -- Walk through all the digger data structs to find info about current tile
  for iI = 1, #aDigDataItem do
    -- Get dig data and data about the specific tile to check for
    local aDigItem<const> = aDigDataItem[iI];
    local FL<const>,   FO<const>,   FA<const>,   FB<const>,
          FC<const>,   TO<const>,   TA<const>,   TB<const> =
          aDigItem[8], aDigItem[1], aDigItem[2], aDigItem[3],
          aDigItem[4], aDigItem[5], aDigItem[6], aDigItem[7];
    -- Perform the checks
    if (FL & DF.MO == 0 or (FL & DF.MO ~= 0 and AId == FO)) and
       (FL & DF.MA == 0 or (FL & DF.MA ~= 0 and TId == FA)) and
       (FL & DF.MB == 0 or (FL & DF.MB ~= 0 and BId == FB)) and
       (FL & DF.MC == 0 or (FL & DF.MC ~= 0 and CId == FC)) then
      -- Terrain should change?
      if FL & DF.SO ~= 0 then
        -- Check ToOver and convert random shaft tiles
        local aDugData<const> = aDugRandShaftData[TO];
        if aDugData then AId = aDugData[random(#aDugData)];
        else AId = TO end;
        -- Successful dig should search for treasure and if successful? Then
        -- Increment the gem found counter for object and it's parent
        if FL & DF.OG ~= 0 and RollTheDice(aObject.X, aObject.Y) then
          SetObjectAndParentCounter(aObject, "GEM") end;
      end
      -- Set tiles if needed
      if FL & DF.SA ~= 0 then TId = TA end;
      if FL & DF.SB ~= 0 then BId = TB end;
      -- Set digger flags if needed
      if FL & DF.OB ~= 0 then aObject.F = aObject.F | OFL.BUSY end;
      if FL & DF.OI ~= 0 then aObject.F = aObject.F & ~OFL.BUSY end;
      -- If tile location is not at the top of the level. Update above tile
      if DP >= iLLAbsW then
        UpdateLevel(DP-iLLAbsW, TId) end;
      -- Update level
      UpdateLevel(DP, AId);
      -- If tile location is not at the bottom of the level. Update below tile
      if DP < iLLAbs-iLLAbsW then
        UpdateLevel(DP+iLLAbsW, BId) end;
      -- Dig was successful
      return true;
    end
  end
  -- Dig failed
  return false;
end
-- Set object health ------------------------------------------------------- --
local function AdjustObjectHealth(aObject, iAmount)
  -- Calculate new health amount and if still alive?
  local iNewHealth<const> = aObject.H + iAmount;
  if iNewHealth > 0 then
    -- Clamp at a 100% if needed or update the objects new health
    if iNewHealth > 100 then aObject.H = 100 else aObject.H = iNewHealth end;
    -- Do not do anything else
    return;
  end;
  -- Object is dead so clamp health to zero or update the objects new health
  if iNewHealth < 0 then aObject.H = 0 else aObject.H = iNewHealth end;
  -- Kill object (Don't move this, for explosion stuff to work)
  SetAction(aObject, ACT.DEATH, JOB.INDANGER, DIR.NONE);
  -- Remove jump and floating status from object
  aObject.F = aObject.F & ~(OFL.JUMPRISE|OFL.JUMPFALL);
  -- Object explodes on death?
  if aObject.F & OFL.EXPLODE ~= 0 then
    -- Enumerate possible destruct positions again. We can't have the TERRAIN
    -- destruction checks in the above enumeration because of the recursive
    -- nature of the OBJECT destruction which would cause problems.
    for iExplodeIndex = 1, #aExplodeDirData do
      -- Get destruct adjacent position data
      local aCoordAdjust<const> = aExplodeDirData[iExplodeIndex];
      -- Clamp the centre tile position of the explosion for the level
      local iX<const>, iY<const> =
        (aObject.X + 8) // 16 + aCoordAdjust[1],
        (aObject.Y + 8) // 16 + aCoordAdjust[2];
      -- Calculate locate of tile and if in valid bounds?
      local iId, iLoc<const> = GetLevelDataFromAbsCoordinates(iX, iY);
      if iId then
        -- Get position
        local iPosX<const>, iPosY<const> = iX*16, iY*16;
        -- Compare against all objects
        for iObject = 1, #aObjects do
          -- Get target object data and if not the same object?
          local aTarget<const> = aObjects[iObject];
          if aTarget ~= aObject then
            -- Get action and if target object...
            local iAction<const> = aTarget.A;
            if iAction ~= ACT.DEATH and        -- ...is not dying?
               iAction ~= ACT.PHASE and        -- *and* not phasing?
               IsSpriteCollide(476, iPosX, iPosY, -- *and* in explosion?
                 aTarget.S, aTarget.X+aTarget.OFX, aTarget.Y+aTarget.OFY) then
              AdjustObjectHealth(aTarget, -100);
            end
          end
        end
        -- Get tile flags and if tile is destructible and not been cleared?
        local iTFlags = aTileData[1 + iId];
        if iTFlags & aTileFlags.D ~= 0 and iTFlags & aTileFlags.AD == 0 then
          -- Increase dug count
          SetObjectAndParentCounter(aObject, "DUG");
          -- Roll the dice and spawn treasure and increase objects gem find
          -- count if found
          if RollTheDice(iX * 16, iY * 16) then
            SetObjectAndParentCounter(aObject, "GEM") end;
          -- Tile blown does not contain water?
          if iTFlags & aTileFlags.W == 0 then
            -- Set cleared dug tile
            UpdateLevel(iLoc, 7);
            -- Test for flooding around the cleared tile
            for iFloodIndex = 1, #aExplodeDirData do
              -- Get flood test data and calculate location to test
              local aFloodTestItem<const> = aExplodeDirData[iFloodIndex];
              local iTLoc<const> = (iLoc + (aFloodTestItem[2] *
                iLLAbsW)) + aFloodTestItem[1];
              -- Get tile id and if valid
              iId = GetLevelDataFromLevelOffset(iTLoc);
              if iId then
                -- Get flags to test for and insert a new flood if found
                local iTFFlags<const> = aFloodTestItem[3];
                if aTileData[1 + iId] & iTFFlags == iTFFlags then
                  aFloodData[1 + #aFloodData] = { iTLoc, iTFFlags };
                end
              end
            end
          else
            -- Set cleared water tile
            UpdateLevel(iLoc, 247);
            -- Test for flood here with all edges exposed
            aFloodData[1 + #aFloodData] = { iLoc, aTileFlags.W|aTileFlags.EA };
          end
          -- Tile blown was firm ground?
          if iTFlags & aTileFlags.F ~= 0 then
            -- Get tile location above
            local iTLoc<const> = iLoc - iLLAbsW;
            -- Get tile id and if valid?
            iId = GetLevelDataFromLevelOffset(iTLoc);
            if iId then
              -- Get above tile flags and if is a gate?
              local iATFlags<const> = aTileData[1 + iId];
              if iATFlags & aTileFlags.G ~= 0 then
                -- Find gate at that position
                for iObjId = 1, #aObjects do
                  -- Get object and if it's a deployed gate? Get its absolute
                  -- location and if it's the same? Destroy the deployed gate.
                  local aObject<const> = aObjects[iObjId];
                  if aObject.ID == TYP.GATEB and
                    GetLevelOffsetFromObject(aObject, 0, 0) == iTLoc then
                      AdjustObjectHealth(aObject, -100) end;
                end
                -- Is watered gate? Set watered cleared tile else normal clear
                if iATFlags & aTileFlags.W ~= 0 then UpdateLevel(iTLoc, 247);
                                                else UpdateLevel(iTLoc, 7) end;
                -- Check if removed gate would cause a flood
                aFloodData[1 + #aFloodData] = { iTLoc, aTileFlags.EA };
              -- Not a gate?
              else
                -- Is a supported tile that we should clear
                local iToTile<const> = aExplodeAboveData[iId];
                if iToTile then UpdateLevel(iTLoc, iToTile) end;
              end
            end
          end
        end
      end
    end
  end
  -- Drop all objects
  while aObject.IS do DropObject(aObject, aObject.IS) end;
  -- Disable menu if object is selected and menu open
  if aActiveObject == aObject and aActiveMenu then aActiveMenu = nil end;
end
-- Render terrain ---------------------------------------------------------- --
local function RenderTerrain()
  -- Render the backdrop
  texLev:BlitSLT(texBg,
    -iLLAbsW + ((iLLAbsW - iPosX) / iLLAbsW * 64),
    -32 + ((iLLAbsH - iPosY) / iLLAbsH * 32));
  -- Calculate the X pixel position to draw at
  local iXdraw<const> = iStageL + iPixCenPosX;
  -- For each screen row to draw tile at
  for iY = 0, iTilesHeight do
    -- Calculate the Y position to grab from the level data
    local iYdest<const> = 1 + (iPosY + iY) * iLLAbsW;
    -- Calculate the Y pixel position to draw at
    local iYdraw<const> = iStageT + iPixCenPosY + (iY * 16);
    -- For each screen column to draw tile at, draw the tile from level data
    for iX = 0, iTilesWidth do
      texLev:BlitSLT(aLevelData[iYdest + iPosX + iX],
        iXdraw + (iX * 16), iYdraw);
    end
  end
end
-- Render shroud data ------------------------------------------------------ --
local function RenderShroud()
  -- Calculate the X pixel position to draw at
  local iXdraw<const> = iStageL + iPixCenPosX;
  -- Set shroud colour
  texSpr:SetCRGB(aShroudColour[1], aShroudColour[2], aShroudColour[3]);
  -- For each screen row to draw tile at
  for iY = 0, iTilesHeight do
    -- Calculate the Y position to grab from the level data
    local iYdest<const> = 1 + (iPosY + iY) * iLLAbsW;
    -- Calculate the Y pixel position to draw at
    local iYdraw<const> = iStageT + iPixCenPosY + (iY * 16);
    -- For each screen column to draw tile at
    for iX = 0, iTilesWidth do
      -- Get shroud information at specified tile and draw it if theres data
      local aItem<const> = aShroudData[iYdest + iPosX + iX];
      if aItem[2] < 0xF then
        texSpr:BlitSLT(aItem[1], iXdraw + (iX * 16), iYdraw);
      end
    end
  end
  -- Restore shroud colour
  texSpr:SetCRGB(1.0, 1.0, 1.0);
end
-- Render all objects ------------------------------------------------------ --
local function RenderObjects()
  -- Calculate viewpoint position
  local nVPX<const>, nVPY<const> = iPixPosX - iStageL, iPixPosY - iStageT;
  -- For each object
  for iObjId = 1, #aObjects do
    -- Get object data
    local aObject<const> = aObjects[iObjId];
    -- Holds objects render position on-screen
    local iXX, iYY = aObject.X - nVPX + aObject.OFX,
                     aObject.Y - nVPY + aObject.OFY;
    BCBlit(texSpr, aObject.S, iXX, iYY, iXX + 16, iYY + 16);
    -- Got an attachment? Draw it too!
    if aObject.STA then
      iXX, iYY = iXX + aObject.OFXA, iYY + aObject.OFYA;
      BCBlit(texSpr, aObject.SA, iXX, iYY, iXX + 16, iYY + 16);
    end
  end
end
-- Render an information frame --------------------------------------------- --
local function DrawInfoFrameAndTitle(iTileId)
  -- Draw the left part of the title bar
  texSpr:BlitSLT(847, 8, 8);
  -- Draw the middle part of the title bar
  for iColumn = 1, 17 do texSpr:BlitSLT(848, 8 + (iColumn * 16), 8) end;
  -- Draw the right part of the title bar
  texSpr:BlitSLT(849, 296, 8);
  -- Print the title bar text
  fontLittle:PrintC(160, 12, iTileId);
  -- Draw transparent backdrop
  RenderFade(0.75, 8, 32, 312, 208);
  -- Draw frame
  texSpr:BlitSLT(850, 8, 32);
  for iX = 24, 280, 16 do texSpr:BlitSLT(851, iX, 32) end;
  texSpr:BlitSLT(852, 296, 32);
  for iY = 48, 176, 16 do
    texSpr:BlitSLT(856, 8, iY);
    texSpr:BlitSLT(858, 296, iY);
  end
  texSpr:BlitSLT(853, 8, 192);
  for iX = 24, 280, 16 do texSpr:BlitSLT(854, iX, 192) end;
  texSpr:BlitSLT(855, 296, 192);
  -- Draw shadows
  RenderShadow(8, 8, 312, 24);
  RenderShadow(8, 32, 312, 208);
end
-- Draw health bar --------------------------------------------------------- --
local function DrawHealthBar(iHealth, iDivisor, iL, iT, iR, iB)
  -- Red if health danger
  if iHealth >= 50 then
    texSpr:SetCRGB(1, 1, (iHealth-50)/50)
    texSpr:BlitSLTRB(1022, iL, iT, iR + iHealth / iDivisor, iB);
    texSpr:SetCRGB(1, 1 ,1);
  elseif iHealth > 0 then
    texSpr:SetCRGB(1, iHealth/50, 0);
    texSpr:BlitSLTRB(1022, iL, iT, iR + iHealth / iDivisor, iB);
    texSpr:SetCRGB(1, 1 ,1);
  end
end
-- Render Interface -------------------------------------------------------- --
local function RenderInterface()
  -- Render terrain
  RenderTerrain();
  -- Render game objects
  RenderObjects();
  -- Render shroud
  RenderShroud();
  -- Render shadows around ui parts at button
  RenderShadow(8, 216, 136, 232);
  RenderShadow(144, 216, 224, 232);
  RenderShadow(232, 216, 312, 232);
  -- Draw bottom left part, money and health
  for iColumn = 0, 6 do
    texSpr:BlitSLT(821 + iColumn, 8 + (iColumn * 16), 216) end;
  -- What object is selected?
  if aActiveObject then
    -- Which indicator to draw?
    local iStatusId;
    -- Object does not have an owner? Draw 'O' indicator above object to
    -- indicate only the program can control this.
    local aParent<const> = aActiveObject.P;
    if not aParent then iStatusId = 980;
    -- If object has an owner but this client is not that owner? Draw 'X'
    -- indicator above object to indicate only the owner can control this.
    elseif aParent ~= aActivePlayer then iStatusId = 976;
    -- Parent is me?
    else
      -- If object is busy? Draw 'Zz' indicator above object to indicate
      -- control is temporarily disabled
      if aActiveObject.F & OFL.BUSY ~= 0 then iStatusId = 984;
      -- 'v' is default, free to control this object
      else iStatusId = 988 end;
      -- Get health and if object has health?
      local iHealth<const> = aActiveObject.H;
      if iHealth > 0 then
        -- Is a digger?
        if aActiveObject.DI then
          -- Draw a pulsating heart
          texSpr:BlitSLT(797 +
            (iGameTicks // (1 + (iHealth // 5))) % 4, 47, 216);
          -- Digger has inventory?
          if aActiveObject.IW > 0 then
            -- X position of object on UI and tile ID
            local iX = 0;
            -- For each inventory
            local aObjInv<const> = aActiveObject.I;
            for iObjIndex = 1, #aObjInv do
              -- Get object
              local aObj<const> = aObjInv[iObjIndex];
              -- Get inventory conversion id and if we got it then draw it
              local iObjConvId<const> = aObjToUIData[aObj.ID];
              if iObjConvId then texSpr:BlitSLT(iObjConvId, 61 + iX, 218);
              -- Draw as resized sprite
              else texSpr:BlitSLTRB(aObj.S, 61 + iX, 218, 69 + iX, 226) end;
              -- Increase X position
              iX = iX + 8;
            end
          end
        end
        -- Draw health bar
        DrawHealthBar(iHealth, 2, 61, 227, 61, 229);
      end
    end
    -- Draw the indicator
    texSpr:BlitSLT(iGameTicks // 5 % 4 + iStatusId,
      iStageL + iPixCenPosX + aActiveObject.X - iPosX * 16,
      iStageT + iPixCenPosY + aActiveObject.Y - iPosY * 16 - 16);
  end
  -- Tile to draw
  local iTile;
  -- Draw digger buttons and activity
  for iDiggerId = 1, 5 do
    -- Pre-calculate Y position
    local iY<const> = iDiggerId * 16;
    -- Get digger data and if Digger is alive?
    local aDigger<const> = aActivePlayer.D[iDiggerId];
    if aDigger then
       -- Digger is selected?
      if aActiveObject and aDigger == aActiveObject then
        -- Show lightened up button
        texSpr:BlitSLT(808 + iDiggerId, 128 + iY, 216)
        -- Object is jumping?
        if aActiveObject.F & OFL.JUMPMASK ~= 0 then iTile = 836;
        -- Not jumping?
        else
          -- Get Digger action and if stopped?
          local iObjAction<const> = aActiveObject.A;
          if iObjAction == ACT.STOP then iTile = 834;
          -- If fighting?
          elseif iObjAction == ACT.FIGHT then iTile = 840;
          -- If teleporting?
          elseif iObjAction == ACT.PHASE then iTile = 841;
          -- Something else?
          else
            -- Get Digger job and if home or inside the home?
            local iObjJob<const> = aActiveObject.J;
            if iObjJob == JOB.HOME or
               iObjAction == ACT.HIDE then iTile = 838;
            -- If searching for treasure?
            elseif iObjJob == JOB.SEARCH then iTile = 839;
            -- If walking or running?
            elseif iObjAction == ACT.WALK or
                   iObjAction == ACT.RUN then iTile = 835;
            -- If digging?
            elseif iObjAction == ACT.DIG then iTile = 837 end;
          end
        end
      -- Show dimmed button
      else texSpr:BlitSLT(803 + iDiggerId, 128 + iY, 216) end;
      -- Status tile to draw
      local iStatusTile;
      -- Digger is in danger?
      if aDigger.J == JOB.INDANGER then
        -- Every second? Play error sound each second
        if iGameTicks % 60 == 0 then PlayStaticSound(aSfxData.ERROR) end;
        -- Every even second set a different blue indicator.
        if iGameTicks % 120 < 60 then iStatusTile = 831;
                                 else iStatusTile = 832 end;
      -- Digger is in impatient and every even second?
      elseif aDigger.JT >= aDigger.PW and
             iGameTicks % 120 < 60 then iStatusTile = 833;
      -- Not in danger but busy?
      elseif aDigger.F & OFL.BUSY ~= 0 then iStatusTile = 830;
      -- Not in danger, not busy but doing something?
      elseif aDigger.A ~= ACT.STOP then iStatusTile = 829;
      -- Not in danger, not busy and not doing something
      else iStatusTile = 828 end;
      -- Show activity indicator (78-84)
      texSpr:BlitSLT(iStatusTile, 128 + iY, 204);
    -- Digger is not alive! Show dimmed button
    else texSpr:BlitSLT(803 + iDiggerId, 128 + iY, 216) end;
  end
  -- Get player and opponent money
  local iPlayerMoney<const>, iOpponentMoney<const> =
    aActivePlayer.M, aOpponentPlayer.M;
  -- Tile was set? Draw it
  if iTile then texSpr:BlitSLT(iTile, 120, 216);
  -- No tile was set
  else
    -- Set tile id based on diggers count
    local iATile, iOTile = 868 + aOpponentPlayer.DC, 874 + aActivePlayer.DC;
    -- Get monies and increase flag depending on who has more money
    if iPlayerMoney > iOpponentMoney then iOTile = iOTile + 1;
    elseif iPlayerMoney < iOpponentMoney then iATile = iATile + 1 end;
    -- Draw flags for both sides
    texSpr:BlitSLT(iATile, 120, 216);
    texSpr:BlitSLT(iOTile, 120, 216);
  end
  -- Animate player one's money
  local iMoney<const> = aActivePlayer.M;
  if iAnimMoney ~= iMoney then
    -- Animated money over actual money?
    if iAnimMoney > iMoney then
      -- Decrement it
      iAnimMoney = iAnimMoney - ceil((iAnimMoney - iMoney) * 0.1);
      -- Update displayed money
      sMoney = format("%04u", min(9999, iAnimMoney));
      -- Red colour, draw money and reset colour
      fontLittle:SetCRGB(1, 0.75, 0.75);
      fontLittle:Print(15, 220, sMoney);
      fontLittle:SetCRGB(1, 1, 1);
    -- Animated money under actual money? Increment
    elseif iAnimMoney < iMoney then
      -- Increment it
      iAnimMoney = iAnimMoney + ceil((iMoney - iAnimMoney) * 0.1);
      -- Update displayed money
      sMoney = format("%04u", min(9999, iAnimMoney));
      -- Green colour, draw money and reset colour
      fontLittle:SetCRGB(0.75, 1, 0.75);
      fontLittle:Print(15, 220, sMoney);
      fontLittle:SetCRGB(1, 1, 1);
    -- No change so set white font
    else
      -- Reset colour and draw money
      fontLittle:SetCRGB(1, 1, 1);
      fontLittle:Print(15, 220, sMoney);
    end
  -- Animated money/actual money is synced, display blue if > 9999
  elseif iMoney > 9999 then
    -- Set other colour and draw money
    fontLittle:SetCRGB(0.75, 0.75, 1);
    fontLittle:Print(15, 220, sMoney);
  -- Normal display
  else
    -- Reset colour and draw money
    fontLittle:SetCRGB(1, 1, 1);
    fontLittle:Print(15, 220, sMoney);
  end
  -- Draw utility button
  texSpr:BlitSLT(814, 232, 216);
  -- Inventory button pressed?
  if iInfoScreen == 1 then
    -- Draw frame and title
    DrawInfoFrameAndTitle("DIGGER INVENTORY");
    -- Set tiny font spacing
    fontTiny:SetLSpacing(2);
    -- For each digger
    for iDiggerId = 1, #aActivePlayer.D do
      -- Calculate Y position
      local iY<const> = iDiggerId * 33;
      -- Print id number of digger
      fontLarge:Print(16, iY + 8, iDiggerId);
      -- Draw health bar background
      texSpr:BlitSLTRB(1023, 24, iY + 31, 291, iY + 33);
      -- Get Digger data and if it exists?
      local aDigger<const> = aActivePlayer.D[iDiggerId];
      if aDigger then
        -- Draw digger health bar
        DrawHealthBar(aDigger.H, 0.375, 24, iY + 31, 24, iY + 33);
        -- Draw digger portrait
        texSpr:BlitSLT(aDigger.S, 31, iY + 8);
        -- Digger has items?
        if aDigger.IW > 0 then
          -- Get digger inventory and enumerate through it and draw it
          local aInventory<const> = aDigger.I;
          for iInvIndex = 1, #aInventory do
            texSpr:BlitSLT(aInventory[iInvIndex].S,
              iInvIndex * 16 + 32, iY + 8) end;
        -- No inventory. Print no inventory message
        else fontTiny:Print(48, iY + 13, "NOT CARRYING ANYTHING") end;
        -- Draw weight and impatience
        fontLittle:PrintR(308, iY + 4,
          format("%03u%%          %03u%%\n\z
                  %03u%%         %05u\n\z
                  %04u          %03u%%",
            aDigger.H, floor(aDigger.IW / aDigger.STR * 100),
            floor(aDigger.JT / aDigger.PL * 100),
            aDigger.DUG, aDigger.GEM, ceil(aDigger.LDT / iGameTicks * 100)));
      -- Digger is dead
      else
        -- Draw grave icon
        texSpr:BlitSLT(319, 31, iY + 8);
        -- Draw dead labels
        fontLittle:PrintR(308, iY + 4,
          "---%          ---%\n\z
           ---%         -----\n\z
           ----          ---%");
      end
      -- Draw labels
      fontTiny:SetCRGB(0, 0.75, 1);
      fontTiny:SetLSpacing(2);
      fontTiny:PrintR(308, iY + 5,
        "HEALTH:             WEIGHT:        \n\z
         IMPATIENCE:         GROADS DUG:        \n\z
         GEMS FOUND:         EFFICIENCY:        ");
    end
    -- Draw on button
    texSpr:BlitSLT(816, 248, 216);
    -- Reset tiny font spacing
    fontTiny:SetLSpacing(1);
  -- Inventory button released?
  else texSpr:BlitSLT(815, 248, 216) end;
  -- Locations button pressed?
  if iInfoScreen == 2 then
    -- Draw frame and title
    DrawInfoFrameAndTitle("DIGGER LOCATIONS");
    -- Draw map grid of level
    for Y = 37, 188, 15 do for X = 141, 291, 15 do
      texSpr:BlitSLT(864, X, Y);
    end end
    -- For each digger
    for iDiggerId = 1, #aActivePlayer.D do
      -- Calculate Y position
      local iY<const> = iDiggerId * 31;
      -- Print id number of digger
      fontLarge:Print(16, iY + 8, iDiggerId);
      -- Draw colour key of digger
      texSpr:BlitSLT(858 + iDiggerId, 31, iY + 11);
      -- Draw X and Y letters
      fontTiny:SetCRGB(0, 0.75, 1);
      fontTiny:Print(64, iY + 8, "X:       Y:");
      -- Draw health bar background
      texSpr:BlitSLTRB(1023, 24, iY + 30, 124, iY + 32);
      -- Get digger and if it exists?
      local aDigger<const> = aActivePlayer.D[iDiggerId];
      if aDigger then
        -- Draw digger health bar
        DrawHealthBar(aDigger.H, 1, 24, iY + 30, 24, iY + 32);
        -- Draw digger item data
        fontLittle:Print(72, iY + 8,
          format("%04u  %04u\n\\%03u  \\%03u",
            aDigger.X, aDigger.Y, aDigger.AX, aDigger.AY));
        -- Draw digger portrait
        texSpr:BlitSLT(aDigger.S, 43, iY + 8);
        -- Draw position of digger
        texSpr:BlitSLT(858 + iDiggerId, 141 + (aDigger.AX * 1.25),
          38 + (aDigger.AY * 1.25));
      -- Digger is dead
      else
        -- Draw grave icon
        texSpr:BlitSLT(319, 43, iY + 8);
        -- Draw dashes for unavailable digger item data
        fontLittle:Print(72, iY + 8, "----  ----\n\\---  \\---");
      end
    end
    -- Draw on button
    texSpr:BlitSLT(818, 264, 216);
  -- Locations button released?
  else texSpr:BlitSLT(817, 264, 216) end;
  -- Status button pressed?
  if iInfoScreen == 3 then
    -- Set font colours
    fontLarge:SetCRGB(1, 1, 1);
    fontLittle:SetCRGB(1, 1, 1);
    -- Draw frame and title
    DrawInfoFrameAndTitle("ZONE STATUS");
    -- Score
    local ScoreAP, ScoreOP = 0, 0;
    -- Print level info
    fontLarge:Print(16, 40, sLevelName);
    fontLittle:Print(16, 56, sLevelType.." TERRAIN");
    fontLarge:PrintR(304, 40, format("%02u:%02u:%02u",
      iGameTicks // 216000 % 24,
      iGameTicks // 3600 % 60,
      iGameTicks // 60 % 60));
    fontLittle:PrintR(304, 56, "OPERATIONS TIME");
    fontLarge:PrintC(160, 72,
      aActivePlayer.RD.NAME.." VS "..aOpponentPlayer.RD.NAME);
    fontLittle:PrintC(160, 88,
      "YOU HAVE "..aActivePlayer.DC.." OF "..
      aActivePlayer.DT.." DIGGERS REMAINING");
    if aActivePlayer.DC > aOpponentPlayer.DC then
      fontLittle:PrintC(160, 96, "YOU HAVE MORE DIGGERS THEN YOUR OPPONENT");
      ScoreAP = ScoreAP + 1;
    elseif aActivePlayer.DC < aOpponentPlayer.DC then
      fontLittle:PrintC(160, 96, "YOUR OPPONENT HAS MORE DIGGERS");
      ScoreOP = ScoreOP + 1;
    else
      fontLittle:PrintC(160,  96, "YOU AND YOUR OPPONENT HAVE EQUAL DIGGERS");
    end
    fontLittle:PrintC(160, 112, "YOU MINED "..aActivePlayer.GEM..
      " GEMS AND "..aActivePlayer.DUG.." GROADS OF TERRAIN");
    if aActivePlayer.DUG > aOpponentPlayer.DUG then
      fontLittle:PrintC(160, 120, "YOU HAVE MINED THE MOST TERRAIN");
    elseif aActivePlayer.DUG < aOpponentPlayer.DUG then
      fontLittle:PrintC(160, 120, "YOUR OPPONENT HAS MINED THE MOST TERRAIN");
    else
      fontLittle:PrintC(160, 120,
        "YOU AND YOUR OPPONENT HAVE MINED EQUAL TERRAIN");
    end
    if aActivePlayer.GEM > aOpponentPlayer.GEM then
      fontLittle:PrintC(160, 128, "YOU HAVE FOUND THE MOST GEMS");
    elseif aActivePlayer.GEM < aOpponentPlayer.GEM then
      fontLittle:PrintC(160, 128, "YOUR OPPONENT HAS FOUND THE MOST GEMS");
    else
      fontLittle:PrintC(160, 128,
        "YOU AND YOUR OPPONENT HAVE FOUND EQUAL GEMS");
    end
    fontLittle:PrintC(160,  146, "YOU HAVE RAISED "..iPlayerMoney..
      " OF "..iWinLimit.." ZOGS ("..floor(iPlayerMoney/iWinLimit*100).."%)");
    if iPlayerMoney > iOpponentMoney then
      fontLittle:PrintC(160, 154, "YOU HAVE THE MOST ZOGS");
      ScoreAP = ScoreAP + 1;
    elseif iPlayerMoney < iOpponentMoney then
      fontLittle:PrintC(160, 154, "YOUR OPPONENT HAS MORE ZOGS");
      ScoreOP = ScoreOP + 1;
    else
      fontLittle:PrintC(160, 154, "YOU AND YOUR OPPONENT HAVE EQUAL ZOGS");
    end
    fontLittle:PrintC(160, 162,
      "RAISE "..(iWinLimit-iPlayerMoney).." MORE ZOGS TO WIN");
    fontLittle:PrintC(160, 178, "THE TRADE CENTRE HAS PREDICTED");
    if ScoreAP > ScoreOP then
      fontLarge:PrintC(160, 186, aActivePlayer.RD.NAME.." IS WINNING");
    elseif ScoreAP < ScoreOP then
      fontLarge:PrintC(160, 186, aOpponentPlayer.RD.NAME.." IS WINNING");
    else
      fontLarge:PrintC(160, 186, "NOBODY IS WINNING");
    end
    -- Draw on button
    texSpr:BlitSLT(803, 280, 216);
  -- Status button released?
  else texSpr:BlitSLT(802, 280, 216) end;
  -- Book button pressed?
  if iInfoScreen == 4 then texSpr:BlitSLT(820, 296, 216);
  -- Book button released?
  else texSpr:BlitSLT(819, 296, 216) end;
  -- If a tip was set? Draw it and unset tip
  if sTip then sTip = SetBottomRightTip(sTip) end;
  -- Done if no context menu selected
  if not aActiveMenu then return end;
  -- Button lookup id and reference to menu data
  local iIndex, aMData<const>, iMaxY<const>, iMaxX<const> =
    1, aActiveMenu[3], aActiveMenu[2], aActiveMenu[1];
  -- Object is busy?
  local bBusy<const> = aActiveObject.F & OFL.BUSY ~= 0;
  -- For each menu button row
  for iY = 1, iMaxY do
    -- Calculate bottom menu position
    local iBottom<const> = iMenuTop + iY * 16;
    -- Calculate top menu position;
    local iTop<const> = iBottom - 16;
    -- For each menu button column
    for iX = 1, iMaxX do
      -- Calculate right menu position
      local iRight<const> = iMenuLeft + iX * 16;
      -- Calculate left menu position
      local iLeft<const> = iRight - 16;
      -- Get sub-menu data
      local aSMData<const> = aMData[iIndex];
      -- Render the button
      BCBlit(texSpr, aSMData[1], iLeft, iTop, iRight, iBottom);
      -- Render a dim if object is busy and tile data says we should
      if bBusy and aSMData[2] & MFL.BUSY ~= 0 then
        BCBlit(texSpr, 801, iLeft, iTop, iRight, iBottom);
      -- If mouse over this button? Set the tip for this object
      elseif IsMouseInBounds(iLeft, iTop, iRight, iBottom) then
        sTip = aSMData[7] end;
      -- Increment button lookup id
      iIndex = iIndex + 1;
    end
  end
  -- Draw context menu shadow
  RenderShadow(iMenuLeft, iMenuTop, iMenuRight, iMenuBottom);
  -- Object has inventory selected?
  if aActiveObject.IS then
    -- Set white font
    fontTiny:SetCRGBAI(0xFFFFFFFF);
    -- Drop menu is open
    if aActiveMenu == aMenuData[MNU.DROP] then
      -- Draw active item
      texSpr:BlitSLT(aActiveObject.IS.S, iMenuLeft+23, iMenuTop+4);
      -- Draw info about item
      fontTiny:Print(iMenuLeft+17, iMenuTop+24, format("%03u%% %02u",
        aActiveObject.IS.H, aActiveObject.IS.W));
    -- Sell menu is open?
    elseif aActiveMenu == aMenuData[MNU.SELL] then
      -- Draw active item
      texSpr:BlitSLT(aActiveObject.IS.S, iMenuLeft+23, iMenuTop+4);
      -- Draw info about item
      fontTiny:Print(iMenuLeft+17, iMenuTop+24, format("%03uz %02u",
        aActiveObject.IS.OD.VALUE/2, aActiveObject.IS.W));
    end
  end
end
-- Check object is in water at specified pixel height ---------------------- --
local function CheckObjectInWater(aObject, iY)
  -- Get tile at the specified position from object and return if water
  local iId<const> = GetLevelDataFromObject(aObject, 8, iY);
  return iId and aTileData[1 + iId] & aTileFlags.W ~= 0;
end
-- Proc object underwater -------------------------------------------------- --
local function CheckObjectUnderwater(aObject)
  -- Ignore if object isn't underwater
  if aObject.F & OFL.AQUALUNG ~= 0 then return end;
  -- If object is not in water
  if not CheckObjectInWater(aObject, 2) then
    -- Remove flag if set
    if aObject.F & OFL.INWATER ~= 0 then
      aObject.F = aObject.F & ~OFL.INWATER end
    -- Done
    return;
  end
  -- Add in water flag if not set.
  if aObject.F & OFL.INWATER == 0 then
    aObject.F = aObject.F | OFL.INWATER end;
  -- If object is a digger and it isn't in danger? Run!
  if aObject.F & OFL.DIGGER ~= 0 and aObject.J ~= JOB.INDANGER then
    SetAction(aObject, ACT.RUN, JOB.INDANGER, DIR.KEEPMOVE) end;
  -- Only reduce health once per four game ticks
  if aObject.AT % aObject.LC == 0 then AdjustObjectHealth(aObject, -1) end;
end
-- Animate object ---------------------------------------------------------- --
local function AnimateObject(aObject)
  -- If sprite timer and not reached speed limit?
  if aObject.ST < aObject.ANT then
    aObject.ST = aObject.ST + 1 return end;
  -- Sprite id not reached the limit yet? Next sprite
  if aObject.S < aObject.S2 then
    aObject.S, aObject.SA = aObject.S+1, aObject.SA+1;
  -- Can sprite reset?
  elseif aObject.F & OFL.NOANIMLOOP == 0 then
    -- Restart sprite number
    aObject.S, aObject.SA = aObject.S1, aObject.S1A;
    -- Do we play the sound for the animation again?
    if aObject.F & OFL.SOUNDLOOP ~= 0 then
      -- Check if theres a sound to play and play sound if there is
      local iSound = aObject.AD.SOUND;
      if iSound then PlaySoundAtObject(aObject, iSound);
      -- No non-repeating sound...
      else
        -- Check if we have a sound with random pitch and if we do?
        iSound = aObject.AD.SOUNDRP;
        if iSound then
          PlaySoundAtObject(aObject, iSound, 0.975 + (random() % 0.05));
        end
      end
    end
  end
  -- Reset sprite timer
  aObject.ST = 0;
end
-- Make object face another ------------------------------------------------ --
local function GetTargetDirection(aObject, aTarget)
  if aTarget.X < aObject.X then return DIR.L;
  else return DIR.R end;
end
-- Check if object is colliding with another ------------------------------- --
local function CheckObjectCollision(aObject)
  -- Walk objects list
  for iIndex = 1, #aObjects do
    -- Get target object and if target object...
    local aTarget<const> = aObjects[iIndex];
    if aTarget ~= aObject and                -- ...is not me?
       aTarget.F & OFL.DIGGER ~= 0 and       -- *and* target is a digger?
       aTarget.F & OFL.WATERBASED == 0 and   -- *and* target isn't water based?
       aObject.H > 0 and                     -- *and* source is alive?
       aTarget.H > 0 and                     -- *and* target is alive?
       aTarget.A ~= ACT.PHASE and            -- *and* target not teleporting?
       aTarget.A ~= ACT.HIDE and             -- *and* target not hidden?
       aObject.A ~= ACT.EATEN and            -- *and* source object not eaten?
       maskSpr:IsCollideEx(                  -- *and* target collides source?
         477, aObject.X, aObject.Y, maskSpr,
         477, aTarget.X, aTarget.Y) then
      -- If object can consume the object?
      if aObject.F & OFL.CONSUME ~= 0 then
        -- Kill egg
        AdjustObjectHealth(aObject, -100);
        -- Eat digger and set it to busy
        SetAction(aTarget, ACT.EATEN, JOB.NONE, DIR.KEEP);
        -- This digger is selected by the client? Unset control menu
        if aActiveObject == aTarget then aActiveMenu = nil end;
        -- Don't need to test collision anymore since we killed the egg
        return;
      -- If target object is not eaten?
      elseif aTarget.A ~= ACT.EATEN then
        -- If object can phase the Digger? Phase target to another object
        if aObject.F & OFL.PHASEDIGGER ~= 0 then
          SetAction(aTarget, ACT.PHASE, JOB.PHASE, DIR.D);
        -- If object can heal the Digger? Increase health
        elseif aObject.F & OFL.HEALNEARBY ~= 0 then
          AdjustObjectHealth(aTarget, 1);
        -- If object can hurt the Digger?
        elseif aObject.F & OFL.HURTDIGGER ~= 0 then
          -- Object is stationary? Make me fight and face the digger
          if aObject.F & OFL.STATIONARY ~= 0 then
            SetAction(aObject, ACT.FIGHT, JOB.NONE,
              GetTargetDirection(aObject, aTarget));
          -- Change to objects direction if object moving parallel to Digger
          elseif aObject.F & OFL.PURSUEDIGGER ~= 0 then
            SetAction(aObject, ACT.KEEP, JOB.KEEP,
              GetTargetDirection(aObject, aTarget)) end;
          -- Target is not jumping?
          if aTarget.F & (OFL.JUMPRISE|OFL.JUMPFALL) == 0 then
            -- Digger isn't running? Make him run!
            if aTarget.A ~= ACT.RUN then
              SetAction(aTarget, ACT.RUN, JOB.INDANGER, DIR.LR);
            else
              SetAction(aTarget, ACT.KEEP, JOB.INDANGER, DIR.KEEP);
            end
          -- Target in danger
          else aTarget.J = JOB.INDANGER end;
          -- Reduce health
          AdjustObjectHealth(aTarget, -1);
        -- Object is dangerous and target is not jumping?
        elseif aObject.F & OFL.DANGEROUS ~= 0 then
          -- Target is not jumping?
          if aTarget.F & (OFL.JUMPRISE|OFL.JUMPFALL) == 0 then
            -- Digger isn't running?
            if aTarget.A ~= ACT.RUN then
              -- Digger not moving in any direction?
              if aTarget.D ~= DIR.NONE then
                SetAction(aTarget, ACT.RUN, JOB.INDANGER, DIR.KEEP);
              -- No direction? so run in opposite direction
              else SetAction(aTarget, ACT.RUN, JOB.INDANGER, DIR.LR) end;
            -- Object is not moving and direction set? Keep dir and set danger
            elseif aTarget.D ~= DIR.NONE then
              SetAction(aTarget, ACT.KEEP, JOB.INDANGER, DIR.KEEP);
            -- No direction? so run in opposite direction of object
            else SetAction(aTarget, ACT.RUN, JOB.INDANGER,
              GetTargetDirection(aTarget, aObject)) end;
          end
        -- Else if object...
        elseif aObject.F & OFL.DIGGER ~= 0 and -- Object is Digger?
           not aObject.FT and                  -- *and* has no fight target?
               aTarget.F & OFL.BUSY == 0 and   -- *and* target object not busy?
               aObject.F & OFL.BUSY == 0 and   -- *and* source object not busy?
               aObject.P ~= aTarget.P and      -- *and* not same owner?
               aTarget.A ~= ACT.JUMP and       -- *and* target not jumping
              (aObject.A == ACT.FIGHT or       -- *and* object is fighting?
               aObject.AT >= 30) then          -- *or* > 1/2 sec action timer
          -- Make us face the target
          SetAction(aObject, ACT.FIGHT, JOB.INDANGER,
            GetTargetDirection(aObject, aTarget));
          -- Fight and set objects fight target
          aObject.FT = aTarget;
          -- Target isn't fighting?
          if not aTarget.FT and aTarget.AT >= 30 then
            -- Make target fight the object
            SetAction(aTarget, ACT.FIGHT, JOB.INDANGER,
              GetTargetDirection(aTarget, aObject));
            -- Set targets fight target to this object
            aTarget.FT = aObject;
          end
        end
      -- Target is eaten? If object can phase the digger
      elseif aObject.F & OFL.PHASEDIGGER ~= 0 then
        -- Make eaten digger phase to some other object and troll it!
        SetAction(aTarget, ACT.PHASE, JOB.PHASE, DIR.DR);
      end
    end
  end
  -- Fight target was found?
  if aObject.FT then
    -- Still fighting?
    if aObject.A == ACT.FIGHT then
      -- Punch sprite?
      if aObject.ST == 1 then
        -- Deal damage equal to my strength
        AdjustObjectHealth(aObject.FT, aObject.STRP);
        if random() < 0.25 then PlaySoundAtObject(aObject, aSfxData.PUNCH) end;
      -- Kick sprite?
      elseif aObject.ST == 4 then
        -- Deal damage equal to my strength
        AdjustObjectHealth(aObject.FT, aObject.STRK);
        if random() < 0.25 then PlaySoundAtObject(aObject, aSfxData.KICK) end;
      end
      -- Make object run in opposite direction if object is...
      if aObject.H < 10 and            -- ...low on health?
         random() < aObject.IN then    -- *and* intelligent enough to run?
        SetAction(aObject, ACT.RUN, JOB.INDANGER, DIR.OPPOSITE) end;
    end
    -- Clear fight target
    aObject.FT = nil;
  -- No more fight targets so stop fighting if...
  elseif aObject.A == ACT.FIGHT and    -- ...object is fighting?
        (aObject.F & OFL.DIGGER ~= 0 or -- *and* object is a Digger?
         aObject.AT >= 360) then        -- *or* action time 360 frames?
    SetAction(aObject, ACT.STOP, JOB.INDANGER, DIR.KEEP);
  end
end
-- Object collides with an object which acts as terrain -------------------- --
local function IsCollidePlatform(aSrcObj, iX, iY)
  -- Ignore if not enough objects to compare
  if #aObjects <= 1 then return false end;
  -- If source object is a blocking platform too then we should use a
  -- different mask id.
  local iSrcMaskId;
  if aSrcObj.F & OFL.BLOCK ~= 0 then
    iSrcMaskId = 474 else iSrcMaskId = 478 end;
  -- For each object...
  for iObj = 1, #aObjects do
    -- Get target object to test with
    local aDstObj<const> = aObjects[iObj];
    -- If the target object set to block? If object isn't the target object?
    -- and the specified object collides with the target object? then yes!
    if aDstObj.F & OFL.BLOCK ~= 0 and aSrcObj ~= aDstObj and
      maskSpr:IsCollideEx(474, aDstObj.X, aDstObj.Y,
      maskSpr, iSrcMaskId, aSrcObj.X + iX, aSrcObj.Y + iY)
        then return true end;
  end
  -- No collision with any other object
  return false;
end
-- Object collides with background? ---------------------------------------- --
local function IsCollideX(aObject, nX)
  return maskZone:IsCollide(maskSpr, 478, aObject.X + nX, aObject.Y) or
    IsCollidePlatform(aObject, nX, 0) end;
local function IsCollideY(aObject, nY)
  return maskZone:IsCollide(maskSpr, 478, aObject.X, aObject.Y + nY) or
    IsCollidePlatform(aObject, 0, nY) end;
local function IsCollide(aObject, nX, nY)
  return maskZone:IsCollide(maskSpr, 478, aObject.X + nX, aObject.Y + nY) or
    IsCollidePlatform(aObject, nX, nY) end;
-- Object colliding with another object ------------------------------------ --
local function IsObjectCollide(aObject, aTarget)
  return maskSpr:IsCollideEx(aObject.S, aObject.X, aObject.Y,
    maskSpr, aTarget.S, aTarget.X, aTarget.Y) end;
-- Adjust object position -------------------------------------------------- --
local function AdjustPosX(aObject, nX)
  SetPositionX(aObject, aObject.X + nX);
  MoveOtherObjects(aObject, nX, 0);
end
local function AdjustPosY(aObject, nY)
  SetPositionY(aObject, aObject.Y + nY);
  MoveOtherObjects(aObject, 0, nY);
end
local function AdjustPos(aObject, nX, nY)
  SetPositionX(aObject, aObject.X + nX);
  SetPositionY(aObject, aObject.Y + nY);
  MoveOtherObjects(aObject, nX, nY);
end
-- Move object vertically -------------------------------------------------- --
local function MoveY(aObject, iY)
  -- Object can't move vertically? Stop the object
  if IsCollideY(aObject, iY) then
    SetAction(aObject, ACT.STOP, JOB.KEEP, DIR.KEEP);
  -- Object can move in requested direction
  else AdjustPosY(aObject, iY) end;
end
-- Move object horizontally ------------------------------------------------ --
local function MoveX(aObj, iX)
  -- Get object flags and if object is...
  local iFlags<const> = aObj.F;
  if iFlags & OFL.JUMPRISE ~= 0 or     -- Jumping (rising)?
     iFlags & OFL.JUMPFALL ~= 0 or     -- Jumping (falling)?
     iFlags & OFL.FLOATING ~= 0 then   -- Floating (water)?
    -- Object is jumping and move to next horizontal pixel? Move horizontally!
    if not IsCollideX(aObj, iX) then AdjustPosX(aObj, iX) end;
    -- Done
    return;
  end
  -- Try walking up to walking down a steep slope
  for iY = 2, -2, -1 do
    if not IsCollide(aObj, iX, iY) then AdjustPos(aObj, iX, iY) return end;
  end
  -- Ignore if falling
  if aObj.FD > 0 then return end;
  -- Get object job and set action for it because it was blocked
  local aBlockData<const> = aDigBlockData[aObj.J];
  SetAction(aObj, aBlockData[1], aBlockData[2], aBlockData[3]);
end
-- Check for colliding objects and move them ------------------------------- --
local function InitMoveOtherObjects()
  -- Actual function
  local function MoveOtherObjects(aObject, nX, nY)
    -- If i'm not a platform then return
    if aObject.F & OFL.BLOCK == 0 then return end;
    -- For each object
    for iTargetIndex = 1, #aObjects do
      -- Get target object and if it...
      local aTarget<const> = aObjects[iTargetIndex];
      if aTarget ~= aObject and           -- ...target object isn't me?
         aTarget.F & OFL.BLOCK == 0 and   -- *and* target object doesn't block?
         aTarget.A ~= ACT.PHASE and       -- *and* is not phasing?
         aTarget.A ~= ACT.DEATH and       -- *and* is not dying?
           maskSpr:IsCollideEx(aObject.S, -- *and* doesn't collide with object?
             aObject.X, aObject.Y, maskSpr, 478, aTarget.X, aTarget.Y) then
        -- If falling from above and? Target is not a device and is not falling
        -- too? Then the object can be considered crushed and die!
        if nY >= 1 and aObject.FD >= 1 and aTarget.F & OFL.DEVICE == 0 and
           aTarget.FS == 1 then AdjustObjectHealth(aTarget, -aObject.FD) end;
        -- Move that object too
        MoveX(aTarget, nX);
        MoveY(aTarget, nY);
        -- Prevent object from taking fall damage
        aTarget.FD, aTarget.FS = 0, 1;
      end
    end
  end
  -- Return actual function
  return MoveOtherObjects;
end
-- Create object function initialiser -------------------------------------- --
local function InitCreateObject()
  -- Direction table for AIFindTarget -------------------------------------- --
  local aFindTargetData<const> = {
    { DIR.UL,  DIR.U,  DIR.UR }, -- This is used for the AI.FIND(SLOW)
    { DIR.L, DIR.NONE,  DIR.R }, -- AI procedure. It's a lookup table for
    { DIR.DL,  DIR.D,  DIR.DR }  -- quick conversion from co-ordinates.
  };
  -- Picks a new target ---------------------------------------------------- --
  local function PickNewTarget(aObject)
    -- Holds potential targets
    local aTargets<const> = { };
    -- For each player...
    for iPlayer = 1, #aPlayers do
      -- Get player data and enumerate their diggers
      local aDiggers<const> = aPlayers[iPlayer].D;
      for iDiggerId = 1, #aDiggers do
        -- Get digger object and insert it in target list if a valid target
        local aDigger<const> = aDiggers[iDiggerId];
        if aDigger and aDigger.F & OFL.PHASETARGET ~= 0 then
          aTargets[1 + #aTargets] = aDigger;
        end
      end
    end
    -- Return if no targets found?
    if #aTargets == 0 then return end;
    -- Pick a random target and set it
    local aTarget<const> = aTargets[random(#aTargets)];
    aObject.T = aTarget;
    -- Initially move in direction of target
    SetAction(aObject, ACT.KEEP, JOB.KEEP,
      GetTargetDirection(aObject, aTarget));
    -- Return target
    return aTarget;
  end
  -- Get target change time
  local iTargetChangeTime<const> = aTimerData.TARGETTIME;
  -- Process find target logic --------------------------------------------- --
  local function AIFindTarget(aObject)
    -- Return if busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Get target and if set?
    local aTarget = aObject.T;
    if aTarget then
      -- If target time elapsed?
      if aObject.AT >= iTargetChangeTime or
         aTarget.F & OFL.PHASETARGET == 0 then
        -- Set a new target and if we got one?
        local aNewTarget<const> = PickNewTarget(aObject);
        if aNewTarget then
          -- Get my unique ID
          local iUId<const> = aObject.U;
          -- Remove me from old targets list
          aTarget.TL[iUId] = nil;
          -- Add me to the new targets list
          aNewTarget.TL[iUId] = aObject;
          -- Set new target
          aTarget = aNewTarget;
        end
      end
    -- No target so...
    else
      -- Initialise a new target and if we did? Add me to targets pursuer list
      aTarget = PickNewTarget(aObject);
      if aTarget then aTarget.TL[aObject.U] = aObject else return end;
    end
    -- Variables for adjusted coordinates
    local iXA, iYA = 0, 0;
    -- This X is left of target X? Move right one pixel
    if aObject.X < aTarget.X then iXA = iXA + 1;
    -- X is right of target X? Move left one pixel
    elseif aObject.X > aTarget.X then iXA = iXA - 1 end;
    -- If we can move to requested horizontal position?
    -- Move to requested horizontal position
    if not IsCollideX(aObject, iXA) then AdjustPosX(aObject, iXA) end;
    -- This Y is left of target Y? Move down one pixel
    if aObject.Y < aTarget.Y then iYA = iYA + 1;
    -- Y is right of target Y? Move up one pixel
    elseif aObject.Y > aTarget.Y then iYA = iYA - 1 end;
    -- If we can move to requested vertical position?
    -- Move to requested vertical position
    if not IsCollideY(aObject, iYA) then AdjustPosY(aObject, iYA) end;
    -- Increment values for table lookup
    iXA, iYA = iXA + 2, iYA + 2;
    -- Direction changed? Change direction!
    local iDirection<const> = aFindTargetData[iYA][iXA]
    if aObject.D ~= iDirection then
      SetAction(aObject, ACT.KEEP, JOB.KEEP, iDirection);
    end
  end
  -- Process find target logic (slow --------------------------------------- --
  local function AIFindTargetSlow(aObject)
    -- Move closer to selected target every even action frame
    if aObject.AT % 2 == 0 then AIFindTarget(aObject) end;
  end
  -- Returns if object has sellable items----------------------------------- --
  local function ObjectHasValuables(aObject)
    -- Get object inventory and if we have items?
    local aInventory<const> = aObject.I;
    if #aInventory > 0 then
      -- Enumerate them...
      for iInvIndex = 1, #aInventory do
        -- Get object in inventory. If is sellable regardless of owner?
        local aInventoryObject<const> = aInventory[iInvIndex];
        if aInventoryObject.F & OFL.SELLABLE ~= 0 and
          CanSellGem(aInventoryObject.ID) then return true end;
      end
    end
    -- Nothing to sell
    return false;
  end
  -- If object can jump left then jump left -------------------------------- --
  local function ObjectTryJumpLeft(aObject)
    if IsCollide(aObject, -1, -2) and not IsCollide(aObject, -1, -16) then
      SetAction(aObject, ACT.JUMP, JOB.KEEP, DIR.KEEP) return true end;
    return false;
  end
  -- If object can jump right then jump right------------------------------- --
  local function ObjectTryJumpRight(aObject)
    if IsCollide(aObject, 1, -2) and not IsCollide(aObject, 1, -16) then
      SetAction(aObject, ACT.JUMP, JOB.KEEP, DIR.KEEP) return true end;
    return false;
  end
  -- Lookup table for jumping ---------------------------------------------- --
  local aJumpCheckFunctions<const> = {
    [DIR.UL] = ObjectTryJumpLeft, [DIR.UR] = ObjectTryJumpRight,
    [DIR.L]  = ObjectTryJumpLeft, [DIR.R]  = ObjectTryJumpRight,
    [DIR.DL] = ObjectTryJumpLeft, [DIR.DR] = ObjectTryJumpRight
  };
  -- Try to jump the object and return if we did --------------------------- --
  local function ObjectJumped(aObject)
    -- Return if digger is digging or digger has fall damage
    if aObject.J == JOB.DIG or aObject.FD > 0 then return false end;
    -- 50% chance to check to jump
    if random() < 0.5 then
      -- Check if direction is important and call the associated function
      local fcbJumpCheck<const> = aJumpCheckFunctions[aObject.D];
      if fcbJumpCheck then return fcbJumpCheck(aObject) end;
    end
    -- Object didn't jump
    return false;
  end
  -- Digger AI choices (Chances to change action per tick) ----------------- --
  local aAIData<const> = {
    -- Ai is stopped?
    [ACT.STOP] = {
      -- No job set?
      [JOB.NONE] = { [DIR.UL] = 0.02,  [DIR.U]    = 0.1,  [DIR.UR] = 0.02,
                     [DIR.L]  = 0.02,  [DIR.NONE] = 0.1,  [DIR.R]  = 0.02,
                     [DIR.DL] = 0.02,  [DIR.D]    = 0.02, [DIR.DR] = 0.02 },
      -- Job is to dig?
      [JOB.DIG] = { [DIR.UL] = 0.02,  [DIR.U]    = 0.02, [DIR.UR] = 0.02,
                    [DIR.L]  = 0.02,  [DIR.NONE] = 0.02, [DIR.R]  = 0.02,
                    [DIR.DL] = 0.02,  [DIR.D]    = 0.02, [DIR.DR] = 0.02 },
    -- Ai is walking?
    }, [ACT.WALK] = {
      -- Job is bouncing around?
      [JOB.BOUNCE] = { [DIR.UL] = 0.001, [DIR.UR] = 0.001,  [DIR.L] = 0.001,
                       [DIR.R]  = 0.001, [DIR.DL] = 0.02,   [DIR.D] = 0.002,
                       [DIR.DR] = 0.02 },
      -- Job is digging?
      [JOB.DIG] = { [DIR.UL] = 0.002, [DIR.UR] = 0.002,  [DIR.L]  = 0.001,
                    [DIR.R]  = 0.001, [DIR.DL] = 0.02,   [DIR.DR] = 0.05 },
      -- Job is digging down? (Very narrow time frame)
      [JOB.DIGDOWN] = { [DIR.D] = 0.95 },
      -- Job is searching for treasure?
      [JOB.SEARCH] = { [DIR.L] = 0.002, [DIR.R] = 0.002 },
    -- Ai is running?
    }, [ACT.RUN] = {
      -- Job is bouncing around?
      [JOB.BOUNCE] = { [DIR.L] = 0.01, [DIR.R] = 0.01 },
      -- Job is in danger?
      [JOB.INDANGER] = { [DIR.L] = 0.002, [DIR.R] = 0.002 },
    },
  };
  -- Fall check directions ------------------------------------------------- --
  local aAIFallCoordsCheck<const> = {
    [DIR.UL] = -1, [DIR.L]  = -1, [DIR.DL] = -1,
    [DIR.UR] =  1, [DIR.R]  =  1, [DIR.DR] =  1
  }
  -- Jump left check logic ------------------------------------------------- --
  local aAIWalkJumpGapLeftData<const>  = { -24, -12 };
  local aAIWalkJumpGapRightData<const> = {  24,  12 };
  local aAIRunJumpGapLeftData<const>   = { -48, -24 };
  local aAIRunJumpGapRightData<const>  = {  48,  24 };
  -- Jump logic ------------------------------------------------------------ --
  local aAIJumpGapLogic<const> = {
    [ACT.WALK] = {
      [DIR.UL] = aAIWalkJumpGapLeftData,  [DIR.L]  = aAIWalkJumpGapLeftData,
      [DIR.UR] = aAIWalkJumpGapRightData, [DIR.DL] = aAIWalkJumpGapLeftData,
      [DIR.R]  = aAIWalkJumpGapRightData, [DIR.DR] = aAIWalkJumpGapRightData
    },
    [ACT.RUN] = {
      [DIR.UL] = aAIRunJumpGapLeftData,  [DIR.L]  = aAIRunJumpGapLeftData,
      [DIR.UR] = aAIRunJumpGapRightData, [DIR.DL] = aAIRunJumpGapLeftData,
      [DIR.R]  = aAIRunJumpGapRightData, [DIR.DR] = aAIRunJumpGapRightData
    }
  };
  -- AI Digger phasing out ------------------------------------------------- --
  local function PhaseHome(aObject)
    -- Reset last dig time
    aObject.LDT = iGameTicks;
    -- Teleport home
    return SetAction(aObject, ACT.PHASE, JOB.PHASE, DIR.U);
  end
  -- AI jumping gap logic -------------------------------------------------- --
  local function CheckForJump(aObject)
    -- Get jump data depending on action and if we got any?
    local aDirData<const> = aAIJumpGapLogic[aObject.A];
    if aDirData then
      -- Get X check coords depending on direction and if we have them?
      local aXCheck<const> = aDirData[aObject.D];
      if aXCheck then
        -- This is the furthest point of the jump and if object...
        local iXFar<const> = aXCheck[1];
        if not IsCollide(aObject, iXFar, -2) and       -- Can go furthest
           not IsCollide(aObject, aXCheck[2], -14) and -- Highest point?
               IsCollide(aObject, iXFar, 13) then      -- Stable ground?
          -- Jump the gap!
          SetAction(aObject, ACT.JUMP, JOB.KEEP, DIR.KEEP);
          -- Success!
          return true;
        end
      end
    end
  end
  -- AI digger logic ------------------------------------------------------- --
  local function AIDiggerLogic(aObject)
    -- Return if busy or not falling
    if aObject.F & OFL.BUSY ~= 0 or aObject.F & OFL.FALL == 0 then return end;
    -- Create object virtual health and double it if not delicate
    local iHealth;
    if aObject.F & OFL.DELICATE == 0 then iHealth = aObject.H * 2;
                                     else iHealth = aObject.H end;
    -- Object is not falling yet?
    if aObject.FD == 0 then
      -- Grab fall check coord adjust
      local iAdjX<const> = aAIFallCoordsCheck[aObject.D];
      if iAdjX then
        -- For water checking as we have to adjust to X centre for it
        local iAdjXW<const>, iAdjY = iAdjX + 8, 1;
        -- Repeat virtual falling...
        repeat
          -- If Digger is not in water?
          if aObject.F & OFL.INWATER == 0 then
            -- Go in opposite direction if we would fall into water
            local iId<const> =
              GetLevelDataFromObject(aObject, iAdjXW, iAdjY);
            if iId then
              -- Get tile data and if its water or firm ground?
              local iTD<const> = aTileData[1 + iId];
              if iTD & aTileFlags.W ~= 0 then break end;
            end
          end
          -- If we collide with the background
          if IsCollide(aObject, iAdjX, iAdjY) then
            -- Check if intelligent enough to jump the gap
            if iAdjY >= 16 and
               random() >= aObject.IN and
               CheckForJump(aObject) then return end;
            -- Falling is safe
            goto FallingIsSafe;
          end
          -- Go down 5 pixels (which removes 1 health)
          iAdjY, iHealth = iAdjY + 5, iHealth - 1;
        -- ...until Digger virtually dies while virtually falling
        until iHealth <= 0;
        -- Do we have intelligence to check the gap?
        if random() >= aObject.IN and CheckForJump(aObject) then return end;
        -- Get last anti-wriggle timeout value and if we exceeded it
        local iAntiWriggleTime<const> = aObject.AW;
        if iGameTicks >= iAntiWriggleTime then
          -- Reset anti-wriggle timeframe to another 5 seconds
          aObject.AW = iGameTicks + 300;
          -- Get wriggle count because it will be reset now
          local iAntiWriggleRemain<const> = aObject.AWR;
          aObject.AWR = 0
          -- too many times in this timeframe?
          if iAntiWriggleRemain >= 10 then return PhaseHome(aObject) end;
        -- Still in the timeframe so keep increasing anti- wriggle counter
        else aObject.AWR = aObject.AWR + 1 end;
        -- This fall would kill cause the digger harm so evade the fall.
        do return SetAction(aObject, ACT.KEEP, JOB.KEEP, DIR.OPPOSITE) end;
        -- Fall is 'safe'
        ::FallingIsSafe::
      end
      -- Teleport home to rest and sell items if these conditions are met...
      if ObjectIsAtHome(aObject) and -- Object is at their home point? *and*
        (aObject.IW > 0 or           -- (Digger is carrying something? *or*
         aObject.H < 75) and         --  Health is under 75%) *and*
         aObject.A == ACT.STOP and   -- Digger has stopped?
         aObject.D ~= DIR.R then     -- Digger hasn't teleported yet?
        return SetAction(aObject, ACT.PHASE, JOB.PHASE, DIR.R);
      end
      -- Stop the Digger if needed so it can heal a bit if...
      if random() < 0.001 and           -- Intelligent enough? (0.1%)
         aObject.H <= 25 and            -- Below quarter health?
         aObject.A ~= ACT.STOP and      -- Not stopped?
         aObject.J ~= JOB.INDANGER then -- Not in danger?
        return SetAction(aObject, ACT.STOP, JOB.NONE, DIR.NONE);
      end
      -- If object...
      if (((random() <= 0.01 and               -- *and* (1% chance?
            aObject.H < 50) or                 -- *and* Health under 50%)
           (random() <= 0.001 and              -- *or* (0.1% chance?
            aObject.H >= 50)) and              -- *and* Health over 50%)
          aObject.J == JOB.INDANGER and        -- Object is in danger?
          aObject.A ~= ACT.STOP) or            -- *and* moving?
         (random() <= 0.001 and                -- *or* (0.1% chance?)
          aObject.IW >= aObject.MI and         -- *and* Digger has full inv?
          ObjectHasValuables(aObject)) or      -- *and* has sellable items?)
         (random() <= 0.01 and                 -- *or* (1% chance?)
          iGameTicks - aObject.LDT >= 7200 and -- *and* Not dug for 2 mins?
          aObject.A == ACT.STOP) then          -- *and* not moving?)
        PhaseHome(aObject);                    -- Phase home
      end
      -- Wait longer if health is needed
      if aObject.H < 50 and        -- Below half health?
         aObject.A == ACT.STOP and -- Stopped?
         random() > 0.001 then     -- Very big chance? (0.1%)
        return;                    -- Do nothing else
      end
      -- Digger is walking?
      if aObject.A == ACT.WALK then
        -- Every 1/2 sec and digger isn't searching? Pick up any treasure!
        if iGameTicks % 30 == 0 and aObject.J ~= JOB.SEARCH then
          PickupObjects(aObject, true);
        -- Check for jump and return if we did
        elseif ObjectJumped(aObject) then return end;
      -- Return if running and we can jump
      elseif aObject.A == ACT.RUN and ObjectJumped(aObject) then return end;
      -- A 0.01% chance occurred each frame?
      if random() < 0.0001 then
        -- Get digger inventory and if we have inventory?
        local aInventory<const> = aObject.I;
        if #aInventory > 0 then
          -- Walk Digger inventory
          for iInvIndex = 1, #aInventory do
            -- Get inventory object and if it is not treasure?
            local aInvObj<const> = aInventory[iInvIndex];
            if aInvObj.F & OFL.TREASURE == 0 then
              -- Drop it and do not drop anything else
              DropObject(aObject, aInvObj);
              break;
            end
          end
        end
      end
      -- Get AI data for action and if we have it
      local aAIDataAction<const> = aAIData[aObject.A];
      if aAIDataAction then
        -- Get AI data for job and if we have it
        local aAIDataJob<const> = aAIDataAction[aObject.J];
        if aAIDataJob then
          -- Get chance to change job and if that chance is triggered?
          local nAIDataDirection<const> = aAIDataJob[aObject.D];
          if nAIDataDirection and random() <= nAIDataDirection then
            -- Set a random job
            return SetRandomJob(aObject);
          end
        end
      end
    -- Teleport home to safely regain health if falling would kill digger
    elseif aObject.FD >= iHealth and   -- Falling damage would kill? *and*
           random() >= aObject.IN then -- Intelligent enough to teleport?
      PhaseHome(aObject);
    end
  end
  -- AI random direction logic initialisation data ------------------------- --
  local aAIRandomLogicInitData<const> = { DIR.U, DIR.D, DIR.L, DIR.R };
  -- AI random direction logic movement data ------------------------------- --
  local aAIRandomLogicMoveData<const> = {
    -- --------------------------------------------------------------------- --
    -- Try to move in these directions first...
    --             TeX TeY     TeX TeY
    --             \_/ \_/     \_/ \_/
    -- --------------------------------------------------------------------- --
    -- When blocked then an alternative list of directions is provided...
    --   TeX TeY Direction  TeX TeY Direction  TeX TeY Direction
    --   \_/ \_/   \_/      \_/ \_/   \_/      \_/ \_/   \_/
    -- --------------------------------------------------------------------- --
    [DIR.U] = { { {  0, -2 }, {  0, -1 } }, -- Going up?
      { { -1, -1, DIR.U }, {  1, -1, DIR.U }, { -1,  0, DIR.L },
        {  1,  0, DIR.R }, {  0,  1, DIR.D } } },
    -- --------------------------------------------------------------------- --
    [DIR.D] = { { {  0,  2 }, {  0,  1 } }, -- Going down?
      { {  0,  1, DIR.D }, { -1,  1, DIR.D }, {  1,  1, DIR.D },
        { -1,  0, DIR.L }, {  1,  0, DIR.R }, {  0, -1, DIR.U } } },
    -- --------------------------------------------------------------------- --
    [DIR.L] = { { { -2,  0 }, { -1,  0 } }, -- Going left?
      { { -1,  0, DIR.L }, { -1, -1, DIR.L }, { -1,  1, DIR.L },
        {  0, -1, DIR.U }, {  0,  1, DIR.D }, {  1,  0, DIR.R } } },
    -- --------------------------------------------------------------------- --
    [DIR.R] = { { {  2,  0 }, {  1,  0 } }, -- Going right?
      { {  1,  0, DIR.R }, {  1, -1, DIR.R }, {  1,  1, DIR.R },
        {  0, -1, DIR.U }, {  0,  1, DIR.D }, { -1,  0, DIR.L } } }
    -- --------------------------------------------------------------------- --
  };
  -- AI random direction logic --------------------------------------------- --
  local function AIRandomLogic(aObject)
    -- Return if busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Set new direction if object has no direction
    local iDirection = aObject.D;
    if iDirection == DIR.NONE then
      return SetAction(aObject, ACT.KEEP, JOB.KEEP,
        aAIRandomLogicInitData[random(#aAIRandomLogicInitData)]) end;
    -- Get direction
    local aDData<const> = aAIRandomLogicMoveData[iDirection];
    -- Try every possible combination but the last
    local aPDData<const> = aDData[1];
    for iI = 1, #aPDData do
      local aPDItem<const> = aPDData[iI];
      if not IsCollide(aObject, aPDItem[1], aPDItem[2]) then
        return AdjustPos(aObject, aPDItem[1], aPDItem[2]) end;
    end
    -- Blocked so we need to find a new direction to move in
    local aDirections = { };
    -- Try every possible combination but the last
    local aPossibleDirections<const> = aDData[2];
    for iI = 1, #aPossibleDirections - 1 do
      local aPDData<const> = aPossibleDirections[iI];
      if not IsCollide(aObject, aPDData[1], aPDData[2]) then
        aDirections[1 + #aDirections] = aPDData[3] end;
    end
    -- If we have a possible direction then go in that direction
    if #aDirections ~= 0 then iDirection = aDirections[random(#aDirections)];
    -- No direction found
    else
      -- Try going back in the previous direction
      local aPDData<const> = aPossibleDirections[#aPossibleDirections];
      if not IsCollide(aObject, aPDData[1], aPDData[2]) then
        iDirection = aPDData[3];
      -- Can't even go back in the previous direction so this position is FAIL
      else iDirection = DIR.NONE end
    end
    -- Pick a new direction from eligible directions
    SetAction(aObject, ACT.KEEP, JOB.KEEP, iDirection);
  end
  -- Bigfoot AI data ------------------------------------------------------- --
  local aAIBigFootData<const> = {
    { ACT.WALK,  JOB.BOUNCE, DIR.L    }, -- Chance to walk left
    { ACT.WALK,  JOB.BOUNCE, DIR.R    }, -- Chance to walk right
    { ACT.PHASE, JOB.PHASE,  DIR.D    }, -- Chance to phase randomly
    { ACT.STOP,  JOB.NONE,   DIR.NONE }, -- Chance to stop
  };
  -- Bigfoot AI ------------------------------------------------------------ --
  local function AIBigfoot(aObject)
    -- Return if busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Return if object is busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Jump if we can
    if aObject.A == ACT.WALK and ObjectJumped(aObject) then return end;
    -- Every 1/2 sec. Try to pick up anything
    if iGameTicks % 30 == 0 then return PickupObjects(aObject, false) end;
    -- If ADHD hasn't set in yet? Just return
    if random() > 0.001 then return end;
    -- Get and set a random action
    local aActionData<const> = aAIBigFootData[random(#aAIBigFootData)];
    SetAction(aObject, aActionData[1], aActionData[2], aActionData[3]);
  end
  -- Basic AI roaming ------------------------------------------------------ --
  local function AIRoam(aObject)
    -- Return if busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Direction to go in
    local iAdjX;
    if aObject.D == DIR.L then iAdjX = -1 else iAdjX = 1 end;
    -- Move if object can move in its current direction, or the target
    -- time exceeds to create a sudden direction change.
    if not IsCollideX(aObject, iAdjX) and
      random() > aTimerData.ROAMDIRCHANGE then
        AdjustPosX(aObject, iAdjX);
    -- Blocked so go in opposite direction
    else SetAction(aObject, ACT.KEEP, JOB.KEEP, DIR.OPPOSITE) end;
  end
  -- Basic AI roaming (slow) ----------------------------------------------- --
  local function AIRoamSlow(aObject)
    -- Return if busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Move around every 4th frame
    if aObject.AT % 4 == 0 then AIRoam(aObject) end;
  end
  -- Basic AI roaming (normal) --------------------------------------------- --
  local function AIRoamNormal(aObject)
    -- Return if busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Move around every odd frame
    if aObject.AT % 2 == 0 then AIRoam(aObject) end;
  end
  -- Tunneller ------------------------------------------------------------- --
  local function AITunneller(aObject)
    -- Ignore if the tunneller is not stopped and a rare random value occurs
    if random() >= 0.001 or aObject.F & OFL.BUSY ~= 0 then return end;
    -- Pick a direction to tunnel in or stop
    if random() < 0.5 then
      SetAction(aObject, ACT.WALK, JOB.DIG, DIR.LR);
    else
      SetAction(aObject, ACT.STOP, JOB.NONE, DIR.NONE);
    end
  end
  -- Corkscrew actions ----------------------------------------------------- --
  local aAICorkscrewActions<const> = {
    [ACT.STOP] = { [JOB.NONE] = {
      { 0.001, ACT.CREEP, JOB.NONE,    DIR.LR   },
      { 0.001, ACT.CREEP, JOB.DIGDOWN, DIR.TCTR } }
    }, [ACT.DIG] = { [JOB.DIGDOWN] = {
      { 0.01,  ACT.STOP,  JOB.NONE,    DIR.NONE },
      { 0.001, ACT.CREEP, JOB.NONE,    DIR.LR   } }
    }, [ACT.CREEP] = { [JOB.NONE] = {
      { 0.001, ACT.STOP,  JOB.NONE,    DIR.LR   },
      { 0.001, ACT.CREEP, JOB.DIGDOWN, DIR.TCTR } }
    } };
  -- Corkscrew ------------------------------------------------------------- --
  local function AICorkscrew(aObject)
    -- Ignore if object is busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Get data for current action
    local aAIActionData<const> = aAICorkscrewActions[aObject.A];
    if not aAIActionData then return end;
    -- Get data for current job
    local aAIJobData<const> = aAIActionData[aObject.J];
    if not aAIJobData then return end;
    -- For each item
    for iIndex = 1, #aAIJobData do
      -- Get item and set new action if the specified chance occurs
      local aAction<const> = aAIJobData[iIndex];
      if random() < aAction[1] then
        return SetAction(aObject,
          aAction[2], aAction[3], aAction[4], aAction[5]);
      end
    end
  end
  -- Exploder -------------------------------------------------------------- --
  local function AIExploder(aObject)
    -- Ignore if a rare random value occurs or object is busy
    if random() > 0.001 or aObject.F & OFL.BUSY ~= 0 then return end;
    -- Get objects parent and return if there is none (should be impossible)
    local aParent<const> = aObject.P;
    if not aParent then return end;
    -- Get object sprite and position
    local iX<const>, iY<const> = aObject.X, aObject.Y;
    -- For each object
    for iIndex = 1, #aObjects do
      -- Get object and if both objects are within reaching distance?
      local aTarget<const> = aObjects[iIndex];
      if abs(aTarget.X - iX) < 16 and abs(aTarget.Y - iY) < 16 then
        -- Target must have a parent and is different to objects
        local aTParent<const> = aTarget.P;
        if aTParent and aTParent ~= aParent then
          -- Set off the object exploding
          SetAction(aObject, ACT.DYING, JOB.NONE, DIR.NONE);
        end
      end
    end
  end
  -- AI for train for rails ------------------------------------------------ --
  local function AITrain(aObject)
    -- Ignore if a rare random value occurs or object is busy
    if random() > 0.001 or aObject.F & OFL.BUSY ~= 0 then return end;
    -- Go left, right or stop
    if random() > 0.5 then
      SetAction(aObject, ACT.WALK, JOB.SEARCH, DIR.LR);
    else
      SetAction(aObject, ACT.STOP, JOB.SEARCH, DIR.NONE);
    end
  end
  -- AI for lift ----------------------------------------------------------- --
  local function AILift(aObject)
    -- Ignore if a rare random value occurs or object is busy
    if random() > 0.001 or aObject.F & OFL.BUSY ~= 0 then return end;
    -- Go up or down or stop
    if random() > 0.5 then
      SetAction(aObject, ACT.CREEP, JOB.NONE, DIR.UD);
    else
      SetAction(aObject, ACT.STOP, JOB.NONE, DIR.NONE);
    end
  end
  -- AI for inflatable boat ------------------------------------------------ --
  local function AIBoat(aObject)
    -- Ignore if a rare random value occurs or object is busy
    if random() > 0.001 or aObject.F & OFL.BUSY ~= 0 then return end;
    -- Go up or down or stop
    if random() > 0.5 then
      SetAction(aObject, ACT.CREEP, JOB.NONE, DIR.LR);
    else
      SetAction(aObject, ACT.STOP, JOB.NONE, DIR.NONE);
    end
  end
  -- AI for anything deployable -------------------------------------------- --
  local function AIDeploy(aObject)
    -- Ignore if a rare random value occurs or object is busy
    if random() > 0.0001 or aObject.F & OFL.BUSY ~= 0 then return end;
    -- Deploy the object
    SetAction(aObject, ACT.DEPLOY, JOB.NONE, DIR.NONE);
  end
  -- AI for flood gate ----------------------------------------------------- --
  local function AIGate(aObject)
    -- Ignore if a rare random value occurs or object is busy
    if random() > 0.00001 or aObject.F & OFL.BUSY ~= 0 then return end;
    -- Open or close the gate
    if random() < 0.5 then
      SetAction(aObject, ACT.OPEN, JOB.NONE, DIR.NONE);
    else
      SetAction(aObject, ACT.CLOSE, JOB.NONE, DIR.NONE);
    end
  end
  -- AI id to function list ------------------------------------------------ --
  local aAIFuncs<const> = {
    [AI.NONE]        = false,            [AI.DIGGER]   = AIDiggerLogic,
    [AI.RANDOM]      = AIRandomLogic,    [AI.FIND]     = AIFindTarget,
    [AI.FINDSLOW]    = AIFindTargetSlow, [AI.CRITTER]  = AIRoamNormal,
    [AI.CRITTERSLOW] = AIRoamSlow,       [AI.BIGFOOT]  = AIBigfoot,
    [AI.TUNNELER]    = AITunneller,      [AI.EXPLODER] = AIExploder,
    [AI.CORKSCREW]   = AICorkscrew,      [AI.TRAIN]    = AITrain,
    [AI.LIFT]        = AILift,           [AI.BOAT]     = AIBoat,
    [AI.DEPLOY]      = AIDeploy,         [AI.GATE]     = AIGate
  }; ----------------------------------------------------------------------- --
  local function CreateObject(iObjId, iX, iY, aParent)
    -- Check parameters
    if not UtilIsInteger(iObjId) then
      error("Object id integer invalid! "..tostring(iObjId)) end;
    if iObjId < 0 then error("Object id "..iObjId.." must be positive!") end;
    if not UtilIsInteger(iX) then
      error("X coord integer invalid! "..tostring(iX)) end;
    if iX < 0 then error("X coord "..iX.." must be positive!") end;
    if iX >= iLLPixW then
      error("Y coord "..iX.." limit is "..iLLPixW.."!") end;
    if not UtilIsInteger(iY) then
      error("Y coord integer invalid! "..tostring(iY)) end;
    if iY < 0 then error("X coord "..iY.." must be positive!") end;
    if iY >= iLLPixH then
      error("Y coord "..iY.." limit is "..iLLPixH.."!") end;
    -- If too many objects? Put warning in console and return!
    if #aObjects >= 100000 then
      return CoreWrite("Warning! Too many objects creating "..iObjId..
        " at X="..iX.." and Y="..iY.." with parent "..tostring(aParent).."!",
        9);
    end
    -- Get and test object information
    local aObjData<const> = aObjectData[iObjId];
    if not UtilIsTable(aObjData) then error("Object data for #"..
      iObjId.." not in objects lookup!") end;
    -- Get object name
    local sName<const> = aObjData.NAME
    if not UtilIsString(sName) then error("Object name for #"..
      iObjId.." not in object data table!") end;
    -- Get and test AI type
    local iAI<const> = aObjData.AITYPE;
    if not UtilIsInteger(iAI) then error("Invalid AI type #"..
      tostring(iAI).." for "..sName.."["..iObjId.."]!") end
    -- Increment unique object id number
    iUniqueId = iUniqueId + 1;
    -- Build new object array
    local aObject<const> = {
      A    = ACT.NONE,                   -- Object action (ACT.*
      AA   = false,                      -- Attachment action data
      AD   = { },                        -- Reference to action data
      AI   = iAI,                        -- Object AI procedure
      ANT  = aObjData.ANIMTIMER,         -- Animation timer
      AT   = 0,                          -- Action timer
      AX   = 0,                          -- Tile X position clamped to 16
      AY   = 0,                          -- Tile Y position clamped to 16
      CS   = not not aObjData[ACT.STOP], -- Object can stop?
      D    = aObjData.DIRECTION,         -- Direction object is going in
      DA   = false,                      -- Attachment direction data
      DD   = { },                        -- Reference to direction data
      DID  = aObjData.DIGDELAY,          -- Digging delay
      DUG  = 0,                          -- Successful dig count
      F    = aObjData.FLAGS or 0,        -- Object flags (OFL.*)
      FD   = 0,                          -- Amount the object has fallen by
      FDD  = DIR.NONE,                   -- Last failed dig direction
      FS   = 1,                          -- Object falling speed
      GEM  = 0,                          -- Gems found
      H    = 100,                        -- Object health
      I    = { },                        -- Inventory for this object
      ID   = iObjId,                     -- Object id (TYP.*)
      IN   = aObjData.INTELLIGENCE,      -- Intelligence
      IS   = nil,                        -- Selected inventory item
      IW   = 0,                          -- Weight of inventory
      J    = JOB.NONE,                   -- Object job (JOB.*)
      JD   = { },                        -- Reference to job data
      JT   = 0,                          -- Job timer
      LC   = aObjData.LUNGS,             -- Lung capacity
      LDT  = iGameTicks,                 -- Last successful dig time
      OD   = aObjData,                   -- Object data table
      OFX  = 0, OFY  = 0,                -- Drawing offset
      OFXA = 0, OFYA = 0,                -- Attachment drawing offset
      P    = aParent,                    -- Parent of object
      PW   = 0,                          -- Patience warning
      PL   = 0,                          -- Patience limit
      S    = 0,                          -- Current sprite frame #
      S1   = 0,                          -- Start sprite frame #
      S1A  = 0,                          -- Start attachment sprite frame #
      S2   = 0,                          -- Ending sprite frame #
      S2A  = 0,                          -- Ending attachment sprite frame #
      SA   = 0,                          -- Current attachment sprite frame #
      SM   = aObjData.STAMINA,           -- Object stamina
      SMM1 = aObjData.STAMINA-1,         -- Object stamina minus one
      ST   = 0,                          -- Sprite animation timer
      STA  = aObjData.ATTACHMENT,        -- Object attachment id
      STR  = aObjData.STRENGTH,          -- Object strength
      STRK = -ceil(aObjData.STRENGTH/4), -- Object damage on kick
      STRP = -ceil(aObjData.STRENGTH/2), -- Object damage on punch
      SX   = iX,                         -- Object starting position
      SY   = iY,                         -- Object starting position
      TD   = { },                        -- Ignored teleport destinations
      TL   = { },                        -- Objects that are targeting this
      U    = iUniqueId,                  -- Unique id
      US   = aParent and aParent.US,     -- Automatically un-shroud?
      W    = aObjData.WEIGHT,            -- Carry amount / requirement
      X    = 0,                          -- Absolute X position
      Y    = 0,                          -- Absolute Y position
    };
    -- Is AI function specified and not a player controlled parent?
    if iAI ~= AI.NONE and (not aParent or aParent.AI) then
      -- Lookup AI function and check that it's a function
      local fcbAIFunc<const> = aAIFuncs[iAI];
      if not UtilIsFunction(fcbAIFunc) then
        error("Unknown AI #"..tostring(iAI).."! "..tostring(fcbAIFunc)) end;
      -- Valid function to set it to call every tick
      aObject.AIF = fcbAIFunc;
    end
    -- Set position of object
    SetPosition(aObject, iX, iY);
    -- Set default action (also sets default direction);
    SetAction(aObject, aObjData.ACTION, aObjData.JOB, aObjData.DIRECTION);
    -- Insert into main object array
    aObjects[1 + #aObjects] = aObject;
    -- Log creation of item
    CoreLog("Created object '"..sName.."'["..iObjId.."] at X:"..
      iX..",Y:"..iY.." in position #"..#aObjects.."!");
    -- Return object
    return aObject;
  end
  -- Return the actual function
  return CreateObject;
end
-- Proc object floating ---------------------------------------------------- --
local function CheckObjectFloating(aObject)
  -- Ignore if object doesn't float
  if aObject.F & OFL.FLOAT == 0 then return false end;
  -- Get tile position and get tile id from level data and return if not water
  if not CheckObjectInWater(aObject, 13) then
    -- Floating if the pixel below is water
    if not CheckObjectInWater(aObject, 14) then
      -- Remove floating flag and all fall back
      aObject.F = (aObject.F | OFL.FALL) & ~OFL.FLOATING;
      -- Not floating so can fall
      return false;
    end
   -- Tile is water?
  else
    -- Set floating flag and remove fall flag
    aObject.F = (aObject.F | OFL.FLOATING) & ~OFL.FALL;
    -- Rise
    MoveY(aObject, -1);
  end
  -- Object is floating
  return true;
end
-- Process phasing logic function ------------------------------------------ --
local function PhaseLogic()
  -- AI player entered trade centre logic
  local function AIEnterTradeCentreLogic(aObj)
    -- Hide the digger
    SetAction(aObj, ACT.HIDE, JOB.PHASE, DIR.R);
    -- Number of items sold
    local iItemsSold = 0;
    -- Get object inventory and if inventory held
    local aInventory<const> = aObj.I;
    if #aInventory == 0 then return end;
    -- Get owner of this object
    local aParent<const> = aObj.P;
    -- For each item in digger inventory. We have to use a while loop
    -- as we need to remove items from the inventory.
    local iObj = 1 while iObj <= #aInventory do
      -- Get the inventory object and if the gem is sellable or the
      -- object has a owner and doesn't belong to this objects owner?
      -- Then try to sell the item and if succeeded? Increment the
      -- items sold.
      local aInvObj<const> = aInventory[iObj];
      local aInvParent<const> = aInvObj.P;
      if (CanSellGem(aInvObj.ID) or
         (aInvParent and aInvParent ~= aParent)) and
        SellItem(aObj, aInvObj) then iItemsSold = iItemsSold + 1;
      -- Conditions fail so try next inventory item.
      else iObj = iObj + 1 end;
    end
    -- Get opponent player
    local iOpponentID;
    if aParent.ID == 1 then iOpponentID = 2 else iOpponentID = 1 end;
    local aOpponent<const> = aPlayers[iOpponentID];
    -- If object...
    if random() > aObj.IN and             -- Isn't intelligent?
       aParent.M > aOpponent.M + 400 then -- *or* Way more cash?
      -- Try to purchase something random to keep the scores fair
      BuyItem(aObj, aShopData[random(#aShopData)])
    end
    -- If items were sold? Check if any player won
    if iItemsSold > 0 then EndConditionsCheck() end;
  end
  -- Phasing home or to a Telepole logic
  local function PhaseHomeOrTelepoleLogic(aObj)
    -- Reduce health as a cost for teleporting
    if aObj.F & OFL.TPMASTER == 0 then AdjustObjectHealth(aObj, -5) end;
    -- Go home?
    local bGoingHome = true;
    -- Is teleport master?
    local bIsTeleMaster<const> = aObj.F & OFL.TPMASTER ~= 0;
    -- Get current teleport destinations
    local aDestinations<const> = aObj.TD;
    -- For each object
    for iObjDestIndex = 1, #aObjects do
      -- Get object
      local TO<const> = aObjects[iObjDestIndex];
      -- If object is a Telepole and object is owned by this object
      -- or object is a teleport master and Telepole status nominal?
      if TO.ID == TYP.TELEPOLE and
         (TO.P == aObj.P or bIsTeleMaster) and
         TO.A == ACT.STOP then
        -- Ignore Telepole?
        local bIgnoreTelepole = false;
        -- Enumerate teleport destinations...
        for IDI = 1, #aDestinations do
          -- Get destination and set true if we've been here before
          local aDestination<const> = aDestinations[IDI];
          if aDestination == TO then bIgnoreTelepole = true break end;
        end
        -- Don't ignore Telepole?
        if not bIgnoreTelepole then
          -- Teleport to this device
          SetPosition(aObj, TO.X, TO.Y);
          -- Re-phase back into stance
          SetAction(aObj, ACT.PHASE, JOB.NONE, DIR.NONE);
          -- Add it to ignore teleport destinations list for next time.
          aDestinations[1 + #aDestinations] = TO;
          -- Don't go home
          bGoingHome = false;
          -- Done
          break;
        end
      end
    end
    -- Going home?
    if bGoingHome == true then
      -- Clear objects ignore destination list
      for iTDIndex = #aDestinations, 1, -1 do
        remove(aDestinations, iTDIndex) end
      -- Set position of object to player's home
      SetPosition(aObj, aObj.P.HX, aObj.P.HY);
      -- Re-phase back into stance
      SetAction(aObj, ACT.PHASE, JOB.NONE, DIR.KEEP);
    end
  end
  -- Player entering trade centre logic
  local function PlayerEnterTradeCentreLogic(aObj)
    -- Now in trade centre
    SetAction(aObj, ACT.HIDE, JOB.PHASE, DIR.R);
    -- Set new active object to this one
    aActiveObject, aActiveMenu = aObj, nil;
    -- We don't want to hear sounds
    SetPlaySounds(false);
    -- Init lobby
    InitLobby(aActiveObject, false, 1);
  end
  -- Phase to a random target logic
  local function PhaseRandomTargetLogic(aObj)
    -- Walk objects list and find candidate objects to teleport to
    local aCandObjs<const> = { };
    for iCandObjId = 1, #aObjects do
      -- Get object and if the object isn't this object and object is
      -- a valid phase target? Insert into valid phase targets list.
      local aCandObj<const> = aObjects[iCandObjId];
      if aCandObj ~= aObj and aCandObj.F & OFL.PHASETARGET ~= 0 then
        aCandObjs[1 + #aCandObjs] = aCandObj;
      end
    end
    -- Have candidate objects?
    if #aCandObjs > 0 then
      -- Pick random object from array
      local aCandObj<const> = aCandObjs[random(#aCandObjs)];
      -- Set this objects position to the object
      SetPosition(aObj, aCandObj.X, aCandObj.Y);
      -- Re-phase back into stance
      SetAction(aObj, ACT.PHASE, JOB.NONE, DIR.KEEP);
    -- Else if object has owner, teleport home
    elseif aObj.P then
      -- Set position of object to player's home
      SetPosition(aObj, aObj.P.HX, aObj.P.HY);
      -- Re-phase back into stance
      SetAction(aObj, ACT.PHASE, JOB.NONE, DIR.KEEP);
    -- No owner = no home, just de-phase
    else SetAction(aObj, ACT.STOP, JOB.NONE, DIR.KEEP) end;
  end
  -- Functions base on direction
  local aFunctions<const> = {
    [DIR.R]  = AIEnterTradeCentreLogic,
    [DIR.U]  = PhaseHomeOrTelepoleLogic,
    [DIR.UL] = PlayerEnterTradeCentreLogic,
    [DIR.D]  = PhaseRandomTargetLogic,
    [DIR.DR] = PhaseRandomTargetLogic
  };
  -- New function
  local function PhaseLogicInitialised(aObj)
    -- Object has finished phasing
    if aObj.J ~= JOB.PHASE then
      -- Object was teleported eaten? Respawn as eaten!
      if aObj.D == DIR.DR then SetAction(aObj, ACT.EATEN, JOB.NONE, DIR.LR);
      -- Object not eaten?
      else SetAction(aObj, ACT.STOP, JOB.NONE, DIR.KEEP) end;
      -- Done
      return;
    end
    -- If not demo mode and this object is selected and not parent
    if not bAIvsAI and aActiveObject == aObj and
      (not aObj.P or aObj.P ~= aActivePlayer) then
      -- Deselect it. Opponents are not allowed to see where they went!
      aActiveObject = nil
    end
    -- Get and check logic function for phasing.
    local fcbLogic<const> = aFunctions[aObj.D];
    if not UtilIsFunction(fcbLogic) then
      error("Invalid phase logic function at "..aObj.D.."! "..
        tostring(fcbLogic)) end;
    -- Execute the phase logic
    fcbLogic(aObj);
  end
  -- Set real function
  PhaseLogic = PhaseLogicInitialised;
end
-- Apply object inventory perks -------------------------------------------- --
local function ApplyObjectInventoryPerks(aObject);
  -- Get objects inventory and return if no inventory
  local aInventory<const> = aObject.I;
  if #aInventory == 0 then return end;
  -- Enumerate each one. Have to use 'while' as inventory can disappear
  local iIndex = 1;
  while iIndex <= #aInventory do
    -- Get object in inventory and if item is first aid?
    local aItem<const> = aInventory[iIndex];
    if aItem.ID == TYP.FIRSTAID then
      -- Get carriers health and if carrier is damaged?
      local iHealth<const> = aObject.H;
      if iHealth > 0 then
        -- Increase holder's health and decrease first aid health
        AdjustObjectHealth(aObject, 1);
        AdjustObjectHealth(aItem, -1);
      end
    end
    -- Item has health or couldn't drop object? Enumerate next item!
    if aItem.H > 0 or not DropObject(aObject, aItem) then
      iIndex = iIndex + 1;
    end
  end
end
-- Animate terrain --------------------------------------------------------- --
local function AnimateTerrain()
  -- For each screen row we are looking at
  for iY = 0, iScrTilesHm1 do
    -- Calculate the Y position to grab from the level data
    local iYdest<const> = (iPosY + iY) * iLLAbsW;
    -- For each screen column we are looking at
    for iX = 0, iScrTilesWm1 do
      -- Get absolute position on map
      local iPos<const> = iYdest + iPosX + iX;
      -- Get tile id and if tile is valid?
      local iTileId<const> = aLevelData[1 + iPos];
      if iTileId ~= 0 then
        -- Get tile flags and if flags say we should animate to next tile?
        local iFlags<const> = aTileData[1 + iTileId];
        if iFlags & aTileFlags.AB ~= 0 then
          aLevelData[1 + iPos] = iTileId + 1;
        -- Tile is end of animation so go back 3 sprites. This rule means
        -- that all animated terrain tiles must be 4 sprites.
        elseif iFlags & aTileFlags.AE ~= 0 then
          aLevelData[1 + iPos] = iTileId - 3 end;
      end
    end
  end
end
-- Process flood data ------------------------------------------------------ --
local function ProcessFloodData()
  -- Until we've removed all the current entries
  for iRemaining = #aFloodData, 1, -1 do
    -- Get flood data and remove it from list
    local aFloodItem<const> = aFloodData[1];
    remove(aFloodData, 1);
    iRemaining = iRemaining - 1;
    -- Get position and flags of tile being flooded and if tile exposes left?
    local iTilePos<const>, iTileFlags<const> = aFloodItem[1], aFloodItem[2];
    if iTileFlags & aTileFlags.EL ~= 0 then
      -- Get information about the tile on the left and if valid
      local iPosition<const> = iTilePos - 1;
      local iTileId<const> = GetLevelDataFromLevelOffset(iPosition);
      if iTileId then
        -- Get file flags and if is water and the right edge is exposed?
        local iTileFlags = aTileData[1 + iTileId];
        if iTileFlags & aTileFlags.W == 0 and
           iTileFlags & aTileFlags.ER ~= 0 then
          -- If the tile is a flood gate tile?
          if iTileFlags & aTileFlags.G ~= 0 then
            -- Get transformation information about this floodgate tile
            local aFGDItem<const> = aFloodGateData[iTileId][1];
            -- Update the flooded gate tile
            aLevelData[1 + iPosition] = aFGDItem[1];
            -- Continue flooding if needed
            if aFGDItem[2] then
              aFloodData[1 + #aFloodData] = { iPosition, iTileFlags } end;
          -- Tile is not a flood gate tile?
          else
            -- Update tile to the same tile with water in it
            aLevelData[1 + iPosition] = iTileId + 240;
            -- Continue flooding if the left edge of the tile is exposed
            if iTileFlags & aTileFlags.EL ~= 0 then
              aFloodData[1 + #aFloodData] = { iPosition, iTileFlags } end;
          end
        end
      end
    end
    -- If tile flags exposed right?
    if iTileFlags & aTileFlags.ER ~= 0 then
      -- Get information about the tile on the right and if valid
      local iPosition<const> = iTilePos + 1;
      local iTileId<const> = GetLevelDataFromLevelOffset(iPosition);
      if iTileId then
        -- Get file flags and if is water and the left edge is exposed?
        local iTileFlags<const> = aTileData[1 + iTileId];
        if iTileFlags & aTileFlags.W == 0 and
           iTileFlags & aTileFlags.EL ~= 0 then
          -- If the tile is a flood gate tile?
          if iTileFlags & aTileFlags.G ~= 0 then
            -- Get transformation information about this floodgate tile
            local aFGDItem<const> = aFloodGateData[iTileId][2];
            -- Update the flooded gate tile
            aLevelData[1 + iPosition] = aFGDItem[1];
            -- Continue flooding if data requests it
            if aFGDItem[2] then
              aFloodData[1 + #aFloodData] = { iPosition, iTileFlags } end;
          -- Tile is not a flood gate tile?
          else
            -- Update tile to the same tile with water in it
            aLevelData[1 + iPosition] = iTileId + 240;
            -- Continue flooding if the left edge of the tile is exposed
            if iTileFlags & aTileFlags.ER ~= 0 then
              aFloodData[1 + #aFloodData] = { iPosition, iTileFlags } end;
          end
        end
      end
    end
    -- If tile flags exposed down?
    if iTileFlags & aTileFlags.EB ~= 0 then
      -- Get information about the tile below and if valid
      local iPosition<const> = iTilePos + iLLAbsW;
      local iTileId<const> = GetLevelDataFromLevelOffset(iPosition);
      if iTileId then
        -- Get file flags and if is water and the left edge is exposed?
        local iTileFlags<const> = aTileData[1 + iTileId];
        if iTileFlags & aTileFlags.W == 0 and
           iTileFlags & aTileFlags.ET ~= 0 then
          -- Update tile to the same tile with water in it and continue
          aLevelData[1 + iPosition] = iTileId + 240;
          aFloodData[1 + #aFloodData] = { iPosition, iTileFlags };
        end
      end
    end
  end
end
-- Check if object is falling ---------------------------------------------- --
local function CheckObjectFalling(aObject)
  -- If object...
  if not CheckObjectFloating(aObject) and -- ...is not floating?
         aObject.F & OFL.FALL ~= 0 and    -- *and* is supposed to fall?
     not IsCollideY(aObject, 1) then      -- *and* not blocked below?
    -- Start from fall speed pixels and count down to 1
    for iYAdj = aObject.FS, 1, -1 do
      -- No collision found with terrain?
      if not IsCollideY(aObject, iYAdj) then
        -- Move Y position down
        MoveY(aObject, iYAdj);
        -- Increase fall speed and clamp maximum speed to half a tile
        if aObject.FS < 8 then aObject.FS = aObject.FS + 1 end;
        -- increase fall damage
        aObject.FD = aObject.FD + 1;
        -- Still falling
        return true;
      end
      -- Collision detected, but we must find how many pixels exactly to fall
      -- by, so reduce the pixel count and try to find the exact value.
    end
  end
  -- We were falling?
  if aObject.FS > 1 then
    -- Reset fall speed
    aObject.FS = 1;
    -- If fall damage is set then object fell and now we must reduce its
    -- health
    local iDamage = aObject.FD;
    if iDamage > 0 then
      -- Object still has fall flag set and object fell >= 16 pixels
      if aObject.F & OFL.FALL ~= 0 and iDamage >= 5 then
        -- Object is delicate? Remove more health
        if aObject.F & OFL.DELICATE ~= 0 then iDamage = -iDamage;
        else iDamage = -(iDamage // 2) end;
        -- Reduce health
        AdjustObjectHealth(aObject, iDamage);
        -- Damage would reduce health < 10 %. Stop object moving
        local iHealth<const> = aObject.H;
        if iHealth > 0 and iHealth < 10 then
          SetAction(aObject, ACT.STOP, JOB.INDANGER, DIR.NONE);
        end
      end
      -- Reset fall damage;
      aObject.FD = 0;
    end
  end
  -- Can't fall (anymore)
  return false;
end
-- Process object movement logic ------------------------------------------- --
local function ProcessObjectMovement()
  -- Move object functions
  local function OBJMoveUp(aObj) MoveY(aObj, -1) end;
  local function OBJMoveDown(aObj) MoveY(aObj, 1) end;
  local function OBJMoveLeft(aObj) MoveX(aObj, -1) end;
  local function OBJMoveRight(aObj) MoveX(aObj, 1) end;
  -- Object move callbacks
  local aMoveFuncs<const> = {
    [DIR.U]  = OBJMoveUp,    [DIR.D] = OBJMoveDown,
    [DIR.UL] = OBJMoveLeft,  [DIR.L] = OBJMoveLeft,  [DIR.DL] = OBJMoveLeft,
    [DIR.UR] = OBJMoveRight, [DIR.R] = OBJMoveRight, [DIR.DR] = OBJMoveRight
  };
  -- Actual function
  local function ProcessObjectMovement(aObj)
    -- Object wants to dig down and object X position is in the middle of
    -- the tile? Make object dig down
    if aObj.J == JOB.DIGDOWN and aObj.X % 16 == 0 then
      SetAction(aObj, ACT.DIG, JOB.DIGDOWN, DIR.D);
    -- Object wants to enter the trading centre? Stop object
    elseif aObj.J == JOB.HOME and ObjectIsAtHome(aObj) then
      -- Go into trade centre
      SetAction(aObj, ACT.PHASE, JOB.PHASE, DIR.UL);
    -- Object is for rails only and train is not on track
    elseif aObj.F & OFL.TRACK ~= 0 then
      -- Get X pos adjust depending on direction
      local iId;
      if aObj.D == DIR.L then iId = 7 else iId = 9 end;
      -- Get absolute tile position and get tile id
      local iLoc<const> = GetLevelOffsetFromObject(aObj, iId, 0);
      if iLoc then
        iId = aLevelData[1 + iLoc];
        -- Tile at end of track? Stop!
        if aTileData[1 + iId] & aTileFlags.T == 0 then
          SetAction(aObj, ACT.STOP, JOB.NONE, DIR.NONE);
        -- Move train
        elseif aObj.D == DIR.L then MoveX(aObj,-1);
        elseif aObj.D == DIR.R then MoveX(aObj, 1) end;
      end
    else
      -- If timer goes over 1 second and object is busy
      -- Unset busy flag as abnormal digging can make it stick
      if aObj.AT >= 60 and aObj.F & OFL.BUSY ~= 0 then
        aObj.F = aObj.F & ~OFL.BUSY end;
      -- Get function associated with direction and move appropriately. It is
      -- assumed that all directions are catered for thus no check required.
      aMoveFuncs[aObj.D](aObj);
    end
  end
  -- Return new function
  return ProcessObjectMovement;
end
-- Process object logic ---------------------------------------------------- --
local function ProcessObjects()
  -- Enumerate through all objects (while/do because they could be deleted)
  local iObjId = 1 while iObjId <= #aObjects do
    -- Get object data
    local aObj<const> = aObjects[iObjId];
    -- If AI function set, not dying and not phasing then call AI function!
    local fcbAI<const> = aObj.AIF;
    if fcbAI then fcbAI(aObj) end;
    -- If object can't fall or it finished falling and isn't floating?
    if not CheckObjectFalling(aObj) then
      -- Object is jumping?
      if aObj.F & OFL.JUMPRISE ~= 0 then
        -- Object can move up and the rise limit hasn't been reached yet
        if not IsCollideY(aObj, -1) and aObj.AT < #aJumpRiseData then
          -- We can actually move up? Move up!
          if aJumpRiseData[aObj.AT+1] > 0 then MoveY(aObj, -1) end;
        -- Object cannot move up or action timer is the last frame
        else
          -- Remove rising and set falling flags
          aObj.F = (aObj.F | OFL.JUMPFALL) & ~OFL.JUMPRISE;
          -- Reset action timer
          aObj.AT = 0;
        end
      -- Object is falling (during the jump)?
      elseif aObj.F & OFL.JUMPFALL ~= 0 then
        -- Object can fall down and the fall limit hasn't been reached yet
        if not IsCollideY(aObj, 1) and aObj.AT < #aJumpFallData then
          -- And we can drop down? Drop down
          if aJumpFallData[aObj.AT+1] > 0 then MoveY(aObj, 1) end;
        -- Object cannot move down or action timer is the last frame
        else
          -- Let object fall normally now and remove busy and falling flags
          aObj.F = (aObj.F | OFL.FALL) & ~(OFL.JUMPFALL | OFL.BUSY);
          -- Thus little tweak makes sure the user can't jump again by just
          -- modifying the fall speed as ACT.JUMP requires it to be 1.
          aObj.FS = 2;
          -- Reset action timer
        end
      end
      -- Object is...
      if aObj.A == ACT.DEATH and                    -- ...dead?
         aObj.AT >= aTimerData.DEADWAIT and         -- ...death timer exceeded?
         DestroyObject(iObjId, aObj) then goto EOO; -- ...object destroyed?
      -- Object is phasing and phase delay reached?
      elseif aObj.A == ACT.PHASE and aObj.AT >= aObj.OD.TELEDELAY then
        -- Process the phase destination
        PhaseLogic(aObj);
      -- Object is hidden and object is in the trade-centre?
      elseif aObj.A == ACT.HIDE and aObj.J == JOB.PHASE then
        -- Health at full?
        if aObj.H >= 100 then
          -- Re-appear the object if is not the player otherwise wait for the
          -- real player to exit the trade centre.
          if bAIvsAI or aObj.P ~= aActivePlayer then
            SetAction(aObj, ACT.PHASE, JOB.NONE, DIR.R) end;
        -- Health not full? Regenerate health
        elseif aObj.AT % 10 == 0 then AdjustObjectHealth(aObj, 1) end;
      -- Object has been eaten
      elseif aObj.A == ACT.EATEN and aObj.AT >= aTimerData.MUTATEWAIT then
        -- Kill digger and spawn alien
        AdjustObjectHealth(aObj, -100);
        CreateObject(TYP.ALIEN, aObj.X, aObj.Y);
      -- Object is dying? Slowly drain it's health
      elseif aObj.A == ACT.DYING and aObj.AT % 6 == 0 then
        AdjustObjectHealth(aObj, -1);
      -- Object is creeping, walking and running? Limit FPS depending on action
      elseif (aObj.A == ACT.CREEP and aObj.AT % 4 == 0) or
             (aObj.A == ACT.WALK and aObj.AT % 2 == 0) or
              aObj.A == ACT.RUN then
        -- Process object movement logic
        ProcessObjectMovement(aObj);
      -- Object is digging and digging delay reached?
      elseif aObj.A == ACT.DIG and aObj.AT >= aObj.DID then
        -- Terrain dig was successful?
        if DigTile(aObj) == true then
          -- Continue to walk in the direction
          SetAction(aObj, ACT.WALK, JOB.KEEP, DIR.KEEP);
          -- Increase dig count
          SetObjectAndParentCounter(aObj, "DUG");
          -- Update dig time for AI
          aObj.LDT = iGameTicks;
        -- Not successful
        else
          -- Update failed dig direction
          aObj.FDD = aObj.D;
          -- Set impatient
          aObj.JT = aObj.PW;
          -- stop the object's actions completely
          SetAction(aObj, ACT.STOP, JOB.NONE, DIR.KEEP);
        end;
      end
      -- Post action job process. Object search for other objects to pickup?
      if aObj.J == JOB.SEARCH and aObj.AT % aTimerData.PICKUPDELAY == 0 then
        -- Pickup nearest treasure
        PickupObjects(aObj, true);
      -- Object is in danger and danger timeout is reached?
      elseif aObj.J == JOB.INDANGER and
             aObj.AT >= aTimerData.DANGERTIMEOUT then
        -- Remove the objects job
        SetAction(aObj, ACT.KEEP, JOB.NONE, DIR.KEEP);
      end
      -- Object can regenerate health? Regenerate health!
      if aObj.F & OFL.REGENERATE ~= 0 and
         aObj.AT % aObj.SM == aObj.SMM1 then AdjustObjectHealth(aObj, 1) end;
    end
    -- If object is not phasing or dying?
    if aObj.A ~= ACT.PHASE and aObj.A ~= ACT.DEATH then
      -- Check object fighting
      CheckObjectCollision(aObj);
      -- Check if object is underwater
      CheckObjectUnderwater(aObj);
      -- Walk object's inventory
      ApplyObjectInventoryPerks(aObj);
    end
    -- Process animations
    AnimateObject(aObj);
    -- Increase action and job timer
    aObj.AT, aObj.JT = aObj.AT + 1, aObj.JT + 1;
    -- Process next object
    iObjId = iObjId + 1;
    -- Label for skipping to next object
    ::EOO::
  end
end
-- Game tick function ------------------------------------------------------ --
local function GameProc()
  -- Process terrain animation
  if iGameTicks % aTimerData.ANIMTERRAIN == 0 then AnimateTerrain() end
  -- Process flood data
  if #aFloodData > 0 then ProcessFloodData() end
  -- Process object logic
  ProcessObjects();
  -- Rotate gems in bank every five minutes
  if iGameTicks % 18000 == 0 then
    -- Move first gem to last gem
    aGemsAvailable[1 + #aGemsAvailable] = aGemsAvailable[1];
    remove(aGemsAvailable, 1);
  end
  -- Increment game ticks processed count
  iGameTicks = iGameTicks + 1;
end
-- Get mouse position on level --------------------------------------------- --
local function GetAbsMousePos()
  return GetMouseX() - iStageL + iPixPosX, GetMouseY() - iStageT + iPixPosY;
end
-- Finds object under mouse cursor ----------------------------------------- --
local function FindObjectUnderCursor()
  -- Translate current mouse position to absolute level position
  local nMouseX<const>, nMouseY<const> = GetAbsMousePos();
  -- Walk through objects in backwards order. This is because objects are
  -- drawn from oldest to newest.
  for iIndex = #aObjects, 1, -1 do
    -- Get object
    local aObject<const> = aObjects[iIndex];
    -- Mouse cursor collides with object? Set object and return success
    if IsSpriteCollide(479, nMouseX, nMouseY,
         aObject.S, aObject.X, aObject.Y) then
      SelectObject(aObject);
      return PlayStaticSound(aSfxData.SELECT);
    end
  end
  -- Nothing found so deselect current object
  SelectObject();
end
-- Update object menu position --------------------------------------------- --
local function UpdateMenuPosition(X, Y)
  -- Get menu size (reuse vars)
  iMenuRight, iMenuBottom = aActiveMenu[1] * 16, aActiveMenu[2] * 16;
  -- Update left and top co-ordinates
  iMenuLeft, iMenuTop = UtilClampInt(X, iStageL, iStageR - iMenuRight),
                        UtilClampInt(Y, iStageT, iStageB - iMenuBottom);
  -- Update right and bottom co-ordinates
  iMenuRight, iMenuBottom = iMenuRight + iMenuLeft, iMenuBottom + iMenuTop;
end
-- Update object menu position with current mouse -------------------------- --
local function UpdateMenuPositionAtMouseCursor()
  UpdateMenuPosition(GetMouseX(), GetMouseY());
end
-- Set active object menu -------------------------------------------------- --
local function SetMenu(Id, UpdatePos)
  -- Set new menu and check that we got the data for it
  aActiveMenu = aMenuData[Id];
  if not UtilIsTable(aActiveMenu) then
    error("Invalid menu data for "..Id.."! "..tostring(aActiveMenu)) end;
  -- Update menu position if requested
  if UpdatePos then UpdateMenuPositionAtMouseCursor();
  -- Just update menu size
  else UpdateMenuPosition(iMenuLeft, iMenuTop) end;
end
-- Open context menu for object -------------------------------------------- --
local function OpenObjectMenu()
  -- Get menu data
  local aMenu<const> = aActiveObject.OD.MENU;
  -- Object has menu and object belongs to active player and object isn't
  -- dead or eaten?
  if aMenu and
     aActiveObject.P == aActivePlayer and
     aActiveObject.A ~= ACT.DEATH and
     aActiveObject.A ~= ACT.EATEN then
    -- Object does belong to active player so play context menu sound and
    -- set the appropriate default menu for the object.
    PlayStaticSound(aSfxData.CLICK);
    SetMenu(aMenu, true);
    -- Success
    return true;
  -- Object does not belong to active player? Play error sound and return fail
  else PlayStaticSound(aSfxData.ERROR) return false end;
end
-- Process context menu item button click----------------------------------- --
local function ProcessMenuClick()
  -- Calculate button pressed and absolute index
  local iIndex<const> =
    ((GetMouseY() - iMenuTop) // 16 * aActiveMenu[1]) +
    ((GetMouseX() - iMenuLeft) // 16 + 1);
  -- Get menu data for id
  local aMIData<const> = aActiveMenu[3][iIndex];
  if not UtilIsTable(aMIData) then
    error("Invalid menu item data for "..iIndex.."! "..tostring(aMIData)) end;
  -- If tile denies if object is busy? Disallow action and play sound
  if aMIData[2] & MFL.BUSY ~= 0 and aActiveObject.F & OFL.BUSY ~= 0 then
    PlayStaticSound(aSfxData.ERROR);
  -- New menu specified? Set new menu and play sound
  elseif aMIData[3] ~= MNU.NONE then
    SetMenu(aMIData[3], false);
    PlayStaticSound(aSfxData.SELECT);
  -- New action specified?
  elseif aMIData[4] ~= 0 and aMIData[5] ~= 0 and aMIData[6] ~= 0 then
    -- Play the click sound
    PlayStaticSound(aSfxData.SELECT);
    -- Set the action and if failed? Play the error sound
    if not SetAction(aActiveObject, aMIData[4], aMIData[5], aMIData[6], true)
      then PlayStaticSound(aSfxData.ERROR) end;
  end
end
-- Player clicked in the numbered digger buttons --------------------------- --
local function ProcessDiggerButtonClick()
  -- Get which button was clicked
  local iButtonId<const> = (GetMouseX() - 144) // 16 + 1;
  -- Get digger associated with the button id and return failure if dead
  local aObject<const> = aActivePlayer.D[iButtonId];
  if not aObject then return PlayStaticSound(aSfxData.ERROR) end;
  -- Select digger and kill menu
  SelectObject(aObject);
  -- Play select sound
  PlayStaticSound(aSfxData.CLICK);
end
-- Return if we can move selected digger ----------------------------------- --
local function SelectedDiggerIsMovable()
  -- Return true if an object selected, object is digger, object belongs to me
  -- and object is not busy else return false.
  return aActiveObject and aActiveObject.F & OFL.DIGGER ~= 0 and
     aActiveObject.P == aActivePlayer and aActiveObject.F & OFL.BUSY == 0;
end
-- Select devices ---------------------------------------------------------- --
local function SelectDevice()
  -- For each object
  for iId = 1, #aObjects do
    -- Get object and if the object belongs to me and is device?
    local aObject<const> = aObjects[iId];
    if aActivePlayer == aObject.P and
       aObject.F & OFL.DEVICE ~= 0 then
      -- Remove and send to front of objects list
      remove(aObjects, iId);
      aObjects[1 + #aObjects] = aObject;
      -- Set as active object
      SelectObject(aObject);
      -- Success!
      return PlayStaticSound(aSfxData.CLICK);
    end
  end
  -- Failed? Play sound
  PlayStaticSound(aSfxData.ERROR);
end
-- Check input state ------------------------------------------------------- --
local function ProcInput()
  -- Get keyboard ids
  local aKeys<const> = Input.KeyCodes;
  -- Keyboard aliases needed
  local iKey1<const>,      iKey2<const>,         iKey3<const>,
        iKey4<const>,      iKey5<const>,         iKeyKp0<const>,
        iKeyKp1<const>,    iKeyKp2<const>,       iKeyKp3<const>,
        iKeyKp4<const>,    iKeyKp5<const>,       iKeyMinus<const>,
        iKeyEquals<const>, iKeyBackspace<const>, iKeyEnter<const>,
        iKeyUp<const>,     iKeyDown<const>,      iKeyLeft<const>,
        iKeyRight<const>,  iKeyEscape<const>,    iKeyS<const>,
        iKeyT<const>,      iKeyHome<const>,      iKeyEnd<const>,
        iKeyDelete<const>, iKeyPgDown<const>,    iKeyReturn<const>,
        iKeyF5<const>,     iKeyF6<const>,        iKeyF7<const>,
        iKeyF8<const>,     iKeyE<const>,         iKeySpacebar<const> =
          aKeys.ONE,       aKeys.TWO,       aKeys.THREE,
          aKeys.FOUR,      aKeys.FIVE,      aKeys.KP_0,
          aKeys.KP_1,      aKeys.KP_2,      aKeys.KP_3,
          aKeys.KP_4,      aKeys.KP_5,      aKeys.MINUS,
          aKeys.EQUAL,     aKeys.BACKSPACE, aKeys.KP_ENTER,
          aKeys.UP,        aKeys.DOWN,      aKeys.LEFT,
          aKeys.RIGHT,     aKeys.ESCAPE,    aKeys.S,
          aKeys.T,         aKeys.HOME,      aKeys.END,
          aKeys.DELETE,    aKeys.PAGE_DOWN, aKeys.ENTER,
          aKeys.F5,        aKeys.F6,        aKeys.F7,
          aKeys.F8,        aKeys.E,         aKeys.SPACE;
  -- Select information screen --------------------------------------------- --
  local function SelectInfoScreen(iScreen)
    -- Play sound effect to show the player clicked it
    PlayStaticSound(aSfxData.CLICK);
    -- If the screen is currently selected? Turn off the info screen
    if iInfoScreen == iScreen then iInfoScreen = nil;
    -- Turn on the specified info screen
    else iInfoScreen = iScreen end;
  end
  -- Select info screens --------------------------------------------------- --
  local function SelectInventoryScreen() SelectInfoScreen(1) end;
  local function SelectLocationScreen() SelectInfoScreen(2) end;
  local function SelectStatusScreen() SelectInfoScreen(3) end;
  -- Init the book --------------------------------------------------------- --
  local function SelectBook()
    -- Play sound effect to show the player clicked it
    PlayStaticSound(aSfxData.CLICK);
    -- Remove play sound function
    SetPlaySounds(false);
    -- Init the book
    InitBook(true);
  end
  -- Select digger if active ----------------------------------------------- --
  local function SelectDigger(iDiggerId)
    -- Get specified digger and return if not found
    local aDigger<const> = aActivePlayer.D[iDiggerId];
    if not aDigger then return end;
    -- Select the digger
    SelectObject(aDigger);
  end
  -- Select an adjacent digger --------------------------------------------- --
  local function SelectAdjacentDigger(iNegate)
    -- Find the object we selected first
    local iCurrentDigger;
    for iI = 1, 5 do
      if aActiveObject == aActivePlayer.D[iI] then iCurrentDigger = iI end
    end
    -- No active digger? Find a digger we own
    if not iCurrentDigger then
      for iI = 1, 5 do if aActivePlayer.D[iI] then iCurrentDigger = iI end end;
    end
    -- Find currently active digger and return if not found
    if not iCurrentDigger then return end;
    -- Walk through the next 4 diggers
    for iI = 1, 4 do
      -- Get next digger wrapped and select and return it if found
      local aDigger<const> =
        aActivePlayer.D[1 + (((iCurrentDigger + iNegate) - 1) % 5)];
      if aDigger then return SelectObject(aDigger) end;
    end
  end
  -- Adjust viewport and prevent scroll ------------------------------------ --
  local function AdjustViewPortXNS(iX)
    AdjustViewPortX(iX); iPixPosTargetX = iPosX * 16 end;
  local function AdjustViewPortYNS(iY)
    AdjustViewPortY(iY); iPixPosTargetY = iPosY * 16 end;
  -- Returns current pixel tile under mouse cursor ------------------------- --
  local function GetTileUnderMouse()
    return
      UtilClampInt((iPosX * 16) + GetMouseX() - iStageL, 0, iLLPixWm1),
      UtilClampInt((iPosY * 16) + GetMouseY() - iStageT, 0, iLLPixHm1);
  end
  -- Returns current absolute tile under mouse cursor ---------------------- --
  local function GetAbsTileUnderMouse()
    local iX<const>, iY<const> = GetTileUnderMouse();
    return iX // 16, iY // 16;
  end
  -- We'll set ProcInput to this so we can keep aliases local to this routine.
  local function ProcInputInitialised()
    -- Process viewport scrolling
    ProcessViewPort();
    -- Player wants to select the first digger?
    if IsKeyPressed(iKey1) or IsKeyPressed(iKeyKp1) then
      return SelectDigger(1) end;
    -- Player wants to select the second digger?
    if IsKeyPressed(iKey2) or IsKeyPressed(iKeyKp2) then
      return SelectDigger(2) end;
    -- Player wants to select the third digger?
    if IsKeyPressed(iKey3) or IsKeyPressed(iKeyKp3) then
      return SelectDigger(3) end;
    -- Player wants to select the fourth digger?
    if IsKeyPressed(iKey4) or IsKeyPressed(iKeyKp4) then
      return SelectDigger(4) end;
    -- Player wants to select the fifth digger?
    if IsKeyPressed(iKey5) or IsKeyPressed(iKeyKp5) then
      return SelectDigger(5) end;
    -- Left button pressed?
    if IsKeyPressed(iKeyMinus) or IsButtonPressed(6) or IsScrollingDown() then
      -- Cycle to previous item if digger inventory menu open?
      if aActiveMenu and aActiveMenu == aMenuData[MNU.DROP] then
        CycleObjInventory(aActiveObject, -1);
      -- Else select previous digger
      else SelectAdjacentDigger(-1) end;
      -- Done
      return;
    end
    -- Right button pressed?
    if IsKeyPressed(iKeyEquals) or IsButtonPressed(7) or IsScrollingUp() then
      -- Cycle to next item if digger inventory menu open?
      if aActiveMenu and aActiveMenu == aMenuData[MNU.DROP] then
        CycleObjInventory(aActiveObject, 1);
      -- Else select next digger
      else SelectAdjacentDigger(1) end;
      -- Done
      return;
    end
    -- Stop digger?
    if IsKeyPressed(iKeyBackspace) and SelectedDiggerIsMovable() then
      return SetAction(aActiveObject, ACT.STOP, JOB.NONE, DIR.NONE, true) end;
    -- Grab items?
    if IsKeyPressed(iKeySpacebar) and SelectedDiggerIsMovable() then
      return SetAction(aActiveObject, ACT.GRAB, JOB.KEEP, DIR.KEEP, true) end;
    -- Teleport?
    if IsKeyPressed(iKeyKp0) and SelectedDiggerIsMovable() then
      return SetAction(aActiveObject, ACT.PHASE, JOB.PHASE, DIR.U, true) end;
    -- Key to select a device?
    if IsKeyPressed(iKeyReturn) then return SelectDevice() end;
    -- Book? Load the book
    if IsKeyPressed(iKeyF5) then return SelectInventoryScreen() end;
    -- Inventory information? Set selected button and info screen
    if IsKeyPressed(iKeyF6) then return SelectLocationScreen() end;
    -- Location information? Set selected button and info screen
    if IsKeyPressed(iKeyF7) then return SelectStatusScreen() end;
    -- Status? Set selected button and info screen
    if IsKeyPressed(iKeyF8) then return SelectBook() end;
    -- Up key pressed?
    if IsKeyPressed(iKeyUp) and SelectedDiggerIsMovable() then
      -- If we're digging left then dig up-left instead
      if aActiveObject.A == ACT.DIG and aActiveObject.D == DIR.L then
        return SetAction(aActiveObject, ACT.DIG, JOB.DIG, DIR.UL, true) end;
      -- If we're digging right then dig up-right instead
      if aActiveObject.A == ACT.DIG and aActiveObject.D == DIR.R then
        return SetAction(aActiveObject, ACT.DIG, JOB.DIG, DIR.UR, true) end;
      -- Jump otherwise
      return SetAction(aActiveObject, ACT.JUMP, JOB.KEEP, DIR.KEEP, true);
    end
    -- Down key pressed?
    if IsKeyPressed(iKeyDown) and SelectedDiggerIsMovable() then
      -- If digging left then dig down- left
      if aActiveObject.A == ACT.DIG and aActiveObject.D == DIR.L then
        return SetAction(aActiveObject, ACT.DIG, JOB.DIG, DIR.DL, true) end;
      -- If digging right then dig down-right
      if aActiveObject.A == ACT.DIG and aActiveObject.D == DIR.R then
        return SetAction(aActiveObject, ACT.DIG, JOB.DIG, DIR.DR, true) end;
      -- Otherwise dig down
      return SetAction(aActiveObject, ACT.WALK, JOB.DIGDOWN, DIR.TCTR, true);
    end
    -- Left key pressed?
    if IsKeyPressed(iKeyLeft) and SelectedDiggerIsMovable() then
      -- If running left then set digger to dig left
      if aActiveObject.A == ACT.RUN and aActiveObject.D == DIR.L then
        return SetAction(aActiveObject, ACT.RUN, JOB.DIG, DIR.L, true) end;
      -- If walking left then set digger to run left
      if aActiveObject.A == ACT.WALK and aActiveObject.D == DIR.L then
        return SetAction(aActiveObject, ACT.RUN, JOB.KNDD, DIR.L, true) end;
      -- Otherwise walk left
      return SetAction(aActiveObject, ACT.WALK, JOB.KNDD, DIR.L, true);
    end
    -- Right key pressed?
    if IsKeyPressed(iKeyRight) and SelectedDiggerIsMovable() then
      -- If running right then set digger to dig right
      if aActiveObject.A == ACT.RUN and aActiveObject.D == DIR.R then
        return SetAction(aActiveObject, ACT.RUN, JOB.DIG, DIR.R, true) end;
      -- If walking right then set digger to dig right
      if aActiveObject.A == ACT.WALK and aActiveObject.D == DIR.R then
        return SetAction(aActiveObject, ACT.RUN, JOB.KNDD, DIR.R, true) end;
      -- Otherwise walk right
      return SetAction(aActiveObject, ACT.WALK, JOB.KNDD, DIR.R, true);
    end
    -- If pause key or button pressed?
    if IsKeyPressed(iKeyEscape) or IsButtonPressed(9) then
      -- Consts
      local iPauseX<const>, iPauseY<const> = 160, 72;
      local iInstructionY<const> = iPauseY + 24;
      local iSmallTipsY<const> = iInstructionY + 32;
      local sPaused<const> = "Game paused!";
      local sInstruction<const> =
        "Press ESCAPE or START to unpause.\n"..
        "\n"..
        "Press Q or START+JB4+JB5 to give up.";
      local sSmallTips<const> =
        "F1 OR SELECT+START BUTTON FOR SETUP\n"..
        "F2 FOR THE GAME AND ENGINE CREDITS\n"..
        "F11 TO RESET WINDOW SIZE AND POSITION\n"..
        "F12 TO TAKE A SCREENSHOT";
      local iKeyQ<const>, iKeyEscape<const> = aKeys.Q, aKeys.ESCAPE;
      -- Save current music
      local muMusic<const> = GetMusic();
      -- Save callbacks
      local fCBProc<const>, fCBRender<const>, fCBInput<const> = GetCallbacks();
      -- Stop music
      StopMusic(1);
      -- Pause string
      local nTimeNext, strPause = 0, nil;
      -- Pause logic callback
      local function ProcPause()
        -- Ignore if next update not elapsed
        if InfoTime() < nTimeNext then return end;
        -- Set new pause string
        strPause = UtilFormatNTime("%a/%H:%M:%S");
        -- Set next update time
        nTimeNext = InfoTime() + 0.25;
      end
      -- Pause render callback
      local function RenderPause()
        -- Render terrain, game objects, and a subtle fade
        RenderInterface();
        RenderFade(0.5);
        -- Write text informations
        DrawInfoFrameAndTitle("GAME PAUSED");
        fontLittle:SetCRGBA(0, 1, 0, 1);
        fontLittle:PrintC(iPauseX, iInstructionY, sInstruction);
        fontTiny:SetCRGBA(0.5, 0.5, 0.5, 1);
        fontTiny:PrintC(iPauseX, iSmallTipsY, sSmallTips);
        fontLittle:SetCRGBA(1, 1, 1, 1);
        -- Get and print local time
        SetBottomRightTip(strPause);
      end
      -- Pause input callback
      local function InputPause()
        -- If pause key or pause button pressed?
        if IsKeyPressed(iKeyEscape) or IsJoyPressed(9) then
          -- If thumb buttons pressed? Allow the quit
          if IsJoyPressed(4) and IsJoyPressed(5) then TriggerEnd(InitLose);
          -- Normal unpause
          else
            -- Resume music if we have it
            if muMusic then PlayMusic(muMusic, nil, 2) end;
            -- Unpause
            SetCallbacks(fCBProc, fCBRender, fCBInput);
          end
        -- Escape or select button pressed? Allow the quit
        elseif IsKeyPressed(iKeyQ) then TriggerEnd(InitLose) end;
      end
      -- Set pause procedure
      SetCallbacks(ProcPause, RenderPause, InputPause);
    end
    -- Return if in test mode
    if GetTestMode() then
      -- Check if Jennite key pressed
      if IsKeyPressed(iKeyT) then
        CreateObject(TYP.JENNITE, GetTileUnderMouse());
        return;
      end
      -- Check if shroud key pressed
      if IsKeyPressed(iKeyS) then
        UpdateShroud(GetAbsTileUnderMouse());
        return;
      end
      -- Check if explosion key pressed
      if IsKeyPressed(iKeyE) then
        AdjustObjectHealth(
          CreateObject(TYP.TNT, GetTileUnderMouse()), -100);
        return;
      end
      -- Check if key moving
      if IsKeyHeld(iKeyHome) then AdjustViewPortYNS(-16);
        if IsKeyHeld(iKeyDelete) then AdjustViewPortXNS(-16);
        elseif IsKeyHeld(iKeyPgDown) then AdjustViewPortXNS(16) end;
        return;
      end
      if IsKeyHeld(iKeyEnd) then AdjustViewPortYNS(16);
        if IsKeyHeld(iKeyDelete) then AdjustViewPortXNS(-16);
        elseif IsKeyHeld(iKeyPgDown) then AdjustViewPortXNS(16); end;
        return;
      end
      if IsKeyHeld(iKeyDelete) then AdjustViewPortXNS(-16);
        if IsKeyHeld(iKeyHome) then AdjustViewPortYNS(-16);
        elseif IsKeyHeld(iKeyEnd) then AdjustViewPortYNS(16) end;
        return;
      end
      if IsKeyHeld(iKeyPgDown) then AdjustViewPortXNS(16);
        if IsKeyHeld(iKeyHome) then AdjustViewPortYNS(-16);
        elseif IsKeyHeld(iKeyEnd) then AdjustViewPortYNS(16) end;
        return;
      end
    end
    -- Left mouse button clicked?
    if IsButtonPressed(0) then
      -- Menu is open and mouse is in its bounds? Process menu click
      if aActiveMenu and
        IsMouseInBounds(iMenuLeft, iMenuTop, iMenuRight, iMenuBottom) then
          return ProcessMenuClick() end;
      -- Utility buttons clicked?
      if IsMouseInBounds(232, 216, 312, 232) then
        -- Device button?
        if IsMouseXLessThan(248) then return SelectDevice() end;
        -- Inventory information? Set selected button and info screen
        if IsMouseXLessThan(264) then return SelectInventoryScreen() end;
        -- Location information? Set selected button and info screen
        if IsMouseXLessThan(280) then return SelectLocationScreen() end;
        -- Status? Set selected button and info screen
        if IsMouseXLessThan(296) then return SelectStatusScreen() end;
        -- Set book proc and return to here when done
        return SelectBook();
      end
      -- Digger button clicked? Process digger button click
      if IsMouseInBounds(144, 216, 224, 232) then
        return ProcessDiggerButtonClick() end;
      -- Check for mouse clicking on object instead
      return FindObjectUnderCursor();
    end
    -- Right mouse button button or Joystick button 1 is held?
    if IsButtonHeld(1) then
    -- Right mouse button held down and menu open?
      if aActiveMenu then UpdateMenuPositionAtMouseCursor();
      -- Is the right mouse button pressed? (Don't release the click). Open the
      -- object menu and cancel the mouse click if failed
      elseif aActiveObject and IsButtonPressedNoRelease(1) and
        not OpenObjectMenu() then IsButtonPressed(1) end
      -- Done
      return;
    end
  end
  -- Set new proc input and execute it
  ProcInput = ProcInputInitialised;
end
ProcInput();
-- Load level -------------------------------------------------------------- --
local function LoadLevel(Id, Music, P1R, P1AI, P2R, P2AI, TP, TR, TI)
  -- Set default callbacks if not set
  if not TP then TP = GameProc end;
  if not TR then TR = RenderInterface end;
  if not TI then TI = ProcInput end;
  -- Set default players if not set
  if not P1R then P1R = aGlobalData.gSelectedRace or TYP.DIGRANDOM end;
  if not P1AI then P1AI = false end;
  if not P2R then P2R = TYP.DIGRANDOM end;
  if not P2AI then P2AI = true end;
  -- De-init/Reset current level
  DeInitLevel();
  -- Set FBU callback
  RegisterFBUCallback("game", FrameBufferUpdated);
  -- Set level number and get data for it.
  local aLevelInfo;
  if UtilIsTable(Id) then iLevelId, aLevelInfo = 1, Id;
  elseif UtilIsInteger(Id) then iLevelId, aLevelInfo = Id, aLevelsData[Id];
  else error("Invalid id '"..tostring(Id).."' of type '"..type(Id).."'!") end;
  if not UtilIsTable(aLevelInfo) then
    error("Invalid level data! "..tostring(aLevelInfo)) end;
  -- Get level type data and level type
  local aLevelTypeData<const> = aLevelInfo.t;
  local iLevelType<const> = aLevelTypeData.i;
  -- Set level name and level type name
  sLevelName, sLevelType = aLevelInfo.n, aLevelTypeData.n;
  -- Set shroud colour
  aShroudColour = aLevelTypeData.s;
  -- Holds required assets to set template to music or no music
  local aAssets;
  if Music then aAssets, aAssetsMusic[4].F = aAssetsMusic, Music;
  else aAssets = aAssetsNoMusic end;
  -- Update asset filenames to load
  local sLevelFile<const> = aLevelInfo.f;
  aAssets[1].F = "lvl/"..sLevelFile..".dat";
  aAssets[2].F = "lvl/"..sLevelFile..".dao";
  aAssets[3].F = aLevelTypeData.f;
  -- Level assets loaded function
  local function OnLoaded(aResources)
    -- Set level data handles
    local binLevel<const> = aResources[1].H;
    -- Set level objects handle
    local aLevelObj<const> = aResources[2].H;
    -- Set textures and masks
    texLev = aResources[3].H;
    -- Makes sure we have the same number of terrain masks as texture tiles
    local iMaskLev<const>, iMaskLevExpect<const> =
      maskLev:Count(), texLev:TileGTC();
    if iMaskLev < iMaskLevExpect then
      error("Got only "..iMaskLev.." of "..iMaskLevExpect.." level masks!");
    end
    -- Each level supports 480 tiles right now (512 on texture)
    texLev:TileSTC(480);
    -- Grab the background part
    texBg = texLev:TileA(0, 256, 512, 512);
    -- Makes sure we have the same number of sprite masks as sprite tiles
    local iMaskSpr<const>, iMaskSprExpect<const> =
      maskSpr:Count(), texSpr:TileGTC();
    if iMaskSpr ~= iMaskSprExpect then
      error("Got only "..iMaskSpr.." of "..iMaskSprExpect.." sprite masks!");
    end
    -- Player starting positions
    local aPlayerStartData<const> = {
      { 195, 198, P1R, P1AI },   -- Player 1 start data
      { 199, 202, P2R, P2AI }    -- Player 2 start data
    };
    -- Create a blank mask
    maskZone = MaskCreateZero(sLevelFile, iLLPixW,
      iLLPixH);
    -- Get minimum and maximum object id
    local iMinObjId<const>, iMaxObjId<const> = TYP.JENNITE, TYP.MAX;
    -- Player starting point data found
    local aPlayersFound<const> = { };
    -- For each row in the data file
    for iY = 0, iLLAbsHm1 do
      -- Calculate precise Y position for object
      local iPreciseY<const> = iY * 16;
      -- Calculate row position in data file
      local iDataY<const> = iY * iLLAbsW;
      -- For each column in the data file
      for iX = 0, iLLAbsWm1 do
        -- Calculate position of tile
        local iPosition<const> = (iDataY + iX) * 2;
        -- Calculate precise X position for object
        local iPreciseX<const> = iX * 16;
        -- Get the 16-bit (little-endian) tile id from the terrain data at
        -- the current position and if it is not a clear tile?
        local iTerrainId<const> = binLevel:RU16LE(iPosition);
        if iTerrainId >= 0 and iTerrainId < #aTileData then
          -- Check if is a player
          for iPlayer = 1, #aPlayerStartData do
            -- Get player data and check for player starting position
            local aPlayerStartItem<const> = aPlayerStartData[iPlayer];
            if iTerrainId >= aPlayerStartItem[1] and
               iTerrainId <= aPlayerStartItem[2] then
              -- Get existing player data and if player exists?
              local aPlayerFound<const> = aPlayersFound[iPlayer];
              if aPlayerFound then
                -- Show map maker in console that the player exists
                CoreWrite("Warning! Player "..iPlayer..
                  " already exists! X="..iX..", Y="..iY..", Abs="..
                  iPosition..". Originally found at X="..aPlayerFound[1]..
                  ", Y="..aPlayerFound[2]..".", 9);
              -- Player doesn't exist? Set the new player
              else aPlayersFound[iPlayer] =
                { iX, iY, aPlayerStartItem[3], aPlayerStartItem[4] }
              end
            end
          end
          -- Draw the appropriate tile for the level bit mask
          maskZone:Copy(maskLev, iTerrainId, iPreciseX, iPreciseY);
          -- Insert into level data array
          aLevelData[1 + #aLevelData] = iTerrainId;
          -- Create entry for shroud data (sprite tile set number)
          aShroudData[1 + #aShroudData] = { 1022, 0x0 };
        -- Show error. Level could be corrupted
        else error("Error! Invalid tile "..iTerrainId.."/"..#aTileData..
          " at X="..iX..", Y="..iY..", Abs="..iPosition.."!") end;
        -- We need to check for objects at the same position now too. We
        -- can store two items on one 16-bit tile...
        for iPos = iPosition, iPosition + 1 do
          -- Get object id at position and if it's interesting?
          local iObjId<const> = aLevelObj:RU8(iPos);
          if iObjId ~= 0 then
            if iObjId < iMinObjId or iObjId >= iMaxObjId then
              -- Show map maker in console that the object id is invalid
              CoreWrite("Warning! Object id invalid! Id="..
                iObjId..", X="..iX..", Y="..iY..", Abs="..
                iPosition..", Slot="..iPos..", Max="..iMaxObjId..".", 9);
            -- Object id is valid? Create the object
            elseif not CreateObject(iObjId, iPreciseX, iPreciseY) then
              -- Show map maker in console that the object id is invalid
              CoreWrite("Warning! Couldn't create object! Id="..
                iObjId..", X="..iX..", Y="..iY..", Abs="..
                iPosition..", Slot="..iPos..", Max="..iMaxObjId..".", 9);
            end
          end
        end
      end
    end
    -- Make sure we got the correct amount of level tiles
    if #aLevelData < iLLAbs then
      error("Only "..#aLevelData.."/"..iLLAbs.." level tiles!") end;
    -- Make sure we found two players
    if #aPlayersFound ~= 2 then
      error("Only "..#aPlayersFound.."/2 players!") end;
    -- Fill border with 1's to prevent objects leaving the playfield
    maskZone:Fill(0, 0, iLLPixW, 1);
    maskZone:Fill(0, 0, 1, iLLPixH);
    maskZone:Fill(iLLPixWm1, 0, 1, iLLPixH);
    maskZone:Fill(0, iLLPixHm1, iLLPixW, 1);
    -- Reset races available
    for iI = 1, #aRacesData do
      aRacesAvailable[1 + #aRacesAvailable] = aRacesData[iI] end;
    -- Create player for each starting position found
    for iPlayerId = 1, #aPlayersFound do
      local aPlayerData<const> = aPlayersFound[iPlayerId];
      CreatePlayer(aPlayerData[1], aPlayerData[2], iPlayerId,
                   aPlayerData[3], aPlayerData[4]);
    end
    -- Set focus on the third Digger on the active player and defocus it
    SelectObject(aActivePlayer.D[3]);
    ForceViewport();
    SelectObject();
    -- Set player race and level if not set (gam_test used)
    if not aGlobalData.gSelectedRace then
      aGlobalData.gSelectedRace = aActivePlayer.R end;
    if not aGlobalData.gSelectedLevel then
      aGlobalData.gSelectedLevel = Id end;
    -- Reset gems available
    local iGemStart<const> = random(#aDigTileData);
    for iId = 1, #aDigTileData do aGemsAvailable[1 + #aGemsAvailable] =
      aDigTileData[1 + ((iGemStart + iId) % #aDigTileData)] end;
    -- Play in-game music if requested
    if Music then PlayMusic(aResources[4].H, 0) end;
    -- Now we want to hear sounds if human player is set
    if P1AI == false then SetPlaySounds(true) end;
    -- Computer is main player?
    if P1AI == true then
      -- Set auto-respawn on all objects death
      for iI = 1, #aObjects do
        local aObject<const> = aObjects[iI];
        aObject.F = aObject.F | OFL.RESPAWN;
      end
    -- Set actual win limit
    end
    -- Set win limit (Credits will not set 'w' for no limit)
    iWinLimit = (aLevelInfo.w or maxinteger) + aGlobalData.gCapitalCarried;
    -- Do one tick at least or the fade will try to render with variables
    -- that haven't been initialised yet
    TP();
    -- Do fade then set requested game callbacks
    local function OnFadeIn() SetCallbacks(TP, TR, TI) end;
    Fade(1, 0, 0.04, TR, OnFadeIn, not not Music);
  end
  -- Load level graphics resources asynchronously
  LoadResources(sLevelName, aAssets, OnLoaded);
end
-- Return value of all capital --------------------------------------------- --
local function GetCapitalValue()
  -- Get type of item we want to check for
  local iDeviceId<const> = OFL.DEVICE;
  -- Capital value
  local iCapitalValue = 0;
  -- Check object and sell it if it's a device
  local function SellDevice(aObj, nDivisor)
    -- Failed if not a device
    if aObj.F & iDeviceId == 0 then return end;
    -- Add capital value (25% value minus current quality)
    iCapitalValue = iCapitalValue +
      (aObj.OD.VALUE / nDivisor) * (aObj.H / 100);
  end
  -- Enumerate all objects
  for iObjIndex = 1, #aObjects do
    -- Get object and if it is owned by the active player?
    local aObj<const> = aObjects[iObjIndex];
    if aActivePlayer == aObj.P then
      -- If object is a device? Add 25% of it's value minus quality because
      -- it cost's money to recover the device. :-)
      SellDevice(aObj, 4);
      -- Get object's inventory and sell it all 50% of it's value since the
      -- object or digger is already carrying the item.
      local aObjInv<const> = aObj.I;
      for iInvIndex = 1, #aObjInv do SellDevice(aObjInv[iInvIndex], 2) end;
    end
  end
  -- Return capital value rounded down
  return floor(iCapitalValue);
end
-- Continue game from book or lobby ---------------------------------------- --
local function InitContinueGame(bMusic, aObject)
  -- Post loaded
  local function PostLoaded()
    -- Set arrow
    SetCursor(aCursorIdData.ARROW);
    -- We want to hear sounds
    SetPlaySounds(true);
    -- Return control to main loop
    SetCallbacks(GameProc, RenderInterface, ProcInput);
    -- If an object was passed? Make it appear
    if aObject then SetAction(aObject, ACT.PHASE, JOB.NONE, DIR.NONE) end;
    -- Check end game conditions as might have won!
    EndConditionsCheck();
  end
  -- Music loaded
  local function OnLoaded(aResource)
    -- Resume game music
    PlayMusic(aResource[1].H, nil, 2);
    -- Post loaded functions
    PostLoaded();
  end
  -- If music reset required then load it
  if bMusic then return LoadResources("Continue", aContAssets, OnLoaded) end;
  -- Run post loaded functions
  PostLoaded();
end
-- ------------------------------------------------------------------------- --
local function GetActiveObject() return aActiveObject end;
-- ------------------------------------------------------------------------- --
local function GetActivePlayer() return aActivePlayer end;
-- ------------------------------------------------------------------------- --
local function GetOpponentPlayer() return aOpponentPlayer end;
-- ------------------------------------------------------------------------- --
local function GetLevelInfo()
  return iLevelId, sLevelName, sLevelType, iWinLimit;
end
-- ------------------------------------------------------------------------- --
local function GetViewportData()
  return iPixPosX, iPixPosY, iPixPosTargetX, iPixPosTargetY, iPixCenPosX,
         iPixCenPosY, iPosX, iPosY, iAbsCenPosX, iAbsCenPosY, iViewportW,
         iViewportH;
end
-- Exports and imports ----------------------------------------------------- --
return { A = {                         -- Exports
  -- Exports --------------------------------------------------------------- --
  AdjustObjectHealth = AdjustObjectHealth, AdjustViewPortX = AdjustViewPortX,
  AdjustViewPortY = AdjustViewPortY, BuyItem = BuyItem,
  CreateObject = CreateObject, DeInitLevel = DeInitLevel,
  EndConditionsCheck = EndConditionsCheck, GameProc = GameProc,
  GetAbsMousePos = GetAbsMousePos, GetActiveObject = GetActiveObject,
  GetActivePlayer = GetActivePlayer, GetCapitalValue = GetCapitalValue,
  GetGameTicks = GetGameTicks, GetLevelInfo = GetLevelInfo,
  GetOpponentPlayer = GetOpponentPlayer, GetViewportData = GetViewportData,
  HaveZogsToWin = HaveZogsToWin, InitContinueGame = InitContinueGame,
  IsSpriteCollide = IsSpriteCollide, LoadLevel = LoadLevel,
  LockViewPort = LockViewPort, ProcInput = ProcInput,
  ProcessViewPort = ProcessViewPort, RenderInterface = RenderInterface,
  RenderObjects = RenderObjects, RenderShroud = RenderShroud,
  RenderTerrain = RenderTerrain, SelectObject = SelectObject,
  SellSpecifiedItems = SellSpecifiedItems, SetPlaySounds = SetPlaySounds,
  UpdateShroud = UpdateShroud, aGemsAvailable = aGemsAvailable,
  aLevelData = aLevelData, aObjects = aObjects, aPlayers = aPlayers
  -- ----------------------------------------------------------------------- --
  }, F = function(GetAPI)              -- Imports
  -- Imports --------------------------------------------------------------- --
  TYP, aLevelsData, LoadResources, aObjectData, ACT, JOB, DIR, aTimerData, AI,
  OFL, aDigTileData, PlayMusic, aTileData, aTileFlags, Fade, BCBlit,
  SetCallbacks, IsMouseInBounds, aDigData, DF, aSfxData, aJumpRiseData,
  aJumpFallData, IsKeyPressed, IsButtonPressed, IsKeyHeld, IsButtonHeld,
  GetMouseX, GetMouseY, PlayStaticSound, PlaySound, IsButtonPressedNoRelease,
  aMenuData, MFL, MNU, IsMouseXLessThan, InitBook, aObjToUIData, GetMusic,
  StopMusic, RenderFade, IsJoyPressed, InitWin, InitWinDead, InitLose,
  InitLoseDead, InitTNTMap, InitLobby, texSpr, fontLarge, fontLittle, fontTiny,
  aDigBlockData, aExplodeDirData, SetCursor, aCursorIdData,
  RegisterFBUCallback, GetCallbacks, GetTestMode, RenderShadow,
  SetBottomRightTip, aRacesData, aDugRandShaftData, aFloodGateData,
  aTrainTrackData, aExplodeAboveData, maskLev, maskSpr, aGlobalData, aShopData,
  IsScrollingDown, IsScrollingUp, aAIChoicesData, aShroudCircle,
  aShroudTileLookup
  = -- --------------------------------------------------------------------- --
  GetAPI("aObjectTypes", "aLevelsData", "LoadResources", "aObjectData",
    "aObjectActions", "aObjectJobs", "aObjectDirections", "aTimerData",
    "aAITypesData", "aObjectFlags", "aDigTileData", "PlayMusic", "aTileData",
    "aTileFlags", "Fade", "BCBlit", "SetCallbacks", "IsMouseInBounds",
    "aDigData", "aDigTileFlags", "aSfxData", "aJumpRiseData", "aJumpFallData",
    "IsKeyPressed", "IsButtonPressed", "IsKeyHeld", "IsButtonHeld",
    "GetMouseX", "GetMouseY", "PlayStaticSound", "PlaySound",
    "IsButtonPressedNoRelease", "aMenuData", "aMenuFlags", "aMenuIds",
    "IsMouseXLessThan", "InitBook", "aObjToUIData", "GetMusic", "StopMusic",
    "RenderFade", "IsJoyPressed", "InitWin", "InitWinDead", "InitLose",
    "InitLoseDead", "InitTNTMap", "InitLobby", "texSpr", "fontLarge",
    "fontLittle", "fontTiny", "aDigBlockData", "aExplodeDirData", "SetCursor",
    "aCursorIdData", "RegisterFBUCallback", "GetCallbacks", "GetTestMode",
    "RenderShadow", "SetBottomRightTip", "aRacesData", "aDugRandShaftData",
    "aFloodGateData", "aTrainTrackData", "aExplodeAboveData", "maskLevel",
    "maskSprites", "aGlobalData", "aShopData", "IsScrollingDown",
    "IsScrollingUp", "aAIChoicesData", "aShroudCircle", "aShroudTileLookup");
  -- Pre-initialisations --------------------------------------------------- --
  CreateObject = InitCreateObject();
  MoveOtherObjects = InitMoveOtherObjects();
  SetAction = InitSetAction();
  ProcessObjectMovement = ProcessObjectMovement();
  PhaseLogic();
  -- ----------------------------------------------------------------------- --
end };
-- ------------------------------------------------------------------------- --
end                                    -- End of 'InternalsScope' namespace
-- ------------------------------------------------------------------------- --
return HighPriorityVars();             -- Return high priority variables
-- ------------------------------------------------------------------------- --
end                                    -- End of 'ExternalsScope' namespace
-- ------------------------------------------------------------------------- --
return MediumPriorityVars();           -- Return module parameters
-- End-of-File ============================================================= --
