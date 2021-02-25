PUMA STL Files
==============

Introduction
------------
The STL mesh files for the PUMA microscope are of variable sizes but many are too large to host on GitHub.
This file gives instructions on how to generate the STL files from the CAD files (see folder 'FreeCAD').

How to Generate STL Files
-------------------------

1. Load the FreeCAD file into FreeCAD.
2. Select the 'Mesh Design' Workbench
3. Left-click on the model you want to create a mesh for so as to select it.
4. Go to 'Meshes' -> 'Create mesh from shape ...' to bring up the Tessellation dialogue box.
5. Select the 'Standard' meshing option set the 'surface deviation' to 0.001 mm and the 'Angular deviation' to 1 degree.
6. Click OK - it may take a while to complete.
7. When the mesh is generated, right click on the mesh in the 'Model' tab of the combo view and select 'Export Mesh ...' from the drop down menu to save the mesh as an STL file.

See the '3D_Printing' folder on the GitHub page for information about printing the meshes.

PJT 25.02.2021
 
