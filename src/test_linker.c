#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

int main(int argc, char const *argv[])
{
	int ret = 0;
	void *handle = NULL;
	const char *param_path = "param.cfg";
	const char *cur_slink_3000 = "param/cur_3000";
	const char *fur_slink_3000 = "param/fur_3000";
	char cur_path[256] = {}, fur_path[256] = {}, tmp_path[256] = {};

	/* init cfg handle */
	ret = cfg_init_handle(param_path, &handle);
	if (ret) {
		printf("cfg_init_handle error: %d\n", ret);
		return 0;
	}

	/* cur 3000 */
	ret = cfg_get_value(handle, "cur_3000", cur_path);
	if (ret)
		printf("cfg_get_value error: %d\n", ret);
	else
		printf("cfg_get_value(cur_3000) %s\n", cur_path);

	/* fur 3000 */
	ret = cfg_get_value(handle, "fur_3000", fur_path);
	if (ret)
		printf("cfg_get_value error: %d\n", ret);
	else
		printf("cfg_get_value(fur_3000) %s\n", fur_path);

	/* download path not exist*/
	if (access(cur_path, F_OK)) {
		memset(cur_path, 0, sizeof(cur_path));
		ret = cfg_set_value(handle, "cur_3000", cur_path);
		if (ret)
			printf("cfg_set_value error: %d\n", ret);
		else
			printf("cfg_set_value(cur_3000) %s\n", cur_path);
	}

	/* unlink symbolic link file */
	unlink(cur_slink_3000);
	perror("unlink");

	/* link param folder */
	if (strlen(cur_path)) {
		memset(tmp_path, 0, sizeof(tmp_path));
		sprintf(tmp_path, "../%s", cur_path);
		if (symlink(tmp_path, cur_slink_3000)) {
			perror("symlink error");
			/* broken symbolic link */
			//remove(cur_slink_3000);
			//perror("remove2");
			return 0;
		} else {
			printf("symlink %s\n", tmp_path);
		}
	}



	/* release cfg handle */
	ret = cfg_release_handle(&handle);
	if (ret) {
		printf("cfg_release_handle error: %d\n", ret);
		return 0;
	}
	return 0;
}
