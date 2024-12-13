#include "StageMain.h"

// �R���g���X�g
StageMain::StageMain()
{
    // �X�e�[�W���f����ǂݍ���
    model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
    //model = new Model("Data/Model/Cube/Cube.mdl");

    //scale.x = scale.z = 100.0f;
    //scale.y = 0.5f;
}

StageMain::~StageMain()
{
    // �X�e�[�W���f����j��
    delete model;
}

// �X�V����
void StageMain::Update(float elasedTime)
{
   //UpdateTransform();
   //// ���C�L���X�g�p�ɂ��ł��ԍs��ɂ��邽�ߒP�ʍs���n��
   //const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0 ,0,1,0,0, 0,0,1,0, 0,0,0,1 };
   //// ���[�J���p�̏����l�O�𑗂�B
   //model->UpdateTransform(transformIdentity);
}

// �`�揈��
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    // �\���p�̂��߃��[���h�s��ɍX�V����
    //model->UpdateTransform(transform);

    shader->Draw(dc, model);
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end,
    HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}

void StageMain::UpdateTransform()
{
    //DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    //
    //DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    //DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //DirectX::XMMATRIX W = S * R * T;
    //DirectX::XMStoreFloat4x4(&transform, W);
}
