#include "Camera.h"

// �w�����������

void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
    // ���_�A�@�����_�A���������r���[�s����쐬
    DirectX::XMVECTOR Eye = {eye.x,eye.y,eye.z};
    /*DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);*/
    // XMLoadFloat3(&eye);
    DirectX::XMVECTOR Focus = { focus.x,focus.y,focus.z};
    DirectX::XMVECTOR Up = { up.x,up.y,up.z};
    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
    DirectX::XMStoreFloat4x4(&view, View);
    // �܂Ƃ߂�

    // �r���[���t�s�񉻂��A���[���h�s��ɖ߂�  (nullptr�قڐ�΂��邯�ǎg���,View)
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    // �J�����̕��������o��
    this->right.x = world._11;
    this->right.y = world._12;
    this->right.z = world._13;

    this->up.x = world._21;
    this->up.y = world._22;
    this->up.z = world._23;

    this->front.x = world._31;
    this->front.y = world._32;
    this->front.z = world._33;
    
    // ���_�A�����_��ۑ�
    this->eye = eye;
    this->focus = focus;
}
// �n�[�X�y�N�e�B�u�ݒ�
void Camera::SetPerspedtiveFov(float forY, float aspect, float nearZ, float farZ)
{
    // �揑���A��ʔ䗦�A�N���b�v��������v���W�F�N�g�s����쐬
    DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(forY, aspect, nearZ, farZ);
    DirectX::XMStoreFloat4x4(&projection, Projection);
    //���ł�view projection���g����̂���
}


