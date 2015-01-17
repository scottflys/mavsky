--
--  Copyright (c) Scott Simpson
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

local debugLabelWidth = 60
local debugRowHeight = 7
local debugColWidth = 68

local function printData(col, row, label, token)
	local val = getValue(token)
	local x = (col - 1) * debugColWidth
    local y = row * debugRowHeight - 6
    lcd.drawText(x, y, label, SMLSIZE)
    lcd.drawText(x + debugLabelWidth - 20, y, val, SMLSIZE)
end

local function printNum(col, row, label, token, precision)
	local val = getValue(token)
    val = math.floor(val * precision) / precision
	local x = (col - 1) * debugColWidth
    local y = row * debugRowHeight - 6
    lcd.drawText(x, y, label, SMLSIZE)
    lcd.drawText(x + debugLabelWidth - 20, y, val, SMLSIZE)
end
		 
local function background() 
end

local function run(event)
	lcd.clear()

	printData(1, 1, "a1", "a1")
	printData(1, 2, "a2", "a2")
	printData(1, 3, "a3", "a3")
	printData(1, 4, "a4", "a4")
	printData(1, 5, "accx", "accx")
	printData(1, 6, "accy", "accy")
	printData(1, 7, "accz", "accz")
	printData(1, 8, "air-spd", "air-speed")

	printData(2, 1, "alt", "altitude")
	printData(2, 2, "consump", "consumption")
	printData(2, 3, "current", "current")
	printData(2, 4, "curr-max", "current-max")
	printData(2, 5, "distance", "distance")
	printData(2, 6, "dte", "dte")
	printData(2, 7, "fuel", "fuel")
	printData(2, 8, "gps-alt", "gps-altitude")

	printData(3, 1, "gps-spd", "gps-speed")	
	printData(3, 2, "heading", "heading")	
	printNum(3, 3, "lat", "latitude", 10000)	
	printNum(3, 4, "long", "longitude", 10000)	
	printData(3, 5, "power", "power")	
	printData(3, 6, "vert-spd", "vertical-speed")	
	printData(3, 7, "temp1", "temp1")	
    printData(3, 8, "temp2", "temp2")

end

return {run=run, background=background}