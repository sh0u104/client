#include "SceneConnection.h"
#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneStandby.h"

// ������
void SceneConnection::Initialize()
{
       
       //�l�b�g���[�N�ڑ����s
     // �X�v���C�g������
        sprites[static_cast<int>(Spritenumber::NetError)] = std::make_unique<Sprite>("Data/Sprite/neterror.png");
        //OK�{�^��
        sprites[static_cast<int>(Spritenumber::OK)] = std::make_unique<Sprite>("Data/Sprite/OK.png");


        //��x����
        if (!SceneManager::Instance().GetconnectionInitialized())
        {
            SceneManager::Instance().SetconnectionInitialized(true);
            //������
            SceneManager::Instance().PlayermanagerInitialize();
            SceneManager::Instance().ConnectionInitialize();
        }
       
        // �v���C���[�}�l�W���[������
        playerManager = SceneManager::Instance().GetPlayerManager();
    
        connection = SceneManager::Instance().GetConnection();
       
    //�ڑ�����
    if (!connection->isConnction)
    {
        connection->Initialize();
    }


}
// �I����
void SceneConnection::Finalize()
{
  
}
// �X�V����
void SceneConnection::Update(float elapsedTime)
{
    //�ڑ�����
    if (connection->isConnction)
    {
        // connection->SetplayerManager(playerManager);
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
    }
}
// �`�揈��
void SceneConnection::Render()
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
    if (!connection->isConnction)
    {
        RenderNetError(dc);
    }


}

void SceneConnection::RenderNetError(ID3D11DeviceContext* dc)
{
    float positionX = 50;
    float positionY = 0;

    //�g�g��
    sprites[static_cast<int>(Spritenumber::NetError)]->Render(dc,
        positionX, positionY, //�`��ʒu
        600, 350,               //�\���T�C�Y
        0, 0,                 //�؂���͂��߈ʒu
        750, 500,           //�摜�T�C�Y
        0.0f,
        1, 1, 1, 1);

    //OK�{�^��
    sprites[static_cast<int>(Spritenumber::OK)]->Render(dc,
        positionX + 190, positionY + 250, //�`��ʒu
        150, 50,               //�\���T�C�Y
        0, 0,                 //�؂���͂��߈ʒu
        400, 200,           //�摜�T�C�Y
        0.0f,
        1, 1, 1, 1);

    if (Uiclick(positionX + 190, positionY + 250, 150, 50))
    {
       
       if (!connection->isConnction)
       {
           connection->Initialize();
           //�ڑ�����
           if (connection->isConnction)
           {
              // connection->SetplayerManager(playerManager);
               SceneManager::Instance().ChangeScene(new SceneLoading(new SceneStandby));
           }
           //���s
           else
           {
               SceneManager::Instance().ChangeScene(new SceneLoading(new SceneConnection));
           }
       }
       
    }
}

bool SceneConnection::Uiclick(float posX, float posY, float sizeX, float sizeY)
{
    DirectX::XMFLOAT3 scereenPosition;

    Mouse& mouse = Input::Instance().GetMouse();
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        scereenPosition.x = static_cast<float>(mouse.GetPositionX());
        scereenPosition.y = static_cast<float>(mouse.GetPositionY());

        if (posX < scereenPosition.x && scereenPosition.x < posX + sizeX
            && posY < scereenPosition.y && scereenPosition.y < posY + sizeY)
        {
            Sleep(10);
            return true;
        }
    }
    return false;
}