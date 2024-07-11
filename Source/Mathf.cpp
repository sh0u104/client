#include "Mathf.h"
#include <stdlib.h>

// 線形補完
float Mathf::Lerp(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}

float Mathf::RandomRange(float min, float max)
{
    // pos.x = RandomRange();
    // pos.z = RandomRange();
    //(rand() / (float)RAND_MAX) * 5.0f + 10.0f;
    // 0.0~1.0の間までのランダム値
    float value = static_cast<float>(rand()) / RAND_MAX;

    // min~maxまでのランダム値に変換
    // 0~1までをminでそこさげする。
    return min + (max - min) * value;

    //return ((float)rand() / RAND_MAX)*(max - min)+min;
    //return (rand() / (float)RAND_MAX)*-min - +max;
}
