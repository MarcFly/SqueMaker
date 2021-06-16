#ifndef _LOAD_MESH_H_
#define _LOAD_MESH_H_

#include <asset_manager/asset_manager.h>
#include <asset_manager/importers/import_mesh.h>

void LoadMesh(SQUE_CtrlAsset* a)
{
    SET_FLAG(a->status_flags, SQ_AS_LOADED);
    SQUE_DataPack* dp = &a->datapack;


    EngineMesh* mesh = new EngineMesh();

    uint32_t num_primitives;
    SQUE_Asset* temp = SQUE_FS_LoadFileRaw(a->location);
    SQUE_InStream istream(temp);

    istream.ReadBytes(&num_primitives);

    for (uint32_t i = 0; i < num_primitives; ++i)
    {
        mesh->primitives.push_back(EngineMeshPrimitive());
        EngineMeshPrimitive* p = mesh->primitives.last();
        SQUE_Mesh* m = &p->config;

        // Get Config
        istream.ReadBytes(&m->draw_config);
        istream.ReadBytes(&m->draw_mode);
        istream.ReadBytes(&m->num_verts);
        istream.ReadBytes(&m->num_index);
        istream.ReadBytes(&m->index_var);
        istream.ReadBytes(&m->index_var_size);

        uint32_t num_attribs;
        istream.ReadBytes(&num_attribs);

        for (uint32_t j = 0; j < num_attribs; ++j)
        {
            m->attributes.push_back(SQUE_VertAttrib());
            SQUE_VertAttrib* v = m->attributes.last();
            
            istream.ReadBytes(&v->id);
            istream.ReadBytes(&v->var_type);
            istream.ReadBytes(&v->num_comp);
            istream.ReadBytes(&v->normalize);
            istream.ReadBytes(&v->var_size);
            istream.ReadBytes(&v->offset);

            uint32_t len;
            istream.ReadBytes(&len);
            istream.ReadBytes(&v->name[0], len);
            v->name[len] = '\0';
        }

        // Get Data
        istream.ReadBytes(&p->vertices_size);
        uint32_t v_size = SQUE_MESH_CalcVertSize(m);
        p->vertices = new char[p->vertices_size];
        istream.ReadBytes(p->vertices, p->vertices_size);
        istream.ReadBytes(&p->indices_size);
        p->indices = new char[p->indices_size];
        istream.ReadBytes(p->indices, p->indices_size);

        // Load to GPU
        SQUE_MESH_GenBuffer(m);
        SQUE_MESH_BindBuffer(*m);
        SQUE_MESH_BlockOffsets(m);
        SQUE_MESH_SetLocations(m);
        SQUE_MESH_SendToGPU(*m, p->vertices, p->indices);
    }


    //delete temp->raw_data;
    //delete temp;

    dp->data.raw_data = (char*)mesh;
}

void UnloadMesh(SQUE_CtrlAsset* a)
{
    
    EngineMesh* mesh = (EngineMesh*)a->datapack.data.raw_data;
    for (uint32_t i = 0; i < mesh->primitives.size(); ++i)
    {
        EngineMeshPrimitive& p = mesh->primitives[i];
        SQUE_MESH_FreeFromGPU(p.config);
        p.~EngineMeshPrimitive();
    }

    delete mesh;
    mesh = NULL;
    a->datapack.data.raw_data = NULL;
}

#endif