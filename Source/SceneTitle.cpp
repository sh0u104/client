#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"

#include "SceneStandby.h"
#include "SceneConnection.h"
// 初期化
void SceneTitle::Initialize()
{
    // スプライト初期化
    sprite = new Sprite("Data/Sprite/Title.png");

}
// 終了化
void SceneTitle::Finalize()
{
    // スプライト終了化
    if (this->sprite)
    {
        delete sprite;
        sprite = nullptr;
    }
}
// 更新処理
void SceneTitle::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // 何かボタンを押したらローディングをはさんでゲームシーンへ切り替え
    const GamePadButton anyButton =
        GamePad::BTN_A|
    GamePad::BTN_B|
    GamePad::BTN_X|
    GamePad::BTN_Y;

   if (gamePad.GetButton() & anyButton)// ロードの次ゲームという書き方
       //SceneStandby
       //SceneGame
       //SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
       SceneManager::Instance().ChangeScene(new SceneLoading(new SceneConnection));
        

}
// 描画処理
void SceneTitle::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // 画面クリア＆レンダーターゲット設定
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };   // RGBA(0.0~1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // 2Dスプライト描画
    {
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());
        float textureWidth = static_cast<float>(sprite->GetTextureWidth());
        float textureHeight = static_cast<float>(sprite->GetTextureHeight());
        // 描画
        sprite->Render(dc,
            0, 0, screenWidth, screenHeight,
            0, 0, textureWidth, textureHeight,
            0,
            1, 1, 1, 1);
        // {位置}{サイズ}{画像どこから}{画像何処まで}
        // dc , ｛範囲｝｛｝
    }

  
}

