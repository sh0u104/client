#pragma once
#include "Player.h"
#include "StateBase.h"
class IdleState : public State
{
public:
	// コンストラクタ
	IdleState(Player* player) :State(player) {};
	// デストラクタ
	~IdleState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

class MoveState : public State
{
public:
	// コンストラクタ
	MoveState(Player* player) :State(player) {};
	// デストラクタ
	~MoveState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

class JumpState : public State
{
public:
	// コンストラクタ
	JumpState(Player* player) :State(player) {};
	// デストラクタ
	~JumpState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

class LandState : public State
{
public:
	// コンストラクタ
	LandState(Player* player) :State(player) {};
	// デストラクタ
	~LandState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

class JumpFlipState : public State
{
public:
	// コンストラクタ
	JumpFlipState(Player* player) :State(player) {};
	// デストラクタ
	~JumpFlipState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};