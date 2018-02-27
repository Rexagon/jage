#include "ModelFactory.h"

#include <stack>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "FileManager.h"
#include "Log.h"

glm::mat4 toGLM(const aiMatrix4x4& value)
{
	glm::mat4 result;
	result[0] = glm::vec4(value[0][0], value[1][0], value[2][0], value[3][0]);
	result[1] = glm::vec4(value[0][1], value[1][1], value[2][1], value[3][1]);
	result[2] = glm::vec4(value[0][2], value[1][2], value[2][2], value[3][2]);
	result[3] = glm::vec4(value[0][3], value[1][3], value[2][3], value[3][3]);
	return result;
}

ModelFactory::ModelFactory(const std::string& filename) :
	AbstractFactory(tag<Model>{}), m_data(nullptr), m_filename(filename)
{
}

void * ModelFactory::load()
{
	if (m_data == nullptr) {
		std::unique_ptr<Model> model = std::make_unique<Model>();

		Assimp::Importer importer;

		std::string data = FileManager::open(m_filename);
		const aiScene* scene = importer.ReadFileFromMemory(data.c_str(), data.size(),
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			throw std::runtime_error("Unable to load model: \"" + m_assignedName + "\" (" + m_filename + 
				"). " + importer.GetErrorString());
		}

		// Loading textures

		// Loading meshes
		model->m_meshes.resize(scene->mNumMeshes);
		for (size_t i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* meshData = scene->mMeshes[i];

			std::vector<vec3> positions(meshData->mNumVertices);
			std::vector<vec2> texCoords(meshData->mNumVertices);
			std::vector<vec3> normals(meshData->mNumVertices);
			std::vector<unsigned int> indices;

			for (size_t j = 0; j < meshData->mNumVertices; ++j) {
				positions[j].x = meshData->mVertices[j].x;
				positions[j].y = meshData->mVertices[j].y;
				positions[j].z = meshData->mVertices[j].z;

				if (meshData->mNormals) {
					normals[j].x = meshData->mNormals[j].x;
					normals[j].y = meshData->mNormals[j].y;
					normals[j].z = meshData->mNormals[j].z;
				}

				if (meshData->mTextureCoords && meshData->mTextureCoords[0]) {
					texCoords[j].x = meshData->mTextureCoords[0][j].x;
					texCoords[j].y = meshData->mTextureCoords[0][j].y;
				}
			}

			indices.reserve(meshData->mNumFaces * 3);
			for (size_t j = 0; j < meshData->mNumFaces; ++j) {
				const aiFace& face = meshData->mFaces[j];
				for (size_t k = 0; k < face.mNumIndices; ++k) {
					indices.push_back(face.mIndices[k]);
				}
			}

			model->m_meshes[i].setName(meshData->mName.C_Str());
			model->m_meshes[i].init(positions, texCoords, normals, indices);
		}

		// Loading tree
		std::stack<aiNode*> modelTree;
		modelTree.push(scene->mRootNode);

		std::stack<GameObject*> resultTree;
		model->m_rootObject = std::make_shared<GameObject>(m_assignedName + "_root");
		resultTree.push(model->m_rootObject.get());

		while (!modelTree.empty()) {
			aiNode* nodeData = modelTree.top();
			modelTree.pop();

			GameObject* currentObject = resultTree.top();
			resultTree.pop();

			currentObject->setName(nodeData->mName.C_Str());

			currentObject->getTransformation().setLocalTransform(toGLM(nodeData->mTransformation));

			for (size_t i = 0; i < nodeData->mNumChildren; ++i) {
				auto child = std::make_shared<GameObject>(nodeData->mChildren[i]->mName.C_Str());

				modelTree.push(nodeData->mChildren[i]);
				resultTree.push(child.get());

				currentObject->addChild(std::move(child));
			}

			for (size_t i = 0; i < nodeData->mNumMeshes; ++i) {
				const Mesh& mesh = model->m_meshes[nodeData->mMeshes[i]];
				std::shared_ptr<MeshObject> meshObject = std::make_shared<MeshObject>(mesh.getName(), &mesh);
				currentObject->addChild(std::move(meshObject));
			}
		}
		
		m_data = std::move(model);
	}

	return m_data.get();
}

void ModelFactory::clear()
{
	m_data.reset(nullptr);
}
