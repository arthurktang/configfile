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
	char cur_path[256] = {}, fur_path[256] = {};

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

	}

	/* link param folder */
	if (access(cur_slink_3000, F_OK)) {
		//if (symlink(cur_path, cur_slink_3000)) {
		//	perror("symlink error");
		//	return 0;
		//} else {
		//	printf("symlink %s\n", cur_path);
		//}
	} else {
		/* remove file */
		if (remove(cur_slink_3000)) {
			perror("remove error");
			return 0;
		} else {
			printf("remove %s\n", cur_slink_3000);
		}
	}

	if (symlink(cur_path, cur_slink_3000)) {
		perror("symlink error");
		return 0;
	} else {
		printf("symlink %s\n", cur_path);
	}




	/* release cfg handle */
	ret = cfg_release_handle(&handle);
	if (ret) {
		printf("cfg_release_handle error: %d\n", ret);
		return 0;
	}
	return 0;
}
