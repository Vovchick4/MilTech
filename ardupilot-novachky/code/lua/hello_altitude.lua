-- Урок 9. Перший Lua-скрипт, який працює ВСЕРЕДИНІ автопілота.
-- Раз на 5 секунд пише в Mission Planner (вкладка Messages) висоту і режим.
-- Тексти — англійською: повідомлення автопілота мають ліміт 50 байт і кирилицю
-- Mission Planner показує «кракозябрами».
--
-- Куди покласти (SITL):  ~/ardupilot/scripts/hello_altitude.lua
-- Увімкнути: параметр SCR_ENABLE = 1, потім перезапустити SITL.

local MAV_INFO = 6   -- «рівень важливості» повідомлення: 6 = інформація

function update()
  local loc  = ahrs:get_location()   -- де ми зараз
  local home = ahrs:get_home()       -- де дім

  if loc and home and ahrs:home_is_set() then
    -- висота в сантиметрах над рівнем моря -> метри над домом
    local altitude = (loc:alt() - home:alt()) / 100
    gcs:send_text(MAV_INFO, string.format("Hello! Alt %.1f m, mode %d", altitude, vehicle:get_mode()))
  else
    gcs:send_text(MAV_INFO, "Hello! Waiting for GPS...")
  end

  return update, 5000   -- запустити update() знову через 5000 мс = 5 с
end

gcs:send_text(MAV_INFO, "hello_altitude started")
return update, 1000
