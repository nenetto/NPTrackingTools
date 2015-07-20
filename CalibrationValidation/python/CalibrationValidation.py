#!/usr/bin/env python

'''
@defgroup Optitrack
@author E. Marinetto
@date 2015-07-14
'''

from PySide import QtCore, QtGui, QtUiTools
import sys
from pandas import DataFrame, read_csv
import numpy as np
import pandas as pd
from subprocess import call

class CalibrationValidation():

    def __init__(self, parent=None):
        app = QtGui.QApplication(sys.argv)
        self.loadUiWidget("CalibrationTest.ui")
        self.ui.show()
        self.logic = CalibrationValidationsLogic(self.ui)
        self.setup()
        sys.exit(app.exec_())

    def setup(self):

        self.ui.analyze.setEnabled(False)

        self.ui.run.connect(QtCore.SIGNAL ('clicked()'), self.logic.run)
        self.ui.analyze.connect(QtCore.SIGNAL ('clicked()'), self.logic.analyze)
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

        self.ui.ResultLabel.setText('Running Test...')

        # Block buttons and wait for running the code for testing

        stringParams = self.calFile + ' ' + str(self.thr) + ' ' + str(self.exp) + ' ' + str(self.led) + ' ' + self.resFile
        print stringParams
        call([r"N:/install/bin/CalibrationValidation.exe ", stringParams])



        self.ui.analyze.setEnabled(True)

        self.ui.ResultLabel.setText('Finished')


    def analyze(self):
        Location = self.resFile
        df = pd.read_csv(Location, sep=';',usecols=['TimeStamp','MarkerIndex','X_3D','Y_3D','Z_3D','CameraUsed1','CameraUsed2','CameraPair'])
        df.columns = ['TimeStamp','MarkerIndex','X_3D','Y_3D','Z_3D','CameraUsed1','CameraUsed2', 'CameraPair']

        # Get number of cameras
        CameraNumber = max(df['CameraUsed1'].max(), df['CameraUsed2'].max())
        print "Camera Number = ", CameraNumber

        # For each camera, check the pairs and extract the number of NaN and the mean and std of the X, Y and Z 3D location

        dfsummary = pd.DataFrame(np.random.randn(CameraNumber+1, 9), columns=['Camera', 'Misscalibrated', 'NaN Index', 'Xmean diff', 'Ymean diff', 'Zmean diff', 'Xstd', 'Ystd', 'Zstd'])

        dfAll = df[df['CameraPair'] == 'All']

        Xmean3D = dfAll['X_3D'].mean()
        Ymean3D = dfAll['Y_3D'].mean()
        Zmean3D = dfAll['Z_3D'].mean()

        Xstd3D = dfAll['X_3D'].std()
        Ystd3D = dfAll['Y_3D'].std()
        Zstd3D = dfAll['Z_3D'].std()

        dfsummary.loc[0] = ['All Cameras','-','-',Xmean3D, Ymean3D, Zmean3D, Xstd3D, Ystd3D, Zstd3D]

        dfPairs = df[df['CameraPair'] == 'Pair']

        CompleteSystem = True

        for i in range(CameraNumber):
            #print "Checking Camera ", i+1

            df_pairs = dfPairs[(dfPairs['CameraUsed1'] == i+1) | (dfPairs['CameraUsed2'] == i+1)]

            try:
                NANumber = len(df_pairs[df_pairs['MarkerIndex'] == 'NoMarkers'].index)

            except:
                NANumber = 0

            indexNan = NANumber/(len(df_pairs.index)+0.0)

            #print "    NAs = ", NANumber, "of ", len(df_pairs.index), " Index = ", indexNan

            Xmean = df_pairs['X_3D'].mean()
            Ymean = df_pairs['Y_3D'].mean()
            Zmean = df_pairs['Z_3D'].mean()

            Xstd = df_pairs['X_3D'].std()
            Ystd = df_pairs['Y_3D'].std()
            Zstd = df_pairs['Z_3D'].std()

            #print "Mean point [ %f , %f , %f ] +- [ %f , %f , %f ]" % (Xmean, Ymean, Zmean, Xstd, Ystd, Zstd)

            Xstdmean = (Xstd + Xstd3D)
            Ystdmean = (Ystd + Ystd3D)
            Zstdmean = (Zstd + Zstd3D)

            #print "Mean std [ %f , %f , %f ]" % (Xstdmean, Ystdmean, Zstdmean)

            factor = 1.5
            condition = (indexNan > 0.05) | (abs(Xmean-Xmean3D) > (factor * Xstdmean)) | (abs(Ymean-Ymean3D) > (factor * Ystdmean)) | (abs(Zmean-Zmean3D) > (factor * Zstdmean))

            if condition:
                misscal = 'Miscalibrated'
                CompleteSystem = CompleteSystem & False
            else:
                misscal = 'Calibrated'
                CompleteSystem = CompleteSystem & True


            dfsummary.loc[i+1] = [i+1,misscal,indexNan,abs(Xmean-Xmean3D), abs(Ymean - Ymean3D), abs(Zmean - Zmean3D), Xstd, Ystd, Zstd]


            df_pairsMarkers = df_pairs[df_pairs['MarkerIndex'] != 'NoMarkers']
            if df_pairsMarkers['MarkerIndex'].mean() > 0:
                # More than one marker in FoV
                dfsummary.loc[i+1] = [i+1,'More than one marker','Error','Error', 'Error', 'Error', 'Error', 'Error', 'Error']

        print "Summary of Analysis"

        if CompleteSystem:
            dfsummary.loc[0] = ['All Cameras','CALIBRATED','-',Xmean3D, Ymean3D, Zmean3D, Xstd3D, Ystd3D, Zstd3D]
            self.ui.ResultLabel.setText('CALIBRATED')
        else:
            dfsummary.loc[0] = ['All Cameras','MISCALIBRATED','-',Xmean3D, Ymean3D, Zmean3D, Xstd3D, Ystd3D, Zstd3D]
            self.ui.ResultLabel.setText('MISCALIBRATED')

        dfsummary

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