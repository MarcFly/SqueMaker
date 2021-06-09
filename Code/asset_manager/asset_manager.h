#ifndef _SQUE_MAKER_ASSET_MANAGER_
#define _SQUE_MAKER_ASSET_MANAGER_

#include <squelib.h>

/*
    What should an asset be and do:
    - Asset holds the actual data (not how to deal with it)
    - Asset knows if the data is in use or how long has it not been in use
    - Asset has properties about the data it is supposed to load (metadata)
    - Callback to load and unload function of data
    - Callback to the specifics of metadata read (all data has metadata, some general and some specific to the type)
    
    What should General Metadata Contain:
    - uint16_t:string_size string:Name, uint64:data_size, uint32_t(enum_type):type, double:MetadataLoadTimeStamp
    - uint16_t:string_size string:DataLocation // Data location when updated creates new metadata where it is located and deletes old metadata
    - uint32_t:specific_metadata_size - {specific_metadata}
    - Metadata naming convention: filename.filextension.meta (easy, this way really metadata does not need more than erasing 5 chars from string and has location)
    - Should location of an asset change, it should report a failure in loading and idk... will manage

    What should asset manager be and do:
    - Manage the calls to load data
    - Hold all the assets
    - Routinely check up for modified data - Filewatching
    - Unload data that has been unused periodically (not immediately)
    - Load data parallelly and tell when specific items are ready or unloaded
    - Classify data by types so that access is specific
    - Own unordered hashed data structure?
    - All assets have to be symbolized - aka loaded with its properties but not the actual data until used
*/

typedef void(ImportFileFun)(const SQUE_CtrlAsset* asset);
static const ImportFileFun* test = [](const SQUE_CtrlAsset* a) {
    SQUE_PRINT(LT_INFO, "Tried importing: %s from %s", a->name, a->location); };

// Will have to be changed into a X3 macro (import and export)
// Then another X2 for (load unload)
#define FILE_IMPORTER_TABLE(ENTRY) \
        ENTRY(FT_UNKNOWN, test) \
        ENTRY(FT_FOLDER, test) \
        ENTRY(FT_META, test) \
        ENTRY(FT_CUSTOM, test) \
        ENTRY(FT_IMAGE, test) \
        ENTRY(FT_OBJECT, test) \
        ENTRY(FT_MATERIAL, test) \
        ENTRY(FT_VERT_SHADER, test) \
        ENTRY(FT_FRAG_SHADER, test) //

enum FileType
{
    FILE_IMPORTER_TABLE(EXPAND_AS_ENUM)
    FILE_TYPE_MAX
};


// This feels bad but I don't know how to deal with it in any other way.
uint32_t GetFileType(const char* path);


void AssetManager_ChangeUnusedTimeUnload(const double time_ms);
uint32_t AssetManager_ExistsAsset(const char* name, const char* location);
uint32_t AssetManager_DeclareAsset(const char* name, const char* location);
SQUE_CtrlAsset* AssetManager_Get(const uint32_t id);
const SQUE_CtrlAsset* AssetManager_GetConstAsset(const uint32_t id);
void AssetManager_UseAsset(const uint32_t id);
void AssetManager_UnuseAsset(const uint32_t id);
const SQUE_Asset AssetManager_GetData(const uint32_t id);
const SQUE_Asset AssetManager_GetMetaData(const uint32_t id);

void AssetManager_HandleDropFile(const char* location);

const sque_vec<SQUE_Dir*>& AssetManager_GetBaseDirs();
const SQUE_Dir* AssetManager_GetDir(const uint32_t id);
sque_vec<SQUE_CtrlAsset*> AssetManger_GetAssetsDir(const uint32_t dir_id);
void AssetManager_RefreshDirRecursive(const uint32_t dir_id);


void AssetManager_Init();
void AssetManager_Update();

const char* AssetManager_GetDefaultDir_Meshes();
const char* AssetManager_GetDefaultDir_Tetxures();

template<class T>
const T* AssetManager_GetDataPack(const uint32_t id)
{
    const SQUE_CtrlAsset* asset = AssetManager_Get(id);
    return (T*)(asset->datapack);
}

#endif