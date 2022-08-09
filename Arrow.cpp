#include "Arrow.hpp"

Arrow::Arrow(Vec3 pos, Vec3 front, Vec3 up, float len) :
	defaultState(pos, front, up, Vec3(D_Length * len + D_Height * 0.035 * len + D_Width * 0.035 * len)) {
	arrowBody = std::move(ModelGen(ModelGen::UNIT_CUBE_CENTER));
	arrowHead = std::move(ModelGen(ModelGen::UNIT_SQ_PYR_BOTTOM));
	currState = defaultState;
}


void Arrow::spin(float angle){
	currState.up = currState.up.RotateByQuaternion(Vec4::FromAxisAngle(currState.front, angle));
}

void Arrow::rotateVertical(float angle) {
	Vec3 right = currState.front.CrossProduct(currState.up);
	currState.front = currState.front.RotateByQuaternion(Vec4::FromAxisAngle(right, angle));
	currState.up = currState.up.RotateByQuaternion(Vec4::FromAxisAngle(right, angle));
}

void Arrow::rotateHorizontal(float angle) {
	currState.front = currState.front.RotateByQuaternion(Vec4::FromAxisAngle(currState.up, angle));
}

void Arrow::setupTransform(){

	State temp = currState;
	temp.scale -= D_Length * 0.035 * temp.scale.DotProduct(D_Length);
	temp.pos -= temp.front * 0.035 * 0.5 * temp.scale.DotProduct(D_Length);

	arrowBody.get_this_model().transform = state_transition(arrowBody.get_model_state(), temp);

	temp.scale = currState.scale - D_Length * (1 - 0.035) * currState.scale.DotProduct(D_Length);
	temp.pos = currState.pos + currState.front * (0.5 - 0.035) * currState.scale.DotProduct(D_Length);
	arrowHead.get_this_model().transform = state_transition(arrowHead.get_model_state(), temp);
}

void Arrow::resetArrow() {
	currState = defaultState;
}

void Arrow::translateBy(Vec3 delR) {
	currState.pos += delR;
}

void Arrow::translateByVel() {
	currState.pos += velocity * GetFrameTime();
}

void Arrow::rotateToVel() {

	if (velocity.Normalize() == currState.front * -1) {
		currState.up = currState.up.RotateByQuaternion(Vec4::FromAxisAngle(
			currState.up.CrossProduct(currState.front), PI
		));
	}
	else {
		Vec4 rotator = Vec4::FromVector3ToVector3(currState.front, velocity.Normalize());
		currState.up = currState.up.RotateByQuaternion(rotator);
	}
	currState.front = velocity.Normalize();
}

Vec3 Arrow::getFront() const {
	return currState.front;
}

Vec3 Arrow::getUp() const {
	return currState.up;
}

Vec3 Arrow::getCenter() const {
	return currState.pos;
}

Vec3 Arrow::getHead() const {
	return  Vec3(currState.pos) + currState.front * 0.5 * Vec3(currState.scale).DotProduct(D_Length);
}

Vec3 Arrow::getTail() const {
	return  Vec3(currState.pos) - currState.front * 0.5 * Vec3(currState.scale).DotProduct(D_Length);
}

GameModel::State Arrow::getState() const {
	return currState;
}

void Arrow::draw(){
	setupTransform();
	arrowBody.get_this_model().Draw(Vec3(0, 0, 0), 1.0f, mCol);
	arrowHead.get_this_model().Draw(Vec3(0, 0, 0), 1.0f, YELLOW);
}
