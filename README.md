# CH554 USB Composite Device Demo (USB复合设备演示程序)

This demo emulates a USB composite device, which has 3 interface available (本演示程序模拟了一个具有3个接口的USB设备):
* USB Mass Storage Device (USB MSD，USB大容量储存设备，就是U盘辣)
* USB Keyboard (USB键盘)
* USB Vendor-Define HID (厂商自定义的USB HID设备)

Author(作者): Rikka0w0 (小六花)

* `keilc51\CH554.H`, `Delay.C` and `Delay.h` comes from official CH554 demo (WCH), the USB MSD implementation is inspired by the USB MSD demo for STM32 (ST). 
(`keilc51\CH554.H`, `Delay.C`和`Delay.h`是从WCH官方的示例代码里提取的，U盘部分程序参考了STM32的USB MSD示例)
* All file in folder `includes` comes from Github repository [Blinkinlabs's ch554_sdcc](https://github.com/Blinkinlabs/ch554_sdcc).
(`includes`文件夹中的文件是从[Blinkinlabs's ch554_sdcc](https://github.com/Blinkinlabs/ch554_sdcc)里复制的)
* Compiler/IDE: Keil uVersion & SDCC. (编译器/开发环境：Keil 礦ision和SDCC)
* A Wiki page describes how to setup SDCC on Windows and Linux. (Wiki中会介绍如何在Windows和Linux上搭建SDCC编译环境)
* Feel free to use this demo as the template/start of a open-source or non-commercial project, modifying source code and republishing is allowed.
(对于开源项目和非商业项目，可以是用本演示代码作为起点或者模板，作者允许修改并重新发布这份代码)
* However you __MUST__ contact the author for permission prior to use this demo for commercial purposes.
(__如果用于商业目的的话，必须提前得到作者的允许__)

__BadApple.zip contains a funny demo which can play the BadApple video on a SDD1306 128x64 OLED screen, the OLED screen is controlled via I2C, 
check out `I2c.c` for details about hardware configuration. (BadApple.zip包含一个有趣的演示，可以在SDD1306 128x64 OLED屏幕上播放BadApple视频，OLED屏幕通过I2C控制，在`I2c.c`中包含如何连接屏幕的信息)__


# Hardware Setup(硬件配置):
* The hardware is based on the minimal circuit setup of CH554
(硬件基于CH554的最小系统)
* Chip is directly powered by Vbus, which is approximately 5V
(芯片直接由Vbus驱动，大约是5V)
* P1.5 = I2C SDA, P1.4 = I2C SCL, defined in `i2c.c`, each pin is pull-up by a 1k resistor to 5V.
(I2C引脚定义在`i2c.c`里面，每个脚都要一个1k的上拉电阻，上拉到5V)
* Pin 1 to pin 4 of 24LC512 are connected to ground, Pin 8 connect to 5V, Pin5 is I2C SDA and Pin6 is I2C SCL, Pin7 is floating.
(24LC512 EEPROM的1到4脚都接地，8脚连接5V，5脚对应I2C SDA，6脚对应I2C SCL，七脚悬空) 
* Led1 is connected between 5V and P1.1 via a 1k resistor, it indicates R/W operations.
(Led21 通过1k电阻接在5V和P1.1之间，用来指示读写操作)
* Led2 is connected between 5V and P1.0 via a 1k resistor, it lights up when the MSD is ejected.
(Led2 通过1k电阻接在5V和P1.0之间，当大容量储存设备被弹出时点亮)

# Capabilities(能做到什么):
## USB Mass Storage Device (USB大容量储存设备)
* Emulate a 512Kib I2C EEPROM 24LC512 as a 64KB USB Drive
(用512Kib的I2C总线EEPROM 24LC512模拟了一个64KB的U盘)
* Read, write, delete and format operations are fully supported.
(完全支持各种读写删除还有格式化操作)
* Support safe media ejection in Windows, Linux and Mac OS X
(在Windows, Linux和Mac OS X上支持安全弹出介质)
* Use USB Endpoint 3 IN and OUT, Interface 2, Bulk-only transfer
(使用了端点3的IN和OUT，2号接口，BOT传输)
## USB Keyboard (USB键盘)
* Simulate key press and release, and send them to the host.
(可以模拟键盘按键操作并发往主机)
* Use USB Endpoint 1 IN, Interface 1, Interrupt transfer
(使用了端点2的IN，1号接口， 中断传输)
## USB Vendor-Define HID (厂商自定义的USB HID设备)
* Exchange data (receive and send) with host, up to 64 bytes each time
(可以和主机交换数据，每次最多64字节)
* Use USB Endpoint 2 IN, Interface 0, Interrupt transfer
(使用了端点2的IN， 0号接口，中断传输)
## UART (串口)
* UART0, no function re-mapping (0号串口，没有io重映射)
* Exchange data (receive and send) with host (可以和主机交换数据)
* Support printf (支持printf)
## I2C
* CH55x does not have built-in I2C peripheral (CH55x系列木有内置I2C通信外设)
* Implement and emulate a I2C peripheral with GPIO (用GPIO来模拟I2C总线通信)

# File Structure(文件结构):
## main.c
* void main(void) {}
* USB Device Interrupt Entry(USB设备中断函数入口)
## Delay.c Delay.h
* Delay (延时函数)
## ch554_platform.h
* Define uint8_t, uint32_t e.t.c (定义uint8_t, uint32_t等等标准类型)
* Handle the difference between Keil C51 and SDCC
(处理Keil C51和SDCC编译器之间的差异)
* `U16B0` returns the lowest byte of a 16-bit integer, regardless of endian, similar defination for `U16B1`, `U32B0` e.t.c
(`U16B0`会返回一个16位整数的低字节，与端无关，`U16B1`, `U32B0`等也是类似功能)
* `U16_XBig` converts a big-endian 16-bit integer to fit the local endian, vise versa, similar defination for `U16_XLittle`, `U32_XBig` and `U32_XLittle`.
(`U16_XBig`用于将一个大端储存的16位数转换成当前的端方式，也可以用于反向转换，`U16_XLittle`, `U32_XBig`和`U32_XLittle`的作用类似)
* Instead of using `sbit led=P1^1`, use `SBIT(var, port, bin)` to declare a bit variable, for example, `SBIT(led, GPIO1, 1)`, this facilitates compilation under different compilers.
(最好用`SBIT(var, port, bin)`来声明位变量，比如`SBIT(led, GPIO1, 1)`来代替`sbit led=P1^1`，这样有助于在不同编译器下编译程序)
## ch554_conf.c ch554_conf.h
* Configure system clock (配置系统时钟)
* Initialize USB Device (初始化USB设备)
* Initialize UART0 (初始化串口0)
* Set system clock frequency and UART0 baud rate in ch554_conf.h
(在ch554_conf.h里设置系统时钟频率和UART0的波特率)
## i2c.c i2c.h
* Emulate a I2C bus host with GPIO (用GPIO来模拟一个I2C总线主机)
* `I2C_DELAY(void)` controls the I2C clock frequency
* `I2C_WriteByte()` sends the data byte in `I2C_Buf` to I2C bus and return the ACK status in bit 0 of `I2C_Buf` then release the I2C bus,
0 means ACK and 1 means NACK. 
(`I2C_WriteByte()`会把`I2C_Buf`里的一个字节发送到I2C总线上并在`I2C_Buf`的最低位返回ACK的状态然后释放I2C总线，0为ACK，1为NACK) 
* `I2C_ReadByte(void)` reads a byte from I2C bus and store it in `I2C_Buf`. 
(`I2C_ReadByte(void)`从I2C总线上读一个字节然后放在`I2C_Buf`里面)
## eeprom.c eeprom.h
* Control read and write of EEPROM (控制EEPROM的读写)
* Transfer length up to 128 bytes (读写每次最多128字节)
## usb_it.c
* USB interrupt service routine (USB中断处理函数)
* Handle USB bus reset/suspend/wakeup (处理USB总线重置/挂起/唤醒)
* Dispatch incoming requests to EndPoint service functions (把主机发来的请求分发给端点的处理函数)
## usb_endp.h
* Declear EndPoint buffers (声明端点的缓存)
* Toggle certain EndPoint service function (开启或者关闭相关的端点处理函数)
## usb_endp.c
* Define EndPoint buffers (定义端点的缓存以及大小)
* `USB_EP_HALT_SET(ep)` and `USB_EP_HALT_CLEAR(ep)` stall or restore an EndPoint as the host requests 
(`USB_EP_HALT_SET(ep)`和`USB_EP_HALT_CLEAR(ep)`根据主机请求挂起stall或者恢复端点)
* Optionally, add your EndPoint service functions here (如果愿意可以把USB的端点处理函数放在这里)
## usb_ep0.c
* Contain service routine for EndPoint 0 SETUP/IN/OUT
(包含端点0的SETUP/IN/OUT事务的处理函数)
* Handle requests sent by host during enumeration phase
(在枚举的时候负责回应主机的请求)
## usb_desc.h
* Define the total length of Config Descriptor, including Interface Descriptor e.t.c (定义配置描述符的总大小，包含接口描述符等)
* Define the total number of String Descriptor (定义字符描述符的数量)
* Define length of HID Report Descriptors (定义各种HID报告描述符的大小)
## usb_desc.c
* Define Device Descriptor (定义设备描述符)
* Define Configuration Descriptor and Interface Descriptor (定义配置和接口描述符)
## usb_hid_desc.c
* Define HID Report Descriptors (定义各种HID报告描述符)
## usb_string_desc.c
* Define custom String Descriptors (定义自定义的字符描述符)
* String Descriptor 0 is reserved for language configuration (0号字符描述符是为语言标识保留的)
## usb_hid_keyboard.h
* `USB_Keyboard_SendKey()` sends a key combination to the host, the first parameter specifies the modifier, e.g. KBD_LCTRL for left control, combination is supported, 
the second parameter determines which key is pressed, e.g. 0x04 for letter a on keyboard. Check out [HID protocol](https://docs.mbed.com/docs/ble-hid/en/latest/api/md_doc_HID.html) for details.
(`USB_Keyboard_SendKey()`向主机发送一个按键组合，第一个参数定义了修饰符，比如KBD_LCTRL是左侧的CTRL键，这里支持组合。第二个参数决定了是哪个按键，比如0x04对应着键盘上的A。戳这里来补充[HID协议](https://docs.mbed.com/docs/ble-hid/en/latest/api/md_doc_HID.html)姿势)
## usb_bot.h
* Forward EndPoint IN/OUT buffer, abstract BOT layer behavior, see section `// BOT_USB Config`
(转发端点的缓存的定义，抽象BOT层，参见`// BOT_USB Config`后面那部分)
* It's possible to migrate the MSD function to other devices
(可以将大容量储存功能移植到其他设备中)
## usb_bot.c
* Mass storage device IN/OUT service routine (大容量储存设备的IN/OUT请求处理函数)
* Implements a BOT state machine (实现了一个BOT状态机)
* `CBW_Decode` translates USB packets to SCSI instructions (`CBW_Decode`将USB包翻译为SCSI指令)
* `Transfer_Data_Request` sends some data to host as reply (`Transfer_Data_Request`向主机发送一些数据作为回复)
* `Reply_Request` sends the content stored in `BOT_Tx_Buf` to host as reply (`Reply_Request`把`BOT_Tx_Buf`里的数据向主机发送作为回复)
* `Transfer_Failed_ReadWrite` sends a empty packet to host and the device will send its CSW in the next transaction
(`Transfer_Failed_ReadWrite`向主机发送一个空包并在下一次向主机发送CSW状态)
* `Set_CSW` sets the CSW status, by set `Send_Permission` to 1, it will send CSW immediately.
(`Set_CSW`用来设置CSW状态，把`Send_Permission`设为1会立刻发送CSW)
## usb_scsi.c
* Process SCSI inquiry and instructions
(处理SCSI的请求和指令)
* Read and Write Operations are controlled by a state machine, see section `// SCSI R/W state machine`
(读写是由一个状态机控制的，参见`// SCSI R/W state machine`)
## usb_scsi.h
* Define SCSI instruction codes, sense keys, e.t.c.
(定义SCSI指令，返回值等)
## usb_mal.h
* Define number of LUNs (Logic unit)
(定义了逻辑单元的数量)
* Declare functions that are necessary for MSD to work
(声明了一些MSD正常工作所需要的函数)
## usb_mal.c
* Implement all functions declared in `usb_mal.h`
(实现所有在usb_mal.h里定义的函数)
* Define `Inquiry Data`
(定义`Inquiry Data`，包括厂商名产品名之类的)
## Files in folder `OLDE` (OLED文件夹中的文件)
* `sdd1306.h` and `sdd1306.c` provides functions to initialize SDD1306-based OLED screen and chunk programming the display pattern.
(`sdd1306.h`和`sdd1306.c`提供了基于SDD1306的OLED屏幕的初始化和块编程显示模式的功能)
* `sdd1306_ascii.h` and `sdd1306_ascii.c` provides functions to display ASCII characters on OLED screen.
(`ssd1306 ascii.h`和`ssd1306 ascii.c`提供在OLED屏幕上显示ASCII字符的功能)

# Notes:
* The only difference between CH554 and CH552 is that, CH552 only supports USB device mode while CH554 can also be programmed as a Host. 
So this demo may work on CH552 as well. (CH552只能作为USB的设备，而CH554还可以作为USB主机，这是两种芯片唯一的区别。因此本Demo可能也能在CH552上正常工作)
* In this demo, __the I2C EEPROM is compolsory__, without it, the enumeration will not succeed. If you don't have the EEPROM, to get rid of it,
comment out the EEPROM read and write operation in `LUN_Write` and `Lun_Read` in `usb_mal.c`.
(这里的I2C的EEPROM是必须的，没有它设备会不能正常枚举，解决方法是在`usb_mal.c`注释掉`LUN_Write` 和 `Lun_Read`里面的读写操作)
* 8051 systems including CH554 is big-endian while x86 and ARM processors are little-endian.
(8051系统包括CH554是大端存储的，即数据的高位放在低内存地址中，而x86和ARM处理器都是小端存储的)
* USB packets are little-endian however the attached SCSI instructions are big-endian, keep this in mind!
(USB包都是小端的然而包含的SCSI指令是大端的，特别需要注意！) 
* SDCC requires that all interrupt service functions must be written inside the same .c file containing `void main(void)`, 
otherwise SDCC can not find them. According to official SDCC documentation, it's not a bug but a feature. Keil C51 doesn't have this limitaion.
(SDCC要求中断处理函数必须和main函数放在一个.c文件中，否则SDCC没办法找到这些函数。根据SDCC官方文档，这是一个特性不算Bug，Keil C51编译器没有这种限制)