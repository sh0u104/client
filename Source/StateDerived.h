#pragma once
#include "Player.h"
#include "StateBase.h"
class IdleState : public State
{
public:
	// コンストラクタ
	IdleState(Player* player) :owner(player) {};
	// デストラクタ
	~IdleState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	Player* owner;
};

class MoveState : public State
{
public:
	// コンストラクタ
	MoveState(Player* player) :owner(player) {};
	// デストラクタ
	~MoveState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
protected:
	Player* owner;
	void Exit()override;
};

class JumpState : public State
{
public:
	// コンストラクタ
	JumpState(Player* player) :owner(player) {};
	// デストラクタ
	~JumpState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	Player* owner;
};

class LandState : public State
{
public:
	// コンストラクタ
	LandState(Player* player) :owner(player) {};
	// デストラクタ
	~LandState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	Player* owner;
};

class JumpFlipState : public State
{
public:
	// コンストラクタ
	JumpFlipState(Player* player) :owner(player) {};
	// デストラクタ
	~JumpFlipState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	Player* owner;
};

class AttackState : public State
{
public:
	// コンストラクタ
	AttackState(Player* player) :owner(player) {};
	// デストラクタ
	~AttackState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
protected:
	Player* owner;
};