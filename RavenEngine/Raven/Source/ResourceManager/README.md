# The Resource Manager module

* The module
* Resources
* Loaders
* Using the module

## The module

Derived from the 'IModule' class, the resource manager is a core module that is initialised at the begining of the engine's lifetime.
The resources are stored in a map as a key/value pair where the key is the resource's file pathh. 

## Resources

So far there are four resources that can be loaded: images (`Texture2D`), meshes (`Mesh`), materials (`Material`) and shaders (`Shader`). To load a resource, you need to tell the resource manager which type 
of resource is being loaded so that it can use the appropriate loader for that resource.

## Loaders

For each resource type, there is an associated loader. They are managed by the resource manager module.

## Using the module

* Load a model from a file:
```c++
std::string path("path/to/model");
LoadResource<Model>(path);
Model* model = GetResource<Model>(path);
std::cout << model->meshes->size(); // will return the number of meshes in model!
```
