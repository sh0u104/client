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

    // èâä˙âª
    void Initialize() override;

    // èIóπâª
    void Finalize() override;
    // çXêVèàóù
    void Update(float elapsedTime)override;
    // ï`âÊèàóù
    void Render() override;

    void Signin();
    bool httpSignin();

    void Signup();
    bool httpSignup();

    int GetDataJson();
    bool NameJson();
    void pngDownload();
    bool httpPngDownload();


    bool UiClick(float posX, float posY, float sizeX, float sizeY);

    void RenderNetError(ID3D11DeviceContext* dc,float screenWidth,float screenHeight);
    void RenderLogin(ID3D11DeviceContext* dc, float screenWidth, float screenHeight);
    void RenderName(ID3D11DeviceContext* dc);


   
private:
    PlayerManager* playerManager = nullptr;
    Connection* connection = nullptr;

    bool isGuest = false;
    bool isSignin = false;
    bool isSignup = false;
    char name[10] = {};
    char pass[10] = {};

    int maxID = 0;
    int ID = 0;
    char Name[10] = {};
    int loginDay = 0;

    bool isname = false;

    bool Getfile = false;

   //std::string hostname = "10.108.0.18";
   std::string hostname = "localhost";
};

