# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/andy/Documents/ece_520/fa22-ece520-final-project-alopez099/software/color_sensor_sw_system/_ide/scripts/debugger_color_sensor_sw-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/andy/Documents/ece_520/fa22-ece520-final-project-alopez099/software/color_sensor_sw_system/_ide/scripts/debugger_color_sensor_sw-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zybo Z7 210351AF248EA" && level==0 && jtag_device_ctx=="jsn-Zybo Z7-210351AF248EA-13722093-0"}
fpga -file /home/andy/Documents/ece_520/fa22-ece520-final-project-alopez099/software/color_sensor_sw/_ide/bitstream/design_1_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/andy/Documents/ece_520/fa22-ece520-final-project-alopez099/software/design_1_wrapper/export/design_1_wrapper/hw/design_1_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source /home/andy/Documents/ece_520/fa22-ece520-final-project-alopez099/software/color_sensor_sw/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow /home/andy/Documents/ece_520/fa22-ece520-final-project-alopez099/software/color_sensor_sw/Debug/color_sensor_sw.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
