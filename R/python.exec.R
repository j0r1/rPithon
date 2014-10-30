#########################################################
# CGB, 20100716
#########################################################

python.exec <- function( python.code, get.exception = TRUE ){

    python.code <- paste( python.code, collapse = "\n" )

    ret <- .C( "py_exec_code", python.code, exit.status = integer(1), message = character(1), PACKAGE = "rPython" )
    if (!get.exception)
    	return(ret$exit.status)

    if (ret$exit.status != 0)
	stop(ret$message)

    invisible( NULL )
}



