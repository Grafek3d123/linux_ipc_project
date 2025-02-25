cd ~/Documents/hw-insitech/linux_ipc_project/build && rm -r * && cmake .. && make

ipcs -q 
ipcrm -q 393216 