#include <stdio.h>
#include <string.h>
#include "config.h"

int main(int argc, char const *argv[])
{
	int ret = 0, age = 0, count = 0, i = 0;
	void *handle = NULL;
	const char *ini_path = "test.ini", *config_path = "test.cfg";
	char key_name[256] = {}, buf[256] = {};
	char **keys = NULL, **values = NULL;

	ret = ini_get_value(ini_path, "param", "fur", key_name);
	printf("result(%d), key_name(%s)\n", ret, key_name);

	ret = ini_get_value_int(ini_path, "arthur", "age", &age);
	printf("result(%d), key_name(%x)(%d)\n", ret, age, age);

	/* init handle */
	ret = cfg_init_handle(config_path, &handle);
	if (ret) {
		printf("cfg_init_handle error: %d\n", ret);
		return 0;
	}

	/* get the value */
	ret = cfg_get_value(handle, "host", buf);
	if (ret) {
		printf("cfg_get_value error: %d\n", ret);
	} else {
		printf("cfg_get_value(host) %s\n", buf);
	}
	ret = cfg_get_value(handle, "version", buf);
	if (ret) {
		printf("cfg_get_value error: %d\n", ret);
		return 0;
	} else {
		printf("cfg_get_value(version) %s\n", buf);
	}

	/* set the value */
	ret = cfg_set_value(handle, "version", "2.0.1");
	if (ret) {
		printf("cfg_set_value error: %d\n", ret);
		return 0;
	} else {
		printf("cfg_set_value success\n");
	}
	//memset(buf, 0, sizeof(buf));
	ret = cfg_get_value(handle, "version", buf);
	if (ret) {
		printf("cfg_get_value error: %d\n", ret);
		return 0;
	} else {
		printf("cfg_get_value(version) %s\n", buf);
	}

	/* add key */
	ret = cfg_add_key(handle, "pool_connection_countxx", "2000");
	if (ret) {
		printf("cfg_add_key error: %d\n", ret);
		return 0;
	}
	ret = cfg_get_value(handle, "version", buf);
	if (ret) {
		printf("cfg_get_value error: %d\n", ret);
		return 0;
	} else {
		printf("cfg_get_value(version) %s\n", buf);
	}

	/* del key */
	ret = cfg_del_key(handle, "connectionMax");
	if (ret) {
		printf("cfg_del_key error: %d\n", ret);
		//return 0;
	}

	/* traverse handle */
	ret = cfg_traverse_nodes(handle, &keys, &values, &count);
	if (ret) {
		printf("cfg_traverse_collection error: %d\n", ret);
		return 0;
	} else {
		for(i = 0; i < count; i++)
			printf("--->%s = %s\n", keys[i], values[i]);
	}
	ret = cfg_free_collection(&keys, &values, &count);
	if (!ret)
		printf("free success\n");

	return 0;
}
