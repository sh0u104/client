#pragma once

#include <DirectXMath.h>

#include "Graphics/Model.h"

// ヒット結果
struct HitResult
{
    DirectX::XMFLOAT3 position = { 0,0,0 }; // レイとポリゴンの交点
    DirectX::XMFLOAT3 normal = { 0,0,0 }; // 衝突したポリゴンの法線ベクトル
    float distance = 0.0f; // レイの始点から交点までの距離
    int materialIndex = -1; //衝突したポリゴンのマテリアル番号
    DirectX::XMFLOAT3 rotation = { 0,0,0 }; // 回転量
};


// コリジョン
class Collision
{
public:
    // 球と球の交差判定
    static bool IntersectSpherVsSphere(
        // 動かない
        const DirectX::XMFLOAT3& positionA,// 始点
        float radiusA,// 当たり判定始点
        const DirectX::XMFLOAT3& positionB,// 終点
        float radiusB,// 当たり判定終点
        // 出された結果
        DirectX::XMFLOAT3& outPositionB// 結果
    );

    // 円柱と円柱の交差判定　うその
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,//ミスター
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,//スライム
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB// 押された相手が変える
    );

    // 球と円柱の交差判定　うその
    static bool IntersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius,
        float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    // レイとモデルの交差判定
    static bool IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
        const Model* model,
        HitResult& result
    );
};