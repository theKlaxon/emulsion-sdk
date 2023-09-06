#ifndef GAMEPADUI_GRADIENT_HELPER_H
#define GAMEPADUI_GRADIENT_HELPER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamepadui.h"
#include "mathlib/mathlib.h"
#include "Color.h"

namespace GradientSides
{
    enum GradientSide
    {
        Left,
        Right,
        Up,
        Down,

        Count
    };
}
//using GradientSide = GradientSides::GradientSide;
using namespace GradientSides;

struct GradientInfo
{
    float flAmount;
    float flExtent;
};

class GradientHelper
{
public:
    GradientHelper()
    {
		memset( &m_LastGradientChange, 0, sizeof( m_LastGradientChange ) );
		memset( &m_LastGradientInfo, 0, sizeof( m_LastGradientInfo ) );
		memset( &m_TargetGradientInfos, 0, sizeof( m_TargetGradientInfos ) );
    }

    void ResetTargets( float flTime )
    {
        for ( int i = 0; i < GradientSides::Count; i++ )
            SetTargetGradient( (GradientSide)i, GradientInfo(), flTime);
            //SetTargetGradient( GradientSide(i), GradientInfo{}, flTime );
    }

    void SetTargetGradient( GradientSide side, GradientInfo info, float flTime )
    {
        m_LastGradientInfo[side]    = GetGradient( side, flTime );
        m_TargetGradientInfos[side] = info;
        m_LastGradientChange[side]  = flTime;
    }

    GradientInfo GetGradient( GradientSide side, float flTime )
    {
        const GradientInfo& target = m_TargetGradientInfos[side];
        const GradientInfo& last   = m_LastGradientInfo[side];

        float flFadeTime = SimpleSpline(
            RemapValClamped( flTime, m_LastGradientChange[side], m_LastGradientChange[side] + 0.5f, 0.0f, 1.0f ));

        //return GradientInfo // Note! very icky, no good
        //{
        //    Lerp( flFadeTime, last.flAmount, target.flAmount ),
        //    last.flAmount && target.flAmount && last.flExtent > target.flExtent
        //        ? Lerp( flFadeTime, last.flExtent, target.flExtent )
        //        : target.flExtent
        //};

        return
        {
            Lerp(flFadeTime, last.flAmount, target.flAmount),
            last.flAmount && target.flAmount && last.flExtent > target.flExtent
                ? Lerp(flFadeTime, last.flExtent, target.flExtent)
                : target.flExtent
        };
    }

private:
    float        m_LastGradientChange [ GradientSides::Count ];
    GradientInfo m_LastGradientInfo   [ GradientSides::Count ];
    GradientInfo m_TargetGradientInfos[ GradientSides::Count ];
};


#endif // GAMEPADUI_GRADIENT_HELPER_H
