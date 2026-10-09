-- Задача 6: «віртуальна прив'язка» на Lua (без перезбирання прошивки).
-- Куди покласти:
--   SITL: ~/ardupilot/scripts/distance_guard.lua   (папка scripts поруч з місцем запуску arducopter)
--   Реальна плата: APM/scripts/ на SD-карті
-- Увімкнути: SCR_ENABLE=1, перезапустити SITL.
--
-- Раз на секунду: пише висоту і відстань до home у GCS;
-- якщо дрон далі ніж MAX_DIST_M — RTL.

local MAX_DIST_M  = 3000      -- метри
local MODE_RTL    = 6         -- номери режимів Copter (див. 01_teoriya.md)
local MODE_LAND   = 9
local MAV_INFO    = 6
local MAV_WARNING = 4

local tick = 0

function update()
  tick = tick + 1

  if not arming:is_armed() then
    return update, 1000
  end

  local loc  = ahrs:get_location()
  local home = ahrs:get_home()
  if not loc or not home then
    return update, 1000
  end

  local dist = loc:get_distance(home)
  local alt  = loc:alt() * 0.01 - home:alt() * 0.01   -- см -> м, над home (AMSL - AMSL)

  if tick % 5 == 0 then
    gcs:send_text(MAV_INFO, string.format("LUA: dist=%.0fm alt=%.1fm", dist, alt))
  end
  gcs:send_named_float("LUA_DIST", dist)

  local mode = vehicle:get_mode()
  if dist > MAX_DIST_M and mode ~= MODE_RTL and mode ~= MODE_LAND then
    gcs:send_text(MAV_WARNING, string.format("LUA: %.0fm > %dm -> RTL", dist, MAX_DIST_M))
    vehicle:set_mode(MODE_RTL)
  end

  return update, 1000
end

gcs:send_text(MAV_INFO, "LUA distance_guard loaded")
return update, 1000
