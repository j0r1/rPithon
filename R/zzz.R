#########################################################
# CGB, 20100716
#########################################################

.onLoad <- function( libname, pkgname ){

    library.dynam( "rPython", pkgname, libname )
   
    script <- file.path(libname, pkgname, "pythonwrapperscript.py")

    .C( "py_init", script, PACKAGE = "rPython" )
}

.onUnload <- function( libpath ){
    .C( "py_close", PACKAGE = "rPython" )
    library.dynam.unload( "rPython", libpath )
}
