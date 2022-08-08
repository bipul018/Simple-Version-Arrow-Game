#include "Arrow.hpp"

Arrow::Arrow(Vec3 pos, Vec3 front, Vec3 up, float len) :
	defaultState(pos, front, up, Vec3(D_Length * len + D_Height * 0.035 * len + D_Width * 0.035 * len)) {
	arrowModel = ModelGen(ModelGen::UNIT_CUBE_CENTER);
	currState = defaultState;
}


void Arrow::spin(float angle){
	currState.up = currState.up.RotateByQuaternion(Vec4::FromAxisAngle(currState.front, angle));
}

void Arrow::setupTransform(){
	arrowModel.get_this_model().transform = state_transition(arrowModel.get_model_state(), currState);
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

void Arrow::draw(){
	setupTransform();
	arrowModel.get_this_model().Draw(Vec3(0, 0, 0), 1.0f, mCol);
}
