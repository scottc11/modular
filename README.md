# Modular Eurorack Microcontroller Projects

DIY projects using Arduino and other C++ based libraries



# Build Command

use `platformio` OR `pio` CLI for building, uploading, testing, and monitoring.

`cd` into project you wish to build

#### build project
```
pio run
```

#### upload project to 'target'
```
pio run --target upload
```

#### Clean build files
```
platformio run --target clean
```

#### run serial monitor
```
pio device monitor --port /dev/cu.usbserial-AI041SA5
```
