# L'Horlogeuse

 Simple clock, like an LCD watch, for STM32L476 Discovery kit (STM32L476G-DISCO).  
 Automatic standby mode when 30 s inactivity.  
 
 Navigation with joystick to display/set time and date.  
 UP and DOWN -> scroll through different displays : time, date, week day.  
 RIGHT -> set time, date or week day. Increase ou decrease values with UP and DOWN. The last RIGHT press save new settings.   
 LEFT -> escape from set modes without saving.  
 Press black button (reset) or joystick_center to wake up board.
 
 The board can work with CR2032 battery only : remove jumpers on JP3, JP5 and JP6. Then connect JP5_pin2 (center) with JP6_pin1 (upper).  
  -> In that case, only MCU STM32L476 is powered and no other accessories as there is no voltage on 3V and 3V3 nets !  
  
 Displays are in French.  
 
 STM32CubeIDE v.1.18 configurable project with MX .ioc file.  
 
 Inspired by ST BSP project :  
 https://github.com/STMicroelectronics/STM32CubeL4/tree/master/Projects/32L476GDISCOVERY/Examples/BSP  
 
