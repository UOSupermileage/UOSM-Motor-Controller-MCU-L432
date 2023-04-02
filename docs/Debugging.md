# Debugging

# Powering from 5V pin

Hwee Choo Liaw
4 years, 6 months ago.
I would like to connect a NUCLEO-L432KC to an external source through pin +5V. The idea is to disable ST-LINK to minimize the power consumption. ...

My solution: The SB9 must be removed as the T_NRST line will ground the STM32L432KC's NRST pin if the ST_LINK is not powered. Further, the pin +5V input can be supplied by a 3.7V Li-Po battery input instead of a 5V input as the 3.3V regulator will stabilize the 3.3V to the microcontroller.

https://os.mbed.com/questions/77388/Powering-a-NUCLEO-L432KC-with-an-externa/

# FreeRTOS Tips:

- Give each task a different priority level. FreeRTOS can get confused if tasks share a priority level.
- If not all tasks are executing, review your intervals and priorities. Do you have a task that is blocking execution of other tasks by firing too frequently?
