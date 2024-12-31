# C Web Server

A simple light web server implemented in C language.

## Indice

- [Features](#features)
- [Dependecies](#dependences)
- [Instalation](#instalation)
- [Use](#use)
- [Contribution](#contribution)

## Features:
- **Modular Responses**: Each response is a method and you can handle every request wharever you like.
- **Multiples Routes**: You can configure a route wharever you like.
- **Dinamic Routes**: Support for Dinamic Routes
    - Add ```:``` to set the route dinamic. Like: ```/users/:id``` you can access in ```char** params``` attribute. **:id** whill be ```params[0]```
    - default params max: **10**
- **Secure File Management**: All server files and assests will be in **wwwroot** folder

## Dependecies

- GCC compiler
- Unix socket API
- Makefile

## Instalation

clone the repository:

```bash
git clone https://github.com/Wendley1/Http-Server-C.git
```

enter the project directory:

**Command exemple:**

```bash
cd Http-Server-C
```

run **make** in console

```bash
make
```

## Use

after compile just run the application **main**

```bash
./main
```

you can chosse the port if you want, by default the port is **3000**

```bash
./main *port*
```

run on port 8080 be like:

```bash
./main 8080
```