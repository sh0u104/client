#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileManager.h"

#include "StateMachine.h"
// アニメーション
enum class Animation
{
    Anim_Attack,
    Anim_Death,
    Anim_Falling,
    Anim_GetHit1,
    Anim_GetHit2,
    Anim_Idle,
    Anim_Jump,
    Anim_Jump_Flip,
    Anim_Landing,
    Anim_Revive,
    Anim_Running,
    Anim_Walking
};
// プレイヤー
class Player : public Character
{
public:
    Player();
    ~Player() override;

    // インスタンス取得
    static Player& Instance();

    enum  Animation
    {
        Anim_Attack,
        Anim_Death,
        Anim_Falling,
        Anim_GetHit1,
        Anim_GetHit2,
        Anim_Idle,
        Anim_Jump,
        Anim_Jump_Flip,
        Anim_Landing,
        Anim_Revive,
        Anim_Running,
        Anim_Walking
    };
     


    // 更新処理
    void Update(float elapsedTime);

    // デバッグプリミティブ描画　デバッグ用
    void DrawDebugPrimitive();


    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);


    // 弾丸と敵の衝突処理
    void CollisionProjectilesVsEnemies();

    //// プレイヤーとエネミーとの衝突処理
    //void CollisionPlayerVsEnemies();

    // デバッグ用GUI描画
    void DrawDebugGUI();
protected:
    // 着地した時に呼ばれる
    void OnLanding() override;

    // 死亡した時に呼ばれる
    void OnDead() override;

    // ダメージを受けた時に呼ばれる
    void OnDamaged() override;
public:
    enum class State
    {
        Idle,
        Move,
        Land,
        Jump,
        JumpFlip,
        Attack,
    };
    StateMachine* GetStateMachine() { return stateMachine.get(); }
    Model* GetModel() { return model; }
public:
    // スティック入力値から移動ベクトルを取得 進行ベクトルを取る進むべき方向
    DirectX::XMFLOAT3 KeyGetMoveVec() const;
    DirectX::XMFLOAT3 MouseGetMoveVec() const;

    // 移動入力処理
    bool InputMove(float elapsedTime);

    // プレイヤーとエネミーとの衝突処理
    void CollisionPlayerVsEnemies();

    // ノードとエネミーの衝突処理
    void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

    // ジャンプ入力処理
    bool InputJump();

    // 弾丸入力
    bool InputProjectile();

    // 攻撃入力
    bool InputAttack();

    int            jumpCount = 0;

    void SetState(State state) { this->state = state; }
    
private:
    std::shared_ptr<StateMachine> stateMachine;
    Model* model = nullptr;

    float          moveSpeed = 5.0f;

    float         turnSpeed = DirectX::XMConvertToRadians(720);

    float          jumpSpeed = 20.0f;
   
    int            jumpLimit = 2;

    //ProjectileManager projectileManager;

    Effect* hitEffect = nullptr;
    Effect* desEffect = nullptr;

    State   state = State::Idle;
    State   stated;

    // 着地場所までの距離　 十分な速度で落とす重力の５倍２、３秒後に着地モーションをする。
   // int jumpfliptime =  grabity * 5;

    int    healse  = 10;

public:
    // ID
    int GetPlayerID() const { return playerID; }
    void SetPlayerID(int id) { playerID = id; }
    
    // 準備フラグ
    bool GetReady() { return ready; }
    void SetReady(bool flg) { ready = flg; }
    
    bool flagVelosity() { return velocity.x != 0 || velocity.y != 0 || velocity.z != 0; }
    DirectX::XMFLOAT3 GetVelocity() { return this->velocity; }
    void SetVelovity(DirectX::XMFLOAT3 velocity) { this->velocity = velocity; }
    
    bool Getoperation() { return operation;}
    void Setoperation(bool flag) { operation = flag;}
    
    State GetState() { return this->state; }
   
    void ChangeState(State state);

    //State GetSendState() { return this->sendState; }
    //State sendState = State::Idle;

 
    int Getteamnumber() { return teamNumber; }
    void Setteamnumber(int number) { teamNumber = number; }

    int Getteamsid(int number) { return teamsId[number]; }
    void Setteamsid(int number, int id) { this->teamsId[number] = id; }
    void ResetTeamsid() { std::fill(teamsId, teamsId + 4, 0); }

    bool GetstartCheck() { return startCheck; }
    void SetstartCheck(bool check) { this->startCheck = check; }

    const char* GetName() { return Name; }
    void SetName(char name[10]){ strcpy_s(this->Name, name); }

   
    void SetisMouseOperation(bool ismouse) { this->isMouseOperation = ismouse; }
    bool GetisMouseOperation() { return this->isMouseOperation; }

    void SetLoginDay(int day) { this->loginDay = day; }
    int GetLoginDay() { return this->loginDay; }

    float mouseAngle = 0;
    float mouselength = 0;


    void SetTeamHost(bool host) { this->teamHost = host; }
    bool GetTeamHost() { return this->teamHost; }

    void SetAttackCollisionFlag(bool attack) { this->attackCollisionFlag = attack; }
    bool GetAttackCollisionFlag() { return this->attackCollisionFlag; }

    float GetLeftHandRadius() { return this->leftHandRadius; }

    float GetPing() { return this->ping; }
    void SetPing(float ping) { this->ping = ping; }

protected:
    int playerID = 0;
    char Name[10] = {};
    int loginDay = 0;
  
    float ping = 0;
  
    int teamNumber = 0;
    int teamsId[4] = {};
    bool teamHost = false;

    bool operation = false;
    bool ready = false;
    bool startCheck = false;
    bool isMouseOperation = true;

    bool attackCollisionFlag = false;
    float leftHandRadius = 0.4f;
};