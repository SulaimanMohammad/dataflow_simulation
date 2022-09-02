#!/bin/bash

activate () {
  . ./dalsim_virtual_env/bin/activate
}

deactivate () {
  deactivate
}

if [ -d "dalsim_virtual_env/bin/" ]
then 
  echo "Activating virtual environment"
  #source ./dalsim/bin/activate
    activate
else 
    virtualenv_version = pip list | grep -F virtualenv 
    if [ -z  virtualenv_version ] #empty var  ,means need to install virtualenv
    then 
      echo "Installing virtualenv"
      python3 -m pip install dalsim_virtual_env > /dev/null # to check if it is installed see /home/sulaiman/.local/lib/python3.10/site-packages there is directory called virtualenv
    fi
    python3 -m virtualenv dalsim_virtual_env > /dev/null
    activate
fi

#check if daliuge is installed in the environment, if not install it 
if test $(find ./dalsim_virtual_env -type d | grep 'dlg' | wc -c) -eq 0
then 
  echo "Installing DALiuGE"
  pip install 'git+https://github.com/ICRAR/daliuge.git#egg&subdirectory=daliuge-common'
  pip install 'git+https://github.com/ICRAR/daliuge.git#egg&subdirectory=daliuge-engine'
  pip install 'git+https://github.com/ICRAR/daliuge.git#egg&subdirectory=daliuge-translator'
fi
# change the extention of the graph from graph to json 
if [ -z "$1" ]
then
  echo "No graph is provided please pass your graph as argument"
else
  Dalgraph="$1"
  if [[ $Dalgraph == *.graph ]]  
  then
    mv -- "$Dalgraph" "${Dalgraph%.graph}.json" 
  fi 
fi
#node_num= "$2"
dlg daemon &
curl -d '{"nodes": ["localhost"]}' -H "Content-Type: application/json" -X POST http://localhost:9000/managers/island/start


# get the name of the file from the directory  
file_name_with_ext= $(basename ${Dalgraph})  
#name without extention 
file_name="${file_name%.*}"   
# navigate to the graph directory 
graph_dir=${Dalgraph%/*}
cd "${DIR}" 

dlg unroll-and-partition -f -N 1 -L "${file_name}.json" -o  "${file_name}_physical_templet.json"
# dlg map -P out_physical_templet.json -o out_physical.json
# dlg submit -P out_physical.json -w
cd ..

echo "deactivate the virtualenv"




