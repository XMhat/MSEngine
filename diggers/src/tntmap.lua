-- TNTMAP.LUA ============================================================== --
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
local TextureCreateTS<const>, ImageRaw<const>, AssetCreate<const>
      = -- ----------------------------------------------------------------- --
      Texture.CreateTS, Image.Raw, Asset.Create;
-- Diggers function and data aliases --------------------------------------- --
local LoadResources, Fade, SetCallbacks, IsMouseInBounds, IsMouseNotInBounds,
  aCursorIdData, SetCursor, aSfxData, PlayStaticSound, aSfxData, IsButtonPressed,
  IsButtonHeld, RenderFade, aTileData, aTileFlags, InitTitle, texSpr,
  SetBottomRightTip, RenderInterface, GameProc, InitContinueGame, GetLevelData,
  aObjects, RenderShadow;
-- Init TNT map screen function -------------------------------------------- --
local function InitTNTMap()
  -- On assets loaded event
  local function OnLoaded(aResources)
    -- Set tntmap graphic
    local texTNTMap = aResources[1].H;
    texTNTMap:SetTile(0, 208, 312, 512, 512);
    -- Calculate bitmap size (24-bit RGB colours)
    local iBSize<const> = 128 * 128 * 3;
    -- Create storage for bitmap data (128x128xRGB)
    local asBData<const> = AssetCreate(iBSize)
    -- Get water tile flags
    local iWFlags<const> = aTileFlags.W;
    -- Get solid tile flags
    local iSFlags<const> = aTileFlags.D + aTileFlags.AD;
    -- Get level data
    local binLevel<const> = GetLevelData();
    -- For each pixel row
    for iY = 0, 127 do
      -- Calculate Y position in destination bitmap
      local iBYPos<const> = (iBSize - ((iY + 1) * 384)) + 3;
      -- Calculate Y position from level data
      local iLYPos<const> = iY * 256;
      -- For each pixel column
      for iX = 0, 127 do
        -- Get tile at level position
        local iTId<const> = binLevel:RU16LE(iLYPos + (iX * 2));
        -- Get tile flags
        local iTFlags<const> = aTileData[1 + iTId];
        -- Get bitmap position and then the locations of the components
        local iBPos<const> = iBYPos + (iX * 3);
        local iBPosGB<const> = iBPos - 2; -- Green/Blue (16-bit write)
        local iBPosR<const> = iBPos - 3; -- Red (8-bit write)
        -- Pixel values to write (Green/Blue + Red)
        local iIGB, iIR;
        -- Tile is clear?
        if iTId == 0 then iIGB, iIR = 0xFACE, 0x87;
        -- Tile is solid diggable block?
        elseif iTId == 3 then iIGB, iIR = 0x1A7F, 0x00;
        -- Tile is water?
        elseif iTFlags & iWFlags ~= 0 then iIGB, iIR = 0xFF00, 0x00;
        -- Tile is not destructable and not dug?
        elseif iTFlags & iSFlags == 0 then iIGB, iIR = 0x7F70, 0x7F;
        -- Other tiles
        else iIGB, iIR = 0x0000, 0x00 end;
        -- Apply pixel
        asBData:WU16LE(iBPosGB, iIGB);
        asBData:WU8(iBPosR, iIR);
      end
    end
    -- For each object, treat as POI
    for iObjectId = 1, #aObjects do
      -- Get object
      local aObj<const> = aObjects[iObjectId];
      -- Get position in pixel in bitmap for object
      local iPos<const> =
        (iBSize - ((aObj.AY + 1) * 384)) + (aObj.AX * 3) + 3;
      -- Make a white RGB dot
      asBData:WU16LE(iPos - 2, 0xFFFF);
      asBData:WU8(iPos - 3, 0xFF);
    end
    -- Push array as texture
    local texTerrain = TextureCreateTS(ImageRaw("TNTMap", asBData,
      128, 128, 24, 0x1907), 128, 64, 0, 0, 0);
    -- POI testing
    local function MouseOverExit()
      return IsMouseNotInBounds(8, 8, 312, 208) end;
    local function MouseOverUpButton()
      return IsMouseInBounds(140, 179, 156, 195) end;
    local function MouseOverDownButton()
      return IsMouseInBounds(162, 179, 178, 195) end;
    local function MouseOverTerrain()
      return IsMouseInBounds(32, 44, 288, 172) end;
    -- Current tip
    local sTip;
    -- Default top page
    local iTerrainPage = 0;
    -- Set tip and cursor
    local function SetTipAndCursor(sNewTip, iCursorId)
      sTip = sNewTip;
      SetCursor(iCursorId);
    end
    -- Set map page
    local function SetMapPage(iPage)
      -- Ignore if already on this p age
      if iTerrainPage == iPage then return end;
      -- Play sound
      PlayStaticSound(aSfxData.SELECT);
      -- Set page
      iTerrainPage = iPage;
    end
    -- Input callback
    local function InputProc()
      -- Check where mouse is and show appropriate tip
      if MouseOverExit() then
        -- Set tip and cursor to go exit
        SetTipAndCursor("CONTINUE", aCursorIdData.EXIT);
        -- Mouse button clicked? Return!
        if IsButtonHeld(0) then
          -- Play sound
          PlayStaticSound(aSfxData.SELECT);
          -- Unreference assets for garbage collector
          texTNTMap, texTerrain = nil, nil;
          -- Start the loading waiting procedure
          SetCallbacks(GameProc, RenderInterface, nil);
          -- Continue game
          return InitContinueGame(false);
        end
      -- Mouse over up button and on first page?
      elseif MouseOverUpButton() and iTerrainPage == 1 then
        -- Set tip and cursor to go up
        SetTipAndCursor("UP", aCursorIdData.SELECT);
        -- Mouse button clicked? Set top half of map
        if IsButtonHeld(0) then SetMapPage(0) end;
      -- Mouse over down button and on first page?
      elseif MouseOverDownButton() and iTerrainPage == 0 then
        -- Set tip and cursor to go down
        SetTipAndCursor("DOWN", aCursorIdData.SELECT);
        -- Mouse button clicked? Set bottom half of map
        if IsButtonHeld(0) then SetMapPage(1) end;
      -- Mouse over something else?
      else SetTipAndCursor("TNT MAP", aCursorIdData.ARROW) end;
    end
    -- Render callback
    local function RenderProc()
      -- Render everything
      RenderInterface();
      -- Render a fade
      RenderFade(0.5);
      -- Blit appropriate background
      texTNTMap:BlitLT(8, 8);
      -- Render shadow
      RenderShadow(8, 8, 312, 208);
      -- Blit terrain
      texTerrain:BlitSLTRB(iTerrainPage, 32, 44, 288, 172);
      -- Dim appropriate button
      if iTerrainPage == 0 then
        texSpr:BlitSLTRB(801, 140, 179, 157, 196);
      elseif iTerrainPage == 1 then
        texSpr:BlitSLTRB(801, 162, 179, 179, 196);
      end
      -- Draw tip
      SetBottomRightTip(sTip);
    end
    -- Set callbacks
    SetCallbacks(GameProc, RenderProc, InputProc);
  end
  -- Load tntmap resources
  LoadResources("TNT Map", {{T=2,F="tntmap",P={0}}}, OnLoaded);
end
-- Exports and imports ----------------------------------------------------- --
return { A = { InitTNTMap = InitTNTMap }, F = function(GetAPI)
  -- Imports --------------------------------------------------------------- --
  LoadResources, SetCallbacks, SetCursor, aCursorIdData, aSfxData,
  PlayStaticSound, Fade, InitTitle, IsButtonHeld, RenderFade, IsMouseInBounds,
  IsMouseNotInBounds, aTileData, aTileFlags, texSpr, SetBottomRightTip,
  RenderInterface, InitContinueGame, GameProc, GetLevelData, aObjects,
  RenderShadow
  = -- --------------------------------------------------------------------- --
  GetAPI("LoadResources", "SetCallbacks", "SetCursor", "aCursorIdData",
   "aSfxData", "PlayStaticSound", "Fade", "InitTitle", "IsButtonHeld",
   "RenderFade", "IsMouseInBounds", "IsMouseNotInBounds", "aTileData",
   "aTileFlags", "texSpr", "SetBottomRightTip", "RenderInterface",
   "InitContinueGame", "GameProc", "GetLevelData", "aObjects", "RenderShadow");
  -- ----------------------------------------------------------------------- --
end };
-- End-of-File ============================================================= --
