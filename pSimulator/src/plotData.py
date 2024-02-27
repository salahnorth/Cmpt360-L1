#!/usr/bin/env python3
from graphics import *
from PIL import Image as NewImage
import glob
import random
import sys

"""===========================================================================
Description: This file plots the data of the log file
Authors    : Salah Mohamed & Vy Tran
Date       : 26 February 2024
Version    : 12
============================================================================"""


def loadRawData(filename):
    '''Purpose  : Loading data file
    Parameter   : filename
    Return      : data'''
    file = open(filename, "r")
    tmp = file.readline()
    rawData = []  # time, PID, status, niceness, cputime, proctime
    while tmp:
        if tmp not in ['\n']:
            rawData.append(tmp)
        tmp = file.readline()
    file.close()
    return rawData


def formatData(rData):
    '''Purpose  : Format raw data
    Parameter   : rData
    Return      : fData'''
    fData = {}
    for line in rData:
        line = line.split(", ")
        fData[int(line[1])] = {}

    for k in fData.keys():
        kData = {}
        for ele in rData:
            ele = ele.split(", ")
            if (int(ele[1]) == k) and int(ele[2]) == 2:
                kData[(ele[9]).replace('\n', '')] = float(ele[5])
        fData[k] = kData
    return fData

def extractCompleteTime(rData):
    '''Purpose  : Extracting complete time for ratio calculation
    Parameter   : rData
    Return      : completeTime'''

    completeTime = 0
    for line in rData:
        line = line.split(", ")
        if int(line[2]) == 3:
            completeTime = float(line[0])
    return completeTime


def loadTimeInfo(filename):
    '''Purpose  : Loading data file
     Parameter   : filename
     Return      : data'''

    file = open(filename, "r")
    tmp = file.readline()
    rawData = []  # PID, turnaround, response, arrivaltime
    while tmp:
        if tmp not in ['\n']:
            rawData.append(tmp)
        tmp = file.readline()
    file.close()
    return rawData


def formatResponseTimeInfo(raw):
    '''Purpose  : Formatting the data of response time
    Parameter   : raw
    Return      : responseData'''
    responseData = {}
    for line in raw:
        line = line.split(", ")
        if int(line[2]) == 3:
            responseData[int(line[1])] = line[8]

    return responseData


def formatTurnaroundTimeInfo(raw):
    '''Purpose  : Formatting the data of turnaround time
    Parameter   : raw
    Return      : turnaroundData'''

    turnaroundData = {}
    for line in raw:
        line = line.split(", ")
        if int(line[2]) == 3:
            turnaroundData[int(line[1])] = line[7]
    return turnaroundData


def averageCalculation(response, turnaround):
    '''Purpose  : Average times calculations
    Parameter   : response, turnaround
    Return      : averageResponseTime, averageTurnaroundTime'''

    rSum = 0
    for rKey in response.keys():
        rSum += float(response[rKey])

    averageResponseTime = rSum/len(response.keys())

    tSum = 0
    for tKey in turnaround.keys():
        tSum += float(turnaround[tKey])

    averageTurnaroundTime = tSum/len(turnaround.keys())

    return averageResponseTime, averageTurnaroundTime


def getValuesByKey(dataDict, targetKey, values=[], current_key=None):
    '''Purpose  : Using (sub)keys to extract the data (values with sub-keys)
    Parameter   : dataDict, targetKey, values=[], current_key
    Return      : values'''
    for key, value in dataDict.items():
        if key == targetKey:
            values.append(value)
            values.append(current_key)
        elif isinstance(value, dict):
            getValuesByKey(value, targetKey, values, key)
    return values


def searchMainkey(dataDict, targetSubkey, currentKey=None):
    '''Purpose  : Extracting main keys from sub-keys
    Parameter   : dataDict, targetSubkey, currentKey
    Return      : mainKeys'''

    mainKeys = []
    for key, value in dataDict.items():
        if key == targetSubkey:
            mainKeys .append(currentKey)
        elif isinstance(value, dict):
            mainKeys .extend(searchMainkey(value, targetSubkey, key))
    return mainKeys


