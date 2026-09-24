#ifndef DEFS_H
#define DEFS_H

#ifdef TRUE
#undef TRUE
#endif

#define TRUE 1

#define KEY_CTRL(key) ((key) & 0x1f)
#define KEY_DEL 0x7f

#endif /* DEFS_H */
