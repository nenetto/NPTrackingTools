import NPTrackingTools as otk

tracker = otk.NPTrackingTools(r'C:\Program Files\OptiTrack\Tracking Tools\lib\NPTrackingToolsx64.dll', True)

tracker.TT_Initialize()
tracker.TT_Shutdown()
tracker.TT_FinalCleanup()
