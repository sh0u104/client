#include "PlayerManager.h"
#include "Logger.h"


void PlayerManager::Update(float elapsedTime)
{
	//�`���b�g�p
	{
		reduceMessageEraseTime(elapsedTime);
		if (GetmessageEraseTime() < 0 && !messages.empty())
		{
			messages.erase(messages.begin());
			SetmessageEraseTime(5.0f);
		}
	}

	for (Player* player : players)
	{
		if (player != nullptr)
		if(player->GetReady())
		player->Update(elapsedTime);
	}
}

void PlayerManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (Player* player : players)
	{
		if (player != nullptr)
		if (player->GetReady())
		player->Render(dc, shader);
	}
}

void PlayerManager::DebugGUI()
{
}


Player* PlayerManager::GetPlayer(int id)
{
	for (Player* player : players)
	{
		if (player->GetPlayerID() == id)
		{
			return player;
		}
	}
	return nullptr;
}

Player* PlayerManager::GetMyPlayer()
{
	for (Player* player : players)
	{
		if (player->GetPlayerID() == myPlayerId)
		{
			return player;
		}
	}
	return nullptr;
}

//�������X�g�ɒǉ�
void PlayerManager::ErasePlayer(int id)
{
	
	for (Player* player : players)
	{
		if (player->GetPlayerID() == id)
		{
			player->SetReady(false);
			deleteList.emplace_back(player);
			break;
		}
	}
}

//Update�ŌĂ�
void PlayerManager::DeletePlayer()
{
	//�������X�g�̒��g�̕�������
	for (Player* player : deleteList)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<Player*>::iterator it = std::find(players.begin(), players.end(), player);
		if (it != players.end())
		{
			players.erase(it);
			SubtractLoginCount();
			SubtractPlayersGenerateCount();
			Logger::Print("�v���C���[������");
		}
		delete player;
	}
	deleteList.clear();
}