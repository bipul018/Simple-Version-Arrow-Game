#pragma once
#include "Bases.hpp"

class Arrow : public GameModel {
public:
	Arrow();
	
	void setupTransform();

	void draw();

	Vec4 mCol = BLUE;
private:

	ModelGen arrowModel;
	State currState;
	const State defaultState;
	Vec3 velocity = front;
};