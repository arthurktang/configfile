/*
 * Copyright (C) 2017 TangKai <kyotk@126.com>
 * FileName: config.c
 * Author: TangKai
 * Version: v0.01
 * Date: 2017-2-22
 * Description: get/set key value in configuration file and etc
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define LINE_BUF_SIZE		256	/* each line in the configuration file buffer size */
#define KEY_BUF_SIZE		128	/* key buffer size */
#define VALUE_BUF_SIZE		128	/* value buffer size*/

/*
 * Author: TangKai
 * Data: 2017-2-22
 * Description: the following content reference to the ini configuration file
 */

static int compare_string(const char *str1, const char *str2);
static int get_value(FILE *fp, const char *name, char *value);

/*
 * ini_get_value - get value by section and key
 *
 * Return: 0 on success or error number in case of failure
 */
int ini_get_value(const char *filename, const char *section,
		const char *keyname, char *keyvalue)
{
	FILE *fp = NULL;
	char buffer[LINE_BUF_SIZE] = {};
	char *str = NULL, *str_section = NULL;
	int result = 0;

	fp = fopen(filename, "r");
	if (NULL == fp)
		return CONFIG_FILE_NOTEXIST;

	while (!feof(fp)) {
		if (NULL == fgets(buffer, LINE_BUF_SIZE, fp))
			break;

		str = buffer;
		while ((' ' == *str) || ('\t' == *str))
			str++;

		/* '['section']' */
		if ('[' == *str) {
			str++;
			while ((' ' == *str) || ('\t' == *str))
				str++;
			str_section = str;
			while ((']' != *str) && ('\0' != *str))
				str++;
			if ('\0' == *str)
				continue;
			while (' ' == *(str - 1))
				str--;
			*str = '\0';

			result = strcmp(str_section, section);
			if (!result) {
				result = get_value(fp, keyname, keyvalue);
				fclose(fp);
				return result;
			}
		} /* if ('[' == *str) */
	} /* while (!feof(fp)) */
	fclose(fp);
	return SECTION_NOTEXIST;

	return 0;
}

/*
 * int_get_value_int - ini_get_value() string convert to hex or dec
 *
 * Return: 0 on success or error number in case of failure
 */
int ini_get_value_int(const char *filename, const char *section,
		const char *keyname, int *keyvalue)
{
	char *str_value = NULL;
	char value[KEY_BUF_SIZE] = {};
	int result = 0;

	result = ini_get_value(filename, section, keyname, value);
	if (result)
		return result;

	str_value = value;
	while ((' ' == *str_value) && ('\t' == *str_value))
		str_value++;
	if (('0' == *str_value) &&
			(('x' == *(str_value + 1)) || ('X' == *(str_value + 1))))
		sscanf(str_value + 2, "%x", keyvalue);
	else
		sscanf(str_value, "%d", keyvalue);

	return 0;
}

/*
 * compare_string - compare two strings be similar to strcmp()
 *
 * Return: 0 on success or error number in case of failure
 */
static int compare_string(const char *str1, const char *str2)
{
	if (strlen(str1) != strlen(str2))
		return STRING_LEN_NOTEQUAL;

	/*
	 * #include <ctype.h>
	 * toupper(): convert the letter c to upper case
	 * while(toupper(*str1) == toupper(*str2))
	 */
	while (*str1 == *str2) {
		if ('\0' == *str1)
			break;
		str1++;
		str2++;
	}

	if (('\0' == *str1) && ('\0' == *str2))
		return STRING_EQUAL;

	return STRING_NOTEQUAL;
}

/*
 * get_key_value - get key and value
 *
 * Return: 0 on success or error number in case of failure
 */
