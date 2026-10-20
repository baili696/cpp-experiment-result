#pragma once

#include <vector>

namespace executor
{

/// 最小可执行动作：单格平移或 90 度转向。
///
/// 所有上层指令（M/L/R/F/B/TR）都会被展开成这里的原子动作后逐条执行，
/// 因此“不能跳跃，只能一格一格前进/后退”这一约束在结构上天然成立，
/// 同时“先前进 1 格再转向”这类“动作顺序”也能被精确表达。
enum class Action {
    kMoveForward,   ///< 沿当前朝向前进一格（朝向不变）
    kMoveBackward,  ///< 沿当前朝向后退一格（朝向不变）
    kTurnLeft,      ///< 原地左转 90 度
    kTurnRight,     ///< 原地右转 90 度
};

using ActionList = std::vector<Action>;

}  // namespace executor
