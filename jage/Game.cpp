#include "Game.h"
#include "Core.h"

void Game::onInit()
{
	m_entityManager = std::make_shared<EntityManager>();

	// Creating systems
	m_renderingSystem = std::make_shared<RenderingSystem>();
	m_entityManager->registerSystem(m_renderingSystem);

	m_skySystem = std::make_shared<SkySystem>();
	m_entityManager->registerSystem(m_skySystem);

	// Loading models
	auto rootObject = m_entityManager->create();
	rootObject->setName("root");

	ResourceManager::bind<ModelFactory>("castle", "castle.fbx");
	auto castle = ResourceManager::get<Model>("castle")->createGameObject(m_entityManager.get(), "castle");
	castle->setPosition(15.0f, 0.0f, -15.0f);
	castle->setRotation(0.0f, glm::pi<float>() / 4.0f, 0.0f);
	rootObject->addChild(castle);

	ResourceManager::bind<ModelFactory>("baracks", "baracks.fbx");
	auto baracks = ResourceManager::get<Model>("baracks")->createGameObject(m_entityManager.get(), "baracks");
	baracks->setPosition(-15.0f, 0.0f, 15.0f);
	rootObject->addChild(baracks);

	ResourceManager::bind<ModelFactory>("ghost", "ghost.fbx");
	auto ghost = ResourceManager::get<Model>("ghost")->createGameObject(m_entityManager.get(), "ghost");
	rootObject->addChild(ghost);

	ResourceManager::bind<ModelFactory>("terrain", "terrain.fbx");
	auto terrain = ResourceManager::get<Model>("terrain")->createGameObject(m_entityManager.get(), "terrain");
	terrain->setScale(100.0f, 30.0f, 100.0f);

	auto terrainMesh = terrain->getChildren()[0]->getChildren()[0];
	if (terrainMesh->isValid() && terrainMesh->hasComponent<MeshComponent>()) {
		terrainMesh->getComponent<MeshComponent>()->getMaterial()->as<MeshMaterial>()->setUVScale(vec2(1000.0f, 1000.0f));
	}
	rootObject->addChild(terrain);

	auto sun = m_skySystem->createSun();
	sun->setPosition(10.0f, 5.0f, 10.0f);
	rootObject->addChild(sun);
	
	m_camera = m_entityManager->create();
	m_camera->setName("main_camera");
	m_camera->setPosition(0.0f, 5.0f, 10.0f);
	auto cameraComponent = m_camera->assign<CameraComponent>();

	// Initializing systems
	m_renderingSystem->setMainCamera(m_camera);

	m_fxaaMaterial = std::make_unique<FxaaMaterial>();
	m_renderingSystem->addPostProcess(0, m_fxaaMaterial.get());

	m_abberationMaterial = std::make_unique<AbberationMaterial>();
	m_renderingSystem->addPostProcess(1, m_abberationMaterial.get());

	m_skySystem->setSun(sun);
	m_skySystem->setTime(8, 0);

	onResize(toGLM((Core::getWindow().getSize())));
}

void Game::onClose()
{
}

void Game::onUpdate(const float dt)
{
	if (Input::getKeyDown(Key::Escape)) {
		SceneManager::deleteScene();
		return;
	}

	m_cameraController.update(dt, m_camera);

	m_renderingSystem->update(dt);
	m_skySystem->update(dt);
}

void Game::onResize(const ivec2& windowSize)
{
	m_entityManager->emit<Events::OnWindowResized>({ ivec2(windowSize) });
}