#pragma once
#include "EnemySlime.h"
#include "StateBase.h"

// ステート
//enum class State
//{
//	Wander,
//	Idle,
//	Pursuit,
//	Attack,
//	IdleBattle,
//	Damage,
//	Death
//};
class EnemyWanderState : public State
{
public:
	// コンストラクタ
	EnemyWanderState(EnemySlime* enemy) :owner(enemy){};
	// デストラクタ
	~EnemyWanderState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyIdleState : public State
{
public:
	// コンストラクタ
	EnemyIdleState(EnemySlime* enemy) :owner(enemy) {};
	// デストラクタ
	~EnemyIdleState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyPursuitState : public State
{
public:
	// コンストラクタ
	EnemyPursuitState(EnemySlime* enemy) :owner(enemy) {};
	// デストラクタ
	~EnemyPursuitState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyAttackState : public State
{
public:
	// コンストラクタ
	EnemyAttackState(EnemySlime* enemy) :owner(enemy) {};
	// デストラクタ
	~EnemyAttackState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyIdleBattleState : public State
{
public:
	// コンストラクタ
	EnemyIdleBattleState(EnemySlime* enemy) :owner(enemy) {};
	// デストラクタ
	~EnemyIdleBattleState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyDamageState : public State
{
public:
	// コンストラクタ
	EnemyDamageState(EnemySlime* enemy) :owner(enemy) {};
	// デストラクタ
	~EnemyDamageState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyDeathState : public State
{
public:
	// コンストラクタ
	EnemyDeathState(EnemySlime* enemy) :owner(enemy) {};
	// デストラクタ
	~EnemyDeathState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	EnemySlime* owner;
};