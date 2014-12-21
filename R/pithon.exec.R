pithon.exec <- function( pithon.code, get.exception = TRUE, instance.name = "" )
{
	pithon.code <- paste( pithon.code, collapse = "\n" )

	ret <- .C( "py_exec_code", pithon.code, instance.name, exit.status = integer(1), message = character(1), PACKAGE = "rPithon" )
	if (!get.exception)
		return(ret$exit.status)

	if (ret$exit.status != 0)
		stop(ret$message)

	invisible(NULL)
}



