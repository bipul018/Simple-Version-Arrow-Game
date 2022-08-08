#pragma once
#include "Bases.hpp"

class Obstacle :public GameModel {

public:
	Obstacle();

	void setupTransform();

	void draw();

	Vec4 mCol = DARKGREEN;
private:

	ModelGen obsModel;
	State currState;
	const State defaultState;

};