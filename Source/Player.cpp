#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "Graphics/Model.h"

#include "PlayerManager.h"
#include <DirectXMath.h>
static Player* instance = nullptr;

// �C���X�^���X�擾
Player& Player::Instance()
{
    return *instance;
}



// �R���X�g���N�^
Player::Player()
{
    // �C���X�^���X�|�C���^�ݒ�
    instance = this;

    //model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
    model = new Model("Data/Model/Jammo/Jammo.mdl");
    /*model->PlayAnimation(0);*/

    // ���f�������������̂ŃX�P�[�����O
    // �L�����N�^�[��1.1�{
    scale.x = scale.y = scale.z = 0.01f;

    // �q�b�g�G�t�F�N�g�Ǎ� 
    //hitEffect = new Effect("Data/Effect/Shot_Hit.efk");
    hitEffect = new Effect("Data/Effect/sunder.efk");
    desEffect = new Effect("Data/Effect/F.efk");


    // �ҋ@�X�e�[�g�֑J��
    TransitionIdleState();
}

// �f�X�g���N�^
Player::~Player()
{
    delete hitEffect;
    delete desEffect;
    delete model;

}




// �X�V����
// elapsedTime(�o�ߎ���)
void Player::Update(float elapsedTime)
{
    // �X�e�[�g���̏���
    if (Getoperation())
    {
        switch (state)
        {
        case State::Idle:
            UpdateIdleState(elapsedTime);
            break;
        case State::Move:
            UpdateMoveState(elapsedTime);
            break;
        case State::Jump:
            UpdateJumpState(elapsedTime);
            break;
        case State::Land:
            UpdateLandState(elapsedTime);
            break;
        case State::JumpFlip:
            UpdatejumpFlipState(elapsedTime);
            break;
        case State::Attack:
            UpdateAttackState(elapsedTime);
            break;
        case State::Damage:
            UpdateDamageState(elapsedTime);
            break;
        case State::Death:
            UpdateDeathState(elapsedTime);
            break;
        case State::Revive:
            UpdateReviveState(elapsedTime);
            break;
        }
    }


    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    UpdateInbincibleTimer(elapsedTime);

    //hitEffect->SetPosition(hitEffect->GetEfeHandle(), this->position);
    //hitEffect->SetScale(hitEffect->GetEfeHandle(),{ 0.1f,0.1f,0.1f });
    hitEffect->SetScale(hitEffect->GetEfeHandle(), { 1,1,1 });

    {
        //// ���͏����擾 // INPUT����Q�[���p�b�g�Ⴆ��
        //GamePad& gamePad = Input::Instance().GetGamePad();
        //float ax = gamePad.GetAxisLX();// ���X�e�B�b�N
        //float ay = gamePad.GetAxisLY();

        //// �ړ�����
        //float moveSpeed = 5.0f * elapsedTime;// �ړ����x
        //{
        //    // ���X�e�B�b�N�̓��͏������Ƃ�XZ���ʂւ̈ړ�����
        //    position.x += ax * moveSpeed;//5�{����
        //    position.z += ay * moveSpeed;
        //}

        //// ��]����
        //float rotateSpeed = DirectX::XMConvertToRadians(360) * elapsedTime;// �P�b�Ԃ�360�x����
        //if (gamePad.GetButton() & GamePad::BTN_A)
        //{
        //    // X����]����
        //    angle.x += rotateSpeed;
        //}
        //if (gamePad.GetButton() & GamePad::BTN_B)
        //{
        //    // Y����]����
        //    angle.y += rotateSpeed;

        //}
        //if (gamePad.GetButton() & GamePad::BTN_X)
        //{
        //    // Z����]����
        //    angle.z += rotateSpeed;

        //}
    }
    // �v���C���[�ƓG�Ƃ̏Փˏ���
    CollisionPlayerVsEnemies();

    CollisionProjectilesVsEnemies();

    // �e�ۍX�V����
    //projectileManager.Update(elapsedTime);

    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);

    // ���f���s��X�V
    // �����ɏo���Ăق��������� 
    model->UpdateTransform(transform);


    //position = position;

}



