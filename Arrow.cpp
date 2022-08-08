#include "Arrow.hpp"

Arrow::Arrow():defaultState(up*5,front,up,Vec3(1,5,10)) {
	arrowModel = ModelGen(ModelGen::UNIT_CUBE_CENTER);
	currState = defaultState;
}

void Arrow::setupTransform(){
	arrowModel.get_this_model().transform = state_transition(arrowModel.get_model_state(), currState);
}

void Arrow::draw(){
	arrowModel.get_this_model().Draw(Vec3(0, 0, 0), 1.0f, mCol);
}
