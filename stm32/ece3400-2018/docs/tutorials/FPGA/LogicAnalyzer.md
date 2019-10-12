# Logic Analyzer Tutorial
## ECE 3400, Fall 2018

Written by Team 5 (Avisha Kumar, Katarina Martucci, Emma Kaufman, and Liam Patterson)

### What is a logic analyzer?

A logic analyzer allows one to view directly the signals acquired directly from a circuit. In the context of this course, this means that instead of viewing individual I2C frames on an oscilloscope, one can see them decoded in real time, including ACK/NACK, data, and START/STOP frames. This allows for supremely simple I2C and other protocol debugging.

Most logic analyzers one might find in a lab/makerspace setting will be primarily of the consumer/portable type. This means only decent resolution and sample rates (on the order of tens to hundreds of MHz). Common vendors for these applications include Digilent ([Analog Discovery 2, $179 with academic discount](https://store.digilentinc.com/analog-discovery-2-100msps-usb-oscilloscope-logic-analyzer-and-variable-power-supply/)) and Saleae (Logic series, from $199 with academic pricing). Each are fairly similar, interfacing with a host computer (macOS/Windows/Linux) over USB.

Personally, I use the Analog Discovery 2 (AD2). In addition to protocol analysis, it can also act as an oscilloscope, voltmeter, voltage source, and even a function generator. Since I do only have this piece of hardware, I can only provide general guidance for similar products, and the tutorial will be tailored for the AD2.

### Usage

The AD2 can be used to sniff UART, SPI, I2C, and CAN out of the box with the default software (Waveforms, downloadable from [Digilent](https://store.digilentinc.com/waveforms-previously-waveforms-2015/)).

Logic analyzers will require being physically connected to the circuit to be inspected. Refer to the manual for your board to figure out which pins can be used for logic analysis. The AD2 allows you to make use of all 16 DIO pins for logic analysis and even view multiple connections simultaneously (if, for instance, you have multiple I2C busses, you’d be able to view them both with this device). Attach a header wire of your choice to each DIO pin on the AD2 you wish to use, connecting each to your protocol’s respective connections (for I2C this would simply be two wires, SDA & SCL). It’s important to also establish a common ground at this step. On that note, the AD2 will not provide internal pullup support, so ensure your I2C or other protocol’s lines are established correctly. Then, knowing the bus rate of your line, select the frequency at which to sample (normally, the defaults provided will be adequate).

Now, you’re ready to sample. Simply click “Receive” in Waveforms (or equivalent, if not using AD2) and watch I2C messages be displayed on the screen. I2C frames will be automatically decoded according the the clock, with a final result being displayed in hexadecimal on the host computer. If you see errant or flooding messages ensure a common ground has been established. If nothing appears, ensure that your device is actively transmitting I2C frames.

The AD2 can also aid in protocol troubleshooting beyond simply viewing frames. If you’re using SPI or I2C, the AD2 can act as a master device and send out the frames you command. Remember, while SPI has multimaster support, I2C does not, so ensure your topology makes logical sense before running.

The AD2 can additionally emulate a specific sensor. This is especially useful when attempting to debug a (possibly) failed component, like an I2C-connected camera, for example. In WaveForms you can write a specific program that the AD2 will follow when given a start command.

### Applicability
We found ourselves using the logic analyzer most with the OV7670 camera. Because vision processing was done onboard an FPGA (and not the arduino), we needed to ensure that the device was setup properly before even isolating our FPGA code. We found many errors in how we’d set the registers on the camera, an ultimately, through hours of debugging, figured out the correct registers to be set. The logic analyzer proved especially useful during this time, helping us to ensure our messages were being sent, and that no wiring issues plagued our circuit.


