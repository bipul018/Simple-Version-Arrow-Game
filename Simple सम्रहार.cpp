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


	Vec3 originalPos = D_Length * -4 + D_Up * 6;
	Vec3 originalTar = originalPos + D_Length * 4;
	MyCamera cam;
	cam.SetMode(CAMERA_CUSTOM);
	cam.position = originalPos;
	cam.target = originalTar;
	cam.up = Vec3(0.0f, 1.0f, 0.0f);
	cam.fovy = 60.0f;
	cam.projection = CAMERA_PERSPECTIVE;

	Arrow arr(D_Up * 6,D_Length,D_Up);
	arr.velocity = arr.velocity ;
	
	MyMouse mouse;
	mouse.update();
	cam.SetMouse(&mouse);

	window.SetTargetFPS(120);
	while (!window.ShouldClose()) {
		cam.Update();

		mouse.update();
		window.BeginDrawing();
		window.ClearBackground(RAYWHITE);

		cam.BeginMode();
		
		DrawPlane(Vec3(0, 0, 0), Vec2(100, 100), GREEN);
		DrawSphere(Vec3(0, 0, 0), 0.05, BLUE);
		DrawSphere(originalPos, 0.05, RED);
		DrawSphere(originalTar, 0.05, YELLOW);
		raylib::Ray rrr(Vec3(0, 0, 0), D_Front);
		rrr.Draw(BLUE);
		rrr = raylib::Ray(Vec3(0, 0, 0), D_Left);
		rrr.Draw(RED);

		arr.draw();

		cam.EndMode();

		std::stringstream ss;
		
		ss << "Mouse position : " << GetMousePosition() << std::endl
			<< "Mouse Delta : " << GetMouseDelta() << std::endl
			<< "My Mouse Delta : " << mouse.getDelta() << std::endl;


		DrawText(ss.str().c_str(), 10, 10, 10, MAROON);

		window.EndDrawing();
	}

}