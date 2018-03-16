# lauSim
Leightweight Automatic fault Simulator:
A Configurable Simulator for Hardware Failures

## Description
Leightweight AUtomatic fault Simulator is an application to simulate node-level faults to enable the possiblity for you to test your fault-tolerant HPC/Cluster/Cloud application. By outsourcing most of its part into plugin, it is a versatile and highly configurable platform. By switching out the fault generation algorithm it can be applied to many different problems, like satelite swarms or conventional HPC clusters.

## Design Considerations
A fault injection system is composed of many different components. In a HPC or a Satelite cluster, there are multiple hosts involved and therefore a Client-Server architecutre is neccessary to coordinate the individual faults. As lauSim is also a tool to test fault tolerant systems reproducability is required. Fault generation and communication methodes should be easily replaceable. For this reason a plugin based design is choosen. To simplify the design, time is discretised into tics. Tics can have various length, depending on the configuration.

## Classifying faults
Depending on the application many different faults can happen. Satelites for example are exposed to high levels of radiation increasing the risc of memory corruptions depending on the system. An overloaded network could loose packets or a failing power distribution unit could result in a fautl of the entire node. The goal of lauSim as a versatile fault Simulator is to replicate the effects of all of these. This is very challenging depending on the fault as for example simulating a power failure by forcefully terminating the application on a node will result in the kernel doing an orderly shutdown of the connection instead of "just not beeing there any more". As the design is restricted to the user space not faults are neccesarly representable.

## The lauSim exo system
As most of lauSims functionality should be easily configurable and extandable, most of it comes in the form of plugins. Plugins are not compiled into the server. They are rather compiled into a dynamically loadable library and the later loaded by the server into memory. To interact with the plugin inside the library a simple interface was designed. Libraries containing plugins need to have a function symbol with the name init. It has to point to an initialization function with a signature similar to a main function with the exception, that it also recevies an a struct used to interact with the plugin manager. The initialization function has to set up the plugins and then register the plugins in the plugin manager. Once the entire configuration is processed and all libraries are loaded, the post init function of each plugin is called. This enables the plugins to do a two stage initialization if they depend on another plugin. A prime example is the transcribe plugin. It pretends to be a fault manager in order to record the tics for later replaying of the same sequence of faults. It does so by waiting for the post initialization and aquires the original fault manager. It the forwards all calls from the server to the nested fault manager while transcribing all the results. The most important plugins are the fault manager, deciding on the faults and their severities, and the communication plugin, interaction with the individual nodes, transmitting the decisions of the fault managers. The main purpose of the server is to load the plugins and to coordinate between the fault manager and the communicaion as well as time management.

## Synchronized Start
One important part to repeatability is that execution on all nodes start at roughly the same time. For that reason execution is paused by the backend until a fault of the node with severity 0 is received.

## Plugins
Most of lauSims functionality comes in form of plugins. For all roles at least on example plugin has been created, the following shortly describes the plugins.

### Echo
The echo plugin is a communication backend with no real node attached to it. Instead of relaying all commands, it simply prints them to standard out. This plugin has use when recoring runs, as the run can be performed "dry" and be replayed in a later run to actual nodes.

### Filelogger
The logger, that is build in into the server is a simple logger, that just loggs to standard out. This plugin adds the option to log to a file instead of standard out. The files name is specified as argument to the init function

### Mosquitto
This plugin uses the mosquitto server as a communicaion backend for both, actors and also external programs. The server paramaters can be set using the parameters during initialization of the library. Each node has its on mosquitto topic like envelope/nodeX components are subtobics of the nodes topic like envelope/nodeX/net for the networking component. It also implements an asynchronous backchannel by subscribing to all subtopics of envelope/master.

### Random
This fault manager creates a number of nodes depending on the arguments to the init function. It uses only a random number generator to decide if and whitch nodes are failing. No considerations about the topology are made.

### Transcribe
This plugin is used to record runs. This plugin requires an acutal fault manager to be loaded to work. It pretends to be a fault manager and is used by the server as such. The plugin however just relaies the function calls from the server to the other fault manager while recording its results to a file. This gives to each tic a list of faults which is written into a file. The nested fault manager and the the name of the transcription file is specified by the arguments to the init function.

### Replay
This Fault manager plugin is more or less a praser for files generated by the transcribe plugin. It reads out the file and replays the faults exactly as they were recorded. The name of the file containing the transcription is specified via arguments to the init function.