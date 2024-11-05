#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileManager.h"

#include "StateMachine.h"
// �A�j���[�V����
enum class Animation
{
    Anim_Attack,
    Anim_Death,
    Anim_Falling,
    Anim_GetHit1,
    Anim_GetHit2,
    Anim_Idle,
    Anim_Jump,
    Anim_Jump_Flip,
    Anim_Landing,
    Anim_Revive,
    Anim_Running,
    Anim_Walking
};
// �v���C���[
class Player : public Character
{
public:
    Player();
    ~Player() override;

    // �C���X�^���X�擾
    static Player& Instance();

    enum  Animation
    {
        Anim_Attack,
        Anim_Death,
        Anim_Falling,
        Anim_GetHit1,
        Anim_GetHit2,
        Anim_Idle,
        Anim_Jump,
        Anim_Jump_Flip,
        Anim_Landing,
        Anim_Revive,
        Anim_Running,
        Anim_Walking
    };
     


    // �X�V����
    void Update(float elapsedTime);

    // �f�o�b�O�v���~�e�B�u�`��@�f�o�b�O�p
    void DrawDebugPrimitive();


    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);


    // �e�ۂƓG�̏Փˏ���
    void CollisionProjectilesVsEnemies();

    //// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
    //void CollisionPlayerVsEnemies();

    // �f�o�b�O�pGUI�`��
    void DrawDebugGUI();
protected:
    // ���n�������ɌĂ΂��
    void OnLanding() override;

    // ���S�������ɌĂ΂��
    void OnDead() override;

    // �_���[�W���󂯂����ɌĂ΂��
    void OnDamaged() override;
public:
    enum class State
    {
        Idle,
        Move,
        Land,
        Jump,
        JumpFlip,
    };
    // �X�e�[�g
    //enum class State
    //{
    //    Idle,
    //    Move,
    //    Jump,
    //    Land,
    //    JumpFlip,
    //    Attack,
    //    Damage,
    //    Death,
    //    Revive,
    //    None,
    //};
    StateMachine* GetStateMachine() { return stateMachine.get(); }
    Model* GetModel() { return model; }
public:
    // �X�e�B�b�N���͒l����ړ��x�N�g�����擾 �i�s�x�N�g�������i�ނׂ�����
    DirectX::XMFLOAT3 KeyGetMoveVec() const;
    DirectX::XMFLOAT3 MouseGetMoveVec() const;

    // �ړ����͏���
    bool InputMove(float elapsedTime);

    // �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
    void CollisionPlayerVsEnemies();

    // �m�[�h�ƃG�l�~�[�̏Փˏ���
    void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

    // �W�����v���͏���
    bool InputJump();

    // �e�ۓ���
    bool InputProjectile();

    // �U������
    bool InputAttack();

    // �ҋ@�X�e�[�g�֑J��
    void TransitionIdleState();
    // �ҋ@�X�e�[�g�X�V
    void UpdateIdleState(float elapsedTime);

    // �ړ��X�e�[�g�֍X�V
    void UpdateMoveState(float elapsedTime);

    // �W�����v�X�e�[�g�X�V����
    void UpdateJumpState(float elapsedTime);

    // ���n�X�e�[�g�֑J��
    void TransitionLandState();

    // ���n�X�e�[�W�X�V����
    void UpdateLandState(float elapsedTime);

    // ��i�W�����v�X�e�[�g�X�V
    void UpdatejumpFlipState(float elapsedTime);
    

    // �U���X�e�[�g�֑J��
    void TransitionAttackState();

    // �U���X�e�[�W�X�V����
    void UpdateAttackState(float elapsedTime);

    // �_���[�W�X�e�[�g�֑J��
    void TransitionDamageState();

    // �_���[�W�X�e�[�g�֍X�V����
    void UpdateDamageState(float elapsedTime);

    // ���S�X�e�[�g�֑J��
    void TransitionDeathState();

    // ���S�X�e�[�g�X�V����
    void UpdateDeathState(float elapsedTime);

    // �����X�e�[�g�֑J��
    void TransitionReviveState();

    // �����X�e�[�g�X�V����
    void UpdateReviveState(float elapsedTime);

    // ��i�W�����v�X�e�[�g�֑J��
    void TransitionJumpFlipState();

    // �W�����v�X�e�[�g�֑J��
    void TransitionJumpState();

    // �ړ��X�e�[�g�֑J��    
    void TransitionMoveState();

    int            jumpCount = 0;

    void SetState(State state) { this->state = state; }
private:
    std::shared_ptr<StateMachine> stateMachine;
   // StateMachine* stateMachine = nullptr;
    Model* model = nullptr;

    float          moveSpeed = 5.0f;

    float         turnSpeed = DirectX::XMConvertToRadians(720);

    float          jumpSpeed = 20.0f;
   
    int            jumpLimit = 2;

    //ProjectileManager projectileManager;

    Effect* hitEffect = nullptr;
    Effect* desEffect = nullptr;

    State   state = State::Idle;
    State   stated;



    // ���n�ꏊ�܂ł̋����@ �\���ȑ��x�ŗ��Ƃ��d�͂̂T�{�Q�A�R�b��ɒ��n���[�V����������B
   // int jumpfliptime =  grabity * 5;

    float  leftHandRadius = 0.4f;
           
    bool   attackCollisionFlag = false;

    int    healse  = 10;

public:
    // ID
    int GetPlayerID() const { return playerID; }
    void SetPlayerID(int id) { playerID = id; }
    
    // �����t���O
    bool GetReady() { return ready; }
    void SetReady(bool flg) { ready = flg; }
    
    bool flagVelosity() { return velocity.x != 0 || velocity.y != 0 || velocity.z != 0; }
    DirectX::XMFLOAT3 GetVelocity() { return this->velocity; }
    void SetVelovity(DirectX::XMFLOAT3 velocity) { this->velocity = velocity; }
    
    bool Getoperation() { return operation;}
    void Setoperation(bool flag) { operation = flag;}
    
    State GetState() { return this->state; }
   
    void ChangeState(State state);

    State GetSendState() { return this->sendState; }
    State sendState = State::Idle;

 
    int Getteamnumber() { return teamNumber; }
    void Setteamnumber(int number) { teamNumber = number; }

    int Getteamsid(int number) { return teamsId[number]; }
    void Setteamsid(int number, int id) { this->teamsId[number] = id; }

    bool GetstartCheck() { return startCheck; }
    void SetstartCheck(bool check) { this->startCheck = check; }

    const char* GetName() { return Name; }
    void SetName(char name[10]){ strcpy_s(this->Name, name); }

   
    void SetisMouseOperation(bool ismouse) { this->isMouseOperation = ismouse; }
    bool GetisMouseOperation() { return this->isMouseOperation; }

    void SetLoginDay(int day) { this->loginDay = day; }
    int GetLoginDay() { return this->loginDay; }

    float mouseAngle = 0;
    float mouselength = 0;

protected:
  
    int teamNumber = 0;
    int teamsId[4] = {};
    bool operation = false;
    int playerID;
    bool ready = false;
    bool startCheck = false;

    char Name[10];
    bool isMouseOperation = true;
    int loginDay = 0;
};