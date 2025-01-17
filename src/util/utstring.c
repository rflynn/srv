/* string.c
 * Copyright (c) 2006
 * Jeff Nettleton
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "util.h"
#include "vector.h"

/* split a string */
char **strsplit(const char *str, const char *delim, unsigned int max,
		unsigned int *count)
{
	char *tmp, *copy;
	char **strings;
	unsigned int i;

#ifdef DEBUG
	assert(NULL != str);
	assert(NULL != delim);
	assert(0 != max);
#endif

	copy = strdup(str);
	strings = malloc(max * sizeof *strings);

	if (NULL == strings) {
		ERRF(__FILE__, __LINE__, "allocating for string array!\n");
		return NULL;
	}

	for (i = 0; i < max && NULL != (tmp = strsep(&copy, delim));) {
		if (!strlen(tmp))
			continue;

		strings[i++] = strdup(tmp);
	}

	if (NULL != copy)
		free(copy);

	*count = i;

	return strings;
}

/* join a split string together */
char *strjoin(const char **words, const char *joiner, unsigned int cnt)
{
	char *newstr;
	size_t newlen;
	unsigned int i;

#ifdef DEBUG
	assert(NULL != words);
	assert(NULL != joiner);
#endif

	newlen = 1;

	for (i = 0; i < cnt; i++) {
		/* calculate the length of the new string */
		newlen += strlen(words[i]);
	}

	newlen += strlen(joiner) * (cnt - 1);

	newstr = malloc(newlen);

	if (NULL == newstr) {
		ERRF(__FILE__, __LINE__, "allocating for a new string!\n");
		return NULL;
	}

	/* create string */
	for (i = 0; i < (cnt - 1); i++) {
		strcat(newstr, words[i]);
		strcat(newstr, joiner);
	}

	strcat(newstr, words[i]);

	return newstr;
}

/* get a segment of a string */
char *strsubstr(const char *str, unsigned int length, int offset)
{
	char *new = NULL;

#ifdef DEBUG
	assert(NULL != str);
#endif

	if (length > strlen(str)) {
		/* impossible ! */
		if (offset >= 0)
			length = (unsigned)strlen(str) - offset;
		else
			length = -1 * offset;
	}

	new = malloc(length + 1);	/* may not be this long, but this is the max */

	if (NULL == new) {
		ERRF(__FILE__, __LINE__, "allocating in strsubstr!\n");
		return NULL;
	}

	if (offset >= 0)
		strncpy(new, str + offset, length);
	else
		strncpy(new, str + (strlen(str) + offset), length);

	return new;
}

/* replace part of a string with another */
char *strreplace(char *str, char *old, char *new, unsigned int cnt)
{
	char *fresh, *p;
	char null = '\0';
	unsigned int i = 0;
	unsigned int j, status = 0;
	size_t oldlen, newlen;
	vector_t *vec;

#ifdef DEBUG
	assert(NULL != str);
	assert(NULL != old);
	assert(NULL != new);
#endif

	vec = vector_new(2, sizeof *p);
	oldlen = strlen(old);
	newlen = strlen(new);

	for (p = str; '\0' != *p; p++) {
		if (i == cnt && cnt) {
			status = 2;
			break;
		}

		if (*p != *old) {
			vector_push(vec, p);
			continue;
		}

		if (strlen(p) < oldlen) {
			status = 2;
			break;
		}

		if (!strncmp(p, old, oldlen)) {
			/* they match */
			for (j = 0; j < newlen; j++) {
				vector_push(vec, new + j);
			}

			i++;
			p += oldlen - 1;
			continue;
		} else {
			vector_push(vec, p);
			continue;
		}
	}

	if (2 == status) {
		for (; '\0' != *p; p++) {
			vector_push(vec, p);
		}
	}

	if (NULL != (char *)vector_get_at(vec, vec->count - 1)) {
		vector_push(vec, &null);
	}

	fresh = strdup((const char *)vec->data);
	vector_free(vec);

	return fresh;
}
