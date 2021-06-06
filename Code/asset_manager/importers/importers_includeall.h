#ifndef _IMPORTERS_INCLUDEALL_H_
#define _IMPORTERS_INCLUDEALL_H_

#include <cstring>
#include <squelib.h>
#include <asset_manager/asset_manager.h>
#include <asset_manager/importers/import_mesh.h>

typedef void(ImportFileFun)(const SQUE_CtrlAsset* asset);
static const ImportFileFun* test = [](const SQUE_CtrlAsset* a) {
    SQUE_PRINT(LT_INFO, "Tried importing: %s from %s", a->name, a->location); };

// Will have to be changed into a X3 macro (import and export)
#define FILE_IMPORTER_TABLE(ENTRY) \
        ENTRY(FT_UNKNOWN, test) \
        ENTRY(FT_FOLDER, test) \
        ENTRY(FT_META, test) \
        ENTRY(FT_CUSTOM, test) \
        ENTRY(FT_IMAGE, test) \
        ENTRY(FT_OBJECT, AssetManager_ImportMesh) \
        ENTRY(FT_MATERIAL, test) \
        ENTRY(FT_VERT_SHADER, test) \
        ENTRY(FT_FRAG_SHADER, test) //\

enum FileType
{
    FILE_IMPORTER_TABLE(EXPAND_AS_ENUM)
    FILE_TYPE_MAX
};

sque_dyn_arr<ImportFileFun*> import_funs = {FILE_IMPORTER_TABLE(EXPAND_AS_VALUE)};

// This feels bad but I don't know how to deal with it in any other way.
uint32_t GetFileType(const char* path)
{
    /*include more extensions as seem fit*/

    const char* ext = strrchr(path, '.');
    if (ext == NULL) return FT_FOLDER;
    if (strcmp(ext, ".meta") == 0)
        return FT_META;
    else if (strstr(ext, ".sq") != NULL)
        return FT_CUSTOM;
    else if (strcmp(ext, ".png") == 0 || strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
        return FT_IMAGE;
    else if (strcmp(ext, ".gltf") == 0 || strcmp(ext, ".obj") == 0 || strcmp(ext, ".fbx") == 0)
        return FT_OBJECT;
    else if (strcmp(ext, ".vert") == 0 || strcmp(ext, ".vs") == 0)
        return FT_VERT_SHADER;
    else if (strcmp(ext, ".frag") == 0 || strcmp(ext, ".fs") == 0)
        return FT_FRAG_SHADER;

    return FT_UNKNOWN;
}

#endif