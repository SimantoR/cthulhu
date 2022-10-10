#include <chrono>
#include <aakara/Global.hpp>

using namespace Global;

std::chrono::high_resolution_clock::time_point last_timestamp = std::chrono::high_resolution_clock::now();

f32 Time::DeltaTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - last_timestamp ).count();
}

f32 Time::DeltaTimeS()
{
    return Time::DeltaTime() / 1000.0f;
}

void Time::Reset()
{
    last_timestamp = std::chrono::high_resolution_clock::now();
}
