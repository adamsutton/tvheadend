/* ***************************************************************************
 *
 * Copyright (C) 2008-2016 Tvheadend Foundation CIC
 *
 * This file is part of Tvheadend
 *
 * Tvheadend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tvheadend is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tvheadend.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For more details, including opportunities for alternative licensing,
 * please read the LICENSE file.
 *
 * **************************************************************************/

/*
 * VFS API
 *
 * The VFS system provides a simple means of accessing a variety of possible
 * file based resources, in some cases transparently handling various ways
 * of providing the resources.
 *
 * This is the public interface to be used by VFS users, there is a separate
 * interface for VFS modules.
 */

#ifndef __TVH_VFS_H__
#define __TVH_VFS_H__

/*
 * Typedefs
 */
typedef uint16_t            vfs_flags_t;
typedef struct   vfs_handle vfs_handle_t;
typedef void     vfs_read_cb
  ( vfs_handle_t *vh, const void *buf, const ssize_t len, void *aux );

/*
 * File open mode
 */
typedef enum {
  VFS_MODE_RD   = 1,
  VFS_MODE_WR   = 2,
  VFS_MODE_RDWR = 3,
} vfs_mode_t;

/*
 * File open flags
 */
#define VFS_OPEN_APPEND    (0x0001) ///< Open (writable) file in append mode
#define VFS_OPEN_NOCLOEXEC (0x0002) ///< Don't close on exec()

/*
 * File seek mode
 */
typedef enum {
  VFS_SEEK_SET, ///< Set position (absolute)
  VFS_SEEK_CUR, ///< Seek relative to current position
  VFS_SEEK_END, ///< Seek relative to the end of the file
} vfs_seek_t;

/**
 * Open a new file
 *
 * @param url URL (or path) for resource to open
 *
 * @return VFS handle if succesful or NULL if failed
 */
vfs_handle_t *
vfs_open ( const char *url, vfs_mode_t mode, vfs_flags_t flags );

/**
 * Close a handle
 *
 * This will free any associated resources
 *
 * @param vh The handle to free
 */
void
vfs_close ( vfs_handle_t *vh );

/**
 * Synchronous Read
 *
 * @param vh  The handle to read from
 * @param buf The buffer to read into
 * @param len The number of bytes to read
 *
 * @return The number of bytes read, -1 on error
 */
ssize_t
vfs_read ( vfs_handle_t *vh, void *buf, size_t len );

/**
 * Setup asynchronous read callback
 *
 * @param vh  The handle to read from
 * @param cb  The callback to call on receipt
 * @param aux The callback parameter
 */
int
vfs_read_async ( vfs_handle_t *vh, vfs_read_cb *cb, void *aux );

/**
 * Setup transfer from one vfs handle to another
 *
 * Note: this uses the same callback as the async read, except the buf param
 *       will be NULL and the len param will contain the amount of data
 *       transfered
 *
 * @param vh  The handle to read from
 * @param out The handle to write to
 * @param cb  Callback to be called on completion
 * @param aux Parameter for the callback
 */
int
vfs_read_txfr
  ( vfs_handle_t *vh, vfs_handle_t *out, vfs_read_cb *cb, void *aux );

/**
 * Write
 *
 * @param vh  The handle to write to
 * @param buf The buffer to write from
 * @param len The number of bytes to write
 *
 * @return The number of bytes written, -1 on error
 */
ssize_t
vfs_write ( vfs_handle_t *vh, void *buf, size_t len );

/**
 * Seek
 *
 * @param vh   The handle to seek
 * @param off  The amount to seek (meaning depends on mode)
 * @param mode The seeking mode @see vfs_seek_t
 *
 * @return The position within the handle if succesful, else -1
 */
off_t
vfs_seek  ( vfs_handle_t *vh, off_t off, vfs_seek_t mode );

#endif /* __TVH_VFS_H__ */

/******************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 *****************************************************************************/
