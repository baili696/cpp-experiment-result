#pragma once

#include <cstdint>

namespace executor
{

/// 朝向取值（需求约定：使用 char 表示 N/S/E/W 四个方向）
constexpr char kNorth = 'N';
constexpr char kSouth = 'S';
constexpr char kEast = 'E';
constexpr char kWest = 'W';

/// 坐标增量。
///
/// 坐标系约定（与需求一致）：
///   X 轴为东西(E/W)方向，Y 轴为南北(N/S)方向；
///   朝向 N 前进 -> y + 1，朝向 S 前进 -> y - 1，
///   朝向 E 前进 -> x + 1，朝向 W 前进 -> x - 1。
struct Offset {
    std::int32_t dx{0};
    std::int32_t dy{0};
};

/// 左转 90 度（逆时针）：N->W->S->E->N
char turnLeft(char heading);

/// 右转 90 度（顺时针）：N->E->S->W->N
char turnRight(char heading);

/// 在给定朝向下“向前”走一格的坐标增量
Offset forwardOffset(char heading);

}  // namespace executor
