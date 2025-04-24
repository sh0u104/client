#include "StageMain.h"

// コントラスト
StageMain::StageMain()
{
    // ステージモデルを読み込み
    //model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
    model = new Model("Data/Model/Cube/Cube.mdl");

    scale.x = scale.z = 30.0f;
    scale.y = 0.1f;
    position.y = -0.5f;
}

StageMain::~StageMain()
{
    // ステージモデルを破棄
    delete model;
}

// 更新処理
void StageMain::Update(float elasedTime)
{
   UpdateTransform();
   //// ローカル用の初期値０を送る。
   model->UpdateTransform(transform);
}

// 描画処理
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end,
    HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}

void StageMain::UpdateTransform()
{
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX W = S * R * T;
    DirectX::XMStoreFloat4x4(&transform, W);
}
