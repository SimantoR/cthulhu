#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <utils.h>

namespace Global::Time
{
    f32 DeltaTime();
    f32 DeltaTimeS();

    /**
     * @brief Reset timedelta for new frame
     *
     */
    void Reset();
};

#endif
