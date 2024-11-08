#include <imgui.h>
#include "EnemySlime.h"
#include "EnemyManager.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"
#include "SceneManager.h"
// �R���X�g���N�^
EnemySlime::EnemySlime()
{
    model = new Model("Data/Model/Slime/Slime.mdl");

    // ���f�������������̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.01f;

    // ���A�����ݒ�(�~��)
    radius = 0.5f;
    height = 1.0f;

    // �p�j�X�e�[�g�֑J��
    TransitionWanderState();
}

// �f�X�g���N�^
EnemySlime::~EnemySlime()
{
    delete model;
}

// �X�V����
void EnemySlime::Update(float elapsedTime)
{
    if (SceneManager::Instance().GetPlayerManager()->GetMyPlayer()->GetTeamHost())
    {
        // �X�e�[�g���̏���
        switch (state)
        {
        case State::Wander:
            UpdateWanderState(elapsedTime);
            break;
        case State::Idle:
            UpdateIdleState(elapsedTime);
            break;
        case State::Pursuit:
            UpdatePursuitState(elapsedTime);
            break;
        case State::Attack:
            UpdateAttackState(elapsedTime);
            break;
        case State::IdleBattle:
            UpdateIdleBattleState(elapsedTime);
            break;
        case State::Damage:
            UpdateDamageState(elapsedTime);
            break;
        case State::Death:
            UpdateDeathState(elapsedTime);
            break;
        }


        // ���͏����X�V
        UpdateVelocity(elapsedTime);
        // ���G���ԍX�V
        UpdateInbincibleTimer(elapsedTime);
    }
        // �I�u�W�F�N�g�s����X�V
        UpdateTransform();

    // ���f���A�j���[�V�����X�V
    model->UpdateAnimation(elapsedTime);

    // ���f���s��X�V
    model->UpdateTransform(transform);
}

// �`�揈��
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemySlime::DrawDebugPrimitive()
{
    // ���N���X�̃f�o�b�O�v���~�e�B�u�`��
    Enemy::DrawDebugPrimitive();

    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // �꒣��͈͂��f�o�b�O�~���`��
    debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
        DirectX::XMFLOAT4(0, 1, 0, 1));

    // �^�[�Q�b�g�ʒu���f�o�b�O���`��
    debugRenderer->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));
    
    // ���G�͈͂��f�o�b�O�~���`��
    debugRenderer->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

    // �U���͈͂��f�o�b�O�~���`��
    debugRenderer->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

// �꒣��ݒ�
void EnemySlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
    territoryOrigin = origin;
    territoryRange = range;

}

void EnemySlime::SetRandomTargetPosition()
{
    //float xposition = Mathf::RandomRange(0, territoryOrigin.x);
    //float zposition = Mathf::RandomRange(0, territoryOrigin.z);
    //targetPosition.x = xposition;
    //targetPosition.y = 0;
    //targetPosition.z = zposition;

    float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
    float range = Mathf::RandomRange(0.0f, territoryRange);
    targetPosition.x = territoryOrigin.x + sinf(theta)*range;
    targetPosition.y = territoryOrigin.y;
    targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

void EnemySlime::MoveToTarget(float elapsedTime, float speedRate)
{
    // �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vz * vz);
    vx /= dist;
    vz /= dist;

    // �ړ�����
    Move(vx, vz, moveSpeed * speedRate);
    Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// �p�j�X�e�[�g�֑J��
void EnemySlime::TransitionWanderState()
{
    state = State::Wander;

    // �ڕW�n�_�ݒ�
    SetRandomTargetPosition();

    // �����A�j���[�V�����Đ�
    model->PlayAnimation(Anim_WalkFWD, true);
}

// �p�j�X�e�[�g�X�V����
void EnemySlime::UpdateWanderState(float elapsedTime)
{
    // �ڕW�n�_�܂�XZ���ʂł̋�������
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z- position.z;
    float distSq = vx * vx + vz * vz;
    if (distSq < radius * radius)
    {
        // ���̖ڕW�n�_�ݒ�
        //SetRandomTargetPosition();

        //// �ҋ@�X�e�[�g��
        TransitionIdleState();
    }

    // �ڕW�n�_�ֈړ�
    MoveToTarget(elapsedTime, 0.5f);

    // �v���C���[���G
    if (SearchPlayer())
    {
        // ����������
        TransitionPursuitState();
    }
}

void EnemySlime::CollisitionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
    // �m�[�h�ʒu�Ɠ����蔻����s��
    Model::Node* node = model->FindNode(nodeName);
    if (node != nullptr)
    {
        // �m�[�h�̃��[���h���W
        DirectX::XMFLOAT3 nodePosition(
        node->worldTransform._41,
        node->worldTransform._42,
        node->worldTransform._43
            );

        // �����蔻��\��
        Graphics::Instance().GetDebugRenderer()->DrawSphere(
            nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
        );

        // �v���C���[�Ɠ����蔻��
        Player& player = Player::Instance();
        DirectX::XMFLOAT3 outPosition;
        if (Collision::IntersectSphereVsCylinder(
            nodePosition,
            nodeRadius,
            player.GetPosition(),
            player.GetRadius(),
            player.GetHeight(),
            outPosition))
        {
            // �_���[�W��^����
            if (player.ApplyDamage(1, 0.5f))
            {
                // �G�𐁂���΂��x�N�g�����Z�o
                DirectX::XMFLOAT3 vec;
                vec.x = outPosition.x - nodePosition.x;
                vec.z = outPosition.z - nodePosition.z;
                float length = sqrtf(vec.x * vec.x + vec.z * vec.z);
                vec.x /= length;
                vec.z /= length;

                // XZ���ʂɐ�����΂��͂�������
                float power = 10.0f;
                vec.x *= power;
                vec.z *= power;
                // Y�����ɂ��͂�������
                vec.y = 5.0f;

                // ������΂�
                //player.AddImpulse(vec);
            }
        }
    }
}

bool EnemySlime::SearchPlayer()
{
    // �v���C���[�ƍ��፷���l�����ĂRD�ł̋������������
    float dist = INT_MAX;
    float keepvx = 0;
    float keepvz = 0;
    std::vector<Player*> players = SceneManager::Instance().GetPlayerManager()->GetPlayers();
    for (Player* player : players)
    {
        const DirectX::XMFLOAT3& playerPosition = player->GetPosition();
        float vx = playerPosition.x - position.x;
        float vy = playerPosition.y - position.y;
        float vz = playerPosition.z - position.z;
        float sqr = sqrtf(vx * vx + vy * vy + vz * vz);
        // ���[�g
        if (dist > sqr)
        {
            dist = sqr;
            nearPlayerId = player->GetPlayerID();
            keepvx = vx;
            keepvz = vz;
        }
    }

    if (dist < searchRange)
    {
        float distXZ = sqrtf(keepvx * keepvx + keepvz * keepvz);
        // �P�ʃx�N�g����
        keepvx /= distXZ;
        keepvz /= distXZ;
        // �O���x�N�g��
        float frontX = sinf(angle.y);
        float frontZ = cosf(angle.y);
        // �Q�̃x�N�g���̓��ϒl�őO�㔻��
        float dot = (frontX * keepvx) + (frontZ * keepvz);
        //0.070�G90�x
        //0.0f�G�����ȏ�
        if (dot > 0.0f)
        {
            return true;
        }
    }
    return false;
}

void EnemySlime::TransitionIdleState()
{
    state = State::Idle;

    // �^�C�}�[�������_����ݒ�@�ҋ@����
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_IdleNormal, true);
}

