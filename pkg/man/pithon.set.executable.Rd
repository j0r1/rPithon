\name{pithon.set.executable}
\alias{pithon.set.executable}
\title{ pithon.set.executable }
\description{
  Sets the path to the Python executable to be used for a particular instance.
}
\usage{
  pithon.set.executable( path.to.python.executable, instance.name="" )
}
\arguments{
  \item{ path.to.python.executable }{ path to the Python executable that should be used. }
  \item{instance.name}{if specified, use the Python instance with this name }
}
\details{
  Sets the path to the Python executable to be used for a particular instance. This function 
  must be used before that instance is started, so before using any other function that specifies
  the instance's name. Otherwise, it will have no further effect.

  If the function is not called, the program that has been set using the 
  \code{pithon.set.defaut.executable} function (either explicitly or it's default value)
  will be executed and communicated with.
}
\value{
    None.
}

\examples{

pithon.set.executable( "/usr/bin/python" )
pithon.set.executable( "python3", instance.name="v3" )

}

\keyword{ manip }

