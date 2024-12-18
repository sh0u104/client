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
    // 初期化
    void Initialize() override;

    // 終了化
    void Finalize() override;
    // 更新処理
    void Update(float elapsedTime)override;
    // 描画処理
    void Render() override;

    void OprerationSelect(ID3D11DeviceContext* dc);
    void Logout(ID3D11DeviceContext* dc);

    void SyncPlayerGenerate();

    void LoginPlayerGenerate();

    bool isSetting = false;
private:
   //enum class Spritenumber : int
   //{
   //    Number,
   //    Flag,
   //    Team,
   //    TeamSelect,
   //    TeamCreate,
   //    TeamJoin,
   //    TeamJoinFrame,
   //    TeamjoinNumber,
   //    TeamNumber,
   //    Close,
   //    Ready,
   //    ReadyzCancel,
   //    Solo,
   //    Multi,
   //    Start,
   //    Name,
   //    Logout,
   //    WASD,
   //    SelectEdge,
   //    Setting,
   //    Mouse,
   //  //ModeSelect,
   //    Max
   //};
    //std::unique_ptr<Sprite> sprites[static_cast<int>(Spritenumber::Max)] = {};

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
    void RenderTeamSelect(ID3D11DeviceContext* dc);
    void RenderTeamJoin(ID3D11DeviceContext* dc);
    void RenderReady(ID3D11DeviceContext* dc,bool isready);
    void RenderMode(ID3D11DeviceContext* dc);
    void RenderGameStart(ID3D11DeviceContext* dc);
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

