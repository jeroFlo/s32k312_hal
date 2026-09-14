## Overall Structure

The implementation is divided into these main components:

- **GPIO.h/timer.h**: Contains public definitions, macros, data types, and function prototypes.

- **GPIO.c/timer.c**: Implements the GPIO/timer configuration and control functions.

- **helper.h**: Structures and definitiones useful across peripherals.

- **main.c**: Demonstrates the use of the HAL through application-level examples.

- **app.h**: Declarations used by the application in main.c


## Software Architecture and Layer Interaction

The project follows a layered architecture where the application interacts with the hardware exclusively through Hardware Abstraction Layers (HALs). This approach isolates register-level operations from the application logic, improving readability, maintainability, and portability.

```text
+----------------------------------------------------+
|                    Application (src)               |
|----------------------------------------------------|
| main.c                                             |
|                                                    |
| - GPIO_Handler_BoardSwitch()                       |
| - timer0_Handler()                                 |
| - Application-specific behavior                    |
+------------------------+---------------------------+
                         |
                         | API Calls
                         v
+----------------------------------------------------+
|           Hardware Abstraction Layer (hal)         |
|----------------------------------------------------|
| GPIO HAL                                           |
| - LED initialization                               |
| - Switch initialization                            |
| - Interrupt configuration                          |
| - Callback registration                            |
|                                                    |
| Timer HAL                                          |
| - Timer initialization                             |
| - Timer start/stop                                 |
| - Interrupt configuration                          |
| - Callback registration                            |
+------------------------+---------------------------+
                         |
                         | Register Access
                         v
+----------------------------------------------------+
|                 Hardware Peripherals                |
|----------------------------------------------------|
| SIUL2                                              |
| PIT (Periodic Interrupt Timer)                     |
| NVIC                                               |
| GPIO Pins                                          |
+----------------------------------------------------+
```
