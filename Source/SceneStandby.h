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

    SceneStandby(){}
    ~SceneStandby(){}
    // èâä˙âª
    void Initialize() override;

    // èIóπâª
    void Finalize() override;
    // çXêVèàóù
    void Update(float elapsedTime)override;
    // ï`âÊèàóù
    void Render() override;

private:
    enum class Spritenumber : int
    {
        Number,
        Flag,
        Team,
        TeamSelect,
        TeamCreate,
        TeamJoin,
        TeamJoinFrame,
        TeamjoinNumber,
        TeamNumber,
        Close,
        Ready,
        ReadyzCancel,
        Solo,
        Multi,
      //ModeSelect,
        Max
    };
    std::unique_ptr<Sprite> sprites[static_cast<int>(Spritenumber::Max)] = {};

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

    void RenderTeam(ID3D11DeviceContext* dc);
    void RenderTeamSelect(ID3D11DeviceContext* dc);
    void RenderTeamJoin(ID3D11DeviceContext* dc);
    void RenderReady(ID3D11DeviceContext* dc,bool isready);
    void RenderMode(ID3D11DeviceContext* dc);
    char input[32];

    bool Uiclick(float posX,float posY,float sizeX,float sizeY);
    std::vector<int>numbers;
    int posxy[12][2];
public:
    //int guimyid;
    //int guiteamnumber;
    int guiteamsid[4];
   
    char name[10];
    char pass[10];

    bool signupok = false;

    bool signinflag = false;
    bool signupflag = false;
    bool geustloginflag = false;

    short searchId = {};
    bool searchflag = false;
   
    bool teamscreenflag = false;
    bool numberinputflag = false;

   
};

