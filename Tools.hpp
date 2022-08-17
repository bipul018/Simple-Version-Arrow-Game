#pragma once
#include "Bases.hpp"
#include <functional>

class MyMouse :public raylib::Mouse {

public:
	using Mouse::Mouse;
	Vec2 getDelta();
	MyMouse& limitInScreen();
	MyMouse& hideCursor();
	MyMouse& showCursor();
	bool isCursorHidden();
	MyMouse& disableCursor();
	MyMouse& enableCursor();
private:
};

class MyCamera :public raylib::Camera3D{
public:
	using Camera3D::Camera3D;

	MyCamera& operator = (const MyCamera&) = default;

	void setLookDir(Vec3 look);

	Vec3 getLookDir() const;

	MyCamera& Update();

	Vec2 getDelAngle() const;

	MyCamera& lookAt(GameModel::State state, Vec3 relativeDir);

	MyCamera& lookAt(GameModel::State state, Vec3 relativeDir, Vec3 manualUp);

	MyCamera& changeAngles(Vec2 delAngle);

	void SetMouse(MyMouse* mptr);

	bool isLikeFirstPerson = false;

//private:
	MyMouse* mouseptr = nullptr;

	//In radians
	Vec2 maxAngles = Vec2(PI / 3, PI / 2.02);
	Vec2 screenAngles = Vec2(PI / 4, PI / 2);
	Vec2 currAngles = Vec2(0, 0);

};


class BoxBase {

public:
	BoxBase(Vec2 position = Vec2(0, 0),Vec2 size = Vec2(0,0));
	virtual ~BoxBase() = default;
	virtual void setSize(Vec2 size);
	virtual void setPosition(Vec2 pos);

	virtual Vec2 getSize() const;
	virtual Vec2 getPosition() const;

	virtual void setMouse(MyMouse *mouse);

	//Packs the child objects as per derived class type inside itself and resizes itself
	virtual BoxBase& packByContent() = 0;

	//Sets position acc to size of parent such that m_pos lies at center of box
	//but does not get outside the parent box
	virtual BoxBase& setDynamicPos(BoxBase& parent, double xfac,double yfac);

	//Sets relative size acc to parent's size
	virtual BoxBase& setDynamicSize(BoxBase& parent, double xfac, double yfac);

	virtual void draw()  = 0;

	//Calls the below listed functions on appropriate actions of mouse
	virtual void callActions();

	//Function pointers for click actions , by default all do nothing
	std::function<void(BoxBase&)> onClickPress  =[](BoxBase& b){};
	std::function<void(BoxBase&)> onClickDown   =[](BoxBase& b){};
	std::function<void(BoxBase&)> onClickRelease=[](BoxBase& b){};
	std::function<void(BoxBase&)> onHover       =[](BoxBase& b){};
	std::function<void(BoxBase&)> onNothing     =[](BoxBase& b){};

	MyMouse* mptr = nullptr;


private:
	Vec2 m_pos;
	Vec2 m_size;

};

class BoxDiv: virtual public BoxBase {
public:
	using BoxBase::BoxBase;
	void draw() ;

	//Packs itself and resizes itself by children
	BoxBase& packByContent();
	//virtual BoxDiv& packByContent();

	//Packs child's contents and arrange them properly
	virtual BoxDiv& packChildren() ;

	void callActions();

	//sets own and children mouse
	void setMouse(MyMouse* mouse);

	//some getters and setters
	GETTERSETTER(float, Border, m_border);
	GETTERSETTER(float, Padding, m_padding);
	GETTERSETTER(raylib::Color, Color, m_Col);
	GETTERSETTER(raylib::Color, BackColor, m_Back);
	std::vector<BoxBase*> childs;
	bool autoPackChild = true;
	bool autoResizeWidth = true;
private:
	//To not draw overall border , set border to 0
	float m_border = 1;
	float m_padding = 1;
	raylib::Color m_Col = BLACK;
	raylib::Color m_Back = BLANK;
};


class MyText :virtual public BoxBase, private raylib::Text{
public:
	using BoxBase::BoxBase;
	using raylib::Text::GetColor;
	using raylib::Text::GetFont;
	using raylib::Text::GetFontSize;
	using raylib::Text::GetSpacing;
	using raylib::Text::GetText;
	using raylib::Text::SetColor;
	
	void SetText(std::string txt);
	void SetFontSize(float fsize);
	void SetSpacing(float space);
	void SetFont(raylib::Font fval);

	BoxBase& packByContent();
	//MyText& packByContent();
	void draw() ;
private:
	
};

class TextBox :  public MyText,  public BoxDiv {
public:
	using BoxDiv::BoxDiv;
	
	BoxBase& packByContent();
	TextBox& packChlidren();
	void draw();


private:

};

class TextEdit : public TextBox {
public:
	using TextBox::TextBox;
	TextEdit(const TextBox&);
	void callActions();



private:

};
