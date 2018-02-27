#pragma once

#include <typeindex>
#include <vector>
#include <memory>
#include <string>
#include <map>

#include "Transformable.h"

class GameObject : public Transformable
{
public:

	GameObject(const std::string& name);
	virtual ~GameObject();

	virtual void onUpdate(const float dt) {}
	virtual void onDraw() {}
	
	virtual std::shared_ptr<GameObject> duplicate();

	virtual void setActive(bool active);
	bool isActive() const;

	virtual void setVisible(bool visible);
	bool isVisible() const;

	void setName(const std::string& name);
	std::string getName() const;

	void setTag(const std::string& tag);
	std::string getTag() const;

	mat4 getGlobalTransformation() const;


	// Tree structure functions
	void setParent(GameObject* parent);
	GameObject* getParent() const;
	
	void addChild(std::shared_ptr<GameObject> node);

	GameObject* getChildByName(const std::string& name, bool deep = false);

	void deleteChildByName(const std::string& name, bool deep = false);

	std::shared_ptr<GameObject> detachChildByName(const std::string& name, bool deep = false);

	std::vector<std::shared_ptr<GameObject>>& getChildren();

protected:
	std::string m_name;
	std::string m_tag;

	bool m_isActive;
	bool m_isVisible;

	GameObject* m_parent;
	std::vector<std::shared_ptr<GameObject>> m_children;
};