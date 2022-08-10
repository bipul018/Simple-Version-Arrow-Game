#include "Tools.hpp"

void MyCamera::setLookDir(Vec3 look) {
	if (isLikeFirstPerson)
		position = Vec3(target) - look;
	else
		target = look + position;
}

Vec3 MyCamera::getLookDir() const {
	return Vec3(target) - position;
}

MyCamera& MyCamera::Update() {

	return changeAngles(getDelAngle());
}

Vec2 MyCamera::getDelAngle() const {
	//Updates Based on our own mpuse derived one
	if (mouseptr == nullptr)
		return Vec2(0, 0);
	Vec2 delMouse = mouseptr->getDelta() * -1;


	Vec2 delAngle = screenAngles * delMouse / Vec2(screenWidth, screenHeight);
	if (maxAngles.x != 0)
		if (((delAngle.x + currAngles.x) <= -maxAngles.x) || ((delAngle.x + currAngles.x) >= maxAngles.x))
			delAngle.x = 0;
	if (maxAngles.y != 0)
		if (((delAngle.y + currAngles.y) <= -maxAngles.y) || ((delAngle.y + currAngles.y) >= maxAngles.y))
			delAngle.y = 0;
	return delAngle;
}

MyCamera& MyCamera::lookAt(GameModel::State state, Vec3 relativeDir) {
	return lookAt(state, relativeDir, state.up);
}

MyCamera& MyCamera::lookAt(GameModel::State state, Vec3 relativeDir, Vec3 manualUp) {
	target = state.pos;
	position = Vec3(target) +
		relativeDir.RotateByQuaternion(Vec4::FromVector3ToVector3(D_Front, state.front));
	up = manualUp;

	return *this;
}

MyCamera& MyCamera::changeAngles(Vec2 delAngle) {
	currAngles += delAngle;
	up = Vec3(up).Normalize();
	Vec3 front = getLookDir() - Vec3(up) * Vec3(up).DotProduct(getLookDir());
	front = front.Normalize();
	Vec3 right = front.CrossProduct(up);
	float len = getLookDir().Length();

	front = getLookDir().RotateByQuaternion(Vec4::FromAxisAngle(up, delAngle.x))
		.RotateByQuaternion(Vec4::FromAxisAngle(right, delAngle.y));

	setLookDir(front);
	return *this;
}

void MyCamera::SetMouse(MyMouse* mptr) {
	mouseptr = mptr;
}


Vec2 MyMouse::getDelta() {
	return  GetMouseDelta();
}

MyMouse& MyMouse::limitInScreen() {
	if (GetX() >= screenWidth)
		SetX(screenWidth - 1);
	if (GetY() >= screenHeight)
		SetY(screenHeight - 1);
	if (GetX() < 0)
		SetX(0);
	if (GetY() < 0)
		SetY(0);
	return *this;
}



MyMouse& MyMouse::hideCursor() {
	HideCursor();
	return *this;
}

MyMouse& MyMouse::showCursor() {
	ShowCursor();
	return *this;
}

MyMouse& MyMouse::disableCursor() {
	DisableCursor();
	return *this;
}

MyMouse& MyMouse::enableCursor() {
	EnableCursor();
	return *this;
}
