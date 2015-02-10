NetworkDelayer

This is a small command line utility that I wrote to simulate network latency. It buffers incoming data and sends it out after a given delay.

It acts as a TCP proxy that listens on localhost on a specified port and forwards the traffic to a specified target address and port after a given time delay.

To be able to compile it the Boost development headers are needed.
Go to the source folder and do the following to compile:
mkdir build
cd build
cmake ..
make

The command line is as follows:
networkdelayer #parameters
--help                Print usage information.
--lp arg              The port on which to listen.
--ra arg              Address to redirect to.
--rp arg              Port to redirect to.
--us arg              The amount of µs to delay each full buffer of data. 
                      The network latency will be twice this value.

Example:
Delay network traffic on port localhost:5678 to 192.168.1.2:80:
networkdelayer --lp 5678 --ra 192.168.1.2 --rp 80 --us 100000

Data sent to localhost:5678 will now be directed to 192.168.1.2:80 with a network delay of 200 000 µs (= 200 ms).

This program comes with no warranty what so ever that it'll function as described in this readme and I take no responsability for any damage the software may cause.

I claim no copyright on this code. Use it to your hearts content and modify it any way you see fit! If you find large parts of the code useful I'd appreciate some small mention, but it's not required.
