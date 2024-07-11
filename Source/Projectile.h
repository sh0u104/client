#pragma once

#include "Graphics/Shader.h"

// �O���錾�@���݃C���N���[�h���Ȃ��悤��
class ProjectileManager;

// �e��
class Projectile
{
public:
    // �V���O���g���ɂ͂��Ȃ��@�G�l�~�[�Ƃ͈Ⴄ
    //Projectile() {}
    // �o�^�����}�l�[�W���[��ێ�����悤�ɂ���B
    Projectile(ProjectileManager* manager);// �����^�C�~���O�ŕێ����Ă�����̂��ق���
    virtual ~Projectile() {}

    // �X�V����
    virtual void Update(float elapsedTime) = 0;

    // �`�揈��
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive();

    // �j��@
    void Destroy();

    // �ʒu�擾
    const DirectX::XMFLOAT3& GetPosition() const { return position;  }

    // ���a�擾
    float GetRadius() const { return radius; }

    // �����擾 �A���O���̕ς��
    const DirectX::XMFLOAT3& GetDirection() const { return direction;  }

    // �X�P�[���擾
    const DirectX::XMFLOAT3& GetScale() const { return scale;  }



protected:
    // �s��X�V����
    void UpdateTransform();

protected:
    DirectX::XMFLOAT3     position = { 0, 0, 0 };
    DirectX::XMFLOAT3     direction = { 0, 0, 1 };// ����
    DirectX::XMFLOAT3     scale = { 1, 1, 1 };
    // �ړ��@��]�@�k�n
    DirectX::XMFLOAT4X4   transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    ProjectileManager* manager = nullptr;
    // ���a�e������
    float  radius = 0.5f;
};