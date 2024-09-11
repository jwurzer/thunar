
#include <stdio.h>


// https://opensource.apple.com/source/dyld/dyld-239.3/include/mach-o/dyld-interposing.h
/*
 *  Example:
 *
 *  static
 *  int
 *  my_open(const char* path, int flags, mode_t mode)
 *  {
 *    int value;
 *    // do stuff before open (including changing the arguments)
 *    value = open(path, flags, mode);
 *    // do stuff after open (including changing the return value(s))
 *    return value;
 *  }
 *  DYLD_INTERPOSE(my_open, open)
 */

#define DYLD_INTERPOSE(_replacement,_replacee) \
   __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
            __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };

void *g_desktop_app_info_new(const char *desktop_id);

__attribute__((visibility("default"))) void *g_desktop_app_info_new(const char *desktop_id)
{
        fprintf(stderr, "call dummy impl of g_desktop_app_info_new(const char *desktop_id) for %s", desktop_id);
        return 0;
}

//__attribute__((visibility("default"))) void *_g_desktop_app_info_new(const char *desktop_id)
static void *my_g_desktop_app_info_new(const char *desktop_id)
{
        fprintf(stderr, "call dummy impl of g_desktop_app_info_new(const char *desktop_id) for %s", desktop_id);
        return 0;
}

DYLD_INTERPOSE(my_g_desktop_app_info_new, g_desktop_app_info_new)
