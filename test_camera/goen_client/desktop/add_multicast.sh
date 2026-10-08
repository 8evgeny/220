#/bin/bash!

RED='\033[0;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

if [ -z "$1" ]; then
  echo "Запустите данный скрипт с аргументом:"
  echo -e "${RED}sudo ./add_multicast.sh \${USER}${RESET}"
  echo -e
  exit
else
  echo "Первый аргумент: $1"
fi

file_sudoers="/etc/sudoers.d/route_multicast"
rule_sudoers="$1 ALL=(root) NOPASSWD: /usr/local/sbin/route_multicast"  
file="/usr/local/sbin/route_multicast"

eface=$(ip link | grep ': e')
echo -e "eface(ip linc)=${eface}"

echo "${a%:*}"
eface_buf="${eface#*:}"
eface_buf=""${eface_buf%:*}""
echo -e "eface_buf=${eface_buf}"


#file="./route_multicast.sh"
if [ -e "$file" ]; then
   echo "File '$file' exists."
else
   echo "File '$file' does not exist."
   # Создаём скрипт, который сможем запустить без sudo
   sudo echo -e "#bin/bash!" >> ${file}
   sudo echo -e "sudo ip r a 224.0.0.0/24 dev ${eface_buf}" >> ${file} 
   sudo chmod a+x ${file}
   
#   echo -e "" > ${file}
fi


if [ -e "$file_sudoers" ]; then
    echo "File '$file_sudoers' exists."
else
	# Создаём правило для созданного скрипта
       	echo "File '$file_sudoers' does not exist."
       	sudo echo "${rule_sudoers}" >> ${file_sudoers}
       	echo "Add string \"${rule_sudoers}\" for file \"${file_sudoers}\""
fi
  

echo "Start route_multicast!"
sudo route_multicast
echo "route_multicast success!"
echo ""
exit

