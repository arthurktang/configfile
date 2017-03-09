/*
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "config.h"
#include "linker.h"

#define PATH_BUF_SIZE		256	/* file path buffer size */

static void delete_dir(char *path);

/*
 * lnk_open - reference cfg_init_handle in config.h
 *
 * Return: 0 on success or error number in case of failure
 */
int lnk_open(const char *cfgpath, void **lnk_handle)
{
	return cfg_init_handle(cfgpath, lnk_handle);
}

/*
 * lnk_set - reference cfg_set_value in config.h
 *
 * Return: 0 on success or error number in case of failure
 */
int lnk_set(void *lnk_handle, const char *linkpath, const char *dirpath)
{
	return cfg_set_value(lnk_handle, linkpath, dirpath);
}

/*
 * lnk_set - reference cfg_release_handle in config.h
 *
 * Return: 0 on success or error number in case of failure
 */
int lnk_close(void **lnk_handle)
{
	return cfg_release_handle(lnk_handle);
}

/*
 * lnk_switch - switch two directory path in configuration file
 *
 * set "dest" to "src" and set "src" to NULL
 *
 * Return: 0 on success or error number in case of failure
 */
int lnk_switch(void *lnk_handle, const char *dest, const char *src)
{
	int ret = 0;
	char dest_path[PATH_BUF_SIZE] = {}, src_path[PATH_BUF_SIZE] = {};
	char tmp_path[PATH_BUF_SIZE] = {};

	if (NULL == lnk_handle || NULL == dest || NULL == src)
		return FUNC_ARG_ERR;

	ret = cfg_get_value(lnk_handle, dest, dest_path);
	if (ret) {
		printf("lnk_switch(cfg_get_value) dest error %d\n", ret);
		return CFG_GET_VALUE_ERR;
	}
	ret = cfg_get_value(lnk_handle, src, src_path);
	if (ret) {
		printf("lnk_switch(cfg_get_value) src error %d\n", ret);
		return CFG_GET_VALUE_ERR;
	}

	/* check directory exist */
	if (access(dest_path, F_OK)) {
		memset(dest_path, 0, PATH_BUF_SIZE);
		ret = cfg_set_value(lnk_handle, dest, dest_path);
		if (ret) {
			printf("lnk_switch(cfg_set_value) dest error: %d\n", ret);
			return CFG_SET_VALUE_ERR;
		}
	}
	if (access(src_path, F_OK)) {
		memset(src_path, 0, PATH_BUF_SIZE);
		ret = cfg_set_value(lnk_handle, src, src_path);
		if (ret) {
			printf("lnk_switch(cfg_set_value) src error: %d\n", ret);
			return CFG_SET_VALUE_ERR;
		}
	}

	if (strlen(src_path)) {
		if (strlen(dest_path)) {
			/* unlink symbolic link file */
			if (!access(dest, F_OK)) {
				ret = unlink(dest);
				if (ret) {
					printf("lnk_switch(unlink) error %d(%s)\n", ret, dest);
					perror("lnk_switch(unlink)");
					return UNLINK_SYMBOLIC_ERR;
				}
			}

			/* delete directory */
			delete_dir(dest_path);
		}
	} else {
		return SRC_ARG_EMPTY;
	}

	/* switch path */
	memset(dest_path, 0, PATH_BUF_SIZE);
	memcpy(dest_path, src_path, PATH_BUF_SIZE);
	ret = cfg_set_value(lnk_handle, dest, dest_path);
	if (ret) {
		printf("lnk_switch(cfg_set_value) dest error: %d\n", ret);
		return CFG_SET_VALUE_ERR;
	}
	memset(src_path, 0, PATH_BUF_SIZE);
	ret = cfg_set_value(lnk_handle, src, src_path);
	if (ret) {
		printf("lnk_switch(cfg_set_value) src error: %d\n", ret);
		return CFG_SET_VALUE_ERR;
	}

	/* symbolic link folder */
	sprintf(tmp_path, "../%s", dest_path);
	if (symlink(tmp_path, dest)) {
		printf("lnk_switch(symlink) %s %s\n", tmp_path, dest);
		perror("lnk_switch(symlink)");
		return SYM_LINK_ERR;
	}

	return 0;
}


/*
 * delete_dir - recursive delete not empty directory
 *
 * Return: void
 */
static void delete_dir(char *path)
{
	char path_buffer[PATH_BUF_SIZE] = {};
	DIR *dir = NULL;
	struct dirent *ent = NULL;

	dir = opendir(path);
	if (NULL == dir) {
		perror("delete_dir(opendir)");
		return ;
	}

	/* recursive delete not empty directory */
	while (NULL != (ent = readdir(dir))) {
		if (!(strcmp(".", ent->d_name)) || !(strcmp("..", ent->d_name)))
			continue;
		memset(path_buffer, 0, PATH_BUF_SIZE);
		sprintf(path_buffer, "%s/%s", path, ent->d_name);

		if (DT_DIR == ent->d_type) {
			delete_dir(path_buffer);
		} else {
			/* file unlink() directory rmdir() */
			if (remove(path_buffer)) {
				printf("delete_dir(remove) %s\n", path_buffer);
				perror("delete_dir(remove)");
			}
		}
	}
	closedir(dir);

	/* delete empty directory */
	if (rmdir(path)) {
		printf("delete_dir(rmdir) %s\n", path);
		perror("delete_dir(rmdir)");
	}

}
