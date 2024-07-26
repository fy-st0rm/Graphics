#include "ecs.h"


/* =======================
 * Component Entry
 * ======================= */


CompEntry* comp_entry_new(Entity ent, void* data) {
	CompEntry* entry = malloc(sizeof(CompEntry));
	entry->ent = ent;
	entry->data = data;
	return entry;
}

void comp_entry_delete(CompEntry* entry) {
	free(entry->data);
	free(entry);
}


/* =======================
 * Component Record
 * ======================= */



CompRecord* comp_record_new(char* name, u32 max_entry_cnt) {
	CompRecord* rec = malloc(sizeof(CompRecord));

	// Initializing variables
	rec->entries_ent = malloc(sizeof(Entity) * max_entry_cnt);
	rec->entries = malloc(sizeof(CompEntry) * max_entry_cnt);
	rec->entry_cnt = 0;
	rec->max_entry_cnt = max_entry_cnt;

	rec->name = malloc(strlen(name));
	strcpy(rec->name, name);

	return rec;
}

void comp_record_delete(CompRecord* rec) {
	for (u32 i = 0; i < rec->max_entry_cnt; i++) {
		CompEntry* entry = rec->entries[i];
		if (entry) comp_entry_delete(entry);
	}
	free(rec->name);
	free(rec->entries_ent);
	free(rec->entries);
	free(rec);
}

b32 comp_record_search(CompRecord* rec, Entity ent) {
	assert(ent < rec->max_entry_cnt, "Tried to access entity of slot `%d` in record of max slot `%d`.\n", ent, rec->max_entry_cnt);

	CompEntry* entry = rec->entries[ent];
	if (entry) return true;
	return false;
}

CompEntry* comp_record_add_entry(CompRecord* rec, Entity ent, void* data) {
	assert(ent < rec->max_entry_cnt, "Tried to access entity of slot `%d` in record of max slot `%d`.\n", ent, rec->max_entry_cnt);
	assert(rec->entry_cnt < rec->max_entry_cnt, "Component record is full. Cannot add entry to entity `%d`.\n", ent);

	CompEntry* entry = comp_entry_new(ent, data);
	rec->entries[ent] = entry;
	rec->entries_ent[rec->entry_cnt++] = ent;

	return entry;
}

CompEntry* comp_record_get_entry(CompRecord* rec, Entity ent) {
	assert(ent < rec->max_entry_cnt, "Tried to access entity of slot `%d` in record of max slot `%d`.\n", ent, rec->max_entry_cnt);

	CompEntry* entry = rec->entries[ent];
	if (entry) return entry;
	return NULL;
}

void comp_record_remove_entry(CompRecord* rec, Entity ent) {
	assert(ent < rec->max_entry_cnt, "Tried to access entity of slot `%d` in record of max slot `%d`.\n", ent, rec->max_entry_cnt);

	CompEntry* entry = rec->entries[ent];
	comp_entry_delete(entry);

	rec->entries[ent] = NULL;
	rec->entry_cnt--;
}


/* =======================
 * Component Table
 * ======================= */


CompTable* comp_table_new(u32 max_record_cnt) {
	CompTable* table = malloc(sizeof(CompTable));
	table->record_cnt = 0;
	table->max_record_cnt = max_record_cnt;
	table->records = malloc(sizeof(CompRecord) * max_record_cnt);
	return table;
}

void comp_table_delete(CompTable* table) {
	for (u32 i = 0; i < table->record_cnt; i++) {
		CompRecord* rec = table->records[i];
		comp_record_delete(rec);
	}
	free(table->records);
	free(table);
}

b32 __comp_table_search(CompTable* table, char* name) {
	for (u32 i = 0; i < table->record_cnt; i++) {
		CompRecord* rec = table->records[i];
		if (strcmp(name, rec->name) == 0) {
			return true;
		}
	}
	return false;
}

