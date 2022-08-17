//C++ Standard Library Includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

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

Font console;

class Instance {
public:

	enum Flags {
		FLAG_EMPTY = -1,
		ARROW_RELEASED,			//For when arrow is released from bow
		ARROW_COLLIDE,			//For when arrow has just collided in previous frame
		ARROW_ON_WALL,			//For when arrow is released but stuck at wall
		ARROW_STRETCHED,		//For when user is stretching the bow
		GAME_PAUSED,			//For when the game is paused
		GAME_OVER, 			//For when user missed the target and now game is reset
		GAME_START,			//For when game is in starting condition
		GAME_QUIT,				//For quitting the game
		GAME_SETTINGS,			//When settings page is displayed
		FLAG_FULL
	};

	Instance():arr(D_Front * -1 - D_Left * 0.35+D_Up*5.85, D_Front, D_Up, 1.5),
		target(D_Front * -1, D_Front * 10 + D_Up * 6  - D_Left * 0.25 , 3),
		window(screenWidth, screenHeight, "BOO NOOB") {
		bowModel = std::move(ModelGen(ModelGen::BOW));
		Vec2 size(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
		
		screenWidth = size.x;
		screenHeight = size.y;

		window.SetSize(size);

		window.SetFullscreen(true);

		shootCam = MyCamera(D_Up * 6 + D_Front * -2, D_Up * 6 + D_Front , Vec3(0.0f, 1.0f, 0.0f));
		shootCam.SetMode(CAMERA_CUSTOM);
		shootCam.SetProjection(CAMERA_PERSPECTIVE);
		shootCam.SetFovy(60.0f);
		refCam = shootCam;
		arrCam = shootCam;

		shootCam.SetMouse(&mouse);
		shootCam.isLikeFirstPerson = true;

		currCam = &refCam;

		arr.velocity = arr.getFront() * 6;

		mouse.hideCursor().disableCursor();
		window.SetTargetFPS(120);

		//Image and resources load
		startImg.Load("final_start.png");
		startImgTex.Load(startImg);


		/*
			Load Image Clips	
		*/
		/*std::string clip = "clip/clip";
		std::string oneZero = "0";
		std::string twoZero = "00";
		std::string png = ".png";
		for (int i = 0; i <= 208; i++) {
			if (i<10 && i>= 0) {
				std::string newS = clip + twoZero + std::to_string(i) + png;
				introimgs.push_back(LoadImage(newS.c_str()));
			}
			else if (i<100&&i>=10) {
				std::string newS = clip + oneZero + std::to_string(i) + png;
				introimgs.push_back(LoadImage(newS.c_str()));
			}
			else {
				std::string newS = clip + std::to_string(i) + png;
				introimgs.push_back(LoadImage(newS.c_str()));
			}
		}

		for (Image& img : introimgs) {
			Texture2D tex = LoadTextureFromImage(img);
			introtex.push_back(tex);
		}*/


		for (auto& x : gameFlags) {
			x = false;
		}
		gameFlags.at(GAME_START) = true;
		
		windTimer = window.GetTime();

		//Initializing all the gui pages
		BoxDiv windiv = getWindowDiv();
		startPage = windiv;
		startPage.SetBorder(0);
		startPage.SetPadding(4 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));
		pausePage = startPage;
		settingsPage = startPage;
		TextBox txt;
		txt.SetText("Enter string here");
		txt.SetFontSize(30 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));
		txt.SetSpacing(8 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));
		txt.SetBorder(5 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));
		txt.SetPadding(9 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));

		std::function<void(BoxBase&,Flags)> setbind = [this](BoxBase& box, Flags flag) {
			setFlag(flag);
		};
		
		std::function<void(BoxBase&,Flags)> unsetbind = [this](BoxBase& box, Flags flag) {
			unsetFlag(flag);
		};

		{
			TextBox tmp(txt);
			tmp.SetText("Enter Your Name");
			tmp.SetBorder(0);
			guiObjs.push_back(new TextBox(tmp));
			startPage.childs.push_back(guiObjs.back());
		}


		txt.onHover = [](BoxBase& base) {
			raylib::Color c(SKYBLUE);
			c.a = 100;
			dynamic_cast<BoxDiv&>(base).SetBackColor(c);
		};

		txt.onNothing = [](BoxBase& base) {
			dynamic_cast<BoxDiv&>(base).SetBackColor(BLANK);
		};

		{
			TextEdit tmp(txt);
			tmp.SetText("");
			guiObjs.push_back(new TextEdit(tmp));
			startPage.childs.push_back(guiObjs.back());
		}
		
		{
			TextBox tmp(txt);
			tmp.SetText("RESUME");
			tmp.onClickRelease = std::bind(unsetbind, std::placeholders::_1, GAME_PAUSED);
			guiObjs.push_back(new TextBox(tmp));
			pausePage.childs.push_back(guiObjs.back());
		}
		
		{
			TextBox tmp(txt);
			tmp.SetText("SETTINGS");
			tmp.onClickRelease = std::bind(setbind, std::placeholders::_1, GAME_SETTINGS);
			guiObjs.push_back(new TextBox(tmp));
			pausePage.childs.push_back(guiObjs.back());
		}
		
		{
			TextBox tmp(txt);
			tmp.SetText("EXIT");
			tmp.onClickRelease = std::bind(setbind, std::placeholders::_1, GAME_QUIT);
			guiObjs.push_back(new TextBox(tmp));
			pausePage.childs.push_back(guiObjs.back());
		}
		
		{
			TextBox tmp(txt);
			tmp.SetText("BACK");
			tmp.onClickRelease = std::bind(unsetbind, std::placeholders::_1, GAME_SETTINGS);
			guiObjs.push_back(new TextBox(tmp));
			settingsPage.childs.push_back(guiObjs.back());
		}

		windiv.childs.push_back(&startPage);
		windiv.packByContent();
		windiv.setMouse(&mouse);
		windiv.childs.pop_back();
		
		windiv.childs.push_back(&pausePage);
		windiv.packByContent();
		windiv.setMouse(&mouse);
		windiv.childs.pop_back();
		
		windiv.childs.push_back(&settingsPage);
		windiv.packByContent();
		windiv.setMouse(&mouse);
		windiv.childs.pop_back();

		


	}
	
	void restart() {

	}

	void run() {

		while (!gameFlags.at(GAME_QUIT)) {
			if (gameFlags.at(GAME_PAUSED))
				drawPauseScreen();
			else if (gameFlags.at(GAME_START))
				drawGameStart();
			else if (gameFlags.at(GAME_OVER))
				drawGameOver();
			else
				drawGameScreen();

			eventHandle();
			runLoop();
		}
	}
	void eventHandle() {

		//Event handling stuff


		//Do stuff if game is not playable
		if (gameFlags.at(GAME_PAUSED) || gameFlags.at(GAME_START) || gameFlags.at(GAME_OVER)) {
			if (IsKeyReleased(KEY_ESCAPE)) {
				gameFlags.at(GAME_PAUSED) = false;
				gameFlags.at(GAME_START) = false;
				gameFlags.at(GAME_OVER) = false;
			}
			if (mouse.isCursorHidden()) {
				mouse.showCursor().enableCursor();
			}
			if (gameFlags.at(GAME_START))
				startPage.callActions();
			else if (gameFlags.at(GAME_PAUSED)) {
				if (gameFlags.at(GAME_SETTINGS))
					settingsPage.callActions();
				else
					pausePage.callActions();
			}
			else if (gameFlags.at(GAME_OVER))
				settingsPage.callActions();

		}

		//Do stuff if game if playable
		else {

			if (!mouse.isCursorHidden()) {
				mouse.hideCursor().disableCursor();
			}

			//Pause if required
			if (IsKeyReleased(KEY_ESCAPE)) {
				gameFlags.at(GAME_PAUSED) = true;
				return;
			}

			//Do stuff if target mode
			if (!gameFlags.at(ARROW_RELEASED)) {
				if (IsKeyPressed(KEY_ENTER)||mouse.IsButtonPressed(MOUSE_BUTTON_LEFT)) {
					gameFlags.at(ARROW_STRETCHED) = true;
				}
				else if (IsKeyReleased(KEY_ENTER) || mouse.IsButtonReleased(MOUSE_BUTTON_LEFT)) {
					gameFlags.at(ARROW_STRETCHED) = false;
				}
			}

			//Some future stuff to do when arrow just collides
			if (gameFlags.at(ARROW_COLLIDE)) {
				gameFlags.at(ARROW_COLLIDE) = false;
			}

		}
	}
	void runLoop() {

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
					//Gravity effect
					arr.velocity -= D_Up * gravityV * GetFrameTime() * 0.1;
					//Wind effect
					arr.velocity -= D_Left * windSpeed * GetFrameTime() ;
				
					//Draw arrow rotated to place
					arr.rotateToVel();

					//Check if collides to wall and change flags if does
					unsigned targetRing = target.getCollisionRing(arr.getFront(), arr.getHead());
					if ((arr.getHead().DotProduct(D_Up)<0) || target.doesCross(arr.getHead())) {
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

				//Bow setup code
				GameModel::State temp = arr.getState();
				temp.up = temp.up.RotateByQuaternion(Vec4::FromAxisAngle(temp.front, PI / 4));
				bowModel.get_this_model().transform = GameModel::state_transition(bowModel.get_model_state(), temp);


				//Wind code
				if (window.GetTime() - windTimer > 1.5) {
					int sign = (windSpeed >= 0) ? 1 : -1;
					windSpeed += sign * GetRandomValue(-500, 1000) / 1000.0;
					windSpeed *= (GetRandomValue(-3, 20) >= 0) ? 1 : -1;
					windTimer = window.GetTime();
				}
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
			if (gameFlags.at(GAME_OVER)) {
				score = 0;
				windSpeed = 0;
			}
			if (IsKeyDown(KEY_BACK))
				gameFlags.at(GAME_QUIT) = true;
		}


	}

	BoxDiv getWindowDiv() const {
		return BoxDiv(Vec2(0, 0), window.GetSize());
	}

	~Instance() {
		for (BoxBase* ptr : guiObjs)
			delete ptr;
		for (Image& img : introimgs)
			UnloadImage(img);
		for (Texture2D& tex : introtex)
			UnloadTexture(tex);
	}
