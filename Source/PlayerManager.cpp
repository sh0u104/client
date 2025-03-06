#include "PlayerManager.h"
#include "Logger.h"


void PlayerManager::Update(float elapsedTime)
{
	//チャット用
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

//消去リストに追加
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

//Updateで呼ぶ
void PlayerManager::DeletePlayer()
{
	//消去リストの中身の物を消す
	for (Player* player : deleteList)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Player*>::iterator it = std::find(players.begin(), players.end(), player);
		if (it != players.end())
		{
			players.erase(it);
			SubtractLoginCount();
			SubtractPlayersGenerateCount();
			Logger::Print("プレイヤーを消去");
		}
		delete player;
	}
	deleteList.clear();
}