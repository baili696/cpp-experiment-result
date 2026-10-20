#include "executor/vehicle_profile.h"

namespace executor {

std::unique_ptr<VehicleProfile> makeVehicleProfile(VehicleType type)
{
    switch (type) {
        case VehicleType::kCar:
            return std::unique_ptr<VehicleProfile>(new CarProfile());
        case VehicleType::kSportsCar:
            return std::unique_ptr<VehicleProfile>(new SportsCarProfile());
        case VehicleType::kBus:
            return std::unique_ptr<VehicleProfile>(new BusProfile());
    }
    return std::unique_ptr<VehicleProfile>(new CarProfile());
}

}  // namespace executor
