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

    // �f�o�b�O�pGUI�`��
    void DrawDebugGUI();


private:
    DirectX::XMFLOAT3       target = { 0, 0, 0 };
    DirectX::XMFLOAT3       angle = { 0, 0, 0 };
    float                   rollSpeed = DirectX::XMConvertToRadians(90);
    // �͈�
    float                   range = 10.0f;
    float                   maxAngleX = DirectX::XMConvertToRadians(45);
    float                   minAngleX = DirectX::XMConvertToRadians(-45);
};
