#pragma once

#include <DirectXMath.h>

#include "Graphics/Model.h"

// �q�b�g����
struct HitResult
{
    DirectX::XMFLOAT3 position = { 0,0,0 }; // ���C�ƃ|���S���̌�_
    DirectX::XMFLOAT3 normal = { 0,0,0 }; // �Փ˂����|���S���̖@���x�N�g��
    float distance = 0.0f; // ���C�̎n�_�����_�܂ł̋���
    int materialIndex = -1; //�Փ˂����|���S���̃}�e���A���ԍ�
    DirectX::XMFLOAT3 rotation = { 0,0,0 }; // ��]��
};


// �R���W����
class Collision
{
public:
    // ���Ƌ��̌�������
    static bool IntersectSpherVsSphere(
        // �����Ȃ�
        const DirectX::XMFLOAT3& positionA,// �n�_
        float radiusA,// �����蔻��n�_
        const DirectX::XMFLOAT3& positionB,// �I�_
        float radiusB,// �����蔻��I�_
        // �o���ꂽ����
        DirectX::XMFLOAT3& outPositionB// ����
    );

    // �~���Ɖ~���̌�������@������
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,//�~�X�^�[
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,//�X���C��
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB// �����ꂽ���肪�ς���
    );

    // ���Ɖ~���̌�������@������
    static bool IntersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius,
        float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    // ���C�ƃ��f���̌�������
    static bool IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
        const Model* model,
        HitResult& result
    );
};