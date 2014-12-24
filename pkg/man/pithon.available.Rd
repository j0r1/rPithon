\name{pithon.available}
\alias{pithon.available}
\title{ pithon.available }
\description{
  Checks if Python is available
}
\usage{
  pithon.available( instance.name="" )
}
\arguments{
  \item{instance.name}{if specified, check the Python instance with this name }
}
\details{
  Returns TRUE if the specified Python instance can be used, or FALSE otherwise.
}
\value{
  TRUE or FALSE, depending on the availability of Python.
}

\examples{

if (pithon.available()) {
	print("Default Python instance is available")
} else {
	print("Default Python instance is NOT available")
}

pithon.set.executable( "python3", instance.name="v3" )
if (pithon.available("v3")) {
	print("Python version 3 is available")
} else {
	print("No Python version 3 can be used")
}

}

\keyword{ manip }

