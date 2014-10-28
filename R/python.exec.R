#########################################################
# CGB, 20100716
#########################################################

python.exec <- function( python.code ){

    ret <- .C( "py_exec_code", python.code, exit.status = integer(1), message = character(1), PACKAGE = "rPython" )

    if (ret$exit.status != 0)
	stop( ret$message )

    invisible( NULL )
}



