# quetzalcoatl_project

## Подготовка

Для работы в среде ROS требуется установить в workspace [репозиторий с общими тулами для ROS](https://github.com/lsd-maddrive/maddrive_ros_shared).

## [Доки по Газели](https://disk.yandex.ru/d/_3zzZoxEZ3Ugxg)

## Notes
Экспорт для окружения для работы симулятора:
~~~bash
export GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:$(rospack find quetzalcoatl_maps)/models
~~~