\name{pithon.exec}
\alias{pithon.exec}
\title{ pithon.exec }
\description{
  Executes Python code contained in an R character vector.
}
\usage{
  pithon.exec( python.code, get.exception = TRUE, instance.name="" )
}
\arguments{
  \item{python.code}{ a character vector containing Python code, typically a single line with indentation and EOL characters as required by Python syntax }
  \item{get.exception}{ logical value indicating whether to check or not for exceptions in Python }
  \item{instance.name}{if specified, use the Python instance with this name }
}
\details{
    This function runs Python code.
    It needs to be provided by the caller in a character vector.

    The vector may consists of a single string with EOL and indentation characters embedded.

    Alternatively, it can be a character vector, each entry containing one or more lines of Python code.

    The \code{get.exception} option allows the user to disregard Python exceptions in cases where safe calls to avoid the overhead of checking for them.
}
\value{
    None.
    If the code produces some output, it is up to the caller to go and fetch if from Python.
}

\examples{

if (pithon.available())
{
	a <- 1:4
	b <- 5:8
	pithon.exec( c( "def concat(a,b):", "\treturn a+b" ) )
	pithon.call( "concat", a, b)
} else {
	print("Unable to execute python")
}

if (pithon.available("A") && pithon.available("B"))
{
	pithon.exec("x = 123", instance.name="A")
	pithon.exec("x = 456", instance.name="B")

	pithon.get("x", instance.name="A") # returns 123
	pithon.get("x", instance.name="B") # returns 456
}

}

\keyword{ manip }

