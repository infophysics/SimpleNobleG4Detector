Geant4:

Download the most recent Geant4 source from the Geant4 webpage (https://geant4.web.cern.ch/support/download).

The Geant4 installation instructions can be found here (https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/installguide.html).

Some of the optional Geant4 datasets are required. It is recommended to download them as part of the installation by using CMake to activate these optional components:

cd /path/to/G4-build
cmake -DGEANT4_INSTALL_DATA=ON .

The visualization component using OpenGL requires another CMake option to activate:

cd /path/to/G4-build
cmake -DGEANT4_USE_OPENGL_X11=ON .

Be advised that this requires both OpenGL and X11. Though typically installed on most devices, be advised that these dependencies might not be met.

To build and install the G4 SimpleNobleG4 code:

cd path/to/SimpleNobleG4Detector
mkdir build
cd build
cmake -DGEANT4_DIR=path/to/Geant4/installation/lib/Geant4[version]/ /path/to/SimpleNobleG4Detector
make install