# satellite-telemetry

This is a college project with an objective to create a CubeSat-like device from scratch.

The code is intended for a Blackpill (STM32F411CE) board and its functionality provides an access to the satellite sensors.
In the final design the Blackpill is connected to an RPi Zero W (V1.1) via USART1 and responds on the RPi requests.

Implemented features:
	
	- Command processing
	- Communication on USART1

Work in progress:
	
	- Light and temperature sensing using ADC

Planned features:

	- Battery management (solar panels input, battery output, shut-off when critically low level of battery..)
	- Gyroscope reading (?)




