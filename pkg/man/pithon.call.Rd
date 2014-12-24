\name{pithon.call}
\alias{pithon.call}
\alias{pithon.method.call}
\title{pithon.call}
\description{
  Calls Python functions and methods from R 
}
\usage{
  pithon.call( py.foo, ..., simplify = TRUE, as.is = FALSE, instance.name="" )
  pithon.method.call( py.object, py.method, ..., instance.name="" )
}
\arguments{
  \item{py.foo}{ rame of a Python function }
  \item{py.object}{ name of a Python object }
  \item{py.method}{ name of a method of such object }
  \item{...}{ R objects to pass as arguments to the Python function or method }
  \item{simplify}{logical value indicating whether simplification of output should be done}
  \item{as.is}{logical value indicating whether length 1 vectors in R should be passed as atomic variables in Python as opposed to length 1 vectors. Note that, e.g., strings such as "hello" in R are vectors of length 1 in R, i.e., \code{"hello"} is the same as \code{c("hello")}. But Python functions operating on arrays will want to receive the array \code{["hello"]} rather than the literal string \code{"hello"}. 
  
  This argument provides little granularity: it affects either all or none of the arguments of the function. Finer control can be obtained using the \code{I()} function as shown in the examples section below.}
  \item{instance.name}{if specified, use the Python instance with this name }
}

\details{
    This function runs a Python function taking as arguments R objects and returning an R object.
    Some limitations exist as to the nature of the objects that can be passed between R and Python.
    As of this writing, atomic arguments and vectors are supported. 

    The user has to be careful to indicate named parameters as required according to Python conventions.
}
\value{
    An R representation of the object returned by the call to the Python function. 
}
\examples{

if (pithon.available())
{
	pithon.call( "len", 1:3 )
	a <- 1:4
	b <- 5:8
	pithon.exec( "def concat(a,b): return a+b" )
	pithon.call( "concat", a, b)

	pithon.assign( "a",  "hola hola" )
	pithon.method.call( "a", "split", " " )
	
} else {
	print("Unable to execute python")
}

}

\keyword{ manip }

