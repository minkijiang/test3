# MyMonitoringTool:

**Author:** Minki Jiang
**Date:** 4/07/2025
**Description:** This program gets system informations on linux computers including memory and cpu usage and cpu cores information and displays it in organized graphs

This readme contains 2 parts:

- **About section:** how to run program, assumptions and general notes
- **Implementation:** all information of implementation of the modules

# About:

##### **command to compile:** 

run these commands in order:

1. make -f Makefile all
2. ./myMonitoringTool [CLAs]

##### **Assumptions:**

- Terminal height has to be at least 175
- if there are only positional arguments and no flags then it will display all information.
- the terminal height has to be at least 175
- all strings will have length strictly less than MAXLENGTH = 1024

##### External libraries:

- string.h
- math.h
- time.h
- stdbool.h
- unistd.h

# Implementation:

### Modules:

**HelperModules:**

- draw.h
- setsignal.h
- signalhandler.h
- graph.h

**Modules:**

- myMonitoringTool
- monitorMemory
- monitorCpu
- monitorCores

**Extras:**

- lib.h
- constants.h
- childsignalhandler.h : implementation of signalhandler.h for child processes
- parentsignalhandler.h : implementation of signalhandler.h for parent process

##### System Information Gathering:

- **Memory information:**  retrived from parsing "/proc/meminfo" file
- **Cpu Utilization Information:** retrived from parsing "/proc/stat" file
- **Core Mmount Information:**  retrived from "/proc/cpuinfo" file
- **Max Cpu Frequency Information:**  retrived from "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq" file
##### 
