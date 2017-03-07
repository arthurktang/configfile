#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define STRING_EQUAL						0x00
#define STRING_NOTEQUAL						0x01
#define STRING_LEN_NOTEQUAL					0x02

#define KEY_NAME_NOTEXIST					0x03
#define SECTION_NOTEXIST					0x04
#define CONFIG_FILE_NOTEXIST				0x05

#define FUNC_ARG_ERR						0x10
#define FUNC_MALLOC_NULL					0x11

int ini_get_value (const char *filename, const char *section,
		const char *keyname, char *keyvalue);
int ini_get_value_int(const char *filename, const char *section,
		const char *keyname, int *keyvalue);

int cfg_init_handle(const char *filepath, void **handle);
int cfg_release_handle(void **handle);
int cfg_get_value(void *handle, const char *key, char *value);
int cfg_set_value(void *handle, const char *key, const char *value);
int cfg_add_key(void *handle, const char *key, const char *value);
int cfg_del_key(void *handle, const char *key);
int cfg_get_count(void *handle, int *count);
int cfg_traverse_nodes(void *handle, char ***keys, char ***values,
		int *count);
int cfg_free_collection(char ***keys, char ***values, int *count);

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H */
