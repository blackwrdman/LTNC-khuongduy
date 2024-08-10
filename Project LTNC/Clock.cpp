#include "Clock.h"

Clock::Clock() {

}

Clock::~Clock() {

}

void Clock::restart() {
	this->clock.restart();
}

sf::Time Clock::getTime() {
	return this->clock.getElapsedTime();
}