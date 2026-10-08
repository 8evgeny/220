#/bin/bash!

RED='\033[0;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

if [ -z "$1" ]; then
  echo "Запустите данный скрипт с аргументом:"
  echo -e "${RED}sudo ./ir_cam_setup.sh \${USER}${RESET}"
  echo -e
  exit
else
  echo "Первый аргумент: $1"
fi

file_sudoers="/etc/sudoers.d/ir_cam_setup"
rule_sudoers="$1 ALL=(root) NOPASSWD: /usr/local/sbin/ir_cam_setup"  
file="/usr/local/sbin/ir_cam_setup"
file_rules="/etc/udev/rules.d/10-ir_cam.rules"

#if [ -e "$file_rules" ]; then
#    echo "File '$file_rules' exists."
#else
#echo "File '$file_rules' does not exist."
   # Создаём правило, которое при наличии ТПВ камеры (проверка по vendor, product) подключает к ней драйвер серийного порта 
#   sudo echo -e "ACTION==\"add\", ATTRS{idVendor}==\"0424\", ATTRS{idProduct}==\"6000\", RUN+=\"/sbin/modprobe  usbserial vendor=0x0424 product=0x6000\"" >> ${file_rules}
#   sudo udevadm control --reload-rules
#   sudo systemctl restart udev
#fi

if [ -e "$file" ]; then
   echo "File '$file' exists."
else
   echo "File '$file' does not exist."
   # Создаём скрипт, который сможем запустить без sudo
   sudo echo -e "#bin/bash!" >> ${file}
   sudo echo -e "echo \"0424 6000\" | sudo tee -a /sys/bus/usb-serial/drivers/generic/new_id" >> ${file}
   #sudo echo -e "echo  \"6-1:1.1\" | sudo tee -a /sys/bus/usb/drivers/usbserial_generic/unbind" >> ${file}
   #sudo echo -e "echo  \"6-1:1.0\" | sudo tee -a /sys/bus/usb/drivers/uvcvideo/unbind" >> ${file}   		  
   #sudo echo -e "echo  \"6-1:1.0\" | sudo tee -a /sys/bus/usb/drivers/uvcvideo/bind" >> ${file}
   sudo chmod a+x ${file}
   
fi


if [ -e "$file_sudoers" ]; then
    echo "File '$file_sudoers' exists."
else
	# Создаём правило для созданного скрипта
       	echo "File '$file_sudoers' does not exist."
       	sudo echo "${rule_sudoers}" >> ${file_sudoers}
       	echo "Add string \"${rule_sudoers}\" for file \"${file_sudoers}\""
fi
  

echo "Start ir_cam_setup!"
sudo ir_cam_setup
echo "ir_cam_setup success!"
echo ""
exit

