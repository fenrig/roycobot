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
Source de ros_workspace 
```{r, engine='bash', count_lines}
source devel/setup.bash
echo $ROS_PACKAGE_PATH
cd src
git clone git@github.com:fenrig/roycobot.git
```
Compileer de pakketen in de workspace
```{r, engine='bash', count_lines}
cd ..
catkin_make
```
Controleer of het roycobot werkt
```{r, engine='bash', count_lines}
rosmsg show two_dim_position
```
