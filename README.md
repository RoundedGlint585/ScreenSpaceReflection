# Screen space reflection

This code repository is an implementation of screen space reflection technique on modern OpenGL;

## Platforms ##

* Linux

### Requirements

These are the base requirements to build

* Cmake
* A C++17-standard-compliant compiler
* GLFW3
* OpenGL4 or higher support
* GLM

## Installing

```bash
   chmod u+x build.sh
   ./build.sh
```  

## How to use

After code compilation, there will be "bin" folder, executive file in this folder as well as all textures, shaders and objects

To run code just run binary file

Current solution based on ImGUI library(they are amazing).


| Key                  | Purpose                                                                         | Default Value |
| :------------------- | :--------------------------------------                                         | :------       |
| Enable SSR           | Turn on/off reflections                                                         | True          |
| Distance bias        | Distance difference from which we will think that there is intersection         | 0.03          |
| rayStep              | Step length + coefficient for adaptive step                                     | 0.1           |
| Iteration count      | Amount of steps in ray marching                                                 | 100           |
| Enable adaptive step | Enabling step that should improve quality                                       | True          |
| Enable sampling      | Enabling additional sampling to improve quality                                 | False         |
| Sample Count         | Amount of additional rays(shows only with enabled sampling). Computational heavy| 4             |    
| Sampling coefficient | Factor to addition to base direction(smaller closer to initial ray)             | 0.001         |

So just start program with these keys
### Examples
![[Reflection example]](report/with_sampling.png)

![[Full app]](report/SSR_with_adaptive_step.png)


## Authors

* **Daniil Smolyakov** - *Initial work* - [DanonOfficial](https://github.com/DanonOfficial)



## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details


