#pragma once
#include "Bases.hpp"

class MyMouse :public raylib::Mouse {

public:
	using Mouse::Mouse;
	Vec2 getDelta();
	MyMouse& limitInScreen();
	MyMouse& hideCursor();
	MyMouse& showCursor();
	MyMouse& disableCursor();
	MyMouse& enableCursor();
private:
};

class MyCamera :public raylib::Camera3D{
public:
	using Camera3D::Camera3D;

	void setLookDir(Vec3 look);

	Vec3 getLookDir() const;

	MyCamera& Update();

	void SetMouse(MyMouse* mptr);

//private:
	MyMouse* mouseptr = nullptr;

	//In radians
	Vec2 maxAngles = Vec2(PI / 3, PI / 2.02);
	Vec2 screenAngles = Vec2(PI / 4, PI / 2);
	Vec2 currAngles = Vec2(0, 0);

};