void EnemySlime::UpdateIdleState(float elapsedTime)
{

    // �^�C�}�[���� �ҋ@����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // �p�j�X�e�[�g�֑J��
        TransitionWanderState();
    }
}

void EnemySlime::TransitionPursuitState()
{
    state = State::Pursuit;

    // ���b�Ԓǔ�����^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    // �����A�j���[�V�����Đ�
    model->PlayAnimation(Anim_RunFWD, true);
}

// �ǔ��X�e�[�g�X�V����
void EnemySlime::UpdatePursuitState(float elapsedTime)
{
    // �ڕW�n�_�����v���C���[�ʒu�ɐݒ�
    
    targetPosition = SceneManager::Instance().GetPlayerManager()->GetPlayer(nearPlayerId)->GetPosition();

    // �ڕW�n�_�ֈړ�
    MoveToTarget(elapsedTime, 1.0f);

    // �^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // �ҋ@�X�e�[�g�֑J��
        TransitionIdleState();
    }

    // �v���C���[�̋߂Â��ƍU���X�e�[�g�֑J��
    float vx = targetPosition.x - position.x;
    float vy = targetPosition.y - position.y;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vy * vy + vz * vz);
    if (dist < attackRange)
    {
        // �U���X�e�[�g�֑J��
        TransitionAttackState();
    }



}

void EnemySlime::TransitionAttackState()
{
    state = State::Attack;

    // �U���A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Attack1, false);
}

void EnemySlime::UpdateAttackState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
    // �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
    float animationTime = model->GetCurrentANimationSeconds();
    if (animationTime >= 0.2f && animationTime <= 0.35f)
    {
        // �ڋʃm�[�h�ƃv���C���[�̏Փˏ���
        CollisitionNodeVsPlayer("EyeBall", 0.2f);
    }
}

void EnemySlime::TransitionIdleBattleState()
{
    state = State::IdleBattle;

    // �U���A�j���[�V�����Đ�
    model->PlayAnimation(Anim_IdleBattle, true);
}

void EnemySlime::UpdateIdleBattleState(float elapsedTime)
{
    // �ڕW�n�_���v���C���[�ʒu�ɐݒ�
    targetPosition = SceneManager::Instance().GetPlayerManager()->GetPlayer(nearPlayerId)->GetPosition();

    // �^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // �v���C���[���U���͈͂ɂ����ꍇ�͍U���X�e�[�g�֑J��
        float vx = targetPosition.x - position.x;
        float vy = targetPosition.y - position.y;
        float vz = targetPosition.z - position.z;
        float dist = sqrtf(vx * vx + vy * vy + vz * vz);
        if (dist < attackRange)
        {
            // �U���X�e�[�g�֑J��
            TransitionAttackState();
        }
        else
        {
            // �p�j�X�e�[�g�֑J��
            TransitionWanderState();
        }
    }
    MoveToTarget(elapsedTime, 0.0f);
}
// �_���[�W�X�e�[�g�֑J��
void EnemySlime::TransitionDamageState()
{
    state = State::Damage;

    // �_���[�W�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_GetHit, false);
}

void EnemySlime::UpdateDamageState(float elapsedTime)
{
    // �_���[�W�A�j���[�V�������I�������퓬�ҋ@�X�e�[�g�֑J��
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

// ���S�X�e�[�g�֑J��
void EnemySlime::TransitionDeathState()
{
    state = State::Death;

    // �_���[�W�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Die, false);
}

void EnemySlime::UpdateDeathState(float elapsedTime)
{
    // �_���[�W�A�j���[�V�������I������玩����j��
    if (!model->IsPlayAnimation())
    {
        Destroy();
    }
}

// ���S�������ɌĂ΂��
void EnemySlime::OnDead()
{
    // ���S�X�e�[�g�֑J��
    TransitionDeathState();
}

void EnemySlime::OnDamaged()
{
    // �_���[�W�X�e�[�g�֑J��
    TransitionDamageState();
}