// �f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
    // �������@�̓��ق����Ȃ玩���Œǉ�
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //// �Փ˔���p�̃f�o�b�O����`��
    //debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

    // �Փ˔���p�̃f�o�b�O�~����`��
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

    // �e�ۃf�o�b�O�v���~�e�B�u�`��
    //projectileManager.DrawDebugPrimitive();

    if (attackCollisionFlag)
    {


        // �U���Փ˗p�̍���m�[�h�f�o�b�O����`��
        Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
        debugRenderer->DrawSphere(DirectX::XMFLOAT3(
            leftHandBone->worldTransform._41,
            leftHandBone->worldTransform._42,
            leftHandBone->worldTransform._43),
            leftHandRadius,
            DirectX::XMFLOAT4(1, 0, 0, 1)
        );
    }
}












// �ړ����͏���
bool Player::InputMove(float elapsedTime)
{
   
    // �i�s�x�N�g���擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();
    // �ړ�����
   /* Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);*/
    Move(moveVec.x, moveVec.z, moveSpeed);
    // ���񏈗�
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
    // �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
    //return moveVec.x ? true:false;
    
    //if (moveVec.x || moveVec.z)
    //    return true;
    if (!Getoperation())
    {
        return false;
    }
    //return false;
    // �����Ă���true�ɊԈ���Ă���false�ɂȂ�B
    return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}


DirectX::XMFLOAT3 Player::GetMoveVec() const
{
   
    // ���͏����擾
    GamePad& gamePad = Input::Instance().GetGamePad();
    //float ax = gamePad.GetAxisLX();
    float ax = 0;
    float ay = mouselength / 50;

    // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
   //Camera& camera = Camera::Instance();
   //const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
   //const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
    // �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
   // // �J�����E�����x�N�g���͂w�y���ʂɐ����ȃx�N�g���ɕϊ�
   // float cameraRightX = cameraRight.x;
   // float cameraRightZ = cameraRight.z;
   // // y��������炸�Ɂ@���̒������擾
   // float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
   // // ����Y�����������Ă邩�@�E�������΂߂ł��^�������i��łق���Y���O
   // //�@�ɂ��鏭���������ς�邾����P�ʃx�N�g���ɂ���P�D�O��
   // if (cameraRightLength > 0.0f)
   // {
   //     // �P�ʃx�N�g����
   //     // �E�����̒P�ʃx�N�g�� 
   //     cameraRightX = cameraRightX / cameraRightLength;
   //     cameraRightZ = cameraRightZ / cameraRightLength;
   //
   //     /*        if (cameraRightLength > DirectX::XM_PI)cameraRightLength -= DirectX::XM_PI * 2;
   //             if (cameraRightLength < -DirectX::XM_PI)cameraRightLength += DirectX::XM_PI * 2;
   //       */
   // }
 
    DirectX::XMVECTOR forward = DirectX::XMVectorSet(cos(mouseAngle), 0, sin(mouseAngle), 0);
    // �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    //float cameraFrontX = cameraFront.x;
    float cameraFrontX = DirectX::XMVectorGetX(forward);
    //float cameraFrontZ = cameraFront.z;
    float cameraFrontZ = DirectX::XMVectorGetZ(forward);
    cameraFrontX *= -1;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        // �P�ʃx�N�g����

        cameraFrontX = cameraFrontX / cameraFrontLength;
        cameraFrontZ = cameraFrontZ / cameraFrontLength;
    }

    // �i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec;// �ړ������i�ނׂ������i�s�x�N�g��
    // ax,ay�X�e�B�b�N�̋�@cameraRightX�i�J�����j
    // (cameraRightX* ax) + (cameraFrontX * ay)����͏�̐i������^�������ɂ���z
    // 
    vec.x = (cameraFrontX * ay);
    vec.z = (cameraFrontZ * ay);
    // Y�������ɂ͈ړ����Ȃ�
    vec.y = 0.0f;
    return vec;



}

