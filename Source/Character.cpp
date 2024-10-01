#include "Character.h"
#include "StageManager.h"
#include "Mathf.h"



// 行列更新処理
void Character::UpdateTransform()
{
    // スケールだけ行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x,scale.y,scale.z);
    //// 回転行列だけを作成
    //DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);
   
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);

    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);

    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);

    DirectX::XMMATRIX R =  Y*X*Z;


    // 位置行列だけを作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x,position.y,position.z);
    // 3つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;// 行列は計算順番変えると結果が変わる
    // 計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);// transformにWを入れてる
    
}

// ダメージを与える
//bool Character::ApplyDamage(int damage /*, float invincibleTime*/)
//{
//    // ダメージが変更されたらTRUEを返す。
//    // ダメージが０の場合は健康状態を変更する必要がない
//    if (damage == 0) return false;
//
//    // 死亡している場合は健康状態を変更しない
//    if (health <= 0)return false;
//
//    // ダメージ処理
//    health -= damage;
//
//
//    // 死亡通知　死んでもダメージ伝えてほしい時はそう改造
//    if (health <= 0)
//    {
//        // 継承先クラスに伝える。
//        OnDead();
//    }
//    // ダメージ通知
//    else
//    {
//        // 継承先クラスに伝える。
//        OnDamaged();
//    }
//
//    // 健康状態が変更した場合はtrueを返す
//    return true;
//}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    //  衝動 impulse
    // 速力に力を加える
    this->velocity.x += impulse.x;
    this->velocity.y += impulse.y;
    this->velocity.z += impulse.z;
}

//
// ダメージを与える
bool Character::ApplyDamage(int damage , float invincibleTime)
{
    // ダメージが０の場合は健康状態を変更する必要がない
    if (damage == 0) return false;

    // 死亡している場合は健康状態を変更しない
    if (health <= 0)return false;

    if (this->invincibleTimer >= 0.0f)return false;

    // 何秒無敵
    this->invincibleTimer = invincibleTime;

    // ダメージ処理
    health -= damage;


    // 死亡通知
    if (health <= 0)
    {
        //DirectX::XMFLOAT3 e = enemy->GetPosition();
        //e.y += enemy->GetHeight() * 0.5f;
        //desEffect->Play();
        OnDead();
    }
    // ダメージ通知
    else
    {
        OnDamaged();
    }

    // 健康状態が変更した場合はtrueを返す
    return true;
}


void Character::Move(float vx, float vz, float speed)
{
    //speed *= elapsedTime;
    //position.x += vx * speed;
    //position.z += vz * speed;

    // 移動方向ベクトルを設定
    // 移動方向
    moveVecX = vx;
    moveVecZ = vz;

    // 最大速度設定　速度
    maxMoveSpeed = speed;
    // 一旦記憶
}

//旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    // 1フレームでどれだけ移動
    speed *= elapsedTime;

    // 進行ベクトルが0ベクトルの場合は処理する必要なし
    // これじゃ動かない
    //if (vx == 0)return;
    //if (vz == 0)return;
    if (vx == 0 && vz == 0)return;
    // 0除算しないように割り算０無理
    float length = sqrtf(vx * vx + vz * vz);
    if (length < 0.001f) return;

    //float Lengthx;
    //float Lengthz;



    // 進行ベクトルを単位ベクトル化
    //Lengthx = sqrtf(vx * vx + vz * vz);
    //Lengthz = sqrtf(vx * vx + vz * vz);

    //Lengthx = vx / Lengthx;
    //Lengthz = vz / Lengthz;
    vx /= length;
    vz /= length;

    // 自身の回転値から全方向を求める
    // 進行方向ベクトル求める（angle.y）で求められる
    // 二つとも同じ値でもそもそも出る値が違うから表現出来る
    // まっすぐ前0,1.0等ある。
    float frontX = sinf(angle.y); // ｙ軸を中心にｘ回転する。左右を見る為
    float frontZ = cosf(angle.y); // ｘ軸を中心にｙ回転する。前後判定のため

    //回転角を求めるため、２つの単位ベクトルの内積を計算する
    // 両方とも単位ベクトルじゃなかったらＣｏｓθ出ない
    // 片方違ったら距離が出る
    float dot = (frontX * vx) + (frontZ * vz);// 内積

    // 内積値は-1.0～1.0で表現されており、２つの単位ベクトルの角度が
    // 小さいほと1.0に近づくという性質を利用して回転速度を調整する
    float rot = 1.0 - dot;// 補正値 角度が小さい程値が小さくなる
    // if(回転速度が5度よりも速いと)毎フレームが５度よりも
    if (rot > speed) rot = speed;

    //// 単位ベクトル化
    //frontX = frontX / frontX;
    //frontZ = frontZ / frontZ;


    // 左右判定を行う為に２つの単位ベクトルの外積を計算する
             //  (進行方向＊進行ベクトル)
    //float cross = (frontX * Lengthz) - (frontZ * Lengthx);
    float cross = (frontZ * vx) - (frontX * vz);
    // 2Dの外積値が正の場合か負の場合によって左右判定が行える
    // 左右判定を行うことによって左右回転を選択する
    if (cross < 0.0f)
    {
        // がくがく
        // 小さくなればそれだけrot×
        //angle.y -= speed*rot;　回転量が少ない
        angle.y -= rot;
    }
    else
    {
        // がくがく
        //angle.y += speed*rot;
        angle.y += rot;
    }


}