static int get_value(FILE *fp, const char *name, char *value)
{
	char buffer[LINE_BUF_SIZE] = {};
	char *str = NULL, *str_name = NULL, *str_value = NULL;
	int result = 0;

	while (!feof(fp)) {
		if (NULL == fgets(buffer, LINE_BUF_SIZE, fp))
			break;

		str = buffer;
		while ((' ' == *str) || ('\t' == *str))
			str++;

		/* continue '#' '//' '\0' 'CRLF' */
		if('#' == *str)
			continue;
		if(('/' == *str) && ('/' ==  *(str + 1)))
			continue;
		if(('\0' == *str) || (0x0d == *str) || (0x0a == *str))
			continue;

		/* break [...]*/
		str_name = str;
		if ('[' == *str) {
			while ((']' != *str) && ('\0' != *str))
				str++;
			if(']' == *str)
				break;
			str = str_name;
		}

		/* [...\0 or abc */
		while (('=' != *str) && ('\0' != *str))
			str++;
		if('\0' == *str)
			continue;
		str_value = str + 1;

		/* ...=... */
		if (str_name == str)
			continue;
		*str = '\0';

		/* delete space and tab */
		str--;
		while ((' ' == *str) || ('\t' == *str)) {
			*str = '\0';
			str--;
		}

		/* compared strcmp() */
		result = compare_string(str_name, name);
		if (!result) {
			str = str_value;
			while ((' ' == *str) || ('\t' == *str))
				str++;
			str_value = str;
			while (('\0' != *str) && (0x0d != *str) && (0x0a != *str)) {
				if (('/' == *str) && ('/' == *(str + 1)))
					break;
				str++;
			}
			*str = '\0';
			memcpy(value, str_value, strlen(str_value));

			return 0;
		}
	} /* while (!feof(fp)) */
	return KEY_NAME_NOTEXIST;
}

/*
 * Author: TangKai
 * Data: 2017-3-2
 * Description: the following content not include the section in configuration file
 */

typedef struct cfg_node {
	char *key;					/* key name */
	char *value;				/* key value */
	struct cfg_node *next;		/* next cfg_node */
}cfg_node;

typedef struct cfg_handle {
	char *filepath;				/* configuration file path */
	struct cfg_node *head;		/* head cfg_node */
}cfg_handle;

static int create_node(cfg_node **node);
static int delete_space(const char *src, char *des);
static int save_file(const char *filepath, cfg_node *head);

/*
 * cfg_release_handle - release each node and handle resources
 *
 * Return: 0 on success or error number in case of failure
 */
int cfg_release_handle(void **handle)
{
	cfg_node *cur_node = NULL, *tmp_node = NULL;
	cfg_handle *ph = NULL;

	if	(NULL == handle)
		return FUNC_ARG_ERR;

	/* release each node resources */
	ph = (cfg_handle *)*handle;
	cur_node = ph->head;
	while (NULL != cur_node) {
		if (NULL != cur_node->key) {
			free(cur_node->key);
			cur_node->key = NULL;
		}
		if (NULL != cur_node->value) {
			free(cur_node->value);
			cur_node->value = NULL;
		}
		tmp_node = cur_node->next;
		free(cur_node);
		cur_node = tmp_node;
	}

	/* release the handle resources */
	if (NULL != ph->filepath) {
		free(ph->filepath);
		ph->filepath = NULL;
	}

	free(ph);
	*handle = NULL;

	return 0;
}

/*
 * cfg_init_handle - initialize each node and handle resources
 *
 * Return: 0 on success or error number in case of failure
 */
