\name{pithon.set.executable}
\alias{pithon.set.executable}
\title{ pithon.set.executable }
\description{
  Sets the path to the Python executable to be used.
}
\usage{
  pithon.set.executable( path.to.python.executable, instance.name="" )
}
\arguments{
  \item{ path.to.python.executable }{ path to the Python executable that should be used. }
  \item{instance.name}{if specified, use the Python instance with this name }
}
\details{
  Sets the path to the Python executable to be used. This function must be used before
  any other function that needs Python, otherwise it will have no further effect.

  If the function is not called, the program "python" will be executed and communicated
  with. Note that this is only possible if the PATH environment variable contains the
  Python path. On most systems this default should be okay, but the function may be
  explicitly set which Python executable should be used.
}
\value{
    None.
}

\examples{

pithon.set.executable( "/usr/bin/python" )
pithon.set.executable( "python3", instance.name="v3" )

}

\keyword{ manip }

