#pragma once
#include "Bases.hpp"

class Arrow : public GameModel {
public:
	Arrow(Vec3 pos = Vec3(D_Up * 3), Vec3 front = Vec3(::D_Front), Vec3 up = Vec3(::D_Up),float len= 2);

	void setupTransform();

	void reset();

	void spin(float angle = PI / 45);

	void rotateVertical(float angle = PI / 180);

	void rotateHorizontal(float angle = PI / 180);

	void rotateFrontTo(Vec3 front);

	void translateBy(Vec3 delR);

	void translateByVel();

	void rotateToVel();

	Vec3 getFront() const;
	
	Vec3 getUp() const;

	Vec3 getCenter() const;

	Vec3 getHead() const;

	Vec3 getTail() const;

	State getState() const;

	void draw();

	Vec4 mCol = DARKBLUE;
	Vec3 velocity = D_Front * 0.5;
private:

	ModelGen arrowBody;
	ModelGen arrowHead;
	ModelGen arrowModel;

	raylib::Texture arrTexture;

	State currState;
	const State defaultState;
};