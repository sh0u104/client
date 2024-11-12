#include "EnemyStateDerived.h"
#include "Mathf.h"
#include "SceneManager.h"
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
void EnemyWanderState::Enter()
{
	// 歩きアニメーション再生
	owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_WalkFWD), true);
	owner->SetState(Enemy::State::Wander);

	// 目標地点設定
	owner->SetRandomTargetPosition();

}

void EnemyWanderState::Execute(float elapsedTime)
{
    // 目標地点までXZ平面での距離判定
    DirectX::XMFLOAT3 targetPosition = owner->GettargetPosition();
    DirectX::XMFLOAT3 position = owner->GetPosition();

    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float distSq = vx * vx + vz * vz;

    float radius = owner->GetRadius();
    if (distSq < radius * radius)
    {
        // 次の目標地点設定
        //SetRandomTargetPosition();
        // 待機ステートへ
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Idle));
      
    }

    // 目標地点へ移動
    owner->MoveToTarget(elapsedTime, 0.5f);

    // プレイヤー索敵
    if (owner->SearchPlayer())
    {
        // 見つかったら
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Pursuit));
    }
}

void EnemyWanderState::Exit()
{
}



void EnemyIdleState::Enter()
{
    // 待機アニメーション再生
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_IdleNormal), true);

    owner->SetState(Enemy::State::Idle);
    // タイマーをランダムを設定　待機時間
    owner->SetStateTimer(Mathf::RandomRange(3.0f, 5.0f));

}

void EnemyIdleState::Execute(float elapsedTime)
{
    // タイマー処理 待機時間
    owner->SetStateTimer(owner->GetStateTimer() - elapsedTime);
    if (owner->GetStateTimer() < 0.0f)
    {
        // 徘徊ステートへ遷移
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Wander));
    }
}

void EnemyIdleState::Exit()
{
}



void EnemyPursuitState::Enter()
{
    // 歩きアニメーション再生
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_RunFWD), true);

    owner->SetState(Enemy::State::Pursuit);

    // 数秒間追尾するタイマーをランダム設定
    owner->SetStateTimer(Mathf::RandomRange(3.0f, 5.0f));

}

void EnemyPursuitState::Execute(float elapsedTime)
{
    if (SceneManager::Instance().GetPlayerManager()->GetPlayer(owner->GetNearPlayerId()) == nullptr)
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Wander));
        return;
    }


    // 目標地点ををプレイヤー位置に設定
    owner->SettargetPosition(SceneManager::Instance().GetPlayerManager()->GetPlayer(owner->GetNearPlayerId())->GetPosition());

    // 目標地点へ移動
    owner->MoveToTarget(elapsedTime, 1.0f);

     // タイマー処理 待機時間
    owner->SetStateTimer(owner->GetStateTimer() - elapsedTime);
    if (owner->GetStateTimer() < 0.0f)
    {
        // 待機ステートへ遷移
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Idle));
    }

    // プレイヤーの近づくと攻撃ステートへ遷移
    DirectX::XMFLOAT3 targetPosition = owner->GettargetPosition();
    DirectX::XMFLOAT3 position = owner->GetPosition();
    float vx = targetPosition.x - position.x;
    float vy = targetPosition.y - position.y;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist < owner->GetAttackRange())
    {
        // 攻撃ステートへ遷移
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Attack));
    }


}

void EnemyPursuitState::Exit()
{
}



void EnemyAttackState::Enter()
{
    // 攻撃アニメーション再生
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_Attack1), false);

    owner->SetState(Enemy::State::Attack);
}

void EnemyAttackState::Execute(float elapsedTime)
{
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::IdleBattle));
    }
    // 任意のアニメーション再生区間でのみ衝突判定処理をする
    float animationTime = owner->GetModel()->GetCurrentANimationSeconds();
    if (animationTime >= 0.2f && animationTime <= 0.35f)
    {
        // 目玉ノードとプレイヤーの衝突処理
        owner->CollisitionNodeVsPlayer("EyeBall", 0.2f);
    }
}

void EnemyAttackState::Exit()
{
}



void EnemyIdleBattleState::Enter()
{
    // 攻撃アニメーション再生
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

    // 目標地点をプレイヤー位置に設定
    owner->SettargetPosition(SceneManager::Instance().GetPlayerManager()->GetPlayer(owner->GetNearPlayerId())->GetPosition());

    // タイマー処理
    owner->SetStateTimer(owner->GetStateTimer() - elapsedTime);
    if (owner->GetStateTimer() < 0.0f)
    {
        // プレイヤーが攻撃範囲にいた場合は攻撃ステートへ遷移
        DirectX::XMFLOAT3 targetPosition = owner->GettargetPosition();
        DirectX::XMFLOAT3 position = owner->GetPosition();
        float vx = targetPosition.x - position.x;
        float vy = targetPosition.y - position.y;
        float vz = targetPosition.z - position.z;
        float dist = sqrtf(vx * vx + vy * vy + vz * vz);
        if (dist < owner->GetAttackRange())
        {
            // 攻撃ステートへ遷移
            owner->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Attack));
        }
        else
        {
            // 徘徊ステートへ遷移
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
    // 攻撃アニメーション再生
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_GetHit), false);

    owner->SetState(Enemy::State::Damage);

}

void EnemyDamageState::Execute(float elapsedTime)
{
    // ダメージアニメーションが終わったら戦闘待機ステートへ遷移
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
    // ダメージアニメーション再生
    owner->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Anim_Die), false);

    owner->SetState(Enemy::State::Death);
}

void EnemyDeathState::Execute(float elapsedTime)
{
    // ダメージアニメーションが終わったら自分を破棄
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->Destroy();
    }
}

void EnemyDeathState::Exit()
{
}