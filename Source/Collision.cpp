#include "Collision.h"

// ���Ƌ��̌�������
    bool Collision::IntersectSpherVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB
)
{
    // A->B�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec =  DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);// ��O������ 2��̂܂�
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);
    
    // lengthsq����͂Q��
    // ��������
    float range = radiusA+radiusB;
    if (lengthSq > range*range)// �����̔�r�@2�擯�m�Ȃ�s����
    {
        // ����Ă��邩�� ��΂�
        return false;
    }

    // A��B�������o��
       // �x�N�g��������Ē�����}��
       // �܂��������P��
    Vec = DirectX::XMVector3Normalize(Vec);
    // ����lengthSq�������~
    Vec = DirectX::XMVectorScale(Vec, range);
    // a�̃|�W�V����������𑫂��Ƃ�������L�т錋�ʂ����ɓ����
    PositionB = DirectX::XMVectorAdd(PositionA, Vec);
    // �����outPositionB�ɓ����
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
        // A�̑�����B�̓���肤���Ȃ瓖�����Ă��Ȃ�
        if (positionA.y > heightB + positionB.y)
        {
            return false;
        }
        // A�̓���B�̑�����肵���Ȃ瓖�����Ă��Ȃ�
        if (positionA.y +heightA< positionB.y)
        {
            return false;
        }

        // XZ���ʂł͈̔̓`�F�b�N

        float vx = positionB.x - positionA.x;//B-A�ł�a->B
        float vz = positionB.z - positionA.z;// ���P�ʃx�N�g��
        float range = radiusA + radiusB;// �ǂꂾ�������o��
        float distXZ = sqrtf(vx * vx + vz * vz); // 
        if (distXZ > range)
        {
            return false;
        }
        //A��B�������o��
        vx /= distXZ;
        vz /= distXZ;
        outPositionB.x = positionA.x + (vx * range);
        outPositionB.y = positionB.y;
        outPositionB.z = positionA.z + (vz * range);

        return true;
    }




    // ���Ɖ~���̌�������
   bool Collision::IntersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius,
        float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition)
    {
        //    
            // �����`�F�b�N
    if (spherePosition.y + sphereRadius < cylinderPosition.y)return false;
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)return false;

        // XZ���ʂł͈̔̓`�F�b�N
        float vx = cylinderPosition.x - spherePosition.x;
        float vz = cylinderPosition.z - spherePosition.z;
        float range = sphereRadius + cylinderRadius;
        float distXZ = sqrtf(vx * vx + vz * vz);
        if (distXZ > range)return false;

        // �e���~���������o��
        vx /= distXZ;
        vz /= distXZ;
        outCylinderPosition.x = spherePosition.x + (vx * range);
        outCylinderPosition.x = cylinderPosition.y;
        outCylinderPosition.x = spherePosition.z + (vz * range);


        // �~���̒�ʂ��܂ޕ��ʂƋ��̌����ɂƂ��ďo�����B
        // �~�̔��a�̂Q�悪�����܂��B
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
   // ���C�ƃ��f���̌�������
   bool Collision::IntersectRayVsModel(
       // ���C�̔��˓_
       const DirectX::XMFLOAT3& start,
       // ���C�̏I���_�d������{���͖��������L����
       const DirectX::XMFLOAT3& end,
       const Model* model,
       // ���C�����������|�C���g��������葼�̎����ł̏������
       HitResult& result)
   {
       //// �ȑO�̏����������������悤�ɉ��̎���
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
        //�@�����x�N�g���@���C
       DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
       // ���C����
       DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
       
       // ���[���h��Ԃ̃��C�̒���
       DirectX::XMStoreFloat(&result.distance, WorldRayLength);

       bool hit = false;
       // ���b�V���̏��
       const ModelResource* resource = model->GetResource();
       for (const ModelResource::Mesh& mesh : resource->GetMeshes())
       {
           // ���b�V���m�[�h�擾
           const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

           

           // ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
           DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
           DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
           // �t�s����g���Ίy�ɍs����
           DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
           DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
           DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E,S);
           DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
           DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

           // ���C�̒���
           float neart;
           DirectX::XMStoreFloat(&neart, Length);

           // �O�p�`(��)�Ƃ̌�������
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
                   // �������b�V���̒��ɂ��镡���T�u�Z�b�g�̒��ɕ����̒��_�����邩����o���B

                   // �O�p�`�̒��_�𒊏o
                   const ModelResource::Vertex& a = vertices.at(indices.at(index));
                   const ModelResource::Vertex& b = vertices.at(indices.at((index + 1)%UINT_MAX));
                   const ModelResource::Vertex& c = vertices.at(indices.at((index + 2)%UINT_MAX));

                   DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                   DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                   DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                   // �O�p�`�̎O�Ӄx�N�g�����Z�o�@���v���
                   DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                   DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                   DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                   // �O�p�`�̖@���x�N�g�����Z�o �����ȃx�N�g��
                   DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

                   // ���K�����Ȃ��ƃo�O��
                   N = DirectX::XMVector3Normalize(N);

                   // �������ʂƂ̌�_����邱������

                   // ���ς̌��ʂ��v���X�Ȃ�Ε\����
                   DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
                   //float dot = DirectX::XMVectorGetX(Dot);
                   float dot;
                   DirectX::XMStoreFloat(&dot, Dot);
                   //if (dot > (1e-6f))
                   //    //result.position.y = 0.0f;
                   //    hit = true;
                   if (dot >= 0)continue;

                   // ���C�ƕ��ʂ̌�_���Z�o
                   //DirectX::XMVECTOR SA = DirectX::XMVector3Dot((DirectX::XMVectorSubtract(A, S)), DirectX::XMVector2Normalize(N));
                   //DirectX::XMVECTOR X = Dot;

                   DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
                   DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(N,SA),Dot);



                   //DirectX::XMStoreFloat(&x, X);
                   //float x = (DirectX::XMVectorGetX(SA)) / dot;
                   //float x = DirectX::XMVectorGetX(SA)/DirectX::XMVectorGetX(X);
                   float x;
                       DirectX::XMStoreFloat(&x, X);

                   // neart�Ƃ͕������_������Ƃ��ł��߂����̈ȊO��؂�B
                   if (x < .0f || x > neart) continue; // ��_�܂ł̋��������܂łɌv�Z�����ŋߋ������
                                                       // �傫�����̓X�L�b�v
                   //DirectX::XMVECTOR P = DirectX::XMVectorScale(S,x);
                   //DirectX::XMVECTOR P = DirectX::XMVectorScale(DirectX::XMVectorAdd(S, V), x);
                   DirectX::XMVECTOR P = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(V,X),S);

                   // ��_���O�p�`�̓����ɂ��邩����
                   DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
                   DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA,AB);
                   DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
                 /*  if (90 > DirectX::XMVectorGetX(Dot1)) continue;*/
                   float dot1;
                   DirectX::XMStoreFloat(&dot1, Dot1);
                   if (dot1 < 0.0f)continue;


                   // 2��
                   DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
                   DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
                   DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
                   float dot2;
                   DirectX::XMStoreFloat(&dot2, Dot2);
                   if (dot2 < 0.0f)continue;
                   // 3��
                   DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
                   DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
                   DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
                   float dot3;
                   DirectX::XMStoreFloat(&dot3, Dot3);
                   if (dot3 < 0.0f)continue;

                   // �ŋߋ������X�V
                   neart = x;

                   // �ł��߂�����ۑ�

                   // ��_�Ɩ@�����X�V
                   HitPosition = P;
                   HitNormal = N;
                   // �}�e���A���C���f�b�N�X�͍ŏ��[�P�ɂȂ��Ă��郁�b�V���P�ʂň��\��������
                   materialIndex = subset.materialIndex;

               }
           }
           if (materialIndex >= 0)
           {
               // ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
               // �ʒu
               DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition,
                   WorldTransform);
               // �����Ŗ@����ۑ����Ă��������ʂɕۑ����Ȃ��\�����Ȃ��̂ł��Ȃ��Ă���
               DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
               DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
               float distance;
               DirectX::XMStoreFloat(&distance, WorldCrossLength);
               // �b��`�����s�I���ƍ��̋��������Ƌ߂���������B
               // �q�b�g���ۑ�
               if (result.distance > distance)
               {
                   // �@��
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
       
       //��Ŏ���
       return hit;

//return false;
   }