// �`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    // �`��
    shader->Draw(dc, model);
    // �e�ە`�揈��
   // projectileManager.Render(dc, shader);
}





// �e�ۂƓG�̏Փˏ���

void Player::CollisionProjectilesVsEnemies()
{
    //EnemyManager& enemyManager = EnemyManager::Instance();
    //
    //// �S�Ă̓G�Ƒ�������ŏՓˏ���
    //int projectileCount = projectileManager.GetProjectileCount();
    //int enemyCount = enemyManager.GetEnemyCount();
    //for (int i = 0; i < projectileCount; ++i)
    //{
    //    Projectile* projectile = projectileManager.GetProjectile(i);
    //    for (int j = 0; j < enemyCount; ++j)
    //    {
    //        Enemy* enemy = enemyManager.GetEnemy(j);
    //
    //
    //        //// �Փˏ���
    //        DirectX::XMFLOAT3 outPositon;
    //
    //        //}
    //        // �~���Ɖ~
    //        if (Collision::IntersectSphereVsCylinder(
    //            projectile->GetPosition(),
    //            projectile->GetRadius(),
    //            enemy->GetPosition(),
    //            enemy->GetRadius(),
    //            enemy->GetHeight(),
    //            outPositon))
    //
    //        {
    //
    //            // �_���[�W��^����B
    //            //enemy->ApplyDamage(1);
    //        // �_���[�W���ʂ����������BTRUE�ɂȂ邩��
    //            if (enemy->ApplyDamage(1, 0.5f))
    //            {
    //                // ������΂�
    //                {
    //                    // �Փ�
    //                    DirectX::XMFLOAT3 impulse;
    //                    const float power = 10.0f;
    //                    const DirectX::XMFLOAT3& e = enemy->GetPosition();
    //                    const DirectX::XMFLOAT3& p = projectile->GetPosition();
    //                    float vx = e.x - p.x;
    //                    float vz = e.z - p.z;
    //                    float lengthXZ = sqrtf(vx * vx + vz * vz);
    //                    vx /= lengthXZ;
    //                    vz /= lengthXZ;
    //
    //                    impulse.x = vx * power;
    //                    impulse.y = power * 0.5f;
    //                    impulse.z = vz * power;
    //
    //                    enemy->AddImpulse(impulse);
    //                }
    //                // �q�b�g�G�t�F�N�g�Đ�
    //                {
    //                    DirectX::XMFLOAT3 e = enemy->GetPosition();
    //                    e.y += enemy->GetHeight() * 0.5f;
    //                    
    //                    
    //                    hitEffect->Play(e);
    //                    //desEffect->Play(e);
    //          
    //                }
    //                // �e�۔j��
    //                projectile->Destroy();
    //            }
    //        }
    //    }
    //
    //
    //}
}

// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓˏ���

    int enemyCount = enemyManager.GetEnemyCount();


        for (int i = 0; i < enemyCount; ++i)
        {
            Enemy* enemy = enemyManager.GetEnemy(i);


            //// �Փˏ���
            DirectX::XMFLOAT3 outPositon;
            //if (Collision::IntersectSpherVsSphere(
            //    position, radius,
            //    enemy->GetPosition(),
            //    enemy->GetRadius(),
            //    outPositon
            //))
            //{
            //// �����o����̈ʒu�ݒ�
            //enemy->SetPosition(outPositon);

            //}




            if (Collision::IntersectCylinderVsCylinder(
                position, radius, height,
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outPositon))

            {

                //{
                //    // �����o����̈ʒu�ݒ�
                //    enemy->SetPosition(outPositon);

                //}


                    DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
                    DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
                    DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
                    DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
                    DirectX::XMFLOAT3 normal;
                    DirectX::XMStoreFloat3(&normal, N);

                    if (normal.y > 0.8f)
                    {
                        if (enemy->ApplyDamage(1, 0.1f))
                        {
                            //���W�����v
                            Jump(jumpSpeed * 0.5f);
                        }

                    }
                    else
                    {
                        // �����o����̈ʒu�ݒ�@
                        enemy->SetPosition(outPositon);
                    }
                
            }
            // 
            //// �Փˏ����~��
            //if (Collision::IntersectCylinderVsCylinder((

            //    ))
            //{
            //    enemy->SetPosition(outPositon);
            //}


        
            

        }
    
    


}

