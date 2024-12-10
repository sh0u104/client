#pragma once

#include <DirectXMath.h>

// �J�����R���g���[���[
class CameraController
{
public:
    CameraController() {}
    ~CameraController() {}

    // �X�V����
    void Update(float elasedTime);



    // �^�[�Q�b�g�ʒu�ݒ� �����ƕϐ�������
    void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

    void  DebugSetAngleX() { this->angle.x = 0.785f; }

    void SetangleY(float angle) { this->angle.y = angle; }
    // �f�o�b�O�pGUI�`��
    void DrawDebugGUI();

    void SetRange(float range) { this->range = range; }
private:
    // �͈�
    float                   range = 15.0f;
    DirectX::XMFLOAT3       target = { 0, 0, 0 };
    DirectX::XMFLOAT3       angle = { 0, 0, 0 };
    float                   rollSpeed = DirectX::XMConvertToRadians(90);
    float                   maxAngleX = DirectX::XMConvertToRadians(45);
    float                   minAngleX = DirectX::XMConvertToRadians(-45);
};
