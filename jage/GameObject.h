#pragma once

#include <typeindex>
#include <vector>
#include <memory>
#include <string>
#include <map>

#include "EntityManager.h"
#include "Math.h"

class GameObject
{
public:
	GameObject(EntityManager* manager, EntityId id);
	virtual ~GameObject();

	virtual void setActive(bool active);
	bool isActive() const;

	void setName(const std::string& name);
	std::string getName() const;

	void setTag(const std::string& tag);
	std::string getTag() const;


	const EntityManager* getEntityManager() const;

	EntityId getId() const;

	bool isPendingDestroy() const;

	// Tree structure functions
	std::shared_ptr<GameObject> clone();

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
		return m_manager->assign<T>(m_id, std::forward<Args>(args)...);
	}

	template<typename T>
	ComponentHandle<T> assignFromCopy(const T& component)
	{
		return m_manager->assign<T>(m_id, std::forward<const T&>(component));
	}

	template<typename T, typename... Args>
	ComponentHandle<T> replace(Args&&... args)
	{
		ComponentHandle<T> handle = getComponent<T>();
		if (handle) {
			(*handle.get()) = T(std::forward<Args>(args)...);
		}
		else {
			handle = m_manager->assign<T>(m_id, std::forward<Args>(args)...);
		}

		return handle;
	}

	template<typename T>
	void remove()
	{
		m_manager->remove<T>(m_id);
	}
	
	template<typename T>
	ComponentHandle<T> getComponent()
	{
		return m_manager->getComponent<T>(m_id);
	}

	template<typename T>
	const ComponentHandle<T> getComponent() const
	{
		return const_cast<const EntityManager*>(m_manager)->getComponent<const T>(m_id);
	}

	template<typename... Ts>
	std::tuple<ComponentHandle<Ts>...> getComponents()
	{
		return m_manager->getComponents<Ts...>(m_id);
	}

	template<typename... Ts>
	std::tuple<ComponentHandle<const Ts>...> getComponents() const
	{
		return const_cast<const EntityManager*>(m_manager)->getComponent<const Ts...>(m_id);
	}

	template<typename T>
	bool hasComponent() const
	{
		return m_manager->hasComponent<T>(m_id);
	}

	template<typename T, typename... Ts>
	void unpack(ComponentHandle<T>& c, ComponentHandle<Ts>&... cs)
	{
		return m_manager->unpack(m_id, cs...);
	}

	std::bitset<MAX_COMPONENTS> getComponentMask() const;

	void destroy();
	void invalidate();
	bool isValid() const;

	bool operator==(const GameObject& other) const;
	bool operator!=(const GameObject& other) const;
	bool operator<(const GameObject& other) const;


	// Transformation functions
	mat4 getGlobalTransformation() const;

	void setTransformationMatrix(const mat4& transformation);
	mat4 getTransformationMatrix() const;

	mat4 getPositionMatrix() const;
	mat4 getPositionMatrixInversed() const;

	mat4 getRotationMatrix() const;
	mat4 getRotationMatrixInversed() const;

	mat4 getScaleMatrix() const;
	mat4 getScaleMatrixInversed() const;

	void move(float x, float y, float z);
	void move(const vec3& vector);
	void setPosition(float x, float y, float z);
	void setPosition(const vec3& position);
	vec3 getPosition() const;

	void rotate(float x, float y, float z);
	void rotate(const vec3& eulerAngles);
	void rotate(const quat& rotation);
	void setRotation(float x, float y, float z);
	void setRotation(const vec3& eulerAngles);
	void setRotation(const quat& rotation);
	quat getRotation() const;
	vec3 getEulerRotation() const;

	void scale(float s);
	void scale(float x, float y, float z);
	void scale(const vec3& s);
	void setScale(float s);
	void setScale(float x, float y, float z);
	void setScale(const vec3& s);
	vec3 getScale() const;

	vec3 getDirectionFront() const;
	vec3 getDirectionRight() const;
	vec3 getDirectionUp() const;

protected:
	std::string m_name;
	std::string m_tag;

	bool m_isActive;

	GameObject* m_parent;
	std::vector<std::shared_ptr<GameObject>> m_children;

private:
	friend class EntityManager;

	void updatePosition() const;
	void updateRotation() const;
	void updateScale() const;

	static std::shared_ptr<GameObject> create(EntityManager* manager, EntityId id);

	EntityManager* m_manager;
	EntityId m_id;

	bool m_isPendingDestroy;

	vec3 m_position;
	quat m_rotation;
	vec3 m_scale;

	mutable vec3 m_directionFront;
	mutable vec3 m_directionRight;
	mutable vec3 m_directionUp;

	mutable mat4 m_transformation;

	mutable mat4 m_positionMatrix;
	mutable mat4 m_positionMatrixInversed;

	mutable mat4 m_rotationMatrix;
	mutable mat4 m_rotationMatrixInversed;

	mutable mat4 m_scaleMatrix;
	mutable mat4 m_scaleMatrixInversed;

	mutable bool m_positionChanged;
	mutable bool m_rotationChanged;
	mutable bool m_scaleChanged;

	mutable bool m_transformationChanged;
};