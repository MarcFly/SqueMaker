#include "asset_manager.h"


static sque_vec<SQUE_CtrlAsset> assets;
static sque_vec<SQUE_Dir> directories;
static sque_vec<SQUE_Dir*> base_parents;
static double unused_time_unload_ms = 5000;
static sque_dyn_arr<ImportFileFun*> import_funs = { FILE_IMPORTER_TABLE(EXPAND_AS_VALUE) };
static sque_dyn_arr<LoadUnloadFun*> load_funs = { FILE_LOAD_UNLOAD_TABLE(EXPAND_AS_ENUM) };
static sque_dyn_arr<LoadUnloadFun*> unload_funs = { FILE_LOAD_UNLOAD_TABLE(EXPAND_AS_VALUE) };

void AssetManager_ChangeUnusedTimeUnload(const double time_ms)
{
	unused_time_unload_ms = time_ms;
}

uint32_t AssetManager_ExistsAsset(const char* name, const char* location)
{
	for (uint32_t i = 0; i < assets.size(); ++i)
	{
		int16_t cmp1 = strcmp(assets[i].name, name);
		int16_t cmp2 = strcmp(assets[i].location, location);
		if (cmp1 == NULL && cmp1 == cmp2)
			return assets[i].id;
	}
	return UINT32_MAX;
}

uint32_t AssetManager_DeclareAsset(const char* name, const char* location)
{
	SQUE_CtrlAsset new_asset;
	new_asset.id = SQUE_RNG();
	new_asset.unused_timer.Kill();
	memcpy(new_asset.name, name, strlen(name));
	memcpy(new_asset.location, location, strlen(location));
	new_asset.type = GetFileType(location);
	new_asset.last_update = 0;
	if (new_asset.type == FT_FOLDER) return -1;
	uint32_t exists = AssetManager_ExistsAsset(name, location);
	if (exists != UINT32_MAX)
		return exists;

	// TODO: String Hash and custom implementation of a map...
	std::string location_str(location);
	uint16_t name_len = strlen(SQUE_FS_GetFileName(location));
	location_str.erase(location_str.length() - name_len);
	uint16_t i;
	for (i = 0; i < directories.size(); ++i)
	{
		if (strcmp(directories[i].location, location_str.c_str()) == 0)
			break;
	}
	if (i == directories.size())
	{
		SQUE_Dir new_dir;
		memcpy(new_dir.location, location_str.c_str(), sizeof(new_dir.location));
		const char* dirname_temp = SQUE_FS_GetFileName(location_str.c_str());
		memcpy(new_dir.name, dirname_temp, sizeof(new_dir.name));
		new_dir.id = SQUE_RNG();
		location_str.erase(location_str.length() - strlen(dirname_temp));
		const char* pardirname_temp = SQUE_FS_GetFileName(location_str.c_str());
		for (i = 0; i < directories.size(); ++i)
		{
			if (strcmp(directories[i].name, pardirname_temp) == 0)
			{
				new_dir.parent_id = directories[i].id;
				break;
			}
		}

		directories.push_back(new_dir);
		if (new_dir.parent_id == -1)
			base_parents.push_back(directories.last());

		new_asset.dir_id = new_dir.id;
	}
	else
		new_asset.dir_id = directories[i].id;

	assets.push_back(new_asset);
	return new_asset.id;
}

SQUE_CtrlAsset* AssetManager_Get(const uint32_t id)
{
	for (uint32_t i = 0; i < assets.size(); ++i)
		if (id == assets[i].id)
			return &assets[i];
	return NULL;
}

const SQUE_CtrlAsset* AssetManager_GetConstAsset(const uint32_t id)
{
	for (uint32_t i = 0; i < assets.size(); ++i)
		if (id == assets[i].id)
			return &assets[i];
	return NULL;
}

void AssetManager_LoadAssetDisk(SQUE_CtrlAsset* asset)
{
	SQUE_Asset* load = SQUE_FS_LoadAssetRaw(asset->location);
	char meta_location[sizeof(asset->location) + 5];
	sprintf(meta_location, "%s.meta", asset->location);
	SQUE_Asset* load_meta = SQUE_FS_LoadAssetRaw(meta_location);

	asset->datapack.data.raw_data= load->raw_data;
	asset->datapack.data.size = load->size;
	asset->datapack.metadata.raw_data = load_meta->raw_data;
	asset->datapack.metadata.size = load_meta->size;
}

void AssetManager_UseAsset(const uint32_t id)
{
	SQUE_CtrlAsset* asset = AssetManager_Get(id);
	if (asset == NULL) return;
	++asset->current_users;
	if (asset->current_users == 0)
	{
		
		asset->unused_timer.Kill();
		AssetManager_LoadAssetDisk(asset);
		asset->Load(asset->location, &asset->datapack);
	}
}

