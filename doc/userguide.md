
![](doc/img.png)
----

# Awa LightweightM2M. 

## User guide.

### Contents.

* [Introduction.](userguide.md#introduction)  
* [The LWM2M client.](userguide.md#the-lwm2m-client)  
    * [The Awa client daemon](userguide.md#the-awa-client-daemon)  
* [The LWM2M server.](userguide.md#the-lwm2m-server)  
    * [The Awa server daemon](userguide.md#the-awa-server-daemon)  
* [The LWM2M Bootstrap server.](userguide.md#the-lwm2m-bootstrap-server) 
    * [The Awa bootstrap server daemon](userguide.md#the-awa-bootstrap-server-daemon)  


----

### Introduction.
LWM2M is a protocol that allows client resources to be accessed by a server. In the Awa library the client and server are daemons (individual processes), each having its own respective API interface over an inter-process communication interface (IPC). The client API is for use exclusively with the client daemon, and the server API is for use exclusively with the server daemon.  
A suite of tools is provided to exercise the main functionality of the API for both the client and server. Later sections describe the configuration of the Awa client and server daemons, along with examples of tools use.

A bootstrap server daemon is also provided that implements the LWM2M bootstrapping protocol which instructs LWM2M clients which LWM2M server to connect to. Later sections describe the configuration of the Awa bootstrap server.

----

### The LWM2M client.

The LWM2M Client runs as a daemon and provides the core LWM2M functionality for:

* Bootstrapping  
* Registration  
* Device management and service enablement  
* Information reporting

The client provides two interfaces: 

* A CoAP interface to talk to the LWM2M server  
* An IPC interface which provides a mechanism for applications to talk to the daemon.


![Awa LWM2M client interfaces](doc/awa_client_interfaces.png)


The IPC interface allows the end user application to define new objects and to perform Get/Set/Delete/Subscribe operations on the client.  
Currently the IPC interface is implemented as a simple UDP channel, with an associated UDP port. It is recommended that only a single user application connect to the daemon's IPC interface at any time.


### The Awa client daemon.

Usage: ````awa_clientd [options] [--bootstrap [URI] | --factoryBootstrap [filename]] ````


| options | description |
|-----|-----|
| --port, -p |  port number for CoAP communications |  
| --ipcPort, -i |  port number for IPC communications |  
| --endPointName, -e | client end point name |  
| --bootstrap, -b  | bootstrap server URI |  
| --factoryBootstrap, -f | factory bootstrap information file |  
| --logFile, -l | log filename |  
| --daemonise, -d | run as daemon |  
| --verbose, -v | enable verbose output |  
| --help | show usage |

Example:

```` awa_clientd --port 6000 --endPointName client1 --bootstrap coap://0.0.0.0:2134 ````


### Awa client daemon tools.

should this be here?

----

### The LWM2M server.

The LWM2M server runs as a daemon which provides an interface to perform LWM2M operations on connected LWM2M clients.

![Awa LWM2M server interfaces](doc/Awa_LWM2M_server_interfaces.png)

The IPC interface allows the end user application to define new objects, list registered clients and perform Read/Write/Delete/Observe operations for a given LWM2M client registered with the server.  
Currently the IPC interface is implemented as a simple UDP channel, with an associated UDP port. It is recommended that only a single user application connect to the daemon's IPC interface at any time.


### The Awa server daemon.


Usage: ````awa_serverd [options] ````

| options | description |
|-----|-----|
| --ip | IP address for server |  
| --interface | Network interface for server |  
| --addressFamily | Address family for network interface. 4 for IPv4, 6 for IPv6 |  
| --port, -p | port number for CoAP communications |  
| --ipcPort, -i | port number for IPC communications |  
| --contentType, -m | Content Type ID (default 1542 - TLV) |  
| --logFile | log filename |  
| --daemonise, -d | run as daemon |  
| --verbose, -v | enable verbose output |  
| --help | show usage |


Example: ````awa_serverd --interface eth0 --addressFamily 4 --port 5683 ````

For examples of how to use the LWM2M server with the LWM2M client see the *LWM2M client usage* section below.


### The LWM2M Bootstrap server.

The LWM2M Bootstrap server runs as a daemon which provides a mechanism to bootstrap LWM2M clients. 


![](doc/Awa_LWM2M_bootstrap_server-interfaces.png)


### The Awa Bootstrap server daemon.

**Command line options.**  
 
A basic bootstrap server is supplied in the development suite which can be run on a local machine and used to re-direct the LWM2M client to any LWM2M server:

Usage: ````awa_bootstrapd [options] ````

| options | description |  
|-----|-----|  
| --ip | IP address for bootstrap server |  
| --interface | Network interface for bootstrap server |  
| --addressFamily | Address family for network interface. 4 for IPv4, 6 for IPv6 |  
| --port, -p | port number for CoAP communications |  
| --config, -c | config file (server list) |  
| --daemonize, -d | daemonize |  
| --verbose, -v | verbose debug output |  
| --logfile  | logfile name |  
| --help | show usage |  


Example: ````awa_bootstrapd --port 15685 --config bootstrap.conf ````


**The configuration file.**  

The configuration file must have the following format:
````
ServerURI=coap://127.0.0.2:5683
SecurityMode=0
PublicKey=[PublicKey]
SecretKey=[SecretKey]
ServerID=1
HoldOffTime=30
ShortServerID=1
Binding=U
LifeTime=30
DefaultMinimumPeriod=1
DefaultMaximumPeriod=-1
DisableTimeout=86400
NotificationStoringWhenDisabledOrOffline=true
````

 * ServerURI specifies the address and port of the LWM2M server to which clients will be directed.  
 * SecurityMode is not yet supported.  
 * PublicKey is not supported.  
 * SecretKey is not supported.  
 * ServerID specifies the numerical ID of the LWM2M server used to associate security and server objects on the LWM2M client.  
 * HoldOffTime is not yet supported.  
 * ShortServerID specifies the numerical ID of the LWM2M server used to associate Security and Server objects on the LWM2M client.  
 * Binding specifies the supported LWM2M binding modes for this server. Only "U" (UDP, non-queuing) is currently supported.  
 * LifeTime specifies the minimum time (in seconds) that the server will wait after receiving a registration or update from the client before terminating that registration.  
 * DefaultMinimumPeriod specifies the default minimum period of observations.  
 * DefaultMaximumPeriod specifies the default maximum period of observations, -1 represents an indefinite period.    
 * DisableTimeout is not supported.  
 * NotificationStoringWhenDisabledOrOffline is not supported.  

----


### Awa client, server and bootstrap example.

### Daemon setup.

Example on how to interconnect all the daemons locally...


### Awa client tools examples.


### Awa server tools examples.


----


## LWM2M Client Usage.

### Connecting the Gateway client to the Gateway LWM2M server.

    $ build/core/src/bootstrap/awa_bootstrapd --verbose --port 15685
    $ build/core/src/server/awa_serverd --verbose
    $ build/core/src/client/awa_clientd --endPointName client1 --bootstrap coap://127.0.0.1:15685

## Awa_API.

 The Awa API provides a method for applications to communicate with the LWM2M Client and Server daemons via the IPC interface.

 The Client API header file can be found in "include/Awa/client.h".

 The Server API header file can be found in "include/Awa/server.h".

 Both server and client APIs are implemented within the libawa library. Applications can be linked against the either the static library "libawa.a" or shared library "libawa.so".

 The api/example directory contains a number of usage examples.

 The tools directory contains a number of useful tools. These are built with the daemons, by default.

## Awa Client API tools.

 A number of command-line tools are provided for user interaction with the LWM2M daemon. These tools provide simple operations such as defining a custom object type, setting a resource value, retrieving a resource value, and waiting for a resource to change or be executed. They interact with the LWM2M daemon via the SDK and IPC channel, and are applications that interact with the daemon locally.

 Note that these are *not* LWM2M Protocol tools - they do not issue LWM2M operations.

### Common Options.

 Each tool accepts the --help option to display all supported options. Common options include:

    -h, --help                 Print help and exit
    -V, --version              Print version and exit
    -v, --verbose              Increase program verbosity (shows more run-time information)
    -d, --debug                Increase program verbosity (shows a lot of run-time information)
    -a, --ipcAddress=ADDRESS   Connect to client daemon at address (default=`127.0.0.1')
    -p, --ipcPort=PORT         Connect to IPC port (default=`12345')

 --ipcAddress is used to specify the IP address of the client daemon to connect to.

 --ipcPort is used to specify the IPC port that the tool uses to communicate with the daemon. Both the daemon and the tool must use the same port. Changing the port allows users to run multiple instances of the client daemon on the same host.

 Most tools take one or more PATH parameters, specified in the format:

    /O       - specifies the object ID for operations on entire object types.
    /O/I     - specifies the object ID and object instance ID for operations on specific object instances.
    /O/I/R   - specifies the object ID, object instance ID, and resource ID for operations on specific resources.
    /O/I/R/i - specifies the object ID, object instance ID, resource ID and resource instance ID for operations on specific resource instances.

 For tools that write data, values can be specified with the format:

    PATH=VALUE

## Defining a New Object definition.

 To create a custom object, the object definition must be registered with the daemon.  The *add-definition* tool can be used to perform this operation.

 **NOTE: Any custom objects must be defined for both the Client and Server daemon - use the awa-server-add-definition tool to define a custom object with the server**

 First, the object itself is defined by providing an ID, descriptive name, mandatory or optional flag (to determine whether the device must provide at least one instance), and whether the object supports single or multiple instances.

    -o, --objectID=ID             Object ID
    -j, --objectName=NAME         Object name
    -m, --objectMandatory         Object is required or optional  (default=off)
    -y, --objectInstances=TYPE    Object supports single or multiple instances
                                    (possible values="single", "multiple"
                                    default=`single')

 Then each resource is specified by a sequence of resource options:

    -r, --resourceID=ID           Resource ID
    -n, --resourceName=NAME       Resource Name
    -t, --resourceType=TYPE       Resource Type  (possible values="opaque",
                                    "integer", "float", "boolean",
                                    "string", "time", "objectlink",
                                    "none")
    -u, --resourceInstances=VALUE Resource supports single or multiple instances
                                    (possible values="single", "multiple")
    -q, --resourceRequired=VALUE  Resource is required or optional  (possible
                                    values="optional", "mandatory")
    -k, --resourceOperations=VALUE
                                  Resource Operation  (possible values="r",
                                    "w", "e", "rw", "rwe")

 For each --resourceID option, all other resource options must be specified.

 For example, to define TestObject2 as ObjectID 1000, with a single mandatory instance, and three resources:

    ./awa-client-define \
     --objectID=1000 --objectName=TestObject2 --objectMandatory --objectInstances=single \
     --resourceID=0 --resourceName=Resource0 --resourceType=string  --resourceInstances=single --resourceRequired=mandatory --resourceOperations=rw \
     --resourceID=1 --resourceName=Resource1 --resourceType=integer --resourceInstances=single --resourceRequired=mandatory --resourceOperations=rw \
     --resourceID=2 --resourceName=Resource2 --resourceType=none    --resourceInstances=single --resourceRequired=optiona   --resourceOperations=e

 Alternatively, an XML Definition file can be used to define a custom object - however this is not yet supported.

## Discovering a Device's Object and Resource Definitions.

You can discover the objects and resources that have been defined on the LWM2M server.  The *awa-client-explore* tool can be used to perform this operation. The tool will list the objects and object-resources that are currently defined within the client daemon.

For example:

    ./awa-client-explore

## Set a Resource Value.

 To set the value of a resource, the *awa-client-set* tool can be used.

 Mandatory resources should always exist provided the object instance exists. However before the value of any resource can be set, the object instance must first be created. And before an optional resource can be set, this resource must first be created.

 For example, consider the case where object /1000 has been defined but no instances have been created. The following
 command can be used to create instance 0 of object 1000:

    ./awa-client-set --create /1000/0

 Any resources defined as optional must also be created before they can be set.

 For example:

    ./awa-client-set --create /1000/0/0

 In the case where no value is specified the resource will be created and populated with the default values.

 Once the resource has been created, the value of the resource can be set.

 For example, to set the value of resource 0 (of type string), within object instance 0 of object 1000, to the value "Happy":

    ./awa-client-set /1000/0/0=Happy

 Note that it is not possible to set the value of a resource that is of type None.

 A specific resource instance of a multi-instance resource can be set with:

    ./awa-client-set /1000/0/1/7=Seventh

 Multiple set operations can be combined on the command line:

    ./awa-client-set --create /1000/0 /1000/0/0=Happy /1000/0/1/7=Seventh

## Get a Resource Value.

 To retrieve the value of a resource and display it on the console, the *awa-client-get* tool can be used.

 For example, to fetch and display the value of resource 0, within instance 0 of object 1000:

    ./awa-client-get /1000/0/0

 Multiple resources can be fetched:

    ./awa-client-get /1000/0/1 /1000/0/5

 Entire object instances can be fetched:

    ./awa-client-get /1000/0

 All object instances for an object ID can be fetched:

    ./awa-client-get /1000

 If the resource specified is a multiple-instance resource, all instances will be retrieved. Individual instances can be displayed by specifying the resource instance index:

    ./awa-client-get /1000/0/5/1 /1000/0/5/2

 The --quiet/-q option can be used to suppress the extra information displayed.

## Subscribe to a Change to a Resource.

 It may be important for a script to block until the value of a resource changes, or an LWM2M Execute operation is performed on an resource that supports the execute operation. For this purpose, the *awa-client-subscribe* tool can be used.

 This tool will display notifications whenever the target resource or object instance changes, or a target resource receives an Execute operation. When such a notification arrives, it will print details in the console.

 Note that this is **not** equivalent to a LWM2M Observe operation. This tool is observing the local resource hosted by the client daemon.

 For example, to wait for a change to resource 200 within instance 0 of object 1000:

    ./awa-client-subscribe /1000/0/200

 To wait for a change to any resource within instance 0 of object 1000:

    ./awa-client-subscribe /1000/0

 Waiting for an LWM2M Execute operation is also possible, however a specific object instance and resource must be specified. For example, to wait on resource 4, which is an executable resource of instance 0 of object 3:

    ./awa-client-subscribe /3/0/4

 By default, *awa-client-subscribe* will wait indefinitely, displaying each notification as it arrives. With the time and count options, *awaclient-subscribe* can terminate after a number of notifications, or an elapsed period of time.

    -t, --waitTime=SECONDS     Time to wait for notification  (default=`0')
    -c, --waitCount=NUMBER     Number of notifications to wait for  (default=`0')

 For example, to wait for no longer than 60 seconds for a single notification:

    ./awa-client-subscribe /3/0/4 --waitTime=60 --waitCount=1

 Multiple paths can be combined on the command line:

    ./awa-client-subscribe /3/0/4 /3/0/5 /4

## Delete a Resource.

 To delete a resource from the client, the *awa-client-delete* tool can be used.

 For example, to delete all object instances of object type 1000:

    ./awa-client-delete /1000

 To delete the object instance of object type 1000 with ID 0:

    ./awa-client-delete /1000/0

 To delete the resource with ID 5 from instance 0 of object ID 1000:

    ./awa-client-delete /1000/0/5

 Unlike the *awa-server-delete* tool, this tool can modify the client's data structures directly, so is not limited by LWM2M Delete rules.

## Awa Server API Tools.

 Server tools are used to communicate with the LWM2M Server daemon and typically issue one or more LWM2M operations to a connected client.

 Server tools often require a target client ID to be specified:

     -c, --clientID=ID          ClientID

 This ID is the client endpoint name used by the client when registering with the LWM2M server.

## List Registered Clients.

 The *awa-server-list-clients* tool can be used to list all clients currently registered with the LWM2M server daemon:

    ./awa-server-list-clients

 The --clientID/-c option is not required. Each client endpoint name is displayed, one per line.

 If --verbose/-v is specified, the output shows the number of registered clients, and their client endpoint names:

    ./awa-server-list-clients --verbose
    2 Registered Clients:

      1 imagination1
      2 chris

 The option --objects/-o can be specified to retrieve and display the objects and object instances currently registered with the LWM2M server, for example:

    ./awa-server-list-clients --objects
      1 imagination1 <2/0>,<4/0>,<7>,<3/0>,<5>,<6>,<0/1>,<1/1>

 The syntax is <ObjectID> or <ObjectID/InstanceID>.


## Define a New Object Definition.

 To use a custom object, the object definition must be registered with the server daemon. The *awa-server-define* tool can be used to perform this operation.

 The *awa-server-define* tool has identical functionality to the *awa-client-define* tool, outlined in the section above.

## Write a Resource Value on a Registered Client.

 To write the value of a resource on a registered client, the *awa-server-write* tool can be used.

 For example, to set the value of resource 0 (of type string) on the client "imagination1", within instance 0 of object 1000, to the value "Happy":

    ./awa-server-write --clientID=imagination1 /1000/0/0=Happy

 Note that it is not possible to set the value of a resource that is of type None.

 A multi-instance resource can be set by specifying the resource instances:

    ./awa-server-write --clientID=imagination1 /1000/0/5/1=123 /1000/0/5/2=456

 To create a new instance of an object on a connected client, the --create option can be used. When an instance is created, any default values provided in the object definition are used.

 For example, to create instance 1 of object 1000 on the client "imagination1":

    ./awa-server-write --clientID=imagination1 --create /1000/1

 To create a new instance with the next available object instance ID:

    ./awa-server-write --clientID=imagination1 --create /1000

 The ID of the newly created object instance is displayed.

 **Note: Create functionality is not yet supported**

## Read a Resource Value from a Registered Client.

 To retrieve the value of a resource and display it on the console, the *awa-server-read* tool can be used.

 For example, to display the value of resource 0, within instance 0 of object 1000:

    ./awa-server-read --clientID=imagination1 /1000/0/0

 Multiple resources and object instances can be read:

    ./awa-server-read -c imagination1 /1000/0/2 /1000/0/3 /1000/1 /1001

## Delete an Object Instance from a Registered Client.

 To delete an instance of an object from a connected client, the *awa-server-delete* tool can be used.

 For example, to delete object 1000, instance 0 from the client "imagination1":

    ./awa-server-delete --clientID=imagination1 /1000/0

 It is not possible to delete individual resources, resource instances, or entire objects due to LWM2M protocol restrictions.

## Observe a Resource on a Registered Client.

 It may be important for a script to block until the value of a resource changes on a specific client. For this purpose, the *awa-server-observe* tool can be used.

 This tool will display notifications whenever the target resource or object instance changes. When such a notification arrives, it will print details in the console.

 This is equivalent to a LWM2M Observe operation. This tool is observing the resources stored by a remote client.

 For example, to wait for a change to resource 200 within instance 0 of object 1000:

    ./awa-server-observe --clientID imagination1 /1000/0/200

 To wait for a change to any resource within instance 0 of object 1000:

    ./awa-server-observe --clientID imagination1 /1000/0

 By default, *awa-server-observe* will wait indefinitely, displaying each notification as it arrives. With the time and count options, *awa-server-observe* can terminate after a number of notifications, or an elapsed period of time.

    -t, --waitTime=SECONDS     Time to wait for notification  (default=`0')
    -c, --waitCount=NUMBER     Number of notifications to wait for  (default=`0')

 For example, to wait for no longer than 60 seconds for a single notification:

    ./awa-server-observe --clientID imagination1 --waitTime=60 --waitCount=1 /1000/0/200

 Observe attributes that affect the way notifications are generated can be changed with the *awa-server-write-attributes* tool.

## Execute a Resource on a Registered Client.

 To initiate an Execute operation on a resource that supports the execute operation, the *awa-server-execute* tool can be used.

 For example, to initiate execution of object 1000, instance 0, resource 4 on the client "imagination1":

    ./awa-server-execute --clientID imagination1 /1000/0/4

 Multiple operations can be initiated with multiple paths:

    ./awa-server-execute --clientID imagination1 /1000/0/4 /1000/0/5

 Opaque data can be supplied as an argument to the execute operation by piping into the process via the --stdin option:

    ./awa-server-execute --stdin --clientID imagination1 /1000/0/4 < mydata

 Note that data supplied is provided to all execute targets.

 It is not possible to initiate an execute operation on an object, an object instance or a resource instance.

 It is not possible to initiate an execute operation on a resource that does not support the execute operation.

## Write Attributes of a Resource or Object Instance on a Registered Client.

 To change the value of attributes associated with a client's resource or object instance, the *awa-server-write-attributes* tool can be used.

 For example, to set the pmin value of object 1000, instance 0, resource 4 on the client "imagination1" to 5 seconds:

    ./awa-server-write_attributes --clientID imagination1 /1000/0/4\?pmin=5

 For example, to set the pmin attribute of object 1000, instance 0 on the client "imagination1" to 5 seconds, and pmax to 100 for the same object instance:

    ./awa-server-write_attributes --clientID imagination1 /1000/0\?pmin=5\&pmax=100

 Note that the ? and & characters need to be escaped for most shells.
