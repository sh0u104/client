#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"


// ’¼i’eŠÛ
class ProjectileStraight : public Projectile
{
public:
  /*  ProjectileStraight();*/
    ProjectileStraight(ProjectileManager* manager);
    ~ProjectileStraight()override;

    // XVˆ— Œp³‚µ‚½‚çoverride‚·‚é
    void Update(float elapsedTime) override;

    // •`‰æˆ—
    void Render(ID3D11DeviceContext* dc, Shader* shader)override;

    // ”­Ë    (  ‚Ç‚Á‚¿‚©‚ç‚Í‚Á‚µ‚áŒü‚«AˆÊ’u‰½ˆ‚©‚ç)
    void Lanch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

    // ”½Ë’eŠÛXVˆ—
    void UpdateReflection(float elapsedTime);

private:
    Model* model = nullptr;
    float  speed = 10.0f;// ‚P•bŠÔ‚P‚Oƒ[ƒgƒ‹
    DirectX::XMFLOAT3 velocity;

    // ’eŠÛ¶–½ŠÔ
    float   lifeTimer = 3.0f;

    float stepOffset = 1.0f;

};
