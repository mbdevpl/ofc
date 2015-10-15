#ifndef __ofc_sema_decl_h__
#define __ofc_sema_decl_h__

#include <ofc/hashmap.h>

typedef struct ofc_sema_decl_s ofc_sema_decl_t;
typedef struct ofc_sema_equiv_s ofc_sema_equiv_t;


struct ofc_sema_equiv_s
{
	unsigned          count;
	ofc_sema_decl_t** decl;
};

struct ofc_sema_decl_s
{
	ofc_sema_type_t* type;
	ofc_str_ref_t    name;
	void*            init;

    bool is_static;
	bool is_volatile;
	bool is_automatic;
	bool is_target;

	ofc_sema_equiv_t* equiv;
};

ofc_sema_decl_t* ofc_sema_decl(
	const ofc_parse_stmt_t* stmt);
void ofc_sema_decl_delete(
	ofc_sema_decl_t* decl);

unsigned ofc_sema_decl_size(
	const ofc_sema_decl_t* decl);

bool ofc_sema_decl_equiv(
	ofc_sema_decl_t* a,
	ofc_sema_decl_t* b);


typedef struct
{
	bool              ignore_case;
	unsigned          count;
	ofc_sema_decl_t** decl;
	ofc_hashmap_t*    map;
} ofc_sema_decl_list_t;

ofc_sema_decl_list_t* ofc_sema_decl_list_create(bool ignore_case);
void ofc_sema_decl_list_delete(ofc_sema_decl_list_t* list);

bool ofc_sema_decl_list_add(
	ofc_sema_decl_list_t* list,
	ofc_sema_decl_t* decl);

ofc_sema_decl_t* ofc_sema_decl_list_find(
	ofc_sema_decl_list_t* list,
	ofc_str_ref_t name);

#endif
