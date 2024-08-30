#ifndef __ECS_H__
#define __ECS_H__

#include <string.h>

#include "core/defines.h"
#include "core/alloc.h"
#include "math/utils.h"
#include "math/vec.h"

/*
 * Table structure
 * 
 * comp_table = {
 *                    CompEntry
 *                         |
 *                        \/
 *	"component_1": { [ent1, comp1], [ent2, comp2], [ent3, comp3] ... }   <== comp_record
 *	"component_2": { [ent1, comp1], [ent2, comp2], [ent3, comp3] ... }
 * }
 */

/* =======================
 * Entity
 * ======================= */

/*
 * @brief Enum to determine if the entity slot is OCCUPIED or FREE
 */

typedef enum {
	FREE,
	OCCUPIED
} EntitySlotState;


/*
 * @brief Iso Entity type definition
 */

typedef u32 Entity;


/* =======================
 * Component Entry
 * ======================= */


/*
 * @brief Component struct that stores entity id and component data
 * @mem ent  = Entity id
 * @mem data = Component data
 */

typedef struct {
	Entity ent;
	void* data;
} CompEntry;


/*
 * @brief Function to create component entry
 * @param ent  = entity
 * @param data = Component data
 * @return Returns pointer to CompEntry struct
 */

CompEntry* comp_entry_new(Entity ent, void* data);


/*
 * @brief Function to delete CompEntry
 * @param entry = Pointer to CompEntry struct
 */

void comp_entry_delete(CompEntry* entry);


/* =======================
 * Component Record
 * ======================= */


/*
 * @brief Struct that holds a multiple entries of a single component type
 * @mem name          = Name of the component
 * @mem entry_cnt     = No of entry created
 * @mem max_entry_cnt = Max amount of entry that can be created
 * @mem entries       = Array of CompEntry
 */

typedef struct {
	char* name;
	u32   entry_cnt;
	u32   max_entry_cnt;
	Entity* entries_ent;
	CompEntry** entries;
} CompRecord;


/*
 * @brief Function to create a new comp_record
 * @param name          = Name of the record
 * @param max_entry_cnt = Max amount of entry to support
 * @return Returns pointer to comp_record struct
 */

CompRecord* comp_record_new(char* name, u32 max_entry_cnt);


/*
 * @brief Function to delete the comp_record
 * @param rec = Pointer to comp_record struct
 */

void comp_record_delete(CompRecord* rec);


/*
 * @brief Function to search an entry in record according to entity id
 * @param rec = Pointer to comp_record
 * @param ent = entity
 * @return Returns True if found else False
 */

b32 comp_record_search(CompRecord* rec, Entity ent);


/*
 * @brief Function to add a new entry in the component record
 * @param rec  = Pointer to comp_record
 * @param ent  = entity
 * @param data = Component data
 * @return Returns pointer to CompEntry struct
 */

CompEntry* comp_record_add_entry(CompRecord* rec, Entity ent, void* data);


/*
 * @brief Function to get the entry from record according to the entity id
 * @param rec = Pointer to the comp_record struct
 * @param ent = entity
 * @return Returns pointer to CompEntry struct
 */

CompEntry* comp_record_get_entry(CompRecord* rec, Entity ent);


/*
 * @brief Function to remove entry from the record
 * @param rec = Pointer to the comp_record
 * @param ent = entity
 */

void comp_record_remove_entry(CompRecord* rec, Entity ent);


/* =======================
 * Component Table
 * ======================= */


/*
 * @brief Structure to hold multiple component records
 * @mem record_cnt     = Total amount of records created
 * @mem max_record_cnt = Max amount of record that can be created
 * @mem records        = Array of comp_record
 */

typedef struct {
	u32 record_cnt;
	u32 max_record_cnt;
	CompRecord** records;
} CompTable;


/*
 * @brief Function to create new comp_table
 * @param max_record_cnt = Maximum amount of record to be created
 * @return Returns pointer to comp_table struct
 */

CompTable* comp_table_new(u32 max_record_cnt);


/*
 * @brief Function to delete comp_table
 * @param table = Pointer to the comp_table
 */

