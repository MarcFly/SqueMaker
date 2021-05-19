#include "./asset_manager.h"
#include "./importers/importers_includeall.h"

static sque_vec<Asset> assets;
static sque_vec<SQUE_Dir> directories;
static sque_vec<SQUE_Dir*> base_parents;
static double unused_time_unload_ms = 5000;

void AssetManager_ChangeUnusedTimeUnload(const double time_ms)
{
	unused_time_unload_ms = time_ms;
}

uint32_t AssetManager_DeclareAsset(const char* name, const char* location)
{
	Asset new_asset;
	new_asset.id = SQUE_RNG();
	new_asset.unused_timer.Kill();
	memcpy(new_asset.name, name, strlen(name));
	memcpy(new_asset.location, location, strlen(location));

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
				new_dir.parent = &directories[i];
				break;
			}
		}

		directories.push_back(new_dir);
		if (new_dir.parent == NULL)
			base_parents.push_back(directories.last());

		new_asset.dir_id = new_dir.id;
	}
	else
		new_asset.dir_id = directories[i].id;

	assets.push_back(new_asset);
	return new_asset.id;
}

Asset* AssetManager_Get(const uint32_t id)
{
	for (uint32_t i = 0; i < assets.size(); ++i)
		if (id == assets[i].id)
			return &assets[i];
	return NULL;
}

const Asset* AssetManager_GetConst(const uint32_t id)
{
	for (uint32_t i = 0; i < assets.size(); ++i)
		if (id == assets[i].id)
			return &assets[i];
	return NULL;
}

void AssetManager_LoadAssetDisk(Asset* asset)
{
	SQUE_Asset* load = SQUE_FS_LoadAssetRaw(asset->location);
	char meta_location[sizeof(asset->location) + 5];
	sprintf(meta_location, "%s.meta", asset->location);
	SQUE_Asset* load_meta = SQUE_FS_LoadAssetRaw(meta_location);

	asset->datapack.data = load->raw_data;
	asset->datapack.data_size = load->size;
	asset->datapack.metadata = load_meta->raw_data;
	asset->datapack.metadata_size = load_meta->size;
}

void AssetManager_UseAsset(const uint32_t id)
{
	Asset* asset = AssetManager_Get(id);
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
	Asset* asset = AssetManager_Get(id);
	if (asset == NULL) return;

	if (!asset->unused_timer.IsActive())
	{
		if (--asset->current_users == 0)
			asset->unused_timer.Start();
	}	
}

const SQUE_Asset AssetManager_GetData(const uint32_t id)
{
	const Asset* asset = AssetManager_GetConst(id);
	SQUE_Asset ret;
	ret.raw_data = (char*)(asset->datapack.data);
	ret.size = asset->datapack.data_size;
	return ret;
}

const SQUE_Asset AssetManager_GetMetaData(const uint32_t id)
{
	const Asset* asset = AssetManager_GetConst(id);
	SQUE_Asset ret;
	ret.raw_data = (char*)(asset->datapack.metadata);
	ret.size = asset->datapack.metadata_size;
	return ret;
}

void AssetManager_HandleDropFile(const char* location)
{
	uint32_t rel_type = GetFileType(location);
	uint32_t asset_id = AssetManager_DeclareAsset(SQUE_FS_GetFileName(location), location);
	const Asset* const_get = AssetManager_GetConst(asset_id);
	import_funs[rel_type](const_get);

}

const sque_vec<SQUE_Dir*>& AssetManager_GetBaseDirs()
{
	return base_parents;
}

sque_vec<Asset*> AssetManger_GetAssetsDir(const uint32_t dir_id)
{
	sque_vec<Asset*> ret;
	for (uint32_t i = 0; i < assets.size(); ++i)
	{
		if (assets[i].dir_id == dir_id)
			ret.push_back(&assets[i]);
	}
	return ret;
}

void AssetManager_Update()
{
	// Dealing with events for loading and such...
	// How to react with that type of things...

	for (uint32_t i = 0; i < assets.size(); ++i)
	{
		if (assets[i].unused_timer.IsActive() && assets[i].unused_timer.ReadMilliSec() > unused_time_unload_ms)
		{
			assets[i].Unload();
			assets[i].unused_timer.Kill();
		}
	}
}