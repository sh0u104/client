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

// インスタンス取得
Player& Player::Instance()
{
    return *instance;
}



// コンストラクタ
Player::Player()
{
    // インスタンスポインタ設定
    instance = this;

    //model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
    model = new Model("Data/Model/Jammo/Jammo.mdl");
    /*model->PlayAnimation(0);*/

    // モデルがおおきいのでスケーリング
    // キャラクターも1.1倍
    scale.x = scale.y = scale.z = 0.01f;

    // ヒットエフェクト読込 
    //hitEffect = new Effect("Data/Effect/Shot_Hit.efk");
    hitEffect = new Effect("Data/Effect/sunder.efk");
    desEffect = new Effect("Data/Effect/F.efk");


    // 待機ステートへ遷移
    TransitionIdleState();
}

// デストラクタ
Player::~Player()
{
    delete hitEffect;
    delete desEffect;
    delete model;

}




// 更新処理
// elapsedTime(経過時間)
void Player::Update(float elapsedTime)
{
    // ステート毎の処理
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


    // 速力処理更新
    UpdateVelocity(elapsedTime);

    UpdateInbincibleTimer(elapsedTime);

    //hitEffect->SetPosition(hitEffect->GetEfeHandle(), this->position);
    //hitEffect->SetScale(hitEffect->GetEfeHandle(),{ 0.1f,0.1f,0.1f });
    hitEffect->SetScale(hitEffect->GetEfeHandle(), { 1,1,1 });

    {
        //// 入力情報を取得 // INPUTからゲームパット貰える
        //GamePad& gamePad = Input::Instance().GetGamePad();
        //float ax = gamePad.GetAxisLX();// 左スティック
        //float ay = gamePad.GetAxisLY();

        //// 移動操作
        //float moveSpeed = 5.0f * elapsedTime;// 移動速度
        //{
        //    // 左スティックの入力情報をもとにXZ平面への移動処理
        //    position.x += ax * moveSpeed;//5倍する
        //    position.z += ay * moveSpeed;
        //}

        //// 回転処理
        //float rotateSpeed = DirectX::XMConvertToRadians(360) * elapsedTime;// １秒間に360度する
        //if (gamePad.GetButton() & GamePad::BTN_A)
        //{
        //    // X軸回転操作
        //    angle.x += rotateSpeed;
        //}
        //if (gamePad.GetButton() & GamePad::BTN_B)
        //{
        //    // Y軸回転操作
        //    angle.y += rotateSpeed;

        //}
        //if (gamePad.GetButton() & GamePad::BTN_X)
        //{
        //    // Z軸回転操作
        //    angle.z += rotateSpeed;

        //}
    }
    // プレイヤーと敵との衝突処理
    CollisionPlayerVsEnemies();

    CollisionProjectilesVsEnemies();

    // 弾丸更新処理
    //projectileManager.Update(elapsedTime);

    // オブジェクト行列を更新
    UpdateTransform();

    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);

    // モデル行列更新
    // 何処に出してほしいここに 
    model->UpdateTransform(transform);


    //position = position;

}



// デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
    // もし立法体等ほしいなら自分で追加
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //// 衝突判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

    // 衝突判定用のデバッグ円柱を描画
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

    // 弾丸デバッグプリミティブ描画
    //projectileManager.DrawDebugPrimitive();

    if (attackCollisionFlag)
    {


        // 攻撃衝突用の左手ノードデバッグ球を描画
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












// 移動入力処理
bool Player::InputMove(float elapsedTime)
{
   
    // 進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();
    // 移動処理
   /* Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);*/
    Move(moveVec.x, moveVec.z, moveSpeed);
    // 旋回処理
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
    // 進行ベクトルがゼロベクトルでない場合は入力された
    //return moveVec.x ? true:false;
    
    //if (moveVec.x || moveVec.z)
    //    return true;
    if (!Getoperation())
    {
        return false;
    }
    //return false;
    // あってたらtrueに間違ってたらfalseになる。
    return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}


DirectX::XMFLOAT3 Player::GetMoveVec() const
{
   
    // 入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();
    //float ax = gamePad.GetAxisLX();
    float ax = 0;
    float ay = mouselength / 50;

    // カメラ方向とスティックの入力値によって進行方向を計算する
   //Camera& camera = Camera::Instance();
   //const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
   //const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
    // 移動ベクトルはXZ平面に水平なベクトルになるようにする
   // // カメラ右方向ベクトルはＸＺ平面に水平なベクトルに変換
   // float cameraRightX = cameraRight.x;
   // float cameraRightZ = cameraRight.z;
   // // y成分を取らずに　矢印の長さを取得
   // float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
   // // 何故Y方向を消してるか　右方向が斜めでも真っ直ぐ進んでほしいYを０
   // //　にする少し距離が変わるだから単位ベクトルにする１．０に
   // if (cameraRightLength > 0.0f)
   // {
   //     // 単位ベクトル化
   //     // 右方向の単位ベクトル 
   //     cameraRightX = cameraRightX / cameraRightLength;
   //     cameraRightZ = cameraRightZ / cameraRightLength;
   //
   //     /*        if (cameraRightLength > DirectX::XM_PI)cameraRightLength -= DirectX::XM_PI * 2;
   //             if (cameraRightLength < -DirectX::XM_PI)cameraRightLength += DirectX::XM_PI * 2;
   //       */
   // }
 
    DirectX::XMVECTOR forward = DirectX::XMVectorSet(cos(mouseAngle), 0, sin(mouseAngle), 0);
    // カメラ前方向ベクトルをXZ単位ベクトルに変換
    //float cameraFrontX = cameraFront.x;
    float cameraFrontX = DirectX::XMVectorGetX(forward);
    //float cameraFrontZ = cameraFront.z;
    float cameraFrontZ = DirectX::XMVectorGetZ(forward);
    cameraFrontX *= -1;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        // 単位ベクトル化

        cameraFrontX = cameraFrontX / cameraFrontLength;
        cameraFrontZ = cameraFrontZ / cameraFrontLength;
    }

    // 進行ベクトルを計算する
    DirectX::XMFLOAT3 vec;// 移動方向進むべき方向進行ベクトル
    // ax,ayスティックの具合　cameraRightX（カメラ）
    // (cameraRightX* ax) + (cameraFrontX * ay)これは上の進方向を真っ直ぐにする奴
    // 
    vec.x = (cameraFrontX * ay);
    vec.z = (cameraFrontZ * ay);
    // Y軸方向には移動しない
    vec.y = 0.0f;
    return vec;



}

// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    // 描画
    shader->Draw(dc, model);
    // 弾丸描画処理
   // projectileManager.Render(dc, shader);
}





// 弾丸と敵の衝突処理

void Player::CollisionProjectilesVsEnemies()
{
    //EnemyManager& enemyManager = EnemyManager::Instance();
    //
    //// 全ての敵と総当たりで衝突処理
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
    //        //// 衝突処理
    //        DirectX::XMFLOAT3 outPositon;
    //
    //        //}
    //        // 円柱と円
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
    //            // ダメージを与える。
    //            //enemy->ApplyDamage(1);
    //        // ダメージが通ったら消える。TRUEになるから
    //            if (enemy->ApplyDamage(1, 0.5f))
    //            {
    //                // 吹き飛ばす
    //                {
    //                    // 衝動
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
    //                // ヒットエフェクト再生
    //                {
    //                    DirectX::XMFLOAT3 e = enemy->GetPosition();
    //                    e.y += enemy->GetHeight() * 0.5f;
    //                    
    //                    
    //                    hitEffect->Play(e);
    //                    //desEffect->Play(e);
    //          
    //                }
    //                // 弾丸破棄
    //                projectile->Destroy();
    //            }
    //        }
    //    }
    //
    //
    //}
}

// プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての敵と総当たりで衝突処理

    int enemyCount = enemyManager.GetEnemyCount();


        for (int i = 0; i < enemyCount; ++i)
        {
            Enemy* enemy = enemyManager.GetEnemy(i);


            //// 衝突処理
            DirectX::XMFLOAT3 outPositon;
            //if (Collision::IntersectSpherVsSphere(
            //    position, radius,
            //    enemy->GetPosition(),
            //    enemy->GetRadius(),
            //    outPositon
            //))
            //{
            //// 押し出し後の位置設定
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
                //    // 押し出し後の位置設定
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
                            //小ジャンプ
                            Jump(jumpSpeed * 0.5f);
                        }

                    }
                    else
                    {
                        // 押し出し後の位置設定　
                        enemy->SetPosition(outPositon);
                    }
                
            }
            // 
            //// 衝突処理円柱
            //if (Collision::IntersectCylinderVsCylinder((

            //    ))
            //{
            //    enemy->SetPosition(outPositon);
            //}


        
            

        }
    
    


}

