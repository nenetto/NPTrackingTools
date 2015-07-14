#!/usr/bin/env python

'''
@defgroup Optitrack
@author E. Marinetto
@date 2015-07-14
'''

from PySide import QtCore, QtGui, QtUiTools


def loadUiWidget(uifilename, parent=None):
    loader = QtUiTools.QUiLoader()
    uifile = QtCore.QFile(uifilename)
    uifile.open(QtCore.QFile.ReadOnly)
    ui = loader.load(uifile, parent)
    uifile.close()
    return ui


if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    MainWindow = loadUiWidget("CalibrationTest.ui")
    MainWindow.show()
    sys.exit(app.exec_())