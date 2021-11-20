#ifndef __FIRMWARE_DOWNLOAD_H
#define __FIRMWARE_DOWNLOAD_H

#include "sys.h"

void write_read_write_flash(u32 u32startaddr, u32 u32applen);
void firmware_setup(u32 addr, u8 *f_buf, u32 f_buf_len);
void firm_download(void);
void firm_init(void);
#endif
