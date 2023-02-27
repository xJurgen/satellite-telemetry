# satellite-telemetry

This is a college project focused on the creation of a CubeSat from scratch.
The code implements a Blackpill (STM32F411CE) as a sensor driving unit.

The final product is intended to be used jointly with an RPi Zero W (v1.1) communicating using USART.

Implemented features:
	
	- Command processing
	- Communication on USART1

Planned features:

	- Light sensor processing using ADC
	- Battery management (input/output/battery current with implemented kill-switch when the battery is too low)
	- Gyroscope sensor
	- and more (yet to be defined)...
