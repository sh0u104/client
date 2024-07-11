#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"

// ������
void SceneLoading::Initialize()
{
    // �X�v���C�g������
    sprite = new Sprite("Data/Sprite/LoadingIcon.png");

    // �X���b�h�J�n      (�֐����A�����̒��g �������M)�X���b�h�����グ��
    // ���������ł��������B�֐��̈����̒����ɂ��
    this->thread = new std::thread(LoadingThread,this);
}

// �I����
void SceneLoading::Finalize()
{
    // �X���b�h�I����
    if (this->thread)
    {
        // �������Ă���Œ��Ƀf���[�g����Ƃ܂���������܂�
        thread->join();
        delete thread;
        //delete thread;
        thread = nullptr;
    }
    
    // �X�v���C�g�I����
    if (this->sprite)
    {
        delete sprite;
        sprite = nullptr;
    }
}
// �X�V����
void SceneLoading::Update(float elapsedTime)
{
    // ��]����
    constexpr float speed = 180;
    angle += speed * elapsedTime;

    // ���̃V�[���̏���������������V�[����؂�ւ���
    //if (IsReady())
    //{

    //}
        // ���̃V�[���̏���������������V�[����؂�ւ���
    if (nextScene->IsReady())
    {
        SceneManager::Instance().ChangeScene(nextScene);
        nextScene = nullptr;
    }


}
// �`�揈��
void SceneLoading::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0�`1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // �QD�X�v���C�g�`��
    {
        // ��ʉE���Ƀ��[�f�B���O�A�C�R����`��
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());
        float textureWidth = static_cast<float>(sprite->GetTextureWidth());
        float textureHeight = static_cast<float>(sprite->GetTextureHeight());
        float positionX = screenWidth - textureWidth;
        float positionY = screenHeight - textureHeight;

        sprite->Render(dc,
            positionX, positionY, textureWidth, textureHeight,
            0, 0, textureWidth, textureHeight,
            angle,
            1, 1, 1, 1);
    }
}


// ���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
    // COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
    // �T�E���h�A�e�N�X�`�����̏�����COM���g���Ă���Ă���B
    // �X���b�h���Ƃ̏��������Ȃ��Ƃ����Ȃ�
    // �������Ă�
    CoInitialize(nullptr);

    // ���̃V�[���̏��������s��
    //scene->Initialize();
    scene->nextScene->Initialize();


    // �X���b�h���I���O��COM�֘A�̏I����
    // �K�v�Ȃ��Ȃ�I����
    CoUninitialize();

    // ���̃V�[���̏��������ݒ�
    scene->nextScene->SetReady();
}
