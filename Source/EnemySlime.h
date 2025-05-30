#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"

// アニメーション
enum  class EnemyAnimation
{
    Anim_IdleNormal,
    Anim_IdleBattle,
    Anim_Attack1,
    Anim_Attack2,
    Anim_WalkFWD,
    Anim_WalkBWD,
    Anim_WalkLeft,
    Anim_WalkRight,
    Anim_RunFWD,
    Anim_SceneSomthinST,
    Anim_SceneSomthinPRT,
    Anim_Taunt,
    Anim_Victory,
    Anim_GetHit,
    Anim_Dizzy,
    Anim_Die,
};
// スライム
class EnemySlime : public Enemy
{
public:
    EnemySlime();
    // virtualしたらoverrideが出来ないこともある
    // エネミー継承しているoverrideをつける継承先で呼ばれない可能性あり
    ~EnemySlime() override;

    // アニメーション
    enum  class Animation
    {
        Anim_IdleNormal,
        Anim_IdleBattle,
        Anim_Attack1,
        Anim_Attack2,
        Anim_WalkFWD,
        Anim_WalkBWD,
        Anim_WalkLeft,
        Anim_WalkRight,
        Anim_RunFWD,
        Anim_SceneSomthinST,
        Anim_SceneSomthinPRT,
        Anim_Taunt,
        Anim_Victory,
        Anim_GetHit,
        Anim_Dizzy,
        Anim_Die,
    };
    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    // override継承先にあるんだろうなと分かる。
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    // デバッグプリミティブ描画　デバッグ用
    void DrawDebugPrimitive();

    // 縄張り設定
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

   
    // ターゲット位置をランダム設定
    void SetRandomTargetPosition();
    // 目標地点へ移動
    void MoveToTarget(float elapsedTime, float speedRate);
    // プレイヤー索敵
    bool SearchPlayer();


   
    // ノードとプレイヤーの衝突処理
    void CollisitionNodeVsPlayer(const char* nodeName, float nodeRadius);

   
private:
   
   //// 徘徊ステートへ遷移
   //void TransitionWanderState();
   //// 徘徊ステート更新処理
   //void UpdateWanderState(float elapsedTime);
   //// 待機ステートへ遷移
   //void TransitionIdleState();
   //// 待機ステート更新処理
   //void UpdateIdleState(float elapsedTime);
   //// 追跡ステートへ遷移
   //void TransitionPursuitState();
   //// 追跡ステート更新処理
   //void UpdatePursuitState(float elapsedTime);
   //// 攻撃ステートへ遷移
   //void TransitionAttackState();
   //// 攻撃ステート更新処理
   //void UpdateAttackState(float elapsedTime);
   //// 戦闘待機ステートへ遷移
   //void TransitionIdleBattleState();
   //// 戦闘待機ステート更新処理
   //void UpdateIdleBattleState(float elapsedTime);
   //// ダメージステートへ遷移
   //void TransitionDamageState();
   //// ダメージステートへ更新処理
   //void UpdateDamageState(float elapsedTime);
   //// 死亡ステートへ遷移
   //void TransitionDeathState();
   //// 死亡ステート更新処理
   //void UpdateDeathState(float elapsedTime);


   
    
private:
protected:
    // 死亡した時に呼ばれる
    void OnDead() override;

    // ダメージを受けた時に呼ばれる
    void OnDamaged() override;

private:
   
    DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
    DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
    float territoryRange = 10.0f;
    float moveSpeed = 3.0f;
    float turnSpeed = DirectX::XMConvertToRadians(360);
    float stateTimer = 0.0f;

    float searchRange = 5.0f;

    float attackRange = 1.5f;

    int nearPlayerId = 0;

public:
    DirectX::XMFLOAT3 GettargetPosition() { return this->targetPosition; }
    void SettargetPosition(DirectX::XMFLOAT3 position) { this->targetPosition = position; }

    void SetStateTimer(float timer) { this->stateTimer = timer; }
    float GetStateTimer() { return this->stateTimer; }

    int GetNearPlayerId() { return nearPlayerId; }
    float GetAttackRange(){return attackRange; }
};