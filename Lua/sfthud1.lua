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
--  imperialUnits = 0 for metric
--  imperialUnits = 1 for imperial
--
local imperialUnits = 0
--
--
--  vehicleIsPlane = 0 for copter
--  vehicleIsPlane = 1 for plane
--
local vehicleIsPlane = 0
--
--
-- Set Slot value to position the field on the screen
-- Slot = 0 to disable
-- Slot = 1 for top left
-- Slot = 2 for middle left
-- Slot = 3 for bottom left
-- Slot = 4 for top right
-- Slot = 5 for middle right
-- Slot = 6 for bottom right
--
local Slot = {}
--
Slot["BatteryVoltage"] = 1
Slot["Current"] = 2
Slot["TotalCurrent"] = 3
Slot["Speed"] = 4
Slot["Altitude"] = 5
Slot["Distance"] = 6

Slot["VerticalSpeed"] = 0
Slot["Heading"] = 0
Slot["DistanceTravelled"] = 0
  
-------------------------------------------------------------------------------------------------------------------------------------    
--
--  GV to use for armed status (default 0 = GV1)
--   -1 - not available
--    0 - Disarmed
--    1 - Armed
-- 
gvArmed = 0
--
-------------------------------------------------------------------------------------------------------------------------------------    
--
--  GV to use for flight mode number (default 1 = GV2)
--   -1 - not available
--    0 - Stabilize
--    1 - Acro
--    2 - Altitude Hold
--  > 2 - see mission planner full parameter list
--
gvFlightMode = 1
--
-------------------------------------------------------------------------------------------------------------------------------------    
--
--  GV to use for new message severity (default 2 = GV3)
--   -1 - no message
--    0 - emergency - System is unusable. This is a "panic" condition
--    1 - alert 	  - Action should be taken immediately. Indicates error in non-critical systems
--    2 - crititcal - Action must be taken immediately. Indicates failure in a primary system
--    3 - error     - Indicates an error in secondary/redundant systems
--    4 - warning   - Indicates an error in secondary/redundant systems
--    5 - notice    - An unusual event has occured, though not an error condition. This should be investigated for the root cause
--    6 - info      - Normal operational messages. Useful for logging. No action is required for these messages
--    7 - debug     - Useful non-operational messages that can assist in debugging. These should not occur during normal operation
--
gvMessageSeverity = 2
--
-------------------------------------------------------------------------------------------------------------------------------------    
--
--  GV to use for HDOP number (default 3 = GV4)
--   -1 - not available
--    n - where n = HDOP value * 10
--
gvHdop = 3
--
-------------------------------------------------------------------------------------------------------------------------------------    
--
--  GV to use for sequence number (default 4 = GV5)
--   -1 - not available
--    n - where n = sequence number
--
gvSequence = 4
--
-------------------------------------------------------------------------------------------------------------------------------------    

local gpsName = "GPS"  
local rpmName = "RPM"
local vfasName = "VFAS"
local vspdName = "VSpd"
local currentName = "Curr"
local altitudeName = "Alt"
local speedName = "GSpd"
local headingName = "Hdg"            
local rssiName = "RSSI"            

FlightMode = {}
local AsciiMap={}
local Position = {}

local messageBuffer = ""
local messageBufferSeverity = 0
local messageBufferSize = 0
local previousMessageWord = 0
local messageLatestTimestamp = 0

MESSAGEBUFFERSIZE = 5
messageArray = {}
messageSeverityArray = {}
messageFirst = 0
messageNext = 0

extensionValue = {}
gpsData = {}

