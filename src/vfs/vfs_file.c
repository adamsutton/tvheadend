/* ****************************************************************************
 *
 * Copyright (C) 2016- Tvheadend Foundation CIC
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
 * ***************************************************************************/

/*
 * VFS file:// scheme handler
 */

/*
 * Internal functions
 */
static vfs_handle_t *vfile_open
  ( vfs_module_t *, const url_t *, vfs_mode_t, vfs_flags_t, mode_t );
static int           vfile_poll1 ( vfs_module_t *vm, vfs_handle_t *vh );
static void       vfile_close ( vfs_handle_t * );
static ssize_t    vfile_read  ( vfs_handle_t *, void *, size_t );
static ssize_t    vfile_write ( vfs_handle_t *, const void *, size_t );
static off_t      vfile_seek  ( vfs_handle_t *, off_t, vfs_seek_t );
static vfs_poll_t vfile_poll2 ( vfs_handle_t * );

/*
 * File operations
 */
static vfs_ops_t vfile_ops = {
  .vh_close = vfile_close,
  .vh_read  = vfile_read,
  .vh_write = vfile_write,
  .vh_seek  = vfile_seek,
  .vh_poll  = vfile_poll2,
};

/*
 * VFS module definition
 */
static vfs_module_t vfile_mod = {
  .vm_scheme = {
    "",         // No scheme (std file path)
    "file://",
    NULL
  },
  .vm_open   = vfile_open,
  .vm_poll   = vfile_poll,
};

/**
 * Open the file
 */
vfs_handle_t *
vfile_open
  ( vfs_module_t *vm,
    const url_t *url, vfs_mode_t mode, vfs_flags_t flags, mode_t mode )
{
  vfile_handle_t *vh;
  int             f = 0;

  /* Mode */
  switch ( mode ) {
    case VFS_RDWR:
      f |= O_RDWR;
      break;
    case VFS_WR:
      f |= O_WRONLY;
      break;
    default:
      f |= O_RDONLY;
      break;
  }

  /* Flags */
  
  /* Open the file */
  int fd = open(url->path, f, mode);
  if (fd < 0)
    return NULL;

  /* Create handle */
  vh = calloc(1, sizeof(vfile_handle_t));
  vh->vh_fd   = fd;
  vh->vh_name = strdup(url->path);
  vh->vh_ops  = &vfile_ops;
  // TODO: register?

  return (vfs_handle_t*)vh;
}

/**
 * Return poll descriptor
 */
int
vfile_poll1 ( vfs_module_t *vm, vfs_handle_t *vh )
{
  return ((vfile_handle_t*)vh)->vh_fd;
}

/**
 * Close the file
 */
void
vfile_close ( vfs_handle_t *vh )
{
  close(((vfile_handle_t*)vh)->vh_fd);
}

/**
 * Read from the file
 */
ssize_t
vfile_read  ( vfs_handle_t *vh, void *buf, size_t len )
{
  return read(((vfile_handle_t*)vh)->vh_fd, buf, len);
}

/**
 * Write to the file
 */
ssize_t
vfile_write ( vfs_handle_t *vh, const void *buf, size_t len )
{
  return write(((vfile_handle_t*)vh)->vh_fd, buf, len);
}

/**
 * Seek
 */
off_t
vfile_seek  ( vfs_handle_t *vh, off_t off, vfs_seek_t whence )
{
  int w = 0;
  switch ( whence ) {
    case VFS_SEEK_SET:
      w = SEEK_SET;
      break;
    case VFS_SEEK_CUR:
      w = SEEK_CUR;
      break;
    case VFS_SEEK_END:
      w = SEEK_END;
      break;
  }

  return lseek(((vfile_handle_t*)vh)->vh_fd, off, w);
}

/**
 * Initialise
 */
void
vfs_file_init ( void )
{
  vfs_module_register(&vfile_mod);
}

/******************************************************************************
 * Editor Configuration
 *
 * vim:sts=2:ts=2:sw=2:et
 *****************************************************************************/
