connect -url tcp:127.0.0.1:3121
source /home/jsk_027/PSOC_ED/EdgeDetection/project_1_vivado/project_1_vivado.sdk/design_1_wrapper_hw_platform_0/ps7_init.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zed 210248492856"} -index 0
loadhw /home/jsk_027/PSOC_ED/EdgeDetection/project_1_vivado/project_1_vivado.sdk/design_1_wrapper_hw_platform_0/system.hdf
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zed 210248492856"} -index 0
stop
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zed 210248492856"} -index 0
rst -processor
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zed 210248492856"} -index 0
dow /home/jsk_027/PSOC_ED/EdgeDetection/project_1_vivado/project_1_vivado.sdk/test_edge_detection/Debug/test_edge_detection.elf
bpadd -addr &main