int cfg_init_handle(const char *filepath, void **handle)
{
	int ret = 0;
	FILE *fp = NULL;
	cfg_node *head_node = NULL, *cur_node = NULL, *tmp_node = NULL;
	cfg_handle *ph = NULL;
	char line_data[LINE_BUF_SIZE] = {};
	char key_buffer[KEY_BUF_SIZE] = {};
	char value_buffer[VALUE_BUF_SIZE] = {};
	char *tmp = NULL;

	if ((NULL == filepath) || (NULL == handle))
		return FUNC_ARG_ERR;

	/* open configuration file */
	fp = fopen(filepath, "r");
	if (!fp)
		return CONFIG_FILE_NOTEXIST;

	/* create the handle */
	ph = (cfg_handle *)malloc(sizeof(cfg_handle));
	if (NULL == ph) {
		fclose(fp);
		fp = NULL;
		printf("cfg_init_handle(malloc) %ld bytes error\n", sizeof(cfg_handle));
		return FUNC_MALLOC_NULL;
	}
	memset(ph, 0, sizeof(cfg_handle));

	/* create head node */
	ret = create_node(&head_node);
	if(ret) {
		free(ph);
		ph = NULL;
		fclose(fp);
		fp = NULL;
		printf("cfg_init_handle(create_node) error: %d\n", ret);
		return ret;
	}
	memset(head_node, 0, sizeof(cfg_node));

	/* save file path and node */
	ph->head = head_node;
	ph->filepath = (char *)malloc(strlen(filepath) + 1);
	if (NULL == ph->filepath) {
		cfg_release_handle((void **)&ph);
		fclose(fp);
		fp = NULL;
		printf("cfg_init_handle(malloc) ph->filepath error\n");
		return FUNC_MALLOC_NULL;
	}
	strcpy(ph->filepath, filepath);

	cur_node = head_node;

	/* read all the data in the configuration file */
	while (!feof(fp)) {
		if (NULL == fgets(line_data, LINE_BUF_SIZE, fp))
			break;

		if (NULL == (tmp = strstr(line_data, "=")))
			continue;

		/* create a new node */
		ret = create_node(&tmp_node);
		if (ret) {
			cfg_release_handle((void **)&ph);
			fclose(fp);
			fp = NULL;
			printf("cfg_init_handle(create_node) error: %d\n", ret);
			return ret;
		}
		memset(key_buffer, 0, KEY_BUF_SIZE);
		memset(value_buffer, 0, VALUE_BUF_SIZE);
		*tmp = '\0';
		strcpy(key_buffer, line_data);
		delete_space(key_buffer, tmp_node->key);
		strcpy(value_buffer, tmp + 1);
		delete_space(value_buffer, tmp_node->value);

		/* insert a new node into the list */
		tmp_node->next = NULL;
		cur_node->next = tmp_node;
		cur_node = tmp_node;
	}

	*handle = ph;

	fclose(fp);
	fp = NULL;
	return 0;
}

/*
 * cfg_get_value - get value based on key
 *
 * Return: 0 on success or error number in case of failure
 */
int cfg_get_value(void *handle, const char *key, char *value)
{
	cfg_handle *ph = NULL;
	cfg_node *cur_node = NULL;
	char *ch = NULL;

	if (NULL == handle || NULL == key || NULL == value)
		return FUNC_ARG_ERR;

	/* find key */
	ph = (cfg_handle *)handle;
	cur_node = ph->head->next;
	while (NULL != cur_node) {
		if (!(strcmp(cur_node->key, key)))
			break;
		cur_node = cur_node->next;
	}
	if (NULL == cur_node)
		return KEY_NAME_NOTEXIST;

	strcpy(value, cur_node->value);

	/* delete CR */
	if(NULL != (ch = strchr(value, '\n')))
		*ch = '\0';

	return 0;
}

/*
 * cfg_set_value - set value based on key
 *
 * Return: 0 on success or error number in case of failure
 */
int cfg_set_value(void *handle, const char *key, const char *value)
{
	int ret = 0;
	cfg_handle *ph = NULL;
	cfg_node *cur_node = NULL;

	if (NULL == handle || NULL == key || NULL == value)
		return FUNC_ARG_ERR;

	/* find key */
	ph = (cfg_handle *)handle;
	cur_node = ph->head->next;
	while (NULL != cur_node) {
		if (!(strcmp(cur_node->key, key)))
			break;
		cur_node = cur_node->next;
	}

	if (NULL == cur_node)
		return KEY_NAME_NOTEXIST;

	/* modify value */
	strcpy(cur_node->value, value);
	if (NULL == strchr(value, '\n'))
		strcat(cur_node->value, "\n");

	ret = save_file(ph->filepath, ph->head);

	return ret;
}

