#include "schemadefs.hpp"
#include <string_view>

Column::Column(std::string_view name, DataType type) : name{ name }, type { type } {}