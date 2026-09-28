#pragma once

#include "helpers.h"

inline bool CollidedWithTerrain(Vector3 position, float worldSize, float ySize ,const Image& masterImg)
{
    Vector2 _2dPos = _3DXZTo2DXY({position.x, position.z}, worldSize, masterImg.width, masterImg.height);

    if(_2dPos.x < 0.0f || _2dPos.x >= masterImg.width || 
    _2dPos.y < 0.0f || _2dPos.y >= masterImg.height)
    {
        return false;    
    }

    Color pixelColor = GetImageColor(masterImg, (int)_2dPos.x, (int)_2dPos.y);

    float heightFromTerrain = (pixelColor.r / 255.0f) * ySize;

    if(position.y <= heightFromTerrain) return true;
   
    return false;
}