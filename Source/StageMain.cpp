#include "StageMain.h"

// コントラスト
StageMain::StageMain()
{
    // ステージモデルを読み込み
    model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
}

StageMain::~StageMain()
{
    // ステージモデルを破棄
    delete model;


}



// 更新処理
void StageMain::Update(float elasedTime)
{
    // 今は特にやることなし
}

// 描画処理
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
// シェーダーにモデルを描画してもらう
    shader->Draw(dc, model);

}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end,
    HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}
