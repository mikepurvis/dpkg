/*
 * libdpkg - Debian packaging suite library routines
 * buffer.h - buffer I/O handling routines
 *
 * Copyright © 1999, 2000 Wichert Akkerman <wakkerma@debian.org>
 * Copyright © 2000-2003 Adam Heath <doogie@debian.org>
 * Copyright © 2005 Scott James Remnant
 * Copyright © 2008, 2009 Guillem Jover <guillem@debian.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef DPKG_BUFFER_H
#define DPKG_BUFFER_H

#include <sys/types.h>

#include <dpkg/macros.h>

DPKG_BEGIN_DECLS

#define BUFFER_WRITE_BUF		0
#define BUFFER_WRITE_VBUF		1
#define BUFFER_WRITE_FD			2
#define BUFFER_WRITE_NULL		3
#define BUFFER_WRITE_STREAM		4
#define BUFFER_WRITE_MD5		5

#define BUFFER_READ_FD			0
#define BUFFER_READ_STREAM		1

#define BUFFER_WRITE_SETUP		1 << 16
#define BUFFER_READ_SETUP		1 << 17
#define BUFFER_WRITE_SHUTDOWN		1 << 18
#define BUFFER_READ_SHUTDOWN		1 << 19

typedef struct buffer_data *buffer_data_t;

typedef off_t (*buffer_proc_t)(buffer_data_t data, void *buf, off_t size,
                               const char *desc);

typedef union buffer_arg {
	void *ptr;
	int i;
} buffer_arg;

struct buffer_data {
	buffer_proc_t proc;
	buffer_arg data;
	int type;
};

#if HAVE_C99
# define fd_md5(fd, hash, limit, ...) \
	buffer_copy_setup_IntPtr(fd, BUFFER_READ_FD, NULL, \
	                         hash, BUFFER_WRITE_MD5, NULL, \
	                         limit, __VA_ARGS__)
# define stream_md5(file, hash, limit, ...) \
	buffer_copy_setup_PtrPtr(file, BUFFER_READ_STREAM, NULL, \
	                         hash, BUFFER_WRITE_MD5, NULL, \
	                         limit, __VA_ARGS__)
# define fd_fd_copy(fd1, fd2, limit, ...) \
	buffer_copy_setup_IntInt(fd1, BUFFER_READ_FD, NULL, \
	                         fd2, BUFFER_WRITE_FD, NULL, \
	                         limit, __VA_ARGS__)
# define fd_buf_copy(fd, buf, limit, ...) \
	buffer_copy_setup_IntPtr(fd, BUFFER_READ_FD, NULL, \
	                         buf, BUFFER_WRITE_BUF, NULL, \
	                         limit, __VA_ARGS__)
# define fd_vbuf_copy(fd, buf, limit, ...) \
	buffer_copy_setup_IntPtr(fd, BUFFER_READ_FD, NULL, \
	                         buf, BUFFER_WRITE_VBUF, NULL, \
	                         limit, __VA_ARGS__)
# define fd_null_copy(fd, limit, ...) \
	if (lseek(fd, limit, SEEK_CUR) == -1) { \
		if (errno != ESPIPE) \
			ohshite(__VA_ARGS__); \
		buffer_copy_setup_IntPtr(fd, BUFFER_READ_FD, NULL, \
		                         NULL, BUFFER_WRITE_NULL, NULL, \
		                         limit, __VA_ARGS__); \
	}
# define stream_null_copy(file, limit, ...) \
	if (fseek(file, limit, SEEK_CUR) == -1) { \
		if (errno != EBADF) \
			ohshite(__VA_ARGS__); \
		buffer_copy_setup_PtrPtr(file, BUFFER_READ_STREAM, NULL, \
		                         NULL, BUFFER_WRITE_NULL, NULL, \
		                         limit, __VA_ARGS__); \
	}
# define stream_fd_copy(file, fd, limit, ...) \
	buffer_copy_setup_PtrInt(file, BUFFER_READ_STREAM, NULL, \
	                         fd, BUFFER_WRITE_FD, NULL, \
	                         limit, __VA_ARGS__)
#else /* HAVE_C99 */
# define fd_md5(fd, hash, limit, desc...) \
	buffer_copy_setup_IntPtr(fd, BUFFER_READ_FD, NULL, \
	                         hash, BUFFER_WRITE_MD5, NULL, \
	                         limit, desc)
