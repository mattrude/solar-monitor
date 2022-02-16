# Solar Monitor

## Summary

A simple solution to prevent a solar powered LoRaWAN gateway from going into brownout when the batteries drop in voltages.

## Hardware (BOM)


| **Identifier** | **Qty** | **Part #**      | **Description**    | **DigiKey**                                                                                                                      |
|:--------------:|:-------:|-----------------|--------------------|----------------------------------------------------------------------------------------------------------------------------------|
|       U1       | 1       | PIC16F15214-I/P | Microcontroller    | [150-PIC16F15214-I/P-ND](https://www.digikey.com/en/products/detail/microchip-technology/PIC16F15214-I-P/12807471)               |
|       Q1       | 1       | PN2222A         | Transistor         | [PN2222AFS-ND](https://www.digikey.com/en/products/detail/onsemi/PN2222ABU/6534)                                                 |
|       Q2       | 1       | MCP102-300E/TO  | Voltage Supervisor | [MCP102-300E/TO-ND](https://www.digikey.com/en/products/detail/microchip-technology/MCP102-300E-TO/716307)                       |
|       C1       | 1       | 10pF            | Ceramic Capacitor  | [K100K15C0GF53L2-ND](https://www.digikey.com/en/products/detail/vishay-beyschlag-draloric-bc-components/K100K15C0GF53L2/2820158) |
|       D1       | 1       | 1N914           | Diode              | [1N914FS-ND](https://www.digikey.com/en/products/detail/onsemi/1N914/978749)                                                     |
|       RV1      | 1       | PV36W103C01B00  | 10K Ohm Trimmer    | [490-2875-ND](https://www.digikey.com/en/products/detail/bourns-inc/PV36W103C01B00/666502)                                       |


## Schematic

**Schematic**
![Schematic Rev-A](Schematic_RevA.PNG)

## Board

**The Top of the Board**
![Board Top Rev-A](Board_Top_RevA.PNG)

**The Bottom of the Board**
![Board Bottom Rev-A](Board_Bottom_RevA.PNG)
