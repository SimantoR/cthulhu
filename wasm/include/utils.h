#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <emscripten/val.h>
using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using i32 = int;
using i16 = short;
using i64 = long long;
using i8  = char;
using f32 = float;

using std::string;

template <typename T> using Array = std::vector<T>;

template <typename T> using Ptr  = std::shared_ptr<T>;
template <typename T> using UPtr = std::unique_ptr<T>;

using JSObject = emscripten::val;
