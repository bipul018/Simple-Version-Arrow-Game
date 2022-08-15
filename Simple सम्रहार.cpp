//C++ Standard Library Includes
#include <iostream>
#include <sstream>

//Custom Include Files
#include "Tools.hpp"
#include "Arrow.hpp"
#include "Obstacles.hpp"
#include "Target.hpp"

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

class Instance {
public:

	Instance():arr(Vec3(originalTar) - D_Left * 0.25, D_Front, D_Up, 1.5),
		target(D_Front * -1, Vec3(originalTar) - D_Left * 0.25 + D_Front * 12, 3),
		window(screenWidth, screenHeight, "BOO NOOB") {

		Vec2 size(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
		
		screenWidth = size.x;
		screenHeight = size.y;

		window.SetSize(size);

		window.SetFullscreen(false);

		shootCam = MyCamera(originalPos, originalTar, Vec3(0.0f, 1.0f, 0.0f));
		shootCam.SetMode(CAMERA_CUSTOM);
		shootCam.SetProjection(CAMERA_PERSPECTIVE);
		shootCam.SetFovy(60.0f);
		shootCam.lookAt(arr.getState(), D_Front * -3 + D_Left * 0.5);
		arrCam = shootCam;
		refCam = shootCam;

		shootCam.SetMouse(&mouse);
		shootCam.isLikeFirstPerson = true;

		currCam = &shootCam;

		arr.velocity = arr.getFront() * 6;

		mouse.hideCursor().disableCursor();
		window.SetTargetFPS(120);


	}

	void run() {

		while (!window.ShouldClose()) {
			runLoop();
		}
	}

	void runLoop() {


		//For full screen 
		
		if (arrowReleased && collided) {
			if (justCollided) {
				justCollided = false;

				collTimeElapsed = window.GetTime();
			}
		}
		if (!arrowReleased && IsKeyReleased(KEY_ENTER)) {
			currCam = &arrCam;
			arrowReleased = !arrowReleased;
		}
		if (window.GetTime() - 3 > collTimeElapsed) {
			//Remember to seed these random values generators
			double delf = GetRandomValue(-3, 7);
			double dels = GetRandomValue(-5, 5);
			target.translateBy(D_Front * delf + D_Left * dels);
			arr.reset();
			collided = false;
			currCam = &shootCam;
			arrowReleased = !arrowReleased;
		}
		if (!gamePaused) {
			shootCam.Update();

			//This follows the arrow but with fixed up
			
			//arrCam.lookAt(arr.getState(), refCam.getLookDir()*-1, D_Up);
			arrCam.lookAt(arr.getState(), D_Front * -3 + D_Left * 0.5, D_Up);
		}
		mouse.limitInScreen();

		if (!gameFlags.at(GAME_PAUSED) || !gameFlags.at(GAME_START) || !gameFlags.at(GAME_OVER)) {
			if (gameFlags.at(ARROW_RELEASED)) {
				//Move the arrow code if released and not already on wall
				if (!gameFlags.at(ARROW_ON_WALL)) {
					arr.translateByVel();
					arr.spin();
					arr.velocity -= D_Up * gravityV * GetFrameTime() * 0.1;

					//Draw arrow rotated to place
					arr.rotateToVel();

					//Check if collides to wall and change flags if does
					unsigned targetRing = target.getCollisionRing(arr.getFront(), arr.getHead());
					if (target.doesCross(arr.getHead())) {
						if (targetRing != 0) {
							score += 5 * targetRing;
						}
						else {
							gameFlags.at(GAME_OVER) = true;
						}
						gameFlags.at(ARROW_ON_WALL) = true;
						gameFlags.at(ARROW_COLLIDE) = true;

						//Start the cooldown countdown timer
						collTimeElapsed = window.GetTime();
					}
				}
				else {

					//Being on wall check for countdown and move wall / reset if necessary
					if (window.GetTime() - collTimeElapsed > CoolDown) {
						collTimeElapsed = 0;
						gameFlags.at(ARROW_ON_WALL) = false;

						//Reset the arrow and move the wall 
						gameFlags.at(ARROW_RELEASED) = false;
						gameFlags.at(ARROW_ON_WALL) = false;
						
					}
				}
				
			}
			else {

				//Arrow targeting code

				arr.rotateFrontTo(shootCam.getLookDir());
				arr.velocity = arr.getFront() * arr.velocity.Length();

				//Arrow release code if prompted
				if (gameFlags.at(ARROW_STRETCHED) && arrStretchTime == 0) {
					//Start the timer if just stretched
					arrStretchTime = window.GetTime();
				}
				if (!gameFlags.at(ARROW_STRETCHED) && arrStretchTime != 0) {
					//Stop and release arrow
					
					//This is for getting optimum release some time after pressing
					//Being a sum of ln and exp functions , change in velocity per time button pressed is 
					//higher at beginning and later 
					//The factors afac and bfac are to be determined experimentally or even theoritically, if possible
					//Can also be adjusted for a difficulty setting thing
					constexpr double afac = 2.0;
					constexpr double bfac = 1.0;
					double delT = GetTime() - arrStretchTime;
					double speedfactor = afac * log(1 + delT) + exp(bfac * delT) - 1;

					arr.velocity = arr.getFront().Normalize() * speedfactor * oneMeter;

					gameFlags.at(ARROW_RELEASED) = true;
					
				}

			}
		}
		else {
			//Do stuff acc to game paused or over or start

		}


		if (!gamePaused) {
			if (!arrowReleased) {
				arr.rotateFrontTo(shootCam.getLookDir());
				arr.velocity = arr.getFront() * arr.velocity.Length();

			}
			else {
				if (!collided) {
					arr.translateByVel();
					arr.rotateToVel();
					arr.spin();
					arr.velocity -= D_Up * gravityV * GetFrameTime() * 0.1;
				}
				//This is collision detection part 
				unsigned targetRing = target.getCollisionRing(arr.getFront(), arr.getHead());
				if (target.doesCross(arr.getHead())) {
					if (!collided) {
						if (targetRing != 0) {
							justCollided = true;
							score += 10 * targetRing;
						}
						
					}
					collided = true;
				}
			}
		}

	}
private:

	void drawGame() {

		window.BeginDrawing();
		window.ClearBackground(SKYBLUE);
		currCam = &refCam;
		currCam->BeginMode();

		DrawPlane(Vec3(0, 0, 0), Vec2(20, 20), GREEN);

		//Vertical plane at front

		//DrawTriangle3D(D_Front * 10 + D_Left * 10,
		//	D_Front * 10 - D_Left * 10,
		//	D_Front * 10 - D_Left * 10 + D_Up * 10,GREEN);
		//DrawTriangle3D(D_Front * 10 + D_Left * 10,
		//	D_Front * 10 - D_Left * 10 + D_Up * 10,
		//	D_Front * 10 + D_Left * 10 + D_Up * 10,GREEN);
		//
		arr.draw();
		target.draw();
		currCam->EndMode();

		Vec2 circle = currCam->GetWorldToScreen(target.getProj(arr.getFront(), arr.getHead()));
		DrawCircleLines(circle.x, circle.y, 10, RED);
		DrawLine(circle.x - 5, circle.y, circle.x + 5, circle.y, RED);
		DrawLine(circle.x, circle.y - 5, circle.x, circle.y + 5, RED);

		std::stringstream ss;

		ss << "FPS : " << GetFPS() << std::endl
			<< "My Camera Angles : " << currCam->currAngles * 180 / PI << std::endl;
		if (collided)
			ss << "It Just Collided at : " << arr.getHead() << std::endl;
		ss << "\n\t\t Score " << score << std::endl;
		DrawText(ss.str().c_str(), 10, 10, 10, MAROON);

		window.EndDrawing();

	};
	void drawPause() {
		window.BeginDrawing();
		window.ClearBackground(SKYBLUE);

		DrawText("Game Paused, Press space to continue\n", screenWidth / 4, screenHeight / 4, 30, MAROON);

		window.EndDrawing();
	};

	const Vec3 originalPos = D_Front * -5 + D_Up * 6;
	const Vec3 originalTar = Vec3(originalPos) + D_Front * 3;

	raylib::Window window;
	MyCamera shootCam;
	MyCamera arrCam;
	MyCamera refCam;
	MyCamera* currCam;
	MyMouse mouse;
	Arrow arr;

	double collTimeElapsed = 0;
	const double CoolDown = 3;

	double arrStretchTime = 0;
	


	enum Flags {
		FLAG_EMPTY = -1,
		ARROW_RELEASED,			//For when arrow is released from bow
		ARROW_COLLIDE ,			//For when arrow has just collided in previous frame
		ARROW_ON_WALL ,			//For when arrow is released but stuck at wall
		ARROW_STRETCHED,		//For when user is stretching the bow
		GAME_PAUSED ,			//For when the game is paused
		GAME_OVER , 			//For when user missed the target and now game is reset
		GAME_START ,			//For when game is in starting condition
		FLAG_FULL 
	};
	std::array<bool, FLAG_FULL> gameFlags;

	bool collided = false;
	bool justCollided = false;
	bool gamePaused = true;
	bool arrowReleased = false;

	unsigned score = 0;

	Target target;
};


int main(){

	Instance ins;
	ins.run();



	return 0;

}