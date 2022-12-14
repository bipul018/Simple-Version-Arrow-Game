#include "GraphicsHelp.hpp"


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

void BoxBase::setMouse(MyMouse* mouse) {
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

void BoxDiv::draw() {
	raylib::Rectangle r(getPosition(), getSize());
	r.Draw(m_Back);
	if (m_border > 0) {
		r.DrawLines(m_Col, m_border);
	}
	for (BoxBase* ptr : childs)
		ptr->draw();

}

BoxBase& BoxDiv::packByContent() {
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
	size = size + Vec2(m_padding + m_border, m_padding + m_border) * 2;
	setPosition(pos);
	setSize(size);

	return *this;
}

BoxDiv& BoxDiv::packChildren() {
	double maxy = 0;
	double maxx = 0;
	for (BoxBase* ptr : childs) {
		ptr->packByContent();
		if (autoVertical) {
			maxy += ptr->getSize().y + m_padding;
			maxx = (ptr->getSize().x > maxx) ? ptr->getSize().x : maxx;
		}
		else {
			maxx += ptr->getSize().x + m_padding;
			maxy = (ptr->getSize().y > maxy) ? ptr->getSize().y : maxy;
		}
	}
	float y = 0;
	if (autoVertical)
		y = BoxDiv(getPosition(), Vec2(0, maxy)).setDynamicPos(*this, 0.5, 0.5).getPosition().y;
	else
		y = BoxDiv(getPosition(), Vec2(maxx, 0)).setDynamicPos(*this, 0.5, 0.5).getPosition().x;
	for (BoxBase* ptr : childs) {
		if (autoResizeDim) {
			if (autoVertical)
				ptr->setSize(Vec2(maxx, ptr->getSize().y));
			else
				ptr->setSize(Vec2(ptr->getSize().x, maxy));

		}
		ptr->setDynamicPos(*this, 0.5, 0.5);
		if (!autoVertical) {
			ptr->setPosition(Vec2(y, ptr->getPosition().y));
			y += ptr->getSize().x + m_padding;
		}
		else {
			ptr->setPosition(Vec2(ptr->getPosition().x, y));
			y += ptr->getSize().y + m_padding;
		}

	}
	return *this;
}

void BoxDiv::callActions() {
	for (BoxBase* ptr : childs)
		ptr->callActions();
	BoxBase::callActions();
}

void BoxDiv::setMouse(MyMouse* mouse) {
	mptr = mouse;
	for (BoxBase* ptr : childs)
		ptr->setMouse(mouse);
}


void MyText::SetText(std::string txt) {
	raylib::Text::SetText(txt);
}

void MyText::SetFontSize(float fsize) {
	raylib::Text::SetFontSize(fsize);
}

void MyText::SetSpacing(float space) {
	raylib::Text::SetSpacing(space);
}

void MyText::SetFont(raylib::Font fval) {
	raylib::Text::SetFont(fval);
}

BoxBase& MyText::packByContent() {
	setSize(MeasureEx());
	return *this;
}

void MyText::draw() {
	Draw(getPosition());
}

BoxBase& TextBox::packByContent() {
	MyText::packByContent();
	setPosition(getPosition() - Vec2(GetBorder() + GetPadding(), GetBorder() + GetPadding()));
	setSize(getSize() + Vec2(GetBorder() + GetPadding(), GetBorder() + GetPadding()) * 2);
	return *this;
}

TextBox& TextBox::packChlidren() {
	packByContent();
	return *this;
}

void TextBox::draw() {
	BoxDiv::draw();
	setPosition(getPosition() + Vec2(GetBorder() + GetPadding(), GetBorder() + GetPadding()));
	MyText::draw();
	setPosition(getPosition() - Vec2(GetBorder() + GetPadding(), GetBorder() + GetPadding()));

}

TextEdit::TextEdit(const TextBox& t) :TextBox(t) {
}

void TextEdit::callActions() {
	TextBox::callActions();
	if (IsKeyReleased(KEY_BACKSPACE) && !GetText().empty()) {
		std::string str = GetText();
		str.pop_back();
		SetText(str);
	}
	else
		for (int i = KEY_A; i <= KEY_Z; ++i) {
			if (IsKeyReleased(i)) {
				std::string str = GetText();
				str = str + static_cast<char>(i);
				if (MeasureTextEx(GetFont(), str.c_str(), GetFontSize(), GetSpacing()).x < (getSize().x - GetPadding() * 2 - GetBorder() * 2))
					SetText(str);
				break;
			}
		}

}
