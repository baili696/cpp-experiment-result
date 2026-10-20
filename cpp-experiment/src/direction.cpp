#include "executor/direction.h"

namespace executor
{

char turnLeft(char heading)
{
    switch (heading) {
        case kNorth:
            return kWest;
        case kWest:
            return kSouth;
        case kSouth:
            return kEast;
        case kEast:
            return kNorth;
        default:
            return heading;
    }
}

char turnRight(char heading)
{
    switch (heading) {
        case kNorth:
            return kEast;
        case kEast:
            return kSouth;
        case kSouth:
            return kWest;
        case kWest:
            return kNorth;
        default:
            return heading;
    }
}

Offset forwardOffset(char heading)
{
    switch (heading) {
        case kNorth:
            return Offset{0, 1};
        case kSouth:
            return Offset{0, -1};
        case kEast:
            return Offset{1, 0};
        case kWest:
            return Offset{-1, 0};
        default:
            return Offset{0, 0};
    }
}

}  // namespace executor
