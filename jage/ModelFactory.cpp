#include "ModelFactory.h"

#include <stack>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "ResourceManager.h"
#include "TextureFactory.h"
#include "MeshMaterial.h"
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
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			throw std::runtime_error("Unable to load model: \"" + m_assignedName + "\" (" + m_filename + 
				"). " + importer.GetErrorString());
		}

		// Loading textures
		ResourceManager::bind<TextureFactory>("default_diffuse", "textures/default_diffuse.png");
		ResourceManager::bind<TextureFactory>("default_normals", "textures/default_normals.png");

		model->m_materials.resize(scene->mNumMaterials);
		for (size_t i = 0; i < scene->mNumMaterials; ++i) {
			const aiMaterial* materialData = scene->mMaterials[i];

			Texture* albedoTexture = nullptr;
			if (materialData->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString file;
				materialData->GetTexture(aiTextureType_DIFFUSE, 0, &file);

				ResourceManager::bind<TextureFactory>(file.C_Str(), file.C_Str());

				try {
					albedoTexture = ResourceManager::get<Texture>(file.C_Str());
					albedoTexture->generateMipmap();
					albedoTexture->setFilters(GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST);
					
					float aniso = 0.0f;
					glBindTexture(GL_TEXTURE_2D, albedoTexture->getHandle());
					glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
				}
				catch (const std::exception& e) {
					Log::write("ERROR:", e.what());
				}
			}
			if (albedoTexture == nullptr) {
				Log::write("WARNING: \"" + m_assignedName + "\" doesn't have a diffuse texture. Default is assigned.");
				albedoTexture = ResourceManager::get<Texture>("default_diffuse");
			}
			
			Texture* normalsTexture = nullptr;
			if (materialData->GetTextureCount(aiTextureType_NORMALS) > 0) {
				aiString file;
				materialData->GetTexture(aiTextureType_NORMALS, 0, &file);

				ResourceManager::bind<TextureFactory>(file.C_Str(), file.C_Str());

				try {
					normalsTexture = ResourceManager::get<Texture>(file.C_Str());
				}
				catch (const std::exception& e) {
					Log::write("ERROR:", e.what());
				}
			}
			if (normalsTexture == nullptr) {
				Log::write("WARNING: \"" + m_assignedName + "\" doesn't have a normals texture. Default is assigned.");
				normalsTexture = ResourceManager::get<Texture>("default_normals");
			}

			model->m_materials[i].setAlbedoTexture(albedoTexture);
			model->m_materials[i].setNormalsTexture(normalsTexture);
		}

		// Loading meshes
		model->m_meshes.resize(scene->mNumMeshes);
		for (size_t i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* meshData = scene->mMeshes[i];

			MeshGeometry geometry;

			geometry.positions = std::vector<vec3>(meshData->mNumVertices);
			geometry.texCoords = std::vector<vec2>(meshData->mNumVertices);
			geometry.normals = std::vector<vec3>(meshData->mNumVertices);

			for (size_t j = 0; j < meshData->mNumVertices; ++j) {
				geometry.positions[j].x = meshData->mVertices[j].x;
				geometry.positions[j].y = meshData->mVertices[j].y;
				geometry.positions[j].z = meshData->mVertices[j].z;

				if (meshData->mNormals) {
					geometry.normals[j].x = meshData->mNormals[j].x;
					geometry.normals[j].y = meshData->mNormals[j].y;
					geometry.normals[j].z = meshData->mNormals[j].z;
				}

				if (meshData->mTextureCoords && meshData->mTextureCoords[0]) {
					geometry.texCoords[j].x = meshData->mTextureCoords[0][j].x;
					geometry.texCoords[j].y = meshData->mTextureCoords[0][j].y;
				}
			}

			geometry.indices = std::vector<unsigned int>(meshData->mNumFaces * 3);
			for (size_t j = 0; j < meshData->mNumFaces; ++j) {
				const aiFace& face = meshData->mFaces[j];
				if (face.mNumIndices != 3)
				{
					Log::write("Warning: Mesh face with not exactly 3 indices, ignoring this primitive.");
					continue;
				}

				for (size_t k = 0; k < face.mNumIndices; ++k) {
					geometry.indices.push_back(face.mIndices[k]);
				}
			}

			model->m_meshes[i].init(geometry);
		}

		// Loading tree
		std::stack<aiNode*> modelTree;
		modelTree.push(scene->mRootNode);

		std::stack<Model::Node*> nodes;		
		nodes.push(&model->m_rootNode);

		while (!modelTree.empty()) {
			aiNode* nodeData = modelTree.top();
			modelTree.pop();

			Model::Node* modelNode = nodes.top();
			nodes.pop();

			modelNode->name = nodeData->mName.C_Str();
			modelNode->localTransformation = toGLM(nodeData->mTransformation);

			modelNode->children.resize(nodeData->mNumChildren + nodeData->mNumMeshes);
			for (size_t i = 0; i < nodeData->mNumChildren; ++i) {
				modelTree.push(nodeData->mChildren[i]);
				nodes.push(&modelNode->children[i]);
			}

			for (size_t i = 0; i < nodeData->mNumMeshes; ++i) {
				Model::Node* childModelNode = &modelNode->children[nodeData->mNumChildren + i];

				const aiMesh* meshData = scene->mMeshes[nodeData->mMeshes[i]];

				childModelNode->name = meshData->mName.C_Str();
				childModelNode->mesh = &model->m_meshes[nodeData->mMeshes[i]];
				childModelNode->material = &model->m_materials[meshData->mMaterialIndex];
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
