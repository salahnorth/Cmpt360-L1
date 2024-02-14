from graphics import *
from PIL import Image as NewImage
import glob
#import random

"""===========================================================================
Description: This file plots the data of the log file
Authors    : Salah Mohamed & Vy Tran
Date       : 04 February 2024
============================================================================"""


def loadRawData(filename):
    '''Purpose  : Loading data file
    Parameter   : filename
    Return      : data'''
    file = open(filename, "r")
    tmp = file.readline().strip()
    rawData = []  # time, PID, status, niceness, cputime, proctime
    while tmp:
        rawData.append(tmp)
        tmp = file.readline().strip()
    file.close()
    return rawData


def formatData(rData):
    '''Purpose  : Format raw data
    Parameter   : rData
    Return      : fData'''
    fData = {}
    for line in rData:
        line = line.split(", ")
        fData[int(line[1])] = float(line[4])
    return fData


def plotData(fData):
    '''Purpose  : Plotting the data
    Parameter   : fData
    Return      : none'''

    HEIGHT, WIDTH = 600, 600

    win = GraphWin("Plotting Data [Lab 2]", HEIGHT, WIDTH)

    # Beginning Points
    xBegin = 50
    yBegin = 187.5
    yEnd   = 312.5

    # Calculation
    xp = [xBegin]
    for k in fData.keys():
        iFlag = 0
        for j in range(len(xp)-1, -1, -1):
            if(iFlag == 0):
                # Ratio: Full Width == 100%
                x = (fData[k] * (WIDTH//2)/100) + xp[j]
                xp.append(x)
                xp.append(x)
                iFlag=1

    centerPoint = []
    colorList   = ["cyan", "red", "green"]
    for idx in range(len(xp)-1):
        r = Rectangle(Point(xp[idx], yBegin), Point(xp[idx+1], yEnd))
        r.draw(win)
        # Fill the rectangles with colors
        r.setFill(colorList[idx-3])
        centerPoint.append(r.getCenter())

        '''This part is used if one doesn't know how many rectangles to fill
        However, there's a small probability that it can generate the same
        colours, so we decided not to use this in this lab.
        r.setFill(color_rgb(random.randint(0, 250),
                            random.randint(0, 100),
                            random.randint(0, 250)))
        centerPoint.append(r.getCenter())'''

    # PID list
    kList = []
    for key in fData.keys():
        kList.append(key)
        kList.append(key)

    # Add labels
    for pos in range(0, len(centerPoint), 2):
        lb = Text(centerPoint[pos], kList[pos])
        lb.setTextColor("white")
        lb.setSize(11)
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

    # save postscipt image
    win.postscript(file="plotDataLab2.eps")
    # use PIL to save and change the file type to PNG
    img = NewImage.open("plotDataLab2.eps")
    img.save("plotDataLab2.png", "png")
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


def main():
    '''Purpose  : Run the program
    Parameter   : none
    Return      : none'''

    files = find_file("Complete*.txt")
    if len(files) == 0:
        print("Error! There's no file.")
    else:
        filename = files[0]
        print(filename)
        rData = loadRawData(filename)
        fData = formatData(rData)
        #print(fData)
        plotData(fData)


if __name__ == "__main__":
    main()
