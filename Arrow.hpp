#pragma once
#include "Bases.hpp"

class Arrow : public GameModel {
public:
	Arrow(Vec3 pos = Vec3(D_Up * 3), Vec3 front = Vec3(::D_Front), Vec3 up = Vec3(::D_Up),float len= 2);
	
	void spin(float angle = PI / 180);

	void setupTransform();

	void resetArrow();

	void translateBy(Vec3 delR);

	void translateByVel();

	void rotateToVel();

	void draw();

	Vec4 mCol = DARKBLUE;
	Vec3 velocity = D_Front * 0.5;
private:

	ModelGen arrowBody;
	ModelGen arrowHead;
	State currState;
	const State defaultState;
};