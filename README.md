# Winbond-W25QXX-Universal-Flash-Driver

Hey Guys!!.......

This is a universal flash driver for Windbond W25QXX Series. Using this driver, you can interface majority of Winbond SPI Flash memory.
It was previously developed by a youtuber named nimaltd , but it contains a lot of bugs. I removed all of the bugs and generated a new version of flash driver.

You can easily write, read and erase the contents using this driver. In addition , this driver will also useful for synchronizing flash driver with a desired file system.

You can program blocks,sectors or pages in order to store your valuable data.......

Following are some general configuration you must do before interfacing.

1. Connect HOLD Pin to VCC
2. Enable SPI and set a CS pin as GPIO Output.
3. Connect MOSI Pin of flash memory with MOSI pin of a Microcontroller.
4. Connect MISO Pin of flash memory with MISO pin of a Microcontroller.
5. Connect SCK Pin of flash memory with SCK Pin of a Microcontroller.
6. Call w25qxx.h in your main file.
7. After then, call  w25qxx_Init() function for viewing flash memory parameters.

After completing this setup, don't forget to erase the flash memory before it's use. 
Also, it is mandatory for you to verify the program instructions, that was written by me in this code, with the instructions set of your flash memory.
