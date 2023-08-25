# pipeline
> A simple pipeline implemented with C++

This project implements a multi-threaded pipeline processing system, where multiple pipes can be added, and the pipeline can be started and stopped as needed.

## Installation

You can just transfer the header and source file to your project and start using it. Or you can build it into a library.
To build it into a library, run these commands in a build directory (e.g. build/) inside your project directory:

```sh
cmake -DCMAKE_BUILD_TYPE=Release .. 
cmake --build .
```
Then the library files will be generated in the build directory (in our example build/).

## Usage example
```c++
pip::Pipeline pipeline(
  3, 
  []() { std::cout << "ctor\n"; return new pip::PipelineElement(); }, 
  [](const pip::PipelineElement* _elem) { std::cout << "fdtor\n"; delete _elem; });

pipeline.AddPipe([](pip::PipelineElement* _pipElem)
{
  if (_pipElem == nullptr)
    return _pipElem;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "first\n";
  return  _pipElem;
});
pipeline.AddPipe([](pip::PipelineElement* _pipElem)
{
  if (_pipElem == nullptr)
    return _pipElem;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "second\n";
  return  _pipElem;
});
pipeline.AddPipe([](pip::PipelineElement* _pipElem)
{
  if (_pipElem == nullptr)
    return _pipElem;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "third\n";
  return  _pipElem;
});
pipeline.AddPipe([](pip::PipelineElement* _pipElem)
{
  if (_pipElem == nullptr)
    return _pipElem;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "forth\n";
  return  _pipElem;
});

pipeline.Run();
```

## Contributing

1. Fork it (<https://github.com/yourname/yourproject/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request
