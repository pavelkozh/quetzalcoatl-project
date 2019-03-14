# Использование модулей периферии контроллера

## Модуль энкодера привода сцепления 
Driver | Pins | Input / Output | Description
-------|------|----------------|------------
EXT1   | PF7  | Input (chA)
-------|------|----------------|------------
EXT1   | PF8  | Input (chB)
-------|------|----------------|------------
GPT3   | Timer| -              | For speed calculation


## Модуль энкодера привода тормоза 
Driver | Pins | Input / Output
-------|------|-------

## Модуль энкодера привода руля 
Driver | Pins | Input / Output
-------|------|-------

## Модуль энкодера привода кпп1 
Driver | Pins | Input / Output
-------|------|-------

## Модуль энкодера привода кпп2 
Driver | Pins | Input / Output
-------|------|-------


## Модуль управления приводом сцепления 
Driver | Pins | Input / Output | Description
-------|------|----------------|-------
PWM1   | PE9  | Output         | PWM use TIM1
-------|------|----------------|-------
EXT1   | PE6  | Input          | Enable button
-------|------|----------------|-------
EXT1   | PE5  | Input          | Direction button
-------|------|----------------|-------
EXT1   | PE4  | Input          | Start button
-------|------|----------------|-------
PAL    | PF14 | Output         | Enable or disable motor (for driver)
-------|------|----------------|-------
PAL    | PE11 | Output         | Control motor rotation direction (for driver)


## Модуль управления приводом тормоза 
Driver | Pins | Input / Output
-------|------|-------

## Модуль управления приводом руля 
Driver | Pins | Input / Output
-------|------|-------

## Модуль управления приводом кпп1
Driver | Pins | Input / Output
-------|------|-------

## Модуль управления приводом кпп2 
Driver | Pins | Input / Output
-------|------|-------

## Модуль аналоговых датчиков
Driver | Pins | Input / Output
-------|------|-------
ADC1   | PC0  | Input
       | PA3  |
-------|------|-------
GPT4   | Trg  |  -
