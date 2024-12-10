#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController
{
public:
    CameraController() {}
    ~CameraController() {}

    // 更新処理
    void Update(float elasedTime);



    // ターゲット位置設定 ちゃんと変数を入れる
    void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

    void  DebugSetAngleX() { this->angle.x = 0.785f; }

    void SetangleY(float angle) { this->angle.y = angle; }
    // デバッグ用GUI描画
    void DrawDebugGUI();

    void SetRange(float range) { this->range = range; }
private:
    // 範囲
    float                   range = 15.0f;
    DirectX::XMFLOAT3       target = { 0, 0, 0 };
    DirectX::XMFLOAT3       angle = { 0, 0, 0 };
    float                   rollSpeed = DirectX::XMConvertToRadians(90);
    float                   maxAngleX = DirectX::XMConvertToRadians(45);
    float                   minAngleX = DirectX::XMConvertToRadians(-45);
};
