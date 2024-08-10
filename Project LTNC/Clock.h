#pragma once
#include <SFML/System/Clock.hpp>

class Clock {
private:
	sf::Clock clock;
public:
	Clock();
	~Clock();
	void restart();
	sf::Time getTime();
};