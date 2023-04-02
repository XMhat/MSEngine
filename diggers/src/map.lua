-- MAP.LUA ================================================================= --
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
local insert<const>, pairs<const> = table.insert, pairs;
-- M-Engine function aliases ----------------------------------------------- --
local UtilClamp<const>, InfoTicks<const> = Util.Clamp, Info.Ticks;
-- Diggers function and data aliases --------------------------------------- --
local LoadResources, Fade, SetCallbacks, IsButtonReleased, IsMouseInBounds,
  IsMouseNotInBounds, aCursorIdData, SetCursor, PlayStaticSound,
  aSfxData, InitCon, aZoneData, IsMouseXGreaterEqualThan,
  IsMouseYGreaterEqualThan, GetMouseX, GetMouseY, IsButtonPressed,
  IsMouseXLessThan, IsMouseYLessThan, InitLobby, SetBottomRightTipAndShadow,
  aLevelData, RegisterFBUCallback, aGlobalData;
-- Init zone selection screen function ------------------------------------- --
local function InitMap()
  -- Reset tip
  local sTip = "";
  -- On loaded
  local function OnLoaded(aRes)
    -- Register framebuffer update
    local iStageL, iStageT;
    local function OnFrameBufferUpdate(...)
      local _ _, _, iStageL, iStageT, _, _ = ...;
    end
    RegisterFBUCallback("map", OnFrameBufferUpdate);
    -- Variables
    local iMapSizeX<const>, iMapSizeY<const> = 640, 350;
    local iZonePosX, iZonePosY = 0, 0;
    local aZoneCache<const>, aFlagCache<const> = { }, { };
    -- Get zone
    local function GetZone()
      -- Get mouse position on whole map adjusted by current map scroll
      local iX<const> = GetMouseX() + iZonePosX + (-iStageL);
      local iY<const> = GetMouseY() + iZonePosY;
      -- For each map data available
      for iZoneId = 1, #aZoneData do
        -- Get map data;
        local aZoneItem<const> = aZoneData[iZoneId];
        -- Mouse cursor inside zone boundry?
        if iX >= aZoneItem[1] and iY >= aZoneItem[2] and
           iX <  aZoneItem[3] and iY <  aZoneItem[4] then
          -- Return cache'd info of zone
          return aZoneCache[iZoneId];
        end
      end
    end
    -- Set texture handles
    local texZone1 = aRes[1].H;
    texZone1:TileS(0, 0, 0, 512, 350);
    local iTileFlagTexId<const> = texZone1:TileA(512, 0, 544, 32);
    local texZone2 = aRes[2].H;
    texZone2:TileS(0, 0, 0, 128, 350);
    -- Render the map
    local function RenderMap()
      -- Draw main chunk of map
      texZone1:BlitLT(-iZonePosX+iStageL, -iZonePosY+iStageT);
      -- Right chunk of map would be visible to player?
      if iZonePosX >= 128+iStageL then
        -- Draw right chunk of map
        texZone2:BlitLT(512+-iZonePosX+iStageL, -iZonePosY+iStageT);
      end
      -- For each flag data in flag cache
      for iFlagId = 1, #aFlagCache do
        -- Get flag data
        local aFlagData<const> = aFlagCache[iFlagId];
        -- Draw the flag to say the level was completed
        texZone1:BlitSLT(iTileFlagTexId, aFlagData[1]-(iZonePosX+(-iStageL)),
                                         aFlagData[2]-(iZonePosY+(-iStageT)));
      end
      -- Draw tip
      if sTip then SetBottomRightTipAndShadow(sTip) end;
    end
    -- Rebuild flag data cache
    for iZoneId in pairs(aGlobalData.gLevelsCompleted) do
      local aZoneItem<const> = aZoneData[iZoneId];
      local iFlagX<const>, iFlagY<const> = aZoneItem[5], aZoneItem[6];
      insert(aFlagCache, { iFlagX, iFlagY, iFlagX+32, iFlagY+32 });
    end
    -- Rebuild zone data cache
    for iZoneId = 1, #aZoneData do
      local aZoneItem<const> = aZoneData[iZoneId];
      local iZoneCompleted;
      if not aGlobalData.gLevelsCompleted[iZoneId] then
        local aDep<const> = aZoneItem[7];
        if #aDep ~= 0 then
          for iDepId = 1, #aDep do
            local iDepZone<const> = aDep[iDepId];
            if aGlobalData.gLevelsCompleted[iDepZone] then
              iZoneCompleted = iZoneId;
              break;
            end
          end
        else iZoneCompleted = iZoneId end;
      end
      insert(aZoneCache, iZoneCompleted or false);
    end
    -- On map faded in
    local function OnFadeIn()
      -- Input procedure
      local function InputMap()
        -- Get maximums
        local iZoneMaxX<const>, iZoneMaxY<const> =
          320+(iStageL*2), 110+(iStageT*2);
        -- Compare map position
        if iZonePosX > 0 and IsMouseXLessThan(16) then
          SetCursor(aCursorIdData.LEFT);
          iZonePosX = UtilClamp(iZonePosX - 8, 0, iZoneMaxX);
        elseif iZonePosX < iZoneMaxX and IsMouseXGreaterEqualThan(304) then
          SetCursor(aCursorIdData.RIGHT);
          iZonePosX = UtilClamp(iZonePosX + 8, 0, iZoneMaxX);
        elseif iZonePosY > 0 and IsMouseYLessThan(16) then
          SetCursor(aCursorIdData.TOP);
          iZonePosY = UtilClamp(iZonePosY - 8, 0, iZoneMaxY);
        elseif iZonePosY < iZoneMaxY and IsMouseYGreaterEqualThan(224) then
          SetCursor(aCursorIdData.BOTTOM);
          iZonePosY = UtilClamp(iZonePosY + 8, 0, iZoneMaxY);
        else
          -- Get zone the mouse cursor is under and if found?
          local iZone<const> = GetZone();
          if iZone then
            -- Set select cursor
            SetCursor(aCursorIdData.SELECT);
            -- Button was pressed?
            if IsButtonPressed(0) then
              -- Set new selected level
              aGlobalData.gSelectedLevel = iZone;
              -- Play sound
              PlayStaticSound(aSfxData.SELECT);
              -- When screen has faded out
              local function OnFadeOut()
                -- Remove FBO update callback
                RegisterFBUCallback("map");
                -- Unreference assets to garbage collector
                texZone1, texZone2 = nil, nil;
                -- Init controller screen
                InitCon();
              end
              -- Start fading out
              Fade(0, 1, 0.04, RenderMap, OnFadeOut);
            end
            -- Get information about selected zone
            local aLevel<const> = aLevelData[iZone];
            -- Get current time (rotated every 60 frames/1 second)
            local iTicks<const> = InfoTicks() % 180;
            -- Show name of zone at 0 to 1 seconds
            if     iTicks <  60 then sTip = aLevel.n;
            -- Show terrain type at 1 to 2 seconds
            elseif iTicks < 120 then sTip = aLevel.t.n;
            -- Show zogs needed to win at 2 to 3 seconds
            else sTip = aLevel.w.." TO WIN" end;
            -- Done
            return;
          -- No valid zone selected so set arrow cursor
          else SetCursor(aCursorIdData.ARROW) end;
          -- Tell user to put mouse over a valid zone
          sTip = "SELECT ZONE";
        end
      end
      -- Set map callbacks
      SetCallbacks(nil, RenderMap, InputMap);
    end
    -- Change render procs
    Fade(1, 0, 0.04, RenderMap, OnFadeIn);
  end
  -- Load texture resource
  LoadResources("Map Select", {{T=2,F="mapa",P={0}},
                               {T=2,F="mapb",P={0}}}, OnLoaded);
