pithon.exec <- function( python.code, get.exception = TRUE, instance.name = "" )
{
	python.code <- paste( python.code, collapse = "\n" )

	ret <- .C( "py_exec_code", python.code, instance.name, exit.status = integer(1), message = character(1), PACKAGE = "rPithon" )
	if (!get.exception)
		return(ret$exit.status)

	if (ret$exit.status != 0)
		stop(ret$message)

	invisible(NULL)
}



