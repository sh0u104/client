#pragma once

#include <DirectXMath.h>
#include "Effect.h"
// �L�����N�^�[
class Character
{
public:
    Character() = default;
    virtual ~Character() = default;

    // ���g��ς����Ȃ��悤��const
    // �����������R�o�p�̒��̕��̖{�̂��Q�Ƃ�n���ς����Ȃ��悤�ɂ���
    // �s��X�V����
    void UpdateTransform();

    // �ʒu�擾
    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    // �ʒu�ݒ�
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // ��]�擾
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }

    // ��]�ݒ�
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    // �X�P�[���擾
    const DirectX::XMFLOAT3& GetSclae() const { return scale; }
    // �X�P�[���擾
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    // �Ռ���^����
    void AddImpulse(const DirectX::XMFLOAT3& impulse);

    // ���a�擾 05
    float GetRadius() const { return radius; }

    // �n�ʂɐݒu���Ă��邩 �G�t�F�N�g����
    bool IsGround() const { return isGround; }

    // �����擾
    float GetHeight() const { return height; }

    // �_���[�W��^���� �_���[�W���󂯂��瑼�ɓn���B
    /*bool ApplyDamage(int damage);*/
    bool ApplyDamage(int damage, float invincibleTime);

    // ���N��Ԃ��擾
    int GetHealth() const { return health; }
    // �ő匒�N��Ԃ��擾
    int GetMaxHealth() const { return maxHealth; }

    //�ǉ�
    DirectX::XMFLOAT3 GetRecvVelocity() { return this->RecvVelocity; }
    void SetRecvVelocity(DirectX::XMFLOAT3 velocity) { this->RecvVelocity = velocity; }

    DirectX::XMFLOAT3 GetRecvPosition() { return this->RecvPosition; }
    void SetRecvPosition(DirectX::XMFLOAT3 position) { this->RecvPosition = position; }

protected:
    // �ړ�����
    //void Move(float elapsedTime, float vx, float vz, float speed);
    void Move(float vx, float vz, float speed);

    // ���񏈗�
    void Turn(float elapsedTime, float vx, float vz, float speed);

    // �W�����v����
    void Jump(float speed);

    // ���͏����X�V
    void UpdateVelocity(float elapsedTime);
    void UpdateInbincibleTimer(float elapsedTime);

    // ���n�������ɌĂׂ��
    virtual void OnLanding() {};

    // �_���[�W���󂯂����ɌĂ΂��
    virtual void OnDamaged() {};
    // ���S�������ɌĂ΂��
    virtual void OnDead() {};
private:
    // �������͍X�V����
    void UpdateVerticalVelocity(float elapsedFrame);

    // �����ړ��X�V����
    void UpdateVerticalMove(float elapsedTime);

    // �������͍X�V����
    void UpdateHorizontalVelocity(float elapsedFrame);
    // �����ړ��X�V����
    void UpdateHorizontalMove(float elapsedTime);

    // ���˒e�ۍX�V����
    void UpdateReflection(float elapsedTime);


protected:// �p����ł��g����
    DirectX::XMFLOAT3             position = { 0, 0, 0 };
    DirectX::XMFLOAT3             angle = { 0, 0, 0 };
    DirectX::XMFLOAT3             scale = { 1, 1, 1 };
    DirectX::XMFLOAT4X4 transform = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    // 05
    float radius = 0.5f;

    float              grabity = -1.0f;

    DirectX::XMFLOAT3  velocity = { 0,0,0 };

    //�ǉ�
    DirectX::XMFLOAT3 RecvVelocity = { 0,0,0 };
    DirectX::XMFLOAT3 RecvPosition = { 0,0,0 };
   

    bool         isGround = false;
    //���[�g��
    float               height = 2.0f;

    int          health = 5;
    int          maxHealth = 5;

    float   invincibleTimer = 1.0f;
    // ���C
    float   friction = 0.5f;

    float acceleration = 1.0f;
    float maxMoveSpeed = 5.0f;
    float moveVecX = 0.0f;
    float moveVecZ = 0.0f;

    float airControl = 0.3f;



    float stepOffset = 1.0f;

    float slopeRate = 1.0f;
};