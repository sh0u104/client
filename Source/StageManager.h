#pragma once

#include <vector>
#include "Stage.h"

// �X�e�[�W�}�l�[�W���[
class StageManager
{
private:
    StageManager() {}
    ~StageManager() {}

public:
    // �B��̃C���X�^���X�擾
    static StageManager& instance()
    {
        static StageManager instance;
        return instance;
    }

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // �X�e�[�W�o�^
    void Register(Stage* stage);

    // �X�e�[�W�S�폜
    void Clear();

    // ���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    // �X�t�B�A�L���X�g
    bool SphereCast(
        const DirectX::XMFLOAT3& origin,
        const DirectX::XMFLOAT3& direction,
        float radius,
        float& distance,
        DirectX::XMFLOAT3& hitPosition,
        DirectX::XMFLOAT3& hitNormal);

    struct CollisionMesh
    {
        struct Triangle
        {
            DirectX::XMFLOAT3	positions[3];
            DirectX::XMFLOAT3	normal;
        };
        std::vector<Triangle>	triangles;
    };

    CollisionMesh						collisionMesh;

    // �X�t�B�A�L���X�g����
    struct SphereCastResult
    {
        DirectX::XMVECTOR	position = { 0, 0, 0 };	// ���C�ƃ|���S���̌�_
        DirectX::XMVECTOR	normal = { 0, 0, 0 };	// �Փ˂����|���S���̖@���x�N�g��
        float				distance = 0.0f; 		// ���C�̎n�_�����_�܂ł̋���
        DirectX::XMVECTOR	verts[3];
        int					materialIndex = -1; 	// �Փ˂����|���S���̃}�e���A���ԍ�
    };
private:
    std::vector<Stage*> stages;
};
