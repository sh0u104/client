#include "StageMain.h"

// �R���g���X�g
StageMain::StageMain()
{
    // �X�e�[�W���f����ǂݍ���
    model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
}

StageMain::~StageMain()
{
    // �X�e�[�W���f����j��
    delete model;


}



// �X�V����
void StageMain::Update(float elasedTime)
{
    // ���͓��ɂ�邱�ƂȂ�
}

// �`�揈��
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
// �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, model);

}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end,
    HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}
