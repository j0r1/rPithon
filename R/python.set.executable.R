python.set.executable <- function(execname)
{
	.C("py_set_exec", execname, PACKAGE = "rPython")

	invisible(NULL)
}
