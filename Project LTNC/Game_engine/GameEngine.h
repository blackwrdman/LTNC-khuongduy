#pragma once

#include "../CommonClass/Map.h"
#include "../CommonClass/Clock.h"
#include "Player.h"
#include "KeyPress.h"
#include "../AI/SimpleAI.h"
#include "../FileLoad/Audio.h"

enum STATUS {
	RUNNING,
	PAUSE,
	RESULT,
	RESTART,
	RETURN,
	EXIT,
};

class GameEngine {
private:
	Map* map;
	int width, height;
	int status;

	Player* p1;
	Player* p2;
	Player* winner;
	int gameMode;

	Coordinate mousePosition;

	Audio sound;

	Clock* clock1;
	Clock* clock2;

public:
	GameEngine(int _width, int _height, int _gameMode);
	~GameEngine();

	void Step(int _key);
	void PlayerTurn(Player* _player, Player* _other);

	Player* Player1() { return p1; }
	Player* Player2() { return p2; }
	Player* GetWinner() { return winner; }
	void SetWinner(Player* player) { winner = player; }

	int GetGameMode() { return gameMode; }
	Map* GetMap() { return map; }
	int GetStatus() { return status; }
	void SetStatus(int _status) { status = _status; }
	void SetMousePos(int _x, int _y) {
		mousePosition.x = _x;
		mousePosition.y = _y;
	}

	bool ValidPos(int _x, int _y);
	bool CheckWin(Coordinate _lastPos);
	bool CheckHave5Length(int _arr[]);

	void Restart();

	Clock* Clock1() { return clock1; }
	Clock* Clock2() { return clock2; }

};