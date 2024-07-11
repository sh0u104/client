#pragma once

#include "Graphics//Model.h"
#include "Projectile.h"

// �ǔ��e��
class ProjectileHoming :public Projectile
{
public:
    ProjectileHoming(ProjectileManager* manager);
    ~ProjectileHoming() override;

    // �X�V����
    virtual void Update(float elapsedTime) override;

    // �`�揈��
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    // ����    (  �ǂ�������͂���������A�ʒu��������)�@target�Ɍ������ăz�[�~���O�X�s�[�h
    void Lanch(const DirectX::XMFLOAT3& direction,
               const DirectX::XMFLOAT3& position,
               const DirectX::XMFLOAT3& target);
private:
    Model* model = nullptr;
    DirectX::XMFLOAT3 target = { 0,0,0 };
    // �i���x
    float          moveSpeed = 10.0f;
    // ��]�̑��x
    float         turnSpeed = DirectX::XMConvertToRadians(180);// 
    float     lifeTimer = 3.0f;

};