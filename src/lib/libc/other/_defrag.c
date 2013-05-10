#include <lib.h>
#include <unistd.h>

PUBLIC int defrag(cFilePath)
const char *cFilePath;
{
  message m;
  int r;

  /* This function is used to load a string into a type m3 message. If the
   * string fits in the message, it is copied there.  If not, a pointer to
   * it is passed. => loadname.c (e.g. posix/_unlink.c)
   */
  _loadname(cFilePath, &m);

  r = _syscall(VFS_PROC_NR, DEFRAG, &m);
  if (r < 0)
	return -1; /* We want -1 if there is an error */
  return r;
}
