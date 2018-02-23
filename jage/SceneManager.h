#pragma once

#include <memory>
#include <stack>

#include <SFGUI/Window.hpp>

#include "Math.h"

// Base 
class Scene
{
public:
    virtual ~Scene() {}

    // Is called when this Scene object is added to Game scenes stack
	virtual void onInit() {}

    // Is called when this Scene is removed from Game scenes stack
    virtual void onClose() {}

    // Is called when new scene is added without deleting current
    virtual void onLeave() {}

    // Is called when the scene which is next on the stack is deleted
    virtual void onReturn() {}


    // Is called every frame before draw function
    virtual void onUpdate(const float dt) {}

    // Is called every frame after update function
    virtual void onDraw(const float dt) {}


    // Is called when window is resized
    virtual void onResize(const vec2& windowSize) {}

    // Is called when window loses focus
    virtual void onFocusLost() {}

    // Is called when window gains focus
    virtual void onFocusGained() {}

protected:
	friend class Core;

	sfg::Window::Ptr m_guiWindow;
};


//
class SceneManager
{
public:
	// Adds new scene on top of the stack
	// T - Scene child class type
	// Args - Scene child class constructor arguments
	template <class T, class... Args>
	static void addScene(Args&&... args)
	{
		static_assert(std::is_base_of<Scene, T>::value, 
			"template parameter in SceneManager::addScene accepts only classes derived from Scene");

		if (!m_scenes.empty()) {
			m_scenes.top()->onLeave();
		}

		std::unique_ptr<Scene> scene(new T(std::forward(args)...));
		scene->onInit();
		m_scenes.push(std::move(scene));
	}

	// Deletes top scene and add new scene at once
	// T - Scene child class type
	// Args - Scene child class constructor arguments
	template <class T, class... Args>
	static void changeScene(Args&&... args)
	{
		static_assert(std::is_base_of<Scene, T>::value,
			"template parameter in SceneManager::addScene accepts only classes derived from Scene");

		if (!m_scenes.empty()) {
			m_scenes.top()->onClose();
			m_scenes.pop();
		}

		std::unique_ptr<Scene> scene(new T(std::forward(args)...));
		scene->onInit();
		m_scenes.push(std::move(scene));
	}

	// Removes scene on top of the stack
	static void deleteScene();

	// Returns current scene if it exists, otherwise nullptr
	static Scene* getCurrentScene();

	// Returns true if there is at least one scene in stack
	static bool hasScenes() { return !m_scenes.empty(); }

	// Removes all scenes
	static void close();

private:
	static std::stack<std::unique_ptr<Scene>> m_scenes;
};