private:

	void drawGameScreen() {

		window.BeginDrawing();
		window.ClearBackground(SKYBLUE);
		currCam->BeginMode();

		DrawPlane(Vec3(0, 0, 0), Vec2(20, 20), GREEN);

		
		arr.draw();
		target.draw();
		
		bowModel.get_this_model().Draw(Vec3(0, 0, 0), 1.0f, arr.mCol);


		currCam->EndMode();

		Vec2 circle = currCam->GetWorldToScreen(target.getProj(arr.getFront(), arr.getHead()));
		DrawCircleLines(circle.x, circle.y, 10, RED);
		DrawLine(circle.x - 5, circle.y, circle.x + 5, circle.y, RED);
		DrawLine(circle.x, circle.y - 5, circle.x, circle.y + 5, RED);

		std::stringstream ss;

		ss << "Wind : " << windSpeed;
		ss << "\n\t\t Score " << score << std::endl;
		DrawText(ss.str().c_str(), 10, 10, 10, MAROON);

		window.EndDrawing();

	};

	void drawPauseScreen() {
		window.BeginDrawing();
		window.ClearBackground();
		startImgTex.Draw(Vec2(0, 0));
		if (gameFlags.at(GAME_PAUSED)) {
			if (gameFlags.at(GAME_SETTINGS))
				settingsPage.draw();
			else
				pausePage.draw();
		}

		window.EndDrawing();
	};

	void drawGameOver() {
		window.BeginDrawing();
		window.ClearBackground();
		startImgTex.Draw(Vec2(0, 0));
		settingsPage.draw();

		window.EndDrawing();

	}

	void drawGameStart() {
		window.BeginDrawing();
		window.ClearBackground();
	

		//Loads while playing 
		if (frameCount > 208) {
			startImgTex.Draw(Vec2(0, 0));
			startPage.draw();
		}
		else {
			std::string clip = "clip/clip";
			std::string oneZero = "0";
			std::string twoZero = "00";
			std::string png = ".png";
			std::string newS = "";
			if (frameCount < 10 && frameCount >= 0) {
				newS = clip + twoZero + std::to_string(frameCount) + png;
			}
			else if (frameCount < 100 && frameCount >= 10) {
				newS = clip + oneZero + std::to_string(frameCount) + png;
			}
			else {
				newS = clip + std::to_string(frameCount) + png;
			}
			introimgs.push_back(LoadImage(newS.c_str()));
			//introtex.push_back(LoadTextureFromImage(introimgs.back()));
			static Texture2D* tex = nullptr;
			if (tex != nullptr)
				UnloadTexture(*tex);
			if (tex == nullptr)
				tex = new Texture2D;
			*tex = LoadTextureFromImage(introimgs.back());

			DrawTexture(*tex, 0, 0, WHITE);
			frameCount++;
		}


		/*if (frameCount <= 208) {
			DrawTexture(introtex.at(frameCount++), 0, 0, WHITE);
		}*/
		//else {
			//startImgTex.Draw(Vec2(0, 0));
		//DrawTexture(introtex.at(2), 0, 0, WHITE);
		
		//}
		DrawFPS(10, 10);
		
		window.EndDrawing();

	}

	//const Vec3 originalPos = D_Front * -5 + D_Up * 6;
	//const Vec3 originalTar = Vec3(originalPos) + D_Front * 3;

	void setFlag(Flags flag) {
		if (flag == FLAG_EMPTY) {
			for (bool& x : gameFlags)
				x = false;
		}
		else if (flag == FLAG_FULL) {
			for (bool& x : gameFlags)
				x = true;
		}
		else
			gameFlags.at(flag) = true;
	}
	
	void unsetFlag(Flags flag) {
		if (flag == FLAG_EMPTY) {
			for (bool& x : gameFlags)
				x = true;
		}
		else if (flag == FLAG_FULL) {
			for (bool& x : gameFlags)
				x = false;
		}
		else
			gameFlags.at(flag) = false;
	}

	raylib::Window window;
	MyCamera shootCam;
	MyCamera arrCam;
	MyCamera refCam;
	MyCamera* currCam;
	MyMouse mouse;
	Arrow arr;
	ModelGen bowModel;

	raylib::Image startImg;
	raylib::Texture2D startImgTex;

	//Divisions for each pages to be displayed
	BoxDiv startPage, pausePage, settingsPage;

	//Vector of gui objects pointer dynamically created
	std::vector<BoxBase*> guiObjs;

	double collTimeElapsed = 0;
	const double CoolDown = 3;

	double arrStretchTime = 0;
	
	double windSpeed = 0;
	double windTimer = 0;

	std::array<bool, FLAG_FULL> gameFlags;

	bool collided = false;
	bool justCollided = false;
	bool gamePaused = true;
	bool arrowReleased = false;

	unsigned score = 0;

	Target target;

	std::vector<Image> introimgs;
	std::vector<Texture2D> introtex;
	
	/******/
	int frameCount = 0;
};


int main(){


	try {
		Instance ins;
		
		ins.run();
	}
	catch (const char* str) {
		std::fstream out("logfile", std::ios::out);
		out << str;
		out.close();
	}
	catch (raylib::RaylibException err) {
		std::fstream out("logfile", std::ios::out);
		out << err.what();
		out.close();
	}

	//UnloadFont(console);
	return 0;

}