def RRdata(fData):
    '''Purpose  : Getting data to plot (RR)
    Parameter   : fData
    Return      : values'''

    keyOrder = []
    subkeyorder = []

    for k in fData.keys():
        keyOrder.append(k)
        for subkey in fData[k]:
            subkeyorder.append(subkey)

    sameK = []
    for item in subkeyorder:
        if subkeyorder.count(item) > 1 and item not in sameK:
            sameK.append(item)
        if len(sameK) == 0:
            sameK.append(item)

    time = []
    for i in sameK:  # Round no. not pid
        time = getValuesByKey(fData, i)
    return time


def SJFdata(fData):
    '''Purpose  : Getting data to plot (SJF)
    Parameter   : fData
    Return      : values'''

    values = []
    for k in fData.keys():
        for sk in fData[k].keys():
            values.append(fData[k][sk])

    n = len(values)
    # Traverse through all elements
    for i in range(n):
        # Find the minimum element in the unsorted portion
        minidx = i
        for j in range(i + 1, n):
            if values[j] < values[minidx]:
                minidx = j

        # Swap the found minimum element with the first element
        values[i], values[minidx] = values[minidx], values[i]

    for key, item in fData.items():
        for k in item.keys():
            for idx in range(len(values)):
                if item[k] == values[idx]:
                    values.insert(idx+1, key)
    return values


def FFdata(fData):
    '''Purpose  : Getting data to plot (FIFO)
    Parameter   : fData
    Return      : time'''

    keyOrder = []
    subkeyorder = []
    for k in fData.keys():
        keyOrder.append(k)
        for subkey in fData[k]:
            subkeyorder.append(subkey)

    sameK = []
    for item in subkeyorder:
        if subkeyorder.count(item) > 1 and item not in sameK:
            sameK.append(item)
        if len(sameK) == 0:
            sameK.append(item)

    time = []
    for i in sameK:  # Round no. not pid
        time = getValuesByKey(fData, i)
    return time


