# How to use this file
```bash
mkdir build && cd build && cmake .. && make
```
After all the executables are built, Tests can be performed as

```bash
cmake .. -DTests=True
```

## simulation with deployment fil
where the graph is represented in XML file to executed it

```bash
./simul_using_deploy small_platform.xml deploy.xml
```


## simulation after parsing json graph
Data and App Drops are stored in vector of objects
values of the each object( task) is parsed from json file

```bash
./simul_using_json Platform  Physical_templet_graph.Json  Physical_graph.Json chunks nb "--log=root.fmt:[%7.6r]%e[%5a]%e%m%n"```


