#include "Tools.hpp"

void MyCamera::setLookDir(Vec3 look) {
	if (isLikeFirstPerson)
		position = Vec3(target) - look;
	else
		target = look + position;
}

Vec3 MyCamera::getLookDir() const {
	return Vec3(target) - position;
}

MyCamera& MyCamera::Update() {
	return changeAngles(getDelAngle());
}

Vec2 MyCamera::getDelAngle() const {
	//Updates Based on our own mpuse derived one
	if (mouseptr == nullptr)
		return Vec2(0, 0);
	Vec2 delMouse = mouseptr->getDelta() * -1;


	Vec2 delAngle = screenAngles * delMouse / Vec2(screenWidth, screenHeight);
	if (maxAngles.x != 0)
		if (((delAngle.x + currAngles.x) <= -maxAngles.x) || ((delAngle.x + currAngles.x) >= maxAngles.x))
			delAngle.x = 0;
	if (maxAngles.y != 0)
		if (((delAngle.y + currAngles.y) <= -maxAngles.y) || ((delAngle.y + currAngles.y) >= maxAngles.y))
			delAngle.y = 0;
	return delAngle;
}

MyCamera& MyCamera::lookAt(GameModel::State state, Vec3 relativeDir) {
	return lookAt(state, relativeDir, state.up);
}

MyCamera& MyCamera::lookAt(GameModel::State state, Vec3 relativeDir, Vec3 manualUp) {
	target = state.pos;
	position = Vec3(target) +
		relativeDir.RotateByQuaternion(Vec4::FromVector3ToVector3(D_Front, state.front));
	up = manualUp;

	return *this;
}

MyCamera& MyCamera::changeAngles(Vec2 delAngle) {
	currAngles += delAngle;
	up = Vec3(up).Normalize();
	Vec3 front = getLookDir() - Vec3(up) * Vec3(up).DotProduct(getLookDir());
	front = front.Normalize();
	Vec3 right = front.CrossProduct(up);
	float len = getLookDir().Length();

	front = getLookDir().RotateByQuaternion(Vec4::FromAxisAngle(up, delAngle.x))
		.RotateByQuaternion(Vec4::FromAxisAngle(right, delAngle.y));

	setLookDir(front);
	return *this;
}

void MyCamera::SetMouse(MyMouse* mptr) {
	mouseptr = mptr;
}


Vec2 MyMouse::getDelta() {
	return  GetMouseDelta();
}

MyMouse& MyMouse::limitInScreen() {
	if (GetX() >= screenWidth)
		SetX(screenWidth - 1);
	if (GetY() >= screenHeight)
		SetY(screenHeight - 1);
	if (GetX() < 0)
		SetX(0);
	if (GetY() < 0)
		SetY(0);
	return *this;
}



MyMouse& MyMouse::hideCursor() {
	Vec2 pos = GetPosition();
	HideCursor();
	SetPosition(pos);
	return *this;
}

MyMouse& MyMouse::showCursor() {
	Vec2 pos = GetPosition();
	ShowCursor();
	SetPosition(pos);
	return *this;
}

bool MyMouse::isCursorHidden() {
	return IsCursorHidden();
}

MyMouse& MyMouse::disableCursor() {
	Vec2 pos = GetPosition();
	DisableCursor();
	SetPosition(pos);
	return *this;
}

MyMouse& MyMouse::enableCursor() {
	Vec2 pos = GetPosition();
	EnableCursor();
	SetPosition(pos);
	return *this;
}

BoxBase::BoxBase(Vec2 position, Vec2 size) {
	setPosition(position);
	setSize(size);
}

void BoxBase::setSize(Vec2 size) {
	m_size = size;
	if (size.x < 0) {
		m_pos.x -= size.x;
		size.x *= -1;
	}
	if (size.y < 0) {
		m_pos.y -= size.y;
		size.y *= -1;
	}
}

void BoxBase::setPosition(Vec2 pos) {
	m_pos = pos;
}

Vec2 BoxBase::getSize() const {
	return m_size;
}

Vec2 BoxBase::getPosition() const {
	return m_pos;
}

void BoxBase::setMouse(MyMouse *mouse) {
	mptr = mouse;
}

