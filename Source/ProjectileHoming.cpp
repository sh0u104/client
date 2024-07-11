#include "ProjectileHoming.h"

// �R���X�g���N�^
ProjectileHoming::ProjectileHoming(ProjectileManager* manager)
    : Projectile(manager)
{
    model = new Model("Data/Model/Sword/Sword.mdl");

    // ���f�����������̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 3.0f;
}
// �f�X�g���N�^
ProjectileHoming::~ProjectileHoming()
{
    delete model;
}

// �X�V����
void ProjectileHoming::Update(float elapsedTime)
{
    // ��������
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)// �������s�����玩�Q
    {
        // �������폜
        Destroy();
    }

    // �ړ�
    {
        float moveSpeed = this->moveSpeed * elapsedTime;
        // �@�����炩���Z�Ō����ɂ��ꂾ���i������ʒu�ɓ��ꂽ
        //position.x += moveSpeed * direction.x;
        //position.y += moveSpeed * direction.y;
        //position.z += moveSpeed * direction.z;
        // �L�΂����������{���Ői
        position.x += direction.x*moveSpeed;
        position.y += direction.y*moveSpeed;
        position.z += direction.z*moveSpeed;
    }

    // ����
    {
        float turnSpeed = this->turnSpeed * elapsedTime;

        // �^�[�Q�b�g�܂ł̃x�N�g�����Z�o
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        // �[���x�N�g���łȂ��Ȃ��]�����@�s�b�^�������Ȃ��]�ł��邩��m�F
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
       
        if (lengthSq > 0.00001f)
        {
 
            // �^�[�Q�b�g�܂ł̃x�N�g����P�ʃx�N�g����
            Vec = DirectX::XMVector3Normalize(Vec);

            // �����Ă�����x�N�g�����Z�o�@direction�P�ʃx�N�g���O��
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

            // �O�������x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̓��ρi�p�x�j���Z�o
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);
      


            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            // ��]���x�����Ō�̔������s���߂��Ȃ��悤�ɏ������������o����B
            // �A�[�N�R�T�C���ł��o���邵������u�ł��̒l������B
            // 2�̒P�ʃx�N�g���̊p�x����������
            // 1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
            //if (dot )
            float rot;
            // 1.0f�@dot�͂O�ɋ߂Â����P�ɂȂ邩��[���Ƃ��������O�ɂȂ�
            rot = 1.0f - dot;
            // ������P�̂ق����ł�����΃X�s�[�h������
            // �^�[���X�s�[�h��胍�b�g�̕�������������
            if (rot > turnSpeed)
            {
                rot = turnSpeed;
            }
           

            // ��]�p�x������Ȃ��]��������@�����łO������ĂȂ��ƊO�ς��S�������ɂȂ��Čv�Z�o���Ȃ�
            if (fabsf(rot) >= 0.0001)
            {
                // ��]�����Z�o  �O��  ���������������� 
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);
                // �덷�h�~�ׂ̈ɒP�ʃx�N�g�������������S
                Axis =  DirectX::XMVector3Normalize(Axis);
                // ��]���Ɖ�]�ʂ����]�s����Z�o ��]�ʂ����߂Ă���B
                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);
                // ���݂̍s�����]������@�������g�̎p��
                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                Transform = DirectX::XMMatrixMultiply(Transform,Rotation) ; // ���������炽���~����  Transform*Rotation
                // DirectX::XMMatrixMultrixMultiply
                // ��]��̑O�����������o���A�P�ʃx�N�g��������
                Direction = DirectX::XMVector3Normalize(Transform.r[2]);// row
                DirectX::XMStoreFloat3(&direction,Direction);

            }
        }
    }

            // �I�u�W�F�N�g�s����X�V
            UpdateTransform();

            // ���f���s��X�V
            model->UpdateTransform(transform);
            

            
            
            

        
    
}

// �`�揈��
void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}



void ProjectileHoming::Lanch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}
