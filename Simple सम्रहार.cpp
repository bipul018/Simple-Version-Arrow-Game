//C++ Standard Library Includes
#include <iostream>

//Custom Include Files
#include "Tools.hpp"
#include "Arrow.hpp"
#include "Obstacles.hpp"



int main(){

	raylib::Window window(screenWidth, screenHeight, "BOO NOOB");


	Vec3 originalPos = D_Length * -4 + D_Up * 6;
	Vec3 originalTar = originalPos + D_Length * 4;
	MyCamera cam;
	cam.SetPosition(Vec3(0, 4, 0));
	cam.SetMode(CAMERA_FIRST_PERSON);
	cam.position = originalPos;
	cam.target = originalTar;
	cam.up = Vec3(0.0f, 1.0f, 0.0f);
	cam.fovy = 60.0f;
	cam.projection = CAMERA_PERSPECTIVE;

	Arrow arr(D_Up * 6,D_Length,D_Up);
	arr.velocity = arr.velocity ;
	

	window.SetTargetFPS(120);
	while (!window.ShouldClose()) {
		cam.Update();

		window.BeginDrawing();
		window.ClearBackground(RAYWHITE);

		cam.BeginMode();
		
		DrawPlane(Vec3(0, 0, 0), Vec2(100, 100), GREEN);
		DrawSphere(Vec3(0, 0, 0), 0.05, BLUE);
		DrawSphere(originalPos, 0.05, RED);
		DrawSphere(originalTar, 0.05, YELLOW);
		raylib::Ray rrr(Vec3(0, 0, 0), D_Front);
		rrr.Draw(BLUE);
		rrr = raylib::Ray(Vec3(0, 0, 0), D_Right);
		rrr.Draw(RED);

		arr.draw();

		cam.EndMode();
		window.EndDrawing();

	}

}