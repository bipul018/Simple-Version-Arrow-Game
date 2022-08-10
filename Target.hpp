#pragma once
#include "Bases.hpp"
#include <array>

class Target :public GameModel {
public:
	Target(Vec3 normal = Vec3(D_Front*-1), Vec3 center = Vec3(D_Front * 50 + D_Up * 50),float radius = 30);
	void draw();
	void setupTransform();
private:
	std::array<ModelGen, 3> models;
	std::array<Vec4, 3>colors;
	State currState;
	const State defaultState;

};