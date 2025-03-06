#pragma once
#include "Graphics/Sprite.h"
#include "Scene.h"

#include "Player.h"
#include "PlayerManager.h"

#include "CameraController.h"

#include "Connection.h"

class SceneStandby  :public Scene
{
public:

    SceneStandby() {}
    ~SceneStandby(){}
    // èâä˙âª
    void Initialize() override;

    // èIóπâª
    void Finalize() override;
    // çXêVèàóù
    void Update(float elapsedTime)override;
    // ï`âÊèàóù
    void Render() override;

    void OprerationSelect(ID3D11DeviceContext* dc);
    void Logout(ID3D11DeviceContext* dc);

    void SyncPlayerGenerate();

    void LoginPlayerGenerate();

    bool isSetting = false;
private:

    CameraController* cameraController = nullptr;

    Player* player = nullptr;

    PlayerManager* playerManager = nullptr;
    Connection* connection = nullptr;

    int TeamNumber = 0;

    bool connectionflag = false;
    bool teamcreate = false;
    bool teamjoin = false;
    bool startcheck = false;

    bool sendteamcreate = false;
    bool sendteamjoin = false;
    bool sendgamestart = false;
    bool sendstartcheck = false;

    void RenderID(ID3D11DeviceContext* dc,
        const DirectX::XMFLOAT4X4& view,
        const DirectX::XMFLOAT4X4& projection
    );
 
    void  RenderTeamNumber(ID3D11DeviceContext* dc,
        const DirectX::XMFLOAT4X4& view,
        const DirectX::XMFLOAT4X4& projection
    );

    void RenderName(ID3D11DeviceContext* dc,
        const DirectX::XMFLOAT4X4& view,
        const DirectX::XMFLOAT4X4& projection);


    void RenderTeam(ID3D11DeviceContext* dc);
    void RenderTeamSelect(ID3D11DeviceContext* dc, float screenWidth, float screenHeight);
    void RenderTeamJoin(ID3D11DeviceContext* dc, float screenWidth, float screenHeight);
    void RenderReady(ID3D11DeviceContext* dc,float screenHeight,bool isready);
    void RenderMode(ID3D11DeviceContext* dc,float screenHeight);
    void RenderGameStart(ID3D11DeviceContext* dc, float screenHeight);
    char input[32] = {};

    bool Uiclick(float posX,float posY,float sizeX,float size);
    std::vector<int>numbers;
    float posxy[12][2] = {};
public:
    //int guimyid;
    //int guiteamnumber;
    int guiteamsid[4] = {};
   
    //char name[10];
    //char pass[10];

    bool signupok = false;

    bool signinflag = false;
    bool signupflag = false;
    bool geustloginflag = false;

    short searchId = {};
    bool searchflag = false;
   
    bool teamRenderflag = true;
    bool numberinputflag = false;

    bool debugGameStart = false;
};

