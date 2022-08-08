#include "Obstacles.hpp"

Obstacle::Obstacle() :defaultState(Vec3(0,0,0), D_Front, D_Up, Vec3(D_Width * 4+ D_Height * 5+D_Length * 10)) {
	obsModel = ModelGen(ModelGen::UNIT_CUBE_CENTER);
	currState = defaultState;
}

void Obstacle::setupTransform(){
	obsModel.get_this_model().transform = state_transition(obsModel.get_model_state(), currState);
}

void Obstacle::draw(){
	setupTransform();
	obsModel.get_this_model().Draw(Vec3(0, 0, 0), 1.0f, mCol);
}
