# Le Thermomètre L476

 Simple thermometer for STM32L476 Discovery kit (STM32L476G-DISCO).  
 Displays temperature and humidity provided by external SHT45 captor (Adafruit 5665) connected on I2C_1 (CN2).  
 
 Automatic standby mode after 30 s inactivity.  
 
 Joystick :
 UP and DOWN -> scroll through different displays : temperature, humidity, ...  

 Press black button (reset) or joystick_center to wake up board.  
 
 In branch "Temperature_Humidity_only" a driver for SHT45 has been added.  
 
 The board can work with CR2032 battery only : remove jumpers on JP3, JP5 and JP6. Then connect JP5_pin2 (center) with JP6_pin1 (upper).  
  -> In that case, only MCU STM32L476 is powered and no other accessories (as there is **no voltage** on 3V and 3V3 nets) !  
  
 Displays are in French.  
 
 STM32CubeIDE v.2.22 configurable project with MX .ioc file.  
 
 Inspired by ST BSP project :  
 https://github.com/STMicroelectronics/STM32CubeL4/tree/master/Projects/32L476GDISCOVERY/Examples/BSP  
 