// �m�[�h�ƓG�̏Փ˔���
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{

    // �m�[�h�擾
    Model::Node* node = model->FindNode(nodeName);
    //worldTransform
    //localTransform
    // �m�[�h�ʒu�擾
    DirectX::XMFLOAT3 nodePosition;
    nodePosition = {
        node->worldTransform._41,
        node->worldTransform._42,
        node->worldTransform._43
    };
    // �}�l�[�W���[�擾
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();
    // �w��̃m�[�h�ƑS�Ă̓G�𑍓�����ŏՓˏ���
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);


        //// �Փˏ���
        DirectX::XMFLOAT3 outPositon;

        // �~���Ɖ~
        if (Collision::IntersectSphereVsCylinder(
            nodePosition,
            nodeRadius,
            enemy->GetPosition(),
            enemy->GetRadius(),
            enemy->GetHeight(),
            outPositon))

        {

            // �_���[�W��^����B
            //enemy->ApplyDamage(1);
        // �_���[�W���ʂ����������BTRUE�ɂȂ邩��
            if (enemy->ApplyDamage(1, 0.5f))
            {

                // ������΂�
                {
                    // �Փ�
                    DirectX::XMFLOAT3 impulse;
                    const float power = 10.0f;
                    const DirectX::XMFLOAT3& e = enemy->GetPosition();
                    const DirectX::XMFLOAT3& p = nodePosition;
                    float vx = e.x - p.x;
                    float vz = e.z - p.z;
                    float lengthXZ = sqrtf(vx * vx + vz * vz);
                    vx /= lengthXZ;
                    vz /= lengthXZ;

                    impulse.x = vx * power;
                    impulse.y = power * 0.5f;
                    impulse.z = vz * power;

                    enemy->AddImpulse(impulse);
                }
                // �q�b�g�G�t�F�N�g�Đ�
                {
                    DirectX::XMFLOAT3 e = enemy->GetPosition();
                    e.y += enemy->GetHeight() * 0.5f;


                    hitEffect->Play(e);


                    //desEffect->Play(e);

                }

            }
        }



    }
 
}



// �f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{   
}

// ���n�������ɌĂ΂��
void Player::OnLanding()
{
    // ���n�������烊�Z�b�g
    jumpCount = 0;

    //// �������̑��͂����ȏ�Ȃ璅�n�X�e�[�g��  �\���ȑ��x�ŗ��Ƃ��d�͂̂T�{�Q�A�R�b��ɒ��n���[�V����������B
    //if (velocity.y < jumpfliptime)
    //{

    //    TransitionLandState();
    //}

    if (state != State::Damage && state != State::Death)
    {
        // ���n�X�e�[�g�֑J��
        TransitionLandState();
    }

}

void Player::OnDead()
{


    // ���S�X�e�[�g�J��
    TransitionDeathState();
}

void Player::OnDamaged()
{
    
        // �_���[�W�X�e�[�g�֑J��
    TransitionDamageState();
}

