#pragma once
#include "Graphics/Sprite.h"
#include "Scene.h"
#include "PlayerManager.h"
#include "Connection.h"
class SceneConnection : public Scene
{
public:
    SceneConnection() {};
    ~SceneConnection() override {}

    // ‰Šú‰»
    void Initialize() override;

    // I—¹‰»
    void Finalize() override;
    // XVˆ—
    void Update(float elapsedTime)override;
    // •`‰æˆ—
    void Render() override;

    void RenderNetError(ID3D11DeviceContext* dc);
    void RenderLogin(ID3D11DeviceContext* dc);
    void RenderName(ID3D11DeviceContext* dc);

    bool UiClick(float posX, float posY, float sizeX, float sizeY);

private:
    PlayerManager* playerManager = nullptr;
    Connection* connection = nullptr;

    enum class SpriteNumber : int
    {
        NetError,
        OK,
        Guest,
        Login,
        NewLogin,
        Name,
        Max
    };
    std::unique_ptr<Sprite> sprites[(int)SpriteNumber::Max] = {};

    bool isGuest = false;
    bool isLogin = false;
    bool isNewLogin = false;
    char name[10];
    char pass[10];
   
};

