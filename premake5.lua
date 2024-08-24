include "./vendor/premake/premake_customization/solution_items.lua"

workspace "AlmaasHttp"
	architecture "x86_64"
	startproject "ServerExample"

	configurations
	{
		"Debug",
		"Release",
		"Dist",
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
	include "AlmaasHttp"
group ""

group "Examples"
	include "Examples/ServerExample"
	include "Examples/ClientExample"
group ""
