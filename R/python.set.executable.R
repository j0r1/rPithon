python.set.executable <- function(execname, instance.name = "")
{
	.C("py_set_exec", execname, instance.name, PACKAGE = "rPython")

	invisible(NULL)
}
