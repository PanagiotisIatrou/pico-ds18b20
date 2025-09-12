# pico-ds18b20
Raspberry Pi Pico C++ library for the ds18b20 temperature sensor

## How to connect
**Note:** Does not support parasite power mode  

You first need to determine whether your ds18b20 has a built-in pull-up resistor or not. ds18b20's with built-in pull-up resistors usually have a small component connected across all three wires (ex. Keystudio ds18b20).

<table>
  <tbody>
    <tr>
      <td>
        <figure>
          <p align="center" width="300px">
            <img src="images/ds18b20_without.jpg" width="300px">
            <figcaption><p align="center">Without built-in pull-up resistor</p></figcaption>
          </p>
        </figure>
      </td>
      <td>
        <figure>
          <p align="center" width="300px">
            <img src="images/ds18b20_with.jpg" width="300px">
            <figcaption><p align="center">With built-in pull-up resistor</p></figcaption>
          </p>
        </figure>
      </td>
    </tr>
  </tbody>
</table>

In case your ds18b20 doesn't have a built-in pull-up resistor, you need to connect one $4.7k\Omega$ yourself.

Connect:
- VDD: to 5V (ex. VBUS pin) or 3.3V (ex. 3V3 pin)
- GND: to ground (any GND pin)
- DQ: to any GPIO pin

For example:

<table>
  <tbody>
    <tr>
      <td>
        <figure>
          <p align="center" width="300px">
            <img src="images/connection_without.png" width="300px">
            <figcaption><p align="center">Without built-in pull-up resistor</p></figcaption>
          </p>
        </figure>
      </td>
      <td>
        <figure>
          <p align="center" width="300px">
            <img src="images/connection_with.png" width="300px">
            <figcaption><p align="center">With built-in pull-up resistor</p></figcaption>
          </p>
        </figure>
      </td>
    </tr>
  </tbody>
</table>

## Features
- Measure temperature
  - &plusmn;0.5°C from -10°C to +85°C
  - &plusmn;1°C from -30°C to +100°C
  - &plusmn;2°C from -55°C to +125°C
- Set resolution (temperature discrete step size)
  - Low: 0.0625°C steps
  - Medium: 0.125°C steps
  - High: 0.25°C steps
  - Very High: 0.5°C steps
- Save resolution for retention after power cycle

## Resources

- [ds18b20 Analog datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf)
- [Book of iButton® Standards](https://www.analog.com/media/en/technical-documentation/tech-articles/book-of-ibuttonreg-standards.pdf)