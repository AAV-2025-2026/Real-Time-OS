#include <iostream>
#include "vehicle_state/vehicle_state.hpp"

int main() {
    std::cout << "Hello QNX!\n";

    vehicle_state::VehicleState state;

    /*
    In a loop
    1. Receive data from ROS
    2. Decode what data type it is
    3. Update vehicle state
    4. Update DB

    In a loop
    1. Listen for data requests
    2. Get data from vehicle state
    3. Return data
    */

    return 0;
}
