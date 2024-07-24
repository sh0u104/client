#include "SceneConnection.h"
#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneStandby.h"

// 初期化
void SceneConnection::Initialize()
{
       
       //ネットワーク接続失敗
     // スプライト初期化
        sprites[static_cast<int>(Spritenumber::NetError)] = std::make_unique<Sprite>("Data/Sprite/neterror.png");
        //OKボタン
        sprites[static_cast<int>(Spritenumber::OK)] = std::make_unique<Sprite>("Data/Sprite/OK.png");


        //一度だけ
        if (!SceneManager::Instance().GetconnectionInitialized())
        {
            SceneManager::Instance().SetconnectionInitialized(true);
            //初期化
            SceneManager::Instance().PlayermanagerInitialize();
            SceneManager::Instance().ConnectionInitialize();
        }
       
        // プレイヤーマネジャー初期化
        playerManager = SceneManager::Instance().GetPlayerManager();
    
        connection = SceneManager::Instance().GetConnection();
       
    //接続処理
    if (!connection->isConnction)
    {
        connection->Initialize();
    }


}
// 終了化
void SceneConnection::Finalize()
{
  
}
// 更新処理
void SceneConnection::Update(float elapsedTime)
{
    //接続成功
    if (connection->isConnction)
    {
        // connection->SetplayerManager(playerManager);
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
    }
}
// 描画処理
void SceneConnection::Render()
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
    if (!connection->isConnction)
    {
        RenderNetError(dc);
    }


}

void SceneConnection::RenderNetError(ID3D11DeviceContext* dc)
{
    float positionX = 50;
    float positionY = 0;

    //枠組み
    sprites[static_cast<int>(Spritenumber::NetError)]->Render(dc,
        positionX, positionY, //描画位置
        600, 350,               //表示サイズ
        0, 0,                 //切り取りはじめ位置
        750, 500,           //画像サイズ
        0.0f,
        1, 1, 1, 1);

    //OKボタン
    sprites[static_cast<int>(Spritenumber::OK)]->Render(dc,
        positionX + 190, positionY + 250, //描画位置
        150, 50,               //表示サイズ
        0, 0,                 //切り取りはじめ位置
        400, 200,           //画像サイズ
        0.0f,
        1, 1, 1, 1);

    if (Uiclick(positionX + 190, positionY + 250, 150, 50))
    {
       
       if (!connection->isConnction)
       {
           connection->Initialize();
           //接続成功
           if (connection->isConnction)
           {
              // connection->SetplayerManager(playerManager);
               SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
           }
           //失敗
           else
           {
               SceneManager::Instance().ChangeScene(new SceneLoading(new SceneConnection));
           }
       }
       
    }
}

bool SceneConnection::Uiclick(float posX, float posY, float sizeX, float sizeY)
{
    DirectX::XMFLOAT3 scereenPosition;

    Mouse& mouse = Input::Instance().GetMouse();
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        scereenPosition.x = static_cast<float>(mouse.GetPositionX());
        scereenPosition.y = static_cast<float>(mouse.GetPositionY());

        if (posX < scereenPosition.x && scereenPosition.x < posX + sizeX
            && posY < scereenPosition.y && scereenPosition.y < posY + sizeY)
        {
            Sleep(10);
            return true;
        }
    }
    return false;
}