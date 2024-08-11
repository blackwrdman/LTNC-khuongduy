#pragma once
#include <SFML/System/Clock.hpp>

class Clock {
private:
	sf::Clock clock;
	sf::Time actualTime;
public:
	Clock();
	~Clock();
	sf::Time Restart();
	sf::Time GetTime();
	void UpdateTime(sf::Time dt);
};