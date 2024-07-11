#include "ProjectileStraight.h"
#include "Collision.h"
#include "StageManager.h"
// �R���X�g���N�^
// �}�l�[�W���[�̃A�h���X�����ɓn���Ȃ���Projectile(manager)����ōs����
// ���Ȃ��ƃG���[
ProjectileStraight::ProjectileStraight(ProjectileManager* manager)
                    :Projectile(manager)
{
    //model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
    model = new Model("Data/Model/Sword/Sword.mdl");

    // �\���T�C�Y�𒲐�
    //scale.x = scale.y = scale.z = 0.5f;// �Ȃ�ƂȂ������悤��
    scale.x = scale.y = scale.z = 3.0f;
}
//ProjectileStraight::ProjectileStraight(ProjectileManager* manager)
//{
//}
// �f�X�g���N�^
ProjectileStraight::~ProjectileStraight()
{

    delete model;
}
// �X�V����
void ProjectileStraight::Update(float elapsedTime)
{
    //�@�������� 
    //float elapsedFrame = 60.0f * elapsedTime;
    //lifeTimer -= lifeTimer* elapsedFrame;
    //if (elapsedTime == 60) lifeTimer--;
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)// �������s�����玩�Q
    {
        // �������폜
        Destroy();
    }
    // �ړ��@�@�b����Ȃ��ƃt���[����
    float speed = this->speed * elapsedTime;
    velocity = { speed,speed,speed };
    // �@�����炩���Z�Ō����ɂ��ꂾ���i������ʒu�ɓ��ꂽ
    position.x += speed * direction.x;
    position.y += speed * direction.y;
    position.z += speed * direction.z;

    UpdateReflection(elapsedTime);

    // �I�u�W�F�N�g�s����X�V�@�ς�����ʒu�̍s���
    UpdateTransform();

    // ���f���s��X�V�@����������炱���Ɍ�����
    model->UpdateTransform(transform);
}
// �`�揈��
void ProjectileStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}
// ����
void ProjectileStraight::Lanch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3&position)
{
    // direction �����@���K�����ē���鍷���Ȃ���X�s�[�h�ς��
    this->direction = direction;
    this->position = position;
}

void ProjectileStraight::UpdateReflection(float elapsedTime)
{
    // �������͗ʌv�Z
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    
    if (velocityLengthXZ > 0.0f)
    {
        // �����ړ��l
        float mx = speed * elapsedTime;
        float mz = speed * elapsedTime;

        // ���C�̊J�n�ʒu�ƏI�_�ʒu
        // �i��������
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // �ړ��������������z
        DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };

        // ���C�L���X�g�ɂ��ǔ���
        HitResult hit;
        if (StageManager::instance().RayCast(start, end, hit))
        {
            // �ǂ܂ł̃x�N�g��
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // �ǂ̖@��
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // ���˃x�N�g����@���Ɏˉe�� a
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);
            Dot = DirectX::XMVectorScale(DirectX::XMVectorScale(Dot,2.0f), 1.1f); // �ǂɂ߂荞�܂Ȃ��悤�ɏ��������␳



            // �␳�ʒu�̌v�Z
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, DirectX::XMVectorScale(Dot, 2.0f), End);
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            // �ǂ�������փ��C�L���X�g
            HitResult hit2;
            if (!StageManager::instance().RayCast(hit.position, collectPosition, hit2))
            {
                // �ǂ�������ŕǂɓ�����Ȃ�������␳�ʒu�Ɉړ�
                position.x = collectPosition.x;
                position.z = collectPosition.z;
                DirectX::XMStoreFloat3(&direction,Dot);
            }
            else
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
        }
        else
        {
            // �ړ�
            position.x += mx;
            position.z += mz;
        }
    }
}



