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

    bool Uiclick(float posX, float posY, float sizeX, float sizeY);
private:
    
    PlayerManager* playerManager = nullptr;
    Connection* connection = nullptr;
    enum class Spritenumber : int
    {
        NetError,
        OK,
        Guest,
        Login,
        NewLogin,
        Max
    };
    std::unique_ptr<Sprite> sprites[(int)Spritenumber::Max] = {};

    bool isGuest = false;
    bool isLogin = false;
    bool isNewLogin = false;
    char name[10];
    char pass[10];
};

