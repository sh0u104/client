#include "StageManager.h"

void StageManager::Update(float elapsedTime)
{
    for (Stage* stage : stages)
    {
        stage->Update(elapsedTime);
    }
}

void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (Stage* stage : stages)
    {
        stage->Render(dc,shader);
    }
}

void StageManager::Register(Stage* stage)
{
    stages.emplace_back(stage);
}

void StageManager::Clear()
{
    for (Stage* stage : stages)
    {
        delete stage;
    }
    // 全て消してから本体消す。
    stages.clear();
}

bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool result = false;
    hit.distance = FLT_MAX;
    for (Stage* stage : stages)
    {
        HitResult tmp;
        if (stage->RayCast(start, end, tmp))
        {
            if (hit.distance > tmp.distance)
            {
                hit = tmp;
                result = true;


            }
        }
        //DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&start);
        //// 敵の位置
        //DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&end);
        //// 自分から敵までの位置を計測
        //DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
        //// ベクトルのながさを２乗する。√つけていない奴
        //DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
        //float d;
        //DirectX::XMStoreFloat(&d, D);

        //if (d > tmp.distance)
        //{
        //    result = true;
        //}

    }

    return result;
}

bool StageManager::SphereCast(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float radius, float& distance, DirectX::XMFLOAT3& hitPosition, DirectX::XMFLOAT3& hitNormal)
{
   // bool hit = false;
   // DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&origin);
   // DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
   // DirectX::XMVECTOR End = DirectX::XMVectorAdd(Start, DirectX::XMVectorScale(Direction, distance));
   //
   // for (const CollisionMesh::Triangle& triangle : collisionMesh.triangles)
   // {
   //     DirectX::XMVECTOR Positions[3] =
   //     {
   //         DirectX::XMLoadFloat3(&triangle.positions[0]),
   //         DirectX::XMLoadFloat3(&triangle.positions[1]),
   //         DirectX::XMLoadFloat3(&triangle.positions[2])
   //     };
   //
   //     SphereCastResult result;
   //     if (IntersectSphereCastVsTriangle(Start, End, radius, Positions, &result))
   //     {
   //         if (distance < result.distance) continue;
   //         distance = result.distance;
   //         DirectX::XMVECTOR HitPosition = DirectX::XMVectorAdd(Start, DirectX::XMVectorScale(Direction, distance));
   //         DirectX::XMStoreFloat3(&hitPosition, HitPosition);
   //         DirectX::XMStoreFloat3(&hitNormal, result.normal);
   //         hit = true;
   //     }
   // }
   // return hit;
    return false;
}
