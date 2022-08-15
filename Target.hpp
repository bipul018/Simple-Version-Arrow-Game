#pragma once
#include "Bases.hpp"
#include <array>

class Target :public GameModel {
public:
	Target(Vec3 normal = Vec3(D_Front*-1), Vec3 center = Vec3(D_Front * 50 + D_Up * 50),float radius = 30);
	void draw();
	void setupTransform();
	State getState() const;
	Vec3 getNormal() const;
	void translateBy(Vec3 delR);
	void reset();
	bool doesCross(Vec3 head) const;
	Vec3 getProj(Vec3 vector,Vec3 point) const;
	unsigned getCollisionRing(Vec3 vector, Vec3 point) const;
private:
	std::array<ModelGen, 5> models;
	std::array<Vec4, 5>colors;
	State currState;
	const State defaultState;

};