#pragma once

#include "Graphics/Shader.h"

// 前方宣言　相互インクルードしないように
class ProjectileManager;

// 弾丸
class Projectile
{
public:
    // シングルトンにはしない　エネミーとは違う
    //Projectile() {}
    // 登録されるマネージャーを保持するようにする。
    Projectile(ProjectileManager* manager);// 消すタイミングで保持しているものがほしい
    virtual ~Projectile() {}

    // 更新処理
    virtual void Update(float elapsedTime) = 0;

    // 描画処理
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive();

    // 破壊　
    void Destroy();

    // 位置取得
    const DirectX::XMFLOAT3& GetPosition() const { return position;  }

    // 半径取得
    float GetRadius() const { return radius; }

    // 方向取得 アングルの変わり
    const DirectX::XMFLOAT3& GetDirection() const { return direction;  }

    // スケール取得
    const DirectX::XMFLOAT3& GetScale() const { return scale;  }



protected:
    // 行列更新処理
    void UpdateTransform();

protected:
    DirectX::XMFLOAT3     position = { 0, 0, 0 };
    DirectX::XMFLOAT3     direction = { 0, 0, 1 };// 方向
    DirectX::XMFLOAT3     scale = { 1, 1, 1 };
    // 移動　回転　縮地
    DirectX::XMFLOAT4X4   transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    ProjectileManager* manager = nullptr;
    // 半径弾だから
    float  radius = 0.5f;
};