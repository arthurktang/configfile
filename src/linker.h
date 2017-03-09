#ifndef __LINKER_H
#define __LINKER_H

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_GET_VALUE_ERR				0x01
#define CFG_SET_VALUE_ERR				0x02
#define SRC_ARG_EMPTY					0x03
#define UNLINK_SYMBOLIC_ERR				0x04
#define SYM_LINK_ERR					0x05

int lnk_switch(void *lnk_handle, const char *dest, const char *src);

#ifdef __cplusplus
}
#endif

#endif /* __LINKER_H */
