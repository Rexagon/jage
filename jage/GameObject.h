#pragma once

#include <typeindex>
#include <vector>
#include <memory>
#include <string>
#include <map>

#include "EntityManager.h"
#include "Transformable.h"

class GameObject : public Transformable
{
public:
	virtual ~GameObject();

	virtual void onUpdate(const float dt) {}
	virtual void onDraw() {}
	
	virtual std::shared_ptr<GameObject> duplicate();

	virtual void setActive(bool active);
	bool isActive() const;

	void setName(const std::string& name);
	std::string getName() const;

	void setTag(const std::string& tag);
	std::string getTag() const;

	mat4 getGlobalTransformation() const;

	const EntityManager* getEntityManager() const;

	EntityId getId() const;

	bool isPendingDestroy() const;

	// Tree structure functions
	void setParent(GameObject* parent);
	GameObject* getParent() const;
	
	void addChild(std::shared_ptr<GameObject> node);

	GameObject* getChildByName(const std::string& name, bool deep = false);

	void deleteChildByName(const std::string& name, bool deep = false);

	std::shared_ptr<GameObject> detachChildByName(const std::string& name, bool deep = false);

	std::vector<std::shared_ptr<GameObject>>& getChildren();

	// Component functions
	template<typename T, typename... Args>
	ComponentHandle<T> assign(Args&&... args)
	{

	}

	bool operator==(const GameObject& other) const;
	bool operator!=(const GameObject& other) const;
	bool operator<(const GameObject& other) const;

protected:
	std::string m_name;
	std::string m_tag;

	bool m_isActive;

	GameObject* m_parent;
	std::vector<std::shared_ptr<GameObject>> m_children;

private:
	friend class EntityManager;

	static std::shared_ptr<GameObject> create(const EntityManager* manager, EntityId id);

	GameObject(const EntityManager* manager, EntityId id);

	const EntityManager* m_manager;
	EntityId m_id;

	bool m_isPendingDestroy;
};

typedef std::shared_ptr<GameObject> object_ptr;