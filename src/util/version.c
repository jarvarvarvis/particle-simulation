#include "version.h"

Version version_new(int major, int minor) {
    Version version;
    version.major = major;
    version.minor = minor;
    return version;
}
