pithon.set.executable <- function(path.to.python.executable, instance.name = "")
{
	.C("py_set_exec", path.to.python.executable, instance.name, PACKAGE = "rPithon")
	invisible(NULL)
}
