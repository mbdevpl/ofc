#ifndef __file_h__
#define __file_h__

#include <stdbool.h>

typedef struct file_s file_t;

/* Path must be valid for as long as the file_t* is */
file_t* file_create(const char* path);
void    file_delete(file_t* file);

const char* file_get_path(const file_t* file);
const char* file_get_strz(const file_t* file);

bool file_get_position(
	const file_t* file, const char* ptr,
	unsigned* row, unsigned* col);

#endif