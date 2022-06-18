# quetzalcoatl_project

## Подготовка

Для работы в среде ROS требуется установить в workspace [репозиторий с общими тулами для ROS](https://github.com/lsd-maddrive/maddrive_ros_shared).

## Some notes

Для работы над данным проектом были добавлены PWM драйверы.

Для установки патча скопируйте папку (не удаляйте предыдущую) `os` из папки `chibios_patch` к себе в папку `chibios*`.

## Установка для ChibiOS

- `export CHIBIOS_ROOT=$HOME/ChibiStudio/ChibiStudio17/chibios176/`
- `sudo apt install gcc-arm-none-eabi`
- `rsync -avp chibios_patch/os/ $CHIBIOS_ROOT/os/`

## Сборка

