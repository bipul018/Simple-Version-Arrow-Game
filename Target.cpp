#include "Target.hpp"

Target::Target(Vec3 normal, Vec3 center, float radius) :
	defaultState(center, Vec3(D_Up).RotateByQuaternion(
		Vec4::FromVector3ToVector3(D_Front * -1, normal)
	), normal, Vec3(radius, 1, radius)) {
	currState = defaultState;
	for (size_t i = 0; i < models.size(); ++i) {
		models.at(i) = ModelGen(ModelGen::UNIT_CYL_CENTER);
	}
	colors.at(0) = BLACK;
	colors.at(1) = BLUE;
	colors.at(2) = YELLOW;
}

void Target::draw() {
	setupTransform();
	for (size_t i = 0; i < models.size(); ++i)
		models.at(i).get_this_model().Draw(Vec3(0, 0, 0), 1.0f,
			colors.at(i));
}

void Target::setupTransform() {
	State temp = currState;

	//Shifting so that top point is at curstate.pos
	temp.pos -= currState.up * currState.scale.y ;

	//Radius variation per circle
	const float stepR = temp.scale.x / models.size();
	//Thickness varies at most by 30% i.e, 0.3
	const float stepT = temp.scale.y * 0.3 / models.size();	
	temp.scale.y -= stepT * models.size();
	for (size_t i = 0; i < models.size(); ++i) {
		models.at(i).get_this_model().transform = 
			state_transition(models.at(i).get_model_state(), temp);
		temp.scale.x -= stepR;
		temp.scale.z -= stepR;
		temp.scale.y += stepT;
	}
}
