# Le Thermomètre L476

 In branch `Temperature_Humidity_only`  
 
 Simple thermometer for STM32L476 Discovery kit (STM32L476G-DISCO).  
 Displays temperature and humidity provided by external SHT45 captor (Adafruit 5665) connected on I2C_1 (CN2).  
 
 Optional automatic standby mode after 30 s inactivity.  
 
 **Joystick** :  
 UP and DOWN -> scroll through different displays : temperature, humidity  
 RIGHT -> enable auto stdby   
 LEFT -> disable auto stdby (default)  
 
 Press black button (reset) or joystick_center to wake up board.   
 
 
 STM32CubeIDE v.2.22 configurable project with MX .ioc file.  
 
 Inspired by ST BSP project :  
 https://github.com/STMicroelectronics/STM32CubeL4/tree/master/Projects/32L476GDISCOVERY/Examples/BSP  
 
