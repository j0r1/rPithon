pithon.set.default.executable <- function(path.to.python.executable)
{
	.C("py_set_default_exec", path.to.python.executable, PACKAGE = "rPithon")
	invisible(NULL)
}
