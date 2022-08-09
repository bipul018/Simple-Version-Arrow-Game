//C++ Standard Library Includes
#include <iostream>
#include <sstream>

//Custom Include Files
#include "Tools.hpp"
#include "Arrow.hpp"
#include "Obstacles.hpp"

//Some debugging tools 

std::ostream& operator<<(std::ostream& os, const Matrix& m) {

	os << "\n";
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			os << (&(m.m0))[i * 4 + j] << "\t";
		}
		os << "\n";
	}
	return os;
}
std::ostream& operator<<(std::ostream& os, const Vec3& m) {
	os << " [" << m.x << " , " << m.y << " , " << m.z << "] ";
	return os;
}
std::ostream& operator<<(std::ostream& os, const Vec2& m) {
	os << " [" << m.x << " , " << m.y << "] ";
	return os;
}


int main(){

	raylib::Window window(screenWidth, screenHeight, "BOO NOOB");
	//window.SetFullscreen(true);

	Vec3 originalPos = D_Front * -5 + D_Up * 6;
	Vec3 originalTar = originalPos + D_Front *3;
	MyCamera cam;
	cam.SetMode(CAMERA_CUSTOM);
	cam.position = originalPos;
	cam.target = originalTar;
	cam.up = Vec3(0.0f, 1.0f, 0.0f);
	cam.fovy = 60.0f;	
	cam.projection = CAMERA_PERSPECTIVE;

	Arrow arr(originalTar //+ D_Left * 1.5 / 2
		, D_Front, D_Up,1.5);
	
	
	arr.rotateVertical(PI / 4);
	arr.velocity = arr.getFront() * 8;

	MyMouse mouse;
	mouse.hideCursor().disableCursor();

	cam.SetMouse(&mouse);

	window.SetTargetFPS(120);
	while (!window.ShouldClose()) {
		//For full screen 
		if (IsKeyReleased(KEY_SPACE)) {
			if (window.IsFullscreen()) {
				window.SetFullscreen(false);
			}
			else {
				window.SetFullscreen(true);
			}
			Vec2 size = window.GetSize();
			screenWidth = size.x;
			screenHeight = size.y;
		}

		cam.Update();
		mouse.limitInScreen();

		//arr.translateByVel();
		arr.rotateToVel();
		arr.velocity -= D_Up * gravityV * GetFrameTime() * 0.1;

		window.BeginDrawing();
		window.ClearBackground(SKYBLUE);

		cam.BeginMode();
		
		DrawPlane(Vec3(0, 0, 0), Vec2(100, 100), GREEN);
		arr.draw();

		cam.EndMode();
	
		std::stringstream ss;
		
		ss << "Mouse position : " << GetMousePosition() << std::endl
			<< "Mouse Delta : " << GetMouseDelta() << std::endl
			<< "My Mouse Delta : " << mouse.getDelta() << std::endl
			<< "My Camera Angles : " << cam.currAngles * 180 / PI << std::endl;

		DrawText(ss.str().c_str(), 10, 10, 10, MAROON);

		window.EndDrawing();
	}

}