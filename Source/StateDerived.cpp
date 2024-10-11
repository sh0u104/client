#include "StateDerived.h"

void IdleState::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Animation::Anim_Idle), true);
    owner->SetState(Player::State::Idle);
}

void IdleState::Execute(float elapsedTime)
{
    // 移動入力処理
    if (owner->InputMove(elapsedTime))
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Move));
    }

    // ジャンプ入力処理
    if (owner->InputJump())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Jump));
    }
}

void IdleState::Exit()
{
}


void MoveState::Enter()
{
  // 走りアニメーション再生
  owner->GetModel()->PlayAnimation(static_cast<int>(Animation::Anim_Running), true);
  owner->SetState(Player::State::Move);
}

void MoveState::Execute(float elapsedTime)
{
    // 移動入力処理
    if (!owner->InputMove(elapsedTime))
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
    }
    // ジャンプ入力処理
    if (owner->InputJump())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Jump));
    }

}

void MoveState::Exit()
{
}

void LandState::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Animation::Anim_Landing), true);
    owner->SetState(Player::State::Land);
}

void LandState::Execute(float elapsedTime)
{
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
    }
}

void LandState::Exit()
{
}

void JumpState::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Animation::Anim_Jump), true);
    owner->SetState(Player::State::Jump);
}

void JumpState::Execute(float elapsedTime)
{
    owner->InputMove(elapsedTime);

    // ジャンプ入力処理
    if (owner->InputJump() && owner->jumpCount == 2)
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::JumpFlip));
    }

     // ジャンプアニメーション終了語
    if (!owner->GetModel()->IsPlayAnimation())
    {
        //落下アニメーション再生
        owner->GetModel()->PlayAnimation(static_cast<int>(Animation::Anim_Falling), true);
    }

    if (owner->IsGround())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
    }
     
    
}

void JumpState::Exit()
{
}


void JumpFlipState::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Animation::Anim_Jump_Flip),false);
    owner->SetState(Player::State::JumpFlip);
}

void JumpFlipState::Execute(float elapsedTime)
{
    owner->InputMove(elapsedTime);
    // ジャンプアニメーション終了語
    if (!owner->GetModel()->IsPlayAnimation())
    {
        //落下アニメーション再生
        owner->GetModel()->PlayAnimation(static_cast<int>(Animation::Anim_Falling), true);
    }
    if (owner->IsGround())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
    }
}

void JumpFlipState::Exit()
{
}



