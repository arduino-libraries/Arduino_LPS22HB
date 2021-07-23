# ArduinoLPS22HB library

## Methods

### `begin()`

Initialize the pressure sensor.

#### Syntax 

```
BARO.begin()
```

#### Parameters

None.

#### Returns

1 on success, 0 on failure.

#### Example

```
if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
}
```

#### See also

* [end()](#end)
* [readPressure()](#readpressure)

### `end()`

De-initialize the pressure sensor.

#### Syntax 

```
BARO.end()
```

#### Parameters

None.

#### Returns

None.

#### Example

```
if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
}

// ...

BARO.end();
```

#### See also

* [begin()](#begin)
* [readPressure()](#readpressure)

### `readPressure()`

Read the sensor's measured pressure value.


#### Syntax 

```
BARO.readPressure()
BARO.readPressure(units)
```

#### Parameters

* _units_: `PSI`  to read the pressure in PSI (pounds per square inch), `MILLIBAR` to read the pressure in millibars and `KILOPASCALS` to read the pressure in kilopascals. If unit parameter is not provided, default is kilopascals   .

#### Returns

The pressure in PSI, millibar or kilopascal, depending on the units requested.

#### Examples

```
float pressure = BARO.readPressure();

Serial.print("Pressure = ");
Serial.print(pressure);
Serial.println(" kPa");
```

#### See also

* [begin()](#begin)
* [end()](#end)