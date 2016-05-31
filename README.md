# SimpleREST
A RESTful webservice library for minimalists. Featuring SimpleJSON parser and generator. Using boost asio.

## Preface
This REST server is not meant for high frequency or high load applications,
as it uses a synchronous / threaded approach.
A much more powerful and fully asynchronous server is currently under construction here (but nowhere near the first stable release: https://github.com/5cript/attender)

## Introduction
SimpleREST aims to provide a RESTful webservice server for people who hate to write more code than absolutely needed.
It tries to mimic the express.js library interface as closely as possible.

Mounting a path to a harddrive directory is not supported as of this time.

## Dependencies
Required Dependencies:
- boost system
- boost asio

Optional Dependencies:
- SimpleJSON
- (SimpleXML) not fully integrated yet.

## How to build and use
(SECTION UNDER CONSTRUCTION - UNDER INVESTIGATION)
You may for now add all sources and headers to your project or build system. Dynamic library builds are not supported.

To build this library a conformant C++14 compiler is required (tested on g++ 5.3.0)
You will have to link against boost_system and, if you are using windows, ws2_32.

## Where can I find detailed documentation?
The following headers contain useful documentation:
- response.hpp
- request.hpp
- restful.hpp
- connection.hpp (never used directly)

You can also generate doxygen documentation from it. 

## Example 1
```C++
#include "SimpleREST/restful.hpp"

int main()
{
  Rest::InterfaceProvider api{8080}; // will bind on port 8080.
  
  // The following request uses a path parameter indicated by the colon.
  // path parameters are path sections that start with a colon.
  api.get("/test/:echo", [](Rest::Request request, Rest::Response response) // or use auto in lambda!
  {
    // Warning! This here will be executed in a detached thread and is therefore not safe to interact
    // with the main thread.
  
    // set response to HTTP response code 200 and send a string.
    response.status(200).send(request.getParameter("echo")).end();
    
    // -> connection will be closed upon leaving this scope
  });
  
  // Compile with -DSREST_SUPPORT_JSON and clone SimpleJSON (currently a submodule, will change)
  api.post("/interval"), [](auto request, auto response) // another way of writing it.
  {
    // Automagic JSON<->struct conversion with SimpleJSON
    auto task = request.getJson <Messages::Task>(); // Messages::Task is a SimpleJSON struct, see my SimpleJSON here on github.
    response.status(200).send("interval set");
    
    // -> connection will be closed upon leaving this scope.
  });
  
  // start server
  api.start();
  
  // <- SOME PAUSE ->
}
```

## Example 2
Header:
```C++
#include "SimpleREST/restful.hpp"

class MyServer
{ 
public:
  MyServer(unsigned short port);
  ~MyServer() = default; // dtor of api_ closes server gracefully.
  
  void start();
  void stop();
  
private:
  void initialize();
  
private:
  Rest::InterfaceProvider api_;
};
```

Source:
```C++
#include "MyServer.h"

MyServer::MyServer(unsigned short port)
  : api_(port)
{
  initialize();
}

void MyServer::initialize()
{
  api_.get("/", [](auto request, auto response)
  {
    response.status(200).type("text/html").sendFile("index.html");
  }
}

void MyServer::start()
{
  api_.start();
}

void MyServer::stop()
{
  api_.stop();
}
```
