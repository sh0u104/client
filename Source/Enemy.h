#pragma once

#include "Graphics/Shader.h"
#include "Character.h"

#include "StateMachine.h"
// エネミー
class Enemy : public Character
{
public:

   
    Enemy() {}
    ~Enemy() override {}

    // 更新処理
    //純粋仮想関数を作ったら向こうで作らないと
    //コンパイルエラーになる
    //
    virtual void Update(float elapsedTime) = 0;

    // 描画処理　
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

    // 破棄
    void Destroy();

    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive();
    // ステート
    enum class State
    {
        Wander,
        Idle,
        Pursuit,
        Attack,
        IdleBattle,
        Damage,
        Death
    };

   
    int ID = -1;
    void SetMyEnemyId(int id) { this->ID = id; }
    int  GetMyEnemyId() { return this->ID; }
    State state = State::Wander;

    State GetState() { return this->state; }
    void SetState(State state) { this->state = state; }

    Model* model = nullptr;
    Model* GetModel() { return model; }

    StateMachine* GetStateMachine() { return stateMachine.get(); }
    std::shared_ptr<StateMachine> stateMachine;
};