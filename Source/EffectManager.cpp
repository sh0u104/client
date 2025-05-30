#include "Graphics/Graphics.h"
#include "EffectManager.h"

// 初期化
void EffectManager::Intialize()
{
    Graphics& graphics = Graphics::Instance();

    // Effekseer レンダラ生成
    effekseerRenderer = EffekseerRendererDX11::Renderer::Create(graphics.GetDevice(),
        graphics.GetDeviceContext(), 2048);

    // Effekseerマネージャー生成
    effekseerManager = Effekseer::Manager::Create(2048);

    // Effekseerレンダラの各種設定（特別なカスタマイズをしない場合は定型敵に以下の設定でOK）
    effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
    effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
    effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
    effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
    effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
    // Effekseer内でのローダーの設定（特別なカスタマイズをしない場合は以下の設定でOK）
    effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
    effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
    effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

    // Effekseerを左て座標系で計算する
    effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

}
// 終了化
void EffectManager::Finalize()
{
    // EffekseerManager等はスマートポインタによって破棄されるので何もしない
}
// 更新処理
void EffectManager::Update(float elapsedTime)
{
    // エフェクト更新処理（引数にはフレーム経過時間を渡す）
    effekseerManager->Update(elapsedTime * 60.0f);
}
// 描画処理
void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    // ビュー＆プロジェクション行列をEffekseerレンダラに設定
    effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
    effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const
        Effekseer::Matrix44*>(&projection));

    // Effekseer描画開始
    effekseerRenderer->BeginRendering();

    // Effekseer描画実行
    // マネージャー単位で描画するので描画順を正業する場合はマネージャーを複数個作成し、
    // Ｄｒａｗ（）関数を実行する順序で制御出来そう
    effekseerManager->Draw();

    // Effekseer描画終了
    effekseerRenderer->EndRendering();

}