void AssetManager_UnuseAsset(const uint32_t id)
{
	SQUE_CtrlAsset* asset = AssetManager_Get(id);
	if (asset == NULL) return;

	if (!asset->unused_timer.IsActive())
	{
		if (--asset->current_users == 0)
			asset->unused_timer.Start();
	}	
}

const SQUE_Asset AssetManager_GetData(const uint32_t id)
{
	const SQUE_CtrlAsset* asset = AssetManager_GetConstAsset(id);
	SQUE_Asset ret;
	ret.raw_data = (char*)(asset->datapack.data.raw_data);
	ret.size = asset->datapack.data.size;
	return ret;
}

const SQUE_Asset AssetManager_GetMetaData(const uint32_t id)
{
	const SQUE_CtrlAsset* asset = AssetManager_GetConstAsset(id);
	SQUE_Asset ret;
	ret.raw_data = (char*)(asset->datapack.metadata.raw_data);
	ret.size = asset->datapack.metadata.size;
	return ret;
}

void AssetManager_HandleDropFile(const char* location)
{
	uint32_t rel_type = GetFileType(location);
	uint32_t asset_id = AssetManager_DeclareAsset(SQUE_FS_GetFileName(location), location);
	const SQUE_CtrlAsset* const_get = AssetManager_GetConstAsset(asset_id);
	if(const_get != NULL) import_funs[rel_type](const_get);
}

const sque_vec<SQUE_Dir*>& AssetManager_GetBaseDirs()
{
	return base_parents;
}

const SQUE_Dir* AssetManager_GetDir(const uint32_t id)
{
	for(uint32_t i = 0; i < directories.size(); ++i)
	{
		if (directories[i].id == id)
			return &directories[i];
	}
	return NULL;
}

sque_vec<SQUE_CtrlAsset*> AssetManger_GetAssetsDir(const uint32_t dir_id)
{
	sque_vec<SQUE_CtrlAsset*> ret;
	for (uint32_t i = 0; i < assets.size(); ++i)
	{
		if (assets[i].dir_id == dir_id)
			ret.push_back(&assets[i]);
	}
	return ret;
}

static void SetChildDirs(const SQUE_Dir* base_dir, sque_vec<const SQUE_Dir*>* dirs)
{
	for (uint32_t i = 0; i < base_dir->children_ids.size(); ++i)
	{
		dirs->push_back(AssetManager_GetDir(base_dir->children_ids[i]));
		SetChildDirs(*dirs->last(), dirs);
	}
}

void AssetManager_RefreshDirRecursive(const uint32_t dir_id)
{
	sque_vec<SQUE_CtrlAsset*> checked;
	sque_vec<const SQUE_Dir*> dirs;
	dirs.push_back(AssetManager_GetDir(dir_id));
	if (dirs[0] == NULL)
		return;

	SetChildDirs(*dirs.last(), &dirs);

	for (uint32_t i = 0; i < assets.size(); ++i)
	{
		for (uint32_t j = 0; j < dirs.size(); ++j)
		{
			if (assets[i].dir_id == dirs[j]->id)
			{
				checked.push_back(&assets[i]);
				break;
			}
		}
	}

	sque_vec<SQUE_FW_NewAsset*> ret = SQUE_FS_CheckDirectoryChanges(dirs[0]->location, checked, NULL);
	
	// Handle New Items Immediately
	char str[512];
	for (uint32_t i = 0; i < ret.size(); ++i)
	{
		uint32_t t_id = AssetManager_DeclareAsset(SQUE_FS_GetFileName(ret[i]->str), ret[i]->str);
		if (t_id != UINT32_MAX)
		{
			SQUE_CtrlAsset* t = (SQUE_CtrlAsset*)AssetManager_GetConstAsset(t_id);
			t->last_update = ret[i]->last_update;
		}
		delete ret[i];
	}
}

void AssetManager_SetAssetUser(uint32_t* user, const uint32_t file_id)
{
	// Detract a user of a previous file
	if (*user != UINT32_MAX)
	{
		for (uint16_t i = 0; i < assets.size(); ++i)
			if (*user == assets[i].id)
			{
				--assets[i].current_users;
				break;
			}
	}

	if (file_id != UINT32_MAX)
	{
		for(uint16_t i = 0; i < assets.size(); ++i)
			if (file_id == assets[i].id)
			{
				++assets[i].current_users;
				if (!CHK_FLAG(assets[i].status_flags, SQ_AS_LOADED))
				{
					load_funs[assets[i].type](&assets[i]);
				}
				break;
			}
	}
	*user = file_id;
}
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

static char tmp_meshes[256];
static char tmp_textures[256];

const char* AssetManager_GetDefaultDir_Meshes()
{
	return tmp_meshes;
}

const char* AssetManager_GetDefaultDir_Textures()
{
	return tmp_textures;
}

