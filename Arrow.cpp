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

void Arrow::setupTransform(){
	State temp = currState;
	temp.scale -= D_Length * temp.scale.DotProduct(D_Length) * 0.035;
	temp.pos -= D_Length * temp.scale.DotProduct(D_Length) * 0.035 / 2;
	arrowBody.get_this_model().transform = state_transition(arrowBody.get_model_state(), temp);

	temp.scale = currState.scale - D_Length * (1 - 0.035) * currState.scale.DotProduct(D_Length);
	temp.pos = currState.pos + D_Length * (0.5 - 0.035) * currState.scale.DotProduct(D_Length);
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

void Arrow::draw(){
	setupTransform();
	arrowBody.get_this_model().Draw(Vec3(0, 0, 0), 1.0f, mCol);
	//arrowHead.get_this_model().Draw(Vec3(0, 0, 0), 1.0f, YELLOW);
}
