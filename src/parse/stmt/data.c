#include "../parse.h"


static unsigned parse_stmt_data__nlist(
	const sparse_t* src, const char* ptr,
	parse_lhs_t** name, unsigned* count, unsigned* max_count)
{
	unsigned ocount = *count;
	unsigned i = 0, c;
	for (c = 0; ptr[i] != '/'; c++)
	{
		unsigned j = i;
		if (c > 0)
		{
			if (ptr[j] != ',')
				break;
			j += 1;
		}

		parse_lhs_t lhs;
		unsigned len = parse_lhs(
			src, &ptr[j], &lhs);
		if (len == 0) break;

		if (*count >= *max_count)
		{
			unsigned ncount = (*max_count << 1);
			if (ncount == 0) ncount = 16;
			parse_lhs_t* nname
				= (parse_lhs_t*)realloc(*name,
					(sizeof(parse_lhs_t) * ncount));
			if (!nname)
			{
				parse_lhs_cleanup(lhs);
				unsigned e;
				for (e = ocount; e < *count; e++)
					parse_lhs_cleanup((*name)[e]);
				*count = ocount;
				return 0;
			}

			*name      = nname;
			*max_count = ncount;
		}

		(*name)[(*count)++] = lhs;
		i = (j + len);
	}

	return i;
}

static unsigned parse_stmt_data__clist(
	const sparse_t* src, const char* ptr,
	parse_expr_t** init, unsigned* count, unsigned* max_count)
{
	unsigned ocount = *count;

	unsigned i = 0;

	if (ptr[i++] != '/')
		return 0;

	unsigned c = 0;
	while (ptr[i] != '/')
	{
		unsigned j = i;
		if (c > 0)
		{
			if (ptr[j] != ',')
				break;
			j += 1;
		}

		unsigned ec = 1;
		unsigned len = parse_unsigned(
			src, &ptr[j], &ec);
		if ((len > 0) && (ptr[j + len] == '*'))
			j += (len + 1);
		else
			ec = 1;

		parse_expr_t expr;
		len = parse_expr(
			src, &ptr[j], &expr);
		if (len == 0) break;
		i = (j + len);

		if ((*count + ec) > *max_count)
		{
			unsigned ncount = *max_count;
			if (ncount == 0) ncount = 16;
			while ((*count + ec) > ncount)
				ncount <<= 1;

			parse_expr_t* ninit
				= (parse_expr_t*)realloc(*init,
					(sizeof(parse_expr_t) * ncount));
			if (!ninit)
			{
				unsigned e;
				for (e = ocount; e < *count; e++)
					parse_expr_cleanup((*init)[e]);
				*count = ocount;
				return 0;
			}

			*init      = ninit;
			*max_count = ncount;
		}

		(*init)[(*count)++] = expr;

		unsigned e;
		for (e = 1; e < ec; e++)
		{
			if (!parse_expr_clone(
				&((*init)[*count]), &expr))
			{
				/* This should never fail. */
				abort();
			}
			*count += 1;
		}

		c += ec;
	}

	if (ptr[i++] != '/')
	{
		unsigned e;
		for (e = ocount; e < *count; e++)
			parse_expr_cleanup((*init)[e]);
		*count = ocount;
		return 0;
	}

	return i;
}

static unsigned parse_stmt_data__list(
	const sparse_t* src, const char* ptr,
	parse_stmt_t* stmt)
{
	unsigned i = 0;

	unsigned nmax = stmt->data.name_count, cmax = stmt->data.init_count;

	unsigned len = parse_stmt_data__nlist(
		src, &ptr[i], &stmt->data.name, &stmt->data.name_count, &nmax);
	if (len == 0) return 0;
	i += len;

	len = parse_stmt_data__clist(
		src, &ptr[i], &stmt->data.init, &stmt->data.init_count, &cmax);
	if (len == 0) return 0;
	i += len;

	return i;
}

unsigned parse_stmt_data(
	const sparse_t* src, const char* ptr,
	parse_stmt_t* stmt)
{
	unsigned i = parse_keyword(
		src, ptr, PARSE_KEYWORD_DATA);
	if (i == 0) return 0;

	stmt->type = PARSE_STMT_DATA;
	stmt->data.name_count = 0;
	stmt->data.name = NULL;
	stmt->data.init_count = 0;
	stmt->data.init = NULL;

	unsigned len = parse_stmt_data__list(
		src, &ptr[i], stmt);
	if (len == 0) return 0;
	i += len;

	while (len > 0)
	{
		/* TODO - Handle multiple list separately so
		          we can recover properly if the number of elements
		          doesn't match the name list. */

		len = parse_stmt_data__list(
			src, &ptr[i], stmt);
		i += len;
	}

	return i;
}
