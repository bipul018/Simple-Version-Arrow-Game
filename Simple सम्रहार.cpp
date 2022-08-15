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

		window.SetFullscreen(true);

		shootCam = MyCamera(originalPos, originalTar, Vec3(0.0f, 1.0f, 0.0f));
		shootCam.SetMode(CAMERA_CUSTOM);
		shootCam.SetProjection(CAMERA_PERSPECTIVE);
		shootCam.SetFovy(60.0f);
		shootCam.lookAt(arr.getState(), D_Front * -3 + D_Left * 0.5);
		arrCam = shootCam;
		refCam = shootCam;

		shootCam.SetMouse(&mouse);
		shootCam.isLikeFirstPerson = true;

		currCam = &refCam;

		arr.velocity = arr.getFront() * 6;

		mouse.hideCursor().disableCursor();
		window.SetTargetFPS(120);

		for (auto& x : gameFlags) {
			x = false;
		}
		gameFlags.at(GAME_START) = true;
		

	}

	void run() {

		while (!window.ShouldClose()) {
			if (gameFlags.at(GAME_PAUSED))
				drawPauseScreen();
			else if (gameFlags.at(GAME_START))
				drawGameStart();
			else if (gameFlags.at(GAME_OVER))
				drawGameOver();
			else
				drawGameScreen();
			runLoop();
		}
	}

	void runLoop() {
		//Event handling stuff

		
		//Do stuff if game is not playable
		if (gameFlags.at(GAME_PAUSED) || gameFlags.at(GAME_START) || gameFlags.at(GAME_OVER)) {
			if (IsKeyPressed(KEY_SPACE)) {
				gameFlags.at(GAME_PAUSED) = false;
				gameFlags.at(GAME_START) = false;
				gameFlags.at(GAME_OVER) = false;
			}
		}

		//Do stuff if game if playable
		else {

			//Do stuff if target mode
			if (!gameFlags.at(ARROW_RELEASED)) {
				if (IsKeyPressed(KEY_ENTER)) {
					gameFlags.at(ARROW_STRETCHED) = true;
				}
				else if (IsKeyReleased(KEY_ENTER)) {
					gameFlags.at(ARROW_STRETCHED) = false;
				}
			}

			//Some future stuff to do when arrow just collides
			if (gameFlags.at(ARROW_COLLIDE)) {
				gameFlags.at(ARROW_COLLIDE) = false;
			}

		}

		//Camera stuff
		shootCam.Update();

		//This follows the arrow but with fixed up
		arrCam.lookAt(arr.getState(), D_Front * -3 + D_Left * 0.5, D_Up);

		//Limit mouse inside screen
		mouse.limitInScreen();

		if (!gameFlags.at(GAME_PAUSED) && !gameFlags.at(GAME_START) && !gameFlags.at(GAME_OVER)) {
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

						arr.reset();

						//Remember to seed these random values generators
						double delf = GetRandomValue(-3, 7);
						double dels = GetRandomValue(-5, 5);

						target.translateBy(D_Front * delf + D_Left * dels);

						currCam = &refCam;

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
					currCam = &arrCam;
					arrStretchTime = 0;
				}

			}
		}
		else {
			//Do stuff acc to game paused or over or start
			if (gameFlags.at(GAME_OVER))
				score = 0;
		}


	}
private:

	void drawGameScreen() {

		window.BeginDrawing();
		window.ClearBackground(SKYBLUE);
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

	void drawPauseScreen() {
		window.BeginDrawing();
		window.ClearBackground(SKYBLUE);

		DrawText("Game Paused, Press space to continue\n", screenWidth / 4, screenHeight / 4, 30, MAROON);

		window.EndDrawing();
	};

	void drawGameOver() {
		window.BeginDrawing();
		window.ClearBackground(SKYBLUE);

		DrawText("Game over, Press space to continue\n", screenWidth / 4, screenHeight / 4, 30, MAROON);

		window.EndDrawing();

	}

	void drawGameStart() {
		window.BeginDrawing();
		window.ClearBackground(SKYBLUE);

		DrawText("Welcome, Press space to continue\n", screenWidth / 4, screenHeight / 4, 30, MAROON);

		window.EndDrawing();

	}

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