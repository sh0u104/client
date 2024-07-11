#include "ProjectileStraight.h"
#include "Collision.h"
#include "StageManager.h"
// コンストラクタ
// マネージャーのアドレスを基底に渡さないとProjectile(manager)これで行ける
// しないとエラー
ProjectileStraight::ProjectileStraight(ProjectileManager* manager)
                    :Projectile(manager)
{
    //model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
    model = new Model("Data/Model/Sword/Sword.mdl");

    // 表示サイズを調整
    //scale.x = scale.y = scale.z = 0.5f;// なんとなく合うように
    scale.x = scale.y = scale.z = 3.0f;
}
//ProjectileStraight::ProjectileStraight(ProjectileManager* manager)
//{
//}
// デストラクタ
ProjectileStraight::~ProjectileStraight()
{

    delete model;
}
// 更新処理
void ProjectileStraight::Update(float elapsedTime)
{
    //　寿命処理 
    //float elapsedFrame = 60.0f * elapsedTime;
    //lifeTimer -= lifeTimer* elapsedFrame;
    //if (elapsedTime == 60) lifeTimer--;
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)// 寿命が尽きたら自害
    {
        // 自分を削除
        Destroy();
    }
    // 移動　　秒じゃなくとフレームに
    float speed = this->speed * elapsedTime;
    velocity = { speed,speed,speed };
    // 　だからかけ算で向きにこれだけ進だから位置に入れた
    position.x += speed * direction.x;
    position.y += speed * direction.y;
    position.z += speed * direction.z;

    UpdateReflection(elapsedTime);

    // オブジェクト行列を更新　変わった位置の行列を
    UpdateTransform();

    // モデル行列更新　かわったからここに言って
    model->UpdateTransform(transform);
}
// 描画処理
void ProjectileStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}
// 発射
void ProjectileStraight::Lanch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3&position)
{
    // direction 方向　正規化して入れる差もなきゃスピード変わる
    this->direction = direction;
    this->position = position;
}

void ProjectileStraight::UpdateReflection(float elapsedTime)
{
    // 水平速力量計算
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    
    if (velocityLengthXZ > 0.0f)
    {
        // 水平移動値
        float mx = speed * elapsedTime;
        float mz = speed * elapsedTime;

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
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, DirectX::XMVectorScale(Dot, 2.0f), End);
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            // 壁ずり方向へレイキャスト
            HitResult hit2;
            if (!StageManager::instance().RayCast(hit.position, collectPosition, hit2))
            {
                // 壁ずり方向で壁に当たらなかったら補正位置に移動
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
            // 移動
            position.x += mx;
            position.z += mz;
        }
    }
}



