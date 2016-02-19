# roycobot
Smart embedded LABO

## catkin_ws
Source jouw ros installatie (zorgt ervoor dat ros aan het path van de HUIDIGE terminal (/bin/bash) wordt toegevoegd
```{r, engine='bash', count_lines}
source /opt/ros/jade/setup.bash
```
Maak een workspace aan
```{r, engine='bash', count_lines}
mkdir -p catkin_ws/src
cd catkin_ws/src/
catkin_init_workspace
cd ..
catkin_make
```
Source de ros_workspace en kopieer de git repo catkin package waarin we ons project gaan maken
```{r, engine='bash', count_lines}
source devel/setup.bash
echo $ROS_PACKAGE_PATH
cd src
git clone git@github.com:fenrig/roycobot.git
```
Compileer de pakketen in de workspace (het kan zijn dat dit niet werkt)
```{r, engine='bash', count_lines}
cd ..
catkin_make
```
-------------------------------
!! Als je problemen hebt met position2d.h dan moet je volgende stappen ondernemen
Je moet de laatste lijnen vanaf **"include_directories(include ${catkin_INCLUDE_DIRS})"** in commentaar zetten ( mbv # ) dan vervolgens catkin_make terug draaien dan vervolgens terug die lijnen uit commentaar halen en nog eens catkin_make draaien:
```{r, engine='bash', count_lines}
nano src/roycobot/CMakeLists.txt
catkin_make
nano src/roycobot/CMakeLists.txt
catkin_make
```
--------------------------------
Controleer of het roycobot package werkt
```{r, engine='bash', count_lines}
rosmsg show position2d
```
