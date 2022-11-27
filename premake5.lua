workspace "D3D12Sis"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "D3D12Sis"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "D3D12Sis"
include "glfw"