# define stream_md5(file, hash, limit, desc...) \
	buffer_copy_setup_PtrPtr(file, BUFFER_READ_STREAM, NULL, \
	                         hash, BUFFER_WRITE_MD5, NULL, \
	                         limit, desc)
# define fd_fd_copy(fd1, fd2, limit, desc...) \
	buffer_copy_setup_IntInt(fd1, BUFFER_READ_FD, NULL, \
	                         fd2, BUFFER_WRITE_FD, NULL, \
	                         limit, desc)
# define fd_buf_copy(fd, buf, limit, desc...) \
	buffer_copy_setup_IntPtr(fd, BUFFER_READ_FD, NULL, \
	                         buf, BUFFER_WRITE_BUF, NULL, \
	                         limit, desc)
# define fd_vbuf_copy(fd, buf, limit, desc...) \
	buffer_copy_setup_IntPtr(fd, BUFFER_READ_FD, NULL, \
	                         buf, BUFFER_WRITE_VBUF, NULL, \
	                         limit, desc)
# define fd_null_copy(fd, limit, desc...) \
	if (lseek(fd, limit, SEEK_CUR) == -1) { \
		if (errno != ESPIPE) \
			ohshite(desc); \
		buffer_copy_setup_IntPtr(fd, BUFFER_READ_FD, NULL, \
		                         NULL, BUFFER_WRITE_NULL, NULL, \
		                         limit, desc); \
	}
# define stream_null_copy(file, limit, desc...) \
	if (fseek(file, limit, SEEK_CUR) == -1) { \
		if (errno != EBADF) \
			ohshite(desc); \
		buffer_copy_setup_PtrPtr(file, BUFFER_READ_STREAM, NULL, \
		                         NULL, BUFFER_WRITE_NULL, NULL, \
		                         limit, desc); \
	}
# define stream_fd_copy(file, fd, limit, desc...)\
	buffer_copy_setup_PtrInt(file, BUFFER_READ_STREAM, NULL, \
	                         fd, BUFFER_WRITE_FD, NULL, \
	                         limit, desc)
#endif /* HAVE_C99 */

off_t buffer_copy_setup_PtrInt(void *p, int typeIn, void *procIn,
                               int i, int typeOut, void *procOut,
                               off_t limit, const char *desc,
                               ...) DPKG_ATTR_PRINTF(8);
off_t buffer_copy_setup_PtrPtr(void *p1, int typeIn, void *procIn,
                               void *p2, int typeOut, void *procOut,
                               off_t limit, const char *desc,
                               ...) DPKG_ATTR_PRINTF(8);
off_t buffer_copy_setup_IntPtr(int i, int typeIn, void *procIn,
                               void *p, int typeOut, void *procOut,
                               off_t limit, const char *desc,
                               ...) DPKG_ATTR_PRINTF(8);
off_t buffer_copy_setup_IntInt(int i1, int typeIn, void *procIn,
                               int i2, int typeOut, void *procOut,
                               off_t limit, const char *desc,
                               ...) DPKG_ATTR_PRINTF(8);
off_t buffer_copy_setup(buffer_arg argIn, int typeIn, void *procIn,
                        buffer_arg argOut, int typeOut, void *procOut,
                        off_t limit, const char *desc);
off_t buffer_write(buffer_data_t data, void *buf,
                   off_t length, const char *desc);
off_t buffer_read(buffer_data_t data, void *buf,
                  off_t length, const char *desc);
off_t buffer_copy(buffer_data_t read_data, buffer_data_t write_data,
                  off_t limit, const char *desc);

DPKG_END_DECLS

#endif /* DPKG_BUFFER_H */
