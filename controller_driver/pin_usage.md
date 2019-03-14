# Controller periphery usage

## Encoder unit for clutch pedal actuator 
Driver | Pins | Input / Output | Description
EXT1   | PF7  | Input (chA)
EXT1   | PF8  | Input (chB)
GPT3   | Timer| -              | For speed calculation


## Encoder unit for brake pedal actuator 
Driver | Pins | Input / Output
-------|------|-------

## Encoder unit for steer actuator
Driver | Pins | Input / Output
-------|------|-------

## Encoder unit for transmition actuator 1
Driver | Pins | Input / Output
-------|------|-------

## Encoder unit for transmition actuator 2 
Driver | Pins | Input / Output
-------|------|-------


## Clutch pedal actuator control unit 
Driver | Pins | Input / Output | Description
PWM1   | PE9  | Output         | PWM use TIM1
EXT1   | PE6  | Input          | Enable button
EXT1   | PE5  | Input          | Direction button
EXT1   | PE4  | Input          | Start button
PAL    | PF14 | Output         | Enable or disable motor (for driver)
PAL    | PE11 | Output         | Control motor rotation direction (for driver)


## Brake pedal actuator control unit 
Driver | Pins | Input / Output
-------|------|-------

## Steer actuator control unit 
Driver | Pins | Input / Output
-------|------|-------

## Transmition actuator 1 control unit 
Driver | Pins | Input / Output
-------|------|-------

## Transmition actuator 2 control unit 
Driver | Pins | Input / Output
-------|------|-------

## Analoge sensors unit
Driver | Pins | Input / Output | Description
ADC1   | PC0  | Input          | -
ADC1   | PA3  | Input          | -
GPT4   | Trg  |  -             | -