local function init()
  local i  
  for i=1, 18 do
      FlightMode[i] = {}
  end

  model.setGlobalVariable(gvArmed, 0, -1)
  model.setGlobalVariable(gvFlightMode, 0, -1)
  model.setGlobalVariable(gvMessageSeverity, 0, -1)
  model.setGlobalVariable(gvHdop, 0, -1)

	if vehicleIsPlane == 0 then
		FlightMode[1].Name="Stabilize"
		FlightMode[2].Name="Acro"
		FlightMode[3].Name="Altitude Hold"
		FlightMode[4].Name="Auto"
		FlightMode[5].Name="Guided"
		FlightMode[6].Name="Loiter"
		FlightMode[7].Name="Return to launch"
		FlightMode[8].Name="Circle"
		FlightMode[9].Name="Invalid Mode"
		FlightMode[10].Name="Land"
		FlightMode[11].Name="Optical Loiter"
		FlightMode[12].Name="Drift"
		FlightMode[13].Name="Invalid Mode"
		FlightMode[14].Name="Sport"
		FlightMode[15].Name="Flip Mode"
		FlightMode[16].Name="Auto Tune"
		FlightMode[17].Name="Position Hold"
		FlightMode[18].Name="Unknown"
  else
		FlightMode[1].Name="Manual"
		FlightMode[2].Name="Circle"
		FlightMode[3].Name="Stabilize"
		FlightMode[4].Name="Training"
		FlightMode[5].Name="Acro"
		FlightMode[6].Name="FBWA"
		FlightMode[7].Name="FBWB"
		FlightMode[8].Name="Cruise"
		FlightMode[9].Name="Autotune"
		FlightMode[10].Name="Unused1"
		FlightMode[11].Name="Auto"
		FlightMode[12].Name="RTL"
		FlightMode[13].Name="Loiter"
		FlightMode[14].Name="Unused2"
		FlightMode[15].Name="Unused3"
		FlightMode[16].Name="Guided"
		FlightMode[17].Name="Unused"
		FlightMode[18].Name="Unknown"
	end
	
  AsciiMap[1] ="A"
  AsciiMap[2] ="B"
  AsciiMap[3] ="C"
  AsciiMap[4] ="D"
  AsciiMap[5] ="E"
  AsciiMap[6] ="F"
  AsciiMap[7] ="G"
  AsciiMap[8] ="H"
  AsciiMap[9] ="I"
  AsciiMap[10] ="J"
  AsciiMap[11] ="K"
  AsciiMap[12] ="L"
  AsciiMap[13] ="M"
  AsciiMap[14] ="N"
  AsciiMap[15] ="O"
  AsciiMap[16] ="P"
  AsciiMap[17] ="Q"
  AsciiMap[18] ="R"
  AsciiMap[19] ="S"
  AsciiMap[20] ="T"
  AsciiMap[21] ="U"
  AsciiMap[22] ="V"
  AsciiMap[23] ="W"
  AsciiMap[24] ="X"
  AsciiMap[25] ="Y"
  AsciiMap[26] ="Z"
  AsciiMap[27] ="0"
  AsciiMap[28] ="1"
  AsciiMap[29] ="2"
  AsciiMap[30] ="3"
  AsciiMap[31] ="4"
  AsciiMap[32] ="5"
  AsciiMap[33] ="6"
  AsciiMap[34] ="7"
  AsciiMap[35] ="8"
  AsciiMap[36] ="9"
  AsciiMap[37] =" "
  AsciiMap[38] ="."
  AsciiMap[39] =","
  AsciiMap[40] ="="
  AsciiMap[41] =":"
  AsciiMap[42] ="!"    
  AsciiMap[43] ="-"    
  AsciiMap[44] ="+" 

  Position[1] = {}
  Position[1].x = 1
  Position[1].y = 12
  Position[2] = {}
  Position[2].x = 1
  Position[2].y = 26
  Position[3] = {}
  Position[3].x = 1
  Position[3].y = 40
  Position[4] = {}
  Position[4].x = 152
  Position[4].y = 12
  Position[5] = {}
  Position[5].x = 152
  Position[5].y = 26
  Position[6] = {}
  Position[6].x = 152
  Position[6].y = 40  
end
      
function char6(c) 
  if c >= 1 and c <= 44 then
    return AsciiMap[c]    
  elseif c == 0 then
    return 0
  else
    return "?"
  end
end

function decode100(encodedValue)
  local power = bit32.rshift(encodedValue, 10)
  if(power == 3) then
    power = -1;
  end
  local base = bit32.band(encodedValue, 1023)
  local result = base * math.pow(10, power)
  return result
end

