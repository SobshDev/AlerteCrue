## Flood alert

The aim of this project is to make bridges safer in the event of flooding.
It measures the distance between the bridge and the water using an ultrasonic sensor, then sends the information to a display and by SMS to a raspberry hosting a website, which updates automatically.


### Requirements

- Atmega324 as the main microcontroller
- 2 GSM Modules that uses AT Commands
- Velman MML16CN to display the curent status of the pont
- A LCD Screen to display the distance and the curent alert level
- A HCSR-04 ultrasonic sensor to measure the distance between the bridge and the water

### Connections

#### Raspberry

- The GSM Module must be plugged into the USB0 port of the Raspberry, then run the script

#### Microcontroller
- The GSM Module needs to be plugged into the UART0
- The Velman needs to be plugged into the UART1
- The LCD Screen needs to be plugged into the PORTA
