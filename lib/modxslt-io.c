/*
 *  mod-xslt -- Copyright (C) 2002-2008 
 *   		 Carlo Contavalli 
 *   		 <ccontavalli at inscatolati.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include "modxslt1/modxslt.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#ifdef HAVE_MMAP
# include <sys/mman.h>
#endif /* HAVE_MMAP */

typedef struct mxslt_mmap_ctx_t {
  char * mem;
  size_t size;
  int fd;
} mxslt_mmap_ctx_t;

#ifdef HAVE_MMAP
static int mxslt_destroy_input_mmap(void * ctx) {
  mxslt_mmap_ctx_t * mmapd=(mxslt_mmap_ctx_t *)ctx;
  int fd;
    
    /* Here, we don't check for error values since
     * we want to free up any used resource */
  munmap(mmapd->mem, mmapd->size);
  fd=mmapd->fd;
  xfree(mmapd); 

  return close(fd);
}
#endif /* HAVE_MMAP */

xmlParserInputBufferPtr mxslt_create_input_file(mxslt_doc_t * doc, char * tmpfile) {
  int fd;

  fd=open(tmpfile, O_RDONLY);
  if(fd < 0) {
    mxslt_error(doc, "couldn't open file: %s, errno: %d\n", tmpfile, errno);
    return NULL;
  }

  return mxslt_create_input_fd(doc, fd);
}

xmlParserInputBufferPtr mxslt_create_input_fd(mxslt_doc_t * doc, int fd) {
  xmlParserInputBufferPtr retval;
  mxslt_mmap_ctx_t * mmapd;
  struct stat buf;
  char * tmp;

  if(fstat(fd, &buf) < 0) {
    mxslt_error(doc, "couldn't stat fd: %d, errno: %d\n", fd, errno);
    close(fd);

    return NULL;
  }

    /* We do not want to deal with weird files... 
     *   no devices nor pipes are allowed! */
  if(!S_ISREG(buf.st_mode)) {
    mxslt_error(doc, "fd is not a regular file: %d\n", fd);
    close(fd);

    return NULL;
  }

  if(!buf.st_size) {
    mxslt_error(doc, "Zero sized files are not valid xml (fd: %d), out of disk space?\n", fd);
    close(fd);

    return NULL;
  }

#ifdef HAVE_MMAP
  tmp=mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    /* If mmap succedes, we should be done */
  if(tmp != MAP_FAILED) {
    retval=xmlParserInputBufferCreateMem(tmp, buf.st_size, XML_CHAR_ENCODING_NONE);
    if(retval) {
      mmapd=(mxslt_mmap_ctx_t *)(xmalloc(sizeof(mxslt_mmap_ctx_t)));
      mmapd->mem=tmp;
      mmapd->size=buf.st_size;
      mmapd->fd=fd;

      retval->closecallback=mxslt_destroy_input_mmap;
      retval->context=mmapd;
      return retval;
    }
    munmap(tmp, buf.st_size);
  }
#endif /* HAVE_MMAP */

  return xmlParserInputBufferCreateFd(fd, XML_CHAR_ENCODING_NONE);
}