void comp_table_delete(CompTable* table);


/*
 * @brief Function to search for component in comp_table
 * @param table = Pointer to the comp_table
 * @param name  = Name of the component to be searched
 * @return Returns True if found else False
 */

b32 __comp_table_search(CompTable* table, char* name);
#define comp_table_search(table, comp) __comp_table_search(table, #comp)


/*
 * @brief Function to add new component record to the table
 * @param table = Pointer to the comp_table
 * @param name  = Name of the component to be added
 */

void __comp_table_add_record(CompTable* table, char* name);
#define comp_table_add_record(table, comp) __comp_table_add_record(table, #comp)


/*
 * @brief Function to get the component record from the table
 * @param table = Pointer to the comp_table
 * @param name  = Name of the component to get
 * @return Returns pointer to comp_record if found else NULL is returned
 */

CompRecord* __comp_table_get_record(CompTable* table, char* name);
#define comp_table_get_record(table, comp) __comp_table_get_record(table, #comp)


/* =======================
 * Entity Component System
 * ======================= */


/*
 * @brief Structure that holds entire component in the system
 * @mem entity_cnt     = No of entity created
 * @mem max_entity_cnt = Max no of entity that can be created
 * @mem slots          = Slots for entity (EntitySlotState)
 * @mem table          = Pointer to the comp_table
 */

typedef struct {
	u32 entity_cnt;
	u32 max_entity_cnt;
	EntitySlotState* slots;
	CompTable* table;
} ECS;


/*
 * @brief Function to create a new ecs
 * @param max_entity_cnt = Maximum amount of entity to be handled by ecs
 * @return Returns pointer to ecs struct
 */

ECS* ecs_new(u32 max_entity_cnt);


/*
 * @brief Function to delete ecs
 * @param ecs = Pointer to ecs struct
 */

void ecs_delete(ECS* ecs);


/* =======================
 * Entity Commands
 * ======================= */


/*
 * @brief Function to create new entity
 * @param ecs = Pointer to ecs
 * @return Returns a entity type which is a entity id
 */

Entity entity_new(ECS* ecs);


/*
 * @brief Function to delete an entity
 * @param ecs = Pointer to the ecs
 * @param ent = entity id
 */

void entity_delete(ECS* ecs, Entity ent);


/*
 * @brief Internal function to add component to entity
 * @param ecs  = Pointer to ecs
 * @param ent  = entity id
 * @param name = Name of the component
 * @param data = Data of the component
 */

void* __entity_add_component(ECS* ecs, Entity ent, char* name, void* data);


/*
 * @brief Internal function to get the component from entity
 * @param ecs  = Pointer to ecs
 * @param ent  = entity id
 * @param name = Name of the component
 * @return Returns pointer to the component data
 */

void* __entity_get_component(ECS* ecs, Entity ent, char* name);


/*
 * @brief Internal function to remove component from entity
 * @param ecs  = Pointer to ecs
 * @param ent  = entity id
 * @param name = Name of the component
 */

void __entity_remove_component(ECS* ecs, Entity ent, char* name);


/*
 * @brief Macro to add a new component
 * @param ecs  = Pointer to ecs
 * @param ent  = entity id
 * @param comp = Component structure
 * @param ...  = Component parameters
 */

#define entity_add_component(ecs, ent, comp, ...)   \
	({                                                    \
		comp c = { __VA_ARGS__ };                           \
		comp* cp = alloc(sizeof(c));                    \
		memcpy(cp, &c, sizeof(c));                          \
		__entity_add_component(ecs, ent, #comp, cp);    \
	})                                                    \


/*
 * @brief Macro to get a component
 * @param ecs  = Pointer to ecs
 * @param ent  = entity id
 * @param comp = Component structure
 */

#define entity_get_component(ecs, ent, comp)\
	__entity_get_component(ecs, ent, #comp)


/*
 * @brief Macro to remove a component
 * @param ecs  = Pointer to ecs
 * @param ent  = entity id
 * @param comp = Component structure
 */

#define entity_remove_component(ecs, ent, comp)\
	__entity_remove_component(ecs, ent, #comp)

#endif // __ECS_H__
