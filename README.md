# 360 Surround View Rendering with OpenGL

The required libraries for this project: GLES(3.0), glfw, glm

For GLES installation, please use this command:

```
sudo apt-get install pkg-config
sudo apt-get install libglfw3-dev
sudo apt-get install libgles2
sudo apt-get install libgles2-mesa-dev
sudo apt-get install libgles2-mesa
sudo apt-get install libxinerama-dev libxcursor-dev libxi-dev
```

For Assimp installation, please use this command:

```
sudo apt-get install libassimp-dev
```

Then clone this repository

```
git clone http://ntustcgalgit.synology.me:8000/360-surround-view/projectivetexture_bowlgrids
```

For glfw and glm, please install using these commands (cd to main dir first):

```
cd projectivetexture_bowlgrids/ProjectiveTexture
git submodule add https://github.com/glfw/glfw.git external/glfw
git submodule add https://github.com/g-truc/glm.git external/glm
```

After all the packages and libraries are installed, please change the mode of the following files by doing:

```
chmod +x ./doall.sh ./buildrun.sh ./run.sh
```

Copy data to resources directory:

```
ProjectiveTexture
├── resources
│   ├── Vehicle1_Way0
│   │   ├── 180
│   │       ├── 0
│   │       └── 1
│   │       └── 2
│   │       └── 3
│   │       └── 4
│   │       └── 5
│   │       └── 6
│   │       └── 7
│   │       └── camera_config_180.json
│   └── Bowl_vertices.txt
│   └── Bowl_indices.txt
└── preparation.py
└── main.cpp
└── ...
```

Prepare Vertices and Indices files for OpenGL:

```
python3 preparation.py --min_R 0.5 --max_R 0.5 --height 0.26 --stackCount 14 --json ./resources/Vehicle1_Way0/180/camera_config_180.json --config camera_config2 --sixcam 1 --FOV 180
```

Then run this command to do all the jobs (cmake, make, etc.)
```
./doall.sh
```