//DirectX::XMFLOAT3 Player::GetMoveVec() const
//{
//    // ���͒l���擾
//    GamePad& gamePad = Input::Instance().GetGamePad();
//    float ax = gamePad.GetAxisLX();// ���X�e�B�b�N
//    float ay = gamePad.GetAxisLY();
//
//    // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
//    Camera& camera = Camera::Instance();
//    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
//    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
//
//    // �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
//
//    // �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
//    float cameraRightX = cameraRight.x;
//    float cameraRightZ = cameraRight.z;
//    float cameraRightLenghth = cameraRightX+ cameraRightZ;
//    if (cameraRightLenghth > 0.0f)
//    {
//        //�������@���K��
//    }

//}
bool Player::InputJump()
{
    if (!Getoperation())
    {
        return false;
    }
    // �{�^���œ��͂ŃW�����v�i�W�����v�񐔐������j
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        // �l������������
        if (jumpCount < jumpLimit) {
            //�n�ʂɂ��܂�false
            isGround = false;
            // �W�����v�̍ő�l
            ++jumpCount;
            Jump(jumpSpeed);

            // �W�����v���͂���
           return true;
        }
        
    }

    return false;
}

bool Player::InputProjectile()
{
   // GamePad& gamePad = Input::Instance().GetGamePad();
   //
   //
   //
   //
   // // ���i�e�۔��ˁ@c�{�^������������
   // if (gamePad.GetButtonDown() & GamePad::BTN_X)
   // {
   //     // �O���� sin�̌v�Z �p�x�̌v�Z
   //     DirectX::XMFLOAT3 dir;
   //     dir.x = sinf( angle.y );// �O�p���΂߂ɂ��Ĉʒu��ς���
   //     dir.y = 0;
   //     dir.z = cosf( angle.y );
   //     //.31���f��������������ꂼ��P�Q�R�����@����������
   //     // �m���}�炢��
   //     //sinf0�x�O�@cosf0�͂P�x
   //     //�X�Osin1,cos0�Ԃ��Ă��鉡
   //     //�S�Tsin0.5,cos0.5�΂�
   //     // 360�x����肭�\���o����B2d�ł��s����B
   //
   //     
   //     // ���ˈʒu�i�v���C���[�̍�������)
   //     DirectX::XMFLOAT3 pos;
   //     pos.x = position.x;
   //     pos.y = position.y + height*0.5f;// �g�����ʒu�̂�
   //     pos.z = position.z;
   //     // ���ˁ@�X�g���[�g�e�ۂ�p��
   //     ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
   //     projectile->Lanch(dir, pos);
   //     // �e�ۃN���X�Ŏ��Ԍo�ߎ��Q�o�����̂ł���Ȃ�
   //     //projectileManager.Register(projectile);
   //     return true;
   // }
   //
   // // �ǔ��e�۔���
   // if (gamePad.GetButtonDown() & GamePad::BTN_Y)
   // {
   //     // �O���� sin�̌v�Z
   //     DirectX::XMFLOAT3 dir;
   //     dir.x = sinf(angle.y);// �O�p���΂߂ɂ��Ĉʒu��ς���
   //     dir.y = 0;
   //     dir.z = cosf(angle.y);
   //     //.31���f��������������ꂼ��P�Q�R�����@����������
   //     // �m���}�炢��
   //     //sinf0�x�O�@cosf0�͂P�x
   //     //�X�Osin1,cos0�Ԃ��Ă��鉡
   //     //�S�Tsin0.5,cos0.5�΂�
   //     // 360�x����肭�\���o����B2d�ł��s����B
   //
   //
   //     // ���ˈʒu�i�v���C���[�̍�������)
   //     DirectX::XMFLOAT3 pos;
   //     pos.x = position.x;
   //     pos.y = position.y + height * 0.5f;// �g�����ʒu�̂�
   //     pos.z = position.z;
   //     //�^�[�Q�b�g�i�f�t�H���g�ł̓v���C���[�̑O���j
   //     DirectX::XMFLOAT3 target;
   //     // �G�����Ȃ��������̂Ȃ߂Ɂ@1000��܂Ŕ��ł���
   //     target.x = pos.x+dir.x*1000.0f;
   //     target.y = pos.y+dir.y * 1000.0f;
   //     target.z = pos.z+dir.z * 1000.0f;
   //
   //     // ��ԋ߂��̓G���^�[�Q�b�g�ɂ���
   //     float dist = FLT_MAX;// float �̍ő�lfloat�S��
   //     EnemyManager& enemyManager = EnemyManager::Instance();
   //     int enemyCount = enemyManager.GetEnemyCount();
   //     for (int i = 0; i < enemyCount; ++i)//float �ő�l�Ȃ��ɂ���G�Ɍ�����
   //     {
   //         // �G�Ƃ̋�������  �G�̐����v�� �S�Ă̓G���Ăɓ����
   //         Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
   //         DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
   //         // �G�̈ʒu
   //         DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
   //         // ��������G�܂ł̈ʒu���v��
   //         DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
   //         // �x�N�g���̂Ȃ������Q�悷��B����Ă��Ȃ��z
   //         DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
   //         float d;
   //         DirectX::XMStoreFloat(&d, D);
   //         if (d < dist)
   //         {
   //             // �������G�̂��̂����鏭�Ȃ�����R�O�Ȃ�R�O�A�P�O�O�Ȃ�P�O�O�����
   //             dist = d;
   //             target = enemy->GetPosition();// �ʒu������
   //             target.y += enemy->GetHeight() * 0.5f;// �ʒu�ɐg����
   //         }
   //     }
   //
   //
   //     // ���ˁ@�X�g���[�g�e�ۂ�p��
   //     ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
   //     projectile->Lanch(dir, pos,target);
   //
   //     return true;
   //
   // }
    return false;
}

