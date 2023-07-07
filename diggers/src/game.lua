-- GAME.LUA ================================================================ --
-- ooooooo.--ooooooo--.ooooo.-----.ooooo.--oooooooooo-oooooooo.----.ooooo..o --
-- 888'-`Y8b--`888'--d8P'-`Y8b---d8P'-`Y8b-`888'---`8-`888--`Y88.-d8P'---`Y8 --
-- 888----888--888--888---------888---------888--------888--.d88'-Y88bo.---- --
-- 888----888--888--888---------888---------888oo8-----888oo88P'---`"Y888o.- --
-- 888----888--888--888----oOOo-888----oOOo-888--"-----888`8b.--------`"Y88b --
-- 888---d88'--888--`88.---.88'-`88.---.88'-888-----o--888-`88b.--oo----.d8P --
-- 888bd8P'--oo888oo-`Y8bod8P'---`Y8bod8P'-o888ooood8-o888o-o888o-8""8888P'- --
-- ========================================================================= --
-- Copyr. (c) MS-Design, 2023   Copyr. (c) Millennium Interactive Ltd., 1994 --
-- ========================================================================= --
-- Core function aliases --------------------------------------------------- --
local format<const>, insert<const>, remove<const>, random<const>, ceil<const>,
  floor<const>, min<const>, max<const>, assert<const>, error<const>,
  abs<const>, maxinteger<const>
  = -- --------------------------------------------------------------------- --
  string.format, table.insert, table.remove, math.random, math.ceil,
  math.floor, math.min, math.max, assert, error, math.abs, math.maxinteger;
-- M-Engine function aliases ----------------------------------------------- --
local UtilClampInt<const>, UtilClamp<const>, UtilBlank<const>, CoreLog<const>,
  ConsoleWrite<const>, InfoTicks<const>, UtilFormatNTime<const>,
  InfoTime<const>, MaskCreateNew<const>, CoreQuit<const>
  = -- --------------------------------------------------------------------- --
  Util.ClampInt, Util.Clamp, Util.Blank, Core.Log, Console.WriteEx, Info.Ticks,
  Util.FormatNTime, Info.Time, Mask.CreateNew, Core.Quit;
-- Game module namespace (because of MAXVARS limit) ------------------------ --
local function GameLuaModule()
-- Diggers shared functions and data --------------------------------------- --
local TYP, aLevelData, LoadResources, aObjectData, ACT, JOB, iPosX, iPosY, DIR,
  aTimerData, AI, OFL, aDigTileData, PlayMusic, aTileData, aTileFlags,
  Fade, BCBlit, SetCallbacks, IsMouseInBounds, aDigData, DF, aSfxData,
  aJumpRiseData, aJumpFallData, IsKeyPressed, IsButtonPressed, IsKeyHeld,
  IsButtonHeld, GetMouseX, GetMouseY, PlayStaticSound, PlaySound,
  IsButtonPressedNoRelease, aMenuData, MFL, MNU, IsMouseXLessThan, InitBook,
  aObjToUIData, GetMusic, StopMusic, RenderFade, IsJoyPressed, InitWin,
  InitWinDead, InitLose, InitLoseDead, InitTNTMap, InitLobby, texSpr,
  fontLarge, fontLittle, fontTiny, aDigBlockData, aExplodeDirData, SetCursor,
  aCursorIdData, RegisterFBUCallback, GetCallbacks, GetTestMode, RenderShadow,
  SetBottomRightTip, aDugRandShaftData, aFloodGateData, aTrainTrackData,
  aExplodeAboveData, aGlobalData, aShopData;
-- Prototype functions (assigned later) ------------------------------------ --
local CreateObject, MoveOtherObjects, PlaySoundAtObject, SetAction;
-- Locals ------------------------------------------------------------------ --
local aGemsAvailable, aObjects, aPlayers, aRacesAvailable, aFloodData, texBg,
  texLev, maskLev, maskZone, maskSpr, binLevel, iMenuLeft, iMenuTop,
  iMenuRight, iMenuBottom, iVPWidth, iVPHeight, PosAX, PosAY, PosPX, PosPY,
  PosPXC, PosPYC, iTilesHeight, iTilesWidth, PosCX, PosCY, Money, sMoney,
  fcbPause, TIP, aActiveMenu, aRaceData, iGameTicks, iWinLimit, aActivePlayer,
  aOpponentPlayer, aActiveObject, iLevelId, sLevelName, sLevelType,
  iInfoScreen, iStageW, iStageH, iStageL, iStageT, iStageR, iStageB,
  TotalTilesWidth, TotalTilesHeight, TotalTilesWidthD2, TotalTilesHeightD2,
  TotalTilesWidthM1, TotalTilesHeightM1, TotalLevelWidthMVPS,
  TotalTilesWidthMVPS, TotalTilesHeightMVPS, TotalLevelHeightMVPS =
    { }, { }, { }, { }, { }, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil,
    nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil,
    nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil,
    nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil;
-- Consts ------------------------------------------------------------------ --
local TotalLevelWidth<const>, TotalLevelHeight<const> = 128, 128;
local TotalLevelWidthM1<const> = TotalLevelWidth - 1;
local TotalLevelHeightM1<const> = TotalLevelHeight - 1;
local ViewPortGranuleWidth<const>, ViewPortGranuleHeight<const> = 4, 3;
local TotalLevelWidthM4<const> = TotalLevelWidth - ViewPortGranuleWidth;
local TotalLevelHeightM4<const> = TotalLevelHeight - ViewPortGranuleHeight;
local TotalLevelWidthX2<const> = TotalLevelWidth * 2;
local TotalLevelWidthX2M1<const> = TotalLevelWidthX2 - 2;
local TotalLevelSize<const> = TotalLevelWidth * TotalLevelHeight * 2;
local TotalLevelSizeMLW<const> = TotalLevelSize - TotalLevelWidthX2;
local TotalLevelSizeM1<const> = TotalLevelSize - 2;
local TotalLevelSize<const> = TotalLevelWidth * TotalLevelHeight * 2;
local TotalLevelWidthPixel<const> = TotalLevelWidth * 16;
local TotalLevelWidthPixelM1<const> = TotalLevelWidthPixel - 1;
local TotalLevelHeightPixel<const> = TotalLevelHeight * 16;
local TotalLevelHeightPixelM1<const> = TotalLevelHeightPixel - 1;
-- Function to play a sound ------------------------------------------------ --
local function DoPlaySoundAtObject(aObject, iSfxId, nPitch)
  -- Check that object is in the players view
  local nX<const> = (aObject.X / 16) - iPosX;
  if nX < 0 or nX >= TotalTilesWidth then return end;
  local nY<const> = (aObject.Y / 16) - iPosY;
  if nY < 0 or nY >= TotalTilesHeight then return end;
  -- Play the sound
  PlaySound(iSfxId, -1+((nX/TotalTilesWidth)*2), nPitch);
end
-- Dump mask to disk ------------------------------------------------------- --
function DumpLevelMask(strFile) maskZone:Save(0, strFile or "mask.tga") end;
-- Update viewport data ---------------------------------------------------- --
local function UpdateViewPort(nPos, iTLMVPS, iTTD2, iTT, iTL)
  -- Obey limits of level
  if nPos > iTLMVPS then nPos = iTLMVPS elseif nPos < 0 then nPos = 0 end;
  -- Reuse limit and calculate centre tile
  iTLMVPS = floor(nPos / 16);
  -- Return calculated results
  return nPos,                         -- PosPX | PosPY (Abs position)
         -(nPos % 16),                 -- PosPXC | PosPYC (Pixel centre)
         iTLMVPS,                      -- iPosX | iPosY (Tile position)
         iTLMVPS + iTTD2,              -- PosCX | PosCY (Centre tile position)
         min(iTLMVPS + iTT, iTL);      -- iVPWidth | iVPHeight (Scroll pos)
end
-- Update horizontal viewport data ----------------------------------------- --
local function SetViewPortX(nX)
  PosPX, PosPXC, iPosX, PosCX, iVPWidth =
    UpdateViewPort(nX, TotalLevelWidthMVPS, TotalTilesWidthD2,
      TotalTilesWidth, TotalLevelWidth);
  if PosPXC < 0 then iTilesWidth = TotalTilesWidth;
                else iTilesWidth = TotalTilesWidthM1 end;
end
-- Update vertical viewport data ------------------------------------------- --
local function SetViewPortY(nY)
  PosPY, PosPYC, iPosY, PosCY, iVPHeight =
    UpdateViewPort(nY, TotalLevelHeightMVPS, TotalTilesHeightD2,
      TotalTilesHeight, TotalLevelWidthM1);
  if PosPYC < 0 then iTilesHeight = TotalTilesHeight;
                else iTilesHeight = TotalTilesHeightM1 end;
end
-- Adjust viewport --------------------------------------------------------- --
local function AdjustViewPortX(iX) SetViewPortX(PosPX + iX) end;
local function AdjustViewPortY(iY) SetViewPortY(PosPY + iY) end;
-- The framebuffer was updated --------------------------------------------- --
local function FrameBufferUpdated(...)
  -- Set new stage bounds
  iStageW, iStageH, iStageL, iStageT, iStageR, iStageB = ...;
  -- Set new limits based on frame buffer size
  TotalTilesWidth, TotalTilesHeight = ceil(iStageW/16), ceil(iStageH/16);
  -- Used in game so calculate them now to prevent unneccesary math
  TotalTilesWidthD2 = floor(TotalTilesWidth/2);
  TotalTilesHeightD2 = floor(TotalTilesHeight/2);
  TotalTilesWidthM1 = TotalTilesWidth-1;
  TotalTilesHeightM1 = TotalTilesHeight-1;
  -- Level width minus viewport size
  TotalLevelWidthMVPS = (TotalLevelWidth-TotalTilesWidth)*16;
  TotalLevelHeightMVPS = (TotalLevelHeight-TotalTilesHeight)*16;
  -- Update viewport limits
  AdjustViewPortX(0);
  AdjustViewPortY(0);
end
-- Update new viewport ----------------------------------------------------- --
local function SetViewPort(iX, iY) SetViewPortX(iX) SetViewPortY(iY) end
-- Set instant focus on object horizonally --------------------------------- --
local function ScrollViewPortTo(iX, iY)
  PosAX, PosAY = UtilClamp(iX, -1, TotalLevelWidthM1),
                 UtilClamp(iY, -1, TotalLevelHeightM1);
end
-- Focus on object --------------------------------------------------------- --
local function ObjectFocus(aObj)
  -- This object is not selected? Ignore it
  if aActiveObject ~= aObj then return end;
  -- Get object absolute position
  local iX<const>, iY<const> = aObj.AX, aObj.AY;
  -- Object is horizonally out of viewport?
  if abs(iX-PosCX) > TotalTilesWidthD2 or
     abs(iY-PosCY) > TotalTilesHeightD2 then
    -- Set instant focus on object horizonally
    ScrollViewPortTo(iX-TotalTilesWidthD2, iY-TotalTilesHeightD2);
    SetViewPort(PosAX*16, PosAY*16);
  -- Centre on the object gradually if close by
  elseif ((iX >= ViewPortGranuleWidth and iX < TotalLevelWidthM4) and
       (iX-ViewPortGranuleWidth < iPosX or
        iX+ViewPortGranuleWidth >= iVPWidth)) or
     ((iY >= ViewPortGranuleHeight and iY < TotalLevelHeightM4) and
       (iY-ViewPortGranuleHeight < iPosY or
        iY+ViewPortGranuleHeight >= iVPHeight)) then
    -- Gradually scroll to this position
    ScrollViewPortTo(iX-TotalTilesWidthD2, iY-TotalTilesHeightD2);
  end
end
-- SelectObject ------------------------------------------------------------ --
local function SelectObject(aObj)
  -- Save active object
  local aObjActive<const> = aActiveObject;
  -- Set active object
  aActiveObject = aObj;
  -- Remove menu if different object
  if aActiveObject ~= aObjActive then aActiveMenu = nil end;
  -- Focus on object
  if aObj then ObjectFocus(aObj) end;
end
-- Return game ticks ------------------------------------------------------- --
local function GetGameTicks() return iGameTicks end;
-- Deinit the level -------------------------------------------------------- --
local function DeInitLevel()
  -- Unset FBU callback
  RegisterFBUCallback("game");
  -- Unreference loaded assets for garbage collector
  binLevel, texBg, texLev, maskZone = nil, nil, nil, nil;
  -- Clear current objects, players, flood, races and gems data
  for sKey in pairs(aObjects) do aObjects[sKey] = nil end;
  for sKey in pairs(aPlayers) do aPlayers[sKey] = nil end;
  for sKey in pairs(aFloodData) do aFloodData[sKey] = nil end;
  for sKey in pairs(aRacesAvailable) do aRacesAvailable[sKey] = nil end;
  for sKey in pairs(aGemsAvailable) do aGemsAvailable[sKey] = nil end;
  -- Reset positions and other variables
  PosAX, PosAY, PosPX, PosPY, iGameTicks, Money,
    iLevelId, iWinLimit, sMoney = 0, 0, 0, 0, 0, 0, nil, nil, nil;
  -- Reset active objects, menus and players
  aActiveObject, aActivePlayer, aActiveMenu, aOpponentPlayer =
    nil, nil, nil, nil;
  -- We don't want to hear sounds
  PlaySoundAtObject = UtilBlank;
end
-- Get level tile location from absolute ca-ordinates ---------------------- --
local function GetTileOffsetFromAbsCoordinates(iX, iY)
  -- Check parameters
  assert(type(iX)=="number", "Invalid X co-ordinate!");
  assert(type(iY)=="number", "Invalid Y co-ordinate!");
  -- Return tile offset if valid
  if iX >= 0 and iX < TotalLevelWidth and
     iY >= 0 and iY < TotalLevelHeight then
    return iY * TotalLevelWidth + iX end;
end
-- Get level tile location from absolute ca-ordinates ---------------------- --
local function GetLevelOffsetFromAbsCoordinates(iX, iY)
  -- Check parameters
  assert(type(iX)=="number", "Invalid X co-ordinate!");
  assert(type(iY)=="number", "Invalid Y co-ordinate!");
  -- Return location if valid
  local iLoc<const> = GetTileOffsetFromAbsCoordinates(iX, iY);
  if iLoc then return iLoc * 2 end;
end
-- Get zero based tile id at specified absolute location ------------------- --
local function GetLevelDataFromAbsCoordinates(iX, iY)
  -- Check parameters
  assert(type(iX)=="number", "Invalid X co-ordinate!");
  assert(type(iY)=="number", "Invalid Y co-ordinate!");
  -- Get tile at specified location
  local iLoc<const> = GetLevelOffsetFromAbsCoordinates(iX, iY);
  if not iLoc then return end;
  -- Return tile at specified location and location
  return binLevel:RU16LE(iLoc), iLoc;
end
-- Get zero based tile id at specified absolute location ------------------- --
local function GetLevelDataFromLevelOffset(iLoc)
  -- Check parameters
  assert(type(iLoc)=="number", "Invalid location!");
  -- Get tile at specified location
  if iLoc >= 0 and
     iLoc < TotalLevelSize and
     iLoc % 2 == 0 then
    return binLevel:RU16LE(iLoc) end
end
-- Get zero based tile id at specified absolute location ------------------- --
local function GetLevelDataFromTileOffset(iLoc)
  -- Check parameters
  assert(type(iLoc)=="number", "Invalid location!");
  -- Get tile at specified location
  return GetLevelDataFromLevelOffset(iLoc * 2);
end
-- Get tile location from pixel co-ordinates ------------------------------- --
local function GetTileOffsetFromCoordinates(nX, nY)
  -- Check parameters
  assert(type(nX)=="number", "Invalid X co-ordinate!");
  assert(type(nY)=="number", "Invalid Y co-ordinate!");
  -- Return location if valid
  if nX >= 0 and nX < TotalLevelWidthPixel and
     nY >= 0 and nY < TotalLevelHeightPixel then
    return (floor(nY / 16) * TotalLevelWidth + floor(nX / 16)) end;
end
-- Get level tile location from co-ordinates ------------------------------- --
local function GetLevelOffsetFromCoordinates(nX, nY)
  -- Check parameters
  assert(type(nX)=="number", "Invalid X co-ordinate!");
  assert(type(nY)=="number", "Invalid Y co-ordinate!");
  -- Return level offset if valid
  local iLoc<const> = GetTileOffsetFromCoordinates(nX, nY);
  if iLoc then return iLoc * 2 end;
end
-- Get zero based tile id at specified location ---------------------------- --
local function GetLevelDataFromCoordinates(nX, nY)
  -- Check parameters
  assert(type(nX)=="number", "Invalid X co-ordinate!");
  assert(type(nY)=="number", "Invalid Y co-ordinate!");
  -- Get tile at specified location
  local iLoc<const> = GetLevelOffsetFromCoordinates(nX, nY);
  if not iLoc then return end;
  -- Return tile at specified location and location
  return binLevel:RU16LE(iLoc), iLoc;
end
-- Get zero based tile id at specified object ------------------------------ --
local function GetLevelDataFromObject(aObject, nX, nY)
  -- Check parameters
  assert(type(aObject)=="table", "Invalid object specified!");
  assert(type(nX)=="number", "Invalid X co-ordinate!");
  assert(type(nY)=="number", "Invalid Y co-ordinate!");
  -- Return level data from object co-ordinates
  return GetLevelDataFromCoordinates(aObject.X + aObject.OFX + nX,
                                     aObject.Y + aObject.OFY + nY);
end
-- Get tile at specified object offset pixels ------------------------------ --
local function GetTileOffsetFromObject(aObject, nX, nY)
  -- Check parameters
  assert(type(aObject)=="table", "Invalid object specified!");
  assert(type(nX)=="number", "Invalid X co-ordinate!");
  assert(type(nY)=="number", "Invalid Y co-ordinate!");
  -- Return tile offset from object co-ordinates
  return GetTileOffsetFromCoordinates(aObject.X + aObject.OFX + nX,
                                      aObject.Y + aObject.OFY + nY);
end
-- Get tile at specified object offset pixels ------------------------------ --
local function GetLevelOffsetFromObject(aObject, nX, nY)
  -- Check parameters
  assert(type(aObject)=="table", "Invalid object specified!");
  assert(type(nX)=="number", "Invalid X co-ordinate!");
  assert(type(nY)=="number", "Invalid Y co-ordinate!");
  -- Get tile offset from location and return level offset if valid
  local iLoc<const> = GetTileOffsetFromObject(aObject, nX, nY);
  if iLoc then return iLoc * 2 end;
end
-- Update level and level mask --------------------------------------------- --
local function UpdateLevel(iPos, iId)
  -- Check parameters
  assert(type(iPos)=="number", "Invalid level tile position!");
  assert(type(iId)=="number", "Invalid level tile index!");
  -- Update level data with specified tile
  binLevel:WU16LE(iPos, iId);
  -- Calculate absolute X and Y position from location
  local iX<const>, iY<const> = floor(iPos / 2) % TotalLevelWidth * 16,
                               floor(iPos / TotalLevelWidthX2) * 16;
  -- Update zone collision bitmask
  maskZone:Copy(maskLev, binLevel:RU16LE(iPos), iX, iY);
  -- This part will keep the 1 pixel barrier around the level
  if iPos < TotalLevelWidth then
    -- Keep top-left corner barrier to stop objects going off screen
    if iPos == 0 then maskZone:Fill(0, 0, 16, 1);
                      maskZone:Fill(0, 0, 1, 16);
    -- Keep top-right corner barrier to stop objects going off screen
    elseif iPos == TotalLevelWidthX2M1 then maskZone:Fill(iX-16, 0, 16, 1);
                                            maskZone:Fill(iX - 1, 0, 1, 16);
    -- Top row
    else maskZone:Fill(iX, 0, iX + 16, 1) end;
  -- Bottom row?
  elseif iPos >= TotalLevelSizeMLW then
    -- Keep bottom-left corner barrier to stop objects going off screen
    if iPos == TotalLevelSizeMLW then maskZone:Fill(0, iY, 1, 16);
                                      maskZone:Fill(0, iY+15, 16, 1);
    -- Keep bottom-right corner barrier to stop objects going off screen
    elseif iPos == TotalLevelSizeM1 then maskZone:Fill(iX+15, iY, 1, 16);
                                         maskZone:Fill(iX, iY+15, 16, 1);
    -- Bottom row?
    else maskZone:Fill(iX, iY+15, 16, 1) end;
    -- Keep left side barrier to stop objects going off screen
  elseif iPos % TotalLevelWidthX2 == 0 then maskZone:Fill(0, iY, 1, iY+16);
  -- Keep right side barrier to stop objects going off screen
  elseif iPos % TotalLevelWidthX2 == TotalLevelWidthX2M1 then
    maskZone:Fill(iX+15, iY, 1, iY+16);
  end
end
-- Get win limit ----------------------------------------------------------- --
local function HaveZogsToWin(aPlayer) return aPlayer.M >= iWinLimit end;
-- Can sell gem? ----------------------------------------------------------- --
local function CanSellGem(iObjId)
  -- Check parameters
  assert(type(iObjId)=="number", "Object not specified!");
  -- Jennites can always be sold
  if iObjId == TYP.JENNITE then return true end;
  -- Not a jennite so we need to check if it's a gem we can sell. If we can
  -- sell it? Then return success.
  for iGem = 1,3 do if aGemsAvailable[iGem] == iObjId then return true end end;
  -- Can't sell this gem so return failed
  return false;
end
-- Set object X position --------------------------------------------------- --
local function SetPositionX(aObject, iX)
  -- Update absolute and pixel position
  aObject.AX, aObject.X = floor(iX / 16), iX;
  -- Refocus on object if needed
  ObjectFocus(aObject);
end
-- Set object Y position --------------------------------------------------- --
local function SetPositionY(aObject, iY)
  -- Update absolute and pixel position
  aObject.AY, aObject.Y = floor(iY / 16), iY;
  -- Refocus on object if needed
  ObjectFocus(aObject);
end
-- Set object X and Y position --------------------------------------------- --
local function SetPosition(aObject, iX, iY)
  -- Update absolute and pixel position
  aObject.AX, aObject.X = floor(iX / 16), iX;
  aObject.AY, aObject.Y = floor(iY / 16), iY;
  -- Refocus on object if needed
  ObjectFocus(aObject);
end
-- Trigger end condition --------------------------------------------------- --
local function TriggerEnd(fcbFunc)
  assert(type(fcbFunc)=="function", "End function not specified");
  fcbFunc(iLevelId, aActivePlayer, aOpponentPlayer);
end
-- Level end conditions check ---------------------------------------------- --
local function EndConditionsCheck()
  -- No activeplayer? (Playing a demo?) Just ignore
  if not aActivePlayer or PlaySoundAtObject == UtilBlank then return end;
  -- For each player
  for iPlayerId = 1, #aPlayers do
    -- Get player data and if player has raised enough zogs?
    local aPlayer<const> = aPlayers[iPlayerId];
    if aPlayer.M >= iWinLimit then
      -- Me? Win!
      if aActivePlayer == aPlayer then return TriggerEnd(InitWin);
      -- Not me? Lose!
      else return TriggerEnd(InitLose) end;
    -- Player has run out of diggers?
    end
    -- Diggers alive counter
    local iActive = 0;
    -- For each digger the player owns
    local aDiggers<const> = aPlayer.D;
    for iDiggerId = 1, #aDiggers do
      -- Increment alive counter if digger is alive
      if aDiggers[iDiggerId] then iActive = iActive + 1 end;
    end
    -- No diggers alive?
    if iActive == 0 then
      -- Me? Lose!
      if aActivePlayer == aPlayer then return TriggerEnd(InitLoseDead);
      -- Not me? Win!
      else return TriggerEnd(InitWinDead) end;
    end
  end
end
-- Destroy object ---------------------------------------------------------- --
local function DestroyObject(iObj, aObj)
  -- Check parameters
  assert(iObj, "Object id not specified!");
  assert(aObj, "Object not specified!");
  assert(type(iObj)=="number", "Specified id is not a number!");
  assert(type(aObj)=="table", "Specified object is not a table!");
  -- Object respawns?
  if aObj.F & OFL.RESPAWN ~= 0 then
    -- Restore object health
    aObj.H = 100;
    -- Move back to start position
    SetPosition(aObj, aObj.SX, aObj.SY);
    -- Phase back in
    SetAction(aObj, ACT.PHASE, JOB.NONE, DIR.NONE);
    -- Not actually destroyed
    return false;
  end
  -- Function to remove specified object from specified list
  local function RemoveObjectFromList(aList, aObj)
    -- Check parameters
    assert(aList, "List not specified!");
    assert(aObj, "Object not specified!");
    assert(type(aList)=="table", "Specified list is not a table!");
    assert(type(aObj)=="table", "Specified object is not a table!");
    -- Return if list empty
    if #aList == 0 then return end;
    -- For each object (Using 'while' because we delete)
    local iObj = 1 while iObj <= #aList do
      -- If target object is our object then delete it from the
      -- specified list else increment to the next object id.
      if aList[iObj] == aObj then remove(aList, iObj) else iObj = iObj + 1 end;
    end
  end
  -- Remove object from the global objects list
  remove(aObjects, iObj);
  -- Get objects owner and return if object does not have an owner
  local aPlayer<const> = aObj.P;
  if not aPlayer then return true end;
  -- Get digger owners' diggers and enumerate them
  local aDiggers<const> = aPlayer.D;
  for iDigger = 1, #aDiggers do
    -- If digger object is owner by player?
    if aDiggers[iDigger] == aObj then
      -- Mark it as dead and reduce players' digger count
      aDiggers[iDigger] = false;
      aPlayer.DC = aPlayer.DC - 1
    end
  end
  -- For each remaining object (original object already removed)
  for iTargetObj = 1, #aObjects do
    -- Get target object and reset objects target if object is targeting it
    local aTargetObj<const> = aObjects[iTargetObj];
    if aTargetObj.T == aObj then aTargetObj.T = nil end
    -- Check objects teleport destination and inventory list
    RemoveObjectFromList(aObj.TD, aObj);
    RemoveObjectFromList(aObj.I, aObj);
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
  -- Check id speciifed
  assert(type(aObject)=="table", "Object not specified!");
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
  assert(type(aObj)=="table", "Object carrier not valid!");
  assert(type(aInvObj)=="table", "Object to collect not valid!");
  -- Find object in objects array
  for iObj = 1, #aObjects do
    -- Get object and if it matches the specified object to pickup?
    if aObjects[iObj] == aInvObj and
      (not bOnlyTreasure or
           aInvObj.F & OFL.TREASURE ~= 0) then
      -- Remove object and add object to player inventory
      remove(aObjects, iObj);
      insert(aObj.I, aInvObj);
      -- If object is phasing then reset the object
      if aInvObj.A == ACT.PHASE then
        SetAction(aInvObj, ACT.STOP, JOB.NONE, DIR.NONE) end;
      -- Add weight
      aObj.IW = aObj.IW + aInvObj.OD.WEIGHT;
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
  assert(type(aObj)=="table", "Invalid object specified!");
  assert(type(iItemId)=="number", "Invalid item id specified!");
  -- Get object data
  local aObjData<const> = aObjectData[iItemId];
  assert(type(aObjData)=="table", "No such item id exists!");
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
      insert(aObjects, aInvObj);
      -- Reduce carrying weight
      aObj.IW = aObj.IW - aInvObj.OD.WEIGHT;
      -- Select next object
      aObj.IS = aObj.I[iInvObj];
      -- If invalid select first object
      if not aObj.IS then aObj.IS = aObj.I[1] end;
      -- Done
      return true;
    end
  end
  -- Failed to drop object
  return false;
end
-- Sell an item ------------------------------------------------------------ --
local function SellItem(aObj, aInvObj)
  -- Check parameters
  assert(type(aObj)=="table", "Object not specified!");
  assert(type(aInvObj)=="table", "Inventory object not specified!");
  -- Remove object from inventory and return if falied
  if not DropObject(aObj, aInvObj) then return false end;
  -- Increment funds but deduct value according to damage
  local aParent<const> = aObj.P;
  aParent.M = aParent.M +
    floor((aInvObj.OD.VALUE / 2) * (aInvObj.H / 100));
  -- Plus time added value if treasure
  if aInvObj.F & OFL.TREASURE ~= 0 then
    aParent.GS = aParent.GS + 1;
    local iAmount<const> = floor(iGameTicks / 18000);
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
  assert(type(aObj)=="table", "Object not specified!");
  assert(type(iItemId)=="number", "Item type not specified!");
  -- Get digger inventory and return if no items
  local aObjInv<const> = aObj.I;
  if #aObjInv == 0 then return -1 end;
  -- Something sold?
  local iItemsSold = 0;
  local iInvId = 1 while iInvId <= #aObjInv do
    -- Get object and if is a matching type and we can sell and we sold itt?
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
  if aObj == aTObj or                        -- ...is me?
     aTObj.F & OFL.PICKUP == 0 or            -- -or- cant be grabbed?
     aObj.IW + aTObj.OD.WEIGHT > aObj.STR or -- -or- too heavy?
     not IsSpriteCollide(                    -- -or- not touching?
       aObj.S, aObj.X+aObj.OFX, aObj.Y+aObj.OFY,
       aTObj.S, aTObj.X+aTObj.OFX, aTObj.Y+aTObj.OFY) then
    return false end;
  -- Add object to objects inventory
  return AddToInventory(aObj, aTObj, bOnlyTreasure);
end
-- Pickup Objects ---------------------------------------------------------- --
local function PickupObjects(aObj, bOnlyTreasure)
  -- Look for objects that are grabbable
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
local function SetRandomJob(...)
  -- AI decisions data
  local aAIChoicesData<const> = {
    {FDD = DIR.L,  F = { ACT.WALK, JOB.DIGDOWN, DIR.TCTR },
                   S = { ACT.WALK, JOB.DIG,     DIR.L    } },
    {FDD = DIR.R,  F = { ACT.WALK, JOB.DIGDOWN, DIR.TCTR },
                   S = { ACT.WALK, JOB.DIG,     DIR.R    } },
    {FDD = DIR.UL, F = { ACT.WALK, JOB.DIG,     DIR.L    },
                   S = { ACT.WALK, JOB.DIG,     DIR.UL   } },
    {FDD = DIR.UR, F = { ACT.WALK, JOB.DIG,     DIR.R    },
                   S = { ACT.WALK, JOB.DIG,     DIR.UR   } },
    {FDD = DIR.DL, F = { ACT.WALK, JOB.DIG,     DIR.L    },
                   S = { ACT.WALK, JOB.DIG,     DIR.DL   } },
    {FDD = DIR.DR, F = { ACT.WALK, JOB.DIG,     DIR.R    },
                   S = { ACT.WALK, JOB.DIG,     DIR.DR   } },
    {FDD = DIR.L,  F = { ACT.WALK, JOB.DIGDOWN, DIR.TCTR },
                   S = { ACT.WALK, JOB.SEARCH,  DIR.L    } },
    {FDD = DIR.R,  F = { ACT.WALK, JOB.DIGDOWN, DIR.TCTR },
                   S = { ACT.WALK, JOB.SEARCH,  DIR.R    } },
    {FDD = DIR.D,  F = { ACT.WALK, JOB.DIG,     DIR.LR   },
                   S = { ACT.WALK, JOB.DIGDOWN, DIR.TCTR } },
  };
  -- Actual function
  local function DoSetRandomJob(aObject, bUser)
    -- Select a random choice
    local aChoice = aAIChoicesData[random(#aAIChoicesData)];
    -- Failed direction matches then try something else
    if aChoice.FDD == aObject.FDD then aChoice = aChoice.F;
    -- We're not blocked from digging so try digging in that direction
    else aChoice = aChoice.S end;
    -- Set new AI choice and return
    SetAction(aObject, aChoice[1], aChoice[2], aChoice[3], bUser);
  end
  -- Call function
  DoSetRandomJob(...);
  -- Set actual funtion
  SetRandomJob = DoSetRandomJob;
end
-- Add player -------------------------------------------------------------- --
local function CreatePlayer(iX, iY, iId, iRace, bIsAI)
  -- Check parameters
  assert(iX ~= nil, "X co-ordinate not specified!");
  assert(iX >= 0, "X co-ordinate "..iX.." too low!");
  assert(iX < TotalLevelWidth, "X co-ordinate "..iX.." too high!");
  assert(iY ~= nil, "Y co-ordinate not specified!");
  assert(iY >= 0, "Y co-ordinate "..iY.."  too low!");
  assert(iY < TotalLevelHeight, "Y co-ordinate "..iY.." too high!");
  assert(iId ~= nil, "Player id not specified!");
  assert(not aPlayers[iId], "Player "..iId.." already initialised!");
  assert(iRace ~= nil, "Race not specified!");
  assert(bIsAI ~= nil, "AI type not specified!");
  assert(#aRacesAvailable > 0, "No races available!");
  -- Digger in races list picked
  local iRacesId;
  -- If random digger requested?
  if iRace == TYP.DIGRANDOM then
    -- Pick a random race from the races array
    iRacesId = random(#aRacesAvailable);
    -- Override the random race type with an actual race type
    iRace = aRacesAvailable[iRacesId];
  -- Actually specified specific race
  else
    -- For each available race
    iRacesId = 1;
    while iRacesId <= #aRacesAvailable do
      -- Get race type id and if we matched requested race then break
      if aRacesAvailable[iRacesId] == iRace then break end;
      -- Try next races id
      iRacesId = iRacesId + 1;
    end
    -- Wasn't able to remove anything? Show error
    if iRacesId > #aRacesAvailable then
      error("Race "..iRace.." not available!") end;
  end
  -- Remove the race from the table so it can't be duplicated
  remove(aRacesAvailable, iRacesId);
  -- Players diggers and number of diggers to create
  local aDiggers<const>, iNumDiggers<const> = { }, 5;
  -- Adjusted source positions
  local iXM1x16<const>, iYP2x16<const> = (iX - 1) * 16, (iY + 2) * 16;
  -- Build player data object
  local aPlayer<const> = {
    D   = aDiggers,                    -- List of diggers
    DT  = iNumDiggers,                 -- Diggers total
    DC  = iNumDiggers,                 -- Diggers count
    DUG = 0,                           -- Dirt dug
    GEM = 0,                           -- Gems found
    GS  = 0,                           -- Gems sold
    PUR = 0,                           -- Purchases made
    BP  = 0,                           -- Purchases made in zogs
    GI  = 0,                           -- Total income
    M   = 100,                         -- Money
    R   = iRace,                       -- Race type (TYP.*)
    I   = iId,                         -- Player index
    HX  = iX * 16 - 2,                 -- Home point X position
    HY  = iY * 16 + 32,                -- Home point Y position
    SX  = iXM1x16,                     -- Adjust home point X
    SY  = iYP2x16,                     -- Adjust home point Y
    RD  = aObjectData[iRace]            -- Race data
  };
  -- Check race data
  assert(aPlayer.RD, "No race data for race "..iRace);
  -- If this is player one
  if iId == 1 and not bIsAI then
    -- Set as my player
    aActivePlayer = aPlayer;
    -- Set viewpoint on this player and synchronise
    ScrollViewPortTo(iX-TotalTilesWidthD2, iY-TotalTilesHeightD2);
    SetViewPort(PosAX * 16, PosAY * 16);
    -- Add capital carried and reset its value
    aActivePlayer.M = aActivePlayer.M + aGlobalData.gCapitalCarried;
    aGlobalData.gCapitalCarried = 0;
  -- Opponent player? Set Opponent
  elseif iId == 2 then aOpponentPlayer = aPlayer end;
  -- AI player override patience logic
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
  -- For each digger of the player
  local iIndex = 0;
  while iIndex < iNumDiggers do
    -- Create a digger
    local aDigger<const> =
      CreateObject(iRace, iXM1x16+(iIndex*5)-1, iYP2x16, aPlayer);
    if aDigger then
      -- Insert digger into playfield
      insert(aDiggers, aDigger);
      -- Digger is player controlled?
      if not bIsAI then
        -- Set patience AI for player controlled digger
        aDigger.AI, aDigger.AIF = AI.PATIENCE, AIPatienceLogic;
        -- Set and verify patience warning value
        aDigger.PW = aDigger.OD.PATIENCE;
        assert(aDigger.PW,
          "Digger "..iIndex.." has no patience warning value!");
        -- Randomise patience by +/- 25%
        local iOffset = random(floor(aDigger.PW * 0.25));
        if random() < 0.5 then iOffset = -iOffset end;
        -- Digger will stray after 60 seconds of indicated impatience
        aDigger.PW = aDigger.PW + iOffset;
        aDigger.PL = aDigger.PW + 3600;
      end
    -- Failed so show map maker in console that the objid is invalid
    else ConsoleWrite("Warning! Digger "..iIndex..
      " not created for player "..iId.."! Id="..iRace..", X="..iX..
      ", Y="..iY..".", 9) end;
    -- Next digger
    iIndex = iIndex + 1;
  end
  -- Add player data to players array
  insert(aPlayers, aPlayer);
  -- Log creation of item
  CoreLog("Created player "..iId.." as '"..aObjectData[iRace].NAME.."'["..
    iRace.."] at AX:"..iX..",AY:"..iY.." in position #"..#aPlayers.."!");
end
-- Object is at home ------------------------------------------------------- --
local function ObjectIsAtHome(aObject)
  assert(aObject, "Object not specified!");
  assert(aObject.P, "Object specified is an orphan!");
  return aObject.X == aObject.P.HX and aObject.Y == aObject.P.HY;
end
-- Set object action ------------------------------------------------------- --
local function DoSetAction(O, A, J, D, U)
  assert(O, "Object not specified!");
  assert(type(O)=="table", "Object not a table!");
  assert(A, "Action not specified!");
  assert(J, "Job not specified!");
  assert(D, "Direction not specified!");
  -- Dying or eaten requested?
  if A == ACT.DEATH or A == ACT.EATEN then
    -- Remove jump and fall flags or if the digger is jumping then busy will
    -- be unset and they will be able to instantly come out of phasing.
    O.F = O.F & ~(OFL.JUMPRISE | OFL.JUMPFALL);
    -- Force normal timer speed for animation
    O.ANT = aTimerData.ANIMNORMAL;
  -- Display map? Display map
  elseif A == ACT.MAP then
    -- Remove play sound function
    PlaySoundAtObject = UtilBlank;
    -- Display map
    return InitTNTMap();
  -- Control flood gate?
  elseif A == ACT.OPEN or A == ACT.CLOSE then
    -- Get location at specified tile and if id is valid?
    local iId<const>, iLoc<const> = GetLevelDataFromAbsCoordinates(O.AX, O.AY);
    if iId then
      -- Location updater and sound palyer
      local function UpdateFloodGate(iTileId, iSfxId)
        -- Update level with specified id and play requested sound effect
        UpdateLevel(iLoc, iTileId);
        PlaySoundAtObject(O, iSfxId);
      end
      -- Open gate?
      if A == ACT.OPEN then
        -- Gate closed (no water any side)?
        if iId == 434 then return UpdateFloodGate(438, aSfxData.GOPEN);
        -- Gate closed (water on left, right or both sides)?
        elseif iId >= 435 and iId <= 437 then
          -- Check if opening caused a flood
          insert(aFloodData, { iLoc, aTileData[1 + iId] });
          -- Set flooded open gate and play open sound
          return UpdateFloodGate(439, aSfxData.GOPEN);
        end
      -- Close gate?
      else
        -- Gate open (water on neither side)?
        if iId == 438 then return UpdateFloodGate(434, aSfxData.GCLOSE);
        -- Gate open (water on both sides)?
        elseif iId == 439 then return UpdateFloodGate(437, aSfxData.GCLOSE) end;
      end
    end
    -- Play error sound and return
    return PlaySoundAtObject(O, aSfxData.ERROR);
  -- Deploy?
  elseif A == ACT.DEPLOY then
    -- Object deployed?
    local DeploySuccess;
    -- Which object? Track
    if O.ID == TYP.TRACK then
      -- Check 5 tiles at object position and lay track
      for I = 0, 4 do
        -- Calculate absolute location of object and get tile id. Break if bad
        local iId, iLoc<const> = GetLevelDataFromObject(O, (I*16)+8, 15);
        if not iId then break end;
        -- Check if it's a tile we can convert and break if we can't
        iId = aTrainTrackData[iId];
        if not iId then break end;
        -- Get terrain tile id blow this tile and if we can deploy on this?
        if aTileData[1+binLevel:RU16LE(iLoc+TotalLevelWidthX2)] &
          aTileFlags.F ~= 0 then
          -- Update level data and set that we deployed successfulyl
          if not DeploySuccess then DeploySuccess = true end;
          UpdateLevel(iLoc, iId);
        end
      end
    -- Flood gate
    elseif O.ID == TYP.GATE then
      -- Calculate absolute location of object below and if valid and the tile
      -- below it is firm ground? Also creation of an invisible flood gate
      -- object was successful?
      local iId, iLoc<const> = GetLevelDataFromObject(O, 8, 16);
      if iId and aTileData[1 + iId] & aTileFlags.F ~= 0 and
        CreateObject(TYP.GATEB,
          floor((iLoc%TotalLevelWidthX2)/2)*16,
          floor((iLoc-TotalLevelWidthX2)/TotalLevelWidthX2)*16, O.P) then
        -- Update tile to a flood gate
        UpdateLevel(iLoc - TotalLevelWidthX2, 438);
        -- Success!
        DeploySuccess = true;
      end
    -- Elevator
    elseif O.ID == TYP.LIFT then
      -- Calculate absolute location of object
      local iLoc<const> = GetLevelOffsetFromObject(O, 8, 0);
      -- Search for a buildable ground surface
      for iBottom = iLoc, 32512, TotalLevelWidthX2 do
        -- Get tile
        local iId<const> = binLevel:RU16LE(iBottom);
        local iTileId<const> = aTileData[1 + iId];
        -- Tile has not been dug
        if iTileId & aTileFlags.AD == 0 then
          -- If we're on firm ground?
          if iTileId & aTileFlags.F ~= 0 then
            -- Search for a buildable above ground surface
            for iTop = iLoc, TotalLevelWidthX2, -TotalLevelWidthX2 do
              -- Get tile
              local iId<const> = binLevel:RU16LE(iTop);
              local iTileId<const> = aTileData[1 + iId];
              -- Tile has not been dug
              if iTileId & aTileFlags.AD == 0 then
                -- Tile is firm buildable ground?
                if iTileId & aTileFlags.F ~= 0 then
                  -- Height check and if ok and creating an object went ok?
                    -- Create lift object
                  if iTop >= TotalLevelWidthX2 and iBottom-iTop >= 768 and
                    CreateObject(TYP.LIFTB,
                      floor((O.X+8)/16)*16, floor((O.Y+15)/16)*16, O.P) then
                    -- We are deploying now so set success
                    DeploySuccess = true;
                    -- Update level data for top and bottom part of lift
                    UpdateLevel(iTop, 62);
                    UpdateLevel(iBottom, 190);
                    -- Draw cable
                    for iTop = iTop+TotalLevelWidthX2,
                      iBottom-TotalLevelWidthX2, TotalLevelWidthX2 do
                      -- Update level data for bottom part of lift
                      UpdateLevel(iTop, 189);
                    end
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
    end
    -- Destroy object if succeeded?
    if DeploySuccess then DestroyObjectUnknown(O);
    -- Failed so play sound
    else PlayStaticSound(aSfxData.ERROR) end;
    -- Do nothing else
    return;
  -- Buy digger?
  elseif A == ACT.REC then
    -- Get alias to owner of this object
    local aPlayer<const> = O.P;
    -- Has enough money?
    local iMoney<const> = aPlayer.M;
    if iMoney > 1000 then
      -- Get alias to object's owners diggers. Find a dead digger
      local aDiggers<const> = aPlayer.D;
      for iDiggerId = 1, #aDiggers do
        -- Get digger object and if digger is dead?
        local aDigger = aDiggers[iDiggerId];
        if not aDigger then
          -- Ressurect it and if successful?
          aDigger = CreateObject(O.ID,
            (aPlayer.HX-19)+(iDiggerId*5)-1, aPlayer.HY, aPlayer);
          if aDigger then
            -- Assign the Digger
            aDigger[iDiggerId] = aDigger;
            -- Deduct funds
            aPlayer.M = iMoney - 1000;
            -- Sale of the century!
            return PlayStaticSound(aSfxData.TRADE);
          end
        end
      end
    end
    -- Failed!
    PlayStaticSound(aSfxData.ERROR);
    -- Do nothing else
    return;
  -- Jump?
  elseif A == ACT.JUMP then
    -- Object is...
    if O.A ~= ACT.FIGHT and            -- ...not fighting -and-
       O.F & OFL.BUSY == 0 and         -- ...not busy -and-
       O.F & OFL.JUMPRISE == 0 and     -- ...not jumping -and-
       O.F & OFL.JUMPFALL == 0 and     -- ...not jump falling -and-
       O.FS == 1 and                   -- ...not actually falling -and-
       O.FD == 0 then                  -- ...not accumulating fall damage
      -- Remove fall flag and add busy and jumping flags
      O.F = (O.F | OFL.BUSY | OFL.JUMPRISE) & ~OFL.FALL;
      -- Play jump sound
      PlaySoundAtObject(O, aSfxData.JUMP);
      -- Reset action timer
      O.AT = 0;
    end
    -- Do nothing else
    return;
  -- Grab nearest item?
  elseif A == ACT.GRAB then
    -- Pickup nearest object. Play error if failed
    if not PickupObjects(O, false) then PlayStaticSound(aSfxData.ERROR) end;
    -- Do nothing else
    return;
  -- Drop selected item?
  elseif A == ACT.DROP then
    -- Drop selected object. Play error if failed
    if not DropObject(O, O.IS) then PlayStaticSound(aSfxData.ERROR) end;
    -- Do nothing else
    return;
  -- Cycle items?
  elseif A == ACT.NEXT then
    -- For each object in inventory
    local aInv<const> = O.I;
    for iInvIndex = 1, #aInv do
      -- Get inventory object and if we got it
      local aInvObj<const> = aInv[iInvIndex];
      if aInvObj == O.IS then
        -- Set next object
        O.IS = aInv[iInvIndex+1];
        -- Invalid object? Select first object! No need to check if this is
        -- valid because 'nil' is valid for 'no more items'.
        if not O.IS then O.IS = aInv[1] end;
        -- Done
        return;
      end
    end
    -- No items so play failed sound
    PlayStaticSound(aSfxData.ERROR);
    -- Do nothing else
    return;
  -- Phasing?
  elseif A == ACT.PHASE then
    -- Phasing home? Refuse action if not enough health
    if J == JOB.PHASE and D == DIR.U and O.H <= 5 and
      O.F & OFL.TPMASTER == 0 then return end;
    -- Remove jump and fall flags or if the digger is jumping then busy will
    -- be unset and they will be able to instantly come out of phasing.
    O.F = O.F & ~(OFL.JUMPRISE | OFL.JUMPFALL);
  end
  -- Reset action timer
  O.AT = 0;
  -- Compare direction. Want to go left or right?
  if D == DIR.LR then
    -- Randomly select left or right
    if random() < 0.5 then D = DIR.L else D = DIR.R end;
  -- Want object to goto the centre of the tile
  elseif D == DIR.TCTR then
    -- Set direction so it heads to the centre of the tile
    if O.X % 16 - 8 < 0 then D = DIR.L else D = DIR.R end;
  -- Want to go in direction of home?
  elseif D == DIR.HOME then
    -- Preserve action but action stopped? Set object walking
    if A == ACT.KEEP and O.A == ACT.STOP then A = ACT.WALK end;
    -- Set direction of home
    if O.X < O.P.HX then D = DIR.R;
    elseif O.X > O.P.HX then D = DIR.L;
    else D = DIR.NONE end;
  -- Want to go in opposite direction?
  elseif D == DIR.OPPOSITE then
    -- Compare current direction. Up-left -> Up-right?
    if O.D == DIR.UL then D = DIR.UR;
    -- No direction set (Go right)
    elseif O.D == DIR.NONE then D = DIR.R;
    -- Left -> Right
    elseif O.D == DIR.L then D = DIR.R;
    -- Down-left -> Down-right
    elseif O.D == DIR.DL then D = DIR.DR;
    -- Up-right -> Up-left
    elseif O.D == DIR.UR then D = DIR.UL;
    -- Right -> Left
    elseif O.D == DIR.R then D = DIR.L;
    -- Down-right -> Down-left
    elseif O.D == DIR.DR then D = DIR.DL end;
  -- Keep existing job? Keep existing job!
  elseif D == DIR.KEEP then D = O.D;
  -- Keep existing job if moving?
  elseif D == DIR.KEEPMOVE then
    -- If not going in any useful direction?
    if O.D ~= DIR.L and O.D ~= DIR.DL and O.D ~= DIR.UR and
       O.D ~= DIR.R and O.D ~= DIR.L and O.D ~= DIR.DR then
      -- Randomly select left or right
      if random() < 0.5 then D = DIR.L else D = DIR.R end;
    -- Else keep existing job
    else D = O.D end;
  end
  -- In-danger?
  if J == JOB.INDANGER then
    -- Keep busy unset if not dead or phasing!
    if O.A ~= ACT.DEATH and O.A ~= ACT.PHASE then O.F = O.F & ~OFL.BUSY end;
  -- Keep existing job but don't dig down?
  elseif J == JOB.KNDD then
    -- Is digging down? Remove job else keep job
    if O.J == JOB.DIGDOWN then J = JOB.NONE else J = O.J end;
  -- Keep existing job?
  elseif J == JOB.KEEP then J = O.J end;
  -- Compare action. Stop requested?
  if A == ACT.STOP then
    -- If object can stop? Keep busy unset!
    if O.CS then O.F = O.F & ~OFL.BUSY;
    -- Can't stop? Set default action and move in opposite direction
    else return DoSetAction(O, O.OD.ACTION, JOB.KEEP, DIR.OPPOSITE) end;
  -- Keep existing job? Keep existing action!
  elseif A == ACT.KEEP then A = O.A end;
  -- Get if action, direction or job changed?
  if A == O.A and J == O.J and D == O.D then
    -- Reset job timer if user initiated
    if U then O.JT = 0 end;
    -- Done
    return;
  end
  -- Set new action, direction and job
  O.A, O.D, O.J = A, D, J;
  -- Remove all flags that are related to object actions
  O.F = O.F & ~(O.AD.FLAGS or 0);
  -- Set action data according to lookup table
  O.AD = O.OD[A];
  assert(O.AD, O.OD.NAME.." actdata for "..A.." not found!");
  -- If object has patience?
  if O.PW then
    -- Object starts as impatient? Set impatient
    if O.AD.FLAGS and O.AD.FLAGS & OFL.IMPATIENT ~= 0 then O.JT = O.PW;
    -- Reset value if the user made this action
    elseif U then O.JT = 0 end;
  -- Patience disabled
  else O.JT = 0 end;
  -- Set directional data according to lookup table
  O.DD = O.AD[D];
  assert(O.DD, O.OD.NAME.." dirdata for "..A.."->"..D.." not found!");
  -- Set sprite data
  O.S1, O.S2 = O.DD[1], O.DD[2];
  -- Random tile requested?
  if O.F & OFL.RNGSPRITE ~= 0 then
    -- Get random sprite id
    local iSprite<const> = random(0) % (O.DD[2] - O.DD[1]);
    -- Does a new animation id need to be set?
    -- Set first animation frame and reset animation timer
    O.S, O.ST = O.S1 + iSprite, iSprite;
  -- Start at zero.
  else
    -- Set sprite data
    O.S1, O.S2 = O.DD[1], O.DD[2];
    -- Does a new animation id need to be set?
    -- Set first animation frame and reset animation timer
    if O.S < O.S1 or O.S > O.S2 then O.S, O.ST = O.S1, 0 end;
  end
  -- Set attachment info
  O.STA = O.OD.ATTACHMENT;
  if O.STA then
    -- Set action data
    O.AA = aObjectData[O.STA][O.A];
    -- Set object data
    O.DA = O.AA[O.D];
    -- Set sprite data
    O.S1A, O.S2A = O.DA[1], O.DA[2];
    -- Set attachment sprite
    if O.SA < O.S1A or O.SA > O.S2A then O.SA = O.S1A end;
    -- Set offset according to attachment
    O.OFXA, O.OFYA = O.DA[3] or 0, O.DA[4] or 0;
  end
  -- Set offset
  O.OFX, O.OFY = O.DD[3] or 0, O.DD[4] or 0;
  -- Re-add flags according to lookup table
  O.F = O.F | (O.AD.FLAGS or 0);
  -- If sound is to be played at default pitch?
  if O.AD.SOUND then PlaySoundAtObject(O, O.AD.SOUND)
  -- Sound with random pitch?
  elseif O.AD.SOUNDRP then
    PlaySoundAtObject(O, O.AD.SOUNDRP, 0.975+(random()%0.05)) end;
end
SetAction = DoSetAction;
-- Roll the dice to spawn treasure at the specified location --------------- --
local function RollTheDice(nX, nY)
  -- Get chance to reveal a gem
  local nChance = aTimerData.GEMCHANCE;
  -- Add up to double chance depending on depth
  if nY >= aTimerData.GEMDEPTHEXTRA then
    nChance = nChance + (((nY - aTimerData.GEMDEPTHEXTRA) /
      aTimerData.GEMDEPTHEXTRA) * nChance) end;
  -- 5% chance to spawn a teasure
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
    CId = binLevel:RU16LE(CP);
    -- Get the tile above and adjacent the object
    if DP >= TotalLevelWidthX2 then
      TId = binLevel:RU16LE(DP-TotalLevelWidthX2) else TId = 0 end
    -- Get the tile below and adjacent to the object
    if DP < TotalLevelSize - TotalLevelWidthX2 then
      BId = binLevel:RU16LE(DP+TotalLevelWidthX2) else BId = 0 end;
    -- Get the tile adjacent to the object
    AId = binLevel:RU16LE(DP);
    -- Get the tile left of the object
    if DP - 2 >= 0 then LId = binLevel:RU16LE(DP-2) else LId = 0 end;
    -- Get the tile right of the object
    if DP + 2 < TotalLevelSize then RId = binLevel:RU16LE(DP+2)
                               else RId = 0 end;
  end
  -- Cache digging direction of object and if going left or upleft?
  local iDirection<const> = aObject.D;
  if iDirection == DIR.L or iDirection == DIR.UL then
    if iDirection == DIR.UL then
      CP = GetLevelOffsetFromObject(aObject, 8, 15) or 0;
    else CP = 0 end;
    DP = GetLevelOffsetFromObject(aObject, 5, 15) or 0;
    GetDigTileData();
    local TDLId<const>, TDTId<const> =
     aTileData[LId+1], aTileData[TId+1];
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
      aTileData[BId+1], aTileData[LId+1], aTileData[TId+1];
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
    local TDTId<const>, TDRId<const> = aTileData[TId+1], aTileData[RId+1];
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
      aTileData[BId+1], aTileData[RId+1], aTileData[TId+1];
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
      aTileData[LId+1], aTileData[TId+1], aTileData[RId+1], aTileData[BId+1];
    if TDTId & aTileFlags.P ~= 0 or
      (TDBId & aTileFlags.W ~= 0 and TDBId & aTileFlags.ET ~= 0) or
      (TDLId & aTileFlags.W ~= 0 and TDLId & aTileFlags.ER ~= 0) or
      (TDRId & aTileFlags.W ~= 0 and TDRId & aTileFlags.EL ~= 0) then
      return false end;
  else return false end;
  -- Get digging data table
  local aDigDataItem<const> = aDigData[iDirection];
  if not aDigDataItem then return false end;
  -- Walk through all the digdata structures to find info about current tile
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
      if DP >= TotalLevelWidthX2 then
        UpdateLevel(DP-TotalLevelWidthX2, TId) end;
      -- Update level
      UpdateLevel(DP, AId);
      -- If tile location is not at the bottom of the level. Update below tile
      if DP < TotalLevelSize-TotalLevelWidthX2 then
        UpdateLevel(DP+TotalLevelWidthX2, BId) end;
      -- Dig was successful
      return true;
    end
  end
  -- Dig failed
  return false;
end
-- Set object health ------------------------------------------------------- --
local function AdjustObjectHealth(aObject, iAmount)
  -- Clamp health between 0 (death) and 100 (maximum) and return if not dead
  aObject.H = UtilClampInt(aObject.H + iAmount, 0, 100);
  if aObject.H > 0 then return end;
  -- Kill object (Don't move this, for explosion stuff to work)
  SetAction(aObject, ACT.DEATH, JOB.INDANGER, DIR.NONE);
  -- Remove jump and floating status from object
  aObject.F = aObject.F & ~(OFL.JUMPRISE|OFL.JUMPFALL);
  -- Object explodes on death?
  if aObject.F & OFL.EXPLODE ~= 0 then
    -- Enumerate possible destruct positions again. We can't have the TERRAIN
    -- destruction checks in the above enumeration because of the rescursive
    -- nature of the OBJECT destruction which would cause problems.
    for iExplodeIndex = 1, #aExplodeDirData do
      -- Get destruct adjacent position data
      local aCoordAdjust<const> = aExplodeDirData[iExplodeIndex];
      -- Clamp the centre tile position of the explosion for the level
      local iX<const>, iY<const> =
        floor((aObject.X + 8) / 16 + aCoordAdjust[1]),
        floor((aObject.Y + 8) / 16 + aCoordAdjust[2]);
      -- Calculate locate of tile and if in valid bounds?
      local iId, iLoc<const> = GetLevelDataFromAbsCoordinates(iX, iY);
      if iId then
        -- Compare against all objects
        for iObject = 1, #aObjects do
          -- Get target object data and kill target...
          local aTarget<const> = aObjects[iObject];
          if aTarget ~= aObject and            -- ...isn't this object?
             aTarget.A ~= ACT.DEATH and        -- -and- not dying?
             aTarget.A ~= ACT.PHASE and        -- -and- not phasing?
            IsSpriteCollide(476, iX*16, iY*16, -- -and- in explosion?
              aTarget.S, aTarget.X+aTarget.OFX, aTarget.Y+aTarget.OFY) then
                AdjustObjectHealth(aTarget, -100) end
        end
        -- Get tile flags and if tile is destructable and not been cleared?
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
              -- Get flood test data and calcuate location to test
              local aFloodTestItem<const> = aExplodeDirData[iFloodIndex];
              local iTLoc<const> = (iLoc + (aFloodTestItem[2] *
                TotalLevelWidthX2)) + (aFloodTestItem[1] * 2);
              -- Get tile id and if valid
              iId = GetLevelDataFromLevelOffset(iTLoc);
              if iId then
                -- Get flags to test for and insert a new flood if found
                local iTFFlags<const> = aFloodTestItem[3];
                if aTileData[1 + iId] & iTFFlags == iTFFlags then
                  insert(aFloodData, { iTLoc, iTFFlags });
                end
              end
            end
          else
            -- Set cleared water tile
            UpdateLevel(iLoc, 247);
            -- Test for flood here with all edges exposed
            insert(aFloodData, { iLoc, aTileFlags.W|aTileFlags.EA });
          end
          -- Tile blown was firm ground?
          if iTFlags & aTileFlags.F ~= 0 then
            -- Get tile location above
            local iTLoc<const> = iLoc - TotalLevelWidthX2;
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
                insert(aFloodData, { iTLoc, aTileFlags.EA });
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
  texLev:BlitSLT(texBg, -TotalLevelWidth+((TotalLevelWidth-iPosX)/TotalLevelWidth*64),
    -32+((TotalLevelHeight-iPosY)/TotalLevelHeight*32));
  -- Calculate the X pixel position to draw at
  local iXdraw<const> = iStageL + PosPXC;
  -- For each screen row to draw tile at
  for iY = 0, iTilesHeight do
    -- Calculate the Y position to grab from the level data
    local iYdest<const> = (iPosY + iY) * TotalLevelWidth;
    -- Calculate the Y pixel position to draw at
    local iYdraw<const> = iStageT + PosPYC + (iY * 16);
    -- For each screen column to draw tile at, draw the tile from level data
    for iX = 0, iTilesWidth do
      texLev:BlitSLT(binLevel:RU16LE((iYdest + (iPosX + iX)) * 2),
                                      iXdraw + (iX * 16), iYdraw);
    end
  end
end
-- Render all objects ------------------------------------------------------ --
local function RenderObjects()
  -- Calculate viewpoint position
  local nVPX<const>, nVPY<const> = PosPX-iStageL, PosPY-iStageT;
  -- For each object
  for iObjId = 1, #aObjects do
    -- Get object data
    local aObject<const> = aObjects[iObjId];
    -- Holds objects render position on-screen
    local iXX, iYY = aObject.X-nVPX+aObject.OFX, aObject.Y-nVPY+aObject.OFY;
    BCBlit(texSpr, aObject.S, iXX, iYY, iXX+16, iYY+16);
    -- Got an attachment? Draw it too!
    if aObject.STA then
      iXX, iYY = iXX+aObject.OFXA, iYY+aObject.OFYA;
      BCBlit(texSpr, aObject.SA, iXX, iYY, iXX+16, iYY+16);
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
  texSpr:BlitSLTRB(857, 8, 32, 312, 208);
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
    if not aActiveObject.P then iStatusId = 980;
    -- If object has an owner but this client is not that owner? Draw 'X'
    -- indicator above object to indicate only the owner can control this.
    elseif aActiveObject.P ~= aActivePlayer then iStatusId = 976;
    -- If object is busy? Draw 'Zz' indicator above object to indicate control
    -- is temporarily disabled
    elseif aActiveObject.F & OFL.BUSY ~= 0 then iStatusId = 984;
    -- 'v' is default, free to control this object
    else iStatusId = 988 end;
    -- Draw the indicator
    texSpr:BlitSLT(iStatusId+(floor(iGameTicks/5)%4),
      iStageL+PosPXC+aActiveObject.X-iPosX*16,
      iStageT+PosPYC+aActiveObject.Y-iPosY*16-16);
  end
  -- Digger status tile to blit
  local T = 867;
  -- Draw digger buttons and activity
  for I = 1, 5 do
    -- Get digger data and if Digger is alive?
    local aDigger<const> = aActivePlayer.D[I];
    if aDigger then
       -- Digger is selected?
      if aActiveObject and aDigger == aActiveObject then
        -- Player has health? Draw a pulsating heart
        if aDigger.H > 0 then
          texSpr:BlitSLT(797 +
            floor(((iGameTicks/(1+(aDigger.H/5))))%4), 47, 216) end;
        -- Show lightened up button
        texSpr:BlitSLT(808+I, 128+(I*16), 216)
        -- Digger has inventory?
        if aActiveObject.IW > 0 then
          -- X position of object on UI and tile ID
          local iX = 0;
          -- For each inventory
          local aObjInv<const> = aActiveObject.I;
          for iI = 1, #aObjInv do
            -- Get object
            local aObj<const> = aObjInv[iI];
            -- Get inventory conversion id and if we got it then draw it
            local iObjConvId<const> = aObjToUIData[aObj.ID];
            if iObjConvId then texSpr:BlitSLT(iObjConvId, 61+iX, 218);
            -- Draw as resized sprite
            else texSpr:BlitSLTRB(aObj.S, 61+iX, 218, 69+iX, 226) end;
            -- Increase X position
            iX = iX + 8;
          end
        end
        -- Which action?
        if aActiveObject.F & OFL.JUMPRISE ~= 0 or
           aActiveObject.F & OFL.JUMPFALL ~= 0 then
          T = 836;
        elseif aActiveObject.A == ACT.STOP then
          T = 834;
        elseif aActiveObject.A == ACT.FIGHT then
          T = 840;
        elseif aActiveObject.A == ACT.PHASE then
          T = 841;
        elseif aActiveObject.J == JOB.HOME or aActiveObject.A == ACT.HIDE then
          T = 838;
        elseif aActiveObject.J == JOB.SEARCH then
          T = 839;
        elseif aActiveObject.A == ACT.WALK or aActiveObject.A == ACT.RUN then
          T = 835;
        elseif aActiveObject.A == ACT.DIG then
          T = 837;
        end
      -- Show dimmed button
      else texSpr:BlitSLT(803+I, 128+(I*16), 216) end;
      -- Tile to blit
      local Tile;
      -- Digger is in danger?
      if aDigger.J == JOB.INDANGER then
        -- Every seond? Play error sound each second
        if floor(iGameTicks%60) == 0 then PlayStaticSound(aSfxData.ERROR) end;
        -- Every even second set a different blue indicator.
        if iGameTicks%120 < 60 then Tile = 831 else Tile = 832 end;
      -- Digger is in impatient and every even second?
      elseif aDigger.JT >= aDigger.PW and iGameTicks%120 < 60 then Tile = 833;
      -- Not in danger but busy?
      elseif aDigger.F & OFL.BUSY ~= 0 then Tile = 830;
      -- Not in danger, not busy but doing something?
      elseif aDigger.A ~= ACT.STOP then Tile = 829;
      -- Not in danger, not busy and not doing something
      else Tile = 828 end;
      -- Show activity indicator (78-84)
      texSpr:BlitSLT(Tile, 128+(I*16), 204);
    -- Digger is not alive! Show dimmed button
    else texSpr:BlitSLT(803+I, 128+(I*16), 216) end;
  end
  -- Object selected and belongs to me? Draw health bar for it
  if aActiveObject and aActiveObject.P == aActivePlayer then
    DrawHealthBar(aActiveObject.H, 2, 61, 227, 61, 229);
  end
  -- Draw object status tile
  texSpr:BlitSLT(T, 120, 216);
  -- Draw money
  fontLittle:Print(15, 220, sMoney);
  fontLittle:SetCRGB(1, 1, 1);
  -- Draw utility button
  texSpr:BlitSLT(814, 232, 216);
  -- Inventory button pressed?
  if iInfoScreen == 1 then
    -- Draw frame and title
    DrawInfoFrameAndTitle("DIGGER INVENTORY");
    -- Set tiny font spacing
    fontTiny:SetLSpacing(2);
    -- For each digger
    for iDigIndex = 1, #aActivePlayer.D do
      -- Calculate Y position
      local iY<const> = iDigIndex * 32;
      -- Print id number of digger
      fontLarge:Print(16, iY+8, iDigIndex);
      -- Draw health bar background
      texSpr:BlitSLTRB(1023, 24, iY+30, 291, iY+32);
      -- Get Digger data and if it exists?
      local aDigger<const> = aActivePlayer.D[iDigIndex];
      if aDigger then
        -- Draw digger health bar
        DrawHealthBar(aDigger.H, 0.375, 24, iY+30, 24, iY+32);
        -- Draw digger portrait
        texSpr:BlitSLT(aDigger.S, 31, iY+8);
        -- Digger has items?
        if aDigger.IW > 0 then
          -- Get digger inventory and enumerate through it and draw it
          local aInventory<const> = aDigger.I;
          for iInvIndex = 1, #aInventory do
            texSpr:BlitSLT(aInventory[iInvIndex].S, iInvIndex*16+32, iY+8);
          end
        -- No inventory. Print no inventory message
        else fontTiny:Print(48, iY+13, "NOT CARRYING ANYTHING") end;
        -- Draw weight and impatience
        fontLittle:PrintR(308, iY+4, format("%03u%%          %03u%%\n%03u%%         %05u\n%04u          %03u%%",
            aDigger.H, floor(aDigger.IW/aDigger.STR*100),
            floor(aDigger.JT/aDigger.PL*100),
            aDigger.DUG, aDigger.GEM, ceil(aDigger.LDT/iGameTicks*100)));
      -- Digger is dead
      else
        -- Draw grave icon
        texSpr:BlitSLT(319, 31, iY+8);
        -- Draw dead labels
        fontLittle:PrintR(308, iY+4, "---%          ---%\n---%         -----\n----          ---%");
      end
      -- Draw labels
      fontTiny:SetCRGB(0, 0.75, 1);
      fontTiny:SetLSpacing(2);
      fontTiny:PrintR(308, iY+5, "HEALTH:             WEIGHT:        \nIMPATIENCE:         GROADS DUG:        \nGEMS FOUND:         EFFICIENCY:        ");
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
    for Y = 38, 188, 15 do for X = 141, 291, 15 do
      texSpr:BlitSLT(864, X, Y);
    end end
    -- For each digger
    for iDigIndex = 1, #aActivePlayer.D do
      -- Calculate Y position
      local iY<const> = iDigIndex * 32;
      -- Print id number of digger
      fontLarge:Print(16, iY+8, iDigIndex);
      -- Draw colour key of digger
      texSpr:BlitSLT(858+iDigIndex, 31, iY+11);
      -- Draw X and Y letters
      fontTiny:SetCRGB(0, 0.75, 1);
      fontTiny:Print(64, iY+8, "X:       Y:");
      -- Draw health bar background
      texSpr:BlitSLTRB(1023, 24, iY+30, 124, iY+32);
      -- Get digger and if it exists?
      local aDigger<const> = aActivePlayer.D[iDigIndex];
      if aDigger then
        -- Draw digger health bar
        DrawHealthBar(aDigger.H, 1, 24, iY+30, 24, iY+32);
        -- Draw digger item data
        fontLittle:Print(72, iY+8,
          format("%04u  %04u\n\\%03u  \\%03u",
            aDigger.X, aDigger.Y, aDigger.AX, aDigger.AY));
        -- Draw digger portrait
        texSpr:BlitSLT(aDigger.S, 43, iY+8);
        -- Draw position of digger
        texSpr:BlitSLT(858+iDigIndex, 141+(aDigger.AX*1.25),
          38+(aDigger.AY*1.25));
      -- Digger is dead
      else
        -- Draw grave icon
        texSpr:BlitSLT(319, 43, iY+8);
        -- Draw dashes for unavailable digger item data
        fontLittle:Print(72, iY+8, "----  ----\n\\---  \\---");
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
      floor(iGameTicks/216000%24),
      floor(iGameTicks/3600%60),
      floor(iGameTicks/60%60)));
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
    fontLittle:PrintC(160,  146, "YOU HAVE RAISED "..aActivePlayer.M..
      " OF "..iWinLimit.." ZOGS");
    if aActivePlayer.M > aOpponentPlayer.M then
      fontLittle:PrintC(160, 154, "YOU HAVE THE MOST ZOGS");
      ScoreAP = ScoreAP + 1;
    elseif aActivePlayer.M < aOpponentPlayer.M then
      fontLittle:PrintC(160, 154, "YOUR OPPONENT HAS MORE ZOGS");
      ScoreOP = ScoreOP + 1;
    else
      fontLittle:PrintC(160, 154, "YOU AND YOUR OPPONENT HAVE EQUAL ZOGS");
    end
    fontLittle:PrintC(160, 162,
      "RAISE "..(iWinLimit-aActivePlayer.M).." MORE ZOGS TO WIN");
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
  if TIP then TIP = SetBottomRightTip(TIP) end;
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
    -- For each menu buttonc column
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
        TIP = aSMData[7] end;
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
      -- Blit active item
      texSpr:BlitSLT(aActiveObject.IS.S, iMenuLeft+23, iMenuTop+4);
      -- Draw info about item
      fontTiny:Print(iMenuLeft+17, iMenuTop+24, format("%03u%% %02u",
        aActiveObject.IS.H, aActiveObject.IS.OD.WEIGHT));
    -- Sell menu is open?
    elseif aActiveMenu == aMenuData[MNU.SELL] then
      -- Blit active item
      texSpr:BlitSLT(aActiveObject.IS.S, iMenuLeft+23, iMenuTop+4);
      -- Draw info about item
      fontTiny:Print(iMenuLeft+17, iMenuTop+24, format("%03uz %02u",
        aActiveObject.IS.OD.VALUE/2, aActiveObject.IS.OD.WEIGHT));
    end
  end
end
-- Apply objects inventory perks ------------------------------------------- --
local function ApplyObjectInventoryPerks(aObject)
  -- Get objects inventory and return if no inventory
  local aInventory<const> = aObject.I;
  if #aInventory == 0 then return end;
  -- Enumerate each one. Have to use 'while' as inventory can dissapear
  local iIndex = 1;
  while iIndex <= #aInventory do
    -- Get object in invntory and if item is first aid?
    local aItem<const> = aInventory[iIndex];
    if aItem.ID == TYP.FIRSTAID then
      -- Get carriers health and if carrier is damaged?
      local iHealth<const> = aObject.H;
      if iHealth > 0 and iHealth < 100 then
        -- Increase holder's health and decrease first aid health
        AdjustObjectHealth(aObject, 1);
        AdjustObjectHealth(aItem, -1);
      end
    end
    -- Item has health or couldn't drop object? Enumerate next item!
    if aItem.H > 0 or
       not DropObject(aObject, aItem) then iIndex = iIndex + 1 end;
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
  -- Return if object is not in water
  if not CheckObjectInWater(aObject, 2) then return end;
  -- If object is a digger and it isnt in danger? Run!
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
      -- Play sound for direction
      if aObject.AD.SOUND then PlaySoundAtObject(aObject, aObject.AD.SOUND);
      -- Sound with random pitch?
      elseif aObject.AD.SOUNDRP then
        PlaySoundAtObject(aObject, aObject.AD.SOUNDRP,
          0.975+(random()%0.05)) end;
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
-- Ceheck if object is colliding with another ------------------------------ --
local function CheckObjectCollision(aObject)
  -- Walk objects list
  for iIndex = 1, #aObjects do
    -- Get object
    local aTarget<const> = aObjects[iIndex];
    -- if target object...
    if aObject ~= aTarget and                -- ...is not me?
       aTarget.F & OFL.DIGGER ~= 0 and       -- -and- target is a digger?
       aTarget.F & OFL.WATERBASED == 0 and   -- -and- target is not waterbased?
       aObject.H > 0 and                     -- -and- source is alive?
       aTarget.H > 0 and                     -- -and- target is alive?
       aTarget.A ~= ACT.PHASE and            -- -and- target not teleporting?
       aTarget.A ~= ACT.HIDE and             -- -and- target not hidden?
       aObject.A ~= ACT.EATEN and            -- -and- source object not eaten?
       maskSpr:IsCollideEx(                  -- -and- target collides source?
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
        -- If object can phase the digger
        if aObject.F & OFL.PHASEDIGGER ~= 0 then
          -- Make object phase to some other object
          SetAction(aTarget, ACT.PHASE, JOB.PHASE, DIR.D);
        -- If object can heal the Digger?
        elseif aObject.F & OFL.HEALNEARBY ~= 0 then
          -- Increase health
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
            -- Object is not moving and direction set? Keep direction, set danger
            elseif aTarget.D ~= DIR.NONE then
              SetAction(aTarget, ACT.KEEP, JOB.INDANGER, DIR.KEEP);
            -- No direction? so run in opposite direction of object
            else SetAction(aTarget, ACT.RUN, JOB.INDANGER,
              GetTargetDirection(aTarget, aObject)) end;
          end
        -- Else if object...
        elseif aObject.F & OFL.DIGGER ~= 0 and -- Object touching is digger?
               aTarget.F & OFL.BUSY == 0 and   -- -and- Target object not busy?
               aObject.F & OFL.BUSY == 0 and   -- -and- Source object not busy?
               aObject.P ~= aTarget.P and      -- -and- not same owner?
               aTarget.A ~= ACT.JUMP and       -- -and- target not jumping
           not aObject.FT then                 -- -and- has no fight target?
          -- Make us face the target and fight and set objects fight target
          SetAction(aObject, ACT.FIGHT, JOB.INDANGER,
            GetTargetDirection(aObject, aTarget));
          aObject.FT = aTarget;
          -- Make target object face us and fight and set targets fight target
          SetAction(aTarget, ACT.FIGHT, JOB.INDANGER,
            GetTargetDirection(aTarget, aObject));
          aTarget.FT = aObject;
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
    -- Punch sprite?
    if aObject.ST == 1 then
      -- Deal damage equal to my strength
      AdjustObjectHealth(aObject.FT, aObject.STRP);
      if random(4) == 1 then PlaySoundAtObject(aObject, aSfxData.PUNCH) end;
    -- Kick sprite?
    elseif aObject.ST == 4 then
      -- Deal damage equal to my strength
      AdjustObjectHealth(aObject.FT, aObject.STRK);
      if random(4) == 1 then PlaySoundAtObject(aObject, aSfxData.KICK) end;
    end
    -- Set last hit tile
    aObject.LH = aObject.ST;
    -- Clear fight target
    aObject.FT = nil;
  -- No more fight targets so stop fighting if...
  elseif aObject.A == ACT.FIGHT  and     -- ...object is fighting?
         (aObject.F & OFL.DIGGER ~= 0 or -- -and- object is a Digger?
          aObject.AT >= 360) then        -- -or- action time 360 frames?
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
  -- OBject can move in requested direction
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
         aTarget.F & OFL.BLOCK == 0 and   -- -and- target object doesn't block?
         aTarget.A ~= ACT.PHASE and       -- -and- is not phasing?
         aTarget.A ~= ACT.DEATH and       -- -and- is not dying?
           maskSpr:IsCollideEx(aObject.S, -- -and- doesn't collide with object?
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
  -- Process find target logic --------------------------------------------- --
  local function AIFindTarget(aObject)
    -- Return if busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Object aquires a target or target's time expired?
    local aTarget = aObject.T;
    if aObject.AT > aTimerData.TARGETTIME or not aTarget then
      -- Holds potential targets
      local aTargets<const> = { };
      -- For each player
      for iPlayer = 1, #aPlayers do
        -- Get player data and enumerate their diggers
        local aDiggers<const> = aPlayers[iPlayer].D;
        for iDiggerId = 1, #aDiggers do
          -- Get digger object and insert it in target list if not hidden
          local aDigger<const> = aDiggers[iDiggerId];
          if aDigger and aDigger.A ~= ACT.HIDE then
            insert(aTargets, aDigger) end;
        end
      end
      -- Return if no targets found
      if #aTargets == 0 then return end;
      -- Pick a random target and set it
      aTarget = aTargets[random(#aTargets)];
      aObject.T = aTarget;
      -- Initially move in direction of target
      SetAction(aObject, ACT.KEEP, JOB.KEEP,
        GetTargetDirection(aObject, aTarget));
    end
    -- Return if no target
    if not aTarget then return end;
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
    -- Return if busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Move closer to selected target every even action frame
    if aObject.AT % 2 == 0 then AIFindTarget(aObject) end;
  end
  -- Returns if object has sellable items----------------------------------- --
  local function ObjectHasValuables(aObject)
    -- Get object inventory and enumerate it
    local aInventory<const> = aObject.I;
    for iInvIndex = 1, #aInventory do
      -- Get object in inventory. If is sellable regardless of owner?
      local aInventoryObject<const> = aInventory[iInvIndex];
      if aInventoryObject.F & OFL.SELLABLE ~= 0 and
        CanSellGem(aInventoryObject.ID) then return true end;
    end
    -- Nothing to sell
    return false;
  end
  -- Try to jump the object and return if we did --------------------------- --
  local function ObjectJumped(aObject)
    -- Return if digger is digging or digger has fall damage
    if aObject.J == JOB.DIG or aObject.FD > 0 then return false end;
    -- 50% chance to check to jump
    if random() < 0.5 then
      -- Object is moving left?
      if aObject.D == DIR.L or    -- Left? -or-
         aObject.D == DIR.UL or   -- Up-left? -or-
         aObject.D == DIR.DL then -- Down-left?
        -- Try to jump if 50% chance triggered and going left would block
        if IsCollide(aObject, -1, -2) and not IsCollide(aObject, -1, -16) then
          SetAction(aObject, ACT.JUMP, JOB.KEEP, DIR.KEEP) return true end;
      end
      -- Object is moving right?
      if aObject.D == DIR.R or    -- Right? -or-
         aObject.D == DIR.UR or   -- Up-right? -or-
         aObject.D == DIR.DR then -- Down-right?
        -- Try to jump if 50% chance triggered and going right would block
        if IsCollide(aObject, 1, -2) and not IsCollide(aObject, 1, -16) then
          SetAction(aObject, ACT.JUMP, JOB.KEEP, DIR.KEEP) return true end;
      end
    end
    -- Object didn't jump
    return false;
  end
  -- Digger AI choices ----------------------------------------------------- --
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
      [JOB.BOUNCE] = { [DIR.UL] = 0.001, [DIR.UR] = 0.001,  [DIR.L]  = 0.001,
                       [DIR.R]  = 0.001, [DIR.DL] = 0.02,   [DIR.D]  = 0.002,
                       [DIR.DR] = 0.02 },
      -- Job is digging?
      [JOB.DIG] = { [DIR.UL] = 0.002, [DIR.UR] = 0.002,  [DIR.L]  = 0.001,
                    [DIR.R]  = 0.001, [DIR.DL] = 0.02,   [DIR.D]  = 0.75,
                    [DIR.DR] = 0.05 },
      -- Job is digging down?
      [JOB.DIGDOWN] = { [DIR.D]  = 0.75 },
      -- Job is searching for treasure?
      [JOB.SEARCH] = { [DIR.L]  = 0.002, [DIR.R]  = 0.002 },
    -- Ai is running?
    }, [ACT.RUN] = {
      -- Job is bouncing around?
      [JOB.BOUNCE] = { [DIR.L]  = 0.01,  [DIR.R]  = 0.01 },
      -- Job is in danger?
      [JOB.INDANGER] = { [DIR.L]  = 0.002, [DIR.R]  = 0.002 },
    },
  };
  -- AI digger logic ------------------------------------------------------- --
  local function AIDiggerLogic(aObject)
    -- Return if busy
    if aObject.F & OFL.BUSY ~= 0 then return end;
    -- Teleport home to safely regain health if these conditions are met...
    if (aObject.F & OFL.FALL ~= 0 and  -- (Object is falling? -and-
        aObject.FD >= aObject.H and    --  Falling damage would kill? -and-
        random() < 0.5) or             --  50% chance of teleporting?) -or-
      ((aObject.H <= 10 or             -- ((Object about to die? -or-
        aObject.J == JOB.INDANGER) and --   Object is in danger?) -and-
        random() < 0.01) then          --  1% chance of teleporting?)
      return SetAction(aObject, ACT.PHASE, JOB.PHASE, DIR.U);
    end
    -- Teleport home to rest and sell items if these conditions are met...
    if ObjectIsAtHome(aObject) and -- Object is at their home point? -and-
      (aObject.IW > 0 or           -- (Digger is carrying something? -or-
       aObject.H < 75) and         --  Health is under 75%) -and-
       aObject.A == ACT.STOP and   -- Digger has stopped?
       aObject.D ~= DIR.R then     -- Digger hasn't teleported yet?
      -- Phase back into the game
      return SetAction(aObject, ACT.PHASE, JOB.PHASE, DIR.R);
    end
    -- Stop the Digger if needed so it can heal a bit
    if aObject.H <= 25 and           -- Below quarter health?
       aObject.A ~= ACT.STOP and     -- Not stopped?
       aObject.J ~= JOB.INDANGER and -- Not in danger?
       random() < 0.001 then         -- A small chance?
      return SetAction(aObject, ACT.STOP, JOB.NONE, DIR.NONE);
    end
    -- Wait longer if health is needed
    if aObject.H < 50 and        -- Below half health?
       aObject.A == ACT.STOP and -- Stopped?
       random() > 0.001 then     -- Very big chance?
      return;                    -- Do nothing else
    end
    -- Teleport home to sell items if these conditions are met...
    if random() < 0.001 and             -- 0.1% chance triggered? -and-
       aObject.A == ACT.STOP and        -- Digger is stopped?
       aObject.IW > 0 and               -- Digger has inventory?
       ObjectHasValuables(aObject) then -- Digger has sellable items?
      return SetAction(aObject, ACT.PHASE, JOB.PHASE, DIR.U);
    end
    -- If the time we last had a successful dig was a long time. The digger
    -- could be stuck, so teleport home.
    if iGameTicks - aObject.LDT >= 7200 and aObject.A == ACT.STOP then
      -- Reset last dig time
      aObject.LDT = iGameTicks;
      -- Teleport home in hope the digger will find a new direction
      return SetAction(aObject, ACT.PHASE, JOB.PHASE, DIR.U);
    end
    -- Digger is walking?
    if aObject.A == ACT.WALK then
      -- Every 1/2 sec and digger isnt searching? Try to pick up any treasure!
      if iGameTicks % 30 == 0 and aObject.J ~= JOB.SEARCH then
        PickupObjects(aObject, false)
      -- Check for jump and return if we did
      elseif ObjectJumped(aObject) then return end;
    -- Digger is running?
    elseif aObject.A == ACT.RUN then
      -- Check for jump and return if we did
      if ObjectJumped(aObject) then return end;
    end
    -- A 0.1% chance occured each frame?
    if random() < 0.001 then
      -- Get digger inventory and if we have inventory?
      local aInventory<const> = aObject.I;
      if #aInventory > 0 then
        -- Walk Digger inventory
        for iInvIndex = 1, #aInventory do
          -- Get inventory object and if it is not treasure?
          local aInvObj<const> = aInventory[iInvIndex];
          if aInvObj.F & OFL.TREASURE == 0 then
            -- Drop it
            DropObject(aObject, aInvObj);
            -- Do not drop anything else
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
        insert(aDirections, aPDData[3]) end;
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
    -- Pick a new direction from eligable directions
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
  -- AI id to function list ------------------------------------------------ --
  local aAIFuncs<const> = {
    [AI.NONE]        = false,            [AI.DIGGER]  = AIDiggerLogic,
    [AI.RANDOM]      = AIRandomLogic,    [AI.FIND]    = AIFindTarget,
    [AI.FINDSLOW]    = AIFindTargetSlow, [AI.CRITTER] = AIRoamNormal,
    [AI.CRITTERSLOW] = AIRoamSlow,       [AI.BIGFOOT]  = AIBigfoot,
  }; ----------------------------------------------------------------------- --
  local function CreateObject(iObjId, iX, iY, aParent)
    -- Check parameters
    assert(iObjId, "Object id not specified!");
    assert(iObjId >= 0, "Object id "..iObjId.." too low!");
    assert(iX ~= nil, "X co-ordinate not specified!");
    assert(iX >= 0, "X co-ordinate "..iX.." too low!");
    assert(iX < TotalLevelWidthPixel, "X co-ordinate "..iX.." too high!");
    assert(iY ~= nil, "Y co-ordinate not specified!");
    assert(iY >= 0, "Y co-ordinate "..iY.."  too low!");
    assert(iY < TotalLevelHeightPixel, "Y co-ordinate "..iY.." too high!");
    -- Return if too many objects
    if #aObjects >= 100000 then return end;
    -- Get and test object information
    local aObjData<const> = aObjectData[iObjId];
    if type(aObjData) ~= "table" then error("Object data for #"..
      iObjId.." not in objects lookup!") end;
    -- Get object name
    local sName<const> = aObjData.NAME
    if type(sName) ~= "string" then error("Object name for #"..
      iObjId.." not in object data table!") end;
    -- Get and test AI type
    local iAI<const> = aObjData.AITYPE;
    if type(iAI) ~= "number" then error("Invalid AI type #"..
      tostring(iAI).." for "..sName.."["..iObjId.."]!") end
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
      DUG  = 0,                          -- Successful dig count
      F    = aObjData.FLAGS or 0,        -- Object flags (OFL.*)
      FD   = 0,                          -- Amount the object has fallen by
      FDD  = DIR.NONE,                   -- Last failed dig direction
      FM   = aTimerData.FALLSPEED,       -- Fall check will go after this time
      FS   = 1,                          -- Object falling speed
      GEM  = 0,                          -- Gems found
      H    = 100,                        -- Object health
      I    = { },                        -- Inventory for this object
      ID   = iObjId,                     -- Object id (TYP.*)
      IS   = nil,                        -- Selected inventory item
      IW   = 0,                          -- Weight of inventory
      J    = JOB.NONE,                   -- Object job (JOB.*)
      JD   = { },                        -- Reference to job data
      JT   = 0,                          -- Job timer
      LC   = aObjData.LUNGS,             -- Lung capacity
      LDT  = iGameTicks,                 -- Last successful dig time
      LH   = 0,                          -- Last hit sprite
      OD   = aObjData,                   -- Object data table
      OFX  = 0, OFY  = 0,                -- Blitting offset
      OFXA = 0, OFYA = 0,                -- Attachment blitting offset
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
      STA  = 0,                          -- Attachment type
      STR  = aObjData.STRENGTH,          -- Object strength
      STRK = -ceil(aObjData.STRENGTH/4), -- Object damage on kick
      STRP = -ceil(aObjData.STRENGTH/2), -- Object damage on punch
      SX   = iX,                         -- Object starting position
      SY   = iY,                         -- Object starting position
      TD   = { },                        -- Ignored teleport destinations
      X    = 0,                          -- Absolute X position
      Y    = 0,                          -- Absolute Y position
    };
    -- Set AI function and check it
    aObject.AIF = aAIFuncs[iAI];
    assert(aObject.AIF ~= nil, "Unknown AI# "..tostring(iAI).."!");
    -- Set position of object
    SetPosition(aObject, iX, iY);
    -- Set default action (also sets default direction);
    SetAction(aObject, aObjData.ACTION, aObjData.JOB, aObjData.DIRECTION);
    -- Insert into main object array
    insert(aObjects, aObject);
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
-- Ceheck if object is falling --------------------------------------------- --
local function CheckObjectFalling(aObject)
  -- If object...
  if CheckObjectFloating(aObject) or -- ...is floating?
     aObject.F & OFL.FALL == 0 or    -- -or- is not supposed to fall?
     IsCollideY(aObject, 1) then     -- -or- blocked below?
    return false end;                -- Then the object is not falling!
  -- Start from 'fallspeed' pixels and count down to 1
  for iFallSpeed = aObject.FS, 1, -1 do
    -- No collision found with terrain?
    if not IsCollideY(aObject, iFallSpeed) then
      -- Move Y position down
      MoveY(aObject, iFallSpeed);
      -- Increase fall speed
      if aObject.FS < 5 then aObject.FS = aObject.FS + 1 end;
      -- Increase fall damage
      aObject.FD = aObject.FD + 1;
      -- break loop
      break;
    end
    -- Collision detected, but we must find how many pixels exactly to fall
    -- by, so reduce the pixel count and try to find the exact value.
  end
  -- Success
  return true;
end
-- Process all objects ----------------------------------------------------- --
local function GameProc()
  -- PROCESS TERRAIN ANIMATION --------------------------------------------- --
  if iGameTicks % aTimerData.ANIMTERRAIN == 0 then
    -- For each screen row we are looking at
    for iY = 0, TotalTilesHeightM1 do
      -- Calculate the Y position to grab from the level data
      local iYdest<const> = (iPosY + iY) * TotalLevelWidth;
      -- For each screen column we are looking at
      for iX = 0, TotalTilesWidthM1 do
        -- Get absolute position on map
        local iPos<const> = (iYdest + (iPosX + iX)) * 2;
        -- Get tile id and if tile is valid?
        local iTileId<const> = binLevel:RU16LE(iPos);
        if iTileId ~= 0 then
          -- Get tile flags and if flags say we should animate to next tile?
          local iFlags<const> = aTileData[iTileId+1];
          if iFlags & aTileFlags.AB ~= 0 then
            binLevel:WU16LE(iPos, iTileId + 1);
          -- Tile is end of animation so go back 3 sprites. This rule means that
          -- all animated terrain tiles must be 4 sprites.
          elseif iFlags & aTileFlags.AE ~= 0 then
            binLevel:WU16LE(iPos, iTileId - 3) end;
        end
      end
    end
  end
  -- PROCESS FLOOD DATA ---------------------------------------------------- --
  if #aFloodData > 0 then
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
        local iPosition<const> = iTilePos - 2;
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
              binLevel:WU16LE(iPosition, aFGDItem[1]);
              -- Continue flooding if needed
              if aFGDItem[2] then
                insert(aFloodData, { iPosition, iTileFlags }) end;
            -- Tile is not a flood gate tile?
            else
              -- Update tile to the same tile with water in it
              binLevel:WU16LE(iPosition, iTileId + 240);
              -- Continue flooding if the left edge of the tile is exposed
              if iTileFlags & aTileFlags.EL ~= 0 then
                insert(aFloodData, { iPosition, iTileFlags }) end;
            end
          end
        end
      end
      -- If tile flags exposed right?
      if iTileFlags & aTileFlags.ER ~= 0 then
        -- Get information about the tile on the right and if valid
        local iPosition<const> = iTilePos + 2;
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
              binLevel:WU16LE(iPosition, aFGDItem[1]);
              -- Continue flooding if data requests it
              if aFGDItem[2] then
                insert(aFloodData, { iPosition, iTileFlags }) end;
            -- Tile is not a flood gate tile?
            else
              -- Update tile to the same tile with water in it
              binLevel:WU16LE(iPosition, iTileId + 240);
              -- Continue flooding if the left edge of the tile is exposed
              if iTileFlags & aTileFlags.ER ~= 0 then
                insert(aFloodData, { iPosition, iTileFlags }) end;
            end
          end
        end
      end
      -- If tile flags exposed down?
      if iTileFlags & aTileFlags.EB ~= 0 then
        -- Get information about the tile below and if valid
        local iPosition<const> = iTilePos + TotalLevelWidthX2;
        local iTileId<const> = GetLevelDataFromLevelOffset(iPosition);
        if iTileId then
          -- Get file flags and if is water and the left edge is exposed?
          local iTileFlags<const> = aTileData[1 + iTileId];
          if iTileFlags & aTileFlags.W == 0 and
             iTileFlags & aTileFlags.ET ~= 0 then
            -- Update tile to the same tile with water in it and continue
            binLevel:WU16LE(iPosition, iTileId + 240);
            insert(aFloodData, { iPosition, iTileFlags });
          end
        end
      end
    end
  end
  -- PROCESS OBJECTS ------------------------------------------------------- --
  local iObjId = 1 while iObjId <= #aObjects do
    -- Get object data
    local O<const> = aObjects[iObjId];
    -- If AI function set, not dying and not phasing then call AI function!
    if O.AI ~= AI.NONE then O.AIF(O) end;
    -- If object can't fall or it finished falling and isn't floating?
    if not CheckObjectFalling(O) then
      -- If fall damage is set then object fell and now we must reduce its
      -- health
      if O.FD > 0 then
        -- Object still has fall flag set and object fell >= 16 pixels
        if O.F & OFL.FALL ~= 0 and O.FD >= 5 then
          -- Object is delicate? Remove more health
          if O.F & OFL.DELICATE ~= 0 then AdjustObjectHealth(O, -O.FD);
          else AdjustObjectHealth(O, -floor(O.FD/2)) end;
          -- Damage would reduce health < 10 %. Stop object moving
          if O.H > 0 and O.H < 10 then
            SetAction(O, ACT.STOP, JOB.INDANGER, DIR.NONE);
          end
        end
        -- Reset fall damage;
        O.FD = 0;
      end
      -- Reset fall speed;
      O.FS = 1;
      -- Object is jumping?
      if O.F & OFL.JUMPRISE ~= 0 then
        -- Object can move up and the rise limit hasnt been reached yet
        if not IsCollideY(O, -1) and O.AT < #aJumpRiseData then
          -- We can actually move up? Move up!
          if aJumpRiseData[O.AT+1] > 0 then MoveY(O, -1) end;
        -- Object cannot move up or action timer is the last frame
        else
          -- Remove rising and set falling flags
          O.F = (O.F | OFL.JUMPFALL) & ~OFL.JUMPRISE;
          -- Reset action timer
          O.AT = 0;
        end
      -- Object is falling (during the jump)?
      elseif O.F & OFL.JUMPFALL ~= 0 then
        -- Object can fall down and the fall limit hasnt been reached yet
        if not IsCollideY(O, 1) and O.AT < #aJumpFallData then
          -- And we can drop down? Drop down
          if aJumpFallData[O.AT+1] > 0 then MoveY(O, 1) end;
        -- Object cannot move down or action timer is the last frame
        else
          -- Let object fall normally now and remove busy and falling flags
          O.F = (O.F | OFL.FALL) & ~(OFL.JUMPFALL | OFL.BUSY);
          -- Thus little tweak makes sure the user can't jump again by just
          -- modifying the fall speed as ACT.JUMP requires it to be 1.
          O.FS = 2;
          -- Reset action timer
        end
      end
      -- Object is...
      if O.A == ACT.DEATH and                    -- ...dead?
         O.AT >= aTimerData.DEADWAIT and         -- ...death timer exceeded?
         DestroyObject(iObjId, O) then goto EOO; -- ...object destroyed?
      -- Object is phasing and phase delay reached?
      elseif O.A == ACT.PHASE and O.AT >= O.OD.TELEDELAY then
        -- Object is phasing home
        if O.J == JOB.PHASE then
          -- If this object is selected
          if aActiveObject == O and (not O.P or O.P ~= aActivePlayer) then
            -- Deselect it. Opponents are not allowed to see where they went!
            aActiveObject = nil
          end
          -- Direction set to the right to sell all items?
          if O.D == DIR.R then
            -- Hide the digger
            SetAction(O, ACT.HIDE, JOB.PHASE, DIR.R);
            -- Number of items sold
            local iItemsSold = 0;
            -- Get object inventory and if inventory held
            local aInventory<const> = O.I;
            if #aInventory > 0 then
              -- Get owner of this object
              local aParent<const> = O.P;
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
                  SellItem(O, aInvObj) then iItemsSold = iItemsSold + 1;
                -- Conditions fail so try next inventory item.
                else iObj = iObj + 1 end;
              end
              -- If items were sold?
              if iItemsSold > 0 then
                -- If object...
                if #aInventory > 0 and                -- Still has inventory?
                   random() < 0.01 and                -- Triggers 1% chance?
                   O.P.M > aActivePlayer.M + 400 then -- Way more cash?
                  -- Purchase something random to keep the scores fair
                  BuyItem(O, aShopData[random(#aShopData)])
                end
                -- Check if any player won
                EndConditionsCheck();
              end
            end
          -- Phase home
          elseif O.D == DIR.U then
            -- Reduce health as a cost for teleporting
            if O.F & OFL.TPMASTER == 0 then AdjustObjectHealth(O, -5) end;
            -- Go home?
            local bGoingHome = true;
            -- Is teleport master?
            local bIsTeleMaster<const> = O.F & OFL.TPMASTER ~= 0;
            -- For each object
            for TI = 1, #aObjects do
              -- Get object
              local TO<const> = aObjects[TI];
              -- If object is a telepole and object is owned by this object
              -- or object is a teleport master and telepole status nominal?
              if TO.ID == TYP.TELEPOLE and
                 (TO.P == O.P or bIsTeleMaster) and
                 TO.A == ACT.STOP then
                -- Ignore telepole?
                local bIgnoreTelepole = false;
                -- Enumerate teleport destinations...
                local aDestinations<const> = O.TD;
                for IDI = 1, #aDestinations do
                  -- Get destination and set true if we've been here before
                  local aDestination<const> = aDestinations[IDI];
                  if aDestination == TO then bIgnoreTelepole = true break end;
                end
                -- Don't ignore telepole?
                if not bIgnoreTelepole then
                  -- Teleport to this device
                  SetPosition(O, TO.X, TO.Y);
                  -- Re-phase back into stance
                  SetAction(O, ACT.PHASE, JOB.NONE, DIR.NONE);
                  -- Add it to ignore teleport destinations list for next time.
                  insert(O.TD, TO);
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
              O.TD = { };
              -- Set position of object to player's home
              SetPosition(O, O.P.HX, O.P.HY);
              -- Re-phase back into stance
              SetAction(O, ACT.PHASE, JOB.NONE, DIR.KEEP);
            end
          -- Going into trade centre
          elseif O.D == DIR.UL then
            -- Now in trade centre
            SetAction(O, ACT.HIDE, JOB.PHASE, DIR.R);
            -- Set new active object to this one
            aActiveObject, aActiveMenu = O, nil;
            -- We don't want to hear sounds
            PlaySoundAtObject = UtilBlank;
            -- Init lobby
            InitLobby(aActiveObject, false, 1);
          -- Phase random target?
          elseif O.D == DIR.D or O.D == DIR.DR then
            -- Valid targets
            local TA = { };
            -- Walk objects list
            for TI = 1, #aObjects do
              -- Get object and if the object isnt this object and object is
              -- a valid phase target? Insert into valid phase targets list.
              local TO<const> = aObjects[TI];
              if TO ~= O and TO.F & OFL.PHASETARGET ~= 0 then
                insert(TA, TO);
              end
            end
            -- List has items?
            if #TA > 0 then
              -- Pick random object from array
              TA = TA[random(#TA)];
              -- Set this objects position to the object
              SetPosition(O, TA.X, TA.Y);
              -- Re-phase back into stance
              SetAction(O, ACT.PHASE, JOB.NONE, DIR.KEEP);
            -- Else if object has owner, teleport home
            elseif O.P then
              -- Set position of object to player's home
              SetPosition(O, O.P.HX, O.P.HY);
              -- Re-phase back into stance
              SetAction(O, ACT.PHASE, JOB.NONE, DIR.KEEP);
            -- No owner = no home, just de-phase
            else SetAction(O, ACT.STOP, JOB.NONE, DIR.KEEP) end;
          end
        -- Object has finished phasing?
        else
          -- Object was teleported eaten? Respawn as eaten!
          if O.D == DIR.DR then SetAction(O, ACT.EATEN, JOB.NONE, DIR.LR);
          -- Object not eaten?
          else SetAction(O, ACT.STOP, JOB.NONE, DIR.KEEP) end;
        end
      -- Object is hidden and object is in the trade-centre?
      elseif O.A == ACT.HIDE and O.J == JOB.PHASE then
        -- Health at full?
        if O.H >= 100 then
          -- Re-appear the object if is not the player otherwise wait for the
          -- real player to exit the trade centre.
          if O.P ~= aActivePlayer then
            SetAction(O, ACT.PHASE, JOB.NONE, DIR.R) end;
        -- Health not full? Regenerate health
        elseif O.AT % 10 == 0 then AdjustObjectHealth(O, 1) end;
      -- Object has been eaten
      elseif O.A == ACT.EATEN and O.AT >= aTimerData.MUTATEWAIT then
        -- Kill digger and spawn alien
        AdjustObjectHealth(O, -100);
        CreateObject(TYP.ALIEN, O.X, O.Y);
      -- Object is dying? Slowly drain it's health
      elseif O.A == ACT.DYING and O.AT % 6 == 0 then
        AdjustObjectHealth(O, -1);
      -- Object is creeping, walking and running? Limit FPS depending on action
      elseif (O.A == ACT.CREEP and O.AT % 4 == 0) or
             (O.A == ACT.WALK and O.AT % 2 == 0) or
              O.A == ACT.RUN then
        -- Object wants to dig down and object X position is in the middle of
        -- the tile? Make object dig down
        if O.J == JOB.DIGDOWN and O.X % 16 == 0 then
          SetAction(O, ACT.DIG, JOB.DIGDOWN, DIR.D);
        -- Object wants to enter the trading centre? Stop object
        elseif O.J == JOB.HOME and ObjectIsAtHome(O) then
          -- Go into trade centre
          SetAction(O, ACT.PHASE, JOB.PHASE, DIR.UL);
        -- Object is for rails only and train is not on track
        elseif O.F & OFL.TRACK ~= 0 then
          -- Get X pos adjust depending on direction
          local iId;
          if O.D == DIR.L then iId = 7 else iId = 9 end;
          -- Get absolute tile position and get tile id
          local iLoc<const> = GetLevelOffsetFromObject(O, iId, 0);
          if iLoc then
            iId = binLevel:RU16LE(iLoc);
            -- Tile at end of track? Stop!
            if aTileData[1+iId] & aTileFlags.T == 0 then
              SetAction(O, ACT.STOP, JOB.NONE, DIR.NONE);
            -- Move train
            elseif O.D == DIR.L then MoveX(O,-1);
            elseif O.D == DIR.R then MoveX(O, 1) end;
          end
        else
          -- If timer goes over 1 second and object is busy
          -- Unset busy flag as abnormal digging can make it stick
          if O.AT >= 60 and O.F & OFL.BUSY ~= 0 then O.F = O.F & ~OFL.BUSY end;
          -- Compare direction. Object is moving up? Move object up
          if O.D == DIR.U then MoveY(O, -1);
          -- Object is moving down? Move object down
          elseif O.D == DIR.D then MoveY(O, 1);
          -- Object is moving up-left, left or down-left? Move object left
          elseif O.D == DIR.UL or O.D == DIR.L or O.D == DIR.DL then
            MoveX(O, -1);
          -- Object is moving up-right, right or down-right? Move object right
          elseif O.D == DIR.UR or O.D == DIR.R or O.D == DIR.DR then
            MoveX(O, 1);
          end
        end
      -- Object is digging and digging delay reached?
      elseif O.A == ACT.DIG and O.AT >= O.OD.DIGDELAY then
        -- Terrain dig was successful?
        if DigTile(O) == true then
          -- Continue to walk in the direction
          SetAction(O, ACT.WALK, JOB.KEEP, DIR.KEEP);
          -- Increase dig count
          SetObjectAndParentCounter(O, "DUG");
          -- Update dig time for AI
          O.LDT = iGameTicks;
        -- Not successful
        else
          -- Update failed dig direction
          O.FDD = O.D;
          -- stop the object's actions completly
          SetAction(O, ACT.STOP, JOB.NONE, DIR.KEEP);
        end;
      end
      -- Post action job process. Object search for other objects to pickup?
      if O.J == JOB.SEARCH and O.AT % aTimerData.PICKUPDELAY == 0 then
        -- Pickup nearest treasure
        PickupObjects(O, true);
      -- Object is in danger and danger timeout is reached?
      elseif O.J == JOB.INDANGER and O.AT >= aTimerData.DANGERTIMEOUT then
        -- Remove the objects job
        SetAction(O, ACT.KEEP, JOB.NONE, DIR.KEEP);
      end
      -- Object can regenerate health? Regenerate health!
      if O.F & OFL.REGENERATE ~= 0 and
         O.AT % O.SM == O.SMM1 then AdjustObjectHealth(O, 1) end;
    end
    -- If object is not phasing or dying?
    if O.A ~= ACT.PHASE and O.A ~= ACT.DEATH then
      -- Check object fighting
      CheckObjectCollision(O);
      -- Check if object is underwater
      CheckObjectUnderwater(O);
      -- Walk object's inventory
      ApplyObjectInventoryPerks(O);
    end
    -- Process animations
    AnimateObject(O);
    -- Increase action and job timer
    O.AT, O.JT = O.AT + 1, O.JT + 1;
    -- Process next object
    iObjId = iObjId + 1;
    -- Label for skipping to next object
    ::EOO::
  end
  -- PROCESS VIEWPORT ------------------------------------------------------ --
  if iPosX ~= PosAX then AdjustViewPortX((PosAX - iPosX) * 0.75) end;
  if iPosY ~= PosAY then AdjustViewPortY((PosAY - iPosY) * 0.75) end;
  -- MONEY LOGIC ----------------------------------------------------------- --
  if iGameTicks % 18000 == 0 then
    -- Move first gem to last gem
    insert(aGemsAvailable, aGemsAvailable[1]);
    remove(aGemsAvailable, 1);
  end
  -- If we have an active player
  if aActivePlayer then
    -- Animated money is different from actual money?
    if Money ~= aActivePlayer.M then
      -- Animated money over actual money?
      if Money > aActivePlayer.M then
        -- Decrement it
        Money = Money - ceil((Money - aActivePlayer.M) * 0.1);
        -- Red colour
        fontLittle:SetCRGB(1, 0.75, 0.75);
        -- Update displayed money
        sMoney = format("%04u", min(9999, Money));
      -- Animated money under actual money? Increment
      elseif Money < aActivePlayer.M then
        -- Increment it
        Money = Money + ceil((aActivePlayer.M - Money) * 0.1);
        -- Green colour
        fontLittle:SetCRGB(0.75, 1, 0.75);
        -- Update displayed money
        sMoney = format("%04u", min(9999, Money));
      end
    -- Animated money/actual money is synced, display blue if > 9999
    elseif aActivePlayer.M > 9999 then fontLittle:SetCRGB(0.75, 0.75, 1);
    -- Normal display
    else fontLittle:SetCRGB(1, 1, 1) end;
  end
  -- OTHER LOGIC ----------------------------------------------------------- --
  iGameTicks = iGameTicks + 1;
end
-- Get mouse position on level --------------------------------------------- --
local function GetAbsMousePos()
  return floor(GetMouseX()-iStageL+PosPX), floor(GetMouseY()-iStageT+PosPY);
end
-- Finds object under mouse cursor ----------------------------------------- --
local function FindObjectUnderCursor()
  -- Translate current mouse position to absolute level position
  local nMouseX<const>, nMouseY<const> = GetAbsMousePos();
  -- Walk through objects in backwards order. This is because ojects are
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
  iMenuRight, iMenuBottom = aActiveMenu[1]*16, aActiveMenu[2]*16;
  -- Update left and top co-ordinates
  iMenuLeft, iMenuTop = UtilClamp(X, iStageL, iStageR-iMenuRight),
                        UtilClamp(Y, iStageT, iStageB-iMenuBottom);
  -- Update right and bottom co-ordinates
  iMenuRight, iMenuBottom = iMenuRight+iMenuLeft, iMenuBottom+iMenuTop;
end
-- Update object menu position with current mouse -------------------------- --
local function UpdateMenuPositionAtMouseCursor()
  UpdateMenuPosition(GetMouseX(), GetMouseY());
end
-- Set active obejct menu -------------------------------------------------- --
local function SetMenu(Id, UpdatePos)
  -- Set new menu and check that we got the data for it
  aActiveMenu = aMenuData[Id];
  assert(aActiveMenu, "No menu data for '"..Id.."'!");
  -- Update menu position if requested
  if UpdatePos then UpdateMenuPositionAtMouseCursor();
  -- Just update menu size
  else UpdateMenuPosition(iMenuLeft, iMenuTop) end;
end
-- Open context menu for object -------------------------------------------- --
local function OpenObjectMenu()
  -- Get menu data
  local MID<const> = aActiveObject.OD.MENU;
  -- Object has menu and object belongs to active player and object isn't
  -- dead or eaten?
  if MID and aActiveObject.P == aActivePlayer and aActiveObject.A ~= ACT.DEATH
    and aActiveObject.A ~= ACT.EATEN then
    -- Object does belong to active player so play context menu sound and
    -- set the appropriate default menu for the object.
    PlayStaticSound(aSfxData.CLICK);
    SetMenu(MID, true);
    -- Success
    return true;
  -- Object does not belong to active player? Play error sound and return fail
  else PlayStaticSound(aSfxData.ERROR) return false end;
end
-- Process context menu item button click----------------------------------- --
local function ProcessMenuClick()
  -- Calculate button pressed and absolute index
  local iIndex<const> =
    floor((GetMouseY() - iMenuTop) / 16) * aActiveMenu[1] +
    floor((GetMouseX() - iMenuLeft) / 16) + 1;
  -- Get menu data for id
  local aMIData<const> = aActiveMenu[3][iIndex];
  assert(aMIData, "No data for menu item "..iIndex.."!");
  -- If tile denies if object is busy? Disallow action and play sound
  if aMIData[2] & MFL.BUSY ~= 0 and aActiveObject.F & OFL.BUSY ~= 0 then
    PlayStaticSound(aSfxData.ERROR);
  -- New menu specified? Set new menu and play sound
  elseif aMIData[3] ~= MNU.NONE then
    SetMenu(aMIData[3], false);
    PlayStaticSound(aSfxData.SELECT);
  -- New action specified? Set new action and play sound
  elseif aMIData[4] ~= 0 and aMIData[5] ~= 0 and aMIData[6] ~= 0 then
    SetAction(aActiveObject, aMIData[4], aMIData[5], aMIData[6], true);
    PlayStaticSound(aSfxData.SELECT);
  end
end
-- Player clicked in the numbered digger buttons --------------------------- --
local function ProcessDiggerButtonClick()
  -- Get which button was clicked
  local iButtonId<const> = floor((GetMouseX() - 144) / 16) + 1;
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
      insert(aObjects, aObject);
      -- Set as active ojbect
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
        iKeyRight<const>,  iKeyEscape<const>,    iKeyT<const>,
        iKeyHome<const>,   iKeyEnd<const>,       iKeyDelete<const>,
        iKeyPgDown<const>, iKeyReturn<const>,    iKeyF5<const>,
        iKeyF6<const>,     iKeyF7<const>,        iKeyF8<const>,
        iKeyE<const>,      iKeySpacebar<const> =
          aKeys.ONE,       aKeys.TWO,       aKeys.THREE,
          aKeys.FOUR,      aKeys.FIVE,      aKeys.KP_0,
          aKeys.KP_1,      aKeys.KP_2,      aKeys.KP_3,
          aKeys.KP_4,      aKeys.KP_5,      aKeys.MINUS,
          aKeys.EQUAL,     aKeys.BACKSPACE, aKeys.KP_ENTER,
          aKeys.UP,        aKeys.DOWN,      aKeys.LEFT,
          aKeys.RIGHT,     aKeys.ESCAPE,    aKeys.T,
          aKeys.HOME,      aKeys.END,       aKeys.DELETE,
          aKeys.PAGE_DOWN, aKeys.ENTER,     aKeys.F5,
          aKeys.F6,        aKeys.F7,        aKeys.F8,
          aKeys.E,         aKeys.SPACE;
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
    PlaySoundAtObject = UtilBlank;
    -- Init the book
    InitBook(true);
  end
  -- Select digger if active ----------------------------------------------- --
  local function SelectDigger(iDigger)
    -- Get specified digger and return if not found
    local aDigger<const> = aActivePlayer.D[iDigger];
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
        aActivePlayer.D[(((iCurrentDigger + iNegate) - 1) % 5) + 1];
      if aDigger then return SelectObject(aDigger) end;
    end
  end
  -- Adjust viewport and prevent scroll ------------------------------------ --
  local function AdjustViewPortXNS(iX) AdjustViewPortX(iX) PosAX = iPosX end;
  local function AdjustViewPortYNS(iY) AdjustViewPortY(iY) PosAY = iPosY end;
  -- We'll set ProcInput to this so we can keep aliases local to this routine.
  local function ProcInputInitialised()
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
    if IsKeyPressed(iKeyMinus) or IsButtonPressed(6) then
      return SelectAdjacentDigger(-1) end;
    -- Right button pressed?
    if IsKeyPressed(iKeyEquals) or IsButtonPressed(7) then
      return SelectAdjacentDigger(1) end;
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
      -- If walking right then set digger to righ right
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
    -- Test mode enabled?
    if GetTestMode() then
      -- Check if Jennite key pressed
      if IsKeyPressed(iKeyT) then
        return CreateObject(TYP.JENNITE,
          UtilClampInt(floor((iPosX*16)+GetMouseX()-iStageL),
            0, TotalLevelWidthPixelM1),
          UtilClampInt(floor(iPosY*16 + GetMouseY()),
            0, TotalLevelHeightPixelM1)) end;
      -- Check if explosion key pressed
      if IsKeyPressed(iKeyE) then
        return AdjustObjectHealth(CreateObject(TYP.TNT,
          UtilClampInt(floor((iPosX*16)+GetMouseX()-iStageL),
            0, TotalLevelWidthPixelM1),
          UtilClampInt(floor(iPosY*16 + GetMouseY()),
            0, TotalLevelHeightPixelM1)), -100) end;
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
  -- Deinit/Reset current level
  DeInitLevel();
  -- Set FBU callback
  RegisterFBUCallback("game", FrameBufferUpdated);
  -- Set level number and get data for it.
  local aLevelInfo;
  if type(Id) == "table" then iLevelId, aLevelInfo = 0, Id;
  elseif type(Id) == "number" then
    if Id < 0 then iLevelId = #aLevelData;
    elseif Id > #aLevelData then iLevelId = 0;
    else iLevelId = Id end;
    aLevelInfo = aLevelData[iLevelId];
    assert(type(aLevelInfo)=="table", "No level info for level "..iLevelId);
  else error("Invalid id!") end;
  -- Get level type data and level type
  local aLevelTypeData<const> = aLevelInfo.t;
  local iLevelType<const> = aLevelTypeData.i;
  -- Set level name and level type name
  sLevelName, sLevelType = aLevelInfo.n, aLevelTypeData.n;
  -- Assets to load
  local sLevelFile<const> = aLevelInfo.f;
  local sLevelTexture<const> = aLevelTypeData.f;
  local aToLoad<const> = {
    {T=5,F="lvl/"..sLevelFile..".dat",P={ }},
    {T=5,F="lvl/"..sLevelFile..".dao",P={ }},
    {T=1,F=sLevelTexture,             P={16,16,0,0,0}},
  }
  -- Add game music to load list?
  if Music then insert(aToLoad, {T=7,F=Music,P={}}) end
  -- Level assets loaded function
  local function OnLoaded(aRes)
    -- Set level data handles
    binLevel = aRes[1].H;
    -- Set level objects handle
    local aLevelObj<const> = aRes[2].H;
    -- Set textures and masks
    texLev = aRes[3].H;
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
    maskZone = MaskCreateNew(TotalLevelWidthPixel, TotalLevelHeightPixel);
    -- Get minimum and maximum object id
    local iMinObjId<const>, iMaxObjId<const> = TYP.JENNITE, TYP.MAX;
    -- Player starting point data found
    local aPlayersFound<const> = { };
    -- For each row in the data file
    for iY = 0, TotalLevelHeightM1 do
      -- Calculate precise Y position for object
      local iPreciseY<const> = iY * 16;
      -- Calculate row position in data file
      local iDataY<const> = iY * TotalLevelWidth;
      -- For each column in the data file
      for iX = 0, TotalLevelWidthM1 do
        -- Calculate position of tile
        local iPosition<const> = (iDataY + iX) * 2;
        -- Calculate precise X position for object
        local iPreciseX<const> = iX * 16;
        -- Get the 16-bit (little-endian) tile id from the terrain data at
        -- the current position and if it is not a clear tile?
        local TerrainId<const> = binLevel:RU16LE(iPosition);
        if TerrainId >= 0 and TerrainId < #aTileData then
          -- Check if is a player
          for iPlayer = 1, #aPlayerStartData do
            -- Get player data and check for player starting position
            local aPlayerStartItem<const> = aPlayerStartData[iPlayer];
            if TerrainId >= aPlayerStartItem[1] and
               TerrainId <= aPlayerStartItem[2] then
              -- Get existing player data and if player exists?
              local aPlayerFound<const> = aPlayersFound[iPlayer];
              if aPlayerFound then
                -- Show map maker in console that the player exists
                ConsoleWrite("Warning! Player "..iPlayer..
                  " already exists! X="..iX..", Y="..iY..", Abs="..
                  iPosition..". Originally found at X="..aPlayerFound[1]..
                  ", Y="..aPlayerFound[2]..".", 9);
              -- Player doesn't exist? Set the new player
              else aPlayersFound[iPlayer] =
                { iX, iY, aPlayerStartItem[3], aPlayerStartItem[4] }
              end
            end
          end
          -- Draw the appropriate tile for the level bitmask
          maskZone:Copy(maskLev, TerrainId, iPreciseX, iPreciseY);
        -- Show error. Level could be corrupted
        else error("Error! Invalid tile "..TerrainId.."/"..#aTileData..
          " at X="..iX..", Y="..iY..", Abs="..iPosition.."!") end;
        -- We need to check for objects at the same position now too. We
        -- can store two items on one 16-bit tile...
        for iPos = iPosition, iPosition + 1 do
          -- Get object id at position and if it's interesting?
          local iObjId<const> = aLevelObj:RU8(iPos);
          if iObjId ~= 0 then
            if iObjId < iMinObjId or iObjId >= iMaxObjId then
              -- Show map maker in console that the objid is invalid
              ConsoleWrite("Warning! Object id invalid! Id="..
                iObjId..", X="..iX..", Y="..iY..", Abs="..
                iPosition..", Slot="..iPos..", Max="..iMaxObjId..".", 9);
            -- Object id is valid? Create the object
            elseif not CreateObject(iObjId, iPreciseX, iPreciseY) then
              -- Show map maker in console that the objid is invalid
              ConsoleWrite("Warning! Couldn't create object! Id="..
                iObjId..", X="..iX..", Y="..iY..", Abs="..
                iPosition..", Slot="..iPos..", Max="..iMaxObjId..".", 9);
            end
          end
        end
      end
    end
    -- Make sure we found two players
    assert(#aPlayersFound == 2, #aPlayersFound.."/2 players!");
    -- Fill border with 1's to prevent objects leaving the playfield
    maskZone:Fill(0, 0, TotalLevelWidthPixel, 1);
    maskZone:Fill(0, 0, 1, TotalLevelHeightPixel);
    maskZone:Fill(TotalLevelWidthPixelM1, 0, 1, TotalLevelHeightPixel);
    maskZone:Fill(0, TotalLevelHeightPixelM1, TotalLevelWidthPixel, 1);
    -- Reset races available
    for iI = 1, #aRaceData do insert(aRacesAvailable, aRaceData[iI]) end;
    -- Create player for each starting position found
    for iPlayerId = 1, #aPlayersFound do
      local aPlayerData<const> = aPlayersFound[iPlayerId];
      CreatePlayer(aPlayerData[1], aPlayerData[2], iPlayerId,
                   aPlayerData[3], aPlayerData[4]);
    end
    -- Active player set?
    if aActivePlayer then
      -- Set player race and level if not set (gam_test used)
      if not aGlobalData.gSelectedRace then
        aGlobalData.gSelectedRace = aActivePlayer.R end;
      if not aGlobalData.gSelectedLevel then
        aGlobalData.gSelectedLevel = Id end;
    end
    -- Reset gems available
    local iGemStart<const> = random(#aDigTileData);
    for iId = 1, #aDigTileData do insert(aGemsAvailable,
      aDigTileData[1 + ((iGemStart + iId) % #aDigTileData)]) end;
    -- Play in-game music if requested
    if Music then PlayMusic(aRes[4].H, 0) end;
    -- Now we want to hear sounds if humanplayer set
    if P1AI == false then PlaySoundAtObject = DoPlaySoundAtObject end;
    -- Computer is main player?
    if P1AI == true then
      -- Set impossible win limit
      iWinLimit = maxinteger;
      -- Set auto-respawn on all objects death
      for iI = 1, #aObjects do
        local aObject<const> = aObjects[iI];
        aObject.F = aObject.F | OFL.RESPAWN;
      end
    -- Set actual winlimit
    else iWinLimit = aLevelInfo.w + aGlobalData.gCapitalCarried end;
    -- Do one tick at least or the fade will try to render with variables
    -- that haven't been initialised yet
    TP();
    -- Do fade then set requested game callbacks
    local function OnFadeIn() SetCallbacks(TP, TR, TI) end;
    Fade(1, 0, 0.04, TR, OnFadeIn, not not Music);
  end
  -- Load level graphics resources asynchronously
  LoadResources(sLevelName, aToLoad, OnLoaded);
end
-- Load infinite play ------------------------------------------------------ --
local function LoadInfinitePlay(iId)
  -- Infinite play tick callback
  local function OnTick()
    -- Switch object every 10 seconds
    if iGameTicks % 600 == 0 and #aPlayers > 0 then
      -- Select a player
      local aPlayer<const> = aPlayers[random(#aPlayers)];
      -- Find a digger from the specified player.
      local aObject = aPlayer.D[random(aPlayer.DT)];
      -- Still not found? Find a random object
      if not aObject then aObject = aObjects[random(#aObjects)] end;
      -- Select the object if we got something!
      if aObject then SelectObject(aObject) end;
    end
    -- Init game proc
    GameProc();
  end
  -- Infinite play render callback
  local function OnRender()
    -- Render terrain
    RenderTerrain();
    -- Render game objects
    RenderObjects();
  end
  -- Infinite play tick callback
  local function OnTickRandom()
    -- Switch level every 5 minutes? Fade out to next random level
    if iGameTicks % 18000 == 17999 then
      Fade(0, 1, 0.04, OnRender, LoadInfinitePlay);
    end
    -- Switch object every 10 seconds
    OnTick();
  end
  -- Callback to use
  local fcbCallback;
  -- If level selected
  if iId then fcbCallback = OnTick;
  else iId, fcbCallback = random(#aLevelData), OnTickRandom end;
  -- Load infinite play (AI vs AI)
  LoadLevel(iId, "game", nil, true, nil, true, fcbCallback, OnRender,
    UtilBlank);
  -- Set sound to play
  PlaySoundAtObject = DoPlaySoundAtObject
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
-- ------------------------------------------------------------------------- --
local function InitContinueGame(bMusic, aObject)
  -- Post loaded
  local function PostLoaded()
    -- Set arrow
    SetCursor(aCursorIdData.ARROW);
    -- We want to hear sounds
    PlaySoundAtObject = DoPlaySoundAtObject;
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
  if bMusic then
    return LoadResources("Continue", {{T=7,F="game",P={}}}, OnLoaded) end;
  -- Run post loaded functions
  PostLoaded();
end
-- Returns level data binary handle ---------------------------------------- --
local function GetLevelData()
  -- Make sure it's available
  assert(binLevel, "Level data not loaded!");
  assert(binLevel:Size() > 0, "Level data empty!");
  -- Return level data
  return binLevel;
end
-- ------------------------------------------------------------------------- --
local function GetActivePlayer() return aActivePlayer end;
local function GetOpponentPlayer() return aOpponentPlayer end;
-- Exports and imports ----------------------------------------------------- --
return { A = {                         -- Exports
  -- Exports --------------------------------------------------------------- --
  LoadLevel = LoadLevel, GetLevelData = GetLevelData, aObjects = aObjects,
  GemsAvailable = aGemsAvailable, EndConditionsCheck = EndConditionsCheck,
  DeInitLevel = DeInitLevel, BuyItem = BuyItem,
  GetGameTicks = GetGameTicks, GetAbsMousePos = GetAbsMousePos,
  HaveZogsToWin = HaveZogsToWin, GameProc = GameProc,
  RenderInterface = RenderInterface, RenderTerrain = RenderTerrain,
  RenderObjects = RenderObjects, SellSpecifiedItems = SellSpecifiedItems,
  GetCapitalValue = GetCapitalValue, InitContinueGame = InitContinueGame,
  SelectObject = SelectObject, IsSpriteCollide = IsSpriteCollide,
  AdjustViewPortX = AdjustViewPortX, AdjustViewPortY = AdjustViewPortY,
  FrameBufferUpdated = FrameBufferUpdated, CreateObject = CreateObject,
  AdjustObjectHealth = AdjustObjectHealth, aPlayers = aPlayers,
  GetActivePlayer = GetActivePlayer, GetOpponentPlayer = GetOpponentPlayer,
  ApplyObjectInventoryPerks = ApplyObjectInventoryPerks,
  LoadInfinitePlay = LoadInfinitePlay
  -- ----------------------------------------------------------------------- --
  }, F = function(GetAPI)              -- Imports
  -- Imports --------------------------------------------------------------- --
  TYP, aLevelData, LoadResources, aObjectData, ACT, JOB, DIR, aTimerData, AI,
  OFL, aDigTileData, PlayMusic, aTileData, aTileFlags, Fade, BCBlit,
  SetCallbacks, IsMouseInBounds, aDigData, DF, aSfxData, aJumpRiseData,
  aJumpFallData, IsKeyPressed, IsButtonPressed, IsKeyHeld, IsButtonHeld,
  GetMouseX, GetMouseY, PlayStaticSound, PlaySound, IsButtonPressedNoRelease,
  aMenuData, MFL, MNU, IsMouseXLessThan, InitBook, aObjToUIData, GetMusic,
  StopMusic, RenderFade, IsJoyPressed, InitWin, InitWinDead, InitLose,
  InitLoseDead, InitTNTMap, InitLobby, texSpr, fontLarge, fontLittle, fontTiny,
  aDigBlockData, aExplodeDirData, SetCursor, aCursorIdData,
  RegisterFBUCallback, GetCallbacks, GetTestMode, RenderShadow,
  SetBottomRightTip, aRaceData, aDugRandShaftData, aFloodGateData,
  aTrainTrackData, aExplodeAboveData, maskLev, maskSpr, aGlobalData, aShopData
  = -- --------------------------------------------------------------------- --
  GetAPI("aObjectTypes", "aLevelData", "LoadResources", "aObjectData",
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
    "RenderShadow", "SetBottomRightTip", "aRaceData", "aDugRandShaftData",
    "aFloodGateData", "aTrainTrackData", "aExplodeAboveData", "maskLevel",
    "maskSprites", "aGlobalData", "aShopData");
  -- ----------------------------------------------------------------------- --
  CreateObject = InitCreateObject();
  MoveOtherObjects = InitMoveOtherObjects();
  -- ----------------------------------------------------------------------- --
end };
-- ------------------------------------------------------------------------- --
end                                    -- End of GameLuaModule namespace
-- ------------------------------------------------------------------------- --
return GameLuaModule();                -- Return module parameters
-- End-of-File ============================================================= --