void __comp_table_add_record(CompTable* table, char* name) {
	assert(table->record_cnt < table->max_record_cnt, "Component table is full. Cant create new record `%s`.\n", name);
	table->records[table->record_cnt++] = comp_record_new(name, table->max_record_cnt);
}

CompRecord* __comp_table_get_record(CompTable* table, char* name) {
	for (u32 i = 0; i < table->record_cnt; i++) {
		CompRecord* rec = table->records[i];
		if (strcmp(name, rec->name) == 0) {
			return rec;
		}
	}
	return NULL;
}


/* =======================
 * Entity Component System
 * ======================= */


ECS* ecs_new(u32 max_entity_cnt) {
	ECS* ecs = malloc(sizeof(ECS));
	ecs->entity_cnt = 0;
	ecs->max_entity_cnt = max_entity_cnt;

	// mallocating entity slots
	ecs->slots = malloc(sizeof(EntitySlotState) * max_entity_cnt);
	memset(ecs->slots, FREE, sizeof(EntitySlotState) * max_entity_cnt);

	// Creating table
	ecs->table = comp_table_new(max_entity_cnt);

	return ecs;
}

void ecs_delete(ECS* ecs) {
	//TODO: Fix this with proper log
	if (ecs->entity_cnt) {
		printf("Not all the entities are deleted.\n");
	}

	comp_table_delete(ecs->table);
	free(ecs->slots);
	free(ecs);
}


/* =======================
 * Entity Commands
 * ======================= */


Entity entity_new(ECS* ecs) {
	assert(ecs->entity_cnt < ecs->max_entity_cnt, "Entity slots are full.\n");
	ecs->entity_cnt++;

	// Generating random entity id
	Entity id = rand_range(0, ecs->max_entity_cnt);
	do {
		id = rand_range(0, ecs->max_entity_cnt);
	} while (ecs->slots[id] != FREE);

	// Making the slot occupied
	ecs->slots[id] = OCCUPIED;
	return id;
}

void entity_delete(ECS* ecs, Entity ent) {
	assert(ecs->slots[ent] == OCCUPIED, "Entity `%d` doesnt exists.\n", ent);

	// Searching for components that entity has
	for (u32 i = 0; i < ecs->table->record_cnt; i++) {
		CompRecord* rec = ecs->table->records[i];
		if (comp_record_search(rec, ent)) {
			comp_record_remove_entry(rec, ent);
		}
	}

	// Reseting the slot
	ecs->slots[ent] = FREE;
	ecs->entity_cnt--;
}

void* __entity_add_component(ECS* ecs, Entity ent, char* name, void* data) {
	assert(ecs->slots[ent] == OCCUPIED, "Entity `%d` doesnt exists.\n", ent);

	if (!__comp_table_search(ecs->table, name)) {
		__comp_table_add_record(ecs->table, name);
	}

	CompRecord* rec  = __comp_table_get_record(ecs->table, name);
	CompEntry* entry = comp_record_add_entry(rec, ent, data);

	assert(entry, "Failed to add component `%s` in entity `%d`.\n", name, ent);
	return entry->data;
}

void* __entity_get_component(ECS* ecs, Entity ent, char* name) {
	assert(__comp_table_search(ecs->table, name), "Component `%s` is not in component table.\n", name);

	CompRecord* rec = __comp_table_get_record(ecs->table, name);
	assert(comp_record_search(rec, ent), "Entity `%d` doesnt have component `%s`.\n", ent, name);

	CompEntry* entry = comp_record_get_entry(rec, ent);
	assert(entry, "Entity `%d` doesnt have component `%s`\n", ent, name);

	return entry->data;
}

void __entity_remove_component(ECS* ecs, Entity ent, char* name) {
	assert(__comp_table_search(ecs->table, name), "Component `%s` is not in table.\n", name);

	CompRecord* rec = __comp_table_get_record(ecs->table, name);
	assert(comp_record_search(rec, ent), "Entity `%d` doesnt have component `%s`.\n", ent, name);

	comp_record_remove_entry(rec, ent);
}

