# ProgrammersGlasses

Programmer's Glasses - a developer's file content viewer

This tool can open and view developer related files for which there are no
viewers, or at least no convenient tools to view the details. The tool can
open the following file formats:

- COFF object and library files (`*.obj` and `*.lib`) from version 0.1
- Portable Executable (PE) files (`*.exe`, `*.dll`, `*.ocx` and `*.sys`)
  from version 0.2
- ELF binaries (`*.so`, no extension) from version 0.3

The plan is to have a version 1.0 when the above file types can be displayed.
Versions after that may support more file types.

# Download

The latest build of the Programmer's Glasses can be found in the
[Appveyor](https://ci.appveyor.com/project/vividos/programmersglasses) build
artifacts:

https://ci.appveyor.com/project/vividos/programmersglasses/build/artifacts

[![Build status](https://ci.appveyor.com/api/projects/status/9v3mbipallo7bf29?svg=true)](https://ci.appveyor.com/project/vividos/programmersglasses)

Please note that you need the Visual C++ 2019 redistributable installed, which
should be the case when you have Visual Studio 2019 installed.

The project is occasionally checked with SonarCloud:

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=ProgrammersGlasses&metric=alert_status)](https://sonarcloud.io/dashboard?id=ProgrammersGlasses)

# License

Programmer's Glasses is licensed under the [MIT License](LICENSE.md).