function checkForExtensionMessage()
  local messageWord = getValue(rpmName)

  if messageWord ~= previousMessageWord then
    local extensionCommand = bit32.rshift(messageWord, 12)
    extensionCommand = bit32.band(extensionCommand, 15)
    local extensionData = bit32.band(messageWord, 4095)
    if extensionCommand == 0 then
      local highByte = bit32.rshift(extensionData, 6)     
      local lowByte  = bit32.band(extensionData, 63)                            
      if highByte ~= 0 then
        if messageBuffer == "" then                             -- first char is severity number
          messageBufferSeverity = highByte - 27                 -- 0 was encoded as 27 (see AsciiMap table)
          model.setGlobalVariable(gvMessageSeverity, 0, messageBufferSeverity)
        else
          messageBuffer = messageBuffer .. char6(highByte)
          messageBufferSize = messageBufferSize + 1
        end
      end
      if lowByte ~= 0 then
          messageBuffer = messageBuffer .. char6(lowByte)
          messageBufferSize = messageBufferSize + 1
      end
      if highByte == 0 or lowByte == 0 then     
        messageArray[(messageNext % MESSAGEBUFFERSIZE) + 1] = messageBuffer
        messageSeverityArray[(messageNext % MESSAGEBUFFERSIZE) + 1] = messageBufferSeverity
        messageBuffer = ""
        messageBufferSeverity = -1                            -- clear severity GV so that OpenTX can pick up changes 
        model.setGlobalVariable(gvMessageSeverity, 0, messageBufferSeverity)
        messageNext = messageNext + 1
        if (messageNext - messageFirst) >= MESSAGEBUFFERSIZE then
            messageFirst = messageNext - MESSAGEBUFFERSIZE
        end
        messageLatestTimestamp = getTime() 
      end                
    elseif extensionCommand == 1 then
      extensionValue["sequence"] = extensionData    
      model.setGlobalVariable(gvSequence, 0, extensionData)           
    elseif extensionCommand == 2 then
      if extensionData <= 360 then
        extensionValue["cog"] = extensionData 
      else
        extensionValue["cog"] = nil 
      end
    elseif extensionCommand == 3 then
      extensionValue["dist_travelled"] = extensionData 
    elseif extensionCommand == 4 then
      extensionValue["hdop"] = extensionData   
      model.setGlobalVariable(gvHdop, 0, extensionData)        
    elseif extensionCommand == 5 then
      extensionValue["fix_type"] = extensionData        
    elseif extensionCommand == 6 then
      extensionValue["sats_visible"] = extensionData        
    elseif extensionCommand == 7 then
      extensionValue["base_mode"] = extensionData                                     
      if extensionData ~= nil then
        local armedBit  
        armedBit = bit32.rshift(extensionData, 7) 
        armedBit = bit32.band(armedBit, 1)       
        model.setGlobalVariable(gvArmed, 0, armedBit)      
      else
        model.setGlobalVariable(gvArmed, 0, -1)               
      end         
    elseif extensionCommand == 8 then
      extensionValue["custom_mode"] = extensionData 
      model.setGlobalVariable(gvFlightMode, 0, extensionData)
    elseif extensionCommand == 9 then
      extensionValue["bar_altitude"] = extensionData 
    elseif extensionCommand == 10 then
      extensionValue["ranger_dist"] = extensionData 
    elseif extensionCommand == 11 then
      extensionValue["batt_remain"] = extensionData 
    elseif extensionCommand == 12 then
      extensionValue["mav_consumed"] = decode100(extensionData);        -- this value is encoded using an exponential encoding to get a higer range of values and 3 digit precision   
    elseif extensionCommand == 13 then
      extensionValue["armed_distance"] = extensionData 
    elseif extensionCommand == 14 then
      extensionValue["calc_consumed"] = decode100(extensionData);       -- this value is encoded using an exponential encoding to get a higer range of values and 3 digit precision         
    elseif extensionCommand == 15 then
      extensionValue["armed_bearing"] = extensionData
    end           
    previousMessageWord = messageWord     
  end
end

function getArmedValue()
  local baseMode = extensionValue["base_mode"]
  local armedBit                                             -- nil if unknown, 1 if armed, 0 if disarmed
  if baseMode ~= nil then
    armedBit = bit32.rshift(baseMode, 7) 
    armedBit = bit32.band(armedBit, 1)         
    return armedBit
  else   
    return nil
  end
end

function loadGpsData()
  local gpsLatLon = getValue(gpsName)
  if (type(gpsLatLon) == "table") then
    gpsData["lat"] = gpsLatLon["lat"]
    gpsData["lon"] = gpsLatLon["lon"]
    if gpsData["armed_lat"] == nil then
      if getArmedValue() == 1 then
        gpsData["armed_lat"] = gpsData["lat"]
        gpsData["armed_lon"] = gpsData["lon"]
      end 
    else
      if getArmedValue() ~= 1 then
        gpsData["armed_lat"] = nil
        gpsData["armed_lon"] = nil 
      end
    end    
  end
