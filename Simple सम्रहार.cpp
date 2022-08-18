//C++ Standard Library Includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <thread>

//Custom Include Files
#include "GraphicsHelp.hpp"
#include "Arrow.hpp"
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

	enum Flags {
		FLAG_EMPTY = -1,
		ARROW_RELEASED,			//For when arrow is released from bow
		ARROW_COLLIDE,			//For when arrow has just collided in previous frame
		ARROW_ON_WALL,			//For when arrow is released but stuck at wall
		ARROW_STRETCHED,		//For when user is stretching the bow
		GAME_PAUSED,			//For when the game is paused
		GAME_OVER, 				//For when user missed the target and now game is reset
		GAME_START,				//For when game is in starting condition
		GAME_QUIT,				//For quitting the game
		GAME_SETTINGS,			//When settings page is displayed
		GAME_SCORES,			//When scores page is to be displayed
		GAME_RESET,				//Made to know if to reset the game 
		GAME_RESIZE,			//Indicates that game has just been resized
		FULL_SCREEN,			//Indicates that game is fullscreen
		FLAG_FULL
	};

	Instance():arr(D_Front * -1 - D_Left * 0.35+D_Up*5.85, D_Front, D_Up, 1.5),
		target(D_Front * -1, D_Front * 10 + D_Up * 6  - D_Left * 0.25 , 3),
		window(screenWidth, screenHeight, "BOO NOOB"),scores(std::function<bool(float,float)>(GreaterThan())) {
		bowModel = std::move(ModelGen(ModelGen::BOW));
		Vec2 size(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
		
		screenWidth = size.x;
		screenHeight = size.y;

		window.SetSize(size);
		window.SetPosition(30, 40);
		window.SetFullscreen(true);

		SetExitKey(KEY_NULL);

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

		scores.clear();

		//Image and resources load
		startImg.Load("final_start.png");
		startImgTex.Load(startImg);

		pauseImg.Load("final_pause.png");
		pauseImgTex.Load(pauseImg);
		
		gameImg.Load("final_game.png");
		gameImgTex.Load(gameImg);
		
		InitAudioDevice();
		startAudio.Load("game_intro1.mp3");
		gamesAudio.Load("game_music1.mp3");
		hoverAudio.Load("hoversound.mp3");
		clickAudio.Load("clicksound.mp3");
		
		startAudio.looping = true;
		gamesAudio.looping = true;

		gamesAudio.Seek(7);

		arr.reset();
		target.reset();

		for (auto& x : gameFlags) {
			x = false;
		}
		gameFlags.at(GAME_START) = true;
		gameFlags.at(GAME_RESET) = true;
		gameFlags.at(GAME_RESIZE) = true;
		gameFlags.at(FULL_SCREEN) = true;
		windTimer = window.GetTime();

		resetUI();
		//parallelRun = new std::thread([this]() {resetUI(); });
		//parallelRun->join();

	}

	void resetUI() {
		if (gameFlags.at(GAME_RESIZE)) {

			Vec2 size = window.GetSize();

			if (gameFlags.at(FULL_SCREEN))
				size = Vec2(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));

			startImgTex.SetHeight(size.y);
			startImgTex.SetWidth(size.x);
			
			pauseImgTex.SetHeight(size.y);
			pauseImgTex.SetWidth(size.x);
			
			gameImgTex.SetHeight(size.y);
			gameImgTex.SetWidth(size.x);
			

			gameFlags.at(GAME_RESIZE) = false;
		}
		//Initializing all the gui pages except name box if already initialized to temporary objs
		if (nameBox != nullptr) {
			nameBox = new TextEdit(*nameBox);
		}

		for (BoxBase* box : guiObjs)
			delete box;
		guiObjs.clear();

		BoxDiv windiv = getWindowDiv();
		startPage = windiv;
		startPage.SetBorder(0);
		startPage.SetPadding(4 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));
		pausePage = startPage;
		settingsPage = startPage;
		scorePage = startPage;
		outPage = startPage;

		TextBox txt;
		txt.SetText("Enter string here");
		txt.SetFontSize(30 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));
		txt.SetSpacing(8 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));
		txt.SetBorder(5 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));
		txt.SetPadding(9 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor()));

		std::function<void(BoxBase&, Flags)> setbind = [this](BoxBase& box, Flags flag) {
			setFlag(flag);
		};

		std::function<void(BoxBase&, Flags)> unsetbind = [this](BoxBase& box, Flags flag) {
			unsetFlag(flag);
		};


		//Start Page stuff
		{
			TextBox tmp(txt);
			tmp.SetText("Enter Your Name");
			tmp.SetBorder(0);
			guiObjs.push_back(new TextBox(tmp));
			startPage.childs.push_back(guiObjs.back());
		}

		
		txt.onHover = [this](BoxBase& base) {
			BoxDiv& div = dynamic_cast<BoxDiv&>(base);
			raylib::Color a = div.GetColor();
			
			if (!hoverAudio.IsPlaying()) {
				hoverAudio.Play();
				hoverAudio.Update();
			}

			raylib::Color c(SKYBLUE);
			c.a = 100;
			div.SetBackColor(c);
		};

		txt.onNothing = [](BoxBase& base) {
			dynamic_cast<BoxDiv&>(base).SetBackColor(BLANK);
		};

		std::function<void()> playclick = [this]() {
			clickAudio.Play();
			clickAudio.Update();
		};

		//Text box for entering name, save pointer for further purposes
		{

			TextEdit tmp(txt);
			tmp.SetText("");
			if (nameBox == nullptr) {
				guiObjs.push_back(new TextEdit(tmp));
				nameBox = dynamic_cast<TextEdit*>(guiObjs.back());
			}
			else {
				guiObjs.push_back(nameBox);
			}
			startPage.childs.push_back(guiObjs.back());
		}


		//Pause screen stuff
		{
			TextBox tmp(txt);
			tmp.SetText("PLAY");
			tmp.onClickRelease = std::bind([this, playclick]() {playclick(); unsetFlag(GAME_PAUSED); });
			guiObjs.push_back(new TextBox(tmp));
			pausePage.childs.push_back(guiObjs.back());
		}

		{
			TextBox tmp(txt);
			tmp.SetText("HIGH SCORES");
			tmp.onClickRelease = std::bind([this, playclick]() {playclick(); setFlag(GAME_SCORES); });
			guiObjs.push_back(new TextBox(tmp));
			pausePage.childs.push_back(guiObjs.back());
		}

		{
			TextBox tmp(txt);
			tmp.SetText("SETTINGS");
			tmp.onClickRelease = std::bind([this, playclick]() {playclick(); setFlag(GAME_SETTINGS); });
			guiObjs.push_back(new TextBox(tmp));
			pausePage.childs.push_back(guiObjs.back());
		}

		{
			TextBox tmp(txt);
			tmp.SetText("EXIT");
			tmp.onClickRelease = std::bind([this, playclick]() {playclick(); setFlag(GAME_QUIT); });
			guiObjs.push_back(new TextBox(tmp));
			pausePage.childs.push_back(guiObjs.back());
		}

		//Settings page stuff
		{
			BoxDiv contain(settingsPage);

			{
				TextBox tmp(txt);
				tmp.onHover = [](BoxBase&) {};
				tmp.SetText("WINDOW SIZE : ");
				tmp.SetBorder(0);
				guiObjs.push_back(new TextBox(tmp));
				contain.childs.push_back(guiObjs.back());
			}
			
			{
				TextBox tmp(txt);
				tmp.SetText("FULL SCREEN");
				tmp.onClickRelease = [this,playclick](BoxBase&) {
					playclick();
					gameFlags.at(FULL_SCREEN) = true;
					gameFlags.at(GAME_RESIZE) = true;
					screenHeight = GetMonitorHeight(GetCurrentMonitor());
					screenWidth= GetMonitorWidth(GetCurrentMonitor());
					window.SetSize(screenWidth, screenHeight);
					window.SetFullscreen(true);
				};
				guiObjs.push_back(new TextBox(tmp));
				contain.childs.push_back(guiObjs.back());
			}
			std::function<void( int, int)> setsize = [this,playclick]( int w, int h) {
				playclick();
				gameFlags.at(FULL_SCREEN) = false;
				if (screenHeight == h && screenWidth == w)
					return;
				gameFlags.at(GAME_RESIZE) = true;
				screenHeight = h;
				screenWidth = w;
				window.SetFullscreen(false);
				window.SetSize(w, h);
			};
			{
				TextBox tmp(txt);
				tmp.SetText("1280x960");
				tmp.onClickRelease = std::bind(setsize, 1280, 960);
				guiObjs.push_back(new TextBox(tmp));
				contain.childs.push_back(guiObjs.back());
			}
			{
				TextBox tmp(txt);
				tmp.SetText("1280x720");
				tmp.onClickRelease = std::bind(setsize, 1280, 720);
				guiObjs.push_back(new TextBox(tmp));
				contain.childs.push_back(guiObjs.back());
			}
			{
				TextBox tmp(txt);
				tmp.SetText("800x480");
				tmp.onClickRelease = std::bind(setsize, 800, 480);
				guiObjs.push_back(new TextBox(tmp));
				contain.childs.push_back(guiObjs.back());
			}
			{
				TextBox tmp(txt);
				tmp.SetText("400x200");
				tmp.onClickRelease = std::bind(setsize, 400, 200);
				guiObjs.push_back(new TextBox(tmp));
				contain.childs.push_back(guiObjs.back());
			}
			contain.autoVertical = false;
			guiObjs.push_back(new BoxDiv(contain));
			settingsPage.childs.push_back(guiObjs.back());
		}

		{
			TextBox tmp(txt);
			tmp.SetText("BACK");
			tmp.onClickRelease = std::bind([this, playclick]() {playclick(); unsetFlag(GAME_SETTINGS); });
			guiObjs.push_back(new TextBox(tmp));
			settingsPage.childs.push_back(guiObjs.back());
		}
		
		
		//Scores page Stuff
		//Score box, save for later too , being dynamic
		{
			TextBox tmp(txt);
			tmp.onHover = [](BoxBase&) {};
			tmp.SetText("Score : " + std::to_string(score));
			tmp.SetBorder(0);
			guiObjs.push_back(new TextBox(tmp));
			scorePage.childs.push_back(guiObjs.back());
			scoreBox = dynamic_cast<TextBox*>(guiObjs.back());
		}
		{
			BoxDiv tempdiv1(scorePage);
			BoxDiv tempdiv2(scorePage);

			for (std::pair<const float,std::string>& x : scores) {

				TextBox tmp1(txt);
				tmp1.onHover = [](BoxBase&) {};
				tmp1.SetText(x.second);
				guiObjs.push_back(new TextBox(tmp1));
				tempdiv1.childs.push_back(guiObjs.back());
				
				TextBox tmp2(txt);
				tmp2.onHover = [](BoxBase&) {};
				tmp2.SetText(std::to_string(x.first));
				guiObjs.push_back(new TextBox(tmp2));
				tempdiv2.childs.push_back(guiObjs.back());

			}
			BoxDiv newDiv(scorePage);
			newDiv.SetBorder((5 * window.GetSize().x / GetMonitorWidth(GetCurrentMonitor())));
			newDiv.autoVertical = false;
			
			guiObjs.push_back(new BoxDiv(tempdiv1));
			newDiv.childs.push_back(guiObjs.back());
			guiObjs.push_back(new BoxDiv(tempdiv2));
			newDiv.childs.push_back(guiObjs.back());

			guiObjs.push_back(new BoxDiv(newDiv));
			scorePage.childs.push_back(guiObjs.back());
		} 
		{
			TextBox tmp(txt);
			tmp.SetText("BACK");
			tmp.onClickRelease = std::bind([this, playclick]() {playclick(); unsetFlag(GAME_SCORES); });
			guiObjs.push_back(new TextBox(tmp));
			scorePage.childs.push_back(guiObjs.back());
		}
		
		//Quit page Stuff
		//Use same score box as above
		{
			outPage.childs.push_back(scoreBox);
		}

		{
			TextBox tmp(txt);
			tmp.SetText("RESTART");
			tmp.onClickRelease = [this](BoxBase&) {
				unsetFlag(GAME_OVER);
				setFlag(GAME_START);
				setFlag(GAME_RESET);
			};
			guiObjs.push_back(new TextBox(tmp));
			outPage.childs.push_back(guiObjs.back());
		}

		{
			TextBox tmp(txt);
			tmp.SetText("QUIT");
			tmp.onClickRelease = std::bind([this, playclick]() {playclick(); setFlag(GAME_QUIT); });
			guiObjs.push_back(new TextBox(tmp));
			outPage.childs.push_back(guiObjs.back());
		}

		outPage.autoVertical = false;

		//Sizing start page
		windiv.childs.push_back(&startPage);
		windiv.packByContent();
		windiv.setMouse(&mouse);
		windiv.childs.pop_back();

		//Sizing pause page
		windiv.childs.push_back(&pausePage);
		windiv.packByContent();
		windiv.setMouse(&mouse);
		windiv.childs.pop_back();

		//Sizing settings page
		windiv.childs.push_back(&settingsPage);
		windiv.packByContent();
		windiv.setMouse(&mouse);
		windiv.childs.pop_back();

		//Sizing scores page
		windiv.childs.push_back(&scorePage);
		windiv.packByContent();
		windiv.setMouse(&mouse);
		windiv.childs.pop_back();


		//Sizing quit page
		windiv.childs.push_back(&outPage);
		windiv.packByContent();
		windiv.setMouse(&mouse);
		windiv.childs.pop_back();

	}

	void run() {

		while (!gameFlags.at(GAME_QUIT)&&!window.ShouldClose()) {
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
		if (gameFlags.at(GAME_RESIZE))
			resetUI();

		//Do stuff if game is not playable
		if (gameFlags.at(GAME_PAUSED) || gameFlags.at(GAME_START) || gameFlags.at(GAME_OVER)) {
			
			if (mouse.isCursorHidden()) {
				mouse.showCursor().enableCursor();
			}
			if (gameFlags.at(GAME_START)) {
				startPage.callActions();
				if (IsKeyReleased(KEY_ENTER)) {
					gameFlags.at(GAME_START) = false;
					gameFlags.at(GAME_PAUSED) = true;
				}
			}
			else if (gameFlags.at(GAME_PAUSED)) {
				if (gameFlags.at(GAME_SETTINGS))
					settingsPage.callActions();
				else if (gameFlags.at(GAME_SCORES))
					scorePage.callActions();
				else
					pausePage.callActions();
			}
			else if (gameFlags.at(GAME_OVER))
				outPage.callActions();

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
							gameFlags.at(GAME_RESET) = true;
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
				scores.insert(std::pair<float, std::string>(score, nameBox->GetText()));
				scoreBox->SetText(std::string("Score : ") + std::to_string(score));
				scoreBox->packByContent();
			}
			else if (gameFlags.at(GAME_START)) {
				if (gameFlags.at(GAME_RESET)) {
					score = 0;
					windSpeed = 0;
					arr.reset();
					target.reset();
					resetUI();
					gameFlags.at(GAME_RESET) = false;
				}
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
		if (parallelRun != nullptr) {
			parallelRun->join();
			delete parallelRun;
		}
	}
private:

	void drawGameScreen() {

		if (startAudio.IsPlaying()) {
			startAudio.Stop();
		}
		if (!gamesAudio.IsPlaying()) {
			gamesAudio.Seek(7);
			gamesAudio.Play();
		}
		gamesAudio.Update();

		window.BeginDrawing();
		window.ClearBackground(SKYBLUE);

		gameImgTex.Draw(Vec2(0, 0));
		currCam->BeginMode();

		
		arr.draw();
		target.draw();
		
		bowModel.get_this_model().Draw(Vec3(0, 0, 0), 1.0f, arr.mCol);


		currCam->EndMode();
		if (!gameFlags.at(ARROW_RELEASED)) {
			Vec2 circle = currCam->GetWorldToScreen(target.getProj(arr.getFront(), arr.getHead()));
			DrawCircleLines(circle.x, circle.y, 10, RED);
			DrawLine(circle.x - 5, circle.y, circle.x + 5, circle.y, RED);
			DrawLine(circle.x, circle.y - 5, circle.x, circle.y + 5, RED);
		}
		std::stringstream ss;

		ss << "Wind : " << windSpeed;
		ss << "\nScore " << score << std::endl;
		DrawText(ss.str().c_str(), window.GetWidth()/2.5,
			10*window.GetHeight()/GetMonitorHeight(GetCurrentMonitor()),
			50 * window.GetHeight() / GetMonitorHeight(GetCurrentMonitor()), DARKBLUE);

		window.EndDrawing();

	};

	void drawPauseScreen() {

		if (gamesAudio.IsPlaying()) {
			gamesAudio.Stop();
		}
		if (!startAudio.IsPlaying()) {
			startAudio.Play();
		}
		startAudio.Update();

		window.BeginDrawing();
		window.ClearBackground();
		pauseImgTex.Draw(Vec2(0, 0));
		if (gameFlags.at(GAME_PAUSED)) {
			if (gameFlags.at(GAME_SETTINGS))
				settingsPage.draw();
			else if (gameFlags.at(GAME_QUIT))
				outPage.draw();
			else if (gameFlags.at(GAME_SCORES)) {
				BoxDiv glob = getWindowDiv();
				glob.childs.push_back(&scorePage);
				glob.packChildren();
				scorePage.draw();
			}
			else
				pausePage.draw();
		}

		window.EndDrawing();
	};

	void drawGameOver() {

		if (gamesAudio.IsPlaying()) {
			gamesAudio.Stop();
		}
		if (!startAudio.IsPlaying()) {
			startAudio.Play();
		}
		startAudio.Update();

		window.BeginDrawing();
		window.ClearBackground();
		pauseImgTex.Draw(Vec2(0, 0));
		BoxDiv glob = getWindowDiv();
		glob.childs.push_back(&outPage);
		glob.packChildren();
		outPage.draw();

		window.EndDrawing();

	}

	void drawGameStart() {

		if (gamesAudio.IsPlaying()) {
			gamesAudio.Stop();
		}
		if (!startAudio.IsPlaying()) {
			startAudio.Play();
		}
		startAudio.Update();

		window.BeginDrawing();
		window.ClearBackground();


		startImgTex.Draw(Vec2(0, 0));
		startPage.draw();


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

	//Resources load from files
	raylib::Image startImg;
	raylib::Texture2D startImgTex;
	
	raylib::Image pauseImg;
	raylib::Texture2D pauseImgTex;
	
	raylib::Image gameImg;
	raylib::Texture2D gameImgTex;

	

	//Divisions for each pages to be displayed
	BoxDiv startPage, pausePage, settingsPage, scorePage, outPage;

	//Pointer of text field for name
	TextEdit* nameBox;
	TextBox* scoreBox;

	//Map of scores and names plus a helful greater than comparator
	class GreaterThan {
	public:
		bool operator()(float a, float b) const {
			return b < a;
		}
	};
	std::map<float, std::string, std::function<bool(float, float)>> scores;

	//Vector of gui objects pointer dynamically created
	std::vector<BoxBase*> guiObjs;

	//Sound effects objects
	raylib::Music startAudio;
	raylib::Music gamesAudio;
	raylib::Music hoverAudio;
	raylib::Music clickAudio;

	//An extra thread for loading objects simultaneously
	std::thread* parallelRun = nullptr;

	double collTimeElapsed = 0;
	const double CoolDown = 3;

	double arrStretchTime = 0;
	
	double windSpeed = 0;
	double windTimer = 0;

	std::array<bool, FLAG_FULL> gameFlags;

	unsigned score = 0;

	Target target;

	
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

	return 0;

}
