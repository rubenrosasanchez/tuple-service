# Tuple Service communications
Client-server communication of tuples with operations on a linked list using UNIX Sockets.

On the client side, the service has been implemented as a dynamic library called libkeys.

## Features
The tuple service will implement a client-server application in which an undetermined number of clients operate within a tuple database stored in memory. The tuple contains:

~~~
Tuple <int key, char *value1, int value2, double value3>
~~~


The libkeys API implements the following features:
* init(): initializes the service by deleting all previously stored tuples.
* set_value(int key, char * value1, int value2, double value3): adds a new tuple.
* get_value(int key, char * value1, int * value2, double * value3): retreive values from tuple with given key.
* modify_value(int key, char * value1, int value2, double value3): modifies values of tuple with given key.
* delete_key(int key)
* exist(int key)
* copy_key(int key1, int key2): copies data from tuple with key1 into a new tuple with key2 if it does not exist. 

## Run Locally  

Clone the project  

~~~bash  
  git clone https://github.com/rubenrosasanchez/tuple-service.git
~~~

Go to the project directory  

~~~bash  
  cd tuple-service
~~~

Create a build directory  

~~~bash  
  mkdir build
~~~

Move to build directory  

~~~bash  
  cd build
~~~

Use cmake to create the makefile
~~~bash  
  cmake ..
~~~

Compile the code
~~~bash  
  make
~~~

Start the server  

~~~bash  
  ./server <port_number>
~~~

The client app must be initialized with the following environment variables in a different terminal:  

~~~bash  
  env PORT_TUPLES=<port_number> IP_TUPLES=<server-ip-address>
~~~
Example:

~~~bash  
  env PORT_TUPLES=8081 IP_TUPLES=localhost ./client
~~~







## License  

[Mozilla Public License](https://www.mozilla.org/en-US/MPL/2.0/)




 
# Hi, I'm Rubén! 👋  
                
## 🚀 About Me  
I'm a Computer Science and Engineering student at Universidad Carlos III de Madrid 
    
## 🔗 Links  
[![linkedin](https://img.shields.io/badge/linkedin-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/ruben-rs/)  
[![Github](https://img.shields.io/badge/github-000?style=for-the-badge&logo=github&logoColor=white)](https://github.com/rubenrosasanchez/)  
    
## 🛠 Skills  
C, C++, Scala, Java
    
## Other facts about me  
    
🧠 I'm currently learning Big Data aplications with Apache Spark using Scala. 
    
👯‍♀️ I'm looking to collaborate on high performance software projects.
    
⚡️ Fun fact: Python is not a programming language.
