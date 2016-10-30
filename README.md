
rPithon
=======

Introduction
------------

`rPithon` is a package which allows you to execute Python code from within R, passing R
variables to Python, and reading Python results back into R. The functions are based on
those from [rPython](http://rpython.r-forge.r-project.org/), but the way it works is
fundamentally different: in this package, an actual **Python process** is started and
communication with it occurs over a so-called **pipe**. To exchange data, the `rPithon`
package also makes use of `RJSONIO` to convert data structures to and from the JSON
format.

This approach of communicating with a separate Python process instead of linking Python
directly into the current R session, has a few advantages:

 - if the Python code causes a crash or exits the process, this does not terminate the
   current R session, only that particular Python process is affected.
 - several Python processes can be used, so that different packages don't need to use
   the same namespace. By letting a package use its own process, you won't need to
   worry about some other package using the same names.
 - different Python interpreters (for example, of versions 2 and 3) can be used, even
   at the same time.
 - works on GNU/Linux, OS X and MS-Windows.

Since the Python execution speed is exactly the same, the only disadvantage I can think 
of is that communication with the Python process could be a bit slower.

As was already mentioned, the R functions are nearly identical to their `rPython`
counterparts, so many thanks to Carlos J. Gil Bellosta for releasing said package.

Usage
-----

After loading the package using
        
    library("rPithon")

there are several functions you can use. The most important ones are `pithon.set.executable`,
`pithon.exec` and `pithon.get` :

 - `pithon.set.executable`: this specifies which executable should be used to start Python.
   If this function is not run first, the default will be `python`. Optionally, a second
   parameter called `instance.name` can be specified, and this parameter can be used in
   other rPithon functions as well. All functions using the same `instance.name` value
   will communicate with the same Python process. Note that once a Python process has
   actually been started (by calling any other function), changing the executable will
   have no effect. An example of this function is:

        pithon.set.executable("python3", instance.name="v3")

 - `pithon.exec`: executes the specified Python code and returns the code's return value.
   Again, `instance.name` can be used to specify with which Python process should be
   communicated. For example:

        pithon.exec("x = 123")                     # Uses the default Python instance
        pithon.exec("x = 456", instance.name="v3") # Uses the Python version 3 instance

 - `pithon.get`: returns the value of the Python variable with the specified name. Here
   also the `instance.name` parameter can be used to select a specific Python process.
   For example:

        pithon.get("x")                            # Returns 123
        pithon.get("x", instance.name="v3")        # Returns 456

The other functions are `pithon.available`, `pithon.assign`, `pithon.call`, `pithon.load` and 
`pithon.method.call`, and make use of the`pithon.exec` and `pithon.get` functions to
accomplish their goals. In each of them, an `instance.name` parameter can be used
to identify a Python process.

 - `pithon.available`: checks if the specified Python process is running or can be started,
   for example:

        if (pithon.available()) {
            print("Default Python instance works")
        } else {
            print("Default Python instance does not work!")
        }
 
 - `pithon.assign`: to the Python variable with the specified name, the value will be
   assigned. For example:

        > a <- 1:3
        > pithon.assign("A", a)
        > pithon.get("A") 
        [1] 1 2 3

 - `pithon.call`: executes the function whose name is the first parameter, and passes
   other parameters as arguments. For example:

        > pithon.exec("def f(x, y, z): return sum([x, y, z])/12.0")
        > pithon.call("f",1,2,3)
        [1] 0.5

 - `pithon.method.call`: can be used if an object name and its method are specified
   more easily as two parameters. For example:

        > pithon.exec("class MyObject(object):\n def f(self, a, b): return a+b")
        > pithon.exec("m = MyObject()")
        > pithon.method.call("m", "f", 4, 5)
        [1] 9
        > pithon.call("m.f", 4, 5) # this is exactly the same thing

 - `pithon.load`: reads the specified file and executes the Python code in it. For 
   example:

        pithon.load("/path/to/my/file.py")