void Character::Jump(float speed)
{
    velocity.y = speed /*/ 60.0f*/;
    //position.y = (std::min)(position.y, speed);
}

// 速力処理
//elapsedTime÷60これが   elapsedTime経過時間
void Character::UpdateVelocity(float elapsedTime)
{ 
    float elapsedFrame = 60.0f * elapsedTime;

    //send前計算
    {
        // 垂直速力更新処理
        UpdateVerticalVelocity(elapsedFrame);

        // 水平速力更新処理
        UpdateHorizontalVelocity(elapsedFrame);
    }


    if (isEnemy)
    {
        EnemyVerticalMove(elapsedTime);
       EnemyHorizontalMove(elapsedTime);
    }
    //recv後の値を使う
    else
    {
        // 垂直移動更新処理
        UpdateVerticalMove(elapsedTime);

        // 水平移動更新処理
        UpdateHorizontalMove(elapsedTime);
    }
}

// 無敵時間更新
void Character::UpdateInbincibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}

void Character::UpdateVerticalVelocity(float elapsedFrame)
{
    // 重力処理
    this->velocity.y += this->grabity * elapsedFrame;
  
    
}

// 垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
   
    // 垂直方向の移動量
    float my = velocity.y * elapsedTime;
    float mry = RecvVelocity.y * elapsedTime;
    slopeRate = 0.0f;

    // キャラクターのY軸方向となる法線ベクトル
    DirectX::XMFLOAT3 normal = { 0,1,0 };

    // 落下中
    if (my < 0.0f)
    {
        // レイの開始位置は足元より少し上
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x, position.y + my , position.z };

        // レイキャストによる地面判定
        HitResult hit;
        // レイキャストを呼ぶための関数
        //if (StageManager::instance().RayCast(start, end, hit))
        if (StageManager::instance().RayCast(start, end, hit))
        {

            // 法線ベクトル取得
            normal = hit.normal;

            //// レイキャストによる地面判定
            //HitResult hit;

            // 地面に設置している
            //position.y = hit.position.y;
            position = hit.position;

            // 回転
            // ｙだけな理由は角度が真横になった時にクモみたいにひっつかないように
            angle.y += hit.rotation.y;

            // 傾斜率の計算 法線とXZ平面に置けるベクトルとｙ軸のベクトルで傾きが求められる。
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));
            // 着地
            if (!isGround)
            {
                // 着地時に呼ぶ
                OnLanding();
            }
            isGround = true;
            velocity.y = 0.0f;
            //RecvVelocity.y = 0.0f;

        }
        else
        {
            // 空中に浮いている
            position.y += mry;
            isGround = false;
        }

    }

    // 上昇中
    else if (mry > 0.0f)
    {
        position.y += mry;
        isGround = false;
    }

    // 地面の向きに沿うようにXZ軸回転
    {
        // Y軸が法線ベクトル方向に向くオイラー角回転を算出する

        float storageAngleX = atan2f(normal.z, normal.y);

        float storageAngleZ = -atan2f(normal.x, normal.y);

        // 線形補完で滑らかに回転する
        // (変化する値,最終的な値,これだけ進)
        angle.x = Mathf::Lerp(angle.x, storageAngleX, 0.2f);
        angle.z = Mathf::Lerp(angle.z, storageAngleZ, 0.2f);

    }


}

