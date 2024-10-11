#pragma once
#include "Player.h"
#include "StateBase.h"
class IdleState : public State
{
public:
	// �R���X�g���N�^
	IdleState(Player* player) :State(player) {};
	// �f�X�g���N�^
	~IdleState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

class MoveState : public State
{
public:
	// �R���X�g���N�^
	MoveState(Player* player) :State(player) {};
	// �f�X�g���N�^
	~MoveState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

class JumpState : public State
{
public:
	// �R���X�g���N�^
	JumpState(Player* player) :State(player) {};
	// �f�X�g���N�^
	~JumpState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

class LandState : public State
{
public:
	// �R���X�g���N�^
	LandState(Player* player) :State(player) {};
	// �f�X�g���N�^
	~LandState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

class JumpFlipState : public State
{
public:
	// �R���X�g���N�^
	JumpFlipState(Player* player) :State(player) {};
	// �f�X�g���N�^
	~JumpFlipState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};