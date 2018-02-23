#pragma once

#include <vector>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#endif

// Static class to manipulate cursor state
class CursorManager
{
public:
	enum CursorStyle { 
		WAIT, 
		TEXT, 
		NORMAL, 
		HAND,

		STYLE_COUNT
	};

	// Initializes system objects
	// Must be called before using 'setStyle' function
	static void init(const sf::WindowHandle& window_handle);

	// Cleares up cursor system objects
	static void close();

	// Changes current cursor style to specified
	static void setStyle(const CursorStyle type);

	// Returns current cursor style
	static CursorStyle getStyle();

private:
	static CursorStyle m_currentStyle;
	static sf::WindowHandle m_windowHandle;

#ifdef SFML_SYSTEM_WINDOWS
	static std::vector<HCURSOR> m_cursorIcons;
#else
	static std::vector<XID> m_cursorIcons;
	static Display* m_display;
#endif
};