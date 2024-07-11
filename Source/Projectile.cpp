#include "Projectile.h"
#include "ProjectileManager.h"
#include "Graphics/Graphics.h"
Projectile::Projectile(ProjectileManager* manager)
    :manager(manager)
{
    // ������z��o�^
    manager->Register(this);
}

void Projectile::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // ���͉����\�����Ȃ�
        //// �Փ˔���p�̃f�o�b�O����`��
    debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

}




void Projectile::UpdateTransform()
{
    // �Ƃ肠�����A���ŉ�]�͖��������s����쐬����B
    //transform._11 = scale.x; // �傫������������z�O�D�P�{�Ȃ炻���Ȃ�
    //transform._12 = 0.0f;
    //transform._13 = 0.0f;
    //transform._14 = 0.0f;
    //transform._21 = 0.0f;
    //transform._22 = scale.y;
    //transform._23 = 0.0f;
    //transform._24 = 0.0f;
    //transform._31 = 0.0f;
    //transform._32 = 0.0f;
    //transform._33 = scale.z;
    //transform._34 = 0.0f;
    //transform._41 = position.x;
    //transform._42 = position.y;
    //transform._43 = position.z;
    //transform._44 = 1.0f;

    // ����
    DirectX::XMVECTOR Front, Up, Right;
    // �O�x�N�g�����Z�o�@�܂���������ɓ����
    Front = DirectX::XMLoadFloat3(&direction);
    Front = DirectX::XMVector3Normalize(Front);
    // ���̏�x�N�g�����Z�o�@�������������l�����Ă����^��ɑł�������
    // �����Ɠ����l���ƃo�O�邩��
    Up = DirectX::XMVectorSet(0.001f, 1, 0, 0);
    Up = DirectX::XMVector3Normalize(Up);

    // �E�x�N�g���Z�o�@�����g����̐V�����������
    Right = DirectX::XMVector3Cross(Up, Front);// �Q�łP�̓z���o��

    Right = DirectX::XMVector3Normalize(Right);
    // ��x�N�g���Z�o ����ɍ������ł���ɐV�����ЂƂ����B
    Up = DirectX::XMVector3Cross(Front, Right);// �΂ߏ�̓z�o����



    // �v�Z���ʂ����o��
    DirectX::XMFLOAT3 right, up, front;
    DirectX::XMStoreFloat3(&right, Right);
    DirectX::XMStoreFloat3(&up, Up);
    DirectX::XMStoreFloat3(&front, Front);

    // �Z�o�������x�N�g������s����쐬 ��]����
    transform._11 = right.x * scale.x; // �傫������������z�O�D�P�{�Ȃ炻���Ȃ�
    transform._12 = right.y * scale.x;// �p�x
    transform._13 = right.z * scale.x;
    transform._14 = 0.0f;
    transform._21 = up.x * scale.y;
    transform._22 = up.y * scale.y;// ���Ԃ�
    transform._23 = up.z * scale.y;
    transform._24 = 0.0f;
    transform._31 = front.x* scale.z;
    transform._32 = front.y * scale.z;
    transform._33 = front.z * scale.z;// ����
    transform._34 = 0.0f;
    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;

    this->direction = front;// �������������ɓ���邱��łނ��̌v�Z
    // ���O��
}

void Projectile::Destroy()
{
    // �e�ۂ��玩�����폜�܂�}�l�[�W���[����@ �Ǘ����Ă���z�����������
    manager->Remove(this);
}
