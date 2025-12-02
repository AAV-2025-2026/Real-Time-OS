# Robot Safety Watchdog 
### Current Status & Issues
* **QNX Environment:** The code has **not** been verified or compiled in a live QNX environment. It is currently a logic prototype.
* **Blocking Logic:** The `estop_protocol.cpp` currently uses a `while(1)` loop, which will hard-lock the thread. A non-blocking state machine or interrupt-based approach is required for a real RTOS implementation.

### To-Do / Missing Components
To make this code compilable and functional, the following must be added/fixed:
1. **E-Stop Reset Logic:** Implement the recovery mechanism in `estop_protocol.cpp` to exit the halted state safely.