// ノードと敵の衝突判定
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{

    // ノード取得
    Model::Node* node = model->FindNode(nodeName);
    //worldTransform
    //localTransform
    // ノード位置取得
    DirectX::XMFLOAT3 nodePosition;
    nodePosition = {
        node->worldTransform._41,
        node->worldTransform._42,
        node->worldTransform._43
    };
    // マネージャー取得
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();
    // 指定のノードと全ての敵を総当たりで衝突処理
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);


        //// 衝突処理
        DirectX::XMFLOAT3 outPositon;

        // 円柱と円
        if (Collision::IntersectSphereVsCylinder(
            nodePosition,
            nodeRadius,
            enemy->GetPosition(),
            enemy->GetRadius(),
            enemy->GetHeight(),
            outPositon))

        {

            // ダメージを与える。
            //enemy->ApplyDamage(1);
        // ダメージが通ったら消える。TRUEになるから
            if (enemy->ApplyDamage(1, 0.5f))
            {

                // 吹き飛ばす
                {
                    // 衝動
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
                // ヒットエフェクト再生
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



// デバッグ用GUI描画
void Player::DrawDebugGUI()
{   
}

// 着地した時に呼ばれる
void Player::OnLanding()
{
    // 着地したからリセット
    jumpCount = 0;

    //// 下方向の速力が一定以上なら着地ステートへ  十分な速度で落とす重力の５倍２、３秒後に着地モーションをする。
    //if (velocity.y < jumpfliptime)
    //{

    //    TransitionLandState();
    //}

    if (state != State::Damage && state != State::Death)
    {
        // 着地ステートへ遷移
        TransitionLandState();
    }

}

void Player::OnDead()
{


    // 死亡ステート遷移
    TransitionDeathState();
}

void Player::OnDamaged()
{
    
        // ダメージステートへ遷移
    TransitionDamageState();
}

//DirectX::XMFLOAT3 Player::GetMoveVec() const
//{
//    // 入力値を取得
//    GamePad& gamePad = Input::Instance().GetGamePad();
//    float ax = gamePad.GetAxisLX();// 左スティック
//    float ay = gamePad.GetAxisLY();
//
//    // カメラ方向とスティックの入力値によって進行方向を計算する
//    Camera& camera = Camera::Instance();
//    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
//    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
//
//    // 移動ベクトルはXZ平面に水平なベクトルになるようにする
//
//    // カメラ右方向ベクトルをXZ単位ベクトルに変換
//    float cameraRightX = cameraRight.x;
//    float cameraRightZ = cameraRight.z;
//    float cameraRightLenghth = cameraRightX+ cameraRightZ;
//    if (cameraRightLenghth > 0.0f)
//    {
//        //距離化　正規化
//    }

//}
bool Player::InputJump()
{
    if (!Getoperation())
    {
        return false;
    }
    // ボタンで入力でジャンプ（ジャンプ回数制限つき）
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        // 値を小さくする
        if (jumpCount < jumpLimit) {
            //地面につくまでfalse
            isGround = false;
            // ジャンプの最大値
            ++jumpCount;
            Jump(jumpSpeed);

            // ジャンプ入力した
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
   // // 直進弾丸発射　cボタンを押したら
   // if (gamePad.GetButtonDown() & GamePad::BTN_X)
   // {
   //     // 前方向 sinの計算 角度の計算
   //     DirectX::XMFLOAT3 dir;
   //     dir.x = sinf( angle.y );// 三角を斜めにして位置を変えた
   //     dir.y = 0;
   //     dir.z = cosf( angle.y );
   //     //.31等デモいいこれをそれぞれ１２３入れる　こういう風
   //     // ノルマらいず
   //     //sinf0度０　cosf0は１度
   //     //９０sin1,cos0返ってくる横
   //     //４５sin0.5,cos0.5斜め
   //     // 360度を上手く表現出来る。2dでも行ける。
   //
   //     
   //     // 発射位置（プレイヤーの腰当たり)
   //     DirectX::XMFLOAT3 pos;
   //     pos.x = position.x;
   //     pos.y = position.y + height*0.5f;// 身長÷位置のｙ
   //     pos.z = position.z;
   //     // 発射　ストレート弾丸を用意
   //     ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
   //     projectile->Lanch(dir, pos);
   //     // 弾丸クラスで時間経過自害出来たのでいらない
   //     //projectileManager.Register(projectile);
   //     return true;
   // }
   //
   // // 追尾弾丸発射
   // if (gamePad.GetButtonDown() & GamePad::BTN_Y)
   // {
   //     // 前方向 sinの計算
   //     DirectX::XMFLOAT3 dir;
   //     dir.x = sinf(angle.y);// 三角を斜めにして位置を変えた
   //     dir.y = 0;
   //     dir.z = cosf(angle.y);
   //     //.31等デモいいこれをそれぞれ１２３入れる　こういう風
   //     // ノルマらいず
   //     //sinf0度０　cosf0は１度
   //     //９０sin1,cos0返ってくる横
   //     //４５sin0.5,cos0.5斜め
   //     // 360度を上手く表現出来る。2dでも行ける。
   //
   //
   //     // 発射位置（プレイヤーの腰当たり)
   //     DirectX::XMFLOAT3 pos;
   //     pos.x = position.x;
   //     pos.y = position.y + height * 0.5f;// 身長÷位置のｙ
   //     pos.z = position.z;
   //     //ターゲット（デフォルトではプレイヤーの前方）
   //     DirectX::XMFLOAT3 target;
   //     // 敵がいなかった時のなめに　1000先まで飛んでくれ
   //     target.x = pos.x+dir.x*1000.0f;
   //     target.y = pos.y+dir.y * 1000.0f;
   //     target.z = pos.z+dir.z * 1000.0f;
   //
   //     // 一番近くの敵をターゲットにする
   //     float dist = FLT_MAX;// float の最大値float全体
   //     EnemyManager& enemyManager = EnemyManager::Instance();
   //     int enemyCount = enemyManager.GetEnemyCount();
   //     for (int i = 0; i < enemyCount; ++i)//float 最大値ないにいる敵に向かう
   //     {
   //         // 敵との距離判定  敵の数も計測 全ての敵をてに入れる
   //         Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
   //         DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
   //         // 敵の位置
   //         DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
   //         // 自分から敵までの位置を計測
   //         DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
   //         // ベクトルのながさを２乗する。√つけていない奴
   //         DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
   //         float d;
   //         DirectX::XMStoreFloat(&d, D);
   //         if (d < dist)
   //         {
   //             // 距離が敵のものを入れる少なくする３０なら３０、１００なら１００入れる
   //             dist = d;
   //             target = enemy->GetPosition();// 位置を入れる
   //             target.y += enemy->GetHeight() * 0.5f;// 位置に身長分
   //         }
   //     }
   //
   //
   //     // 発射　ストレート弾丸を用意
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
    // 攻撃入力処理

    GamePad& gamePad = Input::Instance().GetGamePad();

    // 直進弾丸発射　xボタンを押したら
    if (gamePad.GetButtonDown() & GamePad::BTN_B)
    {
  
        return true;
    }

    return false;
}

// 待機ステートへ遷移
void Player::TransitionIdleState()
{
    state = State::Idle;

    // 待機アニメーション再生
    //model->PlayAnimation(Anim_Idle, true);

     // 走りアニメーション再生
    model->PlayAnimation(Anim_Running, true);
}

// 待機ステート更新処理
void Player::UpdateIdleState(float elapsedTime)
{
    // 移動入力処理
    if (InputMove(elapsedTime))
    {
       
        TransitionMoveState();
    }

    // ジャンプ入力処理
    if (InputJump())
    {
        
        TransitionJumpState();
    }

    if (InputAttack())
    {
        stated = state;
        TransitionAttackState();
    }

    // 弾丸入力処理
    if (InputProjectile())
    {
        TransitionAttackState();
    }


    // 移動入力処理
    //InputMove(elapsedTime);


}

void Player::TransitionMoveState()
{
    state = State::Move;

    // 走りアニメーション再生
    model->PlayAnimation(Anim_Running, true);
}

void Player::ChangeState(State state)
{
    //操作権がないやつのステート遷移
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
    // 移動入力処理
    if (!InputMove(elapsedTime))
    {
        
        TransitionIdleState();
    }


    // ジャンプ入力処理
    if (InputJump())
    {
        
        TransitionJumpState();
    }

    if (InputAttack())
    {
        stated = state;
        TransitionAttackState();
    }

    //// ジャンプ入力処理
    //InputJump();

    // 弾丸入力処理
    if (InputProjectile())
    {
        TransitionAttackState();
    }

    //InputMove(elapsedTime);

    //// 移動入力処理
    //InputMove(elapsedTime);

}




void Player::TransitionJumpState()
{
    state = State::Jump;

    // ジャンプアニメーション再生
    model->PlayAnimation(Anim_Jump, true);
}

void Player::UpdateJumpState(float elapsedTime)
{

    InputMove(elapsedTime);

    // ジャンプ入力処理
    if (InputJump()&&jumpCount == 2)
    {
        TransitionJumpFlipState();
    }

    // ジャンプ入力胥吏
    //if (inputJump())
    // {
    // // ジャンプフリップアニメーション再生
    //  model->PlayAnimation(Anim_Jump_Flip, false);
    //  }
    // 
    // // ジャンプアニメーション終了語
    // if(!model->IsPlayaAnimation())
    // {
    // 落下アニメーション再生
    // model->PlayAnimtion(Anim_Falling,true);
    // }
    // 
    //
  

    // 弾丸入力処理
    if (InputProjectile())
    {
        TransitionAttackState();
    }



}

void Player::TransitionLandState()
{
    state = State::Land;

    // 着地アニメーション再生
    model->PlayAnimation(Anim_Landing, false);
}

void Player::UpdateLandState(float elapsedTime)
{
   

    // もし終わったら待機に変更
    if (!model->IsPlayAnimation())
    {

        TransitionIdleState();
    }


 
}





void Player::TransitionJumpFlipState()
{
    state = State::JumpFlip;

    // 走りアニメーション再生
    model->PlayAnimation(Anim_Jump_Flip, false);
}

void Player::UpdatejumpFlipState(float elapsedTime)
{
    //// 移動入力処理
    //if (!InputMove(elapsedTime))
    //{
    //    TransitionIdleState();
    //}

    InputMove(elapsedTime);
    //// ジャンプ入力処理
    //if (!model->IsPlayAnimation())
    //{
    //    TransitionJumpState();
    //}
}

void Player::TransitionAttackState()
{
    state = State::Attack;

    // 走りアニメーション再生
    model->PlayAnimation(Anim_Attack, false);
}

void Player::UpdateAttackState(float elapsedTime)
{
    // もし終わったら待機に変更
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
    
    // 任意のアニメーション再生区間でのみ衝突判定処理をする
    float animationTime = model->GetCurrentANimationSeconds();
    // 上手く行けば敵が回避行動を取ってくれる行動を用意出来る。
    attackCollisionFlag = animationTime >= 0.3f && animationTime <= 0.4f;
    if (attackCollisionFlag)
    {
        // 左手ノードとエネミーの衝突処理
        CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
    }
}

void Player::TransitionDamageState()
{
    state = State::Damage;

    // ダメージアニメーション再生
    model->PlayAnimation(Anim_GetHit1, false);
}

void Player::UpdateDamageState(float elapsedTime)
{
    // ダメージアニメーション終わったら待機ステートへ
    if (!model->IsPlayAnimation())
    {

            TransitionIdleState();


    }
}

void Player::TransitionDeathState()
{
    state = State::Death;

    // ダメージアニメーション再生
    model->PlayAnimation(Anim_Death, false);
}

void Player::UpdateDeathState(float elapsedTime)
{   
    
    if (!model->IsPlayAnimation())
    {

       // ボタンを押したら復活ステートへ遷移
        GamePad& gamePad = Input::Instance().GetGamePad();
        if (gamePad.GetButtonDown() & GamePad::BTN_A)
        {
            TransitionReviveState();
        }


    }

}
// 復活ステート遷移
void Player::TransitionReviveState()
{
    state = State::Revive;

    // 体力回復
    health = maxHealth;

    // ダメージアニメーション再生
    model->PlayAnimation(Anim_Revive, false);
}

void Player::UpdateReviveState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        TransitionIdleState();

    }
}
