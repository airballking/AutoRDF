#ifndef AUTORDF_WORLD_H
#define AUTORDF_WORLD_H

#include <cstdio>
#include <mutex>

#include <librdf.h>

namespace autordf {
/**
 * This class holds a static singleton on librdf world object.
 * Singleton is created upon first time class instanciation, and
 * is deleted when last instance of class is destroyed
 */
class World {
public:
    World();
    World(const World&) = delete;

    ~World();

    librdf_world *get() const { return _world; }

private:
    static std::mutex _mutex;
    static librdf_world *_world;
    static int _refcount;

    static int logCB(void *user_data, librdf_log_message *message);
    static int warnCB(void *user_data, const char *message, va_list arguments);
    static int errorCB(void *user_data, const char *message, va_list arguments);
};
}

#endif //AUTORDF_WORLD_H
