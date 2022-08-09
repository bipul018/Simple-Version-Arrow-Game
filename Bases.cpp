#include "Bases.hpp"


const char* ModelGen::fileModelNames[END_OF_LIST];
GameModel::State ModelGen::fileModelStates[END_OF_LIST];
bool ModelGen::isInitialized = false;
GameModel::State::State(Vec3 position, Vec3 frontDir, Vec3 upDir, Vec3 scaleVal, float angle) :
	pos(position), front(frontDir.Normalize()), up(upDir.Normalize()), scale(scaleVal), angledir(angle) {
	if (front.DotProduct(up) != 0) {
		throw std::string("Up direction not perpendicular to front direction.");
	}

}

raylib::Matrix GameModel::state_transition(State from, State to){

	to.front = to.front.Normalize();
	to.up = to.up.Normalize();

	Vec3 delT = from.pos;
	delT -= to.pos;

	Vec3 scale = from.scale;
	scale /= to.scale;


	Vec4 rotate = Vec4::FromVector3ToVector3(to.up, from.up);

	Vec3 newfront = to.front.Transform(rotate.ToMatrix()).Normalize();


	double cosval = newfront.DotProduct(from.front);
	double sinval = (newfront.CrossProduct(from.front)).DotProduct(from.up);
	double angle = 0;
	if (sinval >= 0 && cosval >= 0)
		angle = acos(abs(cosval));
	else if (sinval >= 0 && cosval <= 0)
		angle = PI - acos(abs(cosval));
	else if (sinval < 0 && cosval < 0)
		angle = PI + acos(abs(cosval));
	else
		angle = 2 * PI - acos(abs(cosval));




	raylib::Matrix rotM = raylib::Matrix::Rotate(from.up, angle);

	rotM = raylib::Matrix(rotate.ToMatrix()) * rotM;


	raylib::Matrix initMatrix = raylib::Matrix::Translate(delT.x, delT.y, delT.z)
		* rotM
		* raylib::Matrix::Scale(scale.x, scale.y, scale.z);
	initMatrix = initMatrix.Invert();

	return initMatrix;
}

ModelGen::ModelGen(){
	if (!isInitialized)
		initialize_statics();
}

ModelGen::ModelGen(FileModels modelid){
	if (!isInitialized)
		initialize_statics();
	if (static_cast<int>(modelid) < 0) {
		switch (modelid) {
		case UNIT_CYL_CENTER:
			mesh = new raylib::Mesh(raylib::Mesh::Cylinder(1, 1, 30));
			model = new raylib::Model(*mesh);
			modelID = UNIT_CYL_CENTER;
			return;
		case UNIT_CUBE_CENTER:

			mesh = new raylib::Mesh(raylib::Mesh::Cube(1, 1, 1));
			model = new raylib::Model(*mesh);
			modelID = UNIT_CUBE_CENTER;
			return;
		default:
			throw std::string("Error, invalid / unregistered id model requested.\n");
		}
	}
	if (static_cast<int>(modelid) >= static_cast<int>(END_OF_LIST))
		throw std::string("Model id somehow beyond list requested.\n");

	model = new raylib::Model(fileModelNames[modelid]);
	modelID = modelid;
}

ModelGen::~ModelGen(){

	if (!model)
		delete model;
	if (!mesh)
		delete mesh;
}

raylib::Model& ModelGen::get_this_model() const{
	if (model == nullptr)
		throw "ERROR_NO_MODEL";
	return *model;
}

GameModel::State ModelGen::get_model_state() const{
	return return_state(modelID);
}

void ModelGen::initialize_statics(){
	fileModelNames[PERSON] = "model.iqm";
	fileModelNames[BOW] = "model_bow.iqm";

	fileModelStates[PERSON] = GameModel::State(
		Vec3(0, 0, 0),
		Vec3(D_Up * (1)),
		Vec3(D_Front * (1)),
		Vec3(1.0, 1.0, 1.0),
		PI / 2);

	fileModelStates[BOW] = GameModel::State(
		Vec3(0, 0, 0), D_Left * -1, D_Front, Vec3(1, 1, 1), PI / 2
	);
}

GameModel::State ModelGen::return_state(ModelGen::FileModels model){
	switch (model) {

	case FileModels::END_OF_LIST:
		throw std::string("Invalid model state requested.");
	case FileModels::UNIT_CUBE_CENTER:
		return GameModel::State();
	case FileModels::UNIT_CYL_CENTER:
		return GameModel::State();
	default:
		if (static_cast<int>(model) >= 0 && (static_cast<int>(model)< static_cast<int>(END_OF_LIST)))
			return fileModelStates[static_cast<int>(model)];
	}
	throw std::string("No Model Found, internal error");
}
