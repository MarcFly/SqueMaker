#ifndef _LOAD_SHADER_H_
#define _LOAD_SHADER_H_

#include <asset_manager/asset_manager.h>

void LoadShader(SQUE_CtrlAsset* a)
{
	SET_FLAG(a->status_flags, SQ_AS_LOADED);
	SQUE_DataPack* dp = &a->datapack;

	SQUE_Shader* shader = new SQUE_Shader();
	if (a->type == FT_VERT_SHADER)
		SQUE_SHADERS_GenerateID(shader, SQUE_VERTEX_SHADER);
	else if (a->type == FT_FRAG_SHADER)
		SQUE_SHADERS_GenerateID(shader, SQUE_FRAGMENT_SHADER);

	dp->metadata.raw_data = (char*)shader;
	dp->metadata.size = sizeof(*shader);

	SQUE_Asset* temp = SQUE_FS_LoadFileRaw(a->location);
	dp->data.raw_data = temp->raw_data;
	dp->data.size = temp->size;
}

void UnloadShader(SQUE_CtrlAsset* a)
{
	SQUE_DataPack* datapack = &a->datapack;

	SQUE_Shader* shader = (SQUE_Shader*)datapack->metadata.raw_data;
	SQUE_SHADERS_FreeFromGPU(shader->id);
	if (shader != NULL)
		delete shader;
	shader = NULL;

	char* source = (char*)datapack->data.raw_data;
	if(source != NULL)
		delete source;
	source = NULL;

	a->current_users = 0;
	CLR_FLAG(a->status_flags, SQ_AS_LOADED);
}

#endif