#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"


// 直進弾丸
class ProjectileStraight : public Projectile
{
public:
  /*  ProjectileStraight();*/
    ProjectileStraight(ProjectileManager* manager);
    ~ProjectileStraight()override;

    // 更新処理 継承したらoverrideする
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader)override;

    // 発射    (  どっちからはっしゃ向き、位置何処から)
    void Lanch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

    // 反射弾丸更新処理
    void UpdateReflection(float elapsedTime);

private:
    Model* model = nullptr;
    float  speed = 10.0f;// １秒間１０メートル
    DirectX::XMFLOAT3 velocity;

    // 弾丸生命時間
    float   lifeTimer = 3.0f;

    float stepOffset = 1.0f;

};