BoxBase& BoxBase::setDynamicPos(BoxBase& parent, double xfac, double yfac) {
	Vec2 size = getSize();
	Vec2 pos = parent.getPosition() + parent.getSize() * Vec2(xfac, yfac) - size * 0.5;
	if (pos.y > (parent.getPosition().y + parent.getSize().y))
		pos.y = parent.getPosition().y + parent.getSize().y - size.y;
	if (pos.x > (parent.getPosition().x + parent.getSize().x))
		pos.x = parent.getPosition().x + parent.getSize().x - size.x;

	if (pos.x < parent.getPosition().x)
		pos.x = parent.getPosition().x;
	if (pos.y < parent.getPosition().y)
		pos.y = parent.getPosition().y;

	setPosition(pos);
	setSize(size);

	return *this;
}

BoxBase& BoxBase::setDynamicSize(BoxBase& parent, double xfac, double yfac) {



	return *this;
}

void BoxBase::callActions() {
	if (mptr == nullptr)
		return;

	bool isMouseOver = (mptr->GetX() > m_pos.x && mptr->GetY() > m_pos.y) &&
		(mptr->GetX() < (m_pos.x + m_size.x) && mptr->GetY() < (m_pos.y + m_size.y));
	if (isMouseOver) {

		if (mptr->IsButtonPressed(MOUSE_LEFT_BUTTON))
			onClickDown(*this);
		else if (mptr->IsButtonReleased(MOUSE_LEFT_BUTTON))
			onClickRelease(*this);
		else if (mptr->IsButtonDown(MOUSE_LEFT_BUTTON))
			onClickDown(*this);
		else
			onHover(*this);
	}
	else
		onNothing(*this);

}

void BoxDiv::draw()  {
	if (m_border > 0) {
		raylib::Rectangle r(getPosition(), getSize());
		r.Draw(m_Back);
		r.DrawLines(m_Col, m_border);
	}
	for (BoxBase* ptr : childs)
		ptr->draw();
	
}

BoxDiv& BoxDiv::packByContent() {
	if (childs.empty())
		return *this;

	if (autoPackChild)
		packChildren();

	Vec2 pos = childs.at(0)->getPosition();
	Vec2 size = Vec2(0, 0);
	for (BoxBase* ptr : childs) {
		Vec2 temp = ptr->getPosition();
		if (temp.x < pos.x)
			pos.x = temp.x;
		if (temp.y < pos.y)
			pos.y = temp.y;
	}
	
	for (BoxBase* ptr : childs) {
		Vec2 temp = ptr->getPosition() + ptr->getSize() - pos;
		if (temp.x > size.x)
			size.x = temp.x;
		if (temp.y > size.y)
			size.y = temp.y;
	}
	pos = pos - Vec2(m_padding + m_border, m_padding + m_border);
	size = size + Vec2(m_padding + m_border, m_padding + m_border)*2;
	setPosition(pos);
	setSize(size);

	return *this;
}

BoxDiv& BoxDiv::packChildren()  {
	double maxy = 0;
	for (BoxBase* ptr : childs) {
		ptr->packByContent();
		maxy += ptr->getSize().y + m_padding;
	}
	float y = BoxDiv(getPosition(), Vec2(0,maxy)).setDynamicPos(*this, 0.5, 0.5).getPosition().y;
	for (BoxBase* ptr : childs) {
		ptr->setDynamicPos(*this, 0.5, 0.5);
		ptr->setPosition(Vec2(ptr->getPosition().x, y));
		y += ptr->getSize().y + m_padding;
	}
	return *this;
}

void BoxDiv::callActions() {
	BoxBase::callActions();
	for (BoxBase* ptr : childs)
		ptr->callActions();
}

void BoxDiv::setMouse(MyMouse* mouse) {
	mptr = mouse;
	for (BoxBase* ptr : childs)
		ptr->setMouse(mouse);
}


void TextBox::SetText(std::string txt) {
	raylib::Text::SetText(txt);
}

void TextBox::SetFontSize(float fsize) {
	raylib::Text::SetFontSize(fsize);
}

void TextBox::SetSpacing(float space) {
	raylib::Text::SetSpacing(space);
}

void TextBox::SetFont(raylib::Font fval) {
	raylib::Text::SetFont(fval);
}

TextBox& TextBox::packByContent() {
	setSize(MeasureEx());
	return *this;
}

void TextBox::draw()  {
	Draw(getPosition());
}
