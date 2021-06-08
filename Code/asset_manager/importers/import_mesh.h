#ifndef _IMPORTER_MESH_
#define _IMPORTER_MESH_

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <external/tinygltf/tiny_gltf.h>
#include <squelib.h>
#include <asset_manager/asset_manager.h>

struct EngineMaterial
{

};

struct EngineMeshPrimitive
{
	~EngineMeshPrimitive() {
		deleteRange(vertices, vertices + vertices_size);
		deleteRange(indices, indices + indices_size);
		free(indices);
		free(vertices);
	}

	SQUE_Mesh config;
	char* vertices;
	uint32_t vertices_size;
	char* indices;
	uint32_t indices_size;
};

struct EngineMesh
{
	// Textures
	// Materials
	// Skeletons
	// ...
	
	sque_vec <EngineMeshPrimitive> primitives;
};



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
		EngineMesh* mesh = new EngineMesh();
		tinygltf::Mesh& m = model.meshes[i];
		
		// Generate a SQUE_CtrlAsset for the new mesh
		static char name[64];
		sprintf(name, "%s.sq_mesh", m.name.c_str());
		static char path[320];
		sprintf(path, "%s%c%s", AssetManager_GetDefaultDir_Meshes(), FE, name);

		// Ignore if already exists
		// Again, string hashing would make it faster
		if (AssetManager_ExistsAsset(name, path) != UINT32_MAX)
			continue;

		uint32_t new_asset = AssetManager_DeclareAsset(name, path);
		SQUE_CtrlAsset* ctrl_asset = AssetManager_Get(new_asset);
		ctrl_asset->datapack.data.raw_data = (char*)mesh;
		ctrl_asset->datapack.data.size = sizeof(EngineMesh);

		// Primitives
		for (uint16_t j = 0; j < m.primitives.size(); ++j)
		{
			const tinygltf::Primitive& p = model.meshes[i].primitives[j];
			mesh->primitives.push_back(EngineMeshPrimitive());
			EngineMeshPrimitive* e_p = mesh->primitives.last();
			uint16_t ids = 0;
			for (auto attribute : p.attributes)
			{
				tinygltf::Accessor& v = model.accessors[attribute.second];
				SQUE_MESH_AddAttribute(&e_p->config, attribute.first.c_str(), v.componentType, v.normalized, 0); // num_components can't be setup here
				SQUE_VertAttrib* p_attrib = e_p->config.attributes.last();
				p_attrib->offset = v.byteOffset;
				p_attrib->id = e_p->config.attributes.size() - 1;

				tinygltf::BufferView& b_v = model.bufferViews[v.bufferView];
				p_attrib->num_comp = (b_v.byteLength / p_attrib->var_size) / v.count;
				e_p->config.num_verts = v.count / p_attrib->num_comp;
				reallocate(&e_p->vertices, e_p->vertices_size, e_p->vertices_size, e_p->vertices_size + b_v.byteLength);
				memcpy(e_p->vertices+(e_p->vertices_size - b_v.byteLength), &model.buffers[b_v.buffer].data[b_v.byteOffset], b_v.byteLength);
				
				// stride? v.ByteStride(model.bufferViews[v.bufferView]);				
			}

			SQUE_MESH_SetDrawConfig(&e_p->config, SQUE_STATIC_DRAW, p.mode);
			tinygltf::Accessor& v = model.accessors[p.indices];
			SQUE_MESH_SetDataConfig(&e_p->config, e_p->config.num_verts, v.count, v.componentType);
			e_p->indices_size = v.count * e_p->config.index_var_size;
			e_p->indices = allocate<char>(e_p->indices_size);
			tinygltf::BufferView& b_v = model.bufferViews[v.bufferView];
			// data could be that it needs to be loaded from uri (file in relative directory to asset, gltf should be base, then go into folders if so)
			// in case data is not already loaded by tinygltf properly, have to load the buffer block.
			memcpy(e_p->indices, &model.buffers[b_v.buffer].data[b_v.byteOffset], e_p->indices_size);
			e_p->config.draw_mode = p.mode;
		}
		// Materials
		// Skeleton

		// Serialize it
		SQUE_OutStream out;
		// Bruteforce serialization for now
		for (uint16_t k = 0; k < mesh->primitives.size(); ++k)
		{
			out.WriteBytes(&mesh->primitives[k].config, 1);
			// Config has to serialize Attributes...
			out.WriteBytes(&mesh->primitives[k].vertices_size, 1);
			out.WriteBytes(mesh->primitives[k].vertices, mesh->primitives[k].vertices_size);
			out.WriteBytes(&mesh->primitives[k].indices_size, 1);
			out.WriteBytes(mesh->primitives[k].indices, mesh->primitives[k].indices_size);
		}

		SQUE_FS_WriteFileRaw(ctrl_asset->location, out.GetData(), out.GetSize());
	}


	delete temp->raw_data;
	delete temp;
}



#endif