void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
    // XZ平面の速力を減速する 速度の長さ
    float length = sqrtf(velocity.x*velocity.x+velocity.z*velocity.z);
    // 0以上なら移動
    if (length > 0.0f)
    {
        // 摩擦力
        float friction = this->friction * elapsedFrame;

        // 空中にいるときは摩擦力を減らす
        if (!isGround) friction *= airControl;// 元々の摩擦の計数を減らす為に0.3×。30パーセント
        // 摩擦による横方向の減速処理
        if (length > friction)
        {
            // 単位ベクトル化する。
            float vx = velocity.x / length;
            float vz = velocity.z / length;
            // 減速抵抗力の分だけ
            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        // 横方向の速力が摩擦力以下になったので速力を無効化 １秒間に50センチ以上動かない
        else
        {
            velocity.x = 0;
            velocity.z = 0;

        }
    }

    // XZ平面の速力を加速する
    if (length <= maxMoveSpeed)
    {
        // 移動ベクトルがぜろベクトルでないなら加速する
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ* moveVecZ);
        if (moveVecLength > 0.0f)
        {
            float accelration = this->acceleration * elapsedFrame;
            // 空中にいるときは摩擦力を減らす
            if (!isGround) accelration *= airControl;// 鈍くする。30パーセント
            // 移動ベクトルによる加速処理 また速度をいじったから
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;

            // 最大速度制御　ここで速度を測定する。
            //float lengt = sqrtf(velocity.x* velocity.x+ velocity.z* velocity.z);
            float length = sqrtf(velocity.x* velocity.x+ velocity.z* velocity.z);
            if (length > maxMoveSpeed)
            {
                float vx = velocity.x / length;
                float vz = velocity.z / length;

                velocity.x = vx * maxMoveSpeed;
                velocity.z = vz * maxMoveSpeed;
            }
            // 下り坂でガタガタしないようにする
            if (isGround && slopeRate > 0.0f)
            {
                velocity.y -= length * slopeRate * elapsedFrame;
            }
        }
    }
    // 移動ベクトルをリセット アクセル踏んでる状態を解除
    moveVecX = 0.0f;
    moveVecZ = 0.0f;

}

void Character::UpdateHorizontalMove(float elapsedTime)
{
   
    // 水平速力量計算
    float velocityLengthXZ = sqrtf(RecvVelocity.x* RecvVelocity.x+ RecvVelocity.z* RecvVelocity.z);
    if (velocityLengthXZ > 0.0f)
    {
        // 水平移動値
        float mx = RecvVelocity.x * elapsedTime;
        float mz = RecvVelocity.z * elapsedTime;

        // レイの開始位置と終点位置
        // 段差分高く
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // 移動方向分足した奴
        DirectX::XMFLOAT3 end = { position.x+mx, position.y + stepOffset, position.z+mz };

        // レイキャストによる壁判定
        HitResult hit;
        if(StageManager::instance().RayCast(start, end, hit))
        {
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End,Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影長 a
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec),Normal);
            Dot = DirectX::XMVectorScale(Dot, 1.1f); // 壁にめり込まないように少しだけ補正

            // 補正位置の計算
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            // 壁ずり方向へレイキャスト
            HitResult hit2;
            if (!StageManager::instance().RayCast(hit.position, collectPosition, hit2))
            {
                // 壁ずり方向で壁に当たらなかったら補正位置に移動
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
        }
        else
        {
            // 移動
            position.x += mx;
            position.z += mz;
        }
    }


}

