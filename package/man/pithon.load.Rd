\name{pithon.load}
\alias{pithon.load}
\title{ pithon.load }
\description{
  Executes Python code. 
}
\usage{
  pithon.load( file, get.exception = TRUE, instance.name="" )
}
\arguments{
  \item{ file }{ a file containing Python code to be executed }
  \item{ get.exception }{ logical value indicating whether to check or not for exceptions in Python }
  \item{instance.name}{if specified, use the Python instance with this name }
}
\details{
    This function runs Python code contained in a file.
    Typically, this file would contain functions to be called via \code{\link{pithon.call}} or other functions in this package.

    The \code{get.exception} option allows the user to disregard Python exceptions in cases where safe calls to avoid the overhead of checking for them.
}
\value{
    None.
    If the code produces some output, it is up to the caller to go and fetch if from Python using function \code{\link{pithon.get}}.
}

\examples{

if (pithon.available())
{
	a <- 1:4
	b <- 5:8

	# this file contains the definition of function concat
	pithon.load( system.file( "concat.py", package = "rPython" ) )
	pithon.call( "concat", a, b)

} else {
	print("Unable to execute python")
}

}

\keyword{ manip }

