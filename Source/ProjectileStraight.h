#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"


// ���i�e��
class ProjectileStraight : public Projectile
{
public:
  /*  ProjectileStraight();*/
    ProjectileStraight(ProjectileManager* manager);
    ~ProjectileStraight()override;

    // �X�V���� �p��������override����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader)override;

    // ����    (  �ǂ�������͂���������A�ʒu��������)
    void Lanch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

    // ���˒e�ۍX�V����
    void UpdateReflection(float elapsedTime);

private:
    Model* model = nullptr;
    float  speed = 10.0f;// �P�b�ԂP�O���[�g��
    DirectX::XMFLOAT3 velocity;

    // �e�ې�������
    float   lifeTimer = 3.0f;

    float stepOffset = 1.0f;

};
