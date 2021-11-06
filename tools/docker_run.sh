docker run --net=host --user $(id -u):$(id -g) -it -v $(pwd):/home/developer/project dnx-env
#docker run --net=host --env="DISPLAY" --user $(id -u):$(id -g) -it -v ~/Warsztat/Workspace/3DGence/firmware-DG002-f420-hmi:/home/developer/project --volume="$HOME/.Xauthority:/root/.Xauthority:rw" dnx-env
