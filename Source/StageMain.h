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
    // �s��X�V����
    void UpdateTransform();

    Model* model = nullptr;
    DirectX::XMFLOAT3  position = { 0,0,0 };
    DirectX::XMFLOAT3  angle = { 0,0,0 };
    DirectX::XMFLOAT3  scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
};
