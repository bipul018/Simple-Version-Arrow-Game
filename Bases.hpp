#pragma once
#include <raylib-cpp.hpp>

constexpr int screenWidth = 1800;
constexpr int screenHeight = 900;

using Vec2 = raylib::Vector2;
using Vec3 = raylib::Vector3;
using Vec4 = raylib::Vector4;
const Vec3 front(0, 0, 1);  //0 0 1
const Vec3 up(0, 1, 0);		//0 1 0
const Vec3 right(1, 0, 0);	//ok
#undef PI
const float PI = acos(0) * 2;
//Taken from Person model view, 1 unit ~ 1 feet
constexpr double oneMeter = 3.28084;// ~ 0.3048m
constexpr double gravityV = 9.80665 * oneMeter * 0.2;	//Last one is experimental factor

class ModelGen;
class GameModel;

class GameModel {
public:

	struct State {
		Vec3 pos;
		Vec3 front;
		Vec3 up;
		Vec3 scale;
		float angledir;
		State(Vec3 position = Vec3(0, 0, 0), Vec3 frontDir = ::front,
			Vec3 upDir = ::up, Vec3 scale = Vec3(1, 1, 1), float angle = PI / 2);
	};

	static raylib::Matrix state_transition(State from, State to);


private:


};

class ModelGen {

public:
	enum FileModels {
		TOP_OF_LIST = -3,
		UNIT_CYL_CENTER,
		UNIT_CUBE_CENTER ,
		PERSON=0,
		BOW,
		END_OF_LIST
	};

	ModelGen();
	ModelGen(FileModels modelid);
	ModelGen(const ModelGen&) = delete;
	~ModelGen();
	ModelGen& operator =(ModelGen&& model2) = default;
	raylib::Model& get_this_model() const;
	GameModel::State get_model_state() const;
	friend class GameModel;
	static GameModel::State return_state(ModelGen::FileModels model);
private:
	static void initialize_statics();
	raylib::Model* model = nullptr;
	raylib::Mesh* mesh = nullptr;
	FileModels modelID = END_OF_LIST;

	static const char* fileModelNames[END_OF_LIST];
	static GameModel::State fileModelStates[END_OF_LIST];
	static bool isInitialized;
};