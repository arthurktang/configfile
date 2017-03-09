#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>

#include "config.h"

void printf_dir(const char *path)
{
	char buf[128] = {};
	DIR *dir = opendir(path);
	struct dirent *ent = NULL;

	if (NULL == dir)
		return ;
	while (NULL != (ent = readdir(dir))) {
		if (!(strcmp(".", ent->d_name)) || !(strcmp("..", ent->d_name)))
			continue;
		if (DT_DIR == ent->d_type) {
			printf("[%s]\n", ent->d_name);
			sprintf(buf, "%s/%s", path, ent->d_name);
			printf_dir(buf);
		} else {
			printf("%s\n", ent->d_name);
			//remove(ent->d_name);
			//perror("remove file");
		}
	}
	closedir(dir);
	//remove(path);
	//perror("remove dir");
}

int main(int argc, char const *argv[])
{
	int ret = 0;
	void *lnk_handle = NULL;
	const char *param_path = "param.cfg";
	const char *cur_slink_3000 = "param/cur_3000";
	const char *fur_slink_3000 = "param/fur_3000";
	char cur_path[256] = {}, fur_path[256] = {}, tmp_path[256] = {};

	/* init cfg handle */
	//ret = cfg_init_handle(param_path, &lnk_handle);
	ret = lnk_open(param_path, &lnk_handle);
	if (ret) {
		printf("lnk_open error: %d\n", ret);
		return 0;
	}

	/* cur 3000 */
	ret = cfg_get_value(lnk_handle, cur_slink_3000, cur_path);
	if (ret)
		printf("cfg_get_value error: %d\n", ret);
	else
		printf("cfg_get_value(cur_slink_3000) %s\n", cur_path);

	/* fur 3000 */
	ret = cfg_get_value(lnk_handle, fur_slink_3000, fur_path);
	if (ret)
		printf("cfg_get_value error: %d\n", ret);
	else
		printf("cfg_get_value(fur_slink_3000) %s\n", fur_path);

	/* switch directory path */
	ret = lnk_switch(lnk_handle, cur_slink_3000, fur_slink_3000);
	if (ret) {
		printf("lnk_switch error: %d\n", ret);
		return 0;
	}
	printf_dir(cur_slink_3000);

	ret = lnk_set(lnk_handle, fur_slink_3000, cur_path);
	if (ret) {
		printf("lnk_set error: %d\n", ret);
		return 0;
	}

	/* release cfg handle */
	//ret = cfg_release_handle(&lnk_handle);
	ret = lnk_close(&lnk_handle);
	if (ret) {
		printf("cfg_release_handle error: %d\n", ret);
		return 0;
	}
	return 0;
}
