# hs-slider-project
Combining 3d printed and metal parts to build an open source motorized dslr slider.

### More Details
You can find more inforamtion about important features, and how this project was designed in this projects blog post at [https://blog.hirnschall.net/diy-motorized-slider/](https://blog.hirnschall.net/diy-motorized-slider/)

### IMPORTANT NOTE: 
Known issues for pcb v1.0:
- Due to an issue with the current pcb design v1.0 (missing trace), it is required to solder the MS1_PIN to the MS2_PIN on the A4988 stepper driver, to get 8x and 16x micro-stepping working. These pins are labeled on the slider-controller pcb.
- The pinout of the 3.3V regulator ist not correct. The 5V IN and the 3.3V OUT pins have to be swaped. The correct pinout of the pcb is as shown here https://github.com/shirnschall/hs-slider-project/blob/master/schematics/board.pdf


Sorry for the inconvenience!