void Character::UpdateReflection(float elapsedTime)
{
    // 水平速力量計算
    float velocityLengthXZ = sqrtf(RecvVelocity.x * RecvVelocity.x + RecvVelocity.z * RecvVelocity.z);
    if (velocityLengthXZ > 0.0f)
    {
        // 水平移動値
        float mx = RecvVelocity.x * elapsedTime;
        float mz = RecvVelocity.z * elapsedTime;

        // レイの開始位置と終点位置
        // 段差分高く
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // 移動方向分足した奴
        DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };

        // レイキャストによる壁判定
        HitResult hit;
        if (StageManager::instance().RayCast(start, end, hit))
        {
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影長 a
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);
            Dot = DirectX::XMVectorScale(DirectX::XMVectorScale(Dot,2.0f), 1.1f); // 壁にめり込まないように少しだけ補正

            // 補正位置の計算
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            // 壁ずり方向へレイキャスト
            HitResult hit2;
            if (!StageManager::instance().RayCast(hit.position, collectPosition, hit2))
            {
                // 壁ずり方向で壁に当たらなかったら補正位置に移動
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
        }
        else
        {
            // 移動
            position.x += mx;
            position.z += mz;
        }
    }
}

void Character::EnemyVerticalMove(float elapsedTime)
{
    // 垂直方向の移動量
    float my = velocity.y * elapsedTime;
  
    slopeRate = 0.0f;

    // キャラクターのY軸方向となる法線ベクトル
    DirectX::XMFLOAT3 normal = { 0,1,0 };

    // 落下中
    if (my < 0.0f)
    {
        // レイの開始位置は足元より少し上
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x, position.y + my , position.z };

        // レイキャストによる地面判定
        HitResult hit;
        // レイキャストを呼ぶための関数
        //if (StageManager::instance().RayCast(start, end, hit))
        if (StageManager::instance().RayCast(start, end, hit))
        {

            // 法線ベクトル取得
            normal = hit.normal;

            //// レイキャストによる地面判定
            //HitResult hit;

            // 地面に設置している
            //position.y = hit.position.y;
            position = hit.position;

            // 回転
            // ｙだけな理由は角度が真横になった時にクモみたいにひっつかないように
            angle.y += hit.rotation.y;

            // 傾斜率の計算 法線とXZ平面に置けるベクトルとｙ軸のベクトルで傾きが求められる。
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));
            // 着地
            if (!isGround)
            {
                // 着地時に呼ぶ
                OnLanding();
            }
            isGround = true;
            velocity.y = 0.0f;
            //RecvVelocity.y = 0.0f;

        }
        else
        {
            // 空中に浮いている
            position.y += my;
            isGround = false;
        }

    }

    // 上昇中
    else if (my > 0.0f)
    {
        position.y += my;
        isGround = false;
    }

    // 地面の向きに沿うようにXZ軸回転
    {
        // Y軸が法線ベクトル方向に向くオイラー角回転を算出する

        float storageAngleX = atan2f(normal.z, normal.y);

        float storageAngleZ = -atan2f(normal.x, normal.y);

        // 線形補完で滑らかに回転する
        // (変化する値,最終的な値,これだけ進)
        angle.x = Mathf::Lerp(angle.x, storageAngleX, 0.2f);
        angle.z = Mathf::Lerp(angle.z, storageAngleZ, 0.2f);

    }
}

void Character::EnemyHorizontalMove(float elapsedTime)
{
    // 水平速力量計算
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (velocityLengthXZ > 0.0f)
    {
        // 水平移動値
        float mx = velocity.x * elapsedTime;
        float mz = velocity.z * elapsedTime;

        // レイの開始位置と終点位置
        // 段差分高く
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // 移動方向分足した奴
        DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };

        // レイキャストによる壁判定
        HitResult hit;
        if (StageManager::instance().RayCast(start, end, hit))
        {
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影長 a
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);
            Dot = DirectX::XMVectorScale(Dot, 1.1f); // 壁にめり込まないように少しだけ補正

            // 補正位置の計算
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            // 壁ずり方向へレイキャスト
            HitResult hit2;
            if (!StageManager::instance().RayCast(hit.position, collectPosition, hit2))
            {
                // 壁ずり方向で壁に当たらなかったら補正位置に移動
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
        }
        else
        {
            // 移動
            position.x += mx;
            position.z += mz;
        }
    }

}



