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

modelInfo = model.getInfo()
modelName = modelInfo.name
scriptDirectory = "/SCRIPTS/" .. modelName

RIGHTPX = 212
BOTTOMPX = 63

FlightMode = {}
Severity={}
AsciiMap={}

--Init A registers
local A2 = model.getTelemetryChannel(1)
if A2 .unit ~= 3 or A2 .range ~=1024 or A2 .offset ~=0
then
    A2.unit = 3
    A2.range = 1024
    A2.offset = 0
    model.setTelemetryChannel(1, A2)
end

local A3 = model.getTelemetryChannel(2)
if A3.unit ~= 3 or A3.range ~=362 or A3.offset ~=-180
then
    A3.unit = 3
    A3.range = 362
    A3.offset = -180
    A3.alarm1 = -180
    A3.alarm2 = -180
    model.setTelemetryChannel(2, A3)
end

local A4 = model.getTelemetryChannel(3)
if A4.unit ~= 3 or A4.range ~=362 or A4.offset ~=-180
then
    A4.unit = 3
    A4.range = 362
    A4.offset = -180
    A4.alarm1 = -180
    A4.alarm2 = -180
    model.setTelemetryChannel(3, A4)
end

function initialize()
    local i
    
    for i=1, 17 do
        FlightMode[i] = {}
        FlightMode[i].Name=""
    end

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
    
    for i=1,9 do
        Severity[i]={}
        Severity[i].Name=""
        Severity[i].Sound="/SOUNDS/en/ER"..(i-2)..".wav"
    end

    Severity[2].Name="Emergency"
    Severity[3].Name="Alert"
    Severity[4].Name="Critical"
    Severity[5].Name="Error"
    Severity[6].Name="Warning"
    Severity[7].Name="Notice"
    Severity[8].Name="Info"
    Severity[9].Name="Debug"
    
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
end
    
initialize()
      
function char(c) 
    if c >= 48 and c <= 57 then
      return "0" + (c - 48)
    elseif c >= 65 and c <= 90 then
      return AsciiMap[c - 64]
    elseif c >= 97 and c <= 122 then 
      return AsciiMap[c - 96]    
    elseif c == 32 then
      return " "
    elseif c == 46 then
      return "."
    else
      return ""
    end
end

messageBuffer = ""
messageBufferSize = 0
previousMessageWord = 0
footerMessage = ""
messagePriority = -1

function getTextMessage()
    local returnValue = ""
    local messageWord = getValue("rpm")

    if messageWord ~= previousMessageWord then
        local highByte = bit32.rshift(messageWord, 7)
        highByte = bit32.band(highByte, 127)
        local lowByte = bit32.band(messageWord, 127)

        if highByte ~= 0 then
            if highByte >= 48 and highByte <= 57 and messageBuffer == "" then
                messagePriority = highByte - 48
            else
              messageBuffer = messageBuffer .. char(highByte)
              messageBufferSize = messageBufferSize + 1
            end
            if lowByte ~= 0 then
                messageBuffer = messageBuffer .. char(lowByte)
                messageBufferSize = messageBufferSize + 1
            end
        end
        if highByte == 0 or lowByte == 0 then
          returnValue = messageBuffer
          messageBuffer = ""
          messageBufferSize = 0
        end
        previousMessageWord = messageWord        
    end
    return returnValue
end

MESSAGEBUFFERSIZE = 5
messageArray = {}
messageFirst = 0
messageNext = 0
messageLatestTimestamp = 0

function getLatestMessage()
    if messageFirst == messageNext then
        return ""
    end
    return messageArray[((messageNext - 1) % MESSAGEBUFFERSIZE) + 1]
end

function checkForNewMessage()
    local msg = getTextMessage()
    if msg ~= "" then
        if msg ~= getLatestMessage() then
            messageArray[(messageNext % MESSAGEBUFFERSIZE) + 1] = msg
            messageNext = messageNext + 1
            if (messageNext - messageFirst) >= MESSAGEBUFFERSIZE then
                messageFirst = messageNext - MESSAGEBUFFERSIZE
            end
            messageLatestTimestamp = getTime()
        end
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
      
---------------------------------------------------------------------------------------------------      
      
function getDirectionFromTo(fromLat, fromLon, toLat, toLon)
    if(fromLat == toLat and fromLon == toLon) then
        return -1
    end
    local z1 = math.sin(math.rad(toLon) - math.rad(fromLon)) * math.cos(math.rad(toLat))
    local z2 = math.cos(math.rad(fromLat)) * math.sin(math.rad(toLat)) - math.sin(math.rad(fromLat)) * math.cos(math.rad(toLat)) * math.cos(math.rad(toLon) - math.rad(fromLon))
    local directionTo = math.deg(math.atan2(z1, z2))
    if directionTo < 0 then
        directionTo=directionTo+360
    end
    return directionTo
