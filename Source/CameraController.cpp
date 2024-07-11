#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"

// 更新処理
void CameraController::Update(float elasedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisRX();
    float ay = gamePad.GetAxisRY();
    // カメラの回転速度
    float speed = rollSpeed * elasedTime ;

    // スティックの入力値にあわせてX軸とY軸を回転

    angle.x += ay * speed;// 上に倒すと上下
    angle.y += ax * speed;// 左右は自分の周りだからｙ
    
    // カメラ回転値を回転行列に変換
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);
   
    // 回転行列から前方向ベクトルを取り出す
    DirectX::XMVECTOR Front = Transform.r[2];// rowのこと
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    // 注視点から後ろベクトル方向に一定距離れたカメラ視点を求める
    DirectX::XMFLOAT3 eye;

    //eye.x = target.x- front.x;
    //eye.y = target.y-front.y;
    //eye.z = target.z-front.z  ;
    // 向きに長さで矢印を伸ばせる、
    // 矢印を出して　伸ばす
    eye.x = target.x - front.x * range ;
    eye.y = target.y - front.y * range ;
    eye.z = target.z - front.z * range ;




    // カメラの視点と注視点を設定
    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
    // X 軸のカメラ回転を制限
    // ダメ
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

    // Y軸の回転値を-3.14～3.13におさまるようにする
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
        // トランスフォーム
        if (ImGui::CollapsingHeader("Transform0.2", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 位置　// 数値をいじる
            ImGui::InputFloat3("angle", &angle.x);
            // 回転
            DirectX::XMFLOAT3 a;
            // XMConvertToDegrees普通の数字を何度にする
            // float単位で出すラジアンを度単位角度に
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            // 角度を出すそれをラジアンたんいに　
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            //// スケール
            //ImGui::InputFloat3("EYE", &eye.x);

        }
    }
    ImGui::End();
}
