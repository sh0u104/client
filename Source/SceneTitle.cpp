#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"

#include "SceneStandby.h"
#include "SceneConnection.h"
#include "Graphics/SpriteManager.h"
// ������
void SceneTitle::Initialize()
{
    g_SpriteManager.LoadSprites();
    // �X�v���C�g������
   // sprite = new Sprite("Data/Sprite/Title.png");
    SceneManager::Instance().ConnectionClear();
}
// �I����
void SceneTitle::Finalize()
{
    // �X�v���C�g�I����
    //if (this->sprite)
    //{
    //    delete sprite;
    //    sprite = nullptr;
    //}

}
// �X�V����
void SceneTitle::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Graphics& graphics = Graphics::Instance();

    //�}�E�X�̍��N���b�N
    Mouse& mouse = Input::Instance().GetMouse();
    if (mouse.GetPositionX() > graphics.GetScreenWidth() || mouse.GetPositionX() < 0 ||
        mouse.GetPositionY() > graphics.GetScreenHeight() || mouse.GetPositionY() < 0)
        return;
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneConnection));
    }

}
// �`�揈��
void SceneTitle::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };   // RGBA(0.0~1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // 2D�X�v���C�g�`��
    {
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());

        Sprite* titleSprite = g_SpriteManager.GetSprite(SpriteNumber::Title);
 
        float textureWidth = static_cast<float>(titleSprite->GetTextureWidth());
        float textureHeight = static_cast<float>(titleSprite->GetTextureHeight());
        // �`��
        titleSprite->Render(dc,
            0, 0, screenWidth, screenHeight,
            0, 0, textureWidth, textureHeight,
            0,
            1, 1, 1, 1);
        // {�ʒu}{�T�C�Y}{�摜�ǂ�����}{�摜�����܂�}
        // dc , �o�͈́p�o�p
    }

  
}

