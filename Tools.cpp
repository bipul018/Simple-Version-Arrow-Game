#include "Tools.hpp"

void MyCamera::setLookDir(Vec3 look) {
	target = look + position;
}

Vec3 MyCamera::getLookDir() const {
	return Vec3(target) - position;
}

MyCamera& MyCamera::Update() {
	if (mouseptr == nullptr)
		return *this;
	//Needs serious refactoring of code, I mean dead serious, 
	//This part of code is directly copied from random trials in random projects
	//You really should take care of the static variable you created here, that is the absolute worst
	
	Vec2 delMouse = mouseptr->getDelta();
	
	
	Vec2 delAngle = screenAngles * delMouse / Vec2(screenWidth, screenHeight);
	if(maxAngles.x != 0)
		if ((delAngle.x + currAngles.x) < maxAngles.x) {
			if ((delAngle.x + currAngles.x) > -maxAngles.x)
				currAngles.x += delAngle.x;
		}
	if(maxAngles.y != 0)
		if ((delAngle.y + currAngles.y) < maxAngles.y) {
			if ((delAngle.y + currAngles.y) > -maxAngles.y)
				currAngles.y += delAngle.y;
		}
	up = Vec3(up).Normalize();
	Vec3 front = getLookDir() - Vec3(up) * Vec3(up).DotProduct(getLookDir());
	front = front.Normalize();
	Vec3 right = front.CrossProduct(up);
	float len = getLookDir().Length();

	front = front.RotateByQuaternion(Vec4::FromAxisAngle(up, delAngle.x))
		.RotateByQuaternion(Vec4::FromAxisAngle(right, currAngles.y));

	setLookDir(front);	
	return *this;
}

void MyCamera::SetMouse(MyMouse* mptr) {
	mouseptr = mptr;
}

MyMouse& MyMouse::update() {
	prevPos = GetPosition();
	if (limitedCoor) {
		if (GetX() >= screenWidth)
			SetX(screenWidth - 1);
		if (GetY() >= screenHeight)
			SetY(screenHeight - 1);
		if (GetX() < 0)
			SetX(0);
		if (GetY() < 0)
			SetX(0);
	}

	return *this;
}

Vec2 MyMouse::getDelta() {
	return  GetPosition() - prevPos;
}

void MyMouse::limitMouse() {
	limitedCoor = true;
}

void MyMouse::unlimitMouse() {
	limitedCoor = false;
}
