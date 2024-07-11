#pragma once


#include "Graphics/Model.h"

#include "Stage.h"


// �X�e�[�W
class StageMain : public Stage
{
public:

    StageMain();
    ~StageMain() override;



    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    // ���C�L���X�g
    bool RayCast(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        HitResult& hit) override;

private:
    Model* model = nullptr;
};
