#include "Clock.h"

Clock::Clock() {

}

Clock::~Clock() {

}

sf::Time Clock::Restart() {
	return this->clock.restart();
}

sf::Time Clock::GetTime() {
	return this->actualTime;
}

void Clock::UpdateTime(sf::Time dt) {
	this->actualTime += dt;
}

void Clock::RestartTime() {
	this->actualTime = sf::seconds(0);
}