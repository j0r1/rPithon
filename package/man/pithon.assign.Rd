\name{pithon.assign}
\alias{pithon.assign}
\alias{pithon.get}
\title{ Assign and get variables in Python from R }
\description{
  Functions that assign and get Python variables from R.
}
\usage{
  pithon.assign( var.name, value, ..., instance.name="" )
  pithon.get( var.name, instance.name="" )
}
\arguments{
  \item{var.name}{ a character string containing a Python variable name }
  \item{value}{ an R object whose equivalent will be assigned to the variable in Python }
  \item{...}{ arguments to pass to RJSONIO's toJSON function }
  \item{instance.name}{if specified, use the Python instance with this name }
}
\details{
    These functions can assign values to variables in Python as well as get their values back to R.
    Objects are serialized as JSON strings while being transferred between R and Python.
}
\value{
    Function \code{pithon.get} returns a R version of the Python variable \code{py.var}.
}
\references{ \url{http://code.google.com/p/simplejson} }

\examples{

if (pithon.available())
{
	a <- 1:4
	pithon.assign( "a", a )
	pithon.exec( "b = len( a )" )
	pithon.get( "b" )

	pithon.exec( "import math" )
	p <- pithon.get( "math.pi" )

} else {
	print("Unable to execute python")
}

pithon.set.executable("python3", instance.name="v3")
if (pithon.available("v3"))
{
	pithon.assign("x", 123.456, instance.name="v3")
	pithon.get("x", "v3")

} else {
	print("Unable to execute python3")
}

}

\keyword{ manip }

