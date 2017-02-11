# Arduino / ATMega stepper motor control in C++
Rudamentary stepper motor control with linear start/stop speed ramps.

Small demo: https://youtu.be/yUwn1LcOEcw

Control one or more stepper motor drivers that have an 'enable, direction, step' interface.
Linear start/stop speed ramps for smoothisch movement.

![test setup with NEMA24 motor and M542T microstep driver](http://i.imgur.com/yTeQb9S.jpg)

## Building/Installing

### Dependencies

```
apt-get install avc-libc avrdude
```

### Building and flashing

```
make
make install  # Assumes your bord is attached to /dev/ttyUSB0
```

## Classes

### GPIO
```c++
gpio led(gpio::portb, gpio::pin5, gpio::out);
led.set();
```

### Motor
```c++
gpio ena(gpio::portd, gpio::pin2, gpio::out);
gpio dir(gpio::portd, gpio::pin4, gpio::out);
gpio pul(gpio::portd, gpio::pin6, gpio::out);

Motor motor(pul, dir, ena);
motor.enable();
motor.step();
motor.ccw();
motor.cw();
motor.disable();
```

### Control
```c++
Control control(motor, 250, 8000, 4); // motor, start steps/sec, max steps/sec, ramp angle)
control.move(4000); // Rotate shaft CW 4000 steps
control.move(-4000, 2000); // Rotate shaft CCW by 4000 steps @ max 2000 step/sec
```

## License
Do whatever you want with it... If you like it, just buy me a beer if ever happen to meet me.
