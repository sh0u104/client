#include "PlayerManager.h"



void PlayerManager::Update(float elapsedTime)
{
	reduceMessageEraseTime(elapsedTime);
	if (GetmessageEraseTime() < 0&&!messages.empty())
	{
		messages.erase(messages.begin());
		SetmessageEraseTime(5.0f);
	}

	for (Player* player : players)
	{
		if(player->GetReady())
		//if (player != nullptr)
		//if (!GetLogging_in())
		player->Update(elapsedTime);
	}
}

void PlayerManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (Player* player : players)
	{
		//if (player != nullptr)
		if (player->GetReady())
		//if (!GetLogging_in())
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
void PlayerManager::ErasePlayer(int id)
{
	int i = 0;
	for (Player* player : players)
	{
		if (player->GetPlayerID() == id)
		{
			player->SetReady(false);
			deleteList.emplace_back(player);
			break;
		}
		++i;
	}
}

void PlayerManager::DeletePlayer()
{
	for (Player* player : deleteList)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<Player*>::iterator it = std::find(players.begin(), players.end(), player);
		if (it != players.end())
		{
			players.erase(it);
		}
		delete player;
	}
	deleteList.clear();
}