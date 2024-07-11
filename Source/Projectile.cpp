#include "Projectile.h"
#include "ProjectileManager.h"
#include "Graphics/Graphics.h"
Projectile::Projectile(ProjectileManager* manager)
    :manager(manager)
{
    // ì‚Á‚½“z‚ð“o˜^
    manager->Register(this);
}

void Projectile::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // ¡‚Í‰½‚à•\Ž¦‚µ‚È‚¢
        //// Õ“Ë”»’è—p‚ÌƒfƒoƒbƒO‹…‚ð•`‰æ
    debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

}




void Projectile::UpdateTransform()
{
    // ‚Æ‚è‚ ‚¦‚¸A‰¼‚Å‰ñ“]‚Í–³Ž‹‚µ‚½s—ñ‚ðì¬‚·‚éB
    //transform._11 = scale.x; // ‘å‚«‚³‚¾‚¯‚â‚Á‚½“z‚OD‚P”{‚È‚ç‚»‚¤‚È‚é
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

    // ‰½‚©
    DirectX::XMVECTOR Front, Up, Right;
    // ‘OƒxƒNƒgƒ‹‚ðŽZo@‚Ü‚¸Œü‚«‚ðŽè‚É“ü‚ê‚é
    Front = DirectX::XMLoadFloat3(&direction);
    Front = DirectX::XMVector3Normalize(Front);
    // ‰¼‚ÌãƒxƒNƒgƒ‹‚ðŽZo@­‚µ‚¨‚©‚µ‚¢’l‚ð“ü‚ê‚Ä‚¨‚­^ã‚É‘Å‚Á‚½Žž‚É
    // ‚±‚±‚Æ“¯‚¶’l‚¾‚ÆƒoƒO‚é‚©‚ç
    Up = DirectX::XMVectorSet(0.001f, 1, 0, 0);
    Up = DirectX::XMVector3Normalize(Up);

    // ‰EƒxƒNƒgƒ‹ŽZo@ã“ñ‚Â‚ðŽg‚¢ˆê‚Â‚ÌV‚µ‚¢Ž²‚ðì‚é
    Right = DirectX::XMVector3Cross(Up, Front);// ‚Q‚Â‚Å‚P‚Â‚Ì“z‚ðo‚·

    Right = DirectX::XMVector3Normalize(Right);
    // ãƒxƒNƒgƒ‹ŽZo ‚³‚ç‚Éì‚Á‚½“ñ‚Â‚Å‚³‚ç‚ÉV‚µ‚¢‚Ð‚Æ‚Â‚ðì‚éB
    Up = DirectX::XMVector3Cross(Front, Right);// ŽÎ‚ßã‚Ì“zo—ˆ‚é



    // ŒvŽZŒ‹‰Ê‚ðŽæ‚èo‚µ
    DirectX::XMFLOAT3 right, up, front;
    DirectX::XMStoreFloat3(&right, Right);
    DirectX::XMStoreFloat3(&up, Up);
    DirectX::XMStoreFloat3(&front, Front);

    // ŽZo‚µ‚½Ž²ƒxƒNƒgƒ‹‚©‚çs—ñ‚ðì¬ ‰ñ“]‚ ‚è
    transform._11 = right.x * scale.x; // ‘å‚«‚³‚¾‚¯‚â‚Á‚½“z‚OD‚P”{‚È‚ç‚»‚¤‚È‚é
    transform._12 = right.y * scale.x;// Šp“x
    transform._13 = right.z * scale.x;
    transform._14 = 0.0f;
    transform._21 = up.x * scale.y;
    transform._22 = up.y * scale.y;// — •Ô‚é
    transform._23 = up.z * scale.y;
    transform._24 = 0.0f;
    transform._31 = front.x* scale.z;
    transform._32 = front.y * scale.z;
    transform._33 = front.z * scale.z;// Œü‚«
    transform._34 = 0.0f;
    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;

    this->direction = front;// Œü‚«‚ð‚±‚Á‚¿‚É“ü‚ê‚é‚±‚ê‚Å‚Þ‚«‚ÌŒvŽZ
    // ‚ðŠO‚É
}

void Projectile::Destroy()
{
    // ’eŠÛ‚©‚çŽ©•ª‚ðíœ‚Â‚Ü‚èƒ}ƒl[ƒWƒƒ[‚©‚ç@ ŠÇ—‚µ‚Ä‚¢‚é“z‚ð‚©‚ç‚àÁ‚·
    manager->Remove(this);
}