bool Player::InputAttack()
{
    // �U�����͏���

    GamePad& gamePad = Input::Instance().GetGamePad();

    // ���i�e�۔��ˁ@x�{�^������������
    if (gamePad.GetButtonDown() & GamePad::BTN_B)
    {
  
        return true;
    }

    return false;
}

// �ҋ@�X�e�[�g�֑J��
void Player::TransitionIdleState()
{
    state = State::Idle;

    // �ҋ@�A�j���[�V�����Đ�
    //model->PlayAnimation(Anim_Idle, true);

     // ����A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Running, true);
}

// �ҋ@�X�e�[�g�X�V����
void Player::UpdateIdleState(float elapsedTime)
{
    // �ړ����͏���
    if (InputMove(elapsedTime))
    {
       
        TransitionMoveState();
    }

    // �W�����v���͏���
    if (InputJump())
    {
        
        TransitionJumpState();
    }

    if (InputAttack())
    {
        stated = state;
        TransitionAttackState();
    }

    // �e�ۓ��͏���
    if (InputProjectile())
    {
        TransitionAttackState();
    }


    // �ړ����͏���
    //InputMove(elapsedTime);


}

void Player::TransitionMoveState()
{
    state = State::Move;

    // ����A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Running, true);
}

void Player::ChangeState(State state)
{
    //���쌠���Ȃ���̃X�e�[�g�J��
    if (GetState() != state)
    {
        switch (state)
        {
        case State::Idle:
            TransitionIdleState();
            break;
        case State::Move:
            TransitionMoveState();
            break;
        case State::Jump:
            TransitionJumpState();
            break;
        case State::Land:
            TransitionLandState();
            break;
        case  State::JumpFlip:
            TransitionJumpFlipState();
            break;
        }
    }
}

void Player::UpdateMoveState(float elapsedTime)
{
    // �ړ����͏���
    if (!InputMove(elapsedTime))
    {
        
        TransitionIdleState();
    }


    // �W�����v���͏���
    if (InputJump())
    {
        
        TransitionJumpState();
    }

    if (InputAttack())
    {
        stated = state;
        TransitionAttackState();
    }

    //// �W�����v���͏���
    //InputJump();

    // �e�ۓ��͏���
    if (InputProjectile())
    {
        TransitionAttackState();
    }

    //InputMove(elapsedTime);

    //// �ړ����͏���
    //InputMove(elapsedTime);

}




