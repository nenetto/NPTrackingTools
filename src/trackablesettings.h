
//==========================================================================----
//== Trackable Settings
//== Copyright NaturalPoint ==--
//==========================================================================----

#ifndef __TRACKABLESETTINGS_H__
#define __TRACKABLESETTINGS_H__

//==================================================================================-----

namespace Core
{
    const int kTrackableNameMaxLen = 32;
    const int kTrackableModelNameMaxLen = 256;
}

class cTrackableSettings
{
public:
    cTrackableSettings();

    char  mName     [Core::kTrackableNameMaxLen];
    char  mModelName[Core::kTrackableModelNameMaxLen];

    int   TrackableID;
    float ColorR;
    float ColorG;
    float ColorB;

    float MaxMarkerDeflection;
    int   MinimumMarkerCount;
    int   MinimumHitCount;
    int   DynamicOverride;
    int   StaticOverride;
    float MaxFrameRotation;
    float MaxFrameTranslation;
    bool  ShareMarkers;
    float Flexibility;

    bool  DynamicRotationConstraint;
    bool  DynamicTranslationConstraint;
    bool  StaticYawRotationConstraint;
    bool  StaticPitchRotationConstraint;
    bool  StaticRollRotationConstraint;

    float YawGreaterThan;
    float YawLessThan;
    float PitchGreaterThan;
    float PitchLessThan;
    float RollGreaterThan;
    float RollLessThan;

    double TranslationalSmoothing;
    double RotationalSmoothing;

    bool   DisplayUntracked;
    bool   DisplayPivot;
    bool   DisplayUntrackedMarkers;
    bool   DisplayMarkerQuality;
    bool   DisplayQuality;
    bool   Enabled;
    bool   DisplayTracked;
    bool   DisplayLabel;
    bool   DisplayOrientation;
    bool   DisplayModelReplace;

    float  ModelYaw;
    float  ModelPitch;
    float  ModelRoll;
    float  ModelX;
    float  ModelY;
    float  ModelZ;
    float  ModelScale;

    bool   DisplayPositionHistory;
    bool   DisplayOrientationHistory;
    int    DisplayHistoryLength;
    int    DisplayOrientationSpread;
    int    DisplayOrientationSize;

    int    AcquisitionFrames;
    double MaxCalculationTime;
    bool   ForceExhaustive;
    bool   PreciseOrientation;
};


#endif