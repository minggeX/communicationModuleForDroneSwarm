# communicationModuleForDroneSwarm

## briefly introduction
This is a demo communication module for drone swarm.  
The compile environment is *ROS*.  
The data format of the message is **protobuf3**, and the data transformation protocol is **zeromq**.

the overall structure of the communication process is:
![](http://github.com/minggeX/communicationModuleForDroneSwarm/raw/main/protobuf.png)

![](http://github.com/minggeX/communicationModuleForDroneSwarm/raw/main/structure.png)

### Usage  
1.The directory /src contains a ros package delay. You can create a ros package with the same name "delay" and copy all the files in it to your package 
directory. Or you can copy all the files to your own ros package. Once you do so, you are required to modify the corresponding package name in the 
launch file.

2.modify the launch file  
In uav, modify the arg name standing for the id of the uav in client,launch. Making sure that every uav has a unique name.
In cloud Server, modify the parameter num_uav standing for the number of uav will connect to the server in cloudServer.launch.  

3.modify the config.h file  
the server_addr_list is the address the server will listen, connect_addr_list is the address the client will connect to. So you should keep them coinstency
in every uav and server.  
The subaddr is the addr the client subscribe message from server, pubaddr is the address the server publish message. So keep them coinstency.  
And when run the code in actual environment, replace the address in connect_addr_list and subaddr to the server's actual address.

4.compile the project  

    catkin_make

5.run the code  

in uav:

    roslaunch delay client.launch 
    
or:

    roslaunch (your package name) client.launch 
    
in server:

    roslaunch delay cloudServer.launch
    
or:

    roslaunch (your package name) cloudServer.launch
    
#### Adapt
you can adapt the code for your own project focus on the following two points:
1. Modify the delay.proto to your own communication message content. In the demo project, the message is the uavs' position information.
2. In the demo project, the client uav send its position information to the server when its status update, and the server publish this information as soon as it recieve the message. So you can adapt the time when the uav client send its information to server and the time when server publish the message.


