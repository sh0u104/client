#include "EnemyStateDerived.h"
#include "Mathf.h"
#include "SceneManager.h"
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
void EnemyWanderState::Enter()
{
	// �����A�j���[�V�����Đ�
	owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_WalkFWD), true);
	owner->SetState(Enemy::State::Wander);

	// �ڕW�n�_�ݒ�
	owner->SetRandomTargetPosition();

}

void EnemyWanderState::Execute(float elapsedTime)
{
    // �ڕW�n�_�܂�XZ���ʂł̋�������
    DirectX::XMFLOAT3 targetPosition = owner->GettargetPosition();
    DirectX::XMFLOAT3 position = owner->GetPosition();

    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float distSq = vx * vx + vz * vz;

    float radius = owner->GetRadius();
    if (distSq < radius * radius)
    {
        // ���̖ڕW�n�_�ݒ�
        //SetRandomTargetPosition();
        // �ҋ@�X�e�[�g��
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Idle));
      
    }

    // �ڕW�n�_�ֈړ�
    owner->MoveToTarget(elapsedTime, 0.5f);

    // �v���C���[���G
    if (owner->SearchPlayer())
    {
        // ����������
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Pursuit));
    }
}

void EnemyWanderState::Exit()
{
}



void EnemyIdleState::Enter()
{
    // �ҋ@�A�j���[�V�����Đ�
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_IdleNormal), true);

    owner->SetState(Enemy::State::Idle);
    // �^�C�}�[�������_����ݒ�@�ҋ@����
    owner->SetStateTimer(Mathf::RandomRange(3.0f, 5.0f));

}

void EnemyIdleState::Execute(float elapsedTime)
{
    // �^�C�}�[���� �ҋ@����
    owner->SetStateTimer(owner->GetStateTimer() - elapsedTime);
    if (owner->GetStateTimer() < 0.0f)
    {
        // �p�j�X�e�[�g�֑J��
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Wander));
    }
}

void EnemyIdleState::Exit()
{
}



void EnemyPursuitState::Enter()
{
    // �����A�j���[�V�����Đ�
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_RunFWD), true);

    owner->SetState(Enemy::State::Pursuit);

    // ���b�Ԓǔ�����^�C�}�[�������_���ݒ�
    owner->SetStateTimer(Mathf::RandomRange(3.0f, 5.0f));

}

void EnemyPursuitState::Execute(float elapsedTime)
{
    if (SceneManager::Instance().GetPlayerManager()->GetPlayer(owner->GetNearPlayerId()) == nullptr)
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Wander));
        return;
    }


    // �ڕW�n�_�����v���C���[�ʒu�ɐݒ�
    owner->SettargetPosition(SceneManager::Instance().GetPlayerManager()->GetPlayer(owner->GetNearPlayerId())->GetPosition());

    // �ڕW�n�_�ֈړ�
    owner->MoveToTarget(elapsedTime, 1.0f);

     // �^�C�}�[���� �ҋ@����
    owner->SetStateTimer(owner->GetStateTimer() - elapsedTime);
    if (owner->GetStateTimer() < 0.0f)
    {
        // �ҋ@�X�e�[�g�֑J��
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Idle));
    }

    // �v���C���[�̋߂Â��ƍU���X�e�[�g�֑J��
    DirectX::XMFLOAT3 targetPosition = owner->GettargetPosition();
    DirectX::XMFLOAT3 position = owner->GetPosition();
    float vx = targetPosition.x - position.x;
    float vy = targetPosition.y - position.y;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist < owner->GetAttackRange())
    {
        // �U���X�e�[�g�֑J��
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Attack));
    }


}

void EnemyPursuitState::Exit()
{
}



void EnemyAttackState::Enter()
{
    // �U���A�j���[�V�����Đ�
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_Attack1), false);

    owner->SetState(Enemy::State::Attack);
}

void EnemyAttackState::Execute(float elapsedTime)
{
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::IdleBattle));
    }
    // �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
    float animationTime = owner->GetModel()->GetCurrentANimationSeconds();
    if (animationTime >= 0.2f && animationTime <= 0.35f)
    {
        // �ڋʃm�[�h�ƃv���C���[�̏Փˏ���
        owner->CollisitionNodeVsPlayer("EyeBall", 0.2f);
    }
}

void EnemyAttackState::Exit()
{
}



void EnemyIdleBattleState::Enter()
{
    // �U���A�j���[�V�����Đ�
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_IdleBattle), true);

    owner->SetState(Enemy::State::IdleBattle);
}

void EnemyIdleBattleState::Execute(float elapsedTime)
{
    if (SceneManager::Instance().GetPlayerManager()->GetPlayer(owner->GetNearPlayerId()) == nullptr)
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Wander));
        return;
    }

    // �ڕW�n�_���v���C���[�ʒu�ɐݒ�
    owner->SettargetPosition(SceneManager::Instance().GetPlayerManager()->GetPlayer(owner->GetNearPlayerId())->GetPosition());

    // �^�C�}�[����
    owner->SetStateTimer(owner->GetStateTimer() - elapsedTime);
    if (owner->GetStateTimer() < 0.0f)
    {
        // �v���C���[���U���͈͂ɂ����ꍇ�͍U���X�e�[�g�֑J��
        DirectX::XMFLOAT3 targetPosition = owner->GettargetPosition();
        DirectX::XMFLOAT3 position = owner->GetPosition();
        float vx = targetPosition.x - position.x;
        float vy = targetPosition.y - position.y;
        float vz = targetPosition.z - position.z;
        float dist = sqrtf(vx * vx + vy * vy + vz * vz);
        if (dist < owner->GetAttackRange())
        {
            // �U���X�e�[�g�֑J��
            owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Attack));
        }
        else
        {
            // �p�j�X�e�[�g�֑J��
            owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Wander));
        }
    }
    owner->MoveToTarget(elapsedTime, 0.0f);
}

void EnemyIdleBattleState::Exit()
{
}



void EnemyDamageState::Enter()
{
    // �U���A�j���[�V�����Đ�
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_GetHit), false);

    owner->SetState(Enemy::State::Damage);

}

void EnemyDamageState::Execute(float elapsedTime)
{
    // �_���[�W�A�j���[�V�������I�������퓬�ҋ@�X�e�[�g�֑J��
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::IdleBattle));
    }
}

void EnemyDamageState::Exit()
{
}



void EnemyDeathState::Enter()
{
    // �_���[�W�A�j���[�V�����Đ�
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_Die), false);

    owner->SetState(Enemy::State::Death);
}

void EnemyDeathState::Execute(float elapsedTime)
{
    // �_���[�W�A�j���[�V�������I������玩����j��
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->Destroy();
    }
}

void EnemyDeathState::Exit()
{
}