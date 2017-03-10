### Configfile
Get/set operations such as keys in the configuration file

### Synopsis
**Hard Link**： 相同inode号仅文件名不同的文件
* 相同inode 及data block；
* 只能对已存在的文件进行创建；
* 不能交叉文件系统进行硬链接的创建；
* 不能对目录进行创建，只可对文件创建；
* 删除一个硬链接文件并不影响其他有相同inode号的文件

**Soft Link** (symbolic Link)：有着自己的inode号以及用户数据块，文件用户数据块中存放的内容是另一文件的路径名的指向
* 软链接有自己的文件属性及权限等；
* 可对不存在的文件或目录创建软链接；
* 软链接可交叉文件系统；
* 软链接可对文件或目录创建；
* 创建软链接时，链接计数i_nlink不会增加；
* 删除软链接并不影响被指向的文件，但若被指向的原文件被删除，则相关软连接被称为死链接（即dangling link，若被指向路径文件被重新创建，死链接可恢复为正常的软链接）。

```
#include <unistd.h>
int link(const char *oldpath, const char *newpath);

#include <unistd.h>
int symlink(const char *oldpath, const char *newpath);

#include <unistd.h>
int readlink(const char *path, char *buf, size_t bufsiz);
```
### Process
1. `lnk_open` 打开.cfg配置文件，加载符号连接路径和目录文件路径，维护lnk_handle；
2. `lnk_set` 修改目录文件路径；
3. `lnk_switch` 删除旧符号连接，切换目录文件路径建立新连接；
4. `lnk_close` 释放lnk_handle占用资源