end

function getLatestMessage()
  if messageFirst == messageNext then
    return ""
  end
  local latestMessage = messageArray[((messageNext - 1) % MESSAGEBUFFERSIZE) + 1]
  return latestMessage
end
  
local function drawBatteryVoltage(x,y)
	local batteryVoltage = getValue(vfasName)
  lcd.drawText(x, y + 5, "Bat", SMLSIZE)
  lcd.drawNumber(x + 43, y,batteryVoltage*10,MIDSIZE+PREC1)
  lcd.drawText(lcd.getLastPos(), y + 5, "V",SMLSIZE)
end

local function drawCurrent(x, y)
	local current = getValue(currentName)
  lcd.drawText(x, y + 5, "Cur", SMLSIZE)
  lcd.drawNumber(x + 43, y, current*10, MIDSIZE+PREC1)
  local t = lcd.getLastPos() + 1
  lcd.drawText(t, y + 5,"A",SMLSIZE)
end

local function drawTotalCurrent(x,y)
  local totalCurrent = extensionValue["calc_consumed"]
  if totalCurrent == nil then
      totalCurrent = 0
  end
  lcd.drawText(x, y + 5, "Tot", SMLSIZE)
  lcd.drawNumber(x + 43, y, totalCurrent, MIDSIZE)
  local t = lcd.getLastPos() + 1
  lcd.drawText(t, y + 5, "mAh", SMLSIZE)
end

local function drawVerticalSpeed(x, y)
	local vspd = getValue(vspdName)
  lcd.drawText(x, y + 5, "Vsp", SMLSIZE)
  lcd.drawNumber(x + 43, y, vspd*10, MIDSIZE+PREC1)
  local t = lcd.getLastPos() + 1
  lcd.drawText(t, y, "m", SMLSIZE)   
  lcd.drawText(t, y+5, "s", SMLSIZE)    
end

local function drawHeading(x, y)
	local hdg = getValue(headingName)
  lcd.drawText(x, y + 5, "Hdg", SMLSIZE)
  lcd.drawNumber(x + 43, y, hdg, MIDSIZE)
  local t = lcd.getLastPos() + 1
  lcd.drawText(t, y, "o", SMLSIZE)     
end

local function drawSpeed(x, y)
  local speed = getValue(speedName)               -- originally in m/s * 10
  speed = speed * 3.6                             -- now in km/hr
  if imperialUnits == 1 then
    speed = speed * 0.621                       -- to mi/hr
  end
  lcd.drawText(x, y + 5, "Spd", SMLSIZE)
  lcd.drawNumber(x + 43, y, speed, MIDSIZE)
  local t = lcd.getLastPos() + 1

  if imperialUnits == 1 then
    lcd.drawText(t, y, "mi", SMLSIZE)
  else
    lcd.drawText(t, y, "km", SMLSIZE)
  end    
  lcd.drawText(t, y+5, "hr", SMLSIZE)
end

local function drawAltitude(x, y)
	local altitude = getValue(altitudeName)
  if imperialUnits == 1 then
    altitude = altitude * 3.28
  end
  lcd.drawText(x, y + 5, "Alt", SMLSIZE)
  lcd.drawNumber(x + 43, y, altitude, MIDSIZE)
  local t = lcd.getLastPos() + 1
  if imperialUnits == 1 then
    lcd.drawText(t, y + 5, "ft", SMLSIZE)
  else
    lcd.drawText(t, y + 5, "m", SMLSIZE)
  end
end

local function drawDistance(x, y)
  local distance = extensionValue["armed_distance"]    
  if distance == nil or distance == 4095 then
    distance = 0
  end
  if imperialUnits == 1 then
    distance = distance * 3.28
  end    
  lcd.drawText(x, y + 5, "Dst", SMLSIZE)
  lcd.drawNumber(x + 43, y, distance, MIDSIZE)
  local t = lcd.getLastPos() + 1
  if imperialUnits == 1 then
    lcd.drawText(t, y + 5, "ft", SMLSIZE)
  else
    lcd.drawText(t, y + 5, "m", SMLSIZE)
  end    
