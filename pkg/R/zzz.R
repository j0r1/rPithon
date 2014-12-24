.onLoad <- function( libname, pkgname )
{
	library.dynam( "rPithon", pkgname, libname )
	script <- file.path(libname, pkgname, "pythonwrapperscript.py")
	.C( "py_init", script, PACKAGE = "rPithon" )
}

.onUnload <- function( libpath )
{
	.C( "py_close", PACKAGE = "rPithon" )
	library.dynam.unload( "rPithon", libpath )
}