end

lastTime = 0
previousVehicleLat1 = 0
previousVehicleLon1 = 0
previousVehicleLat2 = 0
previousVehicleLon2 = 0
vehicleGroundDirection = -1

function updateGroundDirection()
    local currentVehicleLat = getValue("latitude")
    local currentVehicleLon = getValue("longitude")
    if currentVehicleLat~=0 and currentVehicleLon~=0 and previousVehicleLat2~=0 and previousVehicleLon2~=0 then 
        vehicleGroundDirection = getDirectionFromTo(previousVehicleLat2, previousVehicleLon2, currentVehicleLat, currentVehicleLon)
    end
    previousVehicleLat2 = previousVehicleLat1
    previousVehicleLon2 = previousVehicleLon1      
    previousVehicleLat1 = currentVehicleLat
    previousVehicleLon1 = currentVehicleLon  
end

function getVehicleGroundDirection()
    return vehicleGroundDirection
end

function getDirectionToVehicle()
	local directionToVehicle = -1
	local pilotlat = getValue("pilot-latitude")
	local pilotlon = getValue("pilot-longitude")
	local curlat = getValue("latitude")
	local curlon = getValue("longitude")

	if pilotlat~=0 and curlat~=0 and pilotlon~=0 and curlon~=0 then 
        directionToVehicle = getDirectionFromTo(pilotlat, pilotlon, curlat, curlon)
	end
	return directionToVehicle
end
       
----------------------------------------------------------------------------------------------------       
       
local function drawBatteryVoltage(x,y)
	local batteryVoltage=getValue("vfas")
    lcd.drawNumber(x,y,batteryVoltage*10,MIDSIZE+PREC1)
    lcd.drawText(lcd.getLastPos(),y+5,"V",SMLSIZE)
end

local function drawCurrent(x,y)
	local current=getValue("current")
    lcd.drawNumber(x,y,current*10,MIDSIZE+PREC1)
    lcd.drawText(lcd.getLastPos(),y+5,"A",SMLSIZE)
end

local function drawTotalCurrent(x,y)
	local totalCurrent = getValue("consumption")
    lcd.drawNumber(x, y, totalCurrent, MIDSIZE)
    lcd.drawText(lcd.getLastPos(), y+5, "mAh", SMLSIZE)
end

local function drawSpeed(x,y)
    local speed = getValue("gps-speed")
    lcd.drawText(x, y + 5, "Spd", SMLSIZE)
    lcd.drawNumber(x + 37, y, speed, MIDSIZE)
    local t = lcd.getLastPos() + 1
    --lcd.drawText(x, y, "m", SMLSIZE)
    --lcd.drawText(x, y+5, "s", SMLSIZE)
    lcd.drawText(t, y, "km", SMLSIZE)
    lcd.drawText(t, y+5, "hr", SMLSIZE)
end

local function drawAltitude(x, y)
	local altitude = getValue("altitude")
    lcd.drawText(x, y + 5, "Alt", SMLSIZE)
    lcd.drawNumber(x + 36, y, altitude, MIDSIZE)
    local t = lcd.getLastPos() + 1
    lcd.drawText(t, y + 5, "m", SMLSIZE)
end


local function drawDistance(x, y)
	local distance = getValue("distance")
    lcd.drawText(x, y + 5, "Dst", SMLSIZE)
    lcd.drawNumber(x + 36, y, distance, MIDSIZE)
    local t = lcd.getLastPos() + 1
    lcd.drawText(t, y + 5, "m", SMLSIZE)
end

local function drawHdop(x,y)
	local hdop = getValue("a2")			-- a2 is hdop*10
    if hdop > 99 then
        hdop = 99
        lcd.drawText(x-22, y+3, ">", SMLSIZE)  
    end
	lcd.drawNumber (x, y, hdop, PREC1 + MIDSIZE)
    local t = lcd.getLastPos() + 1
    lcd.drawText(t, y, "hd", SMLSIZE)  
    lcd.drawText(t, y + 6, "op", SMLSIZE)  
end	

local function drawSats(x, y)
    local satValue = getValue("temp1")
    local numSats = (satValue - (satValue % 10)) /10
    local lock = satValue % 10

    if lock == 3 then
        lcd.drawNumber(x + 6, y, numSats, MIDSIZE)
        lcd.drawText(x + 7, y + 5, "sats", SMLSIZE)                  
    elseif lock == 2 then
        lcd.drawText(x + 8, y, "2D", MIDSIZE)
    elseif lock == 1 then
        lcd.drawText(x, y + 5, "Search", SMLSIZE)
    elseif lock == 0 then
        lcd.drawText(x, y + 5, "No Fix", SMLSIZE)
    end
