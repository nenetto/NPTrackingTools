#!/usr/bin/env python

'''
@defgroup Optitrack
@author E. Marinetto
@date 2015-07-14
'''

from PySide import QtCore, QtGui, QtUiTools
import sys

class CalibrationValidation():

    def __init__(self, parent=None):
        app = QtGui.QApplication(sys.argv)
        self.loadUiWidget("CalibrationTest.ui")
        self.ui.show()
        self.logic = CalibrationValidationsLogic(self.ui)
        self.setup()
        sys.exit(app.exec_())

    def setup(self):

        self.ui.run.connect(QtCore.SIGNAL ('clicked()'), self.logic.run)
        self.ui.exposure.connect(QtCore.SIGNAL ('valueChanged(int)'), self.logic.exposure)
        self.ui.threshold.connect(QtCore.SIGNAL ('valueChanged(int)'), self.logic.threshold)
        self.ui.led.connect(QtCore.SIGNAL ('valueChanged(int)'), self.logic.illumination)

    def loadUiWidget(self, uifilename, parent = None):
        loader = QtUiTools.QUiLoader()
        uifile = QtCore.QFile(uifilename)
        uifile.open(QtCore.QFile.ReadOnly)
        self.ui = loader.load(uifile, parent)
        uifile.close()


class CalibrationValidationsLogic():

    def __init__(self, ui):
        self.ui = ui
        self.exp = 1
        self.thr = 1
        self.led = 15
        self.calFile = "Calibration.cal"
        self.resFile = "Results.csv"

    def run(self):
        self.calFile = self.ui.calibrationFile.text()
        self.resFile = self.ui.resultfile.text()


        print "Running..."
        print "Parameters:"
        print "    Exp:", self.exp
        print "    Thr:", self.thr
        print "    Led:", self.led
        print "    Calibration File:", self.calFile
        print "    Result File:", self.resFile

        # Block buttons and wait for running the code for testing

        # Finish the test and perform the analysis

    def exposure(self, value):
        self.exp = value

    def threshold(self, value):
        self.thr = value

    def illumination(self, value):
        self.led = value

    def calFile(self):
        print self.ui.calibrationFile.text()
        self.calFile = self.ui.calibrationFile.text()

    def resFile(self, value):
        self.resFile = value


if __name__ == "__main__":
    CalibrationValidation()