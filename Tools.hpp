#pragma once
#include "Bases.hpp"

class MyMouse :public raylib::Mouse {

public:
	using Mouse::Mouse;
	MyMouse& update();
	Vec2 getDelta();
	void limitMouse();
	void unlimitMouse();
private:
	Vec2 prevPos;
	bool limitedCoor = true;
};

class MyCamera :public raylib::Camera3D{
public:
	using Camera3D::Camera3D;

	void setLookDir(Vec3 look);

	Vec3 getLookDir() const;

	MyCamera& Update();

	void SetMouse(MyMouse* mptr);

private:
	MyMouse* mouseptr = nullptr;

	//In radians
	Vec2 maxAngles = Vec2(PI / 3, PI / 1.99);
	Vec2 screenAngles = Vec2(PI / 4, PI / 2);
	Vec2 currAngles = Vec2(0, 0);

};
