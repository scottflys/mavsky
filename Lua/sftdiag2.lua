--
--  Author: Scott Simpson
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

local debugLabelWidth = 90
local debugRowHeight = 7
local debugColWidth = 106

initialize()

local function printFrSkyData(col, row, label, name)
    local val = getValue(name)
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

local function background() 
end

local function run(event)
	lcd.clear()

    checkForExtensionMessage()
    loadGpsData()
    
	printGpsData(1, 2, "lat", "lat")
	printGpsData(1, 3, "lon", "lon")        
end

return {run=run}