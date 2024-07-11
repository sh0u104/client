#include "Collision.h"

// 球と球の交差判定
    bool Collision::IntersectSpherVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB
)
{
    // A->Bの単位ベクトルを算出
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec =  DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);// √外すため 2乗のまま
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);
    
    // lengthsqこれは２乗
    // 距離判定
    float range = radiusA+radiusB;
    if (lengthSq > range*range)// 距離の比較　2乗同士なら行ける
    {
        // 離れているから 飛ばす
        return false;
    }

    // AがBを押し出す
       // ベクトルを作って長さを図る
       // まず長さを１に
    Vec = DirectX::XMVector3Normalize(Vec);
    // 次にlengthSq分だけ×
    Vec = DirectX::XMVectorScale(Vec, range);
    // aのポジションから線を足すとそこから伸びる結果をｂに入れる
    PositionB = DirectX::XMVectorAdd(PositionA, Vec);
    // それをoutPositionBに入れる
    DirectX::XMStoreFloat3(&outPositionB, PositionB);
 
 

    //
    return true;
}

    //if (Collision::IntersectCylinderVsCylinder(
//    position, radius, height,
//    enemy->GetPosition(),
//    enemy->GetRadius(),
//    enemy->GetHeight(),
//    outPositon))

//{


    bool Collision::IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA, 
        float radiusA, 
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB)
    {
        // Aの足元がBの頭よりうえなら当たっていない
        if (positionA.y > heightB + positionB.y)
        {
            return false;
        }
        // Aの頭がBの足元よりしたなら当たっていない
        if (positionA.y +heightA< positionB.y)
        {
            return false;
        }

        // XZ平面での範囲チェック

        float vx = positionB.x - positionA.x;//B-Aであa->B
        float vz = positionB.z - positionA.z;// ÷単位ベクトル
        float range = radiusA + radiusB;// どれだけ押し出す
        float distXZ = sqrtf(vx * vx + vz * vz); // 
        if (distXZ > range)
        {
            return false;
        }
        //AがBを押し出す
        vx /= distXZ;
        vz /= distXZ;
        outPositionB.x = positionA.x + (vx * range);
        outPositionB.y = positionB.y;
        outPositionB.z = positionA.z + (vz * range);

        return true;
    }




    // 球と円柱の交差判定
   bool Collision::IntersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius,
        float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition)
    {
        //    
            // 高さチェック
    if (spherePosition.y + sphereRadius < cylinderPosition.y)return false;
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)return false;

        // XZ平面での範囲チェック
        float vx = cylinderPosition.x - spherePosition.x;
        float vz = cylinderPosition.z - spherePosition.z;
        float range = sphereRadius + cylinderRadius;
        float distXZ = sqrtf(vx * vx + vz * vz);
        if (distXZ > range)return false;

        // 弾が円柱を押し出す
        vx /= distXZ;
        vz /= distXZ;
        outCylinderPosition.x = spherePosition.x + (vx * range);
        outCylinderPosition.x = cylinderPosition.y;
        outCylinderPosition.x = spherePosition.z + (vz * range);


        // 円柱の底面を含む平面と球の交差にとって出来ル。
        // 円の半径の２乗が得られます。
        float Radius;

        Radius = sqrtf(sphereRadius * sphereRadius);

        /*Radius = Radius - Radius;*/

        if (sphereRadius < Radius &&
            cylinderRadius < Radius)
        {
            return true;
         }

        return true;
    }
   // レイとモデルの交差判定
   bool Collision::IntersectRayVsModel(
       // レイの発射点
       const DirectX::XMFLOAT3& start,
       // レイの終着点重いから本来は無限だが有限に
       const DirectX::XMFLOAT3& end,
       const Model* model,
       // レイが当たったポイントを取ったり他の次いでの情報を取る
       HitResult& result)
   {
       //// 以前の処理が正しく動くように仮の実装
       //if (end.y < 0.0f)
       //{
       //    result.position.x = end.x;
       //    result.position.y = 0.0f;
       //    result.position.z = end.z;
       //    result.normal.x = 0.0f;
       //    result.normal.y = 1.0f;
       //    result.normal.z = 0.0f;
       //    return true;
       //}
       DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
       DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
        //　光線ベクトル　レイ
       DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
       // レイ長さ
       DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
       
       // ワールド空間のレイの長さ
       DirectX::XMStoreFloat(&result.distance, WorldRayLength);

       bool hit = false;
       // メッシュの情報
       const ModelResource* resource = model->GetResource();
       for (const ModelResource::Mesh& mesh : resource->GetMeshes())
       {
           // メッシュノード取得
           const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

           

           // レイをワールド空間からローカル空間へ変換
           DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
           DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
           // 逆行列を使えば楽に行ける
           DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
           DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
           DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E,S);
           DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
           DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

           // レイの長さ
           float neart;
           DirectX::XMStoreFloat(&neart, Length);

           // 三角形(面)との交差判定
           const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
           /*const std::vector<UINT> indices = mesh.indices;*/
           const std::vector<UINT> indices = mesh.indices;

           int materialIndex = -1;
           DirectX::XMVECTOR HitPosition;
           DirectX::XMVECTOR HitNormal;
           for (const ModelResource::Subset& subset : mesh.subsets)
           {
               for (UINT i = 0; i < subset.indexCount; i += 3)
               {
                   UINT index = subset.startIndex + i;
                   // 複数メッシュの中にある複数サブセットの中に複数の頂点があるから取り出す。

                   // 三角形の頂点を抽出
                   const ModelResource::Vertex& a = vertices.at(indices.at(index));
                   const ModelResource::Vertex& b = vertices.at(indices.at((index + 1)%UINT_MAX));
                   const ModelResource::Vertex& c = vertices.at(indices.at((index + 2)%UINT_MAX));

                   DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                   DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                   DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                   // 三角形の三辺ベクトルを算出　時計回り
                   DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                   DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                   DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                   // 三角形の法線ベクトルを算出 垂直なベクトル
                   DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

                   // 正規化しないとバグる
                   N = DirectX::XMVector3Normalize(N);

                   // 無限平面との交点を取るここから

                   // 内積の結果がプラスならば表向き
                   DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
                   //float dot = DirectX::XMVectorGetX(Dot);
                   float dot;
                   DirectX::XMStoreFloat(&dot, Dot);
                   //if (dot > (1e-6f))
                   //    //result.position.y = 0.0f;
                   //    hit = true;
                   if (dot >= 0)continue;

                   // レイと平面の交点を算出
                   //DirectX::XMVECTOR SA = DirectX::XMVector3Dot((DirectX::XMVectorSubtract(A, S)), DirectX::XMVector2Normalize(N));
                   //DirectX::XMVECTOR X = Dot;

                   DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
                   DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(N,SA),Dot);



                   //DirectX::XMStoreFloat(&x, X);
                   //float x = (DirectX::XMVectorGetX(SA)) / dot;
                   //float x = DirectX::XMVectorGetX(SA)/DirectX::XMVectorGetX(X);
                   float x;
                       DirectX::XMStoreFloat(&x, X);

                   // neartとは複数頂点があるとき最も近いもの以外首切る。
                   if (x < .0f || x > neart) continue; // 交点までの距離が今までに計算した最近距離より
                                                       // 大きい時はスキップ
                   //DirectX::XMVECTOR P = DirectX::XMVectorScale(S,x);
                   //DirectX::XMVECTOR P = DirectX::XMVectorScale(DirectX::XMVectorAdd(S, V), x);
                   DirectX::XMVECTOR P = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(V,X),S);

                   // 交点が三角形の内側にあるか判定
                   DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
                   DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA,AB);
                   DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
                 /*  if (90 > DirectX::XMVectorGetX(Dot1)) continue;*/
                   float dot1;
                   DirectX::XMStoreFloat(&dot1, Dot1);
                   if (dot1 < 0.0f)continue;


                   // 2つめ
                   DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
                   DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
                   DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
                   float dot2;
                   DirectX::XMStoreFloat(&dot2, Dot2);
                   if (dot2 < 0.0f)continue;
                   // 3つめ
                   DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
                   DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
                   DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
                   float dot3;
                   DirectX::XMStoreFloat(&dot3, Dot3);
                   if (dot3 < 0.0f)continue;

                   // 最近距離を更新
                   neart = x;

                   // 最も近い物を保存

                   // 交点と法線を更新
                   HitPosition = P;
                   HitNormal = N;
                   // マテリアルインデックスは最初ー１になっているメッシュ単位で一個代表を見つけた
                   materialIndex = subset.materialIndex;

               }
           }
           if (materialIndex >= 0)
           {
               // ローカル空間からワールド空間へ変換
               // 位置
               DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition,
                   WorldTransform);
               // ここで法線を保存してもいいが別に保存しない可能性がないのでやらなくていい
               DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
               DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
               float distance;
               DirectX::XMStoreFloat(&distance, WorldCrossLength);
               // 暫定チャンピオンと今の距離をやると近い物を入れる。
               // ヒット情報保存
               if (result.distance > distance)
               {
                   // 法線
                   DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal ,
                       WorldTransform);
                   result.distance = distance;
                   result.materialIndex = materialIndex;
                   DirectX::XMStoreFloat3(&result.position, WorldPosition);
                   DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
                   hit = true;
               }
           }

       }
       
       //後で実装
       return hit;

//return false;
   }
