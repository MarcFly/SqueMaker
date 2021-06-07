#ifndef _IMPORTER_MESH_
#define _IMPORTER_MESH_

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <external/tinygltf/tiny_gltf.h>
#include <squelib.h>
#include <asset_manager/asset_manager.h>


static tinygltf::Model model;
static tinygltf::TinyGLTF loader;
static std::string err;
static std::string warn;

void AssetManager_ImportMesh(const SQUE_CtrlAsset* asset)
{
	SQUE_CtrlAsset* a = (SQUE_CtrlAsset*)asset;
	SQUE_Asset* temp = SQUE_FS_LoadFileRaw(asset->location);
	a->datapack.data.raw_data = temp->raw_data;
	a->datapack.data.size = temp->size;
	
	const char* ext = strrchr(asset->name, '.');
	if(strcmp(ext, ".gltf") == 0)
		loader.LoadASCIIFromString(&model, &err, &warn, temp->raw_data, temp->size, AssetManager_GetDir(asset->dir_id)->location);
	else if(strcmp(ext, ".glb") == 0)
		loader.LoadBinaryFromMemory(&model, &err, &warn, (uchar*)temp->raw_data, temp->size);

	// Assume every gltf/glb/... file will generate a scene
	// Thus a "Prefab" should be generated per mesh, skeletons, anims,... included
	// Then a "Scene" with Prefabs with the supposed transformations
	
	// If a Mesh* has a skeleton, or a skeleton refers to a base mesh, 
	// that mesh should generate just a single object -> sub items are meshes with "slot" components


	// Temporally, will load 1st Mesh and 1st Primitive only
	// That will be the base mesh
	for (uint16_t i = 0; i < model.meshes.size(); ++i)
	{
		SQUE_Mesh* mesh = new SQUE_Mesh();
		// Setup Name
		// Setup Attributes
		for (uint16_t j = 0; j < model.meshes[i].primitives.size(); ++j)
		{
			const tinygltf::Primitive& p = model.meshes[i].primitives[j];
			
			uint16_t ids = 0;
			for (auto attribute : p.attributes)
			{
				tinygltf::Accessor& v = model.accessors[attribute.second];
				SQUE_MESH_AddAttribute(mesh, attribute.first.c_str(), v.componentType, v.normalized, v.count);
			}
		}
		delete mesh;
		//model.meshes[i].
	}
	

	//model.buffers[0]-

	a->datapack.data.raw_data = (char*) new SQUE_Mesh();


	delete temp->raw_data;
	delete temp;
}



#endif