# Controller periphery usage

## Encoder unit for clutch pedal actuator 
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
EXT1   | PF7  | Input          | channel A
EXT1   | PF8  | Input          | channel B
GPT3   | Timer| -              | -       

## Throttle position (DAC)
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
SPI    | PA5 |                | clock
SPI    | PA6 |                | MISO
SPI    | PA7 |                | MOSI
SPI    | PD14|               | CS


## CAN 
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
CAN1   | PD1  |                |CAN RX
CAN1   | PD0  |                |CAN TX


## Clutch pedal actuator control unit 
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
PWM3   | PC6  | Output         |   
PAL    | PB8 | Output         | Direction



## Brake pedal actuator control unit 
Driver | Pins | Input / Output  | Description
-------|------|-------
PWM4   | PD12  | Output         |   
PAL    | PD11 | Output          | Direction


## Transmition control unit 
Driver | Pins | Input / Output | Description
-------|------|-------         |
PWM11  | PF7  |                | vertical
PWM14  | PF9  |                | gorisontal
PAL    | PG1  |                | vertical Direction
PAL    | PE3  |                | gorisontal Direction


## Steer actuator control unit 
Driver | Pins | Input / Output
-------|------|-------
