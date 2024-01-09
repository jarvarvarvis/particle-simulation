#ifndef VERSION_H
#define VERSION_H

typedef struct {
    int major, minor;
} Version;

Version version_new(int major, int minor);

#endif /* VERSION_H */
