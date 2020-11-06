LPC15xx ADC example

Example description:
--------------------
The LPC15xx ADC example shows how to use the ADC to perform
a sequence of conversions and monitor a threshold crossing.
ADC0 is configured to monitor the internal temperature sensor on
ADC0 channel 0. It is setup to be triggered periodically by the
sysTick interrupt.
ADC1 is configured to monitor an analog input signal on ADC1. The
ADC channel used may vary per board. It is setup to be triggered
periodically by the sysTick interrupt with optional threshold
support.

Special connection requirements:
--------------------------------
To use this example, ADC1 channel 1 needs to be connected to an
analog source.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