/*
 * cfg_add_key - add a key and value
 *
 * Return: 0 on success or error number in case of failure
 */
int cfg_add_key(void *handle, const char *key, const char *value)
{
	int ret = 0;
	cfg_handle *ph = NULL;
	cfg_node *cur_node = NULL, *tmp_node = NULL;

	if (NULL == handle || NULL == key || NULL == value)
		return FUNC_ARG_ERR;

	ph = (cfg_handle *)handle;
	cur_node = ph->head;
	while (NULL != cur_node->next) {
		if(!(strcmp(cur_node->next->key, key)))
			break;
		cur_node = cur_node->next;
	}

	/* node exist */
	if (NULL != cur_node->next)
		return cfg_set_value(handle, key, value);

	/* node not exist */
	ret = create_node(&tmp_node);
	if (ret) {
		printf("cfg_add_key(create_node) error: %d\n", ret);
		return ret;
	}

	/* add a new node */
	if (NULL == strchr(cur_node->value, '\n'))
		strcat(cur_node->value, "\n");
	strcpy(tmp_node->key, key);
	strcpy(tmp_node->value, value);
	if (NULL == strchr(tmp_node->value, '\n'))
		strcat(tmp_node->value, "\n");
	cur_node->next = tmp_node;

	ret = save_file(ph->filepath, ph->head);

	return ret;
}

/*
 * cfg_del_key - delete the key and value
 *
 * Return: 0 on success or error number in case of failure
 */
int cfg_del_key(void *handle, const char *key)
{
	int ret = 0;
	cfg_handle *ph = NULL;
	cfg_node *cur_node = NULL, *prev_node = NULL;

	if (NULL == handle || NULL == key)
		return FUNC_ARG_ERR;

	/* find key*/
	ph = (cfg_handle *)handle;
	prev_node = ph->head;
	cur_node = ph->head->next;
	while (NULL != cur_node) {
		if(!(strcmp(cur_node->key, key)))
			break;
		prev_node = cur_node;
		cur_node = cur_node->next;
	}

	/* node not exist */
	if(NULL == cur_node)
		return KEY_NAME_NOTEXIST;

	/* delete the node */
	prev_node->next = cur_node->next;
	free(cur_node->key);
	free(cur_node->value);
	free(cur_node);
	cur_node->key = NULL;
	cur_node->value = NULL;
	cur_node = NULL;

	ret = save_file(ph->filepath, ph->head);

	return ret;
}

/*
 * cfg_get_count - statistical node number
 *
 * Return: 0 on success or error number in case of failure
 */
int cfg_get_count(void *handle, int *count)
{
	int node_count = 0;
	cfg_handle *ph = NULL;
	cfg_node *cur_node = NULL;

	if (NULL == handle || NULL == count)
		return FUNC_ARG_ERR;

	ph = (cfg_handle *)handle;
	cur_node = ph->head->next;
	while (NULL != cur_node) {
		node_count++;
		cur_node = cur_node->next;
	}
	*count = node_count;

	return 0;
}

/*
 * cfg_traverse_nodes - traverse all node input keys and values
 *
 * Return: 0 on success or error number in case of failure
 */
