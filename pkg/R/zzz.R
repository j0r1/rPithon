.onLoad <- function( libname, pkgname )
{
	library.dynam( "rPithon", pkgname, libname )
	script <- file.path(libname, pkgname, "pythonwrapperscript.py")
	.C( "py_init", script, PACKAGE = "rPithon" )

	# Try to set the default python executable to what find_python_cmd detects
	tryCatch({
	
		pyExec <- find_python_cmd()
		#print("Setting python exe to")
		#print(pyExec)
		.C("py_set_default_exec", pyExec, PACKAGE = "rPithon")
		
	}, error = function(e) { })

}

.onUnload <- function( libpath )
{
	.C( "py_close", PACKAGE = "rPithon" )
	library.dynam.unload( "rPithon", libpath )
}

