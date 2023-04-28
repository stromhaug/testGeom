FROM ubuntu:20.04

# NOTE: This Dockerfile works with version v1.1.0 of Analysis Situs (requiring OCC 7.4.0)
#  The development branch of Analysis Situs requires a newer version of OCC

###############################################################################
#
# - OpenCascade:    this is the geometric modeling kernel which provides the
#                   essential services such as B-rep modeling & shape
#                   interrogation, data exchange, shape healing, etc.
#                   https://github.com/Open-Cascade-SAS/OCCT
#
# - Analysis Situs: the open-source CAD platform providing the feature recognition
#                   services, data model, VTK-based visualization services for
#                   CAD models, and GUI/scripting prototyping framework.
#                   https://gitlab.com/ssv/AnalysisSitus
#
# - Eigen:          linear algebra, vectors, matrices.
#                   https://eigen.tuxfamily.org/index.php?title=Main_Page
#
# - Rapidjson:      output to JSON and export to glTF (Analysis Situs).
#                   https://rapidjson.org
#
# Ex. to build:
# > docker build --pull --rm -f "Dockerfile" -t analysis-situs:latest "." --no-cache
###############################################################################

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get -y install build-essential git cmake wget

# Installing QT
# =============
RUN apt-get -y install qtcreator qt5-default
RUN apt-get -y install libqt5x11extras5-dev
RUN apt-get -y install qttools5-dev

# Building VTK
# ============
# VTK dependencies
RUN apt-get -y install libxt-dev libgl1-mesa-dev
# VTK
WORKDIR /home
RUN wget https://www.vtk.org/files/release/8.2/VTK-8.2.0.tar.gz
RUN tar xvfz ./VTK-8.2.0.tar.gz
WORKDIR /home/VTK-8.2.0
RUN mkdir build
WORKDIR /home/VTK-8.2.0/build
RUN cmake .. \
  -DVTK_Group_Qt=ON \
  -DVTK_QT_VERSION=5 \
  -DVTK_RENDERING_BACKEND=OpenGL2 \
  -DVTK_Group_Imaging=YES \
  -DVTK_Group_Qt=YES \
  -DVTK_Group_Views=YES \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr
RUN make -j8
RUN make -j8 install

# Building OpenCascade
# ====================
# Opencascade dependencies
RUN apt-get install -y tcl tcl-dev tk tk-dev libfreeimage-dev
RUN apt-get install -y libxmu-dev libxi-dev
RUN apt-get install -y libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev libosmesa6-dev
# Following dependencies might only be needed in a docker image, not on a host
RUN apt-get install -y libqt5gui5 && rm -rf /var/lib/apt/lists/*
RUN apt-get update && apt-get install -y xvfb
# OpenCascade
RUN mkdir /home/opencascade
WORKDIR /home/opencascade
RUN wget https://github.com/Open-Cascade-SAS/OCCT/archive/refs/tags/V7_6_0.tar.gz
RUN tar zxvf V7_6_0.tar.gz
RUN mkdir /home/opencascade/OCCT-7_6_0/build
WORKDIR /home/opencascade/OCCT-7_6_0/build
RUN cmake .. \
    -DCMAKE_BUILD_TYPE=release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DUSE_FREEIMAGE=ON
RUN make -j8
RUN make -j8 install

# Installing Freetype
# ===================
RUN apt-get install -y libfreetype6-dev

# Building Analysis Situs
# =======================
# Copy sources of Analysis Situs
COPY cmake          /as/cmake
COPY src            /as/src
COPY data           /as/data
COPY asiExe.sh      /as
COPY CMakeLists.txt /as
# Analysis Situs dependencies
RUN apt-get -y install libeigen3-dev rapidjson-dev
# Hack: symlink the VTK build/lib directory to /usr/lib/vtk-8.2, as the VTK installation doesn't seem to
# copy its files to the /usr/lib directory
RUN ln -s /home/VTK-8.2.0/build/lib /usr/lib/vtk-8.2
# Analysis Situs
WORKDIR /as
RUN mkdir -p build
WORKDIR /as/build
RUN cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DCMAKE_INSTALL_RPATH="" \
  -DINSTALL_DIR=/usr/local/bin/analysissitus \
  -DDISTRIBUTION_TYPE=Complete \
  -D3RDPARTY_DIR=/usr/lib \
  -D3RDPARTY_OCCT_INCLUDE_DIR=/usr/include/opencascade \
  -D3RDPARTY_OCCT_LIBRARY_DIR=/usr/lib \
  -D3RDPARTY_EIGEN_DIR=/usr/include/eigen3/ \
  -D3RDPARTY_QT_DIR_NAME=/usr/lib/qt5/ \
  -D3RDPARTY_tcl_DIR=/usr/lib/x86_64-linux-gnu \
  -D3RDPARTY_tcl_INCLUDE_DIR=/usr/include/tcl8.6 \
  -D3RDPARTY_tcl_LIBRARY_DIR=/usr/lib/x86_64-linux-gnu \
  -D3RDPARTY_vtk_DIR=/usr \
  -D3RDPARTY_vtk_INCLUDE_DIR=/usr/include/vtk-8.2 \
  -D3RDPARTY_vtk_LIBRARY_DIR=/usr/lib/vtk-8.2 \
  -D3RDPARTY_freetype_DIR=/usr \
  -D3RDPARTY_freetype_INCLUDE_DIR=/usr/include/freetype2 \
  -D3RDPARTY_freetype_LIBRARY_DIR=/usr/lib/x86_64-linux-gnu \
  -DUSE_MOBIUS=off \
  -DUSE_INSTANT_MESHES=off \
  -DUSE_RAPIDJSON=off \
  -DUSE_NETGEN=off \
  -DUSE_THREADING=off \
  -D3RDPARTY_DIR=/usr/lib
RUN make -j8
RUN make -j8 install

WORKDIR /usr/local/bin/analysissitus/bin

CMD ./asiExe.sh

### Run with: docker run --rm -it -e DISPLAY=192.168.1.42:0.0 analysis-situs:latest