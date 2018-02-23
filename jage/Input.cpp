#include "Input.h"

#include "Core.h"

std::bitset<Key::KeyCount> Input::m_currentKeyStates;
std::bitset<Key::KeyCount> Input::m_lastKeyStates;

std::bitset<MouseButton::ButtonCount> Input::m_currentMouseStates;
std::bitset<MouseButton::ButtonCount> Input::m_lastMouseStates;

sf::Vector2i Input::m_currentCursorPosition = sf::Mouse::getPosition();
sf::Vector2i Input::m_lastCursorPosition = sf::Mouse::getPosition();

bool Input::getKey(Key keyCode)
{
	if (keyCode > Key::Unknown && keyCode < Key::KeyCount) {
		return m_currentKeyStates.test(static_cast<size_t>(keyCode));
	}
	else {
		return false;
	}
}

bool Input::getKeyDown(Key keyCode)
{
	if (keyCode > Key::Unknown && keyCode < Key::KeyCount) {
		return !m_lastKeyStates.test(static_cast<size_t>(keyCode)) && 
				m_currentKeyStates.test(static_cast<size_t>(keyCode));
	}
	else {
		return false;
	}
}

bool Input::getKeyUp(Key keyCode)
{
	if (keyCode > Key::Unknown && keyCode < Key::KeyCount) {
		return m_lastKeyStates.test(static_cast<size_t>(keyCode)) && 
				!m_currentKeyStates.test(static_cast<size_t>(keyCode));
	}
	else {
		return false;
	}
}

bool Input::getAnyKey()
{
	return m_currentKeyStates.any();
}

bool Input::getAnyKeyDown()
{
	return !m_lastKeyStates.any() && m_currentKeyStates.any();
}

bool Input::getAnyKeyUp()
{
	return m_lastKeyStates.any() && !m_currentKeyStates.any();
}

bool Input::getMouse(MouseButton mouseCode)
{
	if (mouseCode < MouseButton::ButtonCount) {
		return m_currentMouseStates.test(static_cast<size_t>(mouseCode));
	}
	else {
		return false;
	}
}

bool Input::getMouseDown(MouseButton mouseCode)
{
	if (mouseCode < MouseButton::ButtonCount) {
		return !m_lastMouseStates.test(static_cast<size_t>(mouseCode)) && 
				m_currentMouseStates.test(static_cast<size_t>(mouseCode));
	}
	else {
		return false;
	}
}

bool Input::getMouseUp(MouseButton mouseCode)
{
	if (mouseCode < MouseButton::ButtonCount) {
		return m_lastMouseStates.test(static_cast<size_t>(mouseCode)) && 
				!m_currentMouseStates.test(static_cast<size_t>(mouseCode));
	}
	else {
		return false;
	}
}

bool Input::getAnyMouse()
{
	return m_currentMouseStates.any();
}

bool Input::getAnyMouseDown()
{
	return !m_lastMouseStates.any() && m_currentMouseStates.any();
}

bool Input::getAnyMouseUp()
{
	return m_lastMouseStates.any() && !m_currentMouseStates.any();
}

void Input::setMousePosition(const vec2 & pos)
{
	m_currentCursorPosition = sf::Vector2i(static_cast<int>(pos.x), static_cast<int>(pos.y));
	sf::Mouse::setPosition(m_currentCursorPosition, Core::getWindow());
}

vec2 Input::getMousePosition()
{
	return vec2(static_cast<float>(m_currentCursorPosition.x), 
		static_cast<float>(m_currentCursorPosition.y));
}

vec2 Input::getMouseDeltaPosition()
{
	sf::Vector2i temp = m_currentCursorPosition - m_lastCursorPosition;
	return vec2(static_cast<float>(temp.x), static_cast<float>(temp.y));
}

void Input::setCursorVisible(bool visible)
{
	Core::getWindow().setMouseCursorVisible(visible);
}

void Input::update()
{
	m_lastKeyStates = m_currentKeyStates;
	m_lastMouseStates = m_currentMouseStates;
	m_lastCursorPosition = m_currentCursorPosition;
}

void Input::handleEvent(const sf::Event & e)
{
	switch (e.type)
	{
	case sf::Event::MouseMoved:
		m_currentCursorPosition.x = e.mouseMove.x;
		m_currentCursorPosition.y = e.mouseMove.y;
		break;

	case sf::Event::KeyPressed:
		if (e.key.code > Key::Unknown && e.key.code < Key::KeyCount) {
			m_currentKeyStates.set(e.key.code, true);
		}
		break;

	case sf::Event::KeyReleased:
		if (e.key.code > Key::Unknown && e.key.code < Key::KeyCount) {
			m_currentKeyStates.set(e.key.code, false);
		}
		break;

	case sf::Event::MouseButtonPressed:
		if (e.mouseButton.button < MouseButton::ButtonCount) {
			m_currentMouseStates.set(e.mouseButton.button, true);
		}
		break;

	case sf::Event::MouseButtonReleased:
		if (e.mouseButton.button < MouseButton::ButtonCount) {
			m_currentMouseStates.set(e.mouseButton.button, false);
		}
		break;
	default:
		break;
	}
}