end

local function getRelativeGroundDirection()
    local groundDirection = getVehicleGroundDirection()
    if groundDirection >= 0 then 
        local headingToVehicle = getDirectionToVehicle()
        if headingToVehicle >= 0 then
            local relativeHeading = groundDirection - headingToVehicle
            if(relativeHeading < 0) then
                relativeHeading = relativeHeading + 360
            end   
			return relativeHeading
		end
	end
	return -1
end

local function drawHeadingHud(x, y)
    local arrowSide = 5
    local arrowTail = 5
    local arrowLength = 16
    local arrowSideAngle = 120
    --local headingHudInnerRadius = 16
    local headingHudOuterRadius = 15

    local vehicleHeading = getValue("heading")
    local headingToVehicle = getDirectionToVehicle()
    if headingToVehicle >= 0 then
        local relativeHeading = vehicleHeading - headingToVehicle
        if(relativeHeading < 0) then
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


        if getValue("gps-speed") > 0 then
            local relativeGroundDirection = getRelativeGroundDirection()		
            drawLineAtAngle(x, y, 0, headingHudOuterRadius, relativeGroundDirection)
            --lcd.drawPixmap(x, y, scriptDirectory .. "/BMP/DOT8X8.BMP")
            --local i
            --for i = relativeGroundDirection - 4, relativeGroundDirection + 4, 1 do
            --for i = relativeGroundDirection - 20, relativeGroundDirection + 20, 4 do
                --drawLineAtAngle(x, y, headingHudInnerRadius - 1, headingHudOuterRadius-1, i)
            --end                       
        end
	end
	--drawCircle(x, y, headingHudInnerRadius)
	--drawCircle(x, y, headingHudOuterRadius)
end

local function drawTopPanel()
    lcd.drawFilledRectangle(0, 0, 212, 9, 0)
    
    local flightModeNumber = getValue("fuel") + 1
    if flightModeNumber < 1 or flightModeNumber > 17 then
        flightModeNumber = 13
    end
    lcd.drawText(1, 1, FlightMode[flightModeNumber].Name, INVERS)

    lcd.drawTimer(lcd.getLastPos() + 5, 1, model.getTimer(0).value, INVERS)

    lcd.drawText(lcd.getLastPos() + 5, 1, "TX:", INVERS)
    lcd.drawNumber(lcd.getLastPos() + 16, 1, getValue("tx-voltage")*10, 0+PREC1+INVERS)

    lcd.drawText(lcd.getLastPos(), 1, "v", INVERS)

    lcd.drawText(lcd.getLastPos() + 5, 1, "rssi:", INVERS)
    lcd.drawNumber(lcd.getLastPos() + 10, 1, getValue("rssi"), 0+INVERS)
end

local function drawBottomPanel()
    local temp2 = getValue("temp2")
    local armed = 0
    local disarmed = 0
    
    if temp2 == 21844 then
        disarmed = 1
    elseif temp2 == 21845 then
        armed = 1        
    end
    
    local textAttributes = 0
    if armed == 1 then
        textAttributes = INVERS
        lcd.drawFilledRectangle(0, 54, 211, 63, BLINK)
    else
        lcd.drawLine(0, 53, 211, 53, SOLID, FORCE)
    end

    if getTime() < (messageLatestTimestamp + 1000) then
        local footerMessage = getLatestMessage()
        lcd.drawText(1, 55, footerMessage, textAttributes)
    else    
        if armed == 1 then
            lcd.drawText(1, 55, "Armed", textAttributes)
        elseif disarmed == 1 then
            lcd.drawText(1, 55, "Disarmed", textAttributes)
        end            
    end
end
    
local function background() 
end

local function run(event)
    local loopStartTime = getTime()
    if loopStartTime > (lastTime + 50) then
        updateGroundDirection()
        lastTime = loopStartTime
    end 
    
    lcd.clear()
    checkForNewMessage()
    
    drawTopPanel()
    drawBottomPanel()

    drawBatteryVoltage(32, 12)
    drawCurrent(32, 26)
    drawTotalCurrent(32, 40)

    drawSats(72, 40)	
    drawHdop(130, 40)
    
    drawHeadingHud(107, 26)
    
    drawSpeed(159, 12)
    drawAltitude(160, 26)
    drawDistance(160, 40)	
end

return {run=run, background=background}