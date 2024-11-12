#pragma once
#include "EnemySlime.h"
#include "StateBase.h"

// �X�e�[�g
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
	// �R���X�g���N�^
	EnemyWanderState(EnemySlime* enemy) :owner(enemy){};
	// �f�X�g���N�^
	~EnemyWanderState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyIdleState : public State
{
public:
	// �R���X�g���N�^
	EnemyIdleState(EnemySlime* enemy) :owner(enemy) {};
	// �f�X�g���N�^
	~EnemyIdleState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyPursuitState : public State
{
public:
	// �R���X�g���N�^
	EnemyPursuitState(EnemySlime* enemy) :owner(enemy) {};
	// �f�X�g���N�^
	~EnemyPursuitState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyAttackState : public State
{
public:
	// �R���X�g���N�^
	EnemyAttackState(EnemySlime* enemy) :owner(enemy) {};
	// �f�X�g���N�^
	~EnemyAttackState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyIdleBattleState : public State
{
public:
	// �R���X�g���N�^
	EnemyIdleBattleState(EnemySlime* enemy) :owner(enemy) {};
	// �f�X�g���N�^
	~EnemyIdleBattleState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyDamageState : public State
{
public:
	// �R���X�g���N�^
	EnemyDamageState(EnemySlime* enemy) :owner(enemy) {};
	// �f�X�g���N�^
	~EnemyDamageState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
protected:
	EnemySlime* owner;
};

class EnemyDeathState : public State
{
public:
	// �R���X�g���N�^
	EnemyDeathState(EnemySlime* enemy) :owner(enemy) {};
	// �f�X�g���N�^
	~EnemyDeathState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
protected:
	EnemySlime* owner;
};