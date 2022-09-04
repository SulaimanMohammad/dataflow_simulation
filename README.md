## Simulation of DALiuGE
<p align="center">
  <img src="[https://drive.google.com/file/d/1e1XnPS_4otLqfE2aVphISJe4D9Hrxr_N/view?usp=sharing](https://www.dropbox.com/s/0plifphcmdf1cvb/logo_simdal.png?dl=0)">
</p>

This is a simulation  of DALiuGE using <a href="https://simgrid.org/" target="_blank"> SimGrid</a>. SimGrid framework should be installd on the machine.
A graph is desiged using <a href="https://https://eagle.icrar.org//" target="_blank"> EAGLE </a>, then the graph is downloaded. Using <a href="https://daliuge.readthedocs.io/en/latest/cli.html//" target="_blank"> DALiuGE API </a> the graph is translated to Physical graph template and Physical graph.
Those graphs are arguments of the Simulation


# How to use this file
```bash
mkdir build && cd build && cmake .. && make
```
After all the executables are built, Tests can be performed as

```bash
cmake .. -DTests=True
```

## simulation after parsing json graph
Data and App Drops are stored in vector of objects
values of the each object( task) is parsed from json file

on a single node
```bash
./simul_using_json Platform_path  single Path_Physical_graph.Json chunks nb "--log=root.fmt:[%7.6r]%e[%5a]%e%m%n"
```
on multiple nodes
```bash
./simul_using_json Platform_path  Path_Physical_templet_graph.Json  Path_Physical_graph.Json chunks nb "--log=root.fmt:[%7.6r]%e[%5a]%e%m%n"
```
