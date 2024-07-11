#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"

// �X�V����
void CameraController::Update(float elasedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisRX();
    float ay = gamePad.GetAxisRY();
    // �J�����̉�]���x
    float speed = rollSpeed * elasedTime ;

    // �X�e�B�b�N�̓��͒l�ɂ��킹��X����Y������]

    angle.x += ay * speed;// ��ɓ|���Ə㉺
    angle.y += ax * speed;// ���E�͎����̎��肾���炙
    
    // �J������]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);
   
    // ��]�s�񂩂�O�����x�N�g�������o��
    DirectX::XMVECTOR Front = Transform.r[2];// row�̂���
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    // �����_������x�N�g�������Ɉ�苗���ꂽ�J�������_�����߂�
    DirectX::XMFLOAT3 eye;

    //eye.x = target.x- front.x;
    //eye.y = target.y-front.y;
    //eye.z = target.z-front.z  ;
    // �����ɒ����Ŗ���L�΂���A
    // �����o���ā@�L�΂�
    eye.x = target.x - front.x * range ;
    eye.y = target.y - front.y * range ;
    eye.z = target.z - front.z * range ;




    // �J�����̎��_�ƒ����_��ݒ�
    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
    // X ���̃J������]�𐧌�
    // �_��
    //if (angle.x < maxAngleX)
    //{
    //    angle.x += maxAngleX*2;
    //}


    //if (angle.x > minAngleX)
    //{
    //    angle.x -= minAngleX * 2;
    //}

    if (angle.x < minAngleX)
    {
        angle.x = minAngleX;
    }


    if (angle.x > maxAngleX)
    {
        angle.x = maxAngleX;
    }

    // Y���̉�]�l��-3.14�`3.13�ɂ����܂�悤�ɂ���
    if (angle.y < -DirectX::XM_PI)
    {
        angle.y += DirectX::XM_2PI;
    }


    if (angle.y > DirectX::XM_PI)
    {
        angle.y -= DirectX::XM_2PI;
    }
    
}

void CameraController::DrawDebugGUI()
{
    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform0.2", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // �ʒu�@// ���l��������
            ImGui::InputFloat3("angle", &angle.x);
            // ��]
            DirectX::XMFLOAT3 a;
            // XMConvertToDegrees���ʂ̐��������x�ɂ���
            // float�P�ʂŏo�����W�A����x�P�ʊp�x��
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            // �p�x���o����������W�A�����񂢂Ɂ@
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            //// �X�P�[��
            //ImGui::InputFloat3("EYE", &eye.x);

        }
    }
    ImGui::End();
}