end

local function drawDistanceTravelled(x, y)
  local distance = extensionValue["dist_travelled"]    
  if distance == nil or distance == 4095 then
    distance = 0
  end
  if imperialUnits == 1 then
    distance = distance * 3.28
  end    
  lcd.drawText(x, y + 5, "Trv", SMLSIZE)
  lcd.drawNumber(x + 43, y, distance, MIDSIZE)
  local t = lcd.getLastPos() + 1
  if imperialUnits == 1 then
    lcd.drawText(t, y + 5, "ft", SMLSIZE)
  else
    lcd.drawText(t, y + 5, "m", SMLSIZE)
  end    
end

local function drawHdop(x,y)
	local hdop = extensionValue["hdop"]
  if hdop == nil then
      return
  end
  hdop = hdop / 10
  if hdop >= 99 then
    lcd.drawText(x-22, y+3, ">", SMLSIZE)  
    lcd.drawNumber(x, y, 99, MIDSIZE)
  else
    lcd.drawNumber(x, y, hdop*10, PREC1 + MIDSIZE)
  end

  local t = lcd.getLastPos() + 1
  lcd.drawText(t, y, "hd", SMLSIZE)  
  lcd.drawText(t, y + 6, "op", SMLSIZE)  
end	

local function drawSats(x, y)
	local fixType = extensionValue["fix_type"]
  if fixType == nil then
    return
  end
  local satellitesVisible = extensionValue["sats_visible"]
  if satellitesVisible == nil then
    return
  end

  if fixType >= 3 then
    lcd.drawNumber(x + 6, y, satellitesVisible, MIDSIZE)
    lcd.drawText(x + 7, y + 5, "sats", SMLSIZE)                  
  elseif fixType == 2 then
    lcd.drawText(x + 8, y, "2D", MIDSIZE)
  elseif fixType == 1 then
    lcd.drawText(x, y + 5, "Search", SMLSIZE)
  elseif fixType == 0 then
    lcd.drawText(x, y + 5, "No Fix", SMLSIZE)
  end
end

function getXYAtAngle(x, y, angle, length)
	if angle < 0 then
		angle = angle + 360
	elseif angle >= 360 then
		angle = angle - 360
	end
  local x2 = x + math.sin(math.rad(angle)) * length
  local y2 = y - math.cos(math.rad(angle)) * length
	return x2, y2
end

local function drawLineAtAngle(x, y, r1, r2, angle)
  local xStart, yStart = getXYAtAngle(x, y, angle, r1)			
  local xEnd, yEnd = getXYAtAngle(x, y, angle, r2)
  lcd.drawLine(xStart, yStart, xEnd, yEnd, SOLID, FORCE)
end 

local function drawHeadingHud(x, y)
  local arrowSide = 5
  local arrowTail = 5
  local arrowLength = 16
  local arrowSideAngle = 120
  local headingHudOuterRadius = 15

  local vehicleHeading = getValue(headingName)
  local headingToVehicle = extensionValue["armed_bearing"]
  if headingToVehicle == nil then
    return
  end
  local relativeHeading = vehicleHeading - headingToVehicle
  if relativeHeading < 0 then
    relativeHeading = relativeHeading + 360	
  end			
  local xTail, yTail = getXYAtAngle(x, y, relativeHeading - 180, arrowTail)	
  local xLeft, yLeft = getXYAtAngle(xTail, yTail, relativeHeading-arrowSideAngle, arrowSide)
  local xRight, yRight = getXYAtAngle(xTail, yTail, relativeHeading+arrowSideAngle, arrowSide)
  local xNose, yNose = getXYAtAngle(xTail, yTail, relativeHeading, arrowLength)
  lcd.drawLine(xTail, yTail, xLeft, yLeft, SOLID, FORCE)
  lcd.drawLine(xLeft, yLeft, xNose, yNose, SOLID, FORCE)
  lcd.drawLine(xTail, yTail, xRight, yRight, SOLID, FORCE)
  lcd.drawLine(xRight, yRight, xNose, yNose, SOLID, FORCE)
  
  local courseOverGround = extensionValue["cog"]
  if courseOverGround ~= nil then
    local speed = getValue(speedName)
    if speed > 1 then		
      local relativeCourseOverGround = courseOverGround - headingToVehicle
      if relativeCourseOverGround < 0 then
        relativeCourseOverGround = relativeCourseOverGround + 360
      end   
      drawLineAtAngle(x, y, 0, headingHudOuterRadius, relativeCourseOverGround)                 
    end    
  end
