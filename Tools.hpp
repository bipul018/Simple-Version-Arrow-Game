#pragma once
#include "Bases.hpp"

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

	virtual void setSize(Vec2 size);
	virtual void setPosition(Vec2 pos);

	virtual Vec2 getSize() const;
	virtual Vec2 getPosition() const;

	//Packs the child objects as per derived class type inside itself and resizes itself
	virtual BoxBase& packByContent() = 0;

	//Sets position acc to size of parent such that m_pos lies at center of box
	//but does not get outside the parent box
	virtual BoxBase& setDynamicPos(BoxBase& parent, double xfac,double yfac);

	//Sets relative size acc to parent's size
	virtual BoxBase& setDynamicSize(BoxBase& parent, double xfac, double yfac);

	virtual void draw() const = 0;

private:
	Vec2 m_pos;
	Vec2 m_size;

};

class BoxDiv: public BoxBase {
public:
	using BoxBase::BoxBase;
	void draw() const;

	//Packs the child objects as per sequence vertically inside itself and resizes itself
	BoxDiv& packByContent();

	//Packs child's contents 
	BoxDiv& packChildren() ;

private:
	std::vector<BoxBase*> childs;

	//To not draw overall border , set border to 0
	float m_border = 1;
	float m_padding = 1;
	raylib::Color m_Col = BLACK;
};


class TextBox :public BoxBase, raylib::Text{
public:
	using BoxBase::BoxBase;
	
	Vec2 getSize() const;
	Vec2 getPosition() const;

	TextBox& packByContent();
	void draw() const;
private:

};