#include <asset_manager/importers/importers_includeall.h>
#include <asset_manager/loaders/loaders_includeall.h>

void AssetManager_Init()
{
	const char* exec_path = SQUE_FS_GetExecPath();
	int len = strlen(exec_path);

	// Generates the base folders if not existing
	sprintf(tmp_meshes, "%s%cAssets%ctmp_folders%cmeshes", exec_path, FE, FE, FE);
	sprintf(tmp_textures, "%s%cAssets%ctmp_folders%ctextures", exec_path, FE, FE, FE);
	SQUE_FS_CreateDirFullPath(tmp_meshes);
	SQUE_FS_CreateDirFullPath(tmp_textures);
	//SQUE_FS_CreateDirRelative(tmp_meshes);
	//SQUE_FS_CreateDirRelative(tmp_textures);

	// Generate directory structures for assets and resources
	char* assets = new char[len+16];
	char* resources = new char[len+16];
	sprintf(assets, "%s%cAssets", exec_path, FOLDER_ENDING);
	sprintf(resources, "%s%cResources",exec_path, FOLDER_ENDING);

	SQUE_FS_GenDirectoryStructure(assets, &directories);
	base_parents.push_back(directories.begin());
	int last = directories.size();
	SQUE_FS_GenDirectoryStructure(resources, &directories);
	base_parents.push_back(&directories[last]);

	delete[] assets;
	delete[] resources;

	// Setup the variables in the dynarray
	import_funs[FT_OBJECT] = AssetManager_ImportMesh;
	load_funs[FT_VERT_SHADER] = load_funs[FT_FRAG_SHADER] = LoadShader;
	unload_funs[FT_VERT_SHADER] = unload_funs[FT_FRAG_SHADER] = UnloadShader;


}

static SQUE_Timer test_timer;

void AssetManager_Update()
{
	// Dealing with events for loading and such...
	// How to react with that type of things...
	
	if (test_timer.ReadMilliSec() > 10000 && directories.size() > 0)
	{
		AssetManager_RefreshDirRecursive(directories[0].id);
		test_timer.Start();
	}
	
	for (uint32_t i = 0; i < assets.size(); ++i)
	{
		bool unload_time = assets[i].unused_timer.IsActive() && assets[i].unused_timer.ReadMilliSec() > unused_time_unload_ms;
		bool unload_delete = CHK_FLAG(assets[i].status_flags, SQ_AS_DELETED);
		if (unload_time && CHK_FLAG(assets[i].status_flags, SQ_AS_LOADED))
		{
			assets[i].Unload(&assets[i].datapack);
			CLR_FLAG(assets[i].status_flags, SQ_AS_LOADED);
			// Load and Unload of a datapack does not give power over ctrl_asset
			// Thus Engine has to set the flags... better than relying on user?
			assets[i].unused_timer.Kill();
		}
		if (CHK_FLAG(assets[i].status_flags, SQ_AS_DELETED) && !CHK_FLAG(assets[i].status_flags,SQ_AS_LOADED))
		{
			// If the Asset is not loaded, it will be deleted directly
			SQUE_Swap(&assets[i], assets.last());
			assets.pop_back();
		}

		if (CHK_FLAG(assets[i].status_flags, SQ_AS_CHANGED))
		{
			if(CHK_FLAG(assets[i].status_flags, SQ_AS_LOADED)) 
				unload_funs[assets[i].type](&assets[i]);
			load_funs[assets[i].type](&assets[i]);
			CLR_FLAG(assets[i].status_flags, SQ_AS_CHANGED);
		}
	}


}

uint32_t GetFileType(const char* path)
{
	/*include more extensions as seem fit*/

	const char* ext = strrchr(path, '.');
	if (ext == NULL) return FT_FOLDER;
	if (strcmp(ext, ".meta") == 0)
		return FT_META;
	else if (strcmp(ext, ".png") == 0 || strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
		return FT_IMAGE;
	else if (strcmp(ext, ".gltf") == 0 || strcmp(ext, ".obj") == 0 || strcmp(ext, ".fbx") == 0)
		return FT_OBJECT;
	else if (strcmp(ext, ".vert") == 0 || strcmp(ext, ".vs") == 0)
		return FT_VERT_SHADER;
	else if (strcmp(ext, ".frag") == 0 || strcmp(ext, ".fs") == 0)
		return FT_FRAG_SHADER;
	else if (strstr(ext, "sq") != NULL)
	{
		if (strstr(ext, "mesh") != NULL)
			return FT_OBJECT;
		else if (strstr(ext, "frag") != NULL)
			return FT_FRAG_SHADER;
		else if (strstr(ext, "vert") != NULL)
			return FT_VERT_SHADER;
		// add custom types here
	}

	return FT_UNKNOWN;
}