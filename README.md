# dnx RTOS

The dnx RTOS  is  a  general  purpose operating  system  based  on  the FreeRTOS
kernel. The dnx layer is modeled on  well-known Unix architecture. A destination
of  the  system  are  small  microcontrollers supported by the FreeRTOS kernel,
especially  32-bit. The system  is easy scalable to the user's needs,  the user
can write own drivers,  virtual devices, programs and so on. The program layer
is mostly compatible with the C standard. You can say that the dnx RTOS is a
FreeRTOS kernel distribution.

## The minimal hardware requirements
There are minimal hardware requirements that allows to start system but with
very minimalistic software base:
1. 32 KiB Flash memory,
2. 10 KiB RAM memory.
The hardware requirements depends on the user's configuration.

## The system features
- user's terminal (via UART interface in example configuration),
- possibility to run many copies of the same program,
- simple porting of PC programs for the dnx (due to the C standard compatibility)
- simple Drivers Layer,
- support many file systems (the VFS Layer),
- interrupts are not masked by the system (the RTOS feature),
- dynamic memory allocation (simple garbage collector for programs),
- supports many CPU architectures (the FreeRTOS feature),
- modular design.

## The project configuration
To configure the project, type in the terminal:
```
make config
```
or
```
configure
```

To compile the project, type in the terminal:
```
make
```

## Folder structure
- BSP    - board support packages (configurations)
- build  - the project's binaries (created after compilation)
- config - the project's configuration
- doc    - documentation
- src    - the project's sources
- tools  - the project's tools (scripts, wizard, etc)

## Website
The whole documenation and exmaples you can found at
[dnx RTOS official website](https://www.dnx-rtos.org).

There is also additional software repository with
[applications](https://github.com/devdnl/dnx-rtos-apps).
