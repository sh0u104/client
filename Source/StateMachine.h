#pragma once
#include <vector>
#include "StateBase.h"

class StateMachine
{
public:
	// �R���X�g���N�^
	StateMachine() {}
	// �f�X�g���N�^
	~StateMachine();
	// �X�V����
	void Update(float elapsedTime);
	// �X�e�[�g�Z�b�g
	void SetState(int setstate);
	// �X�e�[�g�ύX
	void ChangeState(int newState);
	// �X�e�[�g�o�^
	void RegisterState(State* state);
	// ���݂̃X�e�[�g�ԍ��擾
	int GetStateIndex();
private:
	// ���݂̃X�e�[�g
	State* currentState = nullptr;
	// �e�X�e�[�g��ێ�����z��
	std::vector<State*> statePool;
};