def plotData(type, fData, tComplete, averageTime):
    '''Purpose  : Plotting the data
    Parameter   : fData
    Return      : none'''

    HEIGHT, WIDTH = 800, 1000

    win = GraphWin("Plotting Data", WIDTH, HEIGHT)

    # Beginning Points
    xBegin = 50
    yBegin = HEIGHT/4
    yEnd   = HEIGHT/3

    # Calculation
    keyOrder = []
    subkeyorder = []
    for k in fData.keys():
        keyOrder.append(k)
        for subkey in fData[k]:
            subkeyorder.append(subkey)

    # Time Data to plot
    time = []
    if type == 3:
        time = RRdata(fData)
    elif type == 2:
        time = SJFdata(fData)
    elif type == 1:
        time = FFdata(fData)

    pid = []
    for coor in range(1,len(time),2):
        pid.append(time[coor])
        pid.append(time[coor])

    # Calculation
    xp = [xBegin]
    for pos in range(0, len(time), 2):
        iFlag = 0
        for j in range(len(xp) - 1, -1, -1):
            if (iFlag == 0):
                # Ratio: Full Width == 100%
                x = int((time[pos] * ((WIDTH-150))/tComplete)) + xp[j]
                xp.append(x)
                xp.append(x)
                iFlag = 1

    colorList = {}
    for v in keyOrder:
        color = color_rgb(random.randint(0, 250),
                          random.randint(0, 250),
                          random.randint(0, 250))
        colorList[v] = color

    colorToFill = []
    for process in pid:
        match = 0
        for color in colorList.keys():
            if process == color:
                match = 1

        if match == 1:
            colorToFill.append(colorList[process])

    centerPoint = []
    for idx in range(len(xp)-1):
        r = Rectangle(Point(xp[idx], yBegin), Point(xp[idx+1], yEnd))
        r.draw(win)

        r.setFill(colorToFill[idx])
        centerPoint.append(r.getCenter())

    # Add labels
    for position in range(0, len(centerPoint), 2):
        lb = Text(centerPoint[position], pid[position])
        lb.setTextColor("white")
        lb.setSize(10)
        lb.setStyle("bold")
        lb.draw(win)

    # Drawing x-axis arrow
    # The end point of the arrow will be the largest x
    # point + 50 (50 is a random number)
    xLine = max(xp) + 50
    line  = Line(Point(xBegin, yEnd), Point(xLine, yEnd))
    line.setArrow("last")

    xLabel = Text(Point(xLine, yEnd + 10), "t") # Label x-axis as t

    line.draw(win)
    xLabel.draw(win)

    # Adding average calculations information
    tTitle = Text(Point(170, HEIGHT - 400), "Average turnaround time:")
    rTitle = Text(Point(160, HEIGHT - 300), "Average response time:")
    pTitle = Text(Point(150, HEIGHT - 200), "Number of processes:")

    averageRes = format(averageTime[0], ".2f")
    averageTurn = format(averageTime[1], ".2f")
    numProcess = len(keyOrder)

    turnOut = Text(Point(350, HEIGHT - 400), averageTurn)
    resOut = Text(Point(350, HEIGHT - 300), averageRes)
    nOut = Text(Point(350, HEIGHT - 200), numProcess)

    tTitle.draw(win)
    rTitle.draw(win)
    pTitle.draw(win)

    turnOut.draw(win)
    resOut.draw(win)
    nOut.draw(win)

    # save postscipt image
    win.postscript(file="../log/plotData5.eps")
    # use PIL to save and change the file type to PNG
    img = NewImage.open("../log/plotData5.eps")
    img.save("../log/plotDataLab2.png", "png")
    '''WARNING: The quality of the .png is bad. You may not see the label
    of x-axis as "t". Therefore, you should open the eps file in PDF reader.
    The quality is clearly better.'''

    while not win.isClosed():
    	win.checkMouse()  # pause for click in window to close
    win.close()


def find_file(pattern):
    '''Purpose  : Finding the file to extract data
    Parameter   : pattern
    Return      : filename'''
    filename = glob.glob(pattern)
    return filename


def runningFunctions(type, filename):
    '''Purpose  : Running Functions of the program
    Parameter   : type, filename
    Return      : none'''

    rData = loadRawData(filename)
    fData = formatData(rData)
    cTime = extractCompleteTime(rData)
    rawValue = loadTimeInfo(filename)
    resValue = formatResponseTimeInfo(rawValue)
    turnValue = formatTurnaroundTimeInfo(rawValue)
    averageValues = averageCalculation(resValue, turnValue)

    plotData(type, fData, cTime, averageValues)


def main():
    '''Purpose  : Run the program
    Parameter   : none
    Return      : none'''

    if len(sys.argv) != 2:
        print(f"\nUsage: {sys.argv[0]} <type of algorithm>")
        print(f"FIFO: 1; SJF: 2; Round-Robin: 3")
    else:
        type = int(sys.argv[1])
        if type == 3:   # ROUND-ROBIN
            file1 = find_file("../log/roundrobinLog.txt")
            if len(file1) == 0:
                print("Error! There's no file.")
            else:
                filename      = file1[0]
                runningFunctions(type, filename)

        elif type == 2:    # SJF
            file1 = find_file("../log/SJFLog.txt")

            if len(file1) == 0:
                print("Error! There's no file.")
            else:
                filename = file1[0]
                runningFunctions(type, filename)

        elif type == 1:    # FIFO
            file1 = find_file("../log/FIFOLog.txt")
            if len(file1) == 0:
                print("Error! There's no file.")
            else:
                filename = file1[0]
                runningFunctions(type, filename)
        else:
            print("\nUNIDENTIFIED TYPE OF ALGORITHM\n")
            exit(1)


if __name__ == "__main__":
    main()
