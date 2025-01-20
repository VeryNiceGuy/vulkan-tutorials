@echo off

set VULKAN_SDK_PATH=C:\VulkanSDK\1.3.296.0
set SHADER_COMPILER_PATH=%VULKAN_SDK_PATH%\Bin\glslangValidator.exe
set SHADER_DIRECTORY_PATH=%~dp0

%SHADER_COMPILER_PATH% -V %SHADER_DIRECTORY_PATH%\vertex_shader.vert -o %SHADER_DIRECTORY_PATH%\vertex_shader.spv
%SHADER_COMPILER_PATH% -V %SHADER_DIRECTORY_PATH%\fragment_shader.frag -o %SHADER_DIRECTORY_PATH%\fragment_shader.spv
