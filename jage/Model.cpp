#include "Model.h"

#include <stack>

#include "Log.h"

Model::Model()
{
}

std::shared_ptr<GameObject> Model::createGameObject(EntityManager * manager, const std::string & name)
{
	if (manager == nullptr) { return nullptr; }

	std::shared_ptr<GameObject> modelRoot = manager->create();

	std::stack<GameObject*> gameObjects;
	gameObjects.push(modelRoot.get());

	std::stack<Node*> nodes;
	nodes.push(&m_rootNode);

	while (!nodes.empty()) {
		Node* modelNode = nodes.top();
		nodes.pop();

		GameObject* gameObject = gameObjects.top();
		gameObjects.pop();

		gameObject->setName(modelNode->name);
		gameObject->setTransformationMatrix(modelNode->localTransformation);

		if (modelNode->mesh != nullptr) {
			gameObject->assign<MeshComponent>(modelNode->mesh, *modelNode->material);
		}

		for (size_t i = 0; i < modelNode->children.size(); ++i) {
			std::shared_ptr<GameObject> childObject = manager->create();
			gameObject->addChild(childObject);

			gameObjects.push(childObject.get());
			nodes.push(&modelNode->children[i]);
		}
	}

	return modelRoot;
}
