--
--  Author: Scott Simpson
--  Version 2.1.17
--
-- 	This program is free software: you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation, either version 3 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  A copy of the GNU General Public License is available at <http://www.gnu.org/licenses/>.
--    
-------------------------------------------------------------------------------------------------------------------------------------
--
local debugLabelWidth = 90
local debugRowHeight = 7
local debugColWidth = 106

local function printFrSkyData(col, row, label, name)
    local val = getValue(name)
	local x = (col - 1) * debugColWidth
    local y = row * debugRowHeight - 6
    lcd.drawText(x, y, label, SMLSIZE)
    if val ~= nil then
      lcd.drawText(x + debugLabelWidth - 20, y, val, SMLSIZE)
    end
end

local function printEncodedExtensionData(col, row, label, name)
    local val = extensionValue[name]
    val = decode100(val)
	local x = (col - 1) * debugColWidth
    local y = row * debugRowHeight - 6
    lcd.drawText(x, y, label, SMLSIZE)
    if val ~= nil then
      lcd.drawText(x + debugLabelWidth - 20, y, val, SMLSIZE)
    end
end

local function printExtensionData(col, row, label, name)
    local val = extensionValue[name]
	local x = (col - 1) * debugColWidth
    local y = row * debugRowHeight - 6
    lcd.drawText(x, y, label, SMLSIZE)
    if val ~= nil then
      lcd.drawText(x + debugLabelWidth - 20, y, val, SMLSIZE)
    end
end

local function printGpsData(col, row, label, name)
    local val = gpsData[name]
	local x = (col - 1) * debugColWidth
    local y = row * debugRowHeight - 6
    lcd.drawText(x, y, label, SMLSIZE)
    if val ~= nil then
      lcd.drawText(x + debugLabelWidth - 20, y, val, SMLSIZE)
    end
end

local function init()
end

local function background() 
end

local function run(event)
	lcd.clear()
    
	printExtensionData(1, 1, "sequence", "sequence")
	printExtensionData(1, 2, "cog", "cog")
	printExtensionData(1, 3, "hdop", "hdop")
	printExtensionData(1, 4, "fix_type", "fix_type")
	printExtensionData(1, 5, "sats_visible", "sats_visible")
	printExtensionData(1, 6, "base_mode", "base_mode")
	printExtensionData(1, 7, "custom_mode", "custom_mode")
	printExtensionData(1, 8, "bar_altitude", "bar_altitude")
    
	printExtensionData(2, 1, "ranger_dist", "ranger_dist")
	printExtensionData(2, 2, "batt_remain", "batt_remain")	   
	printExtensionData(2, 3, "armed_distance", "armed_distance")
  printExtensionData(2, 4, "armed_bearing", "armed_bearing")	  
  printExtensionData(2, 5, "mav_consumed", "mav_consumed")
  printExtensionData(2, 6, "calc_consumed", "calc_consumed")  
  printExtensionData(2, 7, "dist_travelled", "dist_travelled")
end

return {init=init, run=run, background=background}