int cfg_traverse_nodes(void *handle, char ***keys, char ***values, int *count)
{
	int ret = 0, node_count = 0, index = 0;
	cfg_handle *ph = NULL;
	cfg_node *cur_node = NULL;
	char *ch = NULL;
	char **arr_key = NULL, **arr_value = NULL;

	if (NULL == handle || NULL == keys || NULL == values || NULL == count)
		return FUNC_ARG_ERR;

	/* compute node number */
	ret = cfg_get_count(handle, &node_count);
	if (ret) {
		printf("cfg_traverse_collection(cfg_get_count) error: %d\n", ret);
		return ret;
	}

	arr_key = (char **)malloc(sizeof(char *) * node_count);
	if (NULL == arr_key) {
		printf("cfg_traverse_collection(malloc) arr_key error \n");
		return FUNC_MALLOC_NULL;
	}
	arr_value = (char **)malloc(sizeof(char *) * node_count);
	if (NULL == arr_value) {
		free(arr_key);
		arr_key = NULL;
		printf("cfg_traverse_collection(malloc) arr_value error \n");
		return FUNC_MALLOC_NULL;
	}

	/* traverse all nodes */
	ph = (cfg_handle *)handle;
	cur_node = ph->head->next;
	while (NULL != cur_node) {
		*(arr_key + index) = cur_node->key;
		*(arr_value + index) = cur_node->value;

		/* delete CR */
		if(NULL != (ch = strchr(*(arr_value + index), '\n')))
			*ch = '\0';

		cur_node = cur_node->next;
		index++;
	}

	*keys = arr_key;
	*values = arr_value;
	*count = node_count;

	return 0;
}

/*
 * cfg_free_collection - free all keys and values
 *
 * Return: 0 on success or error number in case of failure
 */
int cfg_free_collection(char ***keys, char ***values, int *count)
{
	int node_count = 0, index = 0;

	if (NULL == keys || NULL == values || NULL == count)
		return FUNC_ARG_ERR;

	free(*keys);
	free(*values);
	*keys = NULL;
	*values = NULL;
	*count = 0;

	return 0;
}

/*
 * create_node - create a cfg_node
 *
 * Return: 0 on success or error number in case of failure
 */
static int create_node(cfg_node **node)
{
	cfg_node *p = NULL;

	if (NULL == node)
		return FUNC_ARG_ERR;

	p = (cfg_node *)malloc(sizeof(cfg_node));
	if (NULL == p) {
		printf("create_node(malloc) %ld bytes error\n", sizeof(cfg_node));
		return FUNC_MALLOC_NULL;
	}

	p->key = (char *)malloc(KEY_BUF_SIZE);
	if (NULL == p->key) {
		free(p);
		p = NULL;
		printf("create_node(malloc) p->key error\n");
		return FUNC_MALLOC_NULL;
	}

	p->value = (char *)malloc(VALUE_BUF_SIZE);
	if (NULL == p->value) {
		free(p->key);
		p->key = NULL;
		free(p);
		p = NULL;
		printf("create_node(malloc) p->value error\n");
		return FUNC_MALLOC_NULL;
	}
	*node = p;

	return 0;
}

/*
 * delete_space - delete the head and tail string space
 *
 * Return: 0 on success or error number in case of failure
 */
static int delete_space(const char *src, char *des)
{
	unsigned int i = 0, j = 0, len = 0;
	const char *str = src;

	if (NULL == src || NULL == des)
		return FUNC_ARG_ERR;

	while (' ' == *(str + i))
		i++;

	j = strlen(str);
	if (i < j) {
		j--;
		while (' ' == *(str + j))
			j--;
	}

	/* string length*/
	len = j - i + 1;
	memcpy(des, str + i, len);
	*(des + len) = '\0';

	return 0;
}

/*
 * save_file - save configuration file
 *
 * Return: 0 on success or error number in case of failure
 */
static int save_file(const char *filepath, cfg_node *head)
{
	FILE *fp = NULL;
	cfg_node *cur_node = NULL;

	if (NULL == filepath || NULL == head)
		return FUNC_ARG_ERR;

	fp = fopen(filepath, "w");
	if (NULL == fp)
		return CONFIG_FILE_NOTEXIST;

	cur_node = head->next;
	while (NULL != cur_node) {
		fprintf(fp, "%s=%s", cur_node->key, cur_node->value);
		cur_node = cur_node->next;
	}

	fclose(fp);

	return 0;
}


























