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
 * VFS Module API
 *
 */

#ifndef __TVH_VFS_MODULE_H__
#define __TVH_VFS_MODULE_H__

/*
 * VFS operations
 */
typedef struct
{
  /** Read from file */
  ssize_t vo_read  ( vfs_handle_t *vh, void *buf, size_t len );

  /** Write to file */
  ssize_t vo_write ( vfs_handle_t *vh, const void *buf, size_t len );

  /** Close file */
  void    vo_close ( vfs_handle_t *vh );

  /** Seek file */
  off_t   vo_seek  ( vfs_handle_t *vh, off_t off, vfs_seek_t mode );
} vfs_ops_t;

/**
 * VFS module
 */
typedef struct vfs_module;
{
  SLIST_HEAD(vfs_module) vm_link;
  
  // Note: not worth RB use here, not likely to have enough modules
  //       to warrant the overhead
 
  const char           **vm_scheme; ///< List of URL schemes (NULL terminated)

  const vfs_ops_t       *vm_ops;    ///< File operations

  /** Open file */
  vfs_handle_t (       (*vm_open) ( const url_t *url );

} vfs_module_t;

/*
 * Register a new module
 *
 * TODO: is there a way to make this more automatic?
 */
int
vfs_module_register ( vfs_module_t *vm );

#endif /* __TVH_VFS_MODULE_H__ */

/******************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 *****************************************************************************/
