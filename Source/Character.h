#pragma once

#include <DirectXMath.h>
#include "Effect.h"
// キャラクター
class Character
{
public:
    Character() = default;
    virtual ~Character() = default;

    // 中身を変えられないようにconst
    // ＆をつけた理由｛｝の中の物の本体を参照を渡す変えられないようにして
    // 行列更新処理
    void UpdateTransform();

    // 位置取得
    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    // 位置設定
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // 回転取得
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }

    // 回転設定
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    // スケール取得
    const DirectX::XMFLOAT3& GetSclae() const { return scale; }
    // スケール取得
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    // 衝撃を与える
    void AddImpulse(const DirectX::XMFLOAT3& impulse);

    // 半径取得 05
    float GetRadius() const { return radius; }

    // 地面に設置しているか エフェクト等も
    bool IsGround() const { return isGround; }

    // 高さ取得
    float GetHeight() const { return height; }

    // ダメージを与える ダメージを受けたら他に渡す。
    /*bool ApplyDamage(int damage);*/
    bool ApplyDamage(int damage, float invincibleTime);

    // 健康状態を取得
    int GetHealth() const { return health; }
    // 最大健康状態を取得
    int GetMaxHealth() const { return maxHealth; }


    DirectX::XMFLOAT3 GetRecvPosition() { return this->RecvPosition; }
    void SetRecvPosition(DirectX::XMFLOAT3 position) { this->RecvPosition = position; }

    void IsEnemy() { isEnemy = true; }

   
protected:
    // 移動処理
    //void Move(float elapsedTime, float vx, float vz, float speed);
    void Move(float vx, float vz, float speed);

    // 旋回処理
    void Turn(float elapsedTime, float vx, float vz, float speed);

    // ジャンプ処理
    void Jump(float speed);

    // 速力処理更新
    void UpdateVelocity(float elapsedTime);
    void UpdateInbincibleTimer(float elapsedTime);

    // 着地した時に呼べれる
    virtual void OnLanding() {};

    // ダメージを受けた時に呼ばれる
    virtual void OnDamaged() {};
    // 死亡した時に呼ばれる
    virtual void OnDead() {};
private:
    // 垂直速力更新処理
    void UpdateVerticalVelocity(float elapsedFrame);

    // 垂直移動更新処理
    void UpdateVerticalMove(float elapsedTime);

    // 水平速力更新処理
    void UpdateHorizontalVelocity(float elapsedFrame);
    // 水平移動更新処理
    void UpdateHorizontalMove(float elapsedTime);

    // 反射弾丸更新処理
    void UpdateReflection(float elapsedTime);

    // 敵の垂直移動更新処理
    void EnemyVerticalMove(float elapsedTime);
    // 敵の水平移動更新処理
    void EnemyHorizontalMove(float elapsedTime);
   

protected:// 継承先でも使える
    DirectX::XMFLOAT3             position = { 0, 0, 0 };
    DirectX::XMFLOAT3             angle = { 0, 0, 0 };
    DirectX::XMFLOAT3             scale = { 1, 1, 1 };
    DirectX::XMFLOAT4X4 transform = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    // 05
    float radius = 0.5f;

    float              grabity = -1.0f;

    DirectX::XMFLOAT3  velocity = { 0,0,0 };

    //追加
    DirectX::XMFLOAT3 RecvPosition = { 0,0,0 };
   

    bool         isGround = false;
    //メートル
    float               height = 2.0f;

    int          health = 5;
    int          maxHealth = 5;

    float   invincibleTimer = 1.0f;
    // 摩擦
    float   friction = 0.5f;

    float acceleration = 1.0f;
    float maxMoveSpeed = 5.0f;
    float moveVecX = 0.0f;
    float moveVecZ = 0.0f;

    float airControl = 0.3f;
    float stepOffset = 1.0f;
    float slopeRate = 1.0f;

    bool isEnemy = false;
   
};