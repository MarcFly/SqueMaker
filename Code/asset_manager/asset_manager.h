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
    - Routinely check up for modified data
    - Unload data that has been unused periodically (not immediately)
    - Load data parallelly and tell when specific items are ready or unloaded
    - Classify data by types so that access is specific
    - Own unordered hashed data structure?
    - All assets have to be symbolized - aka loaded with its properties but not the actual data until used
*/

enum AssetTypes
{

};

typedef void(*ReadWriteAssetFun)(const char* location, void* data, uint64_t& _data_size);

typedef struct Asset
{
    uint32_t id;
    SQUE_Timer unused_timer;
    char name[64];
    char location[512];
    uint32_t type;
    void* specific_metadata = NULL; // This will be taken care per type basis
    
    ReadWriteAssetFun Save;
    ReadWriteAssetFun Load;

    uint64_t _data_size;
    void* data = NULL;
};



#endif