end
-- Exports and imports ----------------------------------------------------- --
return { A = { InitMap = InitMap }, F = function(GetAPI)
  -- Imports --------------------------------------------------------------- --
  LoadResources, SetCallbacks, SetCursor, IsMouseInBounds, IsButtonReleased,
  PlayStaticSound, Fade, InitLobby, IsMouseNotInBounds, aCursorIdData,
  aSfxData, InitCon, aZoneData, IsMouseXGreaterEqualThan,
  IsMouseYGreaterEqualThan, GetMouseX, GetMouseY, IsButtonPressed,
  IsMouseXLessThan, IsMouseYLessThan, SetBottomRightTipAndShadow, aLevelData,
  RegisterFBUCallback, aGlobalData
  = -- --------------------------------------------------------------------- --
  GetAPI("LoadResources", "SetCallbacks", "SetCursor", "IsMouseInBounds",
    "IsButtonReleased", "PlayStaticSound", "Fade", "InitLobby",
    "IsMouseNotInBounds", "aCursorIdData", "aSfxData", "InitCon", "aZoneData",
    "IsMouseXGreaterEqualThan", "IsMouseYGreaterEqualThan", "GetMouseX",
    "GetMouseY", "IsButtonPressed", "IsMouseXLessThan", "IsMouseYLessThan", "SetBottomRightTipAndShadow", "aLevelData",
    "RegisterFBUCallback", "aGlobalData");
  -- ----------------------------------------------------------------------- --
end };
-- End-of-File ============================================================= --