end

function drawTopPanel()
  lcd.drawFilledRectangle(0, 0, 212, 9, 0)
  
  local flightModeNumber = extensionValue["custom_mode"]
  if flightModeNumber then
    flightModeNumber = flightModeNumber + 1
    if flightModeNumber < 1 or flightModeNumber > 17 then
        flightModeNumber = 18
    end
    lcd.drawText(1, 1, FlightMode[flightModeNumber].Name, INVERS)
  end

  lcd.drawTimer(lcd.getLastPos() + 5, 1, model.getTimer(0).value, INVERS)

  lcd.drawText(lcd.getLastPos() + 5, 1, "TX:", INVERS)
  lcd.drawNumber(lcd.getLastPos() + 16, 1, getValue("tx-voltage")*10, PREC1 + INVERS)

  lcd.drawText(lcd.getLastPos(), 1, "v", INVERS)

  lcd.drawText(lcd.getLastPos() + 5, 1, "rssi:", INVERS)
  local rssiValue = getValue(rssiName)
  lcd.drawNumber(lcd.getLastPos() + 10, 1, rssiValue, 0+INVERS)
end

local function drawBottomPanel()
	local baseMode = extensionValue["base_mode"]
  local armedBit                                             -- nil if unknown, 1 if armed, 0 if disarmed
  if baseMode ~= nil then
    armedBit = bit32.rshift(baseMode, 7) 
    armedBit = bit32.band(armedBit, 1)
  end
  
  local textAttributes = 0
  if armedBit == 1 then
    textAttributes = INVERS
    lcd.drawFilledRectangle(0, 54, 211, 63, BLINK)
  else
    lcd.drawLine(0, 53, 211, 53, SOLID, FORCE)
  end

  if getTime() < (messageLatestTimestamp + 1000) then
    local footerMessage = getLatestMessage()
    lcd.drawText(1, 55, footerMessage, textAttributes)
  else    
    if armedBit == 1 then
      lcd.drawText(1, 55, "Armed", textAttributes)
    elseif armedBit == 0 then
      lcd.drawText(1, 55, "Disarmed", textAttributes)
    end            
  end
end

local function background() 
  checkForExtensionMessage()
end

local function run(event)
  lcd.clear()

  loadGpsData()

  drawTopPanel()
  drawBottomPanel()
  
  if Slot["BatteryVoltage"] > 0 then
    drawBatteryVoltage(Position[Slot["BatteryVoltage"]].x, Position[Slot["BatteryVoltage"]].y)
  end
  
  if Slot["Current"] > 0 then
    drawCurrent(Position[Slot["Current"]].x, Position[Slot["Current"]].y)
  end  
  if Slot["TotalCurrent"] > 0 then
    drawTotalCurrent(Position[Slot["TotalCurrent"]].x, Position[Slot["TotalCurrent"]].y)
  end   
  if Slot["VerticalSpeed"] > 0 then
    drawVerticalSpeed(Position[Slot["VerticalSpeed"]].x, Position[Slot["VerticalSpeed"]].y)
  end
  if Slot["Heading"] > 0 then
    drawVerticalSpeed(Position[Slot["Heading"]].x, Position[Slot["Heading"]].y)
  end
  
  drawSats(72, 40)	
  drawHdop(130, 40) 
  drawHeadingHud(107, 26)
 
  if Slot["Speed"] > 0 then
    drawSpeed(Position[Slot["Speed"]].x, Position[Slot["Speed"]].y)
  end   
  if Slot["Altitude"] > 0 then
    drawAltitude(Position[Slot["Altitude"]].x, Position[Slot["Altitude"]].y)
  end     
  if Slot["Distance"] > 0 then
    drawDistance(Position[Slot["Distance"]].x, Position[Slot["Distance"]].y)
  end 
  if Slot["DistanceTravelled"] > 0 then
    drawDistanceTravelled(Position[Slot["DistanceTravelled"]].x, Position[Slot["DistanceTravelled"]].y)
  end 
  
end

return {init=init, run=run, background=background}
