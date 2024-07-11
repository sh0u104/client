#pragma once

#include "Graphics//Model.h"
#include "Projectile.h"

// 追尾弾丸
class ProjectileHoming :public Projectile
{
public:
    ProjectileHoming(ProjectileManager* manager);
    ~ProjectileHoming() override;

    // 更新処理
    virtual void Update(float elapsedTime) override;

    // 描画処理
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    // 発射    (  どっちからはっしゃ向き、位置何処から)　targetに向かってホーミングスピード
    void Lanch(const DirectX::XMFLOAT3& direction,
               const DirectX::XMFLOAT3& position,
               const DirectX::XMFLOAT3& target);
private:
    Model* model = nullptr;
    DirectX::XMFLOAT3 target = { 0,0,0 };
    // 進速度
    float          moveSpeed = 10.0f;
    // 回転の速度
    float         turnSpeed = DirectX::XMConvertToRadians(180);// 
    float     lifeTimer = 3.0f;

};