void Player::TransitionJumpState()
{
    state = State::Jump;

    // �W�����v�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Jump, true);
}

void Player::UpdateJumpState(float elapsedTime)
{

    InputMove(elapsedTime);

    // �W�����v���͏���
    if (InputJump()&&jumpCount == 2)
    {
        TransitionJumpFlipState();
    }

    // �W�����v�����
    //if (inputJump())
    // {
    // // �W�����v�t���b�v�A�j���[�V�����Đ�
    //  model->PlayAnimation(Anim_Jump_Flip, false);
    //  }
    // 
    // // �W�����v�A�j���[�V�����I����
    // if(!model->IsPlayaAnimation())
    // {
    // �����A�j���[�V�����Đ�
    // model->PlayAnimtion(Anim_Falling,true);
    // }
    // 
    //
  

    // �e�ۓ��͏���
    if (InputProjectile())
    {
        TransitionAttackState();
    }



}

void Player::TransitionLandState()
{
    state = State::Land;

    // ���n�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Landing, false);
}

void Player::UpdateLandState(float elapsedTime)
{
   

    // �����I�������ҋ@�ɕύX
    if (!model->IsPlayAnimation())
    {

        TransitionIdleState();
    }


 
}





void Player::TransitionJumpFlipState()
{
    state = State::JumpFlip;

    // ����A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Jump_Flip, false);
}

void Player::UpdatejumpFlipState(float elapsedTime)
{
    //// �ړ����͏���
    //if (!InputMove(elapsedTime))
    //{
    //    TransitionIdleState();
    //}

    InputMove(elapsedTime);
    //// �W�����v���͏���
    //if (!model->IsPlayAnimation())
    //{
    //    TransitionJumpState();
    //}
}

void Player::TransitionAttackState()
{
    state = State::Attack;

    // ����A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Attack, false);
}

void Player::UpdateAttackState(float elapsedTime)
{
    // �����I�������ҋ@�ɕύX
    if (!model->IsPlayAnimation())
    {
        attackCollisionFlag = false;
        switch (stated)
        {
        case Player::State::Idle:
            TransitionIdleState();
            break;
        case Player::State::Move:
            TransitionMoveState();
            break;
        }
        //TransitionIdleState();
       
    }
    
    // �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
    float animationTime = model->GetCurrentANimationSeconds();
    // ��肭�s���ΓG������s��������Ă����s����p�ӏo����B
    attackCollisionFlag = animationTime >= 0.3f && animationTime <= 0.4f;
    if (attackCollisionFlag)
    {
        // ����m�[�h�ƃG�l�~�[�̏Փˏ���
        CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
    }
}

void Player::TransitionDamageState()
{
    state = State::Damage;

    // �_���[�W�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_GetHit1, false);
}

void Player::UpdateDamageState(float elapsedTime)
{
    // �_���[�W�A�j���[�V�����I�������ҋ@�X�e�[�g��
    if (!model->IsPlayAnimation())
    {

            TransitionIdleState();


    }
}

void Player::TransitionDeathState()
{
    state = State::Death;

    // �_���[�W�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Death, false);
}

void Player::UpdateDeathState(float elapsedTime)
{   
    
    if (!model->IsPlayAnimation())
    {

       // �{�^�����������畜���X�e�[�g�֑J��
        GamePad& gamePad = Input::Instance().GetGamePad();
        if (gamePad.GetButtonDown() & GamePad::BTN_A)
        {
            TransitionReviveState();
        }


    }

}
// �����X�e�[�g�J��
void Player::TransitionReviveState()
{
    state = State::Revive;

    // �̗͉�
    health = maxHealth;

    // �_���[�W�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Revive, false);
}

void Player::UpdateReviveState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        TransitionIdleState